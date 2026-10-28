#@DESCRIPTION: STM32MP machine storage device configuration

# -----------------------------------------------------------------------------
# Define device storage name and type mapping
# -----------------------------------------------------------------------------
DEVICE_STORAGE ?= "EMMC:mmc1, NAND:nand0, NOR:nor0, SDCARD:mmc0, SPINAND:spi-nand0"
DEVICE_STORAGE:stm32mp1common ?= "EMMC:mmc1, NAND:nand0, NOR:nor0, SDCARD:mmc0, SPINAND:spi-nand0"
DEVICE_STORAGE:stm32mp2common ?= "EMMC:mmc1, NAND:nand0, NOR:nor1, SDCARD:mmc0, SPINAND:spi-nand0"

# -----------------------------------------------------------------------------
# Define device storage name alias
# -----------------------------------------------------------------------------
DEVICE_STORAGE_NAMES += "EMMC"
DEVICE_STORAGE_NAMES += "NAND"
DEVICE_STORAGE_NAMES += "NOR"
DEVICE_STORAGE_NAMES += "SDCARD"
DEVICE_STORAGE_NAMES += "SPINAND"

# -----------------------------------------------------------------------------
# Define device storage type
# -----------------------------------------------------------------------------
DEVICE_STORAGE_TYPES += "mmc0"
DEVICE_STORAGE_TYPES += "mmc1"
DEVICE_STORAGE_TYPES += "mmc2"
DEVICE_STORAGE_TYPES += "nand0"
DEVICE_STORAGE_TYPES += "nor0"
DEVICE_STORAGE_TYPES += "spi-nand0"
DEVICE_STORAGE_TYPES += "nor1"

python () {
    """
    This function configure dynamically the needed alias between generic storage
    device name and device type.
    Output for this function is the initialization of:
        DEVICE:<device_name> = '<device_type>'
        DEVICE:<device_type> = '<device_name>'
    NB: we may have multiple <device_name> that refer to same <device_type>,
        but the opposite is not allowed yet
    """
    device_storage_config = d.getVar('DEVICE_STORAGE') or ''
    if not device_storage_config:
        return
    if not device_storage_config.strip():
        return
    if len(device_storage_config.split(',')) > 0:
        # Init supported device storage configuration
        supported_device_names = (d.getVar('DEVICE_STORAGE_NAMES') or '').split()
        supported_device_types = (d.getVar('DEVICE_STORAGE_TYPES') or '').split()
        # Loop for all storage device configurations
        for device_storage in device_storage_config.split(','):
            device_name = device_storage.split(':')[0].strip()
            device_type = device_storage.split(':')[1].strip()
            # Check for configuration consistency
            if device_name and device_type:
                # Make sure configuration is correctly done
                if device_name not in supported_device_names:
                    bb.fatal('Wrong DEVICE_STORAGE configuration: "%s" is not part of supported device name (%s)' % (device_name, supported_device_names))
                if device_type not in supported_device_types:
                    bb.fatal('Wrong DEVICE_STORAGE configuration: "%s" is not part of supported device type (%s)' % (device_type, supported_device_types))
                # Configure alias
                d.setVar('DEVICE:%s' % device_name, device_type)
                if d.getVar('DEVICE:%s' % device_type):
                    d.appendVar('DEVICE:%s' % device_type, ' ' + device_name)
                else:
                    d.setVar('DEVICE:%s' % device_type, device_name)
            else:
                bb.fatal('Wrong DEVICE_STORAGE configuration: expecting DEVICE_STORAGE = "<device_name1>:<devie_type1>,<device_name2>:<device_type2>"')
}

# -----------------------------------------------------------------------------
# EMMC
# Extra space is required to store 'Protective MBR' and 'Primary GPT Header'
# Currently the required size is 17 KiB (i.e. 0x4400)
# We need to align this size to get the first offset to use
DEVICE_START_OFFSET:EMMC    ?= "0x00080000"
# 32 Gbit
DEVICE_MAX_OFFSET:EMMC      ?= "0x100000000"
DEVICE_ALIGNMENT_SIZE:EMMC  ?= "0x00080000"

# Specific to EMMC, there are two boot partitions using specific offset to access
DEVICE_START_OFFSET_BOOT0:EMMC ?= "boot1"
DEVICE_START_OFFSET_BOOT1:EMMC ?= "boot2"

# Configure the rootfs partition uid used in gpt partition table for EMMC
# rootfs
DEVICE_PARTUUID_ROOTFS:EMMC ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_ROOTFS:${DEVICE:EMMC}')) or ''}"
DEVICE_PARTUUID_ROOTFS_A:EMMC ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_ROOTFS_A:${DEVICE:EMMC}')) or ''}"
DEVICE_PARTUUID_ROOTFS_B:EMMC ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_ROOTFS_B:${DEVICE:EMMC}')) or ''}"
# bootfs
DEVICE_PARTUUID_BOOTFS:EMMC ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_BOOTFS:${DEVICE:EMMC}')) or ''}"
DEVICE_PARTUUID_BOOTFS_A:EMMC ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_BOOTFS_A:${DEVICE:EMMC}')) or ''}"
DEVICE_PARTUUID_BOOTFS_B:EMMC ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_BOOTFS_B:${DEVICE:EMMC}')) or ''}"
# vendorfs
DEVICE_PARTUUID_VENDORFS:EMMC ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_VENDORFS:${DEVICE:EMMC}')) or ''}"
DEVICE_PARTUUID_VENDORFS_A:EMMC ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_VENDORFS_A:${DEVICE:EMMC}')) or ''}"
DEVICE_PARTUUID_VENDORFS_B:EMMC ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_VENDORFS_B:${DEVICE:EMMC}')) or ''}"
# userfs (not mandatory)
DEVICE_PARTUUID_USERFS:EMMC ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_USERFS:${DEVICE:EMMC}')) or ''}"
DEVICE_PARTUUID_USERFS_A:EMMC ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_USERFS_A:${DEVICE:EMMC}')) or ''}"
DEVICE_PARTUUID_USERFS_B:EMMC ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_USERFS_B:${DEVICE:EMMC}')) or ''}"

# -----------------------------------------------------------------------------
# NAND
DEVICE_START_OFFSET:NAND   ?= "0x00000000"
# 8 Gbit
DEVICE_MAX_OFFSET:NAND     ?= "0x40000000"
DEVICE_ALIGNMENT_SIZE:NAND ?= "0x00040000"

# -----------------------------------------------------------------------------
# NOR
DEVICE_START_OFFSET:NOR    ?= "0x00000000"
# 512 Mbit, only 1 NOR is used
DEVICE_MAX_OFFSET:NOR      ?= "0x04000000"
DEVICE_ALIGNMENT_SIZE:NOR  ?= "0x00010000"

# Configure the rootfs partition uid used in gpt partition table for NOR
DEVICE_PARTUUID_ROOTFS:NOR ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_ROOTFS:${DEVICE:NOR}')) or ''}"

# -----------------------------------------------------------------------------
# SDCARD
# Extra space is required to store 'Protective MBR' and 'Primary GPT Header'
# Currently the required size is 17 KiB (i.e. 0x4400)
# We need to align this size to get the first offset to use
DEVICE_START_OFFSET:SDCARD    ?= "0x00004400"
# No limit for SDCARD
DEVICE_MAX_OFFSET:SDCARD      ?= "none"
DEVICE_ALIGNMENT_SIZE:SDCARD  ?= "0x00000200"

# Configure the rootfs partition uid used in gpt partition table for SDCARD
# rootfs
DEVICE_PARTUUID_ROOTFS:SDCARD ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_ROOTFS:${DEVICE:SDCARD}')) or ''}"
DEVICE_PARTUUID_ROOTFS_A:SDCARD ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_ROOTFS_A:${DEVICE:SDCARD}')) or ''}"
DEVICE_PARTUUID_ROOTFS_B:SDCARD ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_ROOTFS_B:${DEVICE:SDCARD}')) or ''}"
# bootfs
DEVICE_PARTUUID_BOOTFS:SDCARD ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_BOOTFS:${DEVICE:SDCARD}')) or ''}"
DEVICE_PARTUUID_BOOTFS_A:SDCARD ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_BOOTFS_A:${DEVICE:SDCARD}')) or ''}"
DEVICE_PARTUUID_BOOTFS_B:SDCARD ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_BOOTFS_B:${DEVICE:SDCARD}')) or ''}"
# vendorfs
DEVICE_PARTUUID_VENDORFS:SDCARD ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_VENDORFS:${DEVICE:SDCARD}')) or ''}"
DEVICE_PARTUUID_VENDORFS_A:SDCARD ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_VENDORFS_A:${DEVICE:SDCARD}')) or ''}"
DEVICE_PARTUUID_VENDORFS_B:SDCARD ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_VENDORFS_B:${DEVICE:SDCARD}')) or ''}"
# userfs (not mandatory)
DEVICE_PARTUUID_USERFS:SDCARD ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_USERFS:${DEVICE:SDCARD}')) or ''}"
DEVICE_PARTUUID_USERFS_A:SDCARD ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_USERFS_A:${DEVICE:SDCARD}')) or ''}"
DEVICE_PARTUUID_USERFS_B:SDCARD ?= "${@d.getVar(d.expand('DEVICE_PARTUUID_USERFS_B:${DEVICE:SDCARD}')) or ''}"

# -----------------------------------------------------------------------------
# SPI NAND
DEVICE_START_OFFSET:SPINAND   ?= "0x00000000"
# 2 Gbit
DEVICE_MAX_OFFSET:SPINAND     ?= "0x10000000"
DEVICE_ALIGNMENT_SIZE:SPINAND ?= "0x00020000"

# -----------------------------------------------------------------------------
# Fixed configuration from U-Boot source code
# -----------------------------------------------------------------------------
# fip
DEVICE_TYPEUUID_FIP = "19d5df83-11b0-457b-be2c-7559c13142a5"
DEVICE_PARTUUID_FIP_A = "4fd84c93-54ef-463f-a7ef-ae25ff887087"
DEVICE_PARTUUID_FIP_B = "09c54952-d5bf-45af-acee-335303766fb3"
# fwu mdata
DEVICE_TYPEUUID_FWU_MDATA = "8a7a84a0-8387-40f6-ab41-a8b9a5a60d23"
DEVICE_PARTUUID_FWU_MDATA_1 = "5f91c128-e120-48d7-bb64-9fa9aed4c7e3"
DEVICE_PARTUUID_FWU_MDATA_2 = "f469f981-5985-4206-8fb3-839956035a65"
# u-boot-env
DEVICE_TYPEUUID_UBOOT_ENV = "3de21764-95bd-54bd-a5c3-4abe786f38a8"
# bootfs
DEVICE_METADATA_TYPEUUID_BOOTFS = "8da7c9d3-d368-487b-b28b-71e1b0c68cf0"
DEVICE_TYPEUUID_BOOTFS = "0fc63daf-8483-4772-8e79-3d69d8477de4"
DEVICE_PARTUUID_BOOTFS:mmc0 ?= "c9b01b31-b604-4c2a-95b4-e23ab45fd0c5"
DEVICE_PARTUUID_BOOTFS_A:mmc0 ?= "${DEVICE_PARTUUID_BOOTFS:mmc0}"
DEVICE_PARTUUID_BOOTFS_B:mmc0 ?= "67a73cea-322c-42c0-b477-6ef0039fefed"
DEVICE_PARTUUID_BOOTFS:mmc1 ?= "0ef36fac-9ba4-4e39-823a-bbb2336c32be"
DEVICE_PARTUUID_BOOTFS_A:mmc1 ?= "${DEVICE_PARTUUID_BOOTFS:mmc1}"
DEVICE_PARTUUID_BOOTFS_B:mmc1 ?= "f2c429f5-9000-4201-aa0f-96a95ea8d9f9"
DEVICE_PARTUUID_BOOTFS:mmc2 ?= "697aaa78-8107-11f1-ab07-0242ac120002"
DEVICE_PARTUUID_BOOTFS_A:mmc2 ?= "${DEVICE_PARTUUID_BOOTFS:mmc2}"
DEVICE_PARTUUID_BOOTFS_B:mmc2 ?= "0ccbafb6-042c-4b7f-9668-73242c4c7257"
DEVICE_PARTUUID_BOOTFS:nor1 ?= ""
DEVICE_PARTUUID_BOOTFS:nor2 ?= ""
# vendorfs
DEVICE_METADATA_TYPEUUID_VENDORFS = "3f427af9-e80f-4811-bd33-29aaf35a28f8"
DEVICE_TYPEUUID_VENDORFS = "0fc63daf-8483-4772-8e79-3d69d8477de4"
DEVICE_PARTUUID_VENDORFS:mmc0 ?= "8abdaae5-d940-463a-8326-a2c6af788fe6"
DEVICE_PARTUUID_VENDORFS_A:mmc0 ?= "${DEVICE_PARTUUID_VENDORFS:mmc0}"
DEVICE_PARTUUID_VENDORFS_B:mmc0 ?= "d168dbce-65f3-423c-86af-a11528890b30"
DEVICE_PARTUUID_VENDORFS:mmc1 ?= "9fdbbc13-65d3-4419-9395-88c694a4ea79"
DEVICE_PARTUUID_VENDORFS_A:mmc1 ?= "${DEVICE_PARTUUID_VENDORFS:mmc1}"
DEVICE_PARTUUID_VENDORFS_B:mmc1 ?= "4752ba24-2e0d-45b9-9c42-0028ece6e74b"
DEVICE_PARTUUID_VENDORFS:mmc2 ?= "292056ed-fa8b-4550-94e9-9ae5759042d8"
DEVICE_PARTUUID_VENDORFS_A:mmc2 ?= "${DEVICE_PARTUUID_VENDORFS:mmc2}"
DEVICE_PARTUUID_VENDORFS_B:mmc2 ?= "69a81924-2e4a-4fe3-862a-5c097996f059"
DEVICE_PARTUUID_VENDORFS:nor1 ?= ""
DEVICE_PARTUUID_VENDORFS:nor2 ?= ""
# rootfs
DEVICE_METADATA_TYPEUUID_ROOTFS = "b28961f8-c72c-4088-bbdb-c785861473fa"
DEVICE_TYPEUUID_ROOTFS = "0fc63daf-8483-4772-8e79-3d69d8477de4"
DEVICE_PARTUUID_ROOTFS:mmc0 ?= "e91c4e10-16e6-4c0e-bd0e-77becf4a3582"
DEVICE_PARTUUID_ROOTFS_A:mmc0 ?= "${DEVICE_PARTUUID_ROOTFS:mmc0}"
DEVICE_PARTUUID_ROOTFS_B:mmc0 ?= "360db83f-c038-4e10-ae79-1dce8846d2f0"
DEVICE_PARTUUID_ROOTFS:mmc1 ?= "491f6117-415d-4f53-88c9-6e0de54deac6"
DEVICE_PARTUUID_ROOTFS_A:mmc1 ?= "${DEVICE_PARTUUID_ROOTFS:mmc1}"
DEVICE_PARTUUID_ROOTFS_B:mmc1 ?= "6e01295a-8f81-47d5-945c-cb144ae7500c"
DEVICE_PARTUUID_ROOTFS:mmc2 ?= "fd58f1c7-be0d-4338-8ee9-ad8f050aeb18"
DEVICE_PARTUUID_ROOTFS_A:mmc2 ?= "${DEVICE_PARTUUID_ROOTFS:mmc2}"
DEVICE_PARTUUID_ROOTFS_B:mmc2 ?= "49A88364-AF1A-467A-A049-269F94122EE9"
DEVICE_PARTUUID_ROOTFS:nor0 ?= ""
DEVICE_PARTUUID_ROOTFS:nor1 ?= ""
# userfs
DEVICE_METADATA_TYPEUUID_USERFS = "bf9b43d9-fcb3-4253-8297-afd9c68df1ef"
DEVICE_TYPEUUID_USERFS = "0fc63daf-8483-4772-8e79-3d69d8477de4"
DEVICE_PARTUUID_USERFS:mmc0 ?= "2d750516-056c-4fae-9d73-f3141f1cffb4"
DEVICE_PARTUUID_USERFS_A:mmc0 ?= "${DEVICE_PARTUUID_USERFS:mmc0}"
DEVICE_PARTUUID_USERFS_B:mmc0 ?= "7e036063-979d-4000-949d-33ab3e987de0"
DEVICE_PARTUUID_USERFS:mmc1 ?= "e873e5be-10d5-4670-b8f1-2905b5e7d992"
DEVICE_PARTUUID_USERFS_A:mmc1 ?= "${DEVICE_PARTUUID_USERFS:mmc1}"
DEVICE_PARTUUID_USERFS_B:mmc1 ?= "445a0e58-aa95-4e75-8c4f-ff603dddb60f"
DEVICE_PARTUUID_USERFS:mmc2 ?= "0bad8ae4-5321-4f39-8f82-41f8cbcda3c0"
DEVICE_PARTUUID_USERFS_A:mmc2 ?= "${DEVICE_PARTUUID_USERFS:mmc2}"
DEVICE_PARTUUID_USERFS_B:mmc2 ?= "152401bc-3c21-4fdd-9155-c0c5c4e66600"
DEVICE_PARTUUID_USERFS:nor1 ?= ""
DEVICE_PARTUUID_USERFS:nor2 ?= ""
