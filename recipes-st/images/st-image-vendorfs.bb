SUMMARY = "STM32MP vendorfs Image"
LICENSE = "MIT"

include recipes-st/images/st-image-partitions.inc

# Set ROOTFS_MAXSIZE to expected ROOTFS_SIZE to use the whole disk partition and leave extra space to user
IMAGE_ROOTFS_MAXSIZE     = "${IMAGE_ROOTFS_SIZE}"
IMAGE_OVERHEAD_FACTOR    = "1"
IMAGE_ROOTFS_EXTRA_SPACE = "0"

# Add specific package for our image:
PACKAGE_INSTALL += " \
    ${@bb.utils.contains('MACHINE_FEATURES', 'gpu', d.getVar('GPU_USERLAND_LIBRARIES_INSTALL') or '', '', d)} \
"

# Remove specific systemd task
IMAGE_PREPROCESS_COMMAND:remove = "systemd-systemctl-native;"
IMAGE_PREPROCESS_COMMAND:remove = "systemd_preset_all;"
