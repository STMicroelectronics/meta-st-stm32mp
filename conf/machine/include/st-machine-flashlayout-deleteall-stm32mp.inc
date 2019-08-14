#@DESCRIPTION: STM32MP machine flashlayout deleteall configuration

# Set configuration file to monitor
FLASHLAYOUT_CONFIGURE_FILES_append = " ${STM32MP_BASE}/conf/machine/include/st-machine-flashlayout-deleteall-stm32mp.inc:True "

# -----------------------------------------------------------------------------
# Define config labels
# -----------------------------------------------------------------------------
FLASHLAYOUT_CONFIG_LABELS_deleteall = "disco eval"

# -----------------------------------------------------------------------------
# Define label types
# -----------------------------------------------------------------------------
FLASHLAYOUT_TYPE_LABELS_deleteall_disco = "${STM32MP_DT_FILES_DK}"
FLASHLAYOUT_TYPE_LABELS_deleteall_eval  = "${STM32MP_DT_FILES_ED} ${STM32MP_DT_FILES_EV}"

# -----------------------------------------------------------------------------
# Define partitions to use
#
# NB: To manage bootloader partitions, simplification is done by directly
#     re-using 'fsbl1-boot' and 'ssbl-boot' partitions already defined in file
#     'st-machine-flashlayout-stm32mp.inc'
# -----------------------------------------------------------------------------
FLASHLAYOUT_PARTITION_LABELS_deleteall_disco = "fsbl1-boot ssbl-boot sdcardall"
FLASHLAYOUT_PARTITION_LABELS_deleteall_eval  = "fsbl1-boot ssbl-boot emmcfsbl1 emmcfsbl2 emmcall nandall norall sdcardall"

# -----------------------------------------------------------------------------
# Partition configuration for each partition label
FLASHLAYOUT_PARTITION_ENABLE_emmcfsbl1  = "PED"
FLASHLAYOUT_PARTITION_ENABLE_emmcfsbl2  = "PED"
FLASHLAYOUT_PARTITION_ENABLE_emmcall    = "PED"
FLASHLAYOUT_PARTITION_ENABLE_nandall    = "PED"
FLASHLAYOUT_PARTITION_ENABLE_norall     = "PED"
FLASHLAYOUT_PARTITION_ENABLE_sdcardall  = "PED"

FLASHLAYOUT_PARTITION_ID_emmcfsbl1  = "0x08"
FLASHLAYOUT_PARTITION_ID_emmcfsbl2  = "0x09"
FLASHLAYOUT_PARTITION_ID_emmcall    = "0x30"
FLASHLAYOUT_PARTITION_ID_nandall    = "0x40"
FLASHLAYOUT_PARTITION_ID_norall     = "0x50"
FLASHLAYOUT_PARTITION_ID_sdcardall  = "0x60"

FLASHLAYOUT_PARTITION_TYPE_emmcall      = "RawImage"
FLASHLAYOUT_PARTITION_TYPE_nandall      = "RawImage"
FLASHLAYOUT_PARTITION_TYPE_norall       = "RawImage"
FLASHLAYOUT_PARTITION_TYPE_sdcardall    = "RawImage"

FLASHLAYOUT_PARTITION_DEVICE_emmcfsbl1  = "${DEVICE_EMMC}"
FLASHLAYOUT_PARTITION_DEVICE_emmcfsbl2  = "${DEVICE_EMMC}"
FLASHLAYOUT_PARTITION_DEVICE_emmcall    = "${DEVICE_EMMC}"
FLASHLAYOUT_PARTITION_DEVICE_nandall    = "${DEVICE_NAND}"
FLASHLAYOUT_PARTITION_DEVICE_norall     = "${DEVICE_NOR}"
FLASHLAYOUT_PARTITION_DEVICE_sdcardall  = "${DEVICE_SDCARD}"
# Specific for fsbl1-boot ssbl-boot partitions
FLASHLAYOUT_PARTITION_DEVICE_deleteall_fsbl1-boot = "none"
FLASHLAYOUT_PARTITION_DEVICE_deleteall_ssbl-boot  = "none"

FLASHLAYOUT_PARTITION_OFFSET_deleteall = "0x0"
FLASHLAYOUT_PARTITION_OFFSET_deleteall_emmcfsbl1 = "${FLASHLAYOUT_PARTITION_OFFSET_emmc_fsbl1}"
FLASHLAYOUT_PARTITION_OFFSET_deleteall_emmcfsbl2 = "${FLASHLAYOUT_PARTITION_OFFSET_emmc_fsbl2}"

# -----------------------------------------------------------------------------
# The 'deleteall' bootscheme is a trick to generate flashlayout files to clean
# all memory devices on board. There are no specific 'deleteall' bootloader
# binaries so use the 'trusted' or 'optee' one.
BIN2BOOT_REPLACE_PATTERNS_fsbl1-boot_append = " deleteall;trusted"
BIN2BOOT_REPLACE_PATTERNS_ssbl-boot_append = " ${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', 'deleteall;optee', 'deleteall;trusted', d)}"
