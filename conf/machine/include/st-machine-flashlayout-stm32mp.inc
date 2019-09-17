#@DESCRIPTION: STM32MP machine flashlayout configuration

INHERIT += "flashlayout-stm32mp"

# Set configuration file to monitor
FLASHLAYOUT_CONFIGURE_FILES_append = " ${STM32MP_BASE}/conf/machine/include/st-machine-flashlayout-stm32mp.inc:True "

# Add specific dependencies to get all binaries generated before flashlayout files
FLASHLAYOUT_DEPEND_TASKS += "${@bb.utils.contains('EXTRA_IMAGEDEPENDS', 'tf-a-stm32mp', 'tf-a-stm32mp:do_deploy', '', d)}"
FLASHLAYOUT_DEPEND_TASKS += "${@bb.utils.contains('EXTRA_IMAGEDEPENDS', 'u-boot-stm32mp', 'u-boot-stm32mp:do_deploy', '', d)}"
FLASHLAYOUT_DEPEND_TASKS += "${@bb.utils.contains('EXTRA_IMAGEDEPENDS', 'optee-os-stm32mp', 'optee-os-stm32mp:do_deploy', '', d)}"

# -----------------------------------------------------------------------------
# Define flashlayout devices
# -----------------------------------------------------------------------------
DEVICE_EMMC   = "mmc1"
DEVICE_NAND   = "nand0"
DEVICE_NOR    = "nor0"
DEVICE_SDCARD = "mmc0"
# -----------------------------------------------------------------------------
# EMMC
# Extra space is required to store 'Protective MBR' and 'Primary GPT Header'
# Currently the required size is 17kBytes (i.e. 0x4400)
# We need to align this size to get the first offset to use
DEVICE_START_OFFSET_mmc1    = "0x00080000"
DEVICE_ALIGNMENT_SIZE_mmc1  = "0x00080000"
# -----------------------------------------------------------------------------
# NAND
DEVICE_START_OFFSET_nand0   = "0x00000000"
DEVICE_ALIGNMENT_SIZE_nand0 = "0x00040000"
# -----------------------------------------------------------------------------
# NOR
DEVICE_START_OFFSET_nor0    = "0x00000000"
DEVICE_ALIGNMENT_SIZE_nor0  = "0x00010000"
# -----------------------------------------------------------------------------
# SDCARD
# Extra space is required to store 'Protective MBR' and 'Primary GPT Header'
# Currently the required size is 17kBytes (i.e. 0x4400)
# We need to align this size to get the first offset to use
DEVICE_START_OFFSET_mmc0    = "0x00004400"
DEVICE_ALIGNMENT_SIZE_mmc0  = "0x00000200"

# -----------------------------------------------------------------------------
# Define bootscheme labels
# -----------------------------------------------------------------------------
FLASHLAYOUT_BOOTSCHEME_LABELS ??= "basic optee trusted"

# -----------------------------------------------------------------------------
# Define config labels
# -----------------------------------------------------------------------------
# NOTE: define can be done with following priority assignment:
#           1) list_<BOOTSCHEME>
#           2) list
#           3) Default 'list' to 'none' when not defined
# -----------------------------------------------------------------------------
FLASHLAYOUT_CONFIG_LABELS ??= "emmc nand-4-256 nor-sdcard nor-emmc nor-nand-4-256 sdcard"

# Set by default this variable to 0, and set to 1 only when we are using st-example-image-*
ST_EXAMPLE_IMAGE ??= "0"

# Remove NAND flashlayout when we are using st-example-image-* as rootfs too big for a NAND device size of 1 GBytes
#FLASHLAYOUT_CONFIG_LABELS_remove = "${@bb.utils.contains('ST_EXAMPLE_IMAGE', '1', 'emmc', '', d)}"
FLASHLAYOUT_CONFIG_LABELS_remove = "${@bb.utils.contains('ST_EXAMPLE_IMAGE', '1', 'nand-4-256', '', d)}"
#FLASHLAYOUT_CONFIG_LABELS_remove = "${@bb.utils.contains('ST_EXAMPLE_IMAGE', '1', 'nor-sdcard', '', d)}"
#FLASHLAYOUT_CONFIG_LABELS_remove = "${@bb.utils.contains('ST_EXAMPLE_IMAGE', '1', 'nor-emmc', '', d)}"
FLASHLAYOUT_CONFIG_LABELS_remove = "${@bb.utils.contains('ST_EXAMPLE_IMAGE', '1', 'nor-nand-4-256', '', d)}"
#FLASHLAYOUT_CONFIG_LABELS_remove = "${@bb.utils.contains('ST_EXAMPLE_IMAGE', '1', 'sdcard', '', d)}"

# -----------------------------------------------------------------------------
# Define label types
# -----------------------------------------------------------------------------
# NOTE: define can be done with following priority assignment:
#           1) list_<BOOTSCHEME>_<CONFIG>
#           2) list_<BOOTSCHEME>
#           3) list_<CONFIG>
#           4) list
#           5) Default 'list' to 'none' when not defined
# -----------------------------------------------------------------------------
# EMMC
# Set flashlayout file generation to eval board (mother and daughter) only
FLASHLAYOUT_TYPE_LABELS_emmc = "${STM32MP_DT_FILES_ED} ${STM32MP_DT_FILES_EV}"
# NAND
# Set flashlayout file generation to eval board only
FLASHLAYOUT_TYPE_LABELS_nand-4-256 = "${STM32MP_DT_FILES_EV}"
# NOR
# Set flashlayout file generation to eval board only
FLASHLAYOUT_TYPE_LABELS_nor-emmc       = "${STM32MP_DT_FILES_EV}"
FLASHLAYOUT_TYPE_LABELS_nor-nand-4-256 = "${STM32MP_DT_FILES_EV}"
FLASHLAYOUT_TYPE_LABELS_nor-sdcard     = "${STM32MP_DT_FILES_EV}"
# SDCARD
# Set flashlayout file generation for all boards
FLASHLAYOUT_TYPE_LABELS_sdcard = "${STM32MP_DT_FILES_DK} ${STM32MP_DT_FILES_ED} ${STM32MP_DT_FILES_EV}"

# -----------------------------------------------------------------------------
# Define partitions to use
# -----------------------------------------------------------------------------
# There are few restrictions to follow:
#   - The partition for the first boot loader should follow the naming rule:
#       fsbl*
#   - The partition for the secondary boot loader should follow the naming rule:
#       ssbl
# -----------------------------------------------------------------------------
# NOTE: define can be done with following priority assignment:
#           1) list_<BOOTSCHEME>_<CONFIG>
#           2) list_<BOOTSCHEME>
#           3) list_<CONFIG>
#           4) list
#           5) Default 'list' to 'none' when not defined
# -----------------------------------------------------------------------------
VENDORFS_PART = "${@bb.utils.contains('ST_VENDORFS','1','vendorfs','',d)}"

FLASHLAYOUT_PARTITION_LABELS_basic_emmc           = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl bootfs ${VENDORFS_PART} rootfs userfs"
FLASHLAYOUT_PARTITION_LABELS_basic_nand-4-256     = "none"
FLASHLAYOUT_PARTITION_LABELS_basic_nor-nand-4-256 = "none"
FLASHLAYOUT_PARTITION_LABELS_basic_nor-emmc       = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl env empty bootfs ${VENDORFS_PART} rootfs userfs"
FLASHLAYOUT_PARTITION_LABELS_basic_nor-sdcard     = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl env empty bootfs ${VENDORFS_PART} rootfs userfs"
FLASHLAYOUT_PARTITION_LABELS_basic_sdcard         = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl bootfs ${VENDORFS_PART} rootfs userfs"

FLASHLAYOUT_PARTITION_LABELS_optee_emmc           = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl teeh teed teex bootfs ${VENDORFS_PART} rootfs userfs"
FLASHLAYOUT_PARTITION_LABELS_optee_nand-4-256     = "fsbl1-boot ssbl-boot fsbl1 ssbl ssbl2 teeh teed teex ubifs"
FLASHLAYOUT_PARTITION_LABELS_optee_nor-nand-4-256 = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl env teeh teed teex empty emptyfsbl1 emptyssbl emptyssbl2 emptyteeh emptyteed emptyteex ubifs"
FLASHLAYOUT_PARTITION_LABELS_optee_nor-emmc       = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl env teeh teed teex empty bootfs ${VENDORFS_PART} rootfs userfs"
FLASHLAYOUT_PARTITION_LABELS_optee_nor-sdcard     = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl env teeh teed teex empty bootfs ${VENDORFS_PART} rootfs userfs"
FLASHLAYOUT_PARTITION_LABELS_optee_sdcard         = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl teeh teed teex bootfs ${VENDORFS_PART} rootfs userfs"

FLASHLAYOUT_PARTITION_LABELS_trusted_emmc           = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl bootfs ${VENDORFS_PART} rootfs userfs"
FLASHLAYOUT_PARTITION_LABELS_trusted_nand-4-256     = "fsbl1-boot ssbl-boot fsbl1 ssbl ssbl2 ubifs"
FLASHLAYOUT_PARTITION_LABELS_trusted_nor-nand-4-256 = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl env empty emptyfsbl1 emptyssbl emptyssbl2 ubifs"
FLASHLAYOUT_PARTITION_LABELS_trusted_nor-emmc       = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl env empty bootfs ${VENDORFS_PART} rootfs userfs"
FLASHLAYOUT_PARTITION_LABELS_trusted_nor-sdcard     = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl env empty bootfs ${VENDORFS_PART} rootfs userfs"
FLASHLAYOUT_PARTITION_LABELS_trusted_sdcard         = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl bootfs ${VENDORFS_PART} rootfs userfs"

# To manage properly dynamic boot for NOR use case, our 'boot.src' script scans
# one after one the devices to find out expected binaries from bootfs partition.
# Currently the order followed by the script is:

# So proposal here is to make sure to erase device we do not want to use to boot
# for NOR-* boot configuration
FLASHLAYOUT_PARTITION_LABELS_basic_nor-emmc       += "nandall sdcardall"

FLASHLAYOUT_PARTITION_LABELS_optee_nor-nand-4-256 += "sdcardall"
FLASHLAYOUT_PARTITION_LABELS_optee_nor-emmc       += "nandall sdcardall"

FLASHLAYOUT_PARTITION_LABELS_trusted_nor-nand-4-256 += "sdcardall"
FLASHLAYOUT_PARTITION_LABELS_trusted_nor-emmc       += "nandall sdcardall"

# -----------------------------------------------------------------------------
# Partition configuration for each partition label
# NOTE: each item can be defined with following priority assignment:
#           1) item_<BOOTSCHEME>_<CONFIG>_<PARTITION>
#           2) item_<BOOTSCHEME>_<CONFIG>
#           3) item_<BOOTSCHEME>_<PARTITION>
#           4) item_<BOOTSCHEME>
#           5) item_<CONFIG>_<PARTITION>
#           6) item_<CONFIG>
#           7) item_<PARTITION>
#           8) item
#           9) Default 'item' to 'none' when not defined
# -----------------------------------------------------------------------------
FLASHLAYOUT_PARTITION_ENABLE                = "P"
FLASHLAYOUT_PARTITION_ENABLE_fsbl1-boot     = "-"
FLASHLAYOUT_PARTITION_ENABLE_ssbl-boot      = "-"
FLASHLAYOUT_PARTITION_ENABLE_empty          = "PE"
FLASHLAYOUT_PARTITION_ENABLE_emptyfsbl1     = "PE"
FLASHLAYOUT_PARTITION_ENABLE_emptyssbl      = "PE"
FLASHLAYOUT_PARTITION_ENABLE_emptyssbl2     = "PE"
FLASHLAYOUT_PARTITION_ENABLE_emptyteeh      = "PE"
FLASHLAYOUT_PARTITION_ENABLE_emptyteed      = "PE"
FLASHLAYOUT_PARTITION_ENABLE_emptyteex      = "PE"
FLASHLAYOUT_PARTITION_ENABLE_env            = "PED"

# -----------------------------------------------------------------------------
# Partition ID
# -----------------------------------------------------------------------------
# The STM32CubeProgrammer supported ID range is:
#   0x00 to 0xFF
# Some IDs are reserved for internal usage on STM32CubeProgrammer and special
# management is implemented for binary with STM32 header. This means that for
# flashlayout files, available ID range is only:
#   0x01 to 0x0F for Boot partitions with STM32 header
#   0x10 to 0xF0 for User partitions programmed without header
# Note also that for FSBL and SSBL binaries loaded in RAM to program the devices
# there are two reserved IDs
#   0x01 for FSBL
#   0x03 for SSBL
FLASHLAYOUT_PARTITION_ID_fsbl1-boot     = "0x01"
FLASHLAYOUT_PARTITION_ID_ssbl-boot      = "0x03"
FLASHLAYOUT_PARTITION_ID_fsbl1          = "0x04"
FLASHLAYOUT_PARTITION_ID_fsbl2          = "0x05"
FLASHLAYOUT_PARTITION_ID_ssbl           = "0x06"
FLASHLAYOUT_PARTITION_ID_ssbl2          = "0x07"
FLASHLAYOUT_PARTITION_ID_teeh           = "0x0A"
FLASHLAYOUT_PARTITION_ID_teed           = "0x0B"
FLASHLAYOUT_PARTITION_ID_teex           = "0x0C"
FLASHLAYOUT_PARTITION_ID_empty          = "0x10"
FLASHLAYOUT_PARTITION_ID_emptyfsbl1     = "0x11"
FLASHLAYOUT_PARTITION_ID_emptyssbl      = "0x12"
FLASHLAYOUT_PARTITION_ID_emptyssbl2     = "0x13"
FLASHLAYOUT_PARTITION_ID_emptyteeh      = "0x14"
FLASHLAYOUT_PARTITION_ID_emptyteed      = "0x15"
FLASHLAYOUT_PARTITION_ID_emptyteex      = "0x16"
FLASHLAYOUT_PARTITION_ID_env            = "0x20"
FLASHLAYOUT_PARTITION_ID_ubifs          = "0x21"
FLASHLAYOUT_PARTITION_ID_bootfs         = "0x21"
FLASHLAYOUT_PARTITION_ID_vendorfs       = "0x22"
FLASHLAYOUT_PARTITION_ID_rootfs         = "0x23"
FLASHLAYOUT_PARTITION_ID_userfs         = "0x24"

FLASHLAYOUT_PARTITION_TYPE                  = "Binary"
FLASHLAYOUT_PARTITION_TYPE_nand-4-256_fsbl1 = "Binary(2)"
FLASHLAYOUT_PARTITION_TYPE_ubifs            = "System"
FLASHLAYOUT_PARTITION_TYPE_bootfs           = "System"
FLASHLAYOUT_PARTITION_TYPE_vendorfs         = "FileSystem"
FLASHLAYOUT_PARTITION_TYPE_rootfs           = "FileSystem"
FLASHLAYOUT_PARTITION_TYPE_userfs           = "FileSystem"

FLASHLAYOUT_PARTITION_DEVICE_emmc = "${DEVICE_EMMC}"
FLASHLAYOUT_PARTITION_DEVICE_nand-4-256 = "${DEVICE_NAND}"
FLASHLAYOUT_PARTITION_DEVICE_nor = "${DEVICE_NOR}"
FLASHLAYOUT_PARTITION_DEVICE_nor-emmc          = "${DEVICE_NOR}"
FLASHLAYOUT_PARTITION_DEVICE_nor-emmc_bootfs   = "${DEVICE_EMMC}"
FLASHLAYOUT_PARTITION_DEVICE_nor-emmc_vendorfs = "${DEVICE_EMMC}"
FLASHLAYOUT_PARTITION_DEVICE_nor-emmc_rootfs   = "${DEVICE_EMMC}"
FLASHLAYOUT_PARTITION_DEVICE_nor-emmc_userfs   = "${DEVICE_EMMC}"
FLASHLAYOUT_PARTITION_DEVICE_nor-nand-4-256             = "${DEVICE_NOR}"
FLASHLAYOUT_PARTITION_DEVICE_nor-nand-4-256_emptyfsbl1  = "${DEVICE_NAND}"
FLASHLAYOUT_PARTITION_DEVICE_nor-nand-4-256_emptyssbl   = "${DEVICE_NAND}"
FLASHLAYOUT_PARTITION_DEVICE_nor-nand-4-256_emptyssbl2  = "${DEVICE_NAND}"
FLASHLAYOUT_PARTITION_DEVICE_nor-nand-4-256_emptyteeh   = "${DEVICE_NAND}"
FLASHLAYOUT_PARTITION_DEVICE_nor-nand-4-256_emptyteed   = "${DEVICE_NAND}"
FLASHLAYOUT_PARTITION_DEVICE_nor-nand-4-256_emptyteex   = "${DEVICE_NAND}"
FLASHLAYOUT_PARTITION_DEVICE_nor-nand-4-256_ubifs       = "${DEVICE_NAND}"
FLASHLAYOUT_PARTITION_DEVICE_nor-sdcard          = "${DEVICE_NOR}"
FLASHLAYOUT_PARTITION_DEVICE_nor-sdcard_bootfs   = "${DEVICE_SDCARD}"
FLASHLAYOUT_PARTITION_DEVICE_nor-sdcard_vendorfs = "${DEVICE_SDCARD}"
FLASHLAYOUT_PARTITION_DEVICE_nor-sdcard_rootfs   = "${DEVICE_SDCARD}"
FLASHLAYOUT_PARTITION_DEVICE_nor-sdcard_userfs   = "${DEVICE_SDCARD}"
FLASHLAYOUT_PARTITION_DEVICE_sdcard = "${DEVICE_SDCARD}"
# Specific for fsbl1-boot ssbl-boot partitions
FLASHLAYOUT_PARTITION_DEVICE_basic_fsbl1-boot = "none"
FLASHLAYOUT_PARTITION_DEVICE_basic_ssbl-boot  = "none"
FLASHLAYOUT_PARTITION_DEVICE_optee_fsbl1-boot = "none"
FLASHLAYOUT_PARTITION_DEVICE_optee_ssbl-boot  = "none"
FLASHLAYOUT_PARTITION_DEVICE_trusted_fsbl1-boot = "none"
FLASHLAYOUT_PARTITION_DEVICE_trusted_ssbl-boot  = "none"

# To manage NOR-* boot partition erase
FLASHLAYOUT_PARTITION_DEVICE_nor-emmc_nandall   = "${DEVICE_NAND}"
FLASHLAYOUT_PARTITION_DEVICE_nor-emmc_sdcardall = "${DEVICE_SDCARD}"
FLASHLAYOUT_PARTITION_DEVICE_nor-nand-4-256_sdcardall = "${DEVICE_SDCARD}"

FLASHLAYOUT_PARTITION_OFFSET_fsbl1-boot = "0x0"
FLASHLAYOUT_PARTITION_OFFSET_ssbl-boot  = "0x0"
FLASHLAYOUT_PARTITION_OFFSET_emmc_fsbl1 = "boot1"
FLASHLAYOUT_PARTITION_OFFSET_emmc_fsbl2 = "boot2"
FLASHLAYOUT_PARTITION_OFFSET_emmc_ssbl  = "${DEVICE_START_OFFSET_mmc1}"
FLASHLAYOUT_PARTITION_OFFSET_nand-4-256_fsbl1 = "${DEVICE_START_OFFSET_nand0}"
FLASHLAYOUT_PARTITION_OFFSET_nor-sdcard_fsbl1  = "${DEVICE_START_OFFSET_nor0}"
FLASHLAYOUT_PARTITION_OFFSET_nor-sdcard_bootfs = "${DEVICE_START_OFFSET_mmc0}"
FLASHLAYOUT_PARTITION_OFFSET_nor-emmc_fsbl1  = "${DEVICE_START_OFFSET_nor0}"
FLASHLAYOUT_PARTITION_OFFSET_nor-emmc_bootfs = "${DEVICE_START_OFFSET_mmc1}"
FLASHLAYOUT_PARTITION_OFFSET_nor-nand-4-256_fsbl1      = "${DEVICE_START_OFFSET_nor0}"
FLASHLAYOUT_PARTITION_OFFSET_nor-nand-4-256_emptyfsbl1 = "${DEVICE_START_OFFSET_nand0}"
FLASHLAYOUT_PARTITION_OFFSET_sdcard_fsbl1 = "${DEVICE_START_OFFSET_mmc0}"

# To manage NOR-* boot partition erase
FLASHLAYOUT_PARTITION_OFFSET_nandall    = "0x0"
FLASHLAYOUT_PARTITION_OFFSET_sdcardall  = "0x0"

# Size defined in Kbytes
FLASHLAYOUT_PARTITION_SIZE_fsbl1    = "256"
FLASHLAYOUT_PARTITION_SIZE_nand-4-256_fsbl1 = "2048"
FLASHLAYOUT_PARTITION_SIZE_fsbl2    = "256"
FLASHLAYOUT_PARTITION_SIZE_ssbl     = "2048"
FLASHLAYOUT_PARTITION_SIZE_ssbl2    = "2048"
FLASHLAYOUT_PARTITION_SIZE_env      = "256"
FLASHLAYOUT_PARTITION_SIZE_teeh     = "256"
FLASHLAYOUT_PARTITION_SIZE_teed     = "256"
FLASHLAYOUT_PARTITION_SIZE_teex     = "256"
# Specific override for partition size as the configuration should follow
# the U-Boot source code where these partition sizes are hard coded
FLASHLAYOUT_PARTITION_SIZE_nand-4-256_teeh  = "512"
FLASHLAYOUT_PARTITION_SIZE_nand-4-256_teed  = "512"
FLASHLAYOUT_PARTITION_SIZE_nand-4-256_teex  = "512"
FLASHLAYOUT_PARTITION_SIZE_empty    = "0"
FLASHLAYOUT_PARTITION_SIZE_emptyfsbl1   = "${FLASHLAYOUT_PARTITION_SIZE_nand-4-256_fsbl1}"
FLASHLAYOUT_PARTITION_SIZE_emptyssbl    = "${FLASHLAYOUT_PARTITION_SIZE_ssbl}"
FLASHLAYOUT_PARTITION_SIZE_emptyssbl2   = "${FLASHLAYOUT_PARTITION_SIZE_ssbl2}"
FLASHLAYOUT_PARTITION_SIZE_emptyteeh    = "${FLASHLAYOUT_PARTITION_SIZE_nand-4-256_teeh}"
FLASHLAYOUT_PARTITION_SIZE_emptyteed    = "${FLASHLAYOUT_PARTITION_SIZE_nand-4-256_teed}"
FLASHLAYOUT_PARTITION_SIZE_emptyteex    = "${FLASHLAYOUT_PARTITION_SIZE_nand-4-256_teex}"
FLASHLAYOUT_PARTITION_SIZE_bootfs   = "${BOOTFS_PARTITION_SIZE}"
FLASHLAYOUT_PARTITION_SIZE_vendorfs = "${VENDORFS_PARTITION_SIZE}"
FLASHLAYOUT_PARTITION_SIZE_rootfs   = "${IMAGE_ROOTFS_MAXSIZE}"
FLASHLAYOUT_PARTITION_SIZE_ubifs    = "none"
FLASHLAYOUT_PARTITION_SIZE_userfs   = "${USERFS_PARTITION_SIZE}"

# Set binaries to use for each partition
FLASHLAYOUT_PARTITION_BIN2LOAD_fsbl1-boot   = "tf-a.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD_ssbl-boot    = "u-boot.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD_fsbl1        = "tf-a.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD_basic_fsbl1  = "u-boot-spl.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD_fsbl2        = "tf-a.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD_basic_fsbl2  = "u-boot-spl.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD_ssbl         = "u-boot.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD_basic_ssbl   = "u-boot.img"
FLASHLAYOUT_PARTITION_BIN2LOAD_ssbl2        = "u-boot.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD_teeh         = "tee-header_v2.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD_teed         = "tee-pageable_v2.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD_teex         = "tee-pager_v2.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD_env          = "none"
FLASHLAYOUT_PARTITION_BIN2LOAD_ubifs        = "${IMAGE_LINK_NAME}_nand_4_256_multivolume.ubi"
FLASHLAYOUT_PARTITION_BIN2LOAD_bootfs       = "${STM32MP_BOOTFS_IMAGE}-${DISTRO}-${MACHINE}.ext4"
FLASHLAYOUT_PARTITION_BIN2LOAD_vendorfs     = "${STM32MP_VENDORFS_IMAGE}-${DISTRO}-${MACHINE}.ext4"
FLASHLAYOUT_PARTITION_BIN2LOAD_rootfs       = "${IMAGE_LINK_NAME}.ext4"
FLASHLAYOUT_PARTITION_BIN2LOAD_userfs       = "${STM32MP_USERFS_IMAGE}-${DISTRO}-${MACHINE}.ext4"

# -----------------------------------------------------------------------------
# Use the 'BIN2BOOT_REPLACE_PATTERNS' var to allow dynamic binary renaming for
# the bootloader binaries. This is only required for fsbl1-boot and ssbl-boot
# partitions that provides the binary to flash the device.
# The format to follow is:
#   '<PATTERN2REPLACE_1>;<PATTERN2SET_1> <PATTERN2REPLACE_2>;<PATTERN2SET_2>'
# And the pattern to replace in binary name is only searched as:
#   '-<PATTERN>$'
#   or
#   '-<PATTERN>-'
# -----------------------------------------------------------------------------
# The 'basic' bootscheme does not support Programmer mode, so use 'trusted' one
# (valid for both fsbl1-boot and ssbl-boot)
BIN2BOOT_REPLACE_PATTERNS_DEFAULT = "basic;trusted"
# The daughter board does not support Programmer mode, so use eval one
# (valid for both fsbl1-boot and ssbl-boot)
BIN2BOOT_REPLACE_PATTERNS_DEFAULT_append = " ed1;ev1"

# Apply for fsbl1-boot and ssbl-boot
BIN2BOOT_REPLACE_PATTERNS_fsbl1-boot = "${BIN2BOOT_REPLACE_PATTERNS_DEFAULT}"
BIN2BOOT_REPLACE_PATTERNS_ssbl-boot  = "${BIN2BOOT_REPLACE_PATTERNS_DEFAULT}"

# For fsbl1-boot, the 'optee' bootscheme does not support Programmer mode, so
# prefer the 'trusted' one.
BIN2BOOT_REPLACE_PATTERNS_fsbl1-boot_append = " optee;trusted"
