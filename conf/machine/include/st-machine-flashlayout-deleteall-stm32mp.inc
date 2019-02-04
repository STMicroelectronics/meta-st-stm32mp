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
FLASHLAYOUT_PARTITION_LABELS_deleteall_disco = "fsbl1-boot ssbl-boot sdcard-all"
FLASHLAYOUT_PARTITION_LABELS_deleteall_eval  = "fsbl1-boot ssbl-boot emmc-fsbl1 emmc-fsbl2 emmc-all nand-4-256-all nor-all sdcard-all"

# -----------------------------------------------------------------------------
# Partition configuration for each partition label
FLASHLAYOUT_PARTITION_ENABLE_emmc-fsbl1     = "PED"
FLASHLAYOUT_PARTITION_ENABLE_emmc-fsbl2     = "PED"
FLASHLAYOUT_PARTITION_ENABLE_emmc-all       = "PED"
FLASHLAYOUT_PARTITION_ENABLE_nand-4-256-all = "PED"
FLASHLAYOUT_PARTITION_ENABLE_nor-all        = "PED"
FLASHLAYOUT_PARTITION_ENABLE_sdcard-all     = "PED"

FLASHLAYOUT_PARTITION_ID_emmc-fsbl1     = "0x04"
FLASHLAYOUT_PARTITION_ID_emmc-fsbl2     = "0x05"
FLASHLAYOUT_PARTITION_ID_emmc-all       = "0x30"
FLASHLAYOUT_PARTITION_ID_nand-4-256-all = "0x40"
FLASHLAYOUT_PARTITION_ID_nor-all        = "0x50"
FLASHLAYOUT_PARTITION_ID_sdcard-all     = "0x60"

FLASHLAYOUT_PARTITION_TYPE_emmc-all       = "RawImage"
FLASHLAYOUT_PARTITION_TYPE_nand-4-256-all = "RawImage"
FLASHLAYOUT_PARTITION_TYPE_nor-all        = "RawImage"
FLASHLAYOUT_PARTITION_TYPE_sdcard-all     = "RawImage"

FLASHLAYOUT_PARTITION_DEVICE_emmc-fsbl1     = "${DEVICE_EMMC}"
FLASHLAYOUT_PARTITION_DEVICE_emmc-fsbl2     = "${DEVICE_EMMC}"
FLASHLAYOUT_PARTITION_DEVICE_emmc-all       = "${DEVICE_EMMC}"
FLASHLAYOUT_PARTITION_DEVICE_nand-4-256-all = "${DEVICE_NAND}"
FLASHLAYOUT_PARTITION_DEVICE_nor-all        = "${DEVICE_NOR}"
FLASHLAYOUT_PARTITION_DEVICE_sdcard-all     = "${DEVICE_SDCARD}"
# Specific for fsbl1-boot ssbl-boot partitions
FLASHLAYOUT_PARTITION_DEVICE_deleteall_fsbl1-boot = "none"
FLASHLAYOUT_PARTITION_DEVICE_deleteall_ssbl-boot  = "none"

FLASHLAYOUT_PARTITION_OFFSET_deleteall = "0x0"
FLASHLAYOUT_PARTITION_OFFSET_deleteall_emmc-fsbl1 = "${FLASHLAYOUT_PARTITION_OFFSET_emmc_fsbl1}"
FLASHLAYOUT_PARTITION_OFFSET_deleteall_emmc-fsbl2 = "${FLASHLAYOUT_PARTITION_OFFSET_emmc_fsbl2}"

# -----------------------------------------------------------------------------
# The 'deletall' bootscheme is a trick to generate flashlayout files to clean
# all memory devices on board. There are no specific 'deleteall' bootloader
# binaries so use the 'trusted' or 'optee' one.
BIN2BOOT_REPLACE_PATTERNS_fsbl1-boot_append = " deleteall;trusted"
BIN2BOOT_REPLACE_PATTERNS_ssbl-boot_append = " ${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', 'deleteall;optee', 'deleteall;trusted', d)}"
