#@DESCRIPTION: partition images configuration for STM32 systems

def partImage2partConfig(d, config, fstype=None):
    """
    Convert PARTTIONS_IMAGES['config'] setting format to format expected to feed
    PARTITIONS_CONFIG[xxx].
    Manage <image_name> update respect to 'fstype' provided and apply the rootfs
    namming or standard partition image one.
        FROM: <image_name>,<partition_label>,<mountpoint>,<size>,<type>[,<copy>,<limit fs>]
        TO  : <binary_name>,<partition_label>,<size>,<type>,<copy>
    """
    items = d.getVarFlag('PARTITIONS_IMAGES', config).split(',') or ""
    if len(items) > 7:
        bb.fatal('[partImage2partConfig] Wrong settings for PARTTIONS_IMAGES[%s] : %s' % (config, items))
    overrides = d.getVar('OVERRIDES')
    bin_suffix = '-${MACHINE}'
    addons_partition = ""
    if (d.getVar('ENABLE_PARTITIONS_IMAGE') or "0") != "0":
        addons_partition = "splitted-"
    if fstype:
        bin_name = items[0] + bin_suffix + '.' + addons_partition + items[1] + '.' + fstype
    else:
        bin_name = items[0] + bin_suffix + '.' + addons_partition + items[1]
    if len(items) == 6:
        copy = items[5]
    else:
        copy = '1'
    # Set string for PARTITIONS_CONFIG item: <binary_name>,<partlabel>,<size>,<type>,<copy>
    part_format = bin_name + ',' + items[1] + ',' + items[3] + ',' + items[4] + ',' + copy
    return part_format

# =========================================================================
# Programmer raw partition configuration : data, label, size (Kbytes), type
# =========================================================================
STM32MP_FSBL_PROGRAMMER_DATA ?= "arm-trusted-firmware/tf-a-<TYPE>${STM32MP_DEVICETREE_PROGRAMMER_SUFFIX}-<BOOTSCHEME>-${STM32MP_PROGRAMMER_SCHEME}-${STMP32MP_PROGRAMMER_SERIAL}${TF_A_ENCRYPT_SUFFIX}${TF_A_SIGN_SUFFIX}.stm32"
STM32MP_FSBL_PROGRAMMER_NAME ?= "fsbl-boot"
STM32MP_SSBL1_PROGRAMMER_DATA ?= "fip/fip-<TYPE>${STM32MP_DEVICETREE_PROGRAMMER_SUFFIX}-ddr-<BOOTSCHEME>-${STM32MP_PROGRAMMER_SCHEME}-${STMP32MP_PROGRAMMER_SERIAL}${FIP_ENCRYPT_SUFFIX}${FIP_SIGN_SUFFIX}.bin"
STM32MP_SSBL1_PROGRAMMER_NAME ?= "fip-ddr"
STM32MP_SSBL1_PROGRAMMER_ENABLE ?= "0"
STM32MP_SSBL1_PROGRAMMER_ENABLE:stm32mp2common ?= "1"
STM32MP_SSBL2_PROGRAMMER_DATA ?= "fip/fip-<TYPE>${STM32MP_DEVICETREE_PROGRAMMER_SUFFIX}-<BOOTSCHEME>-${STM32MP_PROGRAMMER_SCHEME}-${STMP32MP_PROGRAMMER_SERIAL}${FIP_ENCRYPT_SUFFIX}${FIP_SIGN_SUFFIX}.bin"
STM32MP_SSBL2_PROGRAMMER_NAME ?= "fip-boot"

# Programmer Partitions configuration
# <binary_name>,<partlabel>,<size>,<type>
PARTITIONS_PROGRAMMER_CONFIG = "\
    ${@ '${STM32MP_FSBL_PROGRAMMER_DATA},${STM32MP_FSBL_PROGRAMMER_NAME},,Binary' if '${STM32MP_FSBL_PROGRAMMER_NAME}' else ''} \
    ${@ '${STM32MP_SSBL1_PROGRAMMER_DATA},${STM32MP_SSBL1_PROGRAMMER_NAME},,FIP' if ${STM32MP_SSBL1_PROGRAMMER_ENABLE} else ''} \
    ${@ '${STM32MP_SSBL2_PROGRAMMER_DATA},${STM32MP_SSBL2_PROGRAMMER_NAME},,FIP' if '${STM32MP_SSBL2_PROGRAMMER_NAME}' else ''} \
    "

# =========================================================================
# Bootloader raw partition configuration : data, label, size (Kbytes), type, copy
# =========================================================================
# -----------------------------------------------------------------------------
# NOTE: There are few restrictions to follow:
#       -   The partition for the first boot loader should follow the naming
#           rule: fsbl*
#       -   The partition for the secondary boot loader should follow the naming
#           rule: fip
# -----------------------------------------------------------------------------
STM32MP_FSBL1_DATA ?= "arm-trusted-firmware/tf-a-<TYPE>-<BOOTSCHEME>-<DEVICE>${TF_A_ENCRYPT_SUFFIX}${TF_A_SIGN_SUFFIX}.stm32"
STM32MP_FSBL1_NAME ?= "${@bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', 'fsbl', 'fsbl1', d)}"
STM32MP_FSBL1_NAME:stm32mp2common ?= "fsbla"
STM32MP_FSBL1_SIZE ?= "256"
STM32MP_FSBL2_DATA ?= "${STM32MP_FSBL1_DATA}"
STM32MP_FSBL2_NAME ?= "fsbl2"
STM32MP_FSBL2_NAME:stm32mp2common ?= "fsbla2"
STM32MP_FSBL2_SIZE ?= "${STM32MP_FSBL1_SIZE}"
STM32MP_SSBL1_DATA ?= "fip/fip-<TYPE>-<BOOTSCHEME>-<DEVICE>${FIP_ENCRYPT_SUFFIX}${FIP_SIGN_SUFFIX}.bin"
STM32MP_SSBL1_NAME ?= "${@bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', 'fip-a', 'fip', d)}"
STM32MP_SSBL1_SIZE ?= "${@bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', '4096', '2048', d)}"
STM32MP_SSBL2_DATA ?= "${@bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', '', '${STM32MP_SSBL1_DATA}', d)}"
STM32MP_SSBL2_NAME ?= "${@bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', 'fip-b', 'fip2', d)}"
STM32MP_SSBL2_SIZE ?= "${STM32MP_SSBL1_SIZE}"
STM32MP_UENV_DATA ?= ""
STM32MP_UENV_NAME ?= "u-boot-env"
STM32MP_UENV_SIZE ?= "512"
STM32MP_METADATA_DATA ?= "${@bb.utils.contains('MACHINE_FEATURES', 'fw-update', 'arm-trusted-firmware/${TF_A_METADATA_BINARY}', '', d)}"
STM32MP_METADATA_NAME ?= "metadata"
STM32MP_METADATA_ENABLE ?= "${@bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', '1', '0', d)}"
STM32MP_METADATA_SIZE ?= "256"

# =========================================================================
# M33TD
# =========================================================================
STM32MP_FSBLM1_DATA ?= "arm-trusted-firmware-m/bl2/bl2-<TYPE>-<DEVICE>${TF_M_ENCRYPT_SUFFIX}${TF_M_SIGN_SUFFIX}.stm32"
STM32MP_FSBLM1_NAME ?= "fsblm1"
STM32MP_FSBLM1_SIZE ?= "256"
STM32MP_FSBLM2_DATA ?= "${STM32MP_FSBLM1_DATA}"
STM32MP_FSBLM2_NAME ?= "fsblm2"
STM32MP_FSBLM2_SIZE ?= "${STM32MP_FSBLM1_SIZE}"
STM32MP_M33DDRA_DATA ?= "m33-firmware/ddr_phy-<TYPE>-<DEVICE>${M33FW_SIGN_SUFFIX}.bin"
STM32MP_M33DDRA_NAME ?= "m33ddr-a"
STM32MP_M33DDRA_SIZE ?= "256"
STM32MP_M33DDRB_DATA ?= ""
STM32MP_M33DDRB_NAME ?= "m33ddr-b"
STM32MP_M33DDRB_SIZE ?= "${STM32MP_M33DDRA_SIZE}"
STM32MP_M33FWA_DATA ?= "m33-firmware/tfm-starterapp-<TYPE>-<DEVICE>-<BOOTDEV_A35>_s_ns${M33FW_SIGN_SUFFIX}.bin"
STM32MP_M33FWA_NAME ?= "m33fw-a"
STM32MP_M33FWA_SIZE ?= "9216"
STM32MP_M33FWB_DATA ?= ""
STM32MP_M33FWB_NAME ?= "m33fw-b"
STM32MP_M33FWB_SIZE ?= "${STM32MP_M33FWA_SIZE}"
STM32MP_M33PS_DATA ?= ""
STM32MP_M33PS_NAME ?= "m33ps"
STM32MP_M33PS_SIZE ?= "1024"

# Specific override for NAND device type regarding partition sizes to follow
# the hard coded configuration on U-Boot source code
STM32MP_FSBL1_SIZE_UBOOT ?= "${@bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', '512', '1024', d)}"
STM32MP_METADATA_SIZE_UBOOT ?= "512"

# Bootloader Partitions configuration
PARTITIONS_BOOTLOADER_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'emmc', '', d)}"
PARTITIONS_BOOTLOADER_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-4-256-1024', 'nand-4-256-1024', '', d)}"
PARTITIONS_BOOTLOADER_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-4-256-1024-sdcard', 'nand-4-256-1024 sdcard', '', d)}"
PARTITIONS_BOOTLOADER_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nor-sdcard', 'nor sdcard', '', d)}"
PARTITIONS_BOOTLOADER_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nor', 'nor', '', d)}"
PARTITIONS_BOOTLOADER_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', 'sdcard', '', d)}"
PARTITIONS_BOOTLOADER_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-2-128-256', 'nand-2-128-256', '', d)}"
PARTITIONS_BOOTLOADER_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-2-128-256-sdcard', 'nand-2-128-256 sdcard', '', d)}"
PARTITIONS_BOOTLOADER_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-4-256-512', 'nand-4-256-512', '', d)}"
PARTITIONS_BOOTLOADER_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-4-256-512-sdcard', 'nand-4-256-512 sdcard', '', d)}"
PARTITIONS_BOOTLOADER_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-custom', 'nand-custom', '', d)}"
PARTITIONS_BOOTLOADER_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-custom-sdcard', 'nand-custom-sdcard', '', d)}"
PARTITIONS_BOOTLOADER_CONFIG:stm32mp2m33td += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nor-sdcard', 'nor-m33td sdcard', '', d)}"
PARTITIONS_BOOTLOADER_CONFIG:stm32mp2m33td += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nor-emmc', 'nor-m33td emmc', '', d)}"

# <binary_name>,<partlabel>,<size>,<type>,<copy>
PARTITIONS_BOOTLOADER_CONFIG[emmc] ?= "\
    ${@ '${STM32MP_FSBL1_DATA},${STM32MP_FSBL1_NAME},${STM32MP_FSBL1_SIZE},Binary,' + bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', '2', '1', d) if '${STM32MP_FSBL1_NAME}' else ''} \
    ${@ '${STM32MP_FSBL2_DATA},${STM32MP_FSBL2_NAME},${STM32MP_FSBL2_SIZE},Binary,1' if '${STM32MP_FSBL2_NAME}' and bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', False, True, d) else ''} \
    ${@ '${STM32MP_METADATA_DATA},${STM32MP_METADATA_NAME},${STM32MP_METADATA_SIZE},FWU_MDATA,2' if ${STM32MP_METADATA_ENABLE} else ''} \
    ${@ '${STM32MP_SSBL1_DATA},${STM32MP_SSBL1_NAME},${STM32MP_SSBL1_SIZE},FIP,1' if '${STM32MP_SSBL1_NAME}' else ''} \
    ${@ '${STM32MP_SSBL2_DATA},${STM32MP_SSBL2_NAME},${STM32MP_SSBL2_SIZE},FIP,1' if '${STM32MP_SSBL2_NAME}' and bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', True, False, d) else ''} \
    ${@ '${STM32MP_UENV_DATA},${STM32MP_UENV_NAME},${STM32MP_UENV_SIZE},ENV,1' if '${STM32MP_UENV_NAME}' else ''} \
    "
PARTITIONS_BOOTLOADER_CONFIG[nand-2-128-256] ?= "\
    ${@ '${STM32MP_FSBL1_DATA},${STM32MP_FSBL1_NAME},${STM32MP_FSBL1_SIZE_UBOOT},Binary,2' if '${STM32MP_FSBL1_NAME}' else ''} \
    ${@ '${STM32MP_METADATA_DATA},${STM32MP_METADATA_NAME},${STM32MP_METADATA_SIZE_UBOOT},FWU_MDATA,2' if ${STM32MP_METADATA_ENABLE} else ''} \
    ${@ '${STM32MP_SSBL1_DATA},${STM32MP_SSBL1_NAME},${STM32MP_SSBL1_SIZE},FIP,' + bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', '2', '1', d) if '${STM32MP_SSBL1_NAME}' else ''} \
    ${@ '${STM32MP_SSBL2_DATA},${STM32MP_SSBL2_NAME},${STM32MP_SSBL1_SIZE},FIP,' + bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', '2', '1', d) if '${STM32MP_SSBL2_NAME}' else ''} \
    "
PARTITIONS_BOOTLOADER_CONFIG[nand-4-256-512] ?= "\
    ${@ '${STM32MP_FSBL1_DATA},${STM32MP_FSBL1_NAME},${STM32MP_FSBL1_SIZE_UBOOT},Binary,2' if '${STM32MP_FSBL1_NAME}' else ''} \
    ${@ '${STM32MP_METADATA_DATA},${STM32MP_METADATA_NAME},${STM32MP_METADATA_SIZE_UBOOT},FWU_MDATA,2' if ${STM32MP_METADATA_ENABLE} else ''} \
    ${@ '${STM32MP_SSBL1_DATA},${STM32MP_SSBL1_NAME},${STM32MP_SSBL1_SIZE},FIP,' + bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', '2', '1', d) if '${STM32MP_SSBL1_NAME}' else ''} \
    ${@ '${STM32MP_SSBL2_DATA},${STM32MP_SSBL2_NAME},${STM32MP_SSBL1_SIZE},FIP,' + bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', '2', '1', d) if '${STM32MP_SSBL2_NAME}' else ''} \
    "
PARTITIONS_BOOTLOADER_CONFIG[nand-4-256-1024] ?= "\
    ${@ '${STM32MP_FSBL1_DATA},${STM32MP_FSBL1_NAME},${STM32MP_FSBL1_SIZE_UBOOT},Binary,2' if '${STM32MP_FSBL1_NAME}' else ''} \
    ${@ '${STM32MP_METADATA_DATA},${STM32MP_METADATA_NAME},${STM32MP_METADATA_SIZE_UBOOT},FWU_MDATA,2' if ${STM32MP_METADATA_ENABLE} else ''} \
    ${@ '${STM32MP_SSBL1_DATA},${STM32MP_SSBL1_NAME},${STM32MP_SSBL1_SIZE},FIP,' + bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', '2', '1', d) if '${STM32MP_SSBL1_NAME}' else ''} \
    ${@ '${STM32MP_SSBL2_DATA},${STM32MP_SSBL2_NAME},${STM32MP_SSBL1_SIZE},FIP,' + bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', '2', '1', d) if '${STM32MP_SSBL2_NAME}' else ''} \
    "
PARTITIONS_BOOTLOADER_CONFIG[nand-custom] ?= "\
    ${@ '${STM32MP_FSBL1_DATA},${STM32MP_FSBL1_NAME},${STM32MP_FSBL1_SIZE_UBOOT},Binary,2' if '${STM32MP_FSBL1_NAME}' else ''} \
    ${@ '${STM32MP_METADATA_DATA},${STM32MP_METADATA_NAME},${STM32MP_METADATA_SIZE_UBOOT},FWU_MDATA,2' if ${STM32MP_METADATA_ENABLE} else ''} \
    ${@ '${STM32MP_SSBL1_DATA},${STM32MP_SSBL1_NAME},${STM32MP_SSBL1_SIZE},FIP,' + bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', '2', '1', d) if '${STM32MP_SSBL1_NAME}' else ''} \
    ${@ '${STM32MP_SSBL2_DATA},${STM32MP_SSBL2_NAME},${STM32MP_SSBL1_SIZE},FIP,' + bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', '2', '1', d) if '${STM32MP_SSBL2_NAME}' else ''} \
    "
PARTITIONS_BOOTLOADER_CONFIG[nor] ?= "\
    ${@ '${STM32MP_FSBLM1_DATA},${STM32MP_FSBLM1_NAME},${STM32MP_FSBLM1_SIZE},Binary,1' if '${STM32MP_FSBLM1_NAME}' and bb.utils.contains('MACHINE_FEATURES', 'm33td', True, False, d) else ''} \
    ${@ '${STM32MP_FSBLM2_DATA},${STM32MP_FSBLM2_NAME},${STM32MP_FSBLM2_SIZE},Binary,1' if '${STM32MP_FSBLM2_NAME}' and bb.utils.contains('MACHINE_FEATURES', 'm33td', True, False, d) else ''} \
    ${@ '${STM32MP_FSBL1_DATA},${STM32MP_FSBL1_NAME},${STM32MP_FSBL1_SIZE},Binary,' + bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', '2', '1', d) if '${STM32MP_FSBL1_NAME}' else ''} \
    ${@ '${STM32MP_FSBL2_DATA},${STM32MP_FSBL2_NAME},${STM32MP_FSBL2_SIZE},Binary,1' if '${STM32MP_FSBL2_NAME}' and bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', False, True, d) else ''} \
    ${@ '${STM32MP_M33DDRA_DATA},${STM32MP_M33DDRA_NAME},${STM32MP_M33DDRA_SIZE},Binary,1' if '${STM32MP_M33DDRA_NAME}' and bb.utils.contains('MACHINE_FEATURES', 'm33td', True, False, d) else ''} \
    ${@ '${STM32MP_M33DDRB_DATA},${STM32MP_M33DDRB_NAME},${STM32MP_M33DDRB_SIZE},Binary,1' if '${STM32MP_M33DDRB_NAME}' and bb.utils.contains('MACHINE_FEATURES', 'm33td', True, False, d) else ''} \
    ${@ '${STM32MP_M33FWA_DATA},${STM32MP_M33FWA_NAME},${STM32MP_M33FWA_SIZE},Binary,1' if '${STM32MP_M33FWA_NAME}' and bb.utils.contains('MACHINE_FEATURES', 'm33td', True, False, d) else ''} \
    ${@ '${STM32MP_M33FWB_DATA},${STM32MP_M33FWB_NAME},${STM32MP_M33FWB_SIZE},Binary,1' if '${STM32MP_M33FWB_NAME}' and bb.utils.contains('MACHINE_FEATURES', 'm33td', True, False, d) else ''} \
    ${@ 'none,${STM32MP_M33PS_NAME}_NU,${STM32MP_M33PS_SIZE},Binary,1' if '${STM32MP_M33PS_NAME}' and bb.utils.contains('MACHINE_FEATURES', 'm33td', True, False, d) else ''} \
    ${@ '${STM32MP_METADATA_DATA},${STM32MP_METADATA_NAME},${STM32MP_METADATA_SIZE},FWU_MDATA,2' if ${STM32MP_METADATA_ENABLE} else ''} \
    ${@ '${STM32MP_SSBL1_DATA},${STM32MP_SSBL1_NAME},${STM32MP_SSBL1_SIZE},FIP,1' if '${STM32MP_SSBL1_NAME}' else ''} \
    ${@ '${STM32MP_SSBL2_DATA},${STM32MP_SSBL2_NAME},${STM32MP_SSBL2_SIZE},FIP,1' if '${STM32MP_SSBL2_NAME}' and bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', True, False, d) else ''} \
    ${@ '${STM32MP_UENV_DATA},${STM32MP_UENV_NAME},${STM32MP_UENV_SIZE},ENV,1' if '${STM32MP_UENV_NAME}' else ''} \
    "
PARTITIONS_BOOTLOADER_CONFIG[sdcard] ?= "\
    ${@ '${STM32MP_FSBL1_DATA},${STM32MP_FSBL1_NAME},${STM32MP_FSBL1_SIZE},Binary,' + bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', '2', '1', d) if '${STM32MP_FSBL1_NAME}' else ''} \
    ${@ '${STM32MP_FSBL2_DATA},${STM32MP_FSBL2_NAME},${STM32MP_FSBL2_SIZE},Binary,1' if '${STM32MP_FSBL2_NAME}' and bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', False, True, d) else ''} \
    ${@ '${STM32MP_METADATA_DATA},${STM32MP_METADATA_NAME},${STM32MP_METADATA_SIZE},FWU_MDATA,2' if ${STM32MP_METADATA_ENABLE} else ''} \
    ${@ '${STM32MP_SSBL1_DATA},${STM32MP_SSBL1_NAME},${STM32MP_SSBL1_SIZE},FIP,1' if '${STM32MP_SSBL1_NAME}' else ''} \
    ${@ '${STM32MP_SSBL2_DATA},${STM32MP_SSBL2_NAME},${STM32MP_SSBL2_SIZE},FIP,1' if '${STM32MP_SSBL2_NAME}' else ''} \
    ${@ '${STM32MP_UENV_DATA},${STM32MP_UENV_NAME},${STM32MP_UENV_SIZE},ENV,1' if '${STM32MP_UENV_NAME}' else ''} \
    "

PARTITION_UBOOT_ENV_UBI_CONFIG ?= "\
    ${IMAGE_LINK_NAME}_u-boot-env-image_<CONFIG>.ubi,${STM32MP_UBIFS_NAME},${STM32MP_UBIFS_SIZE},System,1 \
    "

PARTITIONS_BOOTLOADER_CONFIG[sdcard-m33td] ?= "\
    ${@ '${STM32MP_FSBLM1_DATA},${STM32MP_FSBLM1_NAME},${STM32MP_FSBLM1_SIZE},Binary,1' if '${STM32MP_FSBLM1_NAME}' else ''} \
    ${@ '${STM32MP_FSBLM2_DATA},${STM32MP_FSBLM2_NAME},${STM32MP_FSBLM2_SIZE},Binary,1' if '${STM32MP_FSBLM2_NAME}' else ''} \
    ${@ '${STM32MP_M33DDRA_DATA},${STM32MP_M33DDRA_NAME},${STM32MP_M33DDRA_SIZE},Binary,1' if '${STM32MP_M33DDRA_NAME}' else ''} \
    ${@ '${STM32MP_M33DDRB_DATA},${STM32MP_M33DDRB_NAME},${STM32MP_M33DDRB_SIZE},Binary,1' if '${STM32MP_M33DDRB_NAME}' else ''} \
    ${@ '${STM32MP_M33FWA_DATA},${STM32MP_M33FWA_NAME},${STM32MP_M33FWA_SIZE},Binary,1' if '${STM32MP_M33FWA_NAME}' else ''} \
    ${@ '${STM32MP_M33FWB_DATA},${STM32MP_M33FWB_NAME},${STM32MP_M33FWB_SIZE},Binary,1' if '${STM32MP_M33FWB_NAME}' else ''} \
    "
PARTITIONS_BOOTLOADER_CONFIG[emmc-m33td] ?= "\
    ${@ '${STM32MP_FSBLM1_DATA},${STM32MP_FSBLM1_NAME},${STM32MP_FSBLM1_SIZE},Binary,1' if '${STM32MP_FSBLM1_NAME}' else ''} \
    ${@ '${STM32MP_FSBLM2_DATA},${STM32MP_FSBLM2_NAME},${STM32MP_FSBLM2_SIZE},Binary,1' if '${STM32MP_FSBLM2_NAME}' else ''} \
    ${@ '${STM32MP_M33DDRA_DATA},${STM32MP_M33DDRA_NAME},${STM32MP_M33DDRA_SIZE},Binary,1' if '${STM32MP_M33DDRA_NAME}' else ''} \
    ${@ '${STM32MP_M33DDRB_DATA},${STM32MP_M33DDRB_NAME},${STM32MP_M33DDRB_SIZE},Binary,1' if '${STM32MP_M33DDRB_NAME}' else ''} \
    ${@ '${STM32MP_M33FWA_DATA},${STM32MP_M33FWA_NAME},${STM32MP_M33FWA_SIZE},Binary,1' if '${STM32MP_M33FWA_NAME}' else ''} \
    ${@ '${STM32MP_M33FWB_DATA},${STM32MP_M33FWB_NAME},${STM32MP_M33FWB_SIZE},Binary,1' if '${STM32MP_M33FWB_NAME}' else ''} \
    "
PARTITIONS_BOOTLOADER_CONFIG[nor-m33td] ?= "\
    ${@ '${STM32MP_FSBLM1_DATA},${STM32MP_FSBLM1_NAME},${STM32MP_FSBLM1_SIZE},Binary,1' if '${STM32MP_FSBLM1_NAME}' else ''} \
    ${@ '${STM32MP_FSBLM2_DATA},${STM32MP_FSBLM2_NAME},${STM32MP_FSBLM2_SIZE},Binary,1' if '${STM32MP_FSBLM2_NAME}' else ''} \
    ${@ 'none,${STM32MP_FSBL1_NAME}_NU,${STM32MP_FSBL1_SIZE},Binary,' + bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', '2', '1', d) if '${STM32MP_FSBL1_NAME}' else ''} \
    ${@ 'none,${STM32MP_FSBL2_NAME}_NU,${STM32MP_FSBL2_SIZE},Binary,1' if '${STM32MP_FSBL2_NAME}' and bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', False, True, d) else ''} \
    ${@ '${STM32MP_M33DDRA_DATA},${STM32MP_M33DDRA_NAME},${STM32MP_M33DDRA_SIZE},Binary,1' if '${STM32MP_M33DDRA_NAME}' else ''} \
    ${@ '${STM32MP_M33DDRB_DATA},${STM32MP_M33DDRB_NAME},${STM32MP_M33DDRB_SIZE},Binary,1' if '${STM32MP_M33DDRB_NAME}' else ''} \
    ${@ '${STM32MP_M33FWA_DATA},${STM32MP_M33FWA_NAME},${STM32MP_M33FWA_SIZE},Binary,1' if '${STM32MP_M33FWA_NAME}' else ''} \
    ${@ '${STM32MP_M33FWB_DATA},${STM32MP_M33FWB_NAME},${STM32MP_M33FWB_SIZE},Binary,1' if '${STM32MP_M33FWB_NAME}' else ''} \
    ${@ '${STM32MP_M33PS_DATA},${STM32MP_M33PS_NAME},${STM32MP_M33PS_SIZE},Binary,1' if '${STM32MP_M33PS_NAME}' else ''} \
    ${@ 'none,${STM32MP_METADATA_NAME}_NU,${STM32MP_METADATA_SIZE},FWU_MDATA,2' if ${STM32MP_METADATA_ENABLE} else ''} \
    ${@ 'none,${STM32MP_SSBL1_NAME}_NU,${STM32MP_SSBL1_SIZE},FIP,1' if '${STM32MP_SSBL1_NAME}' else ''} \
    ${@ 'none,${STM32MP_SSBL2_NAME}_NU,${STM32MP_SSBL2_SIZE},FIP,1' if '${STM32MP_SSBL2_NAME}' and bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '1', True, False, d) else ''} \
    ${@ 'none,${STM32MP_UENV_NAME}_NU,${STM32MP_UENV_SIZE},ENV,1' if '${STM32MP_UENV_NAME}' else ''} \
    "

# =========================================================================
# Partition image configuration :
# =========================================================================
# Enable use of extra partition(s)
ST_BOOTFS   ?= "1"
ST_VENDORFS ?= "1"
ST_USERFS   ?= "1"

ST_PARTITIONS_IMAGES = "${@bb.utils.contains('ST_BOOTFS', '1', 'bootfs', '', d)}"
ST_PARTITIONS_IMAGES += "${@bb.utils.contains('ST_VENDORFS', '1', 'vendorfs', '', d)}"
ST_PARTITIONS_IMAGES += "rootfs"
ST_PARTITIONS_IMAGES += "${@bb.utils.contains('ST_USERFS', '1', 'userfs', '', d)}"

PARTITIONS_IMAGES ?= "${@bb.utils.contains('ENABLE_PARTITIONS_IMAGE', '1', '${ST_PARTITIONS_IMAGES}', 'rootfs', d)}"

# Define image to use for extra partitions
STM32MP_BOOTFS_TYPE       ?= "${@bb.utils.contains('MACHINE_FEATURES', 'efi', 'ESP', 'System', d)}"
STM32MP_BOOTFS_IMAGE      ?= "${IMAGE_BASENAME}"
STM32MP_BOOTFS_LABEL      ?= "bootfs"
STM32MP_BOOTFS_MOUNTPOINT ?= "/boot"
# Proposed value for bootfs is 64MB
STM32MP_BOOTFS_SIZE      ?= "65536"
STM32MP_NOT_SUPPORTED_FS_VFAT ?= "vfat"

STM32MP_ROOTFS_IMAGE ?= "${IMAGE_BASENAME}"

STM32MP_ROOTFS_LABEL ?= "rootfs"
# Configure the rootfs size (total_size of storage - bootfs - vendorfs - userfs = total size - 336M)
# limit to size for emmc (4GB)
STM32MP_ROOTFS_SIZE  ?= "3145728"

STM32MP_USERFS_IMAGE      ?= "${IMAGE_BASENAME}"
STM32MP_USERFS_LABEL      ?= "userfs"
STM32MP_USERFS_MOUNTPOINT ?= "/usr/local"
# Proposed value for userfs is 128MB
STM32MP_USERFS_SIZE       ?= "131072"

STM32MP_VENDORFS_IMAGE      ?= "${IMAGE_BASENAME}"
STM32MP_VENDORFS_LABEL      ?= "vendorfs"
STM32MP_VENDORFS_MOUNTPOINT ?= "/vendor"
# Proposed value for vendorfs is 16MB
STM32MP_VENDORFS_SIZE       ?= "16384"
# Override vendorfs size to 48MB
STM32MP_VENDORFS_SIZE:stm32mp2common ?= "49152"

# limit size of rootfs
STM32MP_ROOTFS_SIZE:emmc ?= "3145728"

# <image_name>,<partition_label>,<mountpoint>,<size>,<type>,<copy>,fs not supported
PARTITIONS_IMAGES[bootfs]   ?= "${STM32MP_BOOTFS_IMAGE},${STM32MP_BOOTFS_LABEL},${STM32MP_BOOTFS_MOUNTPOINT},${STM32MP_BOOTFS_SIZE},${STM32MP_BOOTFS_TYPE},0"
PARTITIONS_IMAGES[vendorfs] ?= "${STM32MP_VENDORFS_IMAGE},${STM32MP_VENDORFS_LABEL},${STM32MP_VENDORFS_MOUNTPOINT},${STM32MP_VENDORFS_SIZE},FileSystem,0,${STM32MP_NOT_SUPPORTED_FS_VFAT}"
PARTITIONS_IMAGES[rootfs]   ?= "${STM32MP_ROOTFS_IMAGE},${STM32MP_ROOTFS_LABEL},,${STM32MP_ROOTFS_SIZE},FileSystem,0,${STM32MP_NOT_SUPPORTED_FS_VFAT}"
PARTITIONS_IMAGES[userfs]   ?= "${STM32MP_USERFS_IMAGE},${STM32MP_USERFS_LABEL},${STM32MP_USERFS_MOUNTPOINT},${STM32MP_USERFS_SIZE},FileSystem,0,${STM32MP_NOT_SUPPORTED_FS_VFAT}"

# For efi partition which are using fat
EXTRA_IMAGECMD:vfat = "-v -S 512 -F 32 "

PARTITIONS_SUFFIX ?= "ext4"

# =========================================================================
# Image partition configuration for ubi
# =========================================================================
STM32MP_UBIFS_MULTIVOLUME_SUFFIX ?= "${@bb.utils.contains('ENABLE_PARTITIONS_IMAGE', '1', 'multivolume_splitted', 'multivolume', d)}"
STM32MP_UBIFS_DATA ?= "${IMAGE_LINK_NAME}_<CONFIG>_${STM32MP_UBIFS_MULTIVOLUME_SUFFIX}.ubi"
STM32MP_UBIFS_NAME ?= "ubifs"
STM32MP_UBIFS_SIZE ?= ""

# Partitions configuration
PARTITIONS_CONFIG += "${@bb.utils.contains_any('BOOTDEVICE_LABELS', 'emmc nor-emmc nor-nor-emmc', 'emmc', '', d)}"
PARTITIONS_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-2-128-256', 'nand-2-128-256', '', d)}"
PARTITIONS_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-4-256-512', 'nand-4-256-512', '', d)}"
PARTITIONS_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-4-256-1024', 'nand-4-256-1024', '', d)}"
PARTITIONS_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-custom', 'nand-custom', '', d)}"
PARTITIONS_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nor', 'nor', '', d)}"
PARTITIONS_CONFIG += "${@bb.utils.contains_any('BOOTDEVICE_LABELS', 'sdcard nor-sdcard nor-nor-sdcard', 'sdcard', d)}"

# <binary_name>,<partlabel>,<size>,<type>,<copy>
PARTITIONS_CONFIG[emmc] ?= "\
    ${@' '.join(['%s' % partImage2partConfig(d, config, '${PARTITIONS_SUFFIX}') for config in d.getVar('PARTITIONS_IMAGES').split()])} \
    "
PARTITIONS_CONFIG[nand-2-128-256] ?= "\
    ${STM32MP_UBIFS_DATA},${STM32MP_UBIFS_NAME},${STM32MP_UBIFS_SIZE},System,1 \
    "
PARTITIONS_CONFIG[nand-4-256-512] ?= "\
    ${STM32MP_UBIFS_DATA},${STM32MP_UBIFS_NAME},${STM32MP_UBIFS_SIZE},System,1 \
    "
PARTITIONS_CONFIG[nand-4-256-1024] ?= "\
    ${STM32MP_UBIFS_DATA},${STM32MP_UBIFS_NAME},${STM32MP_UBIFS_SIZE},System,1 \
    "
PARTITIONS_CONFIG[nand-custom] ?= "\
    ${STM32MP_UBIFS_DATA},${STM32MP_UBIFS_NAME},${STM32MP_UBIFS_SIZE},System,1 \
    "
PARTITIONS_CONFIG[nor] ?= "\
    ${@' '.join(['%s' % partImage2partConfig(d, config, '${PARTITIONS_SUFFIX}') for config in d.getVar('PARTITIONS_IMAGES').split()])} \
    "
PARTITIONS_CONFIG[sdcard] ?= "\
    ${@' '.join(['%s' % partImage2partConfig(d, config, '${PARTITIONS_SUFFIX}') for config in d.getVar('PARTITIONS_IMAGES').split()])} \
    "
# =========================================================================
# UBI multi-volume configuration : data, label, size (Kbytes), [empty]
# =========================================================================
# Define UBI volume label to use in kernel command line to mount UBI file system
UBI_VOLNAME ?= "rootfs"
# Manage specific override for UBI bootfs label name to 'boot' (hardcoded on U-Boot)
UBI_VOLNAME:pn-${STM32MP_BOOTFS_IMAGE} = "boot"

# Define two empty volumes to manage U-Boot config beginning of multivolume UBIFS
STM32MP_MULTIUBI_UENV1_NAME ?= "U-Boot-env-1"
STM32MP_MULTIUBI_UENV1_LABEL = "uboot_config"
STM32MP_MULTIUBI_UENV1_SIZE ?= "256"
STM32MP_MULTIUBI_UENV2_NAME ?= "U-Boot-env-2"
STM32MP_MULTIUBI_UENV2_LABEL = "uboot_config_r"
STM32MP_MULTIUBI_UENV2_SIZE ?= "${STM32MP_MULTIUBI_UENV1_SIZE}"

# limit size of rootfs
STM32MP_ROOTFS_SIZE:nand_4_256_1024 ?= "704512"
STM32MP_ROOTFS_SIZE:nand_4_256_512 ?= "414000"
STM32MP_ROOTFS_SIZE:nand_2_128_256 ?= "210000"

STM32MP_UBI_VOLUME_nand_4_256_1024 ?= "\
    ${STM32MP_MULTIUBI_UENV1_NAME},${STM32MP_MULTIUBI_UENV1_LABEL},${STM32MP_MULTIUBI_UENV1_SIZE},empty \
    ${STM32MP_MULTIUBI_UENV2_NAME},${STM32MP_MULTIUBI_UENV2_LABEL},${STM32MP_MULTIUBI_UENV2_SIZE},empty \
    ${@' '.join(['%s' % partImage2partConfig(d, config) for config in d.getVar('PARTITIONS_IMAGES').split()])} \
    "
STM32MP_UBI_VOLUME_nand_4_256_512 ?= "\
    ${STM32MP_MULTIUBI_UENV1_NAME},${STM32MP_MULTIUBI_UENV1_LABEL},${STM32MP_MULTIUBI_UENV1_SIZE},empty \
    ${STM32MP_MULTIUBI_UENV2_NAME},${STM32MP_MULTIUBI_UENV2_LABEL},${STM32MP_MULTIUBI_UENV2_SIZE},empty \
    ${@' '.join(['%s' % partImage2partConfig(d, config) for config in d.getVar('PARTITIONS_IMAGES').split()])} \
    "
STM32MP_UBI_VOLUME_nand_2_128_256 ?= "\
    ${STM32MP_MULTIUBI_UENV1_NAME},${STM32MP_MULTIUBI_UENV1_LABEL},${STM32MP_MULTIUBI_UENV1_SIZE},empty \
    ${STM32MP_MULTIUBI_UENV2_NAME},${STM32MP_MULTIUBI_UENV2_LABEL},${STM32MP_MULTIUBI_UENV2_SIZE},empty \
    ${@' '.join(['%s' % partImage2partConfig(d, config) for config in d.getVar('PARTITIONS_IMAGES').split()])} \
    "
STM32MP_UBI_VOLUME_nand_custom ?= "\
    ${STM32MP_MULTIUBI_UENV1_NAME},${STM32MP_MULTIUBI_UENV1_LABEL},${STM32MP_MULTIUBI_UENV1_SIZE},empty \
    ${STM32MP_MULTIUBI_UENV2_NAME},${STM32MP_MULTIUBI_UENV2_LABEL},${STM32MP_MULTIUBI_UENV2_SIZE},empty \
    ${@' '.join(['%s' % partImage2partConfig(d, config) for config in d.getVar('PARTITIONS_IMAGES').split()])} \
    "

# example of nand mapping for UBI_VOLUME
# STM32MP_UBI_VOLUME_nand_4_256_512 = "\
#    ${STM32MP_MULTIUBI_UENV1_NAME},${STM32MP_MULTIUBI_UENV1_LABEL},${STM32MP_MULTIUBI_UENV1_SIZE},empty \
#    ${STM32MP_MULTIUBI_UENV2_NAME},${STM32MP_MULTIUBI_UENV2_LABEL},${STM32MP_MULTIUBI_UENV2_SIZE},empty \
#    my-bootfs-image-${DISTRO}-${MACHINE}.bootfs,${STM32MP_BOOTFS_LABEL},${STM32MP_BOOTFS_SIZE},System \
#    ${STM32MP_ROOTFS_IMAGE}-${MACHINE}.${@bb.utils.contains('ENABLE_PARTITIONS_IMAGE', '1', 'splitted-', '', d)}vendorfs,${STM32MP_VENDORFS_LABEL},${STM32MP_VENDORFS_SIZE},FileSystem \
#    ${STM32MP_ROOTFS_IMAGE}-${MACHINE}.${@bb.utils.contains('ENABLE_PARTITIONS_IMAGE', '1', 'splitted-', '', d)}rootfs,${STM32MP_ROOTFS_LABEL},${STM32MP_ROOTFS_SIZE},FileSystem \
#    "
#    WARNING: for the addons image, you need to use the full name of image: my-bootfs-image-${DISTRO}-${MACHINE}.bootfs
#    while for all image associated to rootfs, you need to customize (if distro is already present or not IMAGE_BASENAME): ${STM32MP_ROOTFS_IMAGE}-${MACHINE}.splitted-rootfs

