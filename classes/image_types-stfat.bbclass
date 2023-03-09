inherit image_types

do_image_vfat[depends] += " \
        mtools-native:do_populate_sysroot \
        dosfstools-native:do_populate_sysroot \
        "

IMAGE_CMD:vfat () {
    label=$(echo ${IMAGE_NAME_SUFFIX} | sed -e "s/\.//")
    # create filesystem
    mkdosfs -v -S 512 -F 32 -n $label -C ${IMGDEPLOYDIR}/${IMAGE_NAME}${IMAGE_NAME_SUFFIX}.vfat ${ROOTFS_SIZE}
    mcopy -i ${IMGDEPLOYDIR}/${IMAGE_NAME}${IMAGE_NAME_SUFFIX}.vfat -s ${IMAGE_ROOTFS}/* ::/
}
