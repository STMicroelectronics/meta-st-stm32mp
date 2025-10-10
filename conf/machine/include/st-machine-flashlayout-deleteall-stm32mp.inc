#@DESCRIPTION: STM32MP machine flashlayout deleteall configuration

# Add specific scheme to provide flashlayout that will erase all storage devices
FLASHLAYOUT_BOOTSCHEME_LABELS += "deleteall"

# -----------------------------------------------------------------------------
# Define config labels
# -----------------------------------------------------------------------------
FLASHLAYOUT_CONFIG_LABELS:deleteall = "board"

FLASHLAYOUT_CONFIG_LABELS[board] = "\
    ${PARTITIONS_PROGRAMMER_CONFIG} \
    ,emmcboot0,,RawImage            \
    ,emmcboot1,,RawImage            \
    ,emmcall,,RawImage              \
    ,nandall,,RawImage              \
    ,norall,,RawImage               \
    ,sdcardall,,RawImage            \
    ,spinandall,,RawImage           \
    "

# -----------------------------------------------------------------------------
# Define label types for each config
# -----------------------------------------------------------------------------
FLASHLAYOUT_TYPE_LABELS:deleteall:board = "${STM32MP_DEVICETREE_DEFAULT}"

# -----------------------------------------------------------------------------
# Define partitions to consider for flashlayout file generation
#
# NB: We re-use as much as possible partitions already defined in file
#     'st-machine-flashlayout-stm32mp.inc'
# -----------------------------------------------------------------------------
FLASHLAYOUT_PARTITION_LABELS:board = "${@' '.join(['%s' % l.split(',')[1] for l in d.getVarFlag('FLASHLAYOUT_CONFIG_LABELS', 'board').split()])}"

# -----------------------------------------------------------------------------
# Partition configuration for each partition label
FLASHLAYOUT_PARTITION_ENABLE:deleteall = "PED"
FLASHLAYOUT_PARTITION_ENABLE:deleteall:${STM32MP_FSBL_PROGRAMMER_NAME} = "-"
FLASHLAYOUT_PARTITION_ENABLE:deleteall:${STM32MP_SSBL1_PROGRAMMER_NAME} = "-"
FLASHLAYOUT_PARTITION_ENABLE:deleteall:${STM32MP_SSBL2_PROGRAMMER_NAME} = "-"

FLASHLAYOUT_PARTITION_DEVICE:deleteall  = "none:${FLASHLAYOUT_PROGRAMMER_SECTIONS},${DEVICE:EMMC}:emmcboot0 emmcboot1 emmcall,${DEVICE:NAND}:nandall,${DEVICE:NOR}:norall,${DEVICE:SDCARD}:default,${DEVICE:SPINAND}:spinandall"

FLASHLAYOUT_PARTITION_OFFSET:deleteall  = "0x0"
FLASHLAYOUT_PARTITION_OFFSET:deleteall:emmcboot0 = "${DEVICE_START_OFFSET_BOOT0:EMMC}"
FLASHLAYOUT_PARTITION_OFFSET:deleteall:emmcboot1 = "${DEVICE_START_OFFSET_BOOT1:EMMC}"

# -----------------------------------------------------------------------------
# The 'deleteall' bootscheme is a trick to generate flashlayout files to clean
# all memory devices on board. There are no specific 'deleteall' bootloaders
# so we need to manage specific override for FLASHLAYOUT_PROGRAMMER_SECTIONS binaries
BOOTSCHEME_REPLACE = "${@'opteemin' if bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', True, False, d) and not bb.utils.contains('BOOTSCHEME_LABELS', 'optee', True, False, d) else 'optee'}"
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS:${STM32MP_FSBL_PROGRAMMER_NAME}:append = " deleteall;${BOOTSCHEME_REPLACE}"
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS:${STM32MP_SSBL1_PROGRAMMER_NAME}:append = " deleteall;${BOOTSCHEME_REPLACE}"
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS:${STM32MP_SSBL2_PROGRAMMER_NAME}:append = " deleteall;${BOOTSCHEME_REPLACE}"
