#@DESCRIPTION: STM32MP machine flashlayout deleteall configuration

# Add specific scheme to provide flashlayout that will erase all storage devices
FLASHLAYOUT_BOOTSCHEME_LABELS += "deleteall"

# -----------------------------------------------------------------------------
# Define config labels
# -----------------------------------------------------------------------------
FLASHLAYOUT_CONFIG_LABELS:deleteall = "disco eval"

# -----------------------------------------------------------------------------
# Define label types for each config
# -----------------------------------------------------------------------------
FLASHLAYOUT_TYPE_LABELS:deleteall:disco = "${STM32MP_DT_FILES_DK}"
FLASHLAYOUT_TYPE_LABELS:deleteall:eval  = "${STM32MP_DT_FILES_ED} ${STM32MP_DT_FILES_EV}"

# -----------------------------------------------------------------------------
# Define partitions to consider for flashlayout file generation
#
# NB: We re-use as much as possible partitions already defined in file
#     'st-machine-flashlayout-stm32mp.inc'
# -----------------------------------------------------------------------------
FLASHLAYOUT_PARTITION_LABELS:deleteall = "${FLASHLAYOUT_PROGRAMMER_SECTIONS} emmcboot0 emmcboot1 emmcall nandall norall sdcardall spinandall"

# -----------------------------------------------------------------------------
# Partition configuration for each partition label
FLASHLAYOUT_PARTITION_ENABLE:deleteall = "PED"
FLASHLAYOUT_PARTITION_ENABLE:deleteall:${STM32MP_FSBL_PROGAMMER_NAME} = "-"
FLASHLAYOUT_PARTITION_ENABLE:deleteall:${STM32MP_SSBL_PROGAMMER_NAME} = "-"

FLASHLAYOUT_PARTITION_TYPE:emmcall      = "RawImage"
FLASHLAYOUT_PARTITION_TYPE:nandall      = "RawImage"
FLASHLAYOUT_PARTITION_TYPE:norall       = "RawImage"
FLASHLAYOUT_PARTITION_TYPE:sdcardall    = "RawImage"
FLASHLAYOUT_PARTITION_TYPE:spinandall   = "RawImage"

FLASHLAYOUT_PARTITION_DEVICE:deleteall  = "none:${FLASHLAYOUT_PROGRAMMER_SECTIONS},${DEVICE:EMMC}:emmcboot0 emmcboot1 emmcall,${DEVICE:NAND}:nandall,${DEVICE:NOR}:norall,${DEVICE:SDCARD}:sdcardall,${DEVICE:SPINAND}:spinandall"

FLASHLAYOUT_PARTITION_OFFSET:deleteall  = "0x0"
FLASHLAYOUT_PARTITION_OFFSET:deleteall:emmcboot0 = "${DEVICE_START_OFFSET_BOOT0:EMMC}"
FLASHLAYOUT_PARTITION_OFFSET:deleteall:emmcboot1 = "${DEVICE_START_OFFSET_BOOT1:EMMC}"

# -----------------------------------------------------------------------------
# The 'deleteall' bootscheme is a trick to generate flashlayout files to clean
# all memory devices on board. There are no specific 'deleteall' bootloaders
# so we need to manage specific override for FLASHLAYOUT_PROGRAMMER_SECTIONS binaries
BOOTSCHEME_REPLACE = "${@'trusted' if bb.utils.contains('BOOTSCHEME_LABELS', 'trusted', True, False, d) and not bb.utils.contains('BOOTSCHEME_LABELS', 'optee', True, False, d) else 'optee'}"
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS:${STM32MP_SSBL_PROGAMMER_NAME}:append = " deleteall;${BOOTSCHEME_REPLACE}"
