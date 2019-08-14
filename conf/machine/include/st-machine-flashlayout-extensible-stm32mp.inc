#@DESCRIPTION: STM32MP machine flashlayout extensible configuration

# Set configuration file to monitor
FLASHLAYOUT_CONFIGURE_FILES_append = " ${STM32MP_BASE}/conf/machine/include/st-machine-flashlayout-extensible-stm32mp.inc:True "

# -----------------------------------------------------------------------------
# Define config labels
# -----------------------------------------------------------------------------
FLASHLAYOUT_CONFIG_LABELS_extensible = "sdcard"

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
FLASHLAYOUT_PARTITION_LABELS_extensible = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl bootfs ${VENDORFS_PART} rootfs"

# -----------------------------------------------------------------------------
# Partition configuration for each partition label
# Specific for fsbl1-boot ssbl-boot partitions
FLASHLAYOUT_PARTITION_DEVICE_extensible_fsbl1-boot = "none"
FLASHLAYOUT_PARTITION_DEVICE_extensible_ssbl-boot  = "none"

# -----------------------------------------------------------------------------
# The 'extensible' bootscheme is a trick to generate flashlayout files without
# userfs partition for trusted bootscheme ONLY. So rootfs partition will be
# extended up to the end of memory device, leaving plenty of space for user to
# install more applications via 'apt-get'
BIN2BOOT_REPLACE_PATTERNS_extensible_append = " extensible;trusted"
