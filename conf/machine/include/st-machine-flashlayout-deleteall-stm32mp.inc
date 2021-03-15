#@DESCRIPTION: STM32MP machine flashlayout deleteall configuration

# Add specific scheme to provide flashlayout that will erase all storage devices
FLASHLAYOUT_BOOTSCHEME_LABELS += "deleteall"

# -----------------------------------------------------------------------------
# Define config labels
# -----------------------------------------------------------------------------
FLASHLAYOUT_CONFIG_LABELS_deleteall = "disco eval"

# -----------------------------------------------------------------------------
# Define label types for each config
# -----------------------------------------------------------------------------
FLASHLAYOUT_TYPE_LABELS_deleteall_disco = "${STM32MP_DT_FILES_DK}"
FLASHLAYOUT_TYPE_LABELS_deleteall_eval  = "${STM32MP_DT_FILES_ED} ${STM32MP_DT_FILES_EV}"

# -----------------------------------------------------------------------------
# Define partitions to consider for flashlayout file generation
#
# NB: We re-use as much as possible partitions already defined in file
#     'st-machine-flashlayout-stm32mp.inc'
# -----------------------------------------------------------------------------
FLASHLAYOUT_PARTITION_LABELS_deleteall = "${FLASHLAYOUT_PROGRAMMER_SECTIONS} emmcboot0 emmcboot1 emmcall nandall norall sdcardall spinandall"

# -----------------------------------------------------------------------------
# Partition configuration for each partition label
FLASHLAYOUT_PARTITION_ENABLE_deleteall = "PED"
FLASHLAYOUT_PARTITION_ENABLE_deleteall_${STM32MP_FSBL1_NAME}-boot = "-"
FLASHLAYOUT_PARTITION_ENABLE_deleteall_${STM32MP_SSBL1_NAME}-boot = "-"

FLASHLAYOUT_PARTITION_TYPE_emmcall      = "RawImage"
FLASHLAYOUT_PARTITION_TYPE_nandall      = "RawImage"
FLASHLAYOUT_PARTITION_TYPE_norall       = "RawImage"
FLASHLAYOUT_PARTITION_TYPE_sdcardall    = "RawImage"
FLASHLAYOUT_PARTITION_TYPE_spinandall   = "RawImage"

FLASHLAYOUT_PARTITION_DEVICE_deleteall  = "none:${FLASHLAYOUT_PROGRAMMER_SECTIONS},${DEVICE_EMMC}:emmcboot0 emmcboot1 emmcall,${DEVICE_NAND}:nandall,${DEVICE_NOR}:norall,${DEVICE_SDCARD}:sdcardall,${DEVICE_SPINAND}:spinandall"

FLASHLAYOUT_PARTITION_OFFSET_deleteall  = "0x0"
FLASHLAYOUT_PARTITION_OFFSET_deleteall_emmcboot0 = "${DEVICE_START_OFFSET_BOOT0_EMMC}"
FLASHLAYOUT_PARTITION_OFFSET_deleteall_emmcboot1 = "${DEVICE_START_OFFSET_BOOT1_EMMC}"

# -----------------------------------------------------------------------------
# The 'deleteall' bootscheme is a trick to generate flashlayout files to clean
# all memory devices on board. There are no specific 'deleteall' bootloaders
# so we need to manage specific override for FLASHLAYOUT_PROGRAMMER_SECTIONS binaries
BOOTSCHEME_REPLACE = "${@'optee' if bb.utils.contains('BOOTSCHEME_LABELS', 'optee', True, False, d) and not bb.utils.contains('BOOTSCHEME_LABELS', 'trusted', True, False, d) else 'trusted'}"
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS_${STM32MP_SSBL1_NAME}-boot_append = " deleteall;${BOOTSCHEME_REPLACE}"
