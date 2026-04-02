inherit image_types

do_image_vfat[depends] += " \
        mtools-native:do_populate_sysroot \
        dosfstools-native:do_populate_sysroot \
        "
IMAGE_NAME_SUFFIX:pn-st-image-bootfs=".bootfs"
IMAGE_NAME_SUFFIX:pn-st-image-bootfs-efi=".bootfs"
IMAGE_CMD:vfat () {
    label=$(echo ${IMAGE_NAME_SUFFIX} | sed -e "s/\.//")
    # create filesystem
    mkdosfs -v -S 512 -F 32 -n $label -C ${IMGDEPLOYDIR}/${IMAGE_NAME}.vfat ${ROOTFS_SIZE}
    mcopy -i ${IMGDEPLOYDIR}/${IMAGE_NAME}.vfat -s ${IMAGE_ROOTFS}/* ::/
    cd ${IMGDEPLOYDIR}/
    ln -s ${IMAGE_NAME}.vfat ${IMAGE_LINK_NAME}.vfat
    cd -
}
