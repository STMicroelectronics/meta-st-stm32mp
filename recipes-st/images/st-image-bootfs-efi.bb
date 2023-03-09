SUMMARY = "STM32MP bootfs EFI Image"
LICENSE = "MIT"

include recipes-st/images/st-image-partitions.inc

# Set ROOTFS_MAXSIZE to expected ROOTFS_SIZE to use the whole disk partition and leave extra space to user
IMAGE_ROOTFS_MAXSIZE     = "${IMAGE_ROOTFS_SIZE}"
IMAGE_OVERHEAD_FACTOR    = "1"
IMAGE_ROOTFS_EXTRA_SPACE = "0"

# Add specific package for our image:
PACKAGE_INSTALL = " \
    kernel-imageefi \
    stm32mp-efi \
    ${@bb.utils.contains('MACHINE_FEATURES', 'splashscreen', 'u-boot-stm32mp-splash', '', d)} \
"

# Add specific initrd package to bootfs
INITRD_PACKAGE ?= ""
PACKAGE_INSTALL += " \
    ${@bb.utils.contains('MACHINE_FEATURES', 'initrd', '${INITRD_PACKAGE}', '', d)} \
"

# Set IMAGE_FSTYPE to supported EFI fs format
IMAGE_FSTYPES = " ${@bb.utils.contains('MACHINE_FEATURES', 'efi', 'vfat vfat.xz', '', d)} "
