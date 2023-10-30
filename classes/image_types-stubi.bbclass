inherit image_types

UBI_SECTION ?= "ubifs"

write_ubi_config() {
	local vname="$1"

	cat <<EOF > ubinize${vname}-${IMAGE_NAME}.cfg
[${UBI_SECTION}]
mode=ubi
image=${IMGDEPLOYDIR}/${IMAGE_NAME}${vname}.${UBI_IMGTYPE}
vol_id=0
vol_type=${UBI_VOLTYPE}
vol_name=${UBI_VOLNAME}
vol_flags=autoresize
EOF
}

# Append symlink creation for ubinize config file
multiubi_mkfs:append() {
	if [ -n "$vname" ]; then
		cd ${IMGDEPLOYDIR}
		if [ -e ubinize${vname}-${IMAGE_NAME}.cfg ]; then
			ln -sf ubinize${vname}-${IMAGE_NAME}.cfg \
			ubinize${vname}-${IMAGE_LINK_NAME}.cfg
		fi
		cd -
	fi
}


multiubi_mkfs:prepend() {
    if [ "${ENABLE_MULTIVOLUME_UBI}" = "1" ]; then
        # Do not exit on shell error to allow multivolume generation for any configs
        # Check will be done on multivolume creation
        set +e
    fi
}

do_image_multiubi[depends] += " \
        mtd-utils-native:do_populate_sysroot \
        bc-native:do_populate_sysroot \
        "

ENABLE_MULTIVOLUME_UBI ?= "1"

# -----------------------------------------------------------------------------
# Define the list of volumes for the multi UBIFS with 'STM32MP_UBI_VOLUME' var.
# The format to follow is:
#   STM32MP_UBI_VOLUME = "<VOL_NAME_1>,<VOL_LABEL_1>,<VOL_SIZE_1>,<VOL_TYPE_1> <VOL_NAME_2>,<VOL_LABEL_2>,<VOL_SIZE_2>"
# Note that:
#   - 'VOL_NAME' is the image volume name
#   - 'VOL_LABEL' is the volume label name
#   - 'VOL_SIZE' is set in KiB
#   - 'VOL_TYPE' is optional part and could be 'empty' to add empty UBI
# -----------------------------------------------------------------------------
STM32MP_UBI_VOLUME ?= ""

# Add the specific multivolume vars
MULTIUBI_ARGS += "EXTRA_UBIFS_SIZE"
MULTIUBI_ARGS += "STM32MP_UBI_VOLUME"

def getVarOverrided(var, override_suffix, d):
    """
    Compute and return 'var' adding 'override_suffix' as local override
    """
    # Append 'override_suffix' to OVERRIDES
    localdata = bb.data.createCopy(d)
    overrides = localdata.getVar('OVERRIDES')
    if not overrides:
        bb.fatal('OVERRIDES not defined')
    localdata.setVar('OVERRIDES', override_suffix + ':' + overrides)
    # Return var with local override applied
    return localdata.getVar(var)

st_multivolume_ubifs() {
    if [ "${ENABLE_MULTIVOLUME_UBI}" != "1" ]; then
        return
    fi

    ${@' '.join(['%s_%s="%s";' % (arg, name, getVarOverrided('%s_%s' % (arg, name), name, d)) for arg in d.getVar('MULTIUBI_ARGS').split() for name in d.getVar('MULTIUBI_BUILD').split()])}

    for name in ${MULTIUBI_BUILD}; do
        bbnote "Process multivolume UBI for configuration: ${name}"
        # Init multivolume ubinize config file
        rm -f ${IMGDEPLOYDIR}/ubinize_${name}_multivolume-${IMAGE_NAME}.cfg
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
            volume_name=$(echo ${ubivolume} | cut -d',' -f1)
            volume_label=$(echo ${ubivolume} | cut -d',' -f2)
            volume_size=$(echo ${ubivolume} | cut -d',' -f3)
            volume_type=$(echo ${ubivolume} | cut -d',' -f4)
            bbnote "Process UBI volume: ${ubivolume}"
            bbnote "Original name: ${volume_name}"
            bbnote "Original label: ${volume_label}"
            bbnote "Original size: ${volume_size}"
            bbnote "Original type: ${volume_type}"
            # Init ubinize config file name
            ubinize_cfg="ubinize_${name}-${volume_name}.cfg"
            rm -f "${WORKDIR}/${ubinize_cfg}"
            # Get ubinize config file for volume_name to manage multivolume ubinize config file creation
            if [ "${volume_type}" = "empty" ]; then
               bbnote "The UBI volume type is set to 'empty'"
               bbnote ">>> Generate temporary ubinize cfg file for empty UBI volume:"
               echo \[${volume_name}\] > ${WORKDIR}/${ubinize_cfg}
               echo mode=ubi >> ${WORKDIR}/${ubinize_cfg}
               echo vol_id=0 >> ${WORKDIR}/${ubinize_cfg}
               echo vol_type=dynamic >> ${WORKDIR}/${ubinize_cfg}
               echo vol_name=${volume_label} >> ${WORKDIR}/${ubinize_cfg}
               echo vol_flags=autoresize >> ${WORKDIR}/${ubinize_cfg}
            else
                bbnote "The UBI volume type is not set to 'empty'"
                bbnote ">>> Append ${extra_size}KiB extra space to UBIFS volume size"
                volume_size=$(echo "${volume_size} + ${extra_size}" | bc)
                bbnote ">>> Updated UBI volume size: ${volume_size}"
                bbnote ">>> Copy existing ubinize config file to temporary ubinize cfg file:"
                if [ -e ${IMGDEPLOYDIR}/${ubinize_cfg} ]; then
                    cp ${IMGDEPLOYDIR}/${ubinize_cfg} ${WORKDIR}/${ubinize_cfg}
                elif [ -e ${DEPLOY_DIR_IMAGE}/${ubinize_cfg} ]; then
                    cp ${DEPLOY_DIR_IMAGE}/${ubinize_cfg} ${WORKDIR}/${ubinize_cfg}
                    bbnote ">>> Update ubifs path in ubinize config file with DEPLOY_DIR_IMAGE path to avoid issue with RM_WORK feature:"
                    sed 's|^image=.*/\('"${volume_name}"'.*\.ubifs\)$|image='"${DEPLOY_DIR_IMAGE}"'/\1|' -i ${WORKDIR}/${ubinize_cfg}
                else
                    bbnote "Can't find any '${ubinize_cfg}' config file from ${IMGDEPLOYDIR} or ${DEPLOY_DIR_IMAGE} folders"
                    exit
                fi
            fi
            # Update volume id in cfg file
            sed 's|vol_id=0|vol_id='"${volume_id}"'|' -i ${WORKDIR}/${ubinize_cfg}
            # Increment volume id for next loop
            volume_id=$(expr ${volume_id} + 1)
            # Replace 'vol_flags' entry with 'vol_size' one in cfg file except for last volume to allow autoresize
            if [ "${volume_id}" -lt "${volume_nbr}" ]; then
                sed 's|vol_flags=.*|vol_size='"${volume_size}KiB"'|' -i ${WORKDIR}/${ubinize_cfg}
            fi
            # Check for image size
            if grep -q '^image=' ${WORKDIR}/${ubinize_cfg}; then
                image_path=$(grep '^image=' ${WORKDIR}/${ubinize_cfg} | sed 's/^image=//')
                image_size=$(du -ks ${image_path} | awk -F' ' '{print $1}')
                if [ "${image_size}" -gt "${volume_size}" ]; then
                    bbnote "The UBI image size exceeds the volume size: ${image_size} versus ${volume_size}"
                    bbnote "Skip multivolume UBI creation for ${IMAGE_LINK_NAME}_${name}"
                    # Remove on-going configuration file to avoid multivolume ubi creation
                    rm -f "${IMGDEPLOYDIR}/ubinize_${name}_multivolume-${IMAGE_NAME}.cfg"
                    break
                fi
            fi
            # Display temporary ubinize cfg file
            bbnote "Temporary ubinize cfg file:"
            cat ${WORKDIR}/${ubinize_cfg}
            # Append ubinize config file to multivolume one
            cat ${WORKDIR}/${ubinize_cfg} >> ${IMGDEPLOYDIR}/ubinize_${name}_multivolume-${IMAGE_NAME}.cfg
        done

        # Check if multivolume UBI can be generated
        [ -e "${IMGDEPLOYDIR}/ubinize_${name}_multivolume-${IMAGE_NAME}.cfg" ] || continue

        # Generate multivolume UBI
        eval local ubinize_args=\"\$UBINIZE_ARGS_${name}\"
        bbnote "ubinize -o ${IMGDEPLOYDIR}/${IMAGE_NAME}_${name}_multivolume${IMAGE_NAME_SUFFIX}.ubi ${ubinize_args} ${IMGDEPLOYDIR}/ubinize_${name}_multivolume-${IMAGE_NAME}.cfg"
        ubinize -o ${IMGDEPLOYDIR}/${IMAGE_NAME}_${name}_multivolume${IMAGE_NAME_SUFFIX}.ubi ${ubinize_args} ${IMGDEPLOYDIR}/ubinize_${name}_multivolume-${IMAGE_NAME}.cfg

        # Create own symlinks for 'named' volumes
        cd ${IMGDEPLOYDIR}
        if [ -e ${IMAGE_NAME}_${name}_multivolume${IMAGE_NAME_SUFFIX}.ubi ]; then
            ln -sf ${IMAGE_NAME}_${name}_multivolume${IMAGE_NAME_SUFFIX}.ubi ${IMAGE_LINK_NAME}_${name}_multivolume.ubi
            ln -sf ubinize_${name}_multivolume-${IMAGE_NAME}.cfg ubinize_${name}_multivolume-${IMAGE_LINK_NAME}.cfg
        fi
        cd -
    done
}
