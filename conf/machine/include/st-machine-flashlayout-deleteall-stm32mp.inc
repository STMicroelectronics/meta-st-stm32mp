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
FLASHLAYOUT_PARTITION_LABELS_deleteall = "fsbl1-boot ssbl-boot fsbl1 fsbl2 emmcall nandall norall sdcardall"

# -----------------------------------------------------------------------------
# Partition configuration for each partition label
FLASHLAYOUT_PARTITION_ENABLE_deleteall            = "PED"
FLASHLAYOUT_PARTITION_ENABLE_deleteall_fsbl1-boot = "-"
FLASHLAYOUT_PARTITION_ENABLE_deleteall_ssbl-boot  = "-"

FLASHLAYOUT_PARTITION_ID_emmcall        = "0x30"
FLASHLAYOUT_PARTITION_ID_nandall        = "0x40"
FLASHLAYOUT_PARTITION_ID_norall         = "0x50"
FLASHLAYOUT_PARTITION_ID_sdcardall      = "0x60"

FLASHLAYOUT_PARTITION_TYPE_emmcall      = "RawImage"
FLASHLAYOUT_PARTITION_TYPE_nandall      = "RawImage"
FLASHLAYOUT_PARTITION_TYPE_norall       = "RawImage"
FLASHLAYOUT_PARTITION_TYPE_sdcardall    = "RawImage"

FLASHLAYOUT_PARTITION_DEVICE_deleteall  = "none:fsbl1-boot ssbl-boot,${DEVICE_EMMC}:fsbl1 fsbl2 emmcall,${DEVICE_NAND}:nandall,${DEVICE_NOR}:norall,${DEVICE_SDCARD}:sdcardall"

FLASHLAYOUT_PARTITION_OFFSET_deleteall  = "0x0"
FLASHLAYOUT_PARTITION_OFFSET_deleteall_fsbl1 = "${DEVICE_START_OFFSET_BOOT0_EMMC}"
FLASHLAYOUT_PARTITION_OFFSET_deleteall_fsbl2 = "${DEVICE_START_OFFSET_BOOT1_EMMC}"

FLASHLAYOUT_PARTITION_BIN2LOAD_deleteall_fsbl1 = "none"
FLASHLAYOUT_PARTITION_BIN2LOAD_deleteall_fsbl2 = "none"

# -----------------------------------------------------------------------------
# The 'deleteall' bootscheme is a trick to generate flashlayout files to clean
# all memory devices on board. There are no specific 'deleteall' bootloader
# binaries so use the proper one ('serialboot', 'optee' or 'trusted').
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS_fsbl1-boot_append = " deleteall;serialboot"
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS_ssbl-boot_append = " deleteall;trusted"
