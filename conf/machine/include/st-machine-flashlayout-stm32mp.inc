#@DESCRIPTION: STM32MP machine flashlayout configuration

inherit flashlayout-stm32mp

# Add specific dependencies to get all binaries generated before flashlayout files
FLASHLAYOUT_DEPEND_TASKS += "${@bb.utils.contains('EXTRA_IMAGEDEPENDS', 'fip-stm32mp', 'fip-stm32mp:do_deploy', '', d)}"
FLASHLAYOUT_DEPEND_TASKS += "${@bb.utils.contains('EXTRA_IMAGEDEPENDS', 'virtual/trusted-firmware-a', 'virtual/trusted-firmware-a:do_deploy', '', d)}"
FLASHLAYOUT_DEPEND_TASKS += "${@bb.utils.contains('EXTRA_IMAGEDEPENDS', 'virtual/bootloader', 'virtual/bootloader:do_deploy', '', d)}"
FLASHLAYOUT_DEPEND_TASKS += "${@bb.utils.contains('EXTRA_IMAGEDEPENDS', 'virtual-optee-os', 'virtual-optee-os:do_deploy', '', d)}"
FLASHLAYOUT_DEPEND_TASKS += "${@bb.utils.contains('EXTRA_IMAGEDEPENDS', 'virtual/trusted-firmware-m', 'virtual/trusted-firmware-m:do_deploy', '', d)}"

# for aarch64 support on aarch32 machine
FLASHLAYOUT_DEPEND_TASKS += "${@bb.utils.contains('EXTRA_IMAGEDEPENDS', 'lib64-fip-stm32mp', 'lib64-fip-stm32mp:do_deploy', '', d)}"
FLASHLAYOUT_DEPEND_TASKS += "${@bb.utils.contains('EXTRA_IMAGEDEPENDS', 'virtual/lib64-trusted-firmware-a', 'virtual/lib64-trusted-firmware-a:do_deploy', '', d)}"
FLASHLAYOUT_DEPEND_TASKS += "${@bb.utils.contains('EXTRA_IMAGEDEPENDS', 'virtual/lib64-bootloader', 'virtual/lib64-bootloader:do_deploy', '', d)}"
FLASHLAYOUT_DEPEND_TASKS += "${@bb.utils.contains('EXTRA_IMAGEDEPENDS', 'lib64-virtual-optee-os', 'lib64-virtual-optee-os:do_deploy', '', d)}"
FLASHLAYOUT_DEPEND_TASKS += "${@bb.utils.contains('EXTRA_IMAGEDEPENDS', 'virtual/lib64-trusted-firmware-m', 'virtual/lib64-trusted-firmware-m:do_deploy', '', d)}"

FLASHLAYOUT_SUFFIX = "${@bb.utils.contains('ENCRYPT_ENABLE', '1', '${ENCRYPT_SUFFIX}', '', d)}${@bb.utils.contains('SIGN_ENABLE', '1', '${SIGN_SUFFIX}', '', d)}.tsv"

# List the PARTITION_TYPE used for bootloader partition
FLASHLAYOUT_BINARY_TYPES += "Binary"
FLASHLAYOUT_BINARY_TYPES += "ENV"
FLASHLAYOUT_BINARY_TYPES += "FIP"
FLASHLAYOUT_BINARY_TYPES += "FWU_MDATA"

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
#   - <Type>    FLASHLAYOUT_PARTITION_TYPE with optional FLASHLAYOUT_PARTITION_COPY (in case greater than 1)
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
#   1) FLASHLAYOUT_CONFIG_LABELS:<BOOTSCHEME>:<OVERRIDES>
#   2) FLASHLAYOUT_CONFIG_LABELS:<BOOTSCHEME>
#   3) FLASHLAYOUT_CONFIG_LABELS:<OVERRIDES>
#   4) FLASHLAYOUT_CONFIG_LABELS
# -----------------------------------------------------------------------------
FLASHLAYOUT_CONFIG_LABELS += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'emmc', '', d)}"
FLASHLAYOUT_CONFIG_LABELS += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-2-128-256', 'nand-2-128-256', '', d)}"
FLASHLAYOUT_CONFIG_LABELS += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-2-128-256-sdcard', 'nand-2-128-256-sdcard', '', d)}"
FLASHLAYOUT_CONFIG_LABELS += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-4-256-512', 'nand-4-256-512', '', d)}"
FLASHLAYOUT_CONFIG_LABELS += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-4-256-512-sdcard', 'nand-4-256-512-sdcard', '', d)}"
FLASHLAYOUT_CONFIG_LABELS += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-4-256-1024', 'nand-4-256-1024', '', d)}"
FLASHLAYOUT_CONFIG_LABELS += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-4-256-1024-sdcard', 'nand-4-256-1024-sdcard', '', d)}"
FLASHLAYOUT_CONFIG_LABELS += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nor', 'nor', '', d)}"
FLASHLAYOUT_CONFIG_LABELS += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nor-emmc', 'nor-emmc', '', d)}"
FLASHLAYOUT_CONFIG_LABELS += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nor-nor-emmc', 'nor-nor-emmc', '', d)}"
FLASHLAYOUT_CONFIG_LABELS += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nor-nor-sdcard', 'nor-nor-sdcard', '', d)}"
FLASHLAYOUT_CONFIG_LABELS += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nor-sdcard', 'nor-sdcard', '', d)}"
FLASHLAYOUT_CONFIG_LABELS += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', bb.utils.contains('MACHINE_FEATURES', 'efi', 'sdcard sdcard-EFI', 'sdcard', d), '', d)}"

# Override for fastboot
FLASHLAYOUT_CONFIG_LABELS:fastboot += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'emmc', '', d)}"
FLASHLAYOUT_CONFIG_LABELS:fastboot += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', bb.utils.contains('MACHINE_FEATURES', 'efi', 'sdcard sdcard-EFI', 'sdcard', d), '', d)}"

FLASHLAYOUT_CONFIG_LABELS:fastboot-opteemin += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'emmc', '', d)}"
FLASHLAYOUT_CONFIG_LABELS:fastboot-opteemin += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', bb.utils.contains('MACHINE_FEATURES', 'efi', 'sdcard sdcard-EFI', 'sdcard', d), '', d)}"

# Set by default this variable to 0, and set to 1 only when we are using st-example-image-*
ST_EXAMPLE_IMAGE ??= "0"

# Remove NAND flashlayout when we are using st-example-image-* as rootfs too big for a NAND device size of 1 GBytes
FLASHLAYOUT_CONFIG_LABELS:remove = "${@bb.utils.contains('ST_EXAMPLE_IMAGE', '1', 'nand-4-256-1024', '', d)}"
FLASHLAYOUT_CONFIG_LABELS:remove = "${@bb.utils.contains('ST_EXAMPLE_IMAGE', '1', 'nand-2-128-256', '', d)}"
FLASHLAYOUT_CONFIG_LABELS:remove = "${@bb.utils.contains('ST_EXAMPLE_IMAGE', '1', 'nand-4-256-512', '', d)}"

FLASHLAYOUT_PARTITION_EMPTY_CONFIG = "none,empty,0,Binary"

FLASHLAYOUT_CONFIG_LABELS[emmc] = "\
    ${PARTITIONS_PROGRAMMER_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'emmc-m33td').split()]) if bb.utils.contains('MACHINE_FEATURES', 'm33td', True, False, d) else ''} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'emmc').split()])} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_CONFIG', 'emmc').split()])} \
    "
FLASHLAYOUT_CONFIG_LABELS[nand-2-128-256] = "\
    ${PARTITIONS_PROGRAMMER_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'nand-2-128-256').split()])} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_CONFIG', 'nand-2-128-256').split()])} \
    "
FLASHLAYOUT_CONFIG_LABELS[nand-4-256-512] = "\
    ${PARTITIONS_PROGRAMMER_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'nand-4-256-512').split()])} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_CONFIG', 'nand-4-256-512').split()])} \
    "
FLASHLAYOUT_CONFIG_LABELS[nand-4-256-1024] = "\
    ${PARTITIONS_PROGRAMMER_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'nand-4-256-1024').split()])} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_CONFIG', 'nand-4-256-1024').split()])} \
    "
FLASHLAYOUT_CONFIG_LABELS[nor] = "\
    ${PARTITIONS_PROGRAMMER_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'nor').split()])} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_CONFIG', 'nor').split()])} \
    "
FLASHLAYOUT_CONFIG_LABELS[nor-emmc] = "\
    ${PARTITIONS_PROGRAMMER_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', bb.utils.contains('MACHINE_FEATURES', 'm33td', 'nor-m33td', 'nor', d)).split()])} \
    ${FLASHLAYOUT_PARTITION_EMPTY_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'emmc').split()]) if bb.utils.contains('MACHINE_FEATURES', 'm33td', True, False, d) else ''} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_CONFIG', 'emmc').split()])} \
    "
FLASHLAYOUT_CONFIG_LABELS[nor-sdcard] = "\
    ${PARTITIONS_PROGRAMMER_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', bb.utils.contains('MACHINE_FEATURES', 'm33td', 'nor-m33td', 'nor', d)).split()])} \
    ${FLASHLAYOUT_PARTITION_EMPTY_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'sdcard').split()]) if bb.utils.contains('MACHINE_FEATURES', 'm33td', True, False, d) else ''} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_CONFIG', 'sdcard').split()])} \
    "
FLASHLAYOUT_CONFIG_LABELS[sdcard] = "\
    ${PARTITIONS_PROGRAMMER_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'sdcard-m33td').split()]) if bb.utils.contains('MACHINE_FEATURES', 'm33td', True, False, d) else ''} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'sdcard').split()])} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_CONFIG', 'sdcard').split()])} \
    "
FLASHLAYOUT_CONFIG_LABELS[sdcard-EFI] = "\
    ${PARTITIONS_PROGRAMMER_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'sdcard-EFI').split()])} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_CONFIG', 'sdcard-EFI').split()])} \
    "
# nand + sdcard
FLASHLAYOUT_CONFIG_LABELS[nand-2-128-256-sdcard] = "\
    ${PARTITIONS_PROGRAMMER_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'nand-2-128-256').split()])} \
    ${PARTITION_UBOOT_ENV_UBI_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_CONFIG', 'sdcard').split()])} \
    "
FLASHLAYOUT_CONFIG_LABELS[nand-4-256-512-sdcard] = "\
    ${PARTITIONS_PROGRAMMER_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'nand-4-256-512').split()])} \
    ${PARTITION_UBOOT_ENV_UBI_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_CONFIG', 'sdcard').split()])} \
    "
FLASHLAYOUT_CONFIG_LABELS[nand-4-256-1024-sdcard] = "\
    ${PARTITIONS_PROGRAMMER_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'nand-4-256-1024').split()])} \
    ${PARTITION_UBOOT_ENV_UBI_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_CONFIG', 'sdcard').split()])} \
    "
# nor with both m33 and a35 bootloaders
FLASHLAYOUT_CONFIG_LABELS[nor-nor-sdcard] = "\
    ${PARTITIONS_PROGRAMMER_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'nor').split()])} \
    ${FLASHLAYOUT_PARTITION_EMPTY_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_CONFIG', 'sdcard').split()])} \
    "
FLASHLAYOUT_CONFIG_LABELS[nor-nor-emmc] = "\
    ${PARTITIONS_PROGRAMMER_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'nor').split()])} \
    ${FLASHLAYOUT_PARTITION_EMPTY_CONFIG} \
    ${@' '.join(['%s' % l for l in d.getVarFlag('PARTITIONS_CONFIG', 'emmc').split()])} \
    "

# -----------------------------------------------------------------------------
# Define label types for each config
# -----------------------------------------------------------------------------
# NOTE: We use devicetree to allow proper filtering on supported storage device
# -----------------------------------------------------------------------------
# Priority var assignment (where <OVERRIDES> are the usual override mechanism):
#   1) FLASHLAYOUT_TYPE_LABELS:<BOOTSCHEME>:<CONFIG>:<OVERRIDES>
#   2) FLASHLAYOUT_TYPE_LABELS:<BOOTSCHEME>:<CONFIG>
#   3) FLASHLAYOUT_TYPE_LABELS:<BOOTSCHEME>:<OVERRIDES>
#   4) FLASHLAYOUT_TYPE_LABELS:<BOOTSCHEME>
#   5) FLASHLAYOUT_TYPE_LABELS:<CONFIG>:<OVERRIDES>
#   6) FLASHLAYOUT_TYPE_LABELS:<CONFIG>
#   7) FLASHLAYOUT_TYPE_LABELS:<OVERRIDES>
#   8) FLASHLAYOUT_TYPE_LABELS
# -----------------------------------------------------------------------------
# EMMC
FLASHLAYOUT_TYPE_LABELS:emmc = "${STM32MP_DT_FILES_EMMC}"
# NAND
FLASHLAYOUT_TYPE_LABELS:nand-4-256-1024 = "${STM32MP_DT_FILES_NAND}"
FLASHLAYOUT_TYPE_LABELS:nand-4-256-1024-sdcard = "${STM32MP_DT_FILES_NAND}"
# NOR
FLASHLAYOUT_TYPE_LABELS:nor = "${STM32MP_DT_FILES_NOR_all}"
FLASHLAYOUT_TYPE_LABELS:nor-emmc = "${@' '.join(dt for dt in '${STM32MP_DT_FILES_NOR_mb}'.split() if dt in '${STM32MP_DT_FILES_EMMC}'.split())}"
FLASHLAYOUT_TYPE_LABELS:nor-sdcard = "${@' '.join(dt for dt in '${STM32MP_DT_FILES_NOR_mb}'.split() if dt in '${STM32MP_DT_FILES_SDCARD}'.split())}"
FLASHLAYOUT_TYPE_LABELS:nor-nor-emmc = "${@' '.join(dt for dt in '${STM32MP_DT_FILES_NOR_mb_sb}'.split() if dt in '${STM32MP_DT_FILES_EMMC}'.split())}"
FLASHLAYOUT_TYPE_LABELS:nor-nor-sdcard = "${@' '.join(dt for dt in '${STM32MP_DT_FILES_NOR_mb_sb}'.split() if dt in '${STM32MP_DT_FILES_SDCARD}'.split())}"
# SDCARD
FLASHLAYOUT_TYPE_LABELS:sdcard = "${STM32MP_DT_FILES_SDCARD}"
FLASHLAYOUT_TYPE_LABELS:sdcard-EFI = "${STM32MP_DT_FILES_SDCARD}"
# SPINAND
FLASHLAYOUT_TYPE_LABELS:nand-2-128-256 = "${STM32MP_DT_FILES_SPINAND}"
FLASHLAYOUT_TYPE_LABELS:nand-2-128-256-sdcard = "${STM32MP_DT_FILES_SPINAND}"
FLASHLAYOUT_TYPE_LABELS:nand-4-256-512 = "${STM32MP_DT_FILES_SPINAND}"
FLASHLAYOUT_TYPE_LABELS:nand-4-256-512-sdcard = "${STM32MP_DT_FILES_SPINAND}"

# -----------------------------------------------------------------------------
# Define partitions to consider for flashlayout file generation
# -----------------------------------------------------------------------------
# Priority var assignment (where <OVERRIDES> are the usual override mechanism):
#   1) FLASHLAYOUT_PARTITION_LABELS:<BOOTSCHEME>:<CONFIG>:<OVERRIDES>
#   2) FLASHLAYOUT_PARTITION_LABELS:<BOOTSCHEME>:<CONFIG>
#   3) FLASHLAYOUT_PARTITION_LABELS:<BOOTSCHEME>:<OVERRIDES>
#   4) FLASHLAYOUT_PARTITION_LABELS:<BOOTSCHEME>
#   5) FLASHLAYOUT_PARTITION_LABELS:<CONFIG>:<OVERRIDES>
#   6) FLASHLAYOUT_PARTITION_LABELS:<CONFIG>
#   7) FLASHLAYOUT_PARTITION_LABELS:<OVERRIDES>
#   8) FLASHLAYOUT_PARTITION_LABELS
# -----------------------------------------------------------------------------
FLASHLAYOUT_PARTITION_LABELS:emmc = "${@' '.join(['%s' % l.split(',')[1] for l in d.getVarFlag('FLASHLAYOUT_CONFIG_LABELS', 'emmc').split()])}"
FLASHLAYOUT_PARTITION_LABELS:nand-2-128-256 = "${@' '.join(['%s' % l.split(',')[1] for l in d.getVarFlag('FLASHLAYOUT_CONFIG_LABELS', 'nand-2-128-256').split()])}"
FLASHLAYOUT_PARTITION_LABELS:nand-2-128-256-sdcard = "${@' '.join(['%s' % l.split(',')[1] for l in d.getVarFlag('FLASHLAYOUT_CONFIG_LABELS', 'nand-2-128-256-sdcard').split()])}"
FLASHLAYOUT_PARTITION_LABELS:nand-4-256-512 = "${@' '.join(['%s' % l.split(',')[1] for l in d.getVarFlag('FLASHLAYOUT_CONFIG_LABELS', 'nand-4-256-512').split()])}"
FLASHLAYOUT_PARTITION_LABELS:nand-4-256-512-sdcard = "${@' '.join(['%s' % l.split(',')[1] for l in d.getVarFlag('FLASHLAYOUT_CONFIG_LABELS', 'nand-4-256-512-sdcard').split()])}"
FLASHLAYOUT_PARTITION_LABELS:nand-4-256-1024 = "${@' '.join(['%s' % l.split(',')[1] for l in d.getVarFlag('FLASHLAYOUT_CONFIG_LABELS', 'nand-4-256-1024').split()])}"
FLASHLAYOUT_PARTITION_LABELS:nand-4-256-1024-sdcard = "${@' '.join(['%s' % l.split(',')[1] for l in d.getVarFlag('FLASHLAYOUT_CONFIG_LABELS', 'nand-4-256-1024-sdcard').split()])}"
FLASHLAYOUT_PARTITION_LABELS:nor = "${@' '.join(['%s' % l.split(',')[1] for l in d.getVarFlag('FLASHLAYOUT_CONFIG_LABELS', 'nor').split()])}"
FLASHLAYOUT_PARTITION_LABELS:nor-emmc = "${@' '.join(['%s' % l.split(',')[1] for l in d.getVarFlag('FLASHLAYOUT_CONFIG_LABELS', 'nor-emmc').split()])}"
FLASHLAYOUT_PARTITION_LABELS:nor-nor-emmc = "${@' '.join([l.split(',')[1] for l in d.getVarFlag('FLASHLAYOUT_CONFIG_LABELS', 'nor-nor-emmc').split()])}"
FLASHLAYOUT_PARTITION_LABELS:nor-nor-sdcard = "${@' '.join([l.split(',')[1] for l in d.getVarFlag('FLASHLAYOUT_CONFIG_LABELS', 'nor-nor-sdcard').split()])}"
FLASHLAYOUT_PARTITION_LABELS:nor-sdcard = "${@' '.join(['%s' % l.split(',')[1] for l in d.getVarFlag('FLASHLAYOUT_CONFIG_LABELS', 'nor-sdcard').split()])}"
FLASHLAYOUT_PARTITION_LABELS:sdcard  = "${@' '.join(['%s' % l.split(',')[1] for l in d.getVarFlag('FLASHLAYOUT_CONFIG_LABELS', 'sdcard').split()])}"
FLASHLAYOUT_PARTITION_LABELS:sdcard-EFI = "${@' '.join(['%s' % l.split(',')[1] for l in d.getVarFlag('FLASHLAYOUT_CONFIG_LABELS', 'sdcard-EFI').split()])}"

# -----------------------------------------------------------------------------
# Partition properties configuration
# -----------------------------------------------------------------------------
# Priority var assignment (where <OVERRIDES> are the usual override mechanism
# with dynamic append of ongoing 'partition name' item):
#           1) FLASHLAYOUT_PARTITION_xxx:<BOOTSCHEME>:<CONFIG>:<OVERRIDES>
#           2) FLASHLAYOUT_PARTITION_xxx:<BOOTSCHEME>:<CONFIG>
#           3) FLASHLAYOUT_PARTITION_xxx:<BOOTSCHEME>:<OVERRIDES>
#           4) FLASHLAYOUT_PARTITION_xxx:<BOOTSCHEME>
#           5) FLASHLAYOUT_PARTITION_xxx:<CONFIG>:<OVERRIDES>
#           6) FLASHLAYOUT_PARTITION_xxx:<CONFIG>
#           7) FLASHLAYOUT_PARTITION_xxx:<OVERRIDES>
#           8) FLASHLAYOUT_PARTITION_xxx
#           9) Default 'FLASHLAYOUT_PARTITION_xxx' to 'none' when not defined
# -----------------------------------------------------------------------------
FLASHLAYOUT_PARTITION_ENABLE = "P"
FLASHLAYOUT_PARTITION_ENABLE:${STM32MP_FSBL_PROGRAMMER_NAME} = "-"
FLASHLAYOUT_PARTITION_ENABLE:${STM32MP_SSBL1_PROGRAMMER_NAME} = "-"
FLASHLAYOUT_PARTITION_ENABLE:${STM32MP_SSBL2_PROGRAMMER_NAME} = "-"

FLASHLAYOUT_PARTITION_ENABLE:empty = "PE"
# Need to make sure to delete partition that contains U-Boot env before update (gpt partitions only)
FLASHLAYOUT_PARTITION_ENABLE:sdcard:${STM32MP_SSBL1_NAME} = "${@bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', '', 'PD', d)}"
FLASHLAYOUT_PARTITION_ENABLE:sdcard-EFI:${STM32MP_SSBL1_NAME} = "${@bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', '', 'PD', d)}"
FLASHLAYOUT_PARTITION_ENABLE:emmc:${STM32MP_SSBL1_NAME} = "${@bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', '', 'PD', d)}"
FLASHLAYOUT_PARTITION_ENABLE:${STM32MP_METADATA_NAME} = "${@bb.utils.contains('MACHINE_FEATURES', 'fw-update', 'PD', 'PED', d)}"

# Second partition for A/B update: delete is skipped to decrease flashing time (never used)
FLASHLAYOUT_PARTITION_ENABLE:${STM32MP_M33DDRB_NAME} = "PE"
FLASHLAYOUT_PARTITION_ENABLE:${STM32MP_M33FWB_NAME} = "PE"

# -----------------------------------------------------------------------------
# Partition ID
# -----------------------------------------------------------------------------
# For FSBL and SSBL binaries loaded in RAM to program the devices there are
# reserved IDs on STM32CubeProgrammer side:
#   0x01 for FSBL
#   0x02 for SSBL1
#   0x03 for SSBL2
FLASHLAYOUT_PARTITION_ID:${STM32MP_FSBL_PROGRAMMER_NAME} = "0x01"
FLASHLAYOUT_PARTITION_ID:${STM32MP_SSBL1_PROGRAMMER_NAME} = "0x02"
FLASHLAYOUT_PARTITION_ID:${STM32MP_SSBL2_PROGRAMMER_NAME} = "0x03"

FLASHLAYOUT_PARTITION_TYPE = "Binary"
FLASHLAYOUT_PARTITION_TYPE:ubifs = "System"

FLASHLAYOUT_PARTITION_COPY = "1"

FLASHLAYOUT_PROGRAMMER_SECTIONS ?= "${@' '.join(['%s' % l.split(',')[1] for l in d.getVar('PARTITIONS_PROGRAMMER_CONFIG').split()])}"
FLASHLAYOUT_SDCARD_SECTIONS ?= "\
    ${@' '.join(['%s' % l.split(',')[1] for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'sdcard').split()]) if bb.utils.contains('MACHINE_FEATURES', 'm33td', True, False, d) else ''} \
    ${@' '.join(['%s' % l.split(',')[1] for l in d.getVarFlag('PARTITIONS_CONFIG', 'sdcard').split()])} \
    "
FLASHLAYOUT_EMMC_SECTIONS ?= "\
    ${@' '.join(['%s' % l.split(',')[1] for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'emmc').split()]) if bb.utils.contains('MACHINE_FEATURES', 'm33td', True, False, d) else ''} \
    ${@' '.join(['%s' % l.split(',')[1] for l in d.getVarFlag('PARTITIONS_CONFIG', 'emmc').split()])} \
    "

FLASHLAYOUT_SDCARD_ONLY_SECTIONS ?= "\
    ${@' '.join(['%s' % l.split(',')[1] for l in d.getVarFlag('PARTITIONS_CONFIG', 'sdcard').split()])} \
    "
FLASHLAYOUT_EMMC_ONLY_SECTIONS ?= "\
    ${@' '.join(['%s' % l.split(',')[1] for l in d.getVarFlag('PARTITIONS_CONFIG', 'emmc').split()])} \
    "

FLASHLAYOUT_PARTITION_DEVICE:emmc            = "none:${FLASHLAYOUT_PROGRAMMER_SECTIONS},${DEVICE:EMMC}:default"
FLASHLAYOUT_PARTITION_DEVICE:nand-2-128-256  = "none:${FLASHLAYOUT_PROGRAMMER_SECTIONS},${DEVICE:SPINAND}:default"
FLASHLAYOUT_PARTITION_DEVICE:nand-4-256-512  = "none:${FLASHLAYOUT_PROGRAMMER_SECTIONS},${DEVICE:SPINAND}:default"
FLASHLAYOUT_PARTITION_DEVICE:nand-4-256-1024 = "none:${FLASHLAYOUT_PROGRAMMER_SECTIONS},${DEVICE:NAND}:default"
FLASHLAYOUT_PARTITION_DEVICE:nand-2-128-256-sdcard  = "none:${FLASHLAYOUT_PROGRAMMER_SECTIONS},${DEVICE:SPINAND}:default,${DEVICE:SDCARD}:${FLASHLAYOUT_SDCARD_SECTIONS}"
FLASHLAYOUT_PARTITION_DEVICE:nand-4-256-512-sdcard  = "none:${FLASHLAYOUT_PROGRAMMER_SECTIONS},${DEVICE:SPINAND}:default,${DEVICE:SDCARD}:${FLASHLAYOUT_SDCARD_SECTIONS}"
FLASHLAYOUT_PARTITION_DEVICE:nand-4-256-1024-sdcard = "none:${FLASHLAYOUT_PROGRAMMER_SECTIONS},${DEVICE:NAND}:default,${DEVICE:SDCARD}:${FLASHLAYOUT_SDCARD_SECTIONS}"
FLASHLAYOUT_PARTITION_DEVICE:nor             = "none:${FLASHLAYOUT_PROGRAMMER_SECTIONS},${DEVICE:NOR}:default"
FLASHLAYOUT_PARTITION_DEVICE:nor-emmc        = "none:${FLASHLAYOUT_PROGRAMMER_SECTIONS},${DEVICE:NOR}:default,${DEVICE:EMMC}:${FLASHLAYOUT_EMMC_SECTIONS},${DEVICE:NOR}:bootdev:m33,${@bb.utils.contains('MACHINE_FEATURES', 'm33td', '${DEVICE:EMMC}:bootdev:a35', '${DEVICE:NOR}:bootdev:a35', d)}"
FLASHLAYOUT_PARTITION_DEVICE:nor-nor-emmc    = "none:${FLASHLAYOUT_PROGRAMMER_SECTIONS},${DEVICE:NOR}:default,${DEVICE:EMMC}:${FLASHLAYOUT_EMMC_ONLY_SECTIONS},${DEVICE:NOR}:bootdev:m33,${DEVICE:NOR}:bootdev:a35"
FLASHLAYOUT_PARTITION_DEVICE:nor-sdcard      = "none:${FLASHLAYOUT_PROGRAMMER_SECTIONS},${DEVICE:NOR}:default,${DEVICE:SDCARD}:${FLASHLAYOUT_SDCARD_SECTIONS},${DEVICE:NOR}:bootdev:m33,${@bb.utils.contains('MACHINE_FEATURES', 'm33td', '${DEVICE:SDCARD}:bootdev:a35', '${DEVICE:NOR}:bootdev:a35', d)}"
FLASHLAYOUT_PARTITION_DEVICE:nor-nor-sdcard  = "none:${FLASHLAYOUT_PROGRAMMER_SECTIONS},${DEVICE:NOR}:default,${DEVICE:SDCARD}:${FLASHLAYOUT_SDCARD_ONLY_SECTIONS},${DEVICE:NOR}:bootdev:m33,${DEVICE:NOR}:bootdev:a35"
FLASHLAYOUT_PARTITION_DEVICE:sdcard          = "none:${FLASHLAYOUT_PROGRAMMER_SECTIONS},${DEVICE:SDCARD}:default"
FLASHLAYOUT_PARTITION_DEVICE:sdcard-EFI      = "none:${FLASHLAYOUT_PROGRAMMER_SECTIONS},${DEVICE:SDCARD}:default"

STM32MP_BOOT1_NAME = "${@bb.utils.contains('MACHINE_FEATURES', 'm33td', '${STM32MP_FSBLM1_NAME}', '${STM32MP_FSBL1_NAME}', d)}"
STM32MP_BOOT2_NAME = "${@bb.utils.contains('MACHINE_FEATURES', 'm33td', '${STM32MP_FSBLM2_NAME}', '${STM32MP_FSBL2_NAME}', d)}"
FLASHLAYOUT_PARTITION_OFFSET:emmc:${STM32MP_BOOT1_NAME} = "${DEVICE_START_OFFSET_BOOT0:EMMC}"
FLASHLAYOUT_PARTITION_OFFSET:emmc:${STM32MP_BOOT2_NAME} = "${DEVICE_START_OFFSET_BOOT1:EMMC}"
FLASHLAYOUT_PARTITION_OFFSET:nor-emmc:${STM32MP_FSBL1_NAME} = "${DEVICE_START_OFFSET_BOOT0:EMMC}"
FLASHLAYOUT_PARTITION_OFFSET:nor-emmc:${STM32MP_FSBL2_NAME} = "${DEVICE_START_OFFSET_BOOT1:EMMC}"

# Size defined in Kbytes
# Override default partition size for rootfs to increase available free space
# 3GiB for emmc
FLASHLAYOUT_PARTITION_SIZE:emmc:${STM32MP_ROOTFS_LABEL} = "3145728"
# 4GiB for sdcard
FLASHLAYOUT_PARTITION_SIZE:nor-sdcard:${STM32MP_ROOTFS_LABEL} = "4194304"
FLASHLAYOUT_PARTITION_SIZE:nor-nor-sdcard:${STM32MP_ROOTFS_LABEL} = "4194304"
FLASHLAYOUT_PARTITION_SIZE:nor-emmc:${STM32MP_ROOTFS_LABEL} = "3145728"
FLASHLAYOUT_PARTITION_SIZE:nor-nor-emmc:${STM32MP_ROOTFS_LABEL} = "3145728"
FLASHLAYOUT_PARTITION_SIZE:sdcard:${STM32MP_ROOTFS_LABEL}     = "4194304"
FLASHLAYOUT_PARTITION_SIZE:sdcard-EFI:${STM32MP_ROOTFS_LABEL} = "4194304"
FLASHLAYOUT_PARTITION_SIZE:nand-4-256-1024-sdcard:${STM32MP_ROOTFS_LABEL} = "4194304"
FLASHLAYOUT_PARTITION_SIZE:nand-2-128-256-sdcard:${STM32MP_ROOTFS_LABEL} = "4194304"
FLASHLAYOUT_PARTITION_SIZE:nand-4-256-512-sdcard:${STM32MP_ROOTFS_LABEL} = "4194304"
# Override default partition size for vendorfs
FLASHLAYOUT_PARTITION_SIZE:nor-sdcard:${STM32MP_VENDORFS_LABEL}:stm32mp2common = "256000"
FLASHLAYOUT_PARTITION_SIZE:nor-nor-sdcard:${STM32MP_VENDORFS_LABEL}:stm32mp2common = "256000"
FLASHLAYOUT_PARTITION_SIZE:nor-emmc:${STM32MP_VENDORFS_LABEL}:stm32mp2common = "256000"
FLASHLAYOUT_PARTITION_SIZE:nor-nor-emmc:${STM32MP_VENDORFS_LABEL}:stm32mp2common = "256000"
FLASHLAYOUT_PARTITION_SIZE:sdcard:${STM32MP_VENDORFS_LABEL}:stm32mp2common     = "256000"
FLASHLAYOUT_PARTITION_SIZE:emmc:${STM32MP_VENDORFS_LABEL}:stm32mp2common       = "256000"
FLASHLAYOUT_PARTITION_SIZE:nand-4-256-1024-sdcard:${STM32MP_VENDORFS_LABEL} = "199680"
FLASHLAYOUT_PARTITION_SIZE:nand-2-128-256-sdcard:${STM32MP_VENDORFS_LABEL} = "199680"
FLASHLAYOUT_PARTITION_SIZE:nand-4-256-512-sdcard:${STM32MP_VENDORFS_LABEL} = "199680"

# Set binaries to use for each partition
# -----------------------------------------------------------------------------
# Use following pattern in binary name to expand to specific label config:
#       '<CONFIG>'      (to insert label from FLASHLAYOUT_CONFIG_LABELS - NB: substitution for all '-' in label by '_')
#       '<BOOTSCHEME>'  (to insert label from FLASHLAYOUT_BOOTSCHEME_LABELS)
#       '<BOOTDEV_A35>' (to insert label for boot device A35 from FLASHLAYOUT_PARTITION_DEVICE settings)
#       '<BOOTDEV_M33>' (to insert label for boot device M33 from FLASHLAYOUT_PARTITION_DEVICE settings)
#       '<DEVICE>'      (to insert label from FLASHLAYOUT_PARTITION_DEVICE)
#       '<TYPE>'        (to insert label from FLASHLAYOUT_TYPE_LABELS)
# These patterns are processed to expand binary name for each config.
# -----------------------------------------------------------------------------
# Specific rename for 'fastboot' bootscheme as we want to use fastboot fip for programmer
FLASHLAYOUT_PARTITION_BIN2LOAD:fastboot:${STM32MP_FSBL_PROGRAMMER_NAME} = "arm-trusted-firmware/tf-a-<TYPE>${STM32MP_DEVICETREE_PROGRAMMER_SUFFIX}-optee-${STM32MP_PROGRAMMER_SCHEME}-${STMP32MP_PROGRAMMER_SERIAL}${FIP_ENCRYPT_SUFFIX}${FIP_SIGN_SUFFIX}.stm32"

FLASHLAYOUT_PARTITION_BIN2LOAD:fastboot:${STM32MP_SSBL1_PROGRAMMER_NAME} = "fip/fip-<TYPE>${STM32MP_DEVICETREE_PROGRAMMER_SUFFIX}-ddr-<BOOTSCHEME>-<DEVICE>${FIP_ENCRYPT_SUFFIX}${FIP_SIGN_SUFFIX}.bin"
FLASHLAYOUT_PARTITION_BIN2LOAD:fastboot:${STM32MP_SSBL2_PROGRAMMER_NAME} = "fip/fip-<TYPE>${STM32MP_DEVICETREE_PROGRAMMER_SUFFIX}-<BOOTSCHEME>-<DEVICE>${FIP_ENCRYPT_SUFFIX}${FIP_SIGN_SUFFIX}.bin"

FLASHLAYOUT_PARTITION_BIN2LOAD:fastboot-opteemin:${STM32MP_FSBL_PROGRAMMER_NAME} = "arm-trusted-firmware/tf-a-<TYPE>${STM32MP_DEVICETREE_PROGRAMMER_SUFFIX}-opteemin-${STM32MP_PROGRAMMER_SCHEME}-${STMP32MP_PROGRAMMER_SERIAL}${FIP_ENCRYPT_SUFFIX}${FIP_SIGN_SUFFIX}.stm32"
FLASHLAYOUT_PARTITION_BIN2LOAD:fastboot-opteemin:${STM32MP_SSBL1_PROGRAMMER_NAME} = "fip/fip-<TYPE>${STM32MP_DEVICETREE_PROGRAMMER_SUFFIX}-ddr-<BOOTSCHEME>-<DEVICE>${FIP_ENCRYPT_SUFFIX}${FIP_SIGN_SUFFIX}.bin"
FLASHLAYOUT_PARTITION_BIN2LOAD:fastboot-opteemin:${STM32MP_SSBL2_PROGRAMMER_NAME} = "fip/fip-<TYPE>${STM32MP_DEVICETREE_PROGRAMMER_SUFFIX}-<BOOTSCHEME>-<DEVICE>${FIP_ENCRYPT_SUFFIX}${FIP_SIGN_SUFFIX}.bin"

# -----------------------------------------------------------------------------
# Use the 'FLASHLAYOUT_PARTITION_REPLACE_PATTERNS' var to allow dynamic binary
# renaming for the bootloader binaries. This is only required for all FLASHLAYOUT_PROGRAMMER_SECTIONS
# partitions that provides the binary to flash the device.
# The format to follow is:
#   '<PATTERN2REPLACE_1>;<PATTERN2SET_1> <PATTERN2REPLACE_2>;<PATTERN2SET_2>'
# And the pattern to replace in binary name is searched as:
#   '[-_]<PATTERN>([-_.]|$)'
# -----------------------------------------------------------------------------
# The daughter board does not support Programmer mode, so use eval one
# (valid for FLASHLAYOUT_PROGRAMMER_SECTIONS partitions)
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS:${STM32MP_FSBL_PROGRAMMER_NAME}:append = " ed1;ev1"
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS:${STM32MP_SSBL1_PROGRAMMER_NAME}:append = " ed1;ev1"
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS:${STM32MP_SSBL2_PROGRAMMER_NAME}:append = " ed1;ev1"
# Specific rename for 'fastboot' bootscheme as we use same bl2 as 'optee' bootscheme
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS:fastboot:${STM32MP_FSBL1_NAME}:append = " fastboot;optee"
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS:fastboot-opteemin:${STM32MP_FSBL1_NAME}:append = " fastboot-opteemin;opteemin"
# Specific rename for 'fastboot' bootscheme as we use same fip as 'optee' bootscheme
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS:fastboot:${STM32MP_SSBL1_NAME}:append = " fastboot;optee"
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS:fastboot-opteemin:${STM32MP_SSBL1_NAME}:append = " fastboot-opteemin;opteemin"
