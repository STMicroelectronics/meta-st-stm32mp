SUMMARY = "STM32MP userfs Image"
LICENSE = "MIT"

# force image type:
IMAGE_FSTYPES = "ext4"

# Image mount point used on image
IMAGE_PARTITION_MOUNTPOINT = "/user/local"

include recipes-st/images/st-image-partitions.inc

IMAGE_NAME_SUFFIX = ".userfs"

# Define to null ROOTFS_MAXSIZE
IMAGE_ROOTFS_MAXSIZE = ""

# Add specific package for our image:
PACKAGE_INSTALL = " \
    packagegroup-st-demo \
    "

