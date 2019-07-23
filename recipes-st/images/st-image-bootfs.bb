SUMMARY = "STM32MP bootfs Image"
LICENSE = "MIT"

inherit core-image

IMAGE_NAME_SUFFIX = ".${STM32MP_BOOTFS_LABEL}fs"

IMAGE_PARTITION_MOUNTPOINT = "${STM32MP_BOOTFS_MOUNTPOINT_IMAGE}"

# Specific for UBI volume
UBI_VOLNAME = "${STM32MP_BOOTFS_LABEL}"

# Fix to append DISTRO to image name even if we're not using ST distro setting
# This ease flashlayout file configuration
IMAGE_BASENAME_append = "${@'' if 'openstlinuxcommon' in OVERRIDES.split(':') else '-${DISTRO}'}"

# Reset image feature
IMAGE_FEATURE = ""

# Set ROOTFS_MAXSIZE to expected ROOTFS_SIZE to use the whole disk partition and leave extra space to user
IMAGE_ROOTFS_SIZE     = "${BOOTFS_PARTITION_SIZE}"
IMAGE_ROOTFS_MAXSIZE  = "${BOOTFS_PARTITION_SIZE}"
IMAGE_OVERHEAD_FACTOR = "1"
IMAGE_ROOTFS_EXTRA_SPACE = "0"

# Reset PACKAGE_INSTALL to avoid getting installed packages added in machine through IMAGE_INSTALL_append:
PACKAGE_INSTALL = ""

# Reset LINGUAS_INSTALL to avoid getting installed any locale-base package
LINGUAS_INSTALL = ""
IMAGE_LINGUAS = ""

# Add specific package for our image:
PACKAGE_INSTALL += " \
    kernel-imagebootfs \
    u-boot-stm32mp-extlinux \
    ${@bb.utils.contains('MACHINE_FEATURES', 'splashscreen', 'u-boot-stm32mp-splash', '', d)} \
"

# Reset LDCONFIG to avoid runing ldconfig on image.
LDCONFIGDEPEND = ""

# Remove from IMAGE_PREPROCESS_COMMAND useless buildinfo
IMAGE_PREPROCESS_COMMAND_remove = "buildinfo;"
# Remove from IMAGE_PREPROCESS_COMMAND the prelink_image as it could be run after
# we clean rootfs folder leading to cp error if '/etc/' folder is missing:
#   cp: cannot create regular file
#   ‘/local/YOCTO/build/tmp-glibc/work/stm32mp1-openstlinux_weston-linux-gnueabi/st-image-userfs/1.0-r0/rootfs/etc/prelink.conf’:
#   No such file or directory
IMAGE_PREPROCESS_COMMAND_remove = "prelink_image;"

IMAGE_PREPROCESS_COMMAND_append = "reformat_rootfs;"

# Cleanup rootfs newly created
reformat_rootfs() {
    if [ -d ${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT} ]; then
        # Keep only IMAGE_PARTITION_MOUNTPOINT folder
        for f in $(ls -d ${IMAGE_ROOTFS}/*/ | grep -v ${IMAGE_PARTITION_MOUNTPOINT}/)
        do
            rm -rf $f
        done

        # Move all expected files in /rootfs
        mv ${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT}/* ${IMAGE_ROOTFS}/
        # Remove empty boot folder
        rm -rf ${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT}/
    else
        bbwarn "${IMAGE_PARTITION_MOUNTPOINT} folder not available in rootfs folder, no reformat done..."
    fi
}
