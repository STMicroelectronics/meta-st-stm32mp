SUMMARY = "STM32MP bootfs Image"
LICENSE = "MIT"

# force image type:
IMAGE_FSTYPES = "ext4 stvfat"

# Image mount point used on image
IMAGE_PARTITION_MOUNTPOINT = "/boot"

include recipes-st/images/st-image-partitions.inc

IMAGE_NAME_SUFFIX = ".bootfs"

# Set ROOTFS_MAXSIZE to expected ROOTFS_SIZE to use the whole disk partition and leave extra space to user
IMAGE_ROOTFS_MAXSIZE     = "${IMAGE_ROOTFS_SIZE}"
IMAGE_OVERHEAD_FACTOR    = "1"
IMAGE_ROOTFS_EXTRA_SPACE = "0"

# Add specific package for our image:
PACKAGE_INSTALL = " \
    ${ST_KERNEL_IMAGE_PACKAGE} \
    kernel-devicetree \
    ${ST_EXTLINUX_PACKAGE} \
    ${ST_EXTLINUX_SPLASH_PACKAGE} \
    ${ST_DEVICE_OVERLAY_ADDONS} \
"

# Add specific initrd package to bootfs
INITRD_PACKAGE ?= ""
PACKAGE_INSTALL += " \
    ${@bb.utils.contains('MACHINE_FEATURES', 'initrd', '${INITRD_PACKAGE}', '', d)} \
"
