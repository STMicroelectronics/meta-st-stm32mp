#@DESCRIPTION: STM32MP machine flashlayout configuration

inherit flashlayout-stm32mp

# Add specific dependencies to get all binaries generated before flashlayout files
FLASHLAYOUT_DEPEND_TASKS += "${@bb.utils.contains('EXTRA_IMAGEDEPENDS', 'virtual/trusted-firmware-a', 'virtual/trusted-firmware-a:do_deploy', '', d)}"
FLASHLAYOUT_DEPEND_TASKS += "${@bb.utils.contains('EXTRA_IMAGEDEPENDS', 'virtual/trusted-firmware-a-serialboot', 'virtual/trusted-firmware-a-serialboot:do_deploy', '', d)}"
FLASHLAYOUT_DEPEND_TASKS += "${@bb.utils.contains('EXTRA_IMAGEDEPENDS', 'virtual/bootloader', 'virtual/bootloader:do_deploy', '', d)}"
FLASHLAYOUT_DEPEND_TASKS += "${@bb.utils.contains('EXTRA_IMAGEDEPENDS', 'virtual/optee-os', 'virtual/optee-os:do_deploy', '', d)}"

# -----------------------------------------------------------------------------
# For the stm32mp flashlayout files, configuration is done to get output file
# with following name scheme:
#   FlashLayout_<CONFIG_LABEL>[_<TYPE_LABEL>-<BOOTSCHEME_LABEL>].tsv
#
# The different labels are stored as list in different vars:
#   - <BOOTSCHEME_LABEL>    FLASHLAYOUT_BOOTSCHEME_LABELS
#   - <CONFIG_LABEL>        FLASHLAYOUT_CONFIG_LABELS
#   - <TYPE_LABEL>          FLASHLAYOUT_TYPE_LABELS
#
# The flashlayout file name is generated with following loop logic:
#   for bootscheme in FLASHLAYOUT_BOOTSCHEME_LABELS     (expanded according to ongoing OVERRIDES)
#       for config in FLASHLAYOUT_CONFIG_LABELS         (expanded according to ongoing $bootscheme and OVERRIDES)
#           for typelabel in FLASHLAYOUT_TYPE_LABELS    (expanded according to ongoing $bootscheme, $config and OVERRIDES)
#               if FLASHLAYOUT_BOOTSCHEME_LABELS items > 1 && FLASHLAYOUT_TYPE_LABELS items > 1
#                   flashlayout_name = FlashLayout_$config_$typelabel-$bootscheme.tsv
#               else
#                   flashlayout_name = FlashLayout_$config.tsv
#
# Each flashlayout file should contain lines that set the partition properties:
#   <Opt> <Id> <Name> <Type> <IP> <Offset> <Binary>
#
# The partition properties are set with 'FLASHLAYOUT_PARTITION_xxx' vars:
#   - <Opt>     FLASHLAYOUT_PARTITION_ENABLE
#   - <Id>      FLASHLAYOUT_PARTITION_ID
#   - <Name>    Item from FLASHLAYOUT_PARTITION_LABELS list
#   - <Type>    FLASHLAYOUT_PARTITION_TYPE
#   - <IP>      FLASHLAYOUT_PARTITION_DEVICE
#   - <Offset>  FLASHLAYOUT_PARTITION_OFFSET
#   - <Binary>  FLASHLAYOUT_PARTITION_BIN2LOAD
#
# For the '$flashlayout_name' flashlayout file, the partition lines are generated
# with following loop logic:
#   for partition in FLASHLAYOUT_PARTITION_LABELS       (expanded according to ongoing $bootscheme, $config and OVERRIDES)
#       OVERRIDES_PREV = OVERRIDES
#       OVERRIDES = $partition:OVERRIDES
#       Compute partition properties for $partiton      (expanded according to ongoing $bootscheme, $config and OVERRIDES)
#       Write new partition line to $flashlayout_name
#       OVERRIDES = OVERRIDES_PREV
#
# Modifying the OVERRIDES inside the loop allows dynamic configuration for each
# partition property. This leaves plenty of possibility and guarantee specific
# '$partition' override.
# -----------------------------------------------------------------------------

# -----------------------------------------------------------------------------
# Define bootscheme labels
# -----------------------------------------------------------------------------
FLASHLAYOUT_BOOTSCHEME_LABELS += "${BOOTSCHEME_LABELS}"

# -----------------------------------------------------------------------------
# Define config labels
# -----------------------------------------------------------------------------
# Priority var assignment (where <OVERRIDES> are the usual override mechanism):
#   1) FLASHLAYOUT_CONFIG_LABELS_<BOOTSCHEME>_<OVERRIDES>
#   2) FLASHLAYOUT_CONFIG_LABELS_<BOOTSCHEME>
#   3) FLASHLAYOUT_CONFIG_LABELS_<OVERRIDES>
#   4) FLASHLAYOUT_CONFIG_LABELS
# -----------------------------------------------------------------------------
FLASHLAYOUT_CONFIG_LABELS += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'emmc', '', d)}"
FLASHLAYOUT_CONFIG_LABELS += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-4-256', 'nand-4-256', '', d)}"
FLASHLAYOUT_CONFIG_LABELS += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nor-sdcard', 'nor-sdcard', '', d)}"
FLASHLAYOUT_CONFIG_LABELS += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', 'sdcard', '', d)}"

# Set by default this variable to 0, and set to 1 only when we are using st-example-image-*
ST_EXAMPLE_IMAGE ??= "0"

#FIXME need to manage overall device size to abort flashlayout creation in case of oversizing the storage devices

# Remove NAND flashlayout when we are using st-example-image-* as rootfs too big for a NAND device size of 1 GBytes
#FLASHLAYOUT_CONFIG_LABELS_remove = "${@bb.utils.contains('ST_EXAMPLE_IMAGE', '1', 'emmc', '', d)}"
FLASHLAYOUT_CONFIG_LABELS_remove = "${@bb.utils.contains('ST_EXAMPLE_IMAGE', '1', 'nand-4-256', '', d)}"
#FLASHLAYOUT_CONFIG_LABELS_remove = "${@bb.utils.contains('ST_EXAMPLE_IMAGE', '1', 'nor-sdcard', '', d)}"
#FLASHLAYOUT_CONFIG_LABELS_remove = "${@bb.utils.contains('ST_EXAMPLE_IMAGE', '1', 'sdcard', '', d)}"

# -----------------------------------------------------------------------------
# Define label types for each config
# -----------------------------------------------------------------------------
# NOTE: We use devicetree to allow proper filtering on supported storage device
# -----------------------------------------------------------------------------
# Priority var assignment (where <OVERRIDES> are the usual override mechanism):
#   1) FLASHLAYOUT_TYPE_LABELS_<BOOTSCHEME>_<CONFIG>_<OVERRIDES>
#   2) FLASHLAYOUT_TYPE_LABELS_<BOOTSCHEME>_<CONFIG>
#   3) FLASHLAYOUT_TYPE_LABELS_<BOOTSCHEME>_<OVERRIDES>
#   4) FLASHLAYOUT_TYPE_LABELS_<BOOTSCHEME>
#   5) FLASHLAYOUT_TYPE_LABELS_<CONFIG>_<OVERRIDES>
#   6) FLASHLAYOUT_TYPE_LABELS_<CONFIG>
#   7) FLASHLAYOUT_TYPE_LABELS_<OVERRIDES>
#   8) FLASHLAYOUT_TYPE_LABELS
# -----------------------------------------------------------------------------
# EMMC
# Set flashlayout file generation to eval board (mother and daughter) only
FLASHLAYOUT_TYPE_LABELS_emmc = "${STM32MP_DT_FILES_ED} ${STM32MP_DT_FILES_EV}"
# NAND
# Set flashlayout file generation to eval board only
FLASHLAYOUT_TYPE_LABELS_nand-4-256 = "${STM32MP_DT_FILES_EV}"
# NOR
# Set flashlayout file generation to eval board only
FLASHLAYOUT_TYPE_LABELS_nor-sdcard = "${STM32MP_DT_FILES_EV}"
# SDCARD
# Set flashlayout file generation for all boards
FLASHLAYOUT_TYPE_LABELS_sdcard = "${STM32MP_DT_FILES_DK} ${STM32MP_DT_FILES_ED} ${STM32MP_DT_FILES_EV}"

# -----------------------------------------------------------------------------
# Define partitions to consider for flashlayout file generation
# -----------------------------------------------------------------------------
# NOTE: There are few restrictions to follow:
#       -   The partition for the first boot loader should follow the naming
#            rule: fsbl*
#       -   The partition for the secondary boot loader should follow the naming
#           rule: ssbl
# -----------------------------------------------------------------------------
# Priority var assignment (where <OVERRIDES> are the usual override mechanism):
#   1) FLASHLAYOUT_PARTITION_LABELS_<BOOTSCHEME>_<CONFIG>_<OVERRIDES>
#   2) FLASHLAYOUT_PARTITION_LABELS_<BOOTSCHEME>_<CONFIG>
#   3) FLASHLAYOUT_PARTITION_LABELS_<BOOTSCHEME>_<OVERRIDES>
#   4) FLASHLAYOUT_PARTITION_LABELS_<BOOTSCHEME>
#   5) FLASHLAYOUT_PARTITION_LABELS_<CONFIG>_<OVERRIDES>
#   6) FLASHLAYOUT_PARTITION_LABELS_<CONFIG>
#   7) FLASHLAYOUT_PARTITION_LABELS_<OVERRIDES>
#   8) FLASHLAYOUT_PARTITION_LABELS
# -----------------------------------------------------------------------------

# FLASHLAYOUT_PARTITION_IMAGES is initalized through PARTITIONS_CONFIG within 'flashlayout-stm32mp' class
FLASHLAYOUT_PARTITION_IMAGES ?= ""

FLASHLAYOUT_PARTITION_LABELS_optee_emmc         = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl teeh teed teex ${FLASHLAYOUT_PARTITION_IMAGES}"
FLASHLAYOUT_PARTITION_LABELS_optee_nand-4-256   = "fsbl1-boot ssbl-boot fsbl1 ssbl ssbl2 teeh teed teex ubifs"
FLASHLAYOUT_PARTITION_LABELS_optee_nor-sdcard   = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl env teeh teed teex empty ${FLASHLAYOUT_PARTITION_IMAGES}"
FLASHLAYOUT_PARTITION_LABELS_optee_sdcard       = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl teeh teed teex ${FLASHLAYOUT_PARTITION_IMAGES}"

FLASHLAYOUT_PARTITION_LABELS_trusted_emmc       = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl ${FLASHLAYOUT_PARTITION_IMAGES}"
FLASHLAYOUT_PARTITION_LABELS_trusted_nand-4-256 = "fsbl1-boot ssbl-boot fsbl1 ssbl ssbl2 ubifs"
FLASHLAYOUT_PARTITION_LABELS_trusted_nor-sdcard = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl env empty ${FLASHLAYOUT_PARTITION_IMAGES}"
FLASHLAYOUT_PARTITION_LABELS_trusted_sdcard     = "fsbl1-boot ssbl-boot fsbl1 fsbl2 ssbl ${FLASHLAYOUT_PARTITION_IMAGES}"

# -----------------------------------------------------------------------------
# Partition properties configuration
# -----------------------------------------------------------------------------
# Priority var assignment (where <OVERRIDES> are the usual override mechanism
# with dynamic append of ongoing 'partition name' item):
#           1) FLASHLAYOUT_PARTITION_xxx_<BOOTSCHEME>_<CONFIG>_<OVERRIDES>
#           2) FLASHLAYOUT_PARTITION_xxx_<BOOTSCHEME>_<CONFIG>
#           3) FLASHLAYOUT_PARTITION_xxx_<BOOTSCHEME>_<OVERRIDES>
#           4) FLASHLAYOUT_PARTITION_xxx_<BOOTSCHEME>
#           5) FLASHLAYOUT_PARTITION_xxx_<CONFIG>_<OVERRIDES>
#           6) FLASHLAYOUT_PARTITION_xxx_<CONFIG>
#           7) FLASHLAYOUT_PARTITION_xxx_<OVERRIDES>
#           8) FLASHLAYOUT_PARTITION_xxx
#           9) Default 'FLASHLAYOUT_PARTITION_xxx' to 'none' when not defined
# -----------------------------------------------------------------------------
FLASHLAYOUT_PARTITION_ENABLE            = "P"
FLASHLAYOUT_PARTITION_ENABLE_fsbl1-boot = "-"
FLASHLAYOUT_PARTITION_ENABLE_ssbl-boot  = "-"
FLASHLAYOUT_PARTITION_ENABLE_empty      = "PE"
FLASHLAYOUT_PARTITION_ENABLE_env        = "PED"
# Due to association of u-boot and env, the ssbl partition need to be deleted
FLASHLAYOUT_PARTITION_ENABLE_sdcard_ssbl = "PD"
FLASHLAYOUT_PARTITION_ENABLE_emmc_ssbl   = "PD"

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
FLASHLAYOUT_PARTITION_ID_fsbl1-boot = "0x01"
FLASHLAYOUT_PARTITION_ID_ssbl-boot  = "0x03"
FLASHLAYOUT_PARTITION_ID_fsbl1      = "0x04"
FLASHLAYOUT_PARTITION_ID_fsbl2      = "0x05"
FLASHLAYOUT_PARTITION_ID_ssbl       = "0x06"
FLASHLAYOUT_PARTITION_ID_ssbl2      = "0x07"
FLASHLAYOUT_PARTITION_ID_teeh       = "0x0A"
FLASHLAYOUT_PARTITION_ID_teed       = "0x0B"
FLASHLAYOUT_PARTITION_ID_teex       = "0x0C"
FLASHLAYOUT_PARTITION_ID_empty      = "0x10"
FLASHLAYOUT_PARTITION_ID_env        = "0x20"
FLASHLAYOUT_PARTITION_ID_ubifs      = "0x21"

FLASHLAYOUT_PARTITION_TYPE                  = "Binary"
FLASHLAYOUT_PARTITION_TYPE_nand-4-256_fsbl1 = "Binary(2)"
FLASHLAYOUT_PARTITION_TYPE_ubifs            = "System"

FLASHLAYOUT_PARTITION_DEVICE_emmc       = "none:fsbl1-boot ssbl-boot,${DEVICE_EMMC}:default"
FLASHLAYOUT_PARTITION_DEVICE_nand-4-256 = "none:fsbl1-boot ssbl-boot,${DEVICE_NAND}:default"
FLASHLAYOUT_PARTITION_DEVICE_nor-sdcard = "none:fsbl1-boot ssbl-boot,${DEVICE_NOR}:default,${DEVICE_SDCARD}:${FLASHLAYOUT_PARTITION_IMAGES}"
FLASHLAYOUT_PARTITION_DEVICE_sdcard     = "none:fsbl1-boot ssbl-boot,${DEVICE_SDCARD}:default"

FLASHLAYOUT_PARTITION_OFFSET_fsbl1-boot = "0x0"
FLASHLAYOUT_PARTITION_OFFSET_ssbl-boot  = "0x0"
FLASHLAYOUT_PARTITION_OFFSET_emmc_fsbl1 = "${DEVICE_START_OFFSET_BOOT0_EMMC}"
FLASHLAYOUT_PARTITION_OFFSET_emmc_fsbl2 = "${DEVICE_START_OFFSET_BOOT1_EMMC}"
FLASHLAYOUT_PARTITION_OFFSET_emmc_ssbl  = "${DEVICE_START_OFFSET_EMMC}"

# Size defined in Kbytes
FLASHLAYOUT_PARTITION_SIZE_fsbl1    = "256"
FLASHLAYOUT_PARTITION_SIZE_fsbl2    = "256"
FLASHLAYOUT_PARTITION_SIZE_ssbl     = "2048"
FLASHLAYOUT_PARTITION_SIZE_ssbl2    = "2048"
FLASHLAYOUT_PARTITION_SIZE_env      = "512"
FLASHLAYOUT_PARTITION_SIZE_teeh     = "256"
FLASHLAYOUT_PARTITION_SIZE_teed     = "512"
FLASHLAYOUT_PARTITION_SIZE_teex     = "256"
FLASHLAYOUT_PARTITION_SIZE_empty    = "0"
# Specific override for MTD partitions hard coded on U-Boot side
FLASHLAYOUT_PARTITION_SIZE_nand-4-256_fsbl1 = "2048"
FLASHLAYOUT_PARTITION_SIZE_nand-4-256_ssbl  = "2048"
FLASHLAYOUT_PARTITION_SIZE_nand-4-256_ssbl2 = "2048"
FLASHLAYOUT_PARTITION_SIZE_nand-4-256_teeh  = "512"
FLASHLAYOUT_PARTITION_SIZE_nand-4-256_teed  = "512"
FLASHLAYOUT_PARTITION_SIZE_nand-4-256_teex  = "512"
FLASHLAYOUT_PARTITION_SIZE_nor-sdcard_fsbl1 = "256"
FLASHLAYOUT_PARTITION_SIZE_nor-sdcard_fsbl2 = "256"
FLASHLAYOUT_PARTITION_SIZE_nor-sdcard_ssbl  = "2048"
FLASHLAYOUT_PARTITION_SIZE_nor-sdcard_env   = "512"
FLASHLAYOUT_PARTITION_SIZE_nor-sdcard_teeh  = "256"
FLASHLAYOUT_PARTITION_SIZE_nor-sdcard_teed  = "512"
FLASHLAYOUT_PARTITION_SIZE_nor-sdcard_teex  = "256"

# Set binaries to use for each partition
FLASHLAYOUT_PARTITION_BIN2LOAD_fsbl1-boot   = "arm-trusted-firmware/tf-a.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD_ssbl-boot    = "bootloader/u-boot.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD_fsbl1        = "arm-trusted-firmware/tf-a.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD_fsbl2        = "arm-trusted-firmware/tf-a.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD_ssbl         = "bootloader/u-boot.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD_ssbl2        = "bootloader/u-boot.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD_teeh         = "optee/tee-header_v2.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD_teed         = "optee/tee-pageable_v2.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD_teex         = "optee/tee-pager_v2.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD_ubifs        = "${IMAGE_LINK_NAME}_nand_4_256_multivolume.ubi"

# -----------------------------------------------------------------------------
# Use the 'FLASHLAYOUT_PARTITION_REPLACE_PATTERNS' var to allow dynamic binary
# renaming for the bootloader binaries. This is only required for fsbl1-boot and
# ssbl-boot partitions that provides the binary to flash the device.
# The format to follow is:
#   '<PATTERN2REPLACE_1>;<PATTERN2SET_1> <PATTERN2REPLACE_2>;<PATTERN2SET_2>'
# And the pattern to replace in binary name is only searched as:
#   '-<PATTERN>$'
#   or
#   '-<PATTERN>-'
# -----------------------------------------------------------------------------
# We use specific tf-a serialboot mode for any bootscheme for fsbl1-boot
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS_fsbl1-boot_append = " optee;serialboot trusted;serialboot"
# For u-boot the 'optee' bootscheme is the same than trusted so use 'trusted'
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS_ssbl-boot_append = " optee;trusted"
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS_ssbl_append = " optee;trusted"
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS_ssbl2_append = " optee;trusted"

# The daughter board does not support Programmer mode, so use eval one
# (valid for both fsbl1-boot and ssbl-boot)
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS_fsbl1-boot_append = " ed1;ev1"
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS_ssbl-boot_append = " ed1;ev1"
