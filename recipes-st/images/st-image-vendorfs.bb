SUMMARY = "STM32MP vendorfs Image"
LICENSE = "MIT"

include recipes-st/images/st-image-partitions.inc

IMAGE_NAME_SUFFIX = ".vendorfs"
EXTRA_IMAGECMD:ext4 = "-i 4096 -L ${@d.getVar('IMAGE_NAME_SUFFIX').replace('.', '', 1)[:16]} -O metadata_csum,64bit,^dir_index"

# Set ROOTFS_MAXSIZE to expected ROOTFS_SIZE to use the whole disk partition and leave extra space to user
IMAGE_ROOTFS_MAXSIZE     = "${IMAGE_ROOTFS_SIZE}"
IMAGE_OVERHEAD_FACTOR    = "1"
IMAGE_ROOTFS_EXTRA_SPACE = "0"

# Add specific package for our image:
PACKAGE_INSTALL += " \
    ${@bb.utils.contains('MACHINE_FEATURES', 'gpu', d.getVar('GPU_USERLAND_LIBRARIES_INSTALL') or '', '', d)} \
"

# Remove specific systemd task
IMAGE_PREPROCESS_COMMAND:remove = "systemd-systemctl-native"
IMAGE_PREPROCESS_COMMAND:remove = "systemd_preset_all"
