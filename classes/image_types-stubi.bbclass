inherit image_types

do_image_stmultiubi[depends] += " \
        mtd-utils-native:do_populate_sysroot \
        bc-native:do_populate_sysroot \
        "

python stmultiub_environment () {
    if d.getVar('MULTIUBI_BUILD'):
        try:
            f =open( ("%s/stmultiubi_environment" % d.getVar('T')), 'w')
            for build in d.getVar('MULTIUBI_BUILD').split():
                f.write( "export MKUBIFS_ARGS_%s=\"%s\"\n" % (build, d.getVar(('MKUBIFS_ARGS_' + build))) )
                f.write( "export UBINIZE_ARGS_%s=\"%s\"\n" % (build, d.getVar(('UBINIZE_ARGS_' + build))) )
                f.write( "export EXTRA_UBIFS_SIZE_%s=\"%s\"\n" % (build, d.getVar(('EXTRA_UBIFS_SIZE_' + build))) )
            f.close()
        except:
            pass
}

IMAGE_PREPROCESS_COMMAND += "stmultiub_environment;"

IMAGE_CMD_stmultiubi () {
    . ${T}/stmultiubi_environment

    # Split MKUBIFS_ARGS_<name> and UBINIZE_ARGS_<name>
    for name in ${MULTIUBI_BUILD}; do
        eval local mkubifs_args=\"\$MKUBIFS_ARGS_${name}\"
        eval local ubinize_args=\"\$UBINIZE_ARGS_${name}\"
        multiubi_mkfs "${mkubifs_args}" "${ubinize_args}" "${name}"

        cd ${IMGDEPLOYDIR}
        if [ -e ubinize_${name}-${IMAGE_NAME}.cfg ]; then
            # Set correct name for cfg file to allow automatic cleanup
            mv ubinize_${name}-${IMAGE_NAME}.cfg ${IMAGE_NAME}_${name}.ubinize.cfg.ubi
            # Create symlinks
            ln -sf ${IMAGE_NAME}_${name}.ubinize.cfg.ubi ${IMAGE_LINK_NAME}_${name}.ubinize.cfg.ubi
        fi
        cd -
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
    if [ -n "${STM32MP_UBI_VOLUME}" ]; then
        . ${T}/stmultiubi_environment

        # Get total volume number to handle
        volume_nbr="$(echo ${STM32MP_UBI_VOLUME} | wc -w)"

        for name in ${MULTIUBI_BUILD}; do
            # Init extra_size for UBIFS volume size
            eval local extra_size=\"\$EXTRA_UBIFS_SIZE_${name}\"
            # Init var to populate 'vol_id' incrementally
            volume_id=0
            for ubivolume in ${STM32MP_UBI_VOLUME}; do
                # Init UBI volume information
                if [ -z "$(echo ${ubivolume} | grep ':')" ]; then
                    bbfatal "Missing ':' separator between UBI volume name and UBI volume size '${ubivolume}'"
                fi
                volume_name=$(echo ${ubivolume} | cut -d':' -f1)
                volume_size=$(echo ${ubivolume} | cut -d':' -f2)
                volume_type=$(echo ${ubivolume} | cut -d':' -f3)
                bbnote "Original UBI volume size: ${volume_size}"
                # Manage specific UBI volume type
                if [ "${volume_type}" = "empty" ]; then
                    bbnote "The UBI volume type is set to 'empty' for ${volume_name}. Generate ubinize cfg file for empty UBI volume."
                    cfg_filename=${IMGDEPLOYDIR}/${volume_name}_${name}.ubinize.cfg.ubi
                    echo \[${volume_name}\] > ${cfg_filename}
                    echo mode=ubi >> ${cfg_filename}
                    echo vol_id=0 >> ${cfg_filename}
                    echo vol_type=dynamic >> ${cfg_filename}
                    echo vol_name=${volume_name} >> ${cfg_filename}
                    echo vol_flags=autoresize >> ${cfg_filename}
                else
                    # Update volume_name to fit image link name scheme
                    if [ "${volume_name}" = "${IMAGE_BASENAME}" ]; then
                        volume_name=${IMAGE_LINK_NAME}
                    else
                        # Partiton images use case, so make sure to append DISTRO and MACHINE
                        volume_name=${volume_name}-${DISTRO}-${MACHINE}
                    fi
                    if [ -z "${volume_type}" ]; then
                        bbnote "The UBI volume type is not set. Use default configuration for ${volume_name}"
                        bbnote "Append ${extra_size}KiB extra space to UBIFS volume size"
                        volume_size=$(echo "${volume_size} + ${extra_size}" | bc)
                    else
                        bbwarn "The UBI volume type '${volume_type}' is not recognized. No specific action done for ${volume_name}"
                    fi
                fi
                bbnote "Computed UBI volume size: ${volume_size}"
                # Set ubinize config file for current volume
                if [ -e ${IMGDEPLOYDIR}/${volume_name}_${name}.ubinize.cfg.ubi ]; then
                    ubinize_cfg=${IMGDEPLOYDIR}/${volume_name}_${name}.ubinize.cfg.ubi
                elif [ -e ${DEPLOY_DIR_IMAGE}/${volume_name}_${name}.ubinize.cfg.ubi ]; then
                    ubinize_cfg=${DEPLOY_DIR_IMAGE}/${volume_name}_${name}.ubinize.cfg.ubi
                else
                    bbfatal "Can't find any '${volume_name}_${name}.ubinize.cfg.ubi' config file from ${IMGDEPLOYDIR} or ${DEPLOY_DIR_IMAGE} folders"
                fi
                # Create temporary copy of ubinize config file for update
                cp ${ubinize_cfg} ${WORKDIR}/
                # Update ubifs path in cfg file with DEPLOY_DIR_IMAGE to avoid issue with RM_WORK feature
                if [ -e ${DEPLOY_DIR_IMAGE}/${volume_name}_${name}.ubinize.cfg.ubi ]; then
                    sed 's|^image=.*/\('"${volume_name}"'.*\.ubifs\)$|image='"${DEPLOY_DIR_IMAGE}"'/\1|' -i ${WORKDIR}/$(basename ${ubinize_cfg})
                fi
                # Update generic name in cfg file
                sed 's|\[ubifs\]|\['"${volume_name}"'\]|' -i ${WORKDIR}/$(basename ${ubinize_cfg})
                # Update volume id in cfg file
                sed 's|vol_id=0|vol_id='"${volume_id}"'|' -i ${WORKDIR}/$(basename ${ubinize_cfg})
                volume_id=$(expr ${volume_id} + 1)
                # Replace 'vol_flags' entry with 'vol_size' one in cfg file except for last volume to allow proper autoresize
                if [ "${volume_id}" -lt "${volume_nbr}" ]; then
                    sed 's|vol_flags=.*|vol_size='"${volume_size}KiB"'|' -i ${WORKDIR}/$(basename ${ubinize_cfg})
                fi
                # Increment volume id for next loop
                # Append ubinize config file to multivolume one
                cat ${WORKDIR}/$(basename ${ubinize_cfg}) >> ${IMGDEPLOYDIR}/${IMAGE_NAME}_${name}_multivolume.ubinize.cfg.ubi
                # Clean temporary file
                rm -f ${WORKDIR}/$(basename ${ubinize_cfg})
                # Clean also temporary ubinize cfg file for empty UBI volume
                [ "${volume_type}" = "empty" ] && rm -f ${cfg_filename}
            done

            # Generate multivolume UBI
            eval local ubinize_args=\"\$UBINIZE_ARGS_${name}\"
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
    fi
}
