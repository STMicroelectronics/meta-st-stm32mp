SUMMARY = "STM32MP bootfs Image"
LICENSE = "MIT"

include recipes-st/images/st-image-partitions.inc

# Set ROOTFS_MAXSIZE to expected ROOTFS_SIZE to use the whole disk partition and leave extra space to user
IMAGE_ROOTFS_MAXSIZE     = "${IMAGE_ROOTFS_SIZE}"
IMAGE_OVERHEAD_FACTOR    = "1"
IMAGE_ROOTFS_EXTRA_SPACE = "0"

# Add specific package for our image:
PACKAGE_INSTALL += " \
    kernel-imagebootfs \
    u-boot-stm32mp-extlinux \
    ${@bb.utils.contains('MACHINE_FEATURES', 'splashscreen', 'u-boot-stm32mp-splash', '', d)} \
"

# Add specific autoresize package to bootfs
AUTORESIZE ?= ""
PACKAGE_INSTALL += " \
    ${@bb.utils.contains('COMBINED_FEATURES', 'autoresize', '${AUTORESIZE}', '', d)} \
"
