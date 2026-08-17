inherit image_types

UBI_SECTION ?= "ubifs"

PARTITIONS_IMAGES ??= ""

ENABLE_MULTIVOLUME_UBI ?= "1"

# -----------------------------------------------
# Handle PARTITIONS_IMAGES and set internal vars
# ST_PARTITIONS_LABELS
# ST_PARTITIONS_MOUNTS_POINTS
# ST_PARTITIONS_SIZES
# ST_PARTITIONS_NOT_SUPPORTED_FS
python () {
    if d.getVar('ENABLE_PARTITIONS_IMAGE') != "1":
        return

    # verify is the image is not an initrd or an initramfs
    image_base = d.getVar('IMAGE_BASENAME')
    initrd = d.getVar('INITRD_IMAGE_ALL') or d.getVar('INITRD_IMAGE') or ""
    initramfs = d.getVar('INITRAMFS_IMAGE') or ""

    if image_base == initrd or image_base == initramfs:
        return
    if  bb.utils.contains_any('IMAGE_FSTYPES', d.getVar('INITRAMFS_FSTYPES') or 'INITRD_TYPE', True, False, d):
        return

    # nand part
    if "nand-custom" in d.getVar('BOOTDEVICE_LABELS'):
        #calculate x y z
        x, y, z = d.getVar('CUSTOM_NAND_CONFIG').split('-')
        # for MKUBIFS_ARGS
        io_size = int(x) * 1024
        leb = int(y)*1024 - 2*int(x)*1024
        d.setVar('MKUBIFS_ARGS_nand_custom', "--min-io-size {} --leb-size {} --max-leb-cnt {}".format(io_size, leb, int(x)*1024))
        d.setVar('UBINIZE_ARGS_nand_custom', "--min-io-size {} --peb-size {}KiB".format(io_size, y))
        d.setVar('NAND_CUSTOM_SIZE', "{}".format(z))

        if d.getVar('EXTRA_UBIFS_SIZE_nand_custom') == "0":
            bb.warn("NAND CUSTOM: you need to set a correct value to EXTRA_UBIFS_SIZE_nand_custom")
            bb.warn("For more information about EXTRA_UBIFS_SIZE configureation see wiki page: ")
            bb.warn("  https://wiki.st.com/stm32mpu/wiki/How_to_add_a_new_parallel_or_serial_NAND_flash_memory_device_in_Yocto ")
}
# -------------------------------------------------
# -------------------------------------------------
#          multiubi (image_types.bbclass)
# -------------------------------------------------
multiubi_mkfs:prepend() {
    if [ "${ENABLE_MULTIVOLUME_UBI}" = "1" ]; then
        # Do not exit on shell error to allow multivolume generation for any configs
        # Check will be done on multivolume creation
        set +e
    fi
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

# -------------------------------------------------
# -------------------------------------------------
#                 extx
# -------------------------------------------------
#stoe_mkext234fs <ext type> <rootfs directory> <image name> <size> <extra image cmd>
stoe_mkext234fs () {
    fstype=$1; shift
    rootfs=$1; shift
    image_name=$1; shift
    size=$1;
    extra_imagecmd=""

    if [ $# -gt 1 ]; then
        shift
        extra_imagecmd=$@
    fi

    # If generating an empty image the size of the sparse block should be large
    # enough to allocate an ext4 filesystem using 4096 bytes per inode, this is
    # about 60K, so dd needs a minimum count of 60, with bs=1024 (bytes per IO)
    eval local COUNT=\"0\"
    eval local MIN_COUNT=\"60\"
    if [ $size -lt $MIN_COUNT ]; then
        eval COUNT=\"$MIN_COUNT\"
    fi
    rootfs_size=$(du --apparent-size -Pks $rootfs | awk -F' ' '{print $1}')
    # Create a sparse image block
    bbdebug 1 Executing "dd if=/dev/zero of=${IMGDEPLOYDIR}/$image_name.$fstype seek=$size count=$COUNT bs=1024"
    dd if=/dev/zero of=${IMGDEPLOYDIR}/$image_name.$fstype seek=$size count=$COUNT bs=1024
    bbdebug 1 "Actual Rootfs size:  ${rootfs_size}"
    bbdebug 1 "Actual Partition size: `ls -l --block-size=k ${IMGDEPLOYDIR}/$image_name.$fstype | awk -F' ' '{print($5)}' | sed 's/K//'`"
    if [ $(ls -1 ${rootfs}/ | wc -l) -gt 0 ]; then
        bbdebug 1 Executing "mkfs.$fstype -F $extra_imagecmd ${IMGDEPLOYDIR}/$image_name.$fstype -d $rootfs"
        mkfs.$fstype -F $extra_imagecmd ${IMGDEPLOYDIR}/$image_name.$fstype -d $rootfs
    else
        bbdebug 1 Executing "mkfs.$fstype -F $extra_imagecmd ${IMGDEPLOYDIR}/$image_name.$fstype"
        mkfs.$fstype -F $extra_imagecmd ${IMGDEPLOYDIR}/$image_name.$fstype
    fi
    # Error codes 0-3 indicate successfull operation of fsck (no errors or errors corrected)
    fsck.$fstype -pvfD ${IMGDEPLOYDIR}/$image_name.$fstype || [ $? -le 3 ]
}
# -------------------------------------------------
# -------------------------------------------------
#                 squashfs
# -------------------------------------------------
#stoe_mksquashfs <comp> <rootfs directory> <image name> <size> <extra image cmd>
stoe_mksquashfs () {
    local comp=$1; shift
    local rootfs=$1; shift
    local image_name=$1;shift
    local size=$1
    local extra_imagecmd=""
    local suffix=""

    if [ $# -gt 1 ]; then
        shift
        extra_imagecmd=$@
    fi

    if [ "$comp" = "zstd" ]; then
        suffix="zst"
    fi

    # Use the bitbake reproducible timestamp instead of the hardcoded squashfs one
    export SOURCE_DATE_EPOCH=$(stat -c '%Y' ${rootfs})
    mksquashfs $rootfs ${IMGDEPLOYDIR}/${image_name}.squashfs${comp:+-}${suffix:-$comp} -noappend ${comp:+-comp }$comp $extra_imagecmd
}


# -------------------------------------------------
# -------------------------------------------------
#                 nand
# -------------------------------------------------
# st_write_ubi_config <vname> <vol_name> <image name>
st_write_ubi_config() {
    local vname="$1"; shift
    local vol_name="$1"; shift
    local image_name=$1;

    bbnote "[st_write_ubi_config] create ubinize_${vname}-splitted-${image_name}.cfg"
    cat <<EOF > ubinize_${vname}-splitted-${image_name}.cfg
[splitted-${vol_name}]
mode=ubi
image=${IMGDEPLOYDIR}/${image_name}_${vname}.${UBI_IMGTYPE}
vol_id=0
vol_type=${UBI_VOLTYPE}
vol_name=${vol_name}
vol_flags=autoresize
EOF
   mv ubinize_${vname}-splitted-${image_name}.cfg ${IMGDEPLOYDIR}/
   bbnote "Move ubinize_${vname}-splitted-${image_name}.cfg to $(dirname ${IMGDEPLOYDIR})"
}

# stmultiubi_mkfs <ubifs_args> <ubinize_args> <rootfs> <image name> <image link> <nand conf name> <partition name>
# NOTE: config name is alreay present on image name and image link
stmultiubi_mkfs() {
    local mkubifs_args="$1"
    local ubinize_args="$2"
    local prootfs="$3"
    local image_name="$4"
    local image_link="$5";
    local conf_name="$6"
    local part_name="$7"

    if [ "${ENABLE_MULTIVOLUME_UBI}" = "1" ]; then
        # Do not exit on shell error to allow multivolume generation for any configs
        # Check will be done on multivolume creation
        set +e
    fi

    # Added prompt error message for ubi and ubifs image creation.
    if [ -z "$mkubifs_args" ] || [ -z "$ubinize_args" ]; then
        bbfatal "MKUBIFS_ARGS and UBINIZE_ARGS have to be set, see http://www.linux-mtd.infradead.org/faq/ubifs.html for details"
    fi

    bbnote "mkfs.ubifs -r ${prootfs} -o ${IMGDEPLOYDIR}/${image_name}_${conf_name}.ubifs ${mkubifs_args}"
    mkfs.ubifs -r ${prootfs} -o ${IMGDEPLOYDIR}/${image_name}_${conf_name}.ubifs ${mkubifs_args} || bbnote "Size of partitions must be managed by st_multivolume_ubifs"

    # create temporary ubinize for this partition
    st_write_ubi_config "${conf_name}" "${part_name}" "${image_name}"

    # Create own symlinks for 'named' volumes
    cd ${IMGDEPLOYDIR}
    bbnote "Create link for ${image_name}_${conf_name}.ubifs"
    if [ -e ${image_name}_${conf_name}.ubifs ]; then
        bbnote "   Link ${image_link}_${conf_name}.ubifs"
        ln -sf ${image_name}_${conf_name}.ubifs ${image_link}_${conf_name}.ubifs
        bbnote "   Link ubinize_${conf_name}-splitted-${image_link}.cfg"
        ln -sf ubinize_${conf_name}-splitted-${image_name}.cfg ubinize_${conf_name}-splitted-${image_link}.cfg
    fi
    cd -
}

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
    ${@' '.join(['%s_%s="%s";' % (arg, name, getVarOverrided('%s_%s' % (arg, name), name, d)) for arg in d.getVar('MULTIUBI_ARGS').split() for name in d.getVar('MULTIUBI_BUILD').split()])}

    for name in ${MULTIUBI_BUILD}; do
        bbnote "Process multivolume UBI for configuration: ${name}"
        # Init multivolume ubinize config file
        rm -f ${IMGDEPLOYDIR}/ubinize_${name}_multivolume_splitted-${IMAGE_NAME}.cfg
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
            # Get ubinize config file for volume_name to manage multivolume ubinize config file creation

            # try with ubivolme name
            ubinize_cfg="ubinize_${name}-${volume_label}.cfg"
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

                ubinize_cfg="ubinize_${name}-${volume_name}.cfg"
                if [ -e ${IMGDEPLOYDIR}/${ubinize_cfg} ]; then
                    cp ${IMGDEPLOYDIR}/${ubinize_cfg} ${WORKDIR}/${ubinize_cfg}
                elif [ -e ${DEPLOY_DIR_IMAGE}/${ubinize_cfg} ]; then
                    cp ${DEPLOY_DIR_IMAGE}/${ubinize_cfg} ${WORKDIR}/${ubinize_cfg}
                    bbnote ">>> Update ubifs path in ubinize config file with DEPLOY_DIR_IMAGE path to avoid issue with RM_WORK feature:"
                    bbnote ">>> ubinize = ${ubinize_cfg}"
                    sed 's|^image=.*/\('"${volume_name}"'.*\.ubifs\)$|image='"${DEPLOY_DIR_IMAGE}"'/\1|' -i ${WORKDIR}/${ubinize_cfg}
                else
                    bbnote ">>>Ubinize config file not found on DEPLOY_DIR_IMAGE path and local deploy dir (${ubinize_cfg}):"
                    # try with splitted version of ubinize file
                    ubinize_cfg="ubinize_${name}-splitted-${volume_name}.cfg"
                    if [ -e ${IMGDEPLOYDIR}/${ubinize_cfg} ]; then
                        cp ${IMGDEPLOYDIR}/${ubinize_cfg} ${WORKDIR}/${ubinize_cfg}
                    elif [ -e ${DEPLOY_DIR_IMAGE}/${ubinize_cfg} ]; then
                        cp ${DEPLOY_DIR_IMAGE}/${ubinize_cfg} ${WORKDIR}/${ubinize_cfg}
                        bbnote ">>> Update ubifs path in ubinize config file with DEPLOY_DIR_IMAGE path to avoid issue with RM_WORK feature:"
                        sed 's|^image=.*/\('"${volume_name}"'.*\.ubifs\)$|image='"${DEPLOY_DIR_IMAGE}"'/\1|' -i ${WORKDIR}/${ubinize_cfg}
                    else
                        bbwarn "Can't find any '${ubinize_cfg}' config file from ${IMGDEPLOYDIR} or ${DEPLOY_DIR_IMAGE} folders"
                        exit
                    fi
                fi
            fi
            bbnote ">>> use ubinize file: ${ubinize_cfg}"

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
                image_size=$(du --apparent-size -Pks ${image_path} | awk -F' ' '{print $1}')
                if [ "${image_size}" -gt "${volume_size}" ]; then
                    bbwarn "The UBI image size exceeds the volume size: ${image_size} versus ${volume_size}"
                    bbwarn "Skip multivolume UBI creation for ${IMAGE_LINK_NAME}_${name}"
                    # Remove on-going configuration file to avoid multivolume ubi creation
                    rm -f "${IMGDEPLOYDIR}/ubinize_${name}_multivolume_splitted-${IMAGE_NAME}.cfg"
                    break
                fi
            fi
            # Display temporary ubinize cfg file
            bbnote "Temporary ubinize cfg file:"
            cat ${WORKDIR}/${ubinize_cfg}
            # Append ubinize config file to multivolume one
            cat ${WORKDIR}/${ubinize_cfg} >> ${IMGDEPLOYDIR}/ubinize_${name}_multivolume_splitted-${IMAGE_NAME}.cfg
        done

        # Check if multivolume UBI can be generated
        [ -e "${IMGDEPLOYDIR}/ubinize_${name}_multivolume_splitted-${IMAGE_NAME}.cfg" ] || continue

        # Generate multivolume UBI
        eval local ubinize_args=\"\$UBINIZE_ARGS_${name}\"
        bbnote "ubinize -o ${IMGDEPLOYDIR}/${IMAGE_NAME}_${name}_multivolume_splitted-${IMAGE_NAME}.ubi ${ubinize_args} ${IMGDEPLOYDIR}/ubinize_${name}_multivolume-splitted-${IMAGE_NAME}.cfg"
        ubinize -o ${IMGDEPLOYDIR}/${IMAGE_NAME}_${name}_multivolume_splitted-${IMAGE_NAME}.ubi ${ubinize_args} ${IMGDEPLOYDIR}/ubinize_${name}_multivolume_splitted-${IMAGE_NAME}.cfg

        # Create own symlinks for 'named' volumes
        cd ${IMGDEPLOYDIR}
        if [ -e ${IMAGE_NAME}_${name}_multivolume_splitted-${IMAGE_NAME}.ubi ]; then
            ln -sf ${IMAGE_NAME}_${name}_multivolume_splitted-${IMAGE_NAME}.ubi ${IMAGE_LINK_NAME}_${name}_multivolume_splitted.ubi
            ln -sf ubinize_${name}_multivolume_splitted-${IMAGE_NAME}.cfg ubinize_${name}_multivolume_splitted-${IMAGE_LINK_NAME}.cfg
        fi
        cd -
    done
}
# -------------------------------------------------
# -------------------------------------------------
#                 size calcul
# -------------------------------------------------
convert_int() {
    printf '%d' ${1:-} 2>/dev/null || :;
}

calculate_script() {
    cat << EOF > ${WORKDIR}/calculate_partition_size.py
#!/usr/bin/python3

import math
import argparse

def get_rootfs_size(align, overhead, extra,rsize):
    rootfs_alignment = int(align)
    overhead_factor = float(overhead)
    rootfs_extra_space = extra

    size_kb = rsize

    base_size = size_kb * overhead_factor
    base_size2 = base_size + rootfs_extra_space

    base_size = base_size2
    if base_size != int(base_size):
        base_size = int(base_size + 1)
    else:
        base_size = int(base_size)

    base_size_saved = base_size
    base_size += rootfs_alignment - 1
    base_size -= base_size % rootfs_alignment

    return base_size

if __name__ == "__main__":
    # Parse argument
    parser = argparse.ArgumentParser(description='Calculate size of partitionsr')
    parser.add_argument("-a", "--alignment", type=int, action="store", default=4096, help="Alignment size")
    parser.add_argument("-o", "--overhead", type=float, action="store", default=1.0, help="Overhead factor")
    parser.add_argument("-e", "--extra", type=int, action="store", default=0, help="Extra space")
    parser.add_argument("-s", "--size", type=int, action="store", default=0, help="size")

    args = parser.parse_args()
    size = get_rootfs_size(args.alignment, args.overhead, args.extra, args.size)
    print(size)
EOF
    chmod 0755  ${WORKDIR}/calculate_partition_size.py
}
calculate_size() {
    rootfs_label=$1
    rootfs_req_size=$2
    rootfs_path=$3
    CALCULATED_SIZE=0

    rootfs_alignment=${IMAGE_ROOTFS_ALIGNMENT}
    overhead_factor=${IMAGE_OVERHEAD_FACTOR}
    # warning: IMAGE_ROOTFS_EXTRA_SPACE can be '0 + 4096) and in this case
    # the string must be evalated to be a number
    rootfs_extra_space=${@eval(d.getVar('IMAGE_ROOTFS_EXTRA_SPACE'))}

    size_kb=$(du --apparent-size -Pks $rootfs_path | awk -F' ' '{print $1}')
    if [ $(convert_int $size_kb) -lt 1 ]; then
        size_kb=1024
    fi
    base_size=$(${WORKDIR}/calculate_partition_size.py -a $rootfs_alignment -o $overhead_factor -e $rootfs_extra_space -s $size_kb)

    echo "[DEBUG][SIZE][$label] $base_size"

    CALCULATED_SIZE=$base_size
     echo "[DEBUG][SIZE][$label] CALCULATED_SIZE=$CALCULATED_SIZE"
}

# -------------------------------------------------
# -------------------------------------------------
stsplitted_partition_create_fs() {
    parttition=$1
    partition_index=$2
    src_splitted_part=$3

    label=$(echo ${ST_PARTITIONS_LABELS} | cut -d',' -f${partition_index})
    mount_point=$(echo ${ST_PARTITIONS_MOUNTS_POINTS} | cut -d',' -f${partition_index})
    size=$(echo ${ST_PARTITIONS_SIZES} | cut -d',' -f${partition_index})
    not_supported_fs=$(echo ${ST_PARTITIONS_NOT_SUPPORTED_FS} | cut -d',' -f${partition_index})

    image_link_name="${IMAGE_BASENAME}${IMAGE_MACHINE_SUFFIX}.splitted-${label}"
    image_name="${image_link_name}${IMAGE_VERSION_SUFFIX}"

    bbnote "image_link_name -> \"${image_link_name}\""
    bbnote "image_name -> \"${image_name}\""

    # calculate size of partition
    partition_size=$(du --apparent-size -Pks ${splitted_directory} | cut -f 1)
    # add extra space
    calculate_size $label $size ${src_splitted_part}
    # hack for little partition like bootfs, vendorfs
    case $partition in
    boot*|vendor*)
        diff_size=$(expr $size - $CALCULATED_SIZE)
        if [ ${diff_size} -gt 32768 ]; then
            partition_size=$CALCULATED_SIZE
        else
            partition_size=$size
        fi
        ;;
    *)
        partition_size=$CALCULATED_SIZE
        ;;
    esac
    if [ ${partition_size} -gt ${size} ]; then
        partition_size=${size}
    fi
    bbnote "Preffered size for image size ${label} -> ${partition_size}"
    for fs in ${IMAGE_FSTYPES} ${@bb.utils.contains('MACHINE_FEATURES', 'efi', 'vfat', '', d)}; do
        if [ -n "${not_supported_fs}" ]; then
            $(echo ${not_supported_fs} | grep -q ${fs}) && continue;
        fi
        case ${fs} in
        ext2)
            stoe_mkext234fs ext2 ${src_splitted_part} ${image_name} ${partition_size} ${EXTRA_IMAGECMD:ext2} -L ${label}
            extension=ext2
            ;;
        ext3)
            stoe_mkext234fs ext3 ${src_splitted_part} ${image_name} ${partition_size} ${EXTRA_IMAGECMD:ext3} -L ${label}
            extension=ext3
            ;;
        ext4)
            stoe_mkext234fs ext4 ${src_splitted_part} ${image_name} ${partition_size} ${EXTRA_IMAGECMD:ext4} -L ${label}
            extension=ext4
            ;;
        tar.bz2)
            if [ -d ${src_splitted_part} ]; then
                ${IMAGE_CMD_TAR} --sort=name --format=posix --numeric-owner -cf ${IMGDEPLOYDIR}/${image_name}.tar -C ${src_splitted_part} . || [ $? -eq 1 ]
                cd ${IMGDEPLOYDIR}/; pbzip2 -f -k ${IMGDEPLOYDIR}/${image_name}.tar; cd -
                [ -e ${IMGDEPLOYDIR}/${image_name}.tar ] && rm -f ${IMGDEPLOYDIR}/${image_name}.tar
                extension=tar.bz2
            fi
            ;;
        tar.xz)
            if [ -d ${src_splitted_part} ]; then
                ${IMAGE_CMD_TAR} --sort=name --format=posix --numeric-owner -cf ${IMGDEPLOYDIR}/${image_name}.tar -C ${src_splitted_part} . || [ $? -eq 1 ]
                cd ${IMGDEPLOYDIR}/; xz -f -k -c ${XZ_COMPRESSION_LEVEL} ${XZ_DEFAULTS} --check=${XZ_INTEGRITY_CHECK} ${image_name}.tar > ${image_name}.tar.xz; cd -
                [ -e ${IMGDEPLOYDIR}/${image_name}.tar ] && rm -f ${IMGDEPLOYDIR}/${image_name}.tar
                extension=tar.xz
            fi
            ;;
        squashfs)
            if [ -d ${src_splitted_part} ]; then
                stoe_mksquashfs '' ${src_splitted_part} ${image_name} ${size} ${EXTRA_IMAGECMD:squashfs}
                extension=squashfs
            fi
            ;;
        squashfs-xz)
            if [ -d ${src_splitted_part} ]; then
                stoe_mksquashfs 'xz' ${src_splitted_part} ${image_name} ${size} ${EXTRA_IMAGECMD:squashfs}
                extension=squashfs-xz
            fi
            ;;
        squashfs-zst)
            if [ -d ${src_splitted_part} ]; then
                stoe_mksquashfs 'zstd' ${src_splitted_part} ${image_name} ${size} ${EXTRA_IMAGECMD:squashfs}
                extension=squashfs-zst
            fi
            ;;
        jffs2)
            if [ -d ${src_splitted_part} ]; then
                mkfs.jffs2 --root=${src_splitted_part} --faketime --output=${IMGDEPLOYDIR}/${image_name}.jffs2 ${EXTRA_IMAGECMD:jffs2}
                extension=jffs2
            fi
            ;;
        vfat)
            #  mkdosfs -v -S 512 -F 32 -n $label -C ${IMGDEPLOYDIR}/${IMAGE_NAME}.vfat ${ROOTFS_SIZE}
            mkfs.vfat ${EXTRA_IMAGECMD:vfat} -n $label -C ${IMGDEPLOYDIR}/${image_name}.vfat ${partition_size}
            if [ -d ${src_splitted_part} ]; then
                mcopy -i "${IMGDEPLOYDIR}/${image_name}.vfat" -vsmpQ ${src_splitted_part}/* ::/
            fi
            extension=vfat
            ;;
        multiubi)
            if [ "${ENABLE_MULTIVOLUME_UBI}" = "1" ]; then
                # create ubifs
                # Split MKUBIFS_ARGS_<name> and UBINIZE_ARGS_<name>
                for name in ${MULTIUBI_BUILD}; do
                    eval local mkubifs_args=\"\$MKUBIFS_ARGS_${name}\"
                    eval local ubinize_args=\"\$UBINIZE_ARGS_${name}\"
                    image_link_with_config_name="${IMAGE_BASENAME}${IMAGE_MACHINE_SUFFIX}.splitted-${label}"
                    image_with_config_name="${image_name}"

                    bbnote "stmultiubi_mkfs \"${mkubifs_args}\" \"${ubinize_args}\" \"${src_splitted_part}\" \"${image_with_config_name}\" \"${image_link_with_config_name}\" \"${name}\" \"${label}\" "
                    stmultiubi_mkfs "${mkubifs_args}" "${ubinize_args}" "${src_splitted_part}" "${image_with_config_name}" "${image_link_with_config_name}" "${name}" "${label}"
                done
            fi
            ;;
        esac
        if [ -n "${extension}" ]; then
            cd "${IMGDEPLOYDIR}/"; ln -sf ${image_name}.${extension} ${image_link_name}.${extension}; cd -
        fi
    done
}

# -------------------------------------------------
# -------------------------------------------------
# filesystem supported:
#    ext2, ext3, ext4
#    multiubi (multiubi generate ubifs)
#    squashfs squashfs-xz squashfs-zst
#    jffs2
#    stfat
#    tar.xz tar.bz2
ST_SPLIT_PARTITITIONS_DEPENDS_extx = "e2fsprogs-native:do_populate_sysroot"
ST_SPLIT_PARTITITIONS_DEPENDS_ubix = " \
        mtd-utils-native:do_populate_sysroot \
        bc-native:do_populate_sysroot \
    "
ST_SPLIT_PARTITITIONS_DEPENDS_vfat = " \
        mtools-native:do_populate_sysroot \
        dosfstools-native:do_populate_sysroot \
    "
ST_SPLIT_PARTITITIONS_DEPENDS_squashfs = " \
    squashfs-tools-native:do_populate_sysroot \
"
ST_SPLIT_PARTITITIONS_DEPENDS_jffs2 = " \
    mtd-utils-native:do_populate_sysroot \
"
do_image_stsplitpartitions[depends] += " \
    coreutils-native:do_populate_sysroot \
    bc-native:do_populate_sysroot \
    ${@bb.utils.contains_any('IMAGE_FSTYPES', 'ext2 ext2.gz ext2.bz2 ext3 ext3.gz ext3.bz2 ext4 ext4.gz ext4.bz2', '${ST_SPLIT_PARTITITIONS_DEPENDS_extx}', '', d)} \
    ${@bb.utils.contains('IMAGE_FSTYPES', 'multiubi', '${ST_SPLIT_PARTITITIONS_DEPENDS_ubix}', '', d)} \
    ${@bb.utils.contains('IMAGE_FSTYPES', 'squashfs', '${ST_SPLIT_PARTITITIONS_DEPENDS_squashfs}', '', d)} \
    ${ST_SPLIT_PARTITITIONS_DEPENDS_vfat} \
    ${@bb.utils.contains('IMAGE_FSTYPES', 'jffs2', '${ST_SPLIT_PARTITITIONS_DEPENDS_jffs2}', '', d)} \
    "

# We ensure all generaic image are generated
do_image_stsplitpartitions[depends] += " \
    ${@bb.utils.contains_any('IMAGE_FSTYPES', 'ext2 ext2.gz ext2.bz2', '${PN}:do_image_ext2', '', d)} \
    ${@bb.utils.contains_any('IMAGE_FSTYPES', 'ext3 ext3.gz ext3.bz2', '${PN}:do_image_ext3', '', d)} \
    ${@bb.utils.contains_any('IMAGE_FSTYPES', 'ext4 ext4.gz ext4.bz2', '${PN}:do_image_ext4', '', d)} \
    ${@bb.utils.contains_any('IMAGE_FSTYPES', 'multiubi', '${PN}:do_image_multiubi ', '', d)} \
    ${@bb.utils.contains_any('IMAGE_FSTYPES', 'squashfs', '${PN}:do_image_squashfs ', '', d)} \
    ${@bb.utils.contains_any('IMAGE_FSTYPES', 'jffs2', '${PN}:do_image_jffs2 ', '', d)} \
    ${@bb.utils.contains_any('IMAGE_FSTYPES', 'tar tar.bz2 tar.xz', '${PN}:do_image_tar ', '', d)} \
     "

IMAGE_CMD:stsplitpartitions () {
    splitted_rootfs_partition=""

    ${@' '.join(['%s_%s="%s";' % (arg, name, d.getVar('%s_%s' % (arg, name))) for arg in d.getVar('MULTIUBI_ARGS').split() for name in d.getVar('MULTIUBI_BUILD').split()])}

    bbnote "list of partitions to manage: ${ST_PARTITIONS_LABELS}"
    unset i
    # first parse, for rootfs: create splitted rootfs and populate it
    for partition in ${PARTITIONS_IMAGES}; do
        i=$(expr $i + 1)
        label=$(echo ${ST_PARTITIONS_LABELS} | cut -d',' -f${i})
        mount_point=$(echo ${ST_PARTITIONS_MOUNTS_POINTS} | cut -d',' -f${i})
        size=$(echo ${ST_PARTITIONS_SIZES} | cut -d',' -f${i})

        case ${label} in
        root*)
            dest_part="${WORKDIR}/splitted_${label}"
            splitted_rootfs_partition=${dest_part}
            ;;
        *)
            ;;
        esac
    done
    if [ -z "${splitted_rootfs_partition}" ]; then
        bbfatal "A rootfs partition must be specified on PARTITIONS_IMAGES list"
    fi

    # generate script to calculate size of partition
    calculate_script
    # third parse, create filesystem
    # create fs for all sub partition and move content to temporary directory
    # create fs for rootfs
    # move temporary content to rootfs
    mkdir -p ${WORKDIR}/temp_rootfs/
    unset i
    for partition in ${PARTITIONS_IMAGES}; do
        i=$(expr $i + 1)
        mount_point=$(echo ${ST_PARTITIONS_MOUNTS_POINTS} | cut -d',' -f${i})

        case $partition in
        rootfs)
            ;;
        *)
            stsplitted_partition_create_fs ${partition} ${i} ${WORKDIR}/rootfs/${mount_point}
            if [ $(ls -1 ${WORKDIR}/rootfs/${mount_point}/ | wc -l) -gt 0 ]; then
                if [ -d ${WORKDIR}/temp_rootfs/${mount_point}/ ]; then
                    rm -rf ${WORKDIR}/temp_rootfs/${mount_point}/
                fi
                mkdir -p ${WORKDIR}/temp_rootfs/${mount_point}/
                mv -f ${WORKDIR}/rootfs/${mount_point}/* ${WORKDIR}/temp_rootfs/${mount_point}/
            fi
            ;;
        esac
    done
    unset i
    for partition in ${PARTITIONS_IMAGES}; do
        i=$(expr $i + 1)
        case $partition in
        rootfs)
            stsplitted_partition_create_fs ${partition} ${i} ${WORKDIR}/rootfs
            ;;
        *)
            ;;
        esac
    done
    # move content from temp to rootfs
    unset i
    for partition in ${PARTITIONS_IMAGES}; do
        i=$(expr $i + 1)
        mount_point=$(echo ${ST_PARTITIONS_MOUNTS_POINTS} | cut -d',' -f${i})

        case $partition in
        rootfs)
            ;;
        *)
            if [ $(ls -1 ${WORKDIR}/temp_rootfs/${mount_point}/ | wc -l) -gt 0 ]; then
                mv ${WORKDIR}/temp_rootfs/${mount_point}/* ${WORKDIR}/rootfs/${mount_point}/
            fi
            ;;
        esac
    done
    rm -rf  ${WORKDIR}/temp_rootfs/

    if [ "${ENABLE_MULTIVOLUME_UBI}" = "1" ]; then
        # create multi ubi
        for fs in ${IMAGE_FSTYPES}; do
            case ${fs} in
            multiubi)
                st_multivolume_ubifs
                ;;
            *)
                ;;
            esac
        done
    fi
}

