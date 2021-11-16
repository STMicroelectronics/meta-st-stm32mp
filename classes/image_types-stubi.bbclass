inherit image_types

do_image_stmultiubi[depends] += " \
        mtd-utils-native:do_populate_sysroot \
        bc-native:do_populate_sysroot \
        "

python stmultiub_environment () {
    # Get the MULTIUBI_BUILD list without any duplicates
    ubiconfigs = list(dict.fromkeys((d.getVar('MULTIUBI_BUILD') or "").split()))
    if ubiconfigs:
        try:
            f =open( ("%s/stmultiubi_environment" % d.getVar('T')), 'w')
            for build in ubiconfigs:
                # Append 'build' to OVERRIDES
                localdata = bb.data.createCopy(d)
                overrides = localdata.getVar('OVERRIDES')
                if not overrides:
                    bb.fatal('OVERRIDES not defined')
                localdata.setVar('OVERRIDES', build + ':' + overrides)
                # Compute export vars
                f.write( "export MKUBIFS_ARGS_%s=\"%s\"\n" % (build, localdata.getVar('MKUBIFS_ARGS')) )
                f.write( "export UBINIZE_ARGS_%s=\"%s\"\n" % (build, localdata.getVar('UBINIZE_ARGS')) )
                f.write( "export EXTRA_UBIFS_SIZE_%s=\"%s\"\n" % (build, localdata.getVar('EXTRA_UBIFS_SIZE')) )
                f.write( "export STM32MP_UBI_VOLUME_%s=\"%s\"\n" % (build, localdata.getVar('STM32MP_UBI_VOLUME')) )
            f.close()
        except:
            pass
}

IMAGE_PREPROCESS_COMMAND += "stmultiub_environment;"

IMAGE_CMD_stmultiubi () {
    . ${T}/stmultiubi_environment

    # Split MKUBIFS_ARGS_<name> and UBINIZE_ARGS_<name>
    for name in ${MULTIUBI_BUILD}; do
        bbnote "Process multiubi for ${name}"
        eval local mkubifs_args=\"\$MKUBIFS_ARGS_${name}\"
        eval local ubinize_args=\"\$UBINIZE_ARGS_${name}\"

        if multiubi_mkfs "${mkubifs_args}" "${ubinize_args}" "${name}"; then
            if [ -e ${IMGDEPLOYDIR}/ubinize_${name}-${IMAGE_NAME}.cfg ]; then
                # Set correct name for cfg file to allow automatic cleanup
                mv ${IMGDEPLOYDIR}/ubinize_${name}-${IMAGE_NAME}.cfg ${IMGDEPLOYDIR}/${IMAGE_NAME}_${name}.ubinize.cfg.ubi
                # Create symlinks
                (cd ${IMGDEPLOYDIR} && ln -sf ${IMAGE_NAME}_${name}.ubinize.cfg.ubi ${IMAGE_LINK_NAME}_${name}.ubinize.cfg.ubi)
            fi
        fi
    done
}

ENABLE_MULTIVOLUME_UBI ?= "1"

# -----------------------------------------------------------------------------
# Define the list of volumes for the multi UBIFS with 'STM32MP_UBI_VOLUME' var.
# The format to follow is:
#   STM32MP_UBI_VOLUME = "<VOL_NAME_1>:<VOL_SIZE_1>:<VOL_TYPE_1> <VOL_NAME_2>:<VOL_SIZE_2>"
# Note that:
#   - 'VOL_NAME' is the image volume name
#   - 'VOL_SIZE' is set in KiB
#   - 'VOL_TYPE' is optional part and could be 'empty' to add empty UBI with
#     volume name set to 'VOL_NAME'
# -----------------------------------------------------------------------------
STM32MP_UBI_VOLUME ?= ""

st_multivolume_ubifs() {
    if [ "${ENABLE_MULTIVOLUME_UBI}" != "1" ]; then
        return
    fi

    . ${T}/stmultiubi_environment

    for name in ${MULTIUBI_BUILD}; do
        bbnote "Process multivolume UBI for configuration: ${name}"
        # Init stm32mp_ubi_volume for UBIFS multivolume build
        eval local stm32mp_ubi_volume=\"\$STM32MP_UBI_VOLUME_${name}\"
        bbnote "Volume list to parse: ${stm32mp_ubi_volume}"
        # Init extra_size for UBIFS volume size
        eval local extra_size=\"\$EXTRA_UBIFS_SIZE_${name}\"
        # Init var to populate 'vol_id' incrementally
        volume_id=0
        # Init total volume number to handle
        volume_nbr="$(echo ${stm32mp_ubi_volume} | wc -w)"

        for ubivolume in ${stm32mp_ubi_volume}; do
            # Init UBI volume information
            volume_name=$(echo ${ubivolume} | cut -d':' -f1)
            volume_size=$(echo ${ubivolume} | cut -d':' -f2)
            volume_type=$(echo ${ubivolume} | cut -d':' -f3)
            bbnote "Process UBI volume: ${ubivolume}"
            bbnote "Original name: ${volume_name}"
            bbnote "Original size: ${volume_size}"
            bbnote "Original type: ${volume_type}"
            # Manage no UBI volume type
            if [ -z "${volume_type}" ]; then
                bbnote "The UBI volume type is not set:"
                bbnote ">>> Apply image link name scheme to UBIFS volume name"
                if [ "${volume_name}" = "${IMAGE_BASENAME}" ]; then
                    volume_name=${IMAGE_LINK_NAME}
                else
                    # Partiton images use case: append DISTRO and MACHINE
                    volume_name=${volume_name}-${DISTRO}-${MACHINE}
                fi
                bbnote ">>> Updated UBI volume name: ${volume_name}"
                bbnote ">>> Append ${extra_size}KiB extra space to UBIFS volume size"
                volume_size=$(echo "${volume_size} + ${extra_size}" | bc)
                bbnote ">>> Updated UBI volume size: ${volume_size}"
            fi
            # Init ubinize config file name
            ubinize_cfg="${volume_name}_${name}.ubinize.cfg.ubi"
            # Create temporary copy of ubinize config file to manage multivolume update
            if [ "${volume_type}" = "empty" ]; then
                bbnote "The UBI volume type is set to 'empty': generate temporary ubinize cfg file for empty UBI volume."
                echo \[${volume_name}\] > ${WORKDIR}/${ubinize_cfg}
                echo mode=ubi >> ${WORKDIR}/${ubinize_cfg}
                echo vol_id=0 >> ${WORKDIR}/${ubinize_cfg}
                echo vol_type=dynamic >> ${WORKDIR}/${ubinize_cfg}
                echo vol_name=${volume_name} >> ${WORKDIR}/${ubinize_cfg}
                echo vol_flags=autoresize >> ${WORKDIR}/${ubinize_cfg}
            elif [ -e ${IMGDEPLOYDIR}/${ubinize_cfg} ]; then
                cp ${IMGDEPLOYDIR}/${ubinize_cfg} ${WORKDIR}/${ubinize_cfg}
            elif [ -e ${DEPLOY_DIR_IMAGE}/${ubinize_cfg} ]; then
                cp ${DEPLOY_DIR_IMAGE}/${ubinize_cfg} ${WORKDIR}/${ubinize_cfg}
            else
                bbnote "Can't find any '${ubinize_cfg}' config file from ${IMGDEPLOYDIR} or ${DEPLOY_DIR_IMAGE} folders"
                exit
            fi
            # Update generic name in cfg file
            sed 's|\[ubifs\]|\['"${volume_name}"'\]|' -i ${WORKDIR}/${ubinize_cfg}
            # Update volume id in cfg file
            sed 's|vol_id=0|vol_id='"${volume_id}"'|' -i ${WORKDIR}/${ubinize_cfg}
            # Increment volume id for next loop
            volume_id=$(expr ${volume_id} + 1)
            # Replace 'vol_flags' entry with 'vol_size' one in cfg file except for last volume to allow proper autoresize
            if [ "${volume_id}" -lt "${volume_nbr}" ]; then
                sed 's|vol_flags=.*|vol_size='"${volume_size}KiB"'|' -i ${WORKDIR}/${ubinize_cfg}
            fi
            # Update ubifs path in cfg file with DEPLOY_DIR_IMAGE to avoid issue with RM_WORK feature
            if [ -e ${DEPLOY_DIR_IMAGE}/${ubinize_cfg} ]; then
                sed 's|^image=.*/\('"${volume_name}"'.*\.ubifs\)$|image='"${DEPLOY_DIR_IMAGE}"'/\1|' -i ${WORKDIR}/${ubinize_cfg}
            fi
            # Check for image size
            if grep -q '^image=' ${WORKDIR}/${ubinize_cfg}; then
                image_path=$(grep '^image=' ${WORKDIR}/${ubinize_cfg} | sed 's/^image=//')
                image_size=$(du -ks ${image_path} | awk -F' ' '{print $1}')
                if [ "${image_size}" -gt "${volume_size}" ]; then
                    bbnote "The UBI image size exeeds the volume size: ${image_size} versus ${volume_size}"
                    # Set specific input log when image size exceeds volume_size set
                    sed 's|\(\['"${volume_name}"'\]\)$|\1img_oversize_vol\['"${image_size}"'KiB\]\['"${volume_size}"'KiB\]|' -i ${WORKDIR}/${ubinize_cfg}
                fi
            fi
            # Append ubinize config file to multivolume one
            cat ${WORKDIR}/${ubinize_cfg} >> ${IMGDEPLOYDIR}/${IMAGE_NAME}_${name}_multivolume.ubinize.cfg.ubi
            # Clean temporary file
            rm -f ${WORKDIR}/${ubinize_cfg}
        done

        # Check if multivolume UBI can be generated
        if grep -q '\]img_oversize_vol\[' "${IMGDEPLOYDIR}/${IMAGE_NAME}_${name}_multivolume.ubinize.cfg.ubi"; then
            display_log=$(grep '\]img_oversize_vol\[' "${IMGDEPLOYDIR}/${IMAGE_NAME}_${name}_multivolume.ubinize.cfg.ubi" | sed 's/img_oversize_vol//g')
            bbnote "Skip multivolume UBI creation for ${IMAGE_LINK_NAME}_${name} ([volume name][image_size][volume_size set]):\n${display_log}"
            continue
        fi

        # Generate multivolume UBI
        eval local ubinize_args=\"\$UBINIZE_ARGS_${name}\"
        bbnote "ubinize -o ${IMGDEPLOYDIR}/${IMAGE_NAME}_${name}_multivolume${IMAGE_NAME_SUFFIX}.ubi ${ubinize_args} ${IMGDEPLOYDIR}/${IMAGE_NAME}_${name}_multivolume.ubinize.cfg.ubi"
        ubinize -o ${IMGDEPLOYDIR}/${IMAGE_NAME}_${name}_multivolume${IMAGE_NAME_SUFFIX}.ubi ${ubinize_args} ${IMGDEPLOYDIR}/${IMAGE_NAME}_${name}_multivolume.ubinize.cfg.ubi

        # Create own symlinks for 'named' volumes
        cd ${IMGDEPLOYDIR}
        if [ -e ${IMAGE_NAME}_${name}_multivolume${IMAGE_NAME_SUFFIX}.ubi ]; then
            ln -sf ${IMAGE_NAME}_${name}_multivolume${IMAGE_NAME_SUFFIX}.ubi ${IMAGE_LINK_NAME}_${name}_multivolume.ubi
            ln -sf ${IMAGE_NAME}_${name}_multivolume.ubinize.cfg.ubi ${IMAGE_LINK_NAME}_${name}_multivolume.ubinize.cfg.ubi
        fi
        cd -

        # Cleanup also DEPLOY_DIR_IMAGE from any other ubi artifacts
        # This avoid duplicating data in DEPLOY_DIR_IMAGE
        rm -f ${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}-*_${name}_multivolume${IMAGE_NAME_SUFFIX}.ubi
        rm -f ${DEPLOY_DIR_IMAGE}/${IMAGE_LINK_NAME}-*_${name}_multivolume.ubinize.cfg.ubi
    done
}

# -----------------------------------------------------------------------------
# Manage specific var dependency:
# Because of local overrides within st_multivolume_ubifs() function, we
# need to make sure to add each variables to the vardeps list.
MULTIUBI_LABELS_VARS = "MKUBIFS_ARGS UBINIZE_ARGS EXTRA_UBIFS_SIZE STM32MP_UBI_VOLUME"
MULTIUBI_LABELS_OVERRIDES = "${MULTIUBI_BUILD}"
stmultiub_environment[vardeps] += "${@' '.join(['%s_%s' % (v, o) for v in d.getVar('MULTIUBI_LABELS_VARS').split() for o in d.getVar('MULTIUBI_LABELS_OVERRIDES').split()])}"
