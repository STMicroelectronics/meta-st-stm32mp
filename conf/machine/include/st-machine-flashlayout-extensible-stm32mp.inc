#@DESCRIPTION: STM32MP machine flashlayout extensible configuration

# Add specific scheme to provide flashlayout that will make rootfs extensible to
# remaining device free space
#   NOTE: this scheme is ONLY compatible with disco board and trusted bootscheme
FLASHLAYOUT_BOOTSCHEME_LABELS += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'trusted', 'extensible', '', d)}"

# -----------------------------------------------------------------------------
# Define config labels
# -----------------------------------------------------------------------------
FLASHLAYOUT_CONFIG_LABELS_extensible = "${@bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', 'sdcard', 'none', d)}"

# -----------------------------------------------------------------------------
# Define label types
# -----------------------------------------------------------------------------
FLASHLAYOUT_TYPE_LABELS_extensible = "${STM32MP_DT_FILES_DK}"

# -----------------------------------------------------------------------------
# Define partitions to use
#
# NB: To manage bootloader partitions, simplification is done by directly
#     re-using 'fsbl1-boot' and 'ssbl-boot' partitions already defined in file
#     'st-machine-flashlayout-stm32mp.inc'
# -----------------------------------------------------------------------------
FLASHLAYOUT_PARTITION_LABELS_extensible = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl ${FLASHLAYOUT_PARTITION_IMAGES}"
FLASHLAYOUT_PARTITION_LABELS_extensible_remove = "userfs"

# -----------------------------------------------------------------------------
# The 'extensible' bootscheme is a trick to generate flashlayout files without
# userfs partition for trusted bootscheme ONLY. So rootfs partition will be
# extended up to the end of memory device, leaving plenty of space for user to
# install more applications via 'apt-get'
# Make sure to use 'trusted' bootscheme for binary naming instead of 'extensible'
# each time it is required
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS_extensible_prepend = "extensible;trusted "
# Additionnal replacement is expected for specific fsbl1 bootloader binary, as
# we should use the 'serialboot' one instead of the 'trusted' one
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS_extensible_fsbl1-boot_append = " trusted;serialboot"
