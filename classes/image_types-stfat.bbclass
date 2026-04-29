inherit image_types

do_image_stvfat[depends] += " \
        mtools-native:do_populate_sysroot \
        dosfstools-native:do_populate_sysroot \
        "
IMAGE_CMD:stvfat () {
    label=$(echo ${IMAGE_NAME_SUFFIX} | sed -e "s/\.//")
    # create filesystem
    #mkdosfs -v -S 512 -F 32 -n $label -C ${IMGDEPLOYDIR}/${IMAGE_NAME}.vfat ${ROOTFS_SIZE}
    mkfs.vfat ${EXTRA_IMAGECMD:vfat} -n $label -C ${IMGDEPLOYDIR}/${IMAGE_NAME}.vfat ${ROOTFS_SIZE}
    mcopy -i ${IMGDEPLOYDIR}/${IMAGE_NAME}.vfat -s ${IMAGE_ROOTFS}/* ::/
    cd ${IMGDEPLOYDIR}/
    ln -s ${IMAGE_NAME}.vfat ${IMAGE_LINK_NAME}.vfat
    cd -
}
