inherit image_types

do_image_stmultiubi[depends] += "mtd-utils-native:do_populate_sysroot"

python stmultiub_environment () {
    if d.getVar('MULTIUBI_BUILD'):
        try:
            f =open( ("%s/stmultiubi_environment" % d.getVar('T')), 'w')
            for build in d.getVar('MULTIUBI_BUILD').split():
                f.write( "export MKUBIFS_ARGS_%s=\"%s\"\n" % (build, d.getVar(('MKUBIFS_ARGS_' + build))) )
                f.write( "export UBINIZE_ARGS_%s=\"%s\"\n" % (build, d.getVar(('UBINIZE_ARGS_' + build))) )
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

st_multivolume_ubifs() {
    # This function depends on IMAGE_FSTYPES 'stmultiubi'
    if [ "${@bb.utils.contains('IMAGE_FSTYPES', 'stmultiubi', '1', '0', d)}" != "1" ]; then
        return
    fi

    if [ "${ENABLE_MULTIVOLUME_UBI}" != "1" ]; then
        return
    fi

    # -----------------------------------------------------------------------------
    # Define the list of volumes for the multi UBIFS with 'STM32MP_UBI_VOLUME' var.
    # The format to follow is:
    #   STM32MP_UBI_VOLUME = "<VOL_NAME_1>:<VOL_SIZE_1> <VOL_NAME_2>:<VOL_SIZE_2>"
    # Note that:
    #   - 'VOL_NAME' should follow 'IMAGE_LINK_NAME' format
    #   - 'VOL_SIZE' is set in KiB
    # -----------------------------------------------------------------------------

    # We check that user as explicitly provided multi volume UBIFS var
    # and that partition images are also provided
    if [ -n "${STM32MP_UBI_VOLUME}" ] && [ -n "${PARTITIONS_IMAGE}" ]; then

        # We should only generate multi volume UBIFS for rootfs image and not
        # any of the partition image one
        for partition in ${PARTITIONS_IMAGE}; do
            [ "${partition}-${DISTRO}-${MACHINE}" = "${IMAGE_LINK_NAME}" ] && return
        done

        . ${T}/stmultiubi_environment

        for name in ${MULTIUBI_BUILD}; do
            # Init var to populate 'vol_id' incrementally
            volume_id=0
            for ubivolume in ${STM32MP_UBI_VOLUME}; do
                # Init UBI volume information
                if [ -z "$(echo ${ubivolume} | grep ':')" ]; then
                    bbfatal "Missing ':' separator between UBI volume name and UBI volume size '${ubivolume}'"
                fi
                volume_name=$(echo ${ubivolume} | cut -d':' -f1)
                volume_size=$(echo ${ubivolume} | cut -d':' -f2)
                # Set ubinize config file for current volume
                if [ -e ${IMGDEPLOYDIR}/${volume_name}_${name}.ubinize.cfg.ubi ]; then
                    ubinize_cfg=${IMGDEPLOYDIR}/${volume_name}_${name}.ubinize.cfg.ubi
                elif [ -e ${DEPLOY_DIR_IMAGE}/${volume_name}_${name}.ubinize.cfg.ubi ]; then
                    ubinize_cfg=${DEPLOY_DIR_IMAGE}/${volume_name}_${name}.ubinize.cfg.ubi
                else
                    bbfatal "Can't find any '${name}' ubinize config file for ${volume_name} in ${IMGDEPLOYDIR} or ${DEPLOY_DIR_IMAGE} folders"
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
                # Replace 'vol_flags' entry with 'vol_size' one in cfg file
                sed 's|vol_flags=.*|vol_size='"${volume_size}KiB"'|' -i ${WORKDIR}/$(basename ${ubinize_cfg})
                # Append ubinize config file to multivolume one
                cat ${WORKDIR}/$(basename ${ubinize_cfg}) >> ${IMGDEPLOYDIR}/${IMAGE_NAME}_${name}_multivolume.ubinize.cfg.ubi
                # Clean temporary file
                rm -f ${WORKDIR}/$(basename ${ubinize_cfg})
            done
            # Add 'vol_flags' entry in ubinize multivolume config file
            echo "vol_flags=autoresize" >> ${IMGDEPLOYDIR}/${IMAGE_NAME}_${name}_multivolume.ubinize.cfg.ubi

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

IMAGE_POSTPROCESS_COMMAND += " st_multivolume_ubifs ;"
