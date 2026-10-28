#!/bin/bash - 
#===============================================================================
#  ORGANIZATION: STMicroelectronics
#     COPYRIGHT: Copyright (C) 2025, STMicroelectronics - All Rights Reserved
#       CREATED: 03/19/2025 16:57
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

command -v realpath > /dev/null 2>&1 || { echo "[ERROR] Missing 'realpath' on HOST!\nPlease install it before running the script again.\n" ; exit 1 ; }

########################################################
# local variable for generation script
MACHINE=
MX=0
SECURITY=0
MX_NAME=""
OUTPUT_SCRIPT=
COMPONENTS=
COMPONENTS_FIP=
COMPONENTS_M33FW=

#######################################################
########################################################
#           common
function config_stm32mp_debug() {
    echo ""
    echo "# ----------------------------"
    echo "# Component configuration"
    echo "# ----------------------------"
    echo ""
    echo "PARALLEL_MAKE="
    echo "# parallel make argument:"
    echo "# ex.: "
    echo "# PARALLEL_MAKE=-j8"
    echo ""
    echo "# ----------------------------"
    echo "# DEBUG/LOG parameters"
    echo "#"
    echo "# TF-A"
    echo "# TF_A_DEBUG: activate debug log"
    echo "# TF_A_LOG_LEVEL: level of log (Preconize 20 by default and 40 for debug)"
    echo "# (uncomment and/or adapt to your need)"
    echo "# export TF_A_DEBUG=0"
    echo "# export TF_A_LOG_LEVEL=20"
    echo ""
    echo "# ----------------------------"
    echo "# OPTEE-OS"
    echo "# ----------------------------"
    echo "# DEBUG/LOG parameters"
    echo "#"
    echo "# OPTEE-OS"
    echo "# OPTEE_DEBUG_CORE: activate debug core"
    echo "#   For stm32mp15, OPTEE_DEBUG_CORE MUST be desactivated to save space on memory"
    echo "# OPTEE_LOG_LEVEL: set log  level at runtime (Preconize 2 by default)"
    echo "#    0: Minimum level"
    echo "#    1: Error level"
    echo "#    2: Info level"
    echo "#    3: DEBUG level"
    echo "#    4: Flow level (used to print the execution flox, typically the in/out of functions)"
    echo "# (comment and/or adapt to your need)"
    echo "export OPTEE_DEBUG_CORE=y"
    echo "export OPTEE_LOG_LEVEL=2"
    echo ""
    echo "# ----------------------------"
    echo "# U-BOOT"
    echo "# ----------------------------"
    echo "# Configuration"
    echo "# (list of fragments with relative path to source code)"
    echo "# fragments available:"
    echo "#   ../fragment-02-signature.fragment : For FIT usage, activate fit and signature"
    echo "#   ../fragment-03-efi.fragment : For EFI usage, active support of load of EFI firwmare"
    echo "#   ../fragment-06-m33td-no-splashscreen.fragment: For M33TD, permit to not activate splashscreen"
    echo "#   ../fragment-07-firmware-update-ab.fragment : For firmware AB, adapt number of image available"
    echo "# export UBOOT_FRAGMENT_LIST=\"\""
    echo "# ex.: For firmware update AB you MUST use the fragment: fragment-07-firmware-update-ab.fragment"
    echo "#    export UBOOT_FRAGMENT_LIST=\"../fragment-07-firmware-update-ab.fragment\""
    echo "# export UBOOT_PROGRAMMER_FRAGMENT_LIST=\"\""
    echo ""
    echo "# ----------------------------"
    echo "# KERNEL"
    echo "# ----------------------------"
    echo "# Build parameters"
    echo "# Verbose parameter to pass to Makefile"
    echo "# (uncomment and/or adapt to your need)"
    echo "# export VERBOSE_PARAM=\"V=1\""
    echo ""
    echo "# Configuration"
    echo "# (list of fragments with relative path to source code)"
    echo "# List of fragments available:"
    echo "#     fragment-03-systemd.config : configuration for systemd usage"
    echo "#     fragment-04-modules.config : addons st configuration"
    echo "# List of optional fragment available:"
    echo "#     optional-fragment-05-signature.config : configuration to use signed modules"
    echo "#     optional-fragment-06-nosmp.config : configuration to disable smp"
    echo "#     optional-fragment-07-efi.config: configuration to enable EFI functionality"
    echo "#     optional-fragment-07-efi-stm32mp1.config : specific addons cleanup for efi configuration for stm32mp1"
    echo "#     optional-fragment-08-disable-etnaviv.config: configuration to disable etnaviv"
    echo "export KERNEL_FRAGMENT_LIST=\"../fragment-03-systemd.config ../fragment-04-modules.config\""
    echo ""
}

function config_stm32mp2_m33td_debug() {
    echo "# ----------------------------"
    echo "# TF-M"
    echo "# ----------------------------"
    echo "# TF-M profile:"
    echo "# Profile Small: Designed for ultra-constrained devices with a minimal memory footprint;"
    echo "# uses isolation level 1 (SFN backend) and requires symmetric cryptography (like TLS-PSK)."
    echo "# export TF_M_PROFILE=profile_small"
    echo "#"
    echo "# Profile Medium: Targets standard IoT requirements with isolation level 2 (IPC) and adds"
    echo "# features like Protected Storage alongside Crypto, ITS, and Attestation."
    echo "# export TF_M_PROFILE=profile_medium"
    echo ""
}

function config_security_restriction() {
    echo "##################################################################"
    echo "##################################################################"
    echo "### Signature / Encryption configuration"
    echo "##################################################################"
    echo "##################################################################"
    echo "if [ \${SIGN_ENABLE} -eq 1 ]; then"
    echo "      export TF_A_DEBUG=0"
    echo "      export TF_A_LOG_LEVEL=20"
    echo "fi"
    echo "##################################################################"
    echo "##################################################################"
    echo "### RESTRICTION of Signature / Encryption"
    echo "##################################################################"
    echo "##################################################################"
    echo ""
}

function config_firmware_update() {
    echo "# ----------------------------"
    echo "# Firmware update"
    echo "# ----------------------------"
    echo "# Firmware update is activated by default on component configuration"
    echo ""
    echo "# There is several set of configuration for firmware update:"
    echo "# 1: use the firmware update only on fip image"
    echo "# 2: use the firmware update for fip and several linux fs (like bootfs, rootfs, ...)"
    echo "# To use the second configuration for linux fs support with firmware update, you need to activate"
    echo "# the following variable:"
    echo "# export FIRMWARE_UPDATE_AB=1"
    echo ""
    echo "# Metadata configuration:"
    echo "# A specific configuration can by use for generating metadata firmware via TF-A Makefile:"
    echo "# METADATA_IMAGE_NUMBER: number of image described on metadata"
    echo "# METADATA_NAME: name of metadata firmware generated"
    echo "# METADATA_TOOL_ARGS: configuration of metadata"
    echo "#   METADATA_TOOL_ARGS=\" -g -i <number of image> -b <number of bank> -v <metadata version: 1 or 2> \\"
    echo "#       <uuid image>,<uuid type of image>, <gpt uuid imge A>,<gpt uuid image B>  # for first image, can be repeated for each image \\"
    echo "#       .... \\"
    echo "#       \""
    echo "export METADATA_IMAGE_NUMBER=1"
    echo ""
    echo "# basic configuration of metadata which manage only fip image:"
    echo "# export METADATA_IMAGE_NUMBER=1"
    echo "# export METADATA_NAME=\"metadata_fip.bin\""
    echo "# export METADATA_TOOL_ARGS=\"-g -i 1 -b 2 -v 2 \\"
    echo "#    8a7a84a0-8387-40f6-ab41-a8b9a5a60d23,19d5df83-11b0-457b-be2c-7559c13142a5,4fd84c93-54ef-463f-a7ef-ae25ff887087,09c54952-d5bf-45af-acee-335303766fb3\""
    echo ""
    echo "# configuration of metadata which fip, bootfs and rootfs image on sdcard:"
    echo "# export METADATA_IMAGE_NUMBER=3"
    echo "# export METADATA_NAME=\"metadata_fip_bootfs_rootfs_sdcard.bin\""
    echo "# export METADATA_TOOL_ARGS=\"-g -i 3 -b 2 -v 2 \\"
    echo "#    8a7a84a0-8387-40f6-ab41-a8b9a5a60d23,19d5df83-11b0-457b-be2c-7559c13142a5,4fd84c93-54ef-463f-a7ef-ae25ff887087,09c54952-d5bf-45af-acee-335303766fb3 \\"
    echo "#    8a7a84a0-8387-40f6-ab41-a8b9a5a60d23,8da7c9d3-d368-487b-b28b-71e1b0c68cf0,0ef36fac-9ba4-4e39-823a-bbb2336c32be,f2c429f5-9000-4201-aa0f-96a95ea8d9f9 \\"
    echo "#    8a7a84a0-8387-40f6-ab41-a8b9a5a60d23,b28961f8-c72c-4088-bbdb-c785861473fa,491f6117-415d-4f53-88c9-6e0de54deac6,6e01295a-8f81-47d5-945c-cb144ae7500c\""
    echo ""
    echo "# configuration of metadata which fip, bootfs and rootfs image on emmc:"
    echo "# export METADATA_IMAGE_NUMBER=3"
    echo "# export METADATA_NAME=\"metadata_fip_bootfs_rootfs_emmc.bin\""
    echo "# export METADATA_TOOL_ARGS=\"-g -i 3 -b 2 -v 2 \\"
    echo "#    8a7a84a0-8387-40f6-ab41-a8b9a5a60d23,19d5df83-11b0-457b-be2c-7559c13142a5,4fd84c93-54ef-463f-a7ef-ae25ff887087,09c54952-d5bf-45af-acee-335303766fb3 \\"
    echo "#    8a7a84a0-8387-40f6-ab41-a8b9a5a60d23,8da7c9d3-d368-487b-b28b-71e1b0c68cf0,0ef36fac-9ba4-4e39-823a-bbb2336c32be,f2c429f5-9000-4201-aa0f-96a95ea8d9f9 \\"
    echo "#    8a7a84a0-8387-40f6-ab41-a8b9a5a60d23,b28961f8-c72c-4088-bbdb-c785861473fa,491f6117-415d-4f53-88c9-6e0de54deac6,6e01295a-8f81-47d5-945c-cb144ae7500c\""
    echo ""
    echo "# configuration of metadata which fip, bootfs, rootfs, vendorfs and userfs image on sdcard:"
    echo "# export METADATA_IMAGE_NUMBER=3"
    echo "# export METADATA_NAME=\"metadata_fip_bootfs_rootfs_vendorfs_userfs_sdcard.bin\""
    echo "# export METADATA_TOOL_ARGS=\"-g -i 3 -b 2 -v 2 \\"
    echo "#    8a7a84a0-8387-40f6-ab41-a8b9a5a60d23,19d5df83-11b0-457b-be2c-7559c13142a5,4fd84c93-54ef-463f-a7ef-ae25ff887087,09c54952-d5bf-45af-acee-335303766fb3 \\"
    echo "#    8a7a84a0-8387-40f6-ab41-a8b9a5a60d23,8da7c9d3-d368-487b-b28b-71e1b0c68cf0,0ef36fac-9ba4-4e39-823a-bbb2336c32be,f2c429f5-9000-4201-aa0f-96a95ea8d9f9 \\"
    echo "#    8a7a84a0-8387-40f6-ab41-a8b9a5a60d23,b28961f8-c72c-4088-bbdb-c785861473fa,491f6117-415d-4f53-88c9-6e0de54deac6,6e01295a-8f81-47d5-945c-cb144ae7500c \\"
    echo "#    8a7a84a0-8387-40f6-ab41-a8b9a5a60d23,3f427af9-e80f-4811-bd33-29aaf35a28f8,8abdaae5-d940-463a-8326-a2c6af788fe6,d168dbce-65f3-423c-86af-a11528890b30 \\"
    echo "#    8a7a84a0-8387-40f6-ab41-a8b9a5a60d23,bf9b43d9-fcb3-4253-8297-afd9c68df1ef,2d750516-056c-4fae-9d73-f3141f1cffb4,7e036063-979d-4000-949d-33ab3e987de0 \""
    echo ""
    echo "# configuration of metadata which fip, bootfs, rootfs, vendorfs and userfs image on emmc:"
    echo "# export METADATA_IMAGE_NUMBER=3"
    echo "# export METADATA_NAME=\"metadata_fip_bootfs_rootfs_vendorfs_userfs_emmc.bin\""
    echo "# export METADATA_TOOL_ARGS=\"-g -i 3 -b 2 -v 2 \\"
    echo "#    8a7a84a0-8387-40f6-ab41-a8b9a5a60d23,19d5df83-11b0-457b-be2c-7559c13142a5,4fd84c93-54ef-463f-a7ef-ae25ff887087,09c54952-d5bf-45af-acee-335303766fb3 \\"
    echo "#    8a7a84a0-8387-40f6-ab41-a8b9a5a60d23,8da7c9d3-d368-487b-b28b-71e1b0c68cf0,0ef36fac-9ba4-4e39-823a-bbb2336c32be,f2c429f5-9000-4201-aa0f-96a95ea8d9f9 \\"
    echo "#    8a7a84a0-8387-40f6-ab41-a8b9a5a60d23,b28961f8-c72c-4088-bbdb-c785861473fa,491f6117-415d-4f53-88c9-6e0de54deac6,6e01295a-8f81-47d5-945c-cb144ae7500c \\"
    echo "#    8a7a84a0-8387-40f6-ab41-a8b9a5a60d23,3f427af9-e80f-4811-bd33-29aaf35a28f8,9fdbbc13-65d3-4419-9395-88c694a4ea79,4752ba24-2e0d-45b9-9c42-0028ece6e74b \\"
    echo "#    8a7a84a0-8387-40f6-ab41-a8b9a5a60d23,bf9b43d9-fcb3-4253-8297-afd9c68df1ef,e873e5be-10d5-4670-b8f1-2905b5e7d992,445a0e58-aa95-4e75-8c4f-ff603dddb60f\""
}

########################################################
#           STM32MP1
function config_stm32mp1_common() {
    echo "##################################################################"
    echo "##################################################################"
    echo "### General configuration"
    echo "##################################################################"
    echo "##################################################################"

    echo "# Path to SDK environment (where is 'environment-setup-cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi')"
    echo "# Usually: /opt/st/stm32mp1/<X.X.X>-openstlinux-<X.X>-yocto-<version_name>-mpu-v<XX.XX.XX>/"
    echo "sdk_absolute_path=\"<SDK absolute path>\""
    echo "# Name of environment setup script for sdk"
    echo "sdk_env_file=\"environment-setup-cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi\""
    echo ""
    echo "# Configuration for building stm32mp1x"
    echo "your_board_name=\"<Your board name>\""
    echo ""
    echo "# your_soc_name can be stm32mp15 or stm32mp13"
    echo "your_soc_name=\"<Your soc name>\""
    echo ""
    echo "# Type of security can be 'optee' or 'opteemin' ('opteemin' is preconize for stm32mp15)"
    echo "your_storage_boot_scheme_security=\"<your_storage_boot_scheme_security>\""
    echo ""
    echo "# Type of boot scheme storage used for Cortex A7 part, ex.:"
    echo "#    optee-emmc"
    echo "#    optee-sdcard"
    echo "#    optee-nor"
    echo "#    optee-nand"
    echo "#    optee-spinand"
    echo "#    opteemin-emmc"
    echo "#    opteemin-sdcard"
    echo "#    opteemin-nor"
    echo "#    opteemin-nand"
    echo "#    opteemin-spinand"
    echo "your_storage_boot_scheme_cortex_a=\"<your_storage_boot_scheme_cortex_a>\""
    echo ""
    echo "# Location to store all the binaries generated"
    echo "# An absolute path is required"
    echo "your_deploy_dir_path=\"<your_deploy_dir_path>\""
    echo ""
    echo "# Define build subdirectory (by default set to ../build)"
    echo "your_build_subdir_path=\"<your_build_subdir_path>\""
    echo ""
}
function config_stm32mp1_externaldt() {
    echo "# ----------------------------------------------------------------"
    echo "# ----------------------------------------------------------------"
    echo "### External DT configuration"
    echo "# ----------------------------------------------------------------"
    echo "# ----------------------------------------------------------------"
    echo "# External DT  devicetree tree"
    echo "#  <external dt path>/stm32mp1/"
    echo "#    ├── linux"
    echo "#    ├── optee"
    echo "#    ├── tf-a"
    echo "#    └── u-boot"
    echo ""

    echo "# ------------------------------"
    echo "# Via external dt"
    echo "# force external dt path to external-dt env path"
    echo "# (EXTDT_DIR can be changed by overwriting the path on external-dt_set funcion)"
    echo "externaldt_path=\"<external dt path>\""
    echo ""
    echo "# external dt subpath for Optee"
    echo "externaldt_optee_path=stm32mp1/optee"
    echo "externaldt_optee_programmer_path=stm32mp1/optee"
    echo "# external dt U-boot"
    echo "externaldt_uboot_path=stm32mp1/u-boot"
    echo "externaldt_uboot_programmer_path=stm32mp1/u-boot"
    echo "# TF-A"
    echo "externaldt_tfa_path=stm32mp1/tf-a"
    echo "externaldt_tfa_programmer_path=stm32mp1/tf-a"
    echo "# Kernel linux"
    echo "externaldt_linux_path=stm32mp1/linux"
    echo ""
    echo "# Name of devicetree to use"
    echo "# Optee dt name"
    echo "optee_dt_name=\${your_board_name}"
    echo "optee_dt_programmer_name=\${your_board_name}"
    echo "# U-boot dt name"
    echo "uboot_defconfig=\${your_soc_name}_defconfig"
    echo "uboot_dt_name=\${your_board_name}"
    echo "uboot_programmer_dt_name=\${your_board_name}"
    echo "# TF-A dt name"
    echo "tfa_dt_name=\${your_board_name}"
    echo "tfa_dt_programmer_name=\${your_board_name}"
    echo "# Linux kernel dt name"
    echo "linux_dtb_name=\${your_board_name}"
}
function config_stm32mp1_mx() {
    echo "# ----------------------------"
    echo "# CUBE MX Project"
    echo "# ----------------------------"
    echo "# define the cube mx project name"
    echo "# ex.: for <path>/CA7/DeviceTree/cube_mx_project/kernel/<cube mx dt>"
    echo "#      your_cubemx_project_name=cube_mx_project"
    echo "#      externaldt_path=path"
    echo "your_cubemx_project_name=<cubemx project name>"
    echo ""
    echo "# ----------------------------------------------------------------"
    echo "# ----------------------------------------------------------------"
    echo "### CubeMX DT configuration"
    echo "# ----------------------------------------------------------------"
    echo "# ----------------------------------------------------------------"
    echo "# CubeMX devicetree tree"
    echo "#  <CUBMEMX project path>/CA7/DeviceTree/<your_cubemx_project_name>/"
    echo "#    ├── kernel"
    echo "#    ├── optee-os"
    echo "#    ├── tf-a"
    echo "#    └── u-boot"
    echo ""

    echo "# ------------------------------"
    echo "# Via CubeMX dt"
    echo "# force external dt path to CubeMX project path"
    echo "# ex.: externaldt_path=my_project_path"
    echo "externaldt_path=<Path to CubeMX project path>"
    echo ""
    echo "# external dt subpath for Optee"
    echo "externaldt_optee_path=CA7/DeviceTree/\${your_cubemx_project_name}/optee-os"
    echo "externaldt_optee_programmer_path=CA7/DeviceTree/\${your_cubemx_project_name}/optee-os"
    echo "# external dt U-boot"
    echo "externaldt_uboot_path=CA7/DeviceTree/\${your_cubemx_project_name}/u-boot"
    echo "externaldt_uboot_programmer_path=CA7/DeviceTree/\${your_cubemx_project_name}/u-boot"
    echo "# TF-A"
    echo "externaldt_tfa_path=CA7/DeviceTree/\${your_cubemx_project_name}/tf-a"
    echo "externaldt_tfa_programmer_path=CA7/DeviceTree/\${your_cubemx_project_name}/tf-a"
    echo "# Kernel linux"
    echo "externaldt_linux_path=CA7/DeviceTree/\${your_cubemx_project_name}/kernel"
    echo ""
    echo "# Name of devicetree to use"
    echo "# Optee dt name"
    echo "optee_dt_name=\${your_board_name}"
    echo "optee_dt_programmer_name=\${your_board_name}"
    echo "# U-boot dt name"
    echo "uboot_defconfig=\${your_soc_name}_defconfig"
    echo "uboot_dt_name=\${your_board_name}"
    echo "uboot_programmer_dt_name=\${your_board_name}"
    echo "# TF-A dt name"
    echo "tfa_dt_name=\${your_board_name}"
    echo "tfa_dt_programmer_name=\${your_board_name}"
    echo "# Linux kernel dt name"
    echo "linux_dtb_name=\${your_board_name}"
}
function config_stm32mp1_security() {
    echo ""
    echo "##################################################################"
    echo "##################################################################"
    echo "### Signature / Encryption configuration"
    echo "##################################################################"
    echo "##################################################################"
    echo "#"
    echo "#          STM32MP13: CORTEX A7               STM32MP15: CORTEX A7"
    echo "#   ---------------------------------------    ---------------------"
    echo "#   |                   BL2               |   |           BL2      |"
    echo "#   |                 (TF-A)              |   |        (TF-A)      |"
    echo "#   |                                     |   |                    |"
    echo "#   |    verified with  | decrypted with  |   |   verified with    |"
    echo "#   |       PKHTH       |       EDMK      |   |        PKHTH       |"
    echo "#   --------------------------------------    ----------------------"
    echo "#   |                 FIP                 |   |         FIP        |"
    echo "#   |        (FW-CONFIG - TF-A)           |   | (FW-CONFIG - TF-A) |"
    echo "#   |               (optee)               |   |    (optee)         |"
    echo "#   |                                     |   |                    |"
    echo "#   |    verified with  | decrypted with  |   |   verified with    |"
    echo "#   |      PKHTH        |       EDMK      |   |       PKHTH        |"
    echo "#   ---------------------------------------   ---------------------"
    echo "# WARNING: For FIP will be verified (and decrypted) by BL2, you need to compile"
    echo "#   TF-A with specific option for signature support:"
    echo "#     for all platform: TRUSTED_BOARD_BOOT=1 MBEDTLS_DIR=<path to mbedtls source code>"
    echo "#   TF-A with specific option for encryption support:"
    echo "#     for all platform: DECRYPTION_SUPPORT=aes_gcm ENCRYPT_BL32=1 MBEDTLS_DIR=<path to mbedtls source code>"
    echo "#            ENCRYPT_BL31=1"
    echo "#"
    echo "#"
    echo "# Activate signature for BL2 and FIP on cortex A7"
    echo "SIGN_ENABLE=0"
    echo ""
    echo "# Activate Encryption for BL2 and FIP on cortex A7"
    echo "# NOTE: encryption is not supported on stm32mp15 platform"
    echo "ENCRYPT_ENABLE=0"
    echo ""
    echo "# Absolute path to signing tools"
    echo "SIGN_TOOL=\"<absolute path>/STM32CubeProgrammer-x.xx/bin/STM32_SigningTool_CLI\""
    echo ""
    echo "# Directory on which all key are available"
    echo "your_absolute_key_path=<absolute path to key directory>"
    echo ""
    echo "# For naming the output file, use the following suffix:"
    echo "signature_suffix=\"_Signed\""
    echo "encryption_suffix=\"_Encrypted\""
    echo ""
    echo "# ------ Key associated to PKHTH or PKH ---------------------"
    echo "# Private Key for signature (absolute path to key)"
    echo "SIGN_KEY=\"\${your_absolute_key_path}/a7_PKHTH_privateKey00.pem\""
    echo "# List of Public key for signature"
    echo "# NOTE:"
    echo "# for stm32mp15 platform, only one key pair public/private is supported"
    echo "# for stm32mp13 platform, up to 8 key pairs public/private are supported"
    echo "SIGN_KEY_PUB=\"\\"
    echo "    \${your_absolute_key_path}/a7_PKHTH_publicKey00.pem \\"
    echo "    \${your_absolute_key_path}/a7_PKHTH_publicKey01.pem \\"
    echo "    \${your_absolute_key_path}/a7_PKHTH_publicKey02.pem \\"
    echo "    \${your_absolute_key_path}/a7_PKHTH_publicKey03.pem \\"
    echo "    \${your_absolute_key_path}/a7_PKHTH_publicKey04.pem \\"
    echo "    \${your_absolute_key_path}/a7_PKHTH_publicKey05.pem \\"
    echo "    \${your_absolute_key_path}/a7_PKHTH_publicKey06.pem \\"
    echo "    \${your_absolute_key_path}/a7_PKHTH_publicKey07.pem \\"
    echo "\""
    echo "# Password (key pass) for all signature key"
    echo "# NOTE:"
    echo "# for stm32mp15 platform, only one key is requested (it can be empty)"
    echo "# for stm32mp13 platform, up to 8 keys can be requested (means put all password according to key)"
    echo "SIGN_KEY_PASS=\"pass0 pass1 pass2 pass3 pass4 pass5 pass6 pass7\""
    echo ""
    echo "# Password key associated to private key selected via SIGN_KEY"
    echo "SIGN_KEY_PASS_FOR_SPECIFC_KEY=\"pass0\""
    echo ""
    echo "# ------ Key associated to EDMK---------------------------"
    echo "# encryption key use to decrypt Bl2 (TF-A) (128bits)"
    echo "ENCRYPT_FSBL_KEY=\"\${your_absolute_key_path}/a7_EDMK_key_128bits.bin\""
    echo ""
    echo "# ------ Key associated to EDMK ---------------------------"
    echo "# Encryption key use for fip content (256bits on txt)"
    echo "ENCRYPT_FIP_KEY=\"\${your_absolute_key_path}/a7_EDMK_key_256bits.txt\""
    echo ""
    echo "##################################################################"
    echo "##################################################################"
    echo "### End of Signature / Encryption"
    echo "##################################################################"
    echo "##################################################################"
    echo ""
}

function config_stm32mp1() {
    local mx=$1
    local security=$2
    config_stm32mp1_common

    if [ $mx -eq 1 ]; then
        config_stm32mp1_mx
    else
        config_stm32mp1_externaldt
    fi
    config_stm32mp_debug
    config_firmware_update
    echo ""
    echo "##################################################################"
    echo "##################################################################"
    echo "### End of general configuration"
    echo "##################################################################"
    echo "##################################################################"
    if [ $security -eq 1 ]; then
        config_stm32mp1_security
        config_security_restriction
    fi
}
function dump_stm32mp1_config() {
    local mx=$1
    echo "echo \"\""
    echo "echo \"Your configuration:\""
    echo "echo \"     sdk_absolute_path                 = \${sdk_absolute_path}\""
    echo "echo \"     sdk_env_file                      = \${sdk_env_file}\""
    echo "echo \"     your_board_name                   = \${your_board_name}\""
    echo "echo \"     your_soc_name                     = \${your_soc_name}\""
    echo "echo \"     your_storage_boot_scheme_security = \${your_storage_boot_scheme_security}\""
    echo "echo \"     your_storage_boot_scheme_cortex_a = \${your_storage_boot_scheme_cortex_a}\""
    echo "echo \"     your_deploy_dir_path              = \${your_deploy_dir_path}\""
    echo "echo \"     your_build_subdir_path            = \${your_build_subdir_path}\""
    if [ $mx -eq 1 ]; then
    echo "echo \"     your_cubemx_project_name          = \${your_cubemx_project_name}\""
    fi
    echo "echo \"     externaldt_path                   = \${externaldt_path}\""
    echo "echo \"\""
}

function signature_encryption_stm32mp1_default_configuration() {
    echo "if [ \${your_soc_name} == \"stm32mp15\" ]; then"
    echo "    # encryption is not supported"
    echo "    ENCRYPT_ENABLE=0"
    echo "fi"
    echo "# Coprocessor signature / encrytption are not supported"
    echo "SIGN_COPRO_ENABLE=0"
    echo "ENCRYPT_COPRO_ENABLE=0"
    echo ""
    echo "# add STM32 image information"
    echo "if [ \${your_soc_name} == \"stm32mp15\" ]; then"
    echo "    SIGN_HEADER_VERSION=\"1\""
    echo "    SIGN_TOOL_EXTRA=\"\""
    echo "    SIGN_ENCRYPT_TOOL_EXTRA=\" -of 0x80000003\""
    echo "    TF_A_ENCRYPT_DC=\"0x0E5F2025\""
    echo "else"
    echo "    SIGN_HEADER_VERSION=\"2\""
    echo "    SIGN_TOOL_EXTRA=\" -of 0x00000001\""
    echo "    SIGN_ENCRYPT_TOOL_EXTRA=\" -of 0x80000003\""
    echo "    TF_A_ENCRYPT_DC=\"0x0E5F2025\""

    echo "fi"
}

########################################################
#           STM32MP2
function config_stm32mp2_common() {
    echo "##################################################################"
    echo "##################################################################"
    echo "### General configuration"
    echo "##################################################################"
    echo "##################################################################"

    echo "# Path to SDK environment (where is 'environment-setup-cortexa35-ostl-linux')"
    echo "# Usually: /opt/st/stm32mp2/<X.X.X>-openstlinux-<X.X>-yocto-<version_name>-mpu-v<XX.XX.XX>/"
    echo "sdk_absolute_path=\"<SDK absolute path>\""
    echo "# Name of environment setup script for sdk"
    echo "sdk_env_file=\"environment-setup-cortexa35-ostl-linux\""
    echo ""

    echo "# Configuration for building stm32mp2x"
    echo "your_board_name=\"<Your board name>\""
    echo ""
    echo "# your_soc_name can be stm32mp21, stm32mp23, stm32mp25"
    echo "your_soc_name=\"<Your soc name>\""
    echo ""
    echo "# Type of security can be 'optee' or 'opteemin' ('optee' is preconize)"
    echo "your_storage_boot_scheme_security=\"<your_storage_boot_scheme_security>\""
    echo ""
    echo "# Type of boot scheme storage used for Cortex A35 part, ex.:"
    echo "#    optee-emmc"
    echo "#    optee-sdcard"
    echo "#    optee-nor"
    echo "#    optee-nand"
    echo "#    optee-spinand"
    echo "your_storage_boot_scheme_cortex_a=\"<your_storage_boot_scheme_cortex_a>\""
    echo ""
    echo "# Location to store all the binaries generated"
    echo "# An absolute path is required"
    echo "your_deploy_dir_path=\"<your_deploy_dir_path>\""
    echo ""
    echo "# Define build subdirectory (by default set to ../build)"
    echo "your_build_subdir_path=\"<your_build_subdir_path>\""
    echo ""

}
function config_stm32mp2_externaldt() {
    echo "# ----------------------------------------------------------------"
    echo "# ----------------------------------------------------------------"
    echo "### External DT configuration"
    echo "# ----------------------------------------------------------------"
    echo "# ----------------------------------------------------------------"
    echo "# External DT  devicetree tree"
    echo "#  <external dt path>/stm32mp2/a35-td/"
    echo "#    ├── linux"
    echo "#    ├── optee"
    echo "#    ├── tf-a"
    echo "#    ├── tfm"
    echo "#    └── u-boot"
    echo ""

    echo "# ------------------------------"
    echo "# Via external dt"
    echo "# force external dt path to external-dt env path"
    echo "# (EXTDT_DIR can be changed by overwriting the path on external-dt_set function)"
    echo "externaldt_path=\"<external dt path>\""
    echo ""
    echo "# external dt subpath for Optee"
    echo "externaldt_optee_path=stm32mp2/a35-td/optee"
    echo "externaldt_optee_programmer_path=stm32mp2/a35-td/optee"
    echo "# external dt U-boot"
    echo "externaldt_uboot_path=stm32mp2/a35-td/u-boot"
    echo "externaldt_uboot_programmer_path=stm32mp2/a35-td/u-boot"
    echo "# TF-A"
    echo "externaldt_tfa_path=stm32mp2/a35-td/tf-a"
    echo "externaldt_tfa_programmer_path=stm32mp2/a35-td/tf-a"
    echo "# Kernel linux"
    echo "externaldt_linux_path=stm32mp2/a35-td/linux"
    echo ""
    echo "# Name of devicetree to use"
    echo "# Optee dt name"
    echo "optee_dt_name=\${your_board_name}"
    echo "optee_dt_programmer_name=\${your_board_name}"
    echo "# U-boot dt name"
    echo "uboot_defconfig=\${your_soc_name}_defconfig"
    echo "uboot_dt_name=\${your_board_name}"
    echo "uboot_programmer_dt_name=\${your_board_name}"
    echo "# TF-A dt name"
    echo "tfa_dt_name=\${your_board_name}"
    echo "tfa_dt_programmer_name=\${your_board_name}"
    echo "# Linux kernel dt name"
    echo "linux_dtb_name=\${your_board_name}"
}
function config_stm32mp2_mx() {
    echo "# ----------------------------"
    echo "# CUBE MX Project"
    echo "# ----------------------------"
    echo "# define the cube mx project name"
    echo "# ex.: for <path>/CA35/DeviceTree/cube_mx_project/kernel/<cube mx dt>"
    echo "#      your_cubemx_project_name=cube_mx_project"
    echo "#      externaldt_path=path"
    echo "your_cubemx_project_name=<cubemx project name>"
    echo ""
    echo "# ----------------------------------------------------------------"
    echo "# ----------------------------------------------------------------"
    echo "### CubeMX DT configuration"
    echo "# ----------------------------------------------------------------"
    echo "# ----------------------------------------------------------------"
    echo "# CubeMX devicetree tree"
    echo "#  <CUBMEMX project path>/CA35/DeviceTree/<your_cubemx_project_name>/"
    echo "#    ├── kernel"
    echo "#    ├── optee-os"
    echo "#    ├── tf-a"
    echo "#    └── u-boot"
    echo ""
    echo "# ------------------------------"
    echo "# Via CubeMX dt"
    echo "# force external dt path to CubeMX project path"
    echo "# ex.: externaldt_path=my_project_path/"
    echo "externaldt_path=<Path to CubeMX project path>"
    echo "# external dt subpath for Optee"
    echo "externaldt_optee_path=CA35/DeviceTree/\${your_cubemx_project_name}/optee-os"
    echo "externaldt_optee_programmer_path=CA35/DeviceTree/\${your_cubemx_project_name}/optee-os"
    echo "# external dt U-boot"
    echo "externaldt_uboot_path=CA35/DeviceTree/\${your_cubemx_project_name}/u-boot"
    echo "externaldt_uboot_programmer_path=CA35/DeviceTree/\${your_cubemx_project_name}/u-boot"
    echo "# TF-A"
    echo "externaldt_tfa_path=CA35/DeviceTree/\${your_cubemx_project_name}/tf-a"
    echo "externaldt_tfa_programmer_path=CA35/DeviceTree/\${your_cubemx_project_name}/tf-a"
    echo "# Kernel linux"
    echo "externaldt_linux_path=CA35/DeviceTree/\${your_cubemx_project_name}/kernel"
    echo ""
    echo "# Name of devicetree to use"
    echo "# Optee dt name"
    echo "optee_dt_name=\${your_board_name}"
    echo "optee_dt_programmer_name=\${your_board_name}"
    echo "# U-boot dt name"
    echo "uboot_defconfig=\${your_soc_name}_defconfig"
    echo "uboot_dt_name=\${your_board_name}"
    echo "uboot_programmer_dt_name=\${your_board_name}"
    echo "# TF-A dt name"
    echo "tfa_dt_name=\${your_board_name}"
    echo "tfa_dt_programmer_name=\${your_board_name}"
    echo "# Linux kernel dt name"
    echo "linux_dtb_name=\${your_board_name}"
}
function config_stm32mp2_security() {
    echo ""
    echo "##################################################################"
    echo "##################################################################"
    echo "### Signature / Encryption configuration"
    echo "##################################################################"
    echo "##################################################################"
    echo "#"
    echo "#          STM32MP2: CORTEX A35 A35TD               CORTEX M33"
    echo "#   ---------------------------------------    ---------------------"
    echo "#   |                   BL2               |   |                    |"
    echo "#   |                 (TF-A)              |   |                    |"
    echo "#   |                                     |   |                    |"
    echo "#   |    verified with  | decrypted with  |   |                    |"
    echo "#   |       OEM-KEY1    |       (*1)      |   |                    |"
    echo "#   --------------------------------------    ----------------------"
    echo "#   |                 FIP                 |   |  co-processor FW   |"
    echo "#   |        (FW-CONFIG - TF-A)           |   |    (M33Project)    |"
    echo "#   |               (optee)               |   |                    |"
    echo "#   [          (DDR fw - TF-A)            |   |   verified with    |"
    echo "#   |                                     |   |     RPROC-FW-PKH   |"
    echo "#   |    verified with  | decrypted with  |   |   decrypted with   |"
    echo "#   |      OEM-KEY1     |     FIP-EDMK    |   |   RPROC-FW-ENC-KEY |"
    echo "#    --------------------------------------   ----------------------"
    echo "# (*1):"
    echo "#   for stm32mp21 is EDMK1-128b"
    echo "#   for stm32mp23/25 is EDMK1"
    echo "#"
    echo "# WARNING: For FIP will be verified (and decrypted) by BL2, you need to compile"
    echo "#   TF-A with specific option for signature support:"
    echo "#     for all platform: TRUSTED_BOARD_BOOT=1 MBEDTLS_DIR=<path to mbedtls source code> BRANCH_PROTECTION=0"
    echo "#   TF-A with specific option for encryption support:"
    echo "#     for all platform: DECRYPTION_SUPPORT=aes_gcm ENCRYPT_BL32=1 MBEDTLS_DIR=<path to mbedtls source code>"
    echo "#"
    echo ""
    echo "# Activate signature for BL2 and FIP on cortex A35"
    echo "SIGN_ENABLE=0"
    echo ""
    echo "# Activate Encryption for BL2 and FIP on cortex A35"
    echo "ENCRYPT_ENABLE=0"
    echo ""
    echo "# Activate signature during for loading firmware on Cortex M33 (coprocessor)"
    echo "SIGN_COPRO_ENABLE=0"
    echo "# Activate encryption during for loading firmware on Cortex M33 (coprocessor)"
    echo "ENCRYPT_COPRO_ENABLE=0"
    echo ""
    echo "# Absolute path to signing tools"
    echo "SIGN_TOOL=\"<absolute path>/STM32CubeProgrammer-x.xx/bin/STM32_SigningTool_CLI\""
    echo ""
    echo "# Directory on which all key are available"
    echo "your_absolute_key_path=<absolute path to key directory>"
    echo ""
    echo "# For naming the output file, use the following suffix:"
    echo "signature_suffix=\"_Signed\""
    echo "encryption_suffix=\"_Encrypted\""
    echo ""
    echo "# ------ Key associated to OEM-KEY1 ---------------------"
    echo "# Private Key for signature (absolute path to key)"
    echo "SIGN_KEY=\"\${your_absolute_key_path}/a35_OEM-KEY1_privateKey00.pem\""
    echo "# List of Public key for signature"
    echo "# NOTE:"
    echo "# for stm32mp2 platform, 8 key pairs public/private are supported"
    echo "SIGN_KEY_PUB=\"\\"
    echo "    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey00.pem \\"
    echo "    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey01.pem \\"
    echo "    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey02.pem \\"
    echo "    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey03.pem \\"
    echo "    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey04.pem \\"
    echo "    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey05.pem \\"
    echo "    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey06.pem \\"
    echo "    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey07.pem \\"
    echo "\""
    echo "# Password (key pass) for all signature key"
    echo "# NOTE:"
    echo "# for stm32mp2 platform, up to 8 keys can be requested (means put all password according to key)"
    echo "SIGN_KEY_PASS=\"pass0 pass1 pass2 pass3 pass4 pass5 pass6 pass7\""
    echo ""
    echo "# Password key associated to private key selected via SIGN_KEY"
    echo "SIGN_KEY_PASS_FOR_SPECIFC_KEY=\"pass0\""
    echo ""
    echo "# ------ Key associated to EDMK1 or EDMK1-128b------------"
    echo "# encryption key use to decrypt Bl2 (TF-A) (128bits)"
    echo "ENCRYPT_FSBL_KEY=\"\${your_absolute_key_path}/a35_EDMK1-128b_key_128bits.bin\""
    echo ""
    echo "# ------ Key associated to FIP-EDMK------------"
    echo "# encryption key use to decrypt FIP component (256bits)"
    echo "ENCRYPT_FIP_KEY=\"\${your_absolute_key_path}/a35_FIP-EDMK_key_256bits.bin\""
    echo ""
    echo "# ------ Key associated to RPROC-FW-PKH for coprocessor signature----------"
    echo "# Type of key use for coprocessor signature:"
    echo "# (256bits symmetric key)"
    echo "#  RSA: Asymetric cryptography algorithm"
    echo "#  ECC: Elliptic curve cryptography"
    echo "SIGN_COPRO_TYPE=ecc"
    echo ""
    echo "# For RSA KEY"
    echo "SIGN_COPRO_RSA_INFOKEY=\"\""
    echo "SIGN_COPRO_RSA_PRIVKEY=\"\""
    echo "SIGN_COPRO_RSA_PASS=\"\""
    echo ""
    echo "# For ECC KEY"
    echo "SIGN_COPRO_ECC_INFOKEY=\"\${your_absolute_key_path}/m33_RPROC-FW-PKH_ecc_publicKey.der\""
    echo "SIGN_COPRO_ECC_PRIVKEY=\"\${your_absolute_key_path}/m33_RPROC-FW-PKH_ecc_privateKey.pem\""
    echo "SIGN_COPRO_ECC_PASS=\"passrproc\""
    echo ""
    echo "# ------ Key associated to RPROC-FW-ENC-KEY for coprocessor encryption-------"
    echo "# Encryption key (256bits)"
    echo "ENCRYPT_COPRO_KEY=\"\${your_absolute_key_path}/m33_RPROC-FW-ENC-KEY_key_256bits.bin\""
    echo ""
    echo "##################################################################"
    echo "##################################################################"
    echo "### End of Signature / Encryption configuration"
    echo "##################################################################"
    echo "##################################################################"
}

function config_stm32mp2() {
    local mx=$1
    local security=$2
    config_stm32mp2_common
    if [ $mx -eq 1 ]; then
        config_stm32mp2_mx
    else
        config_stm32mp2_externaldt
    fi
    config_stm32mp_debug
    config_firmware_update
    echo ""
    echo "##################################################################"
    echo "##################################################################"
    echo "### End of general configuration"
    echo "##################################################################"
    echo "##################################################################"
    if [ $security -eq 1 ]; then
        config_stm32mp2_security
        config_security_restriction
    fi
}
function dump_stm32mp2_config() {
    local mx=$1
    echo "echo \"\""
    echo "echo \"Your configuration:\""
    echo "echo \"     sdk_absolute_path                 = \${sdk_absolute_path}\""
    echo "echo \"     sdk_env_file                      = \${sdk_env_file}\""
    echo "echo \"     your_board_name                   = \${your_board_name}\""
    echo "echo \"     your_soc_name                     = \${your_soc_name}\""
    echo "echo \"     your_storage_boot_scheme_security = \${your_storage_boot_scheme_security}\""
    echo "echo \"     your_storage_boot_scheme_cortex_a = \${your_storage_boot_scheme_cortex_a}\""
    echo "echo \"     your_deploy_dir_path              = \${your_deploy_dir_path}\""
    echo "echo \"     your_build_subdir_path            = \${your_build_subdir_path}\""
    if [ $mx -eq 1 ]; then
    echo "echo \"     your_cubemx_project_name          = \${your_cubemx_project_name}\""
    fi
    echo "echo \"     externaldt_path                   = \${externaldt_path}\""
    echo "echo \"\""
}

function signature_encryption_stm32mp2_default_configuration() {
    echo "# add STM32 image information"
    echo "case \${your_soc_name} in"
    echo "    stm32mp21)"
    echo "        SIGN_HEADER_VERSION=\"2.3\""
    echo "        SIGN_TOOL_EXTRA=\"--header-version 2.3 -of 0x00000001\""
    echo "        SIGN_ENCRYPT_TOOL_EXTRA=\"--header-version 2.3 -of 0x80000003\""
    echo "        TF_A_ENCRYPT_DC=\"0x25205f0e\""
    echo "        ;;"
    echo "    stm32mp23)"
    echo "        SIGN_HEADER_VERSION=\"2.2\""
    echo "        SIGN_TOOL_EXTRA=\"--header-version 2.2 -of 0x00000001\""
    echo "        SIGN_ENCRYPT_TOOL_EXTRA=\"--header-version 2.2 -of 0x80000003\""
    echo "        TF_A_ENCRYPT_DC=\"0x25205f0e\""
    echo "        ;;"
    echo "    stm32mp25)"
    echo "        SIGN_HEADER_VERSION=\"2.2\""
    echo "        SIGN_TOOL_EXTRA=\"--header-version 2.2 -of 0x00000001\""
    echo "        SIGN_ENCRYPT_TOOL_EXTRA=\"--header-version 2.2 -of 0x80000003\""
    echo "        TF_A_ENCRYPT_DC=\"0x25205f0e\""
    echo "        ;;"
    echo "    *)"
    echo "       die 'PLATFORM not supported for SIGN_HEADER_VERSION'"
    echo "        ;;"
    echo "esac"
}


########################################################
#           STM32MP2-m33td
function config_stm32mp2-m33td_common() {
    echo "declare -A DEVICETREE_SUFFIX_ARRAY"
    echo "DEVICETREE_SUFFIX_ARRAY[optee-emmc]=\"-emmc\""
    echo "DEVICETREE_SUFFIX_ARRAY[optee-sdcard]=\"-sdcard\""
    echo "DEVICETREE_SUFFIX_ARRAY[optee-nor]=\"-snor\""
    echo "DEVICETREE_SUFFIX_ARRAY[optee-programmer-usb]=\"\""
    echo "DEVICETREE_SUFFIX_ARRAY[optee-programmer-serial]=\"\""

    echo "##################################################################"
    echo "##################################################################"
    echo "### General configuration"
    echo "##################################################################"
    echo "##################################################################"

    echo "# Path to SDK environment (where is 'environment-setup-cortexa35-ostl-linux')"
    echo "# Usually: /opt/st/stm32mp2/<X.X.X>-openstlinux-<X.X>-yocto-<version_name>-mpu-v<XX.XX.XX>/"
    echo "sdk_absolute_path=\"<SDK absolute path>\""
    echo "# Name of environment setup script for sdk"
    echo "sdk_env_file=\"environment-setup-cortexa35-ostl-linux\""
    echo ""
    echo "# Configuration for building stm32mp2x-M33TD"
    echo "your_board_name=\"<Your board name>\""
    echo ""
    echo "# your_soc_name can be stm32mp21, stm32mp25"
    echo "your_soc_name=\"<Your soc name>\""
    echo ""
    echo "# Type of security can be 'optee' or 'opteemin' ('optee' is preconize)"
    echo "your_storage_boot_scheme_security=\"<your_storage_boot_scheme_security>\""
    echo ""
    echo "# Type of boot scheme storage used for Cortex A35 part, ex.:"
    echo "#    optee-emmc"
    echo "#    optee-sdcard"
    echo "#    optee-nor"
    echo "#    optee-nand"
    echo "#    optee-spinand"
    echo "your_storage_boot_scheme_cortex_a=\"<your_storage_boot_scheme_cortex_a>\""
    echo ""
    echo "# Type of boot scheme storage used for Cortex M33 part, ex.:"
    echo "#        m33td_emmc"
    echo "#        m33td_nor"
    echo "#        m33td_nor-emmc"
    echo "#        m33td_nor-sdcard"
    echo "#        m33td_sdcard"
    echo "your_storage_boot_scheme_cortex_m=\"<your_storage_boot_scheme_cortex_m>\""
    echo ""
    echo "# Name of profile use to generate TF-M binaries, ex:"
    echo "#        stm/stm32mp257f_ev1"
    echo "#        stm/stm32mp215f_dk"
    echo "#        stm/stm2mp2_generic"
    echo "#        CM33/PlatformTF-M for CubeMX configuration"
    echo "your_m33_profile=\"<your_m33_profile>\""
    echo ""
    echo "# Name of project used on M33 processor, ex.:"
    echo "#  Name of CubeMX project "
    echo "#  cat CM33/NonSecure/CMakeLists.txt | grep set.CMAKE_PROJECT_NAME | sed \"s|set(CMAKE_PROJECT_NAME \(.*\)_CM33_NS)|\1|\" "
    echo "your_cube_m33td_project_name=\"<your_cube_m33td_project_name>\""
    echo ""
    echo "# Path for project used on M33 processor:"
    echo "#   Absolute path to CM33/NonSecure on CubeMX project"
    echo "#  ex.: <path>/CM33/NonSecure"
    echo "your_cube_m33td_project_path=\"<your_cube_m33td_project_path>\""
    echo ""
    echo "# Specific cmake flags to apply for Cube M33TD project build, ex.:"
    echo "#       -DBUILD_CONFIG=FULL -DBOOT_SPLASHSCREEN=STATIC"
    echo "#your_cube_m33td_bld_opts=\"<your_cube_m33td_bld_opts>\""
    echo "your_cube_m33td_bld_opts=\"--preset Release\""
    echo ""
    echo "# Force suffix name of M33 NonSecure binary to found"
    echo "export M33FW_TYPE=_CM33_NS"
    echo ""
    echo "# Location to store all the binaries generated"
    echo "# An absolute path is required"
    echo "your_deploy_dir_path=\"<your_deploy_dir_absolute_path>\""
    echo ""
    echo "# Define build subdirectory (by default set to ../build)"
    echo "your_build_subdir_path=\"<your_build_subdir_path>\""
    echo ""
    echo "# Force header version number of M33 DDR firmware image"
    echo "# (information set on image for firmware update)"
    echo "# can be set via environment variable, ex.: SIGN_VERSION_DDR=1.1"
    echo "export M33FW_SIGN_VERSION_DDR=\${M33FW_SIGN_VERSION_DDR:-}"
    echo ""
    echo "# Force header version number of M33 firmware image (secure+non secure)"
    echo "# (information set on image for firmware update)"
    echo "# can be set via environment variable, ex.: SIGN_VERSION_FW=1.2"
    echo "export M33FW_SIGN_VERSION=\${M33FW_SIGN_VERSION:-}"
    echo ""
    echo "# Force header image confirm flag"
    echo "# effective for M33 DDR firmware and M33 firmware"
    echo "export M33FW_IMAGE_CONFIRM=1"
    echo ""
}
function config_stm32mp2-m33td_externaldt() {
    echo "# ----------------------------------------------------------------"
    echo "# ----------------------------------------------------------------"
    echo "### External DT configuration"
    echo "# ----------------------------------------------------------------"
    echo "# ----------------------------------------------------------------"
    echo "# External DT  devicetree tree"
    echo "#  <external dt path>/stm32mp2/m33-td/"
    echo "#    ├── linux"
    echo "#    ├── optee"
    echo "#    ├── tf-a"
    echo "#    ├── mcuboot"
    echo "#    ├── tfm"
    echo "#    └── u-boot"
    echo ""

    echo "# ------------------------------"
    echo "# Via external dt"
    echo "# force external dt path to externaldt env path"
    echo "# (EXTDT_DIR can be changed by overwriting the path on external-dt_set funcion)"
    echo "externaldt_path=<external dt path>"
    echo ""
    echo "# external dt subpath for Optee"
    echo "externaldt_optee_path=stm32mp2/m33-td/optee"
    echo "externaldt_optee_programmer_path=stm32mp2/m33-td/optee"
    echo "# external dt U-boot"
    echo "externaldt_uboot_path=stm32mp2/m33-td/u-boot"
    echo "externaldt_uboot_programmer_path=stm32mp2/m33-td/u-boot"
    echo "# TF-A"
    echo "externaldt_tfa_path=stm32mp2/m33-td/tf-a"
    echo "externaldt_tfa_programmer_path=stm32mp2/m33-td/tf-a"
    echo "# TF-M"
    echo "externaldt_tfm_path=stm32mp2/m33-td/tfm"
    echo "externaldt_mcuboot_path=stm32mp2/m33-td/mcuboot"
    echo "# Kernel linux"
    echo "externaldt_linux_path=stm32mp2/m33-td/linux"
    echo ""
    echo "# Name of devicetree to use"
    echo "# Optee dt name"
    echo "optee_dt_name=\${your_board_name}-cm33tdcid-ostl\${DEVICETREE_SUFFIX_ARRAY[\${your_storage_boot_scheme_cortex_a}]}"
    echo "optee_dt_programmer_name=\${your_board_name}-cm33tdcid-ostl-serial-ca35tdcid"
    echo "# U-boot dt name"
    echo "uboot_defconfig=\${your_soc_name}_defconfig"
    echo "uboot_dt_name=\${your_board_name}-cm33tdcid-ostl\${DEVICETREE_SUFFIX_ARRAY[\${your_storage_boot_scheme_cortex_a}]}"
    echo "uboot_programmer_dt_name=\${your_board_name}-cm33tdcid-ostl-serial-ca35tdcid"
    echo "# TF-A dt name"
    echo "tfa_dt_name=\${your_board_name}-cm33tdcid-ostl\${DEVICETREE_SUFFIX_ARRAY[\${your_storage_boot_scheme_cortex_a}]}"
    echo "tfa_dt_programmer_name=\${your_board_name}-cm33tdcid-ostl-serial-ca35tdcid"
    echo "# Linux kernel dt name"
    echo "linux_dtb_name=\${your_board_name}-cm33tdcid-ostl\${DEVICETREE_SUFFIX_ARRAY[\${your_storage_boot_scheme_cortex_a}]}"
    echo "# TF-M dt name"
    echo "tfm_dt_name=\${your_board_name}-cm33tdcid-ostl"
    echo ""
    echo "# Use externaldt suffix (defined via DEVICETREE_SUFFIX_ARRAY)"
    echo "use_externaldt_suffix=1"
}
function config_stm32mp2-m33td_mx() {
    echo "# ----------------------------"
    echo "# CUBE MX Project"
    echo "# ----------------------------"
    echo "# define the cube mx project name"
    echo "# ex.: for <path>/C35/DeviceTree/cube_mx_project/kernel/<cube mx dt>"
    echo "#      your_cubemx_project_name=cube_mx_project"
    echo "#      externaldt_path=path"
    echo "your_cubemx_project_name=<cubemx project name>"
    echo ""

    echo "# ----------------------------------------------------------------"
    echo "# ----------------------------------------------------------------"
    echo "### CubeMX DT configuration"
    echo "# ----------------------------------------------------------------"
    echo "# ----------------------------------------------------------------"
    echo "# CubeMX devicetree tree"
    echo "#  <CUBMEMX project path>/CA35/DeviceTree/<your_cubemx_project_name>/"
    echo "#    ├── kernel"
    echo "#    ├── optee-os"
    echo "#    ├── tf-a"
    echo "#    └── u-boot"
    echo "#  <CUBMEMX project path>/CM33/DeviceTree/<your_cubemx_project_name>/"
    echo "#    ├── tf-m"
    echo "#    └── mcuboot"
    echo "#  <CUBMEMX project path>/ExtMemLoader/DeviceTree/"
    echo "#    ├── optee-os"
    echo "#    ├── tf-a"
    echo "#    └── u-boot"
    echo ""

    echo "# ------------------------------"
    echo "# Via CubeMX dt"
    echo "# force external dt path to CubeMX project path"
    echo "externaldt_path=<Path to CubeMX project path>"
    echo "# external dt subpath for Optee"
    echo "externaldt_optee_path=CA35/DeviceTree/\${your_cubemx_project_name}/optee-os"
    echo "externaldt_optee_programmer_path=ExtMemLoader/DeviceTree/optee-os"
    echo "# external dt U-boot"
    echo "externaldt_uboot_path=CA35/DeviceTree/\${your_cubemx_project_name}/u-boot"
    echo "externaldt_uboot_programmer_path=ExtMemLoader/DeviceTree/u-boot"
    echo "# TF-A"
    echo "externaldt_tfa_path=CA35/DeviceTree/\${your_cubemx_project_name}/tf-a"
    echo "externaldt_tfa_programmer_path=ExtMemLoader/DeviceTree/tf-a"
    echo "# TF-M"
    echo "externaldt_tfm_path=CM33/DeviceTree/\${your_cubemx_project_name}/tf-m"
    echo "externaldt_mcuboot_path=CM33/DeviceTree/\${your_cubemx_project_name}/mcuboot"
    echo "# Kernel linux"
    echo "externaldt_linux_path=CA35/DeviceTree/\${your_cubemx_project_name}/kernel"
    echo ""
    echo "# Name of devicetree to use"
    echo "# Optee dt name"
    echo "optee_dt_name=\${your_board_name}"
    echo "optee_dt_programmer_name=\${your_board_name}"
    echo "# U-boot dt name"
    echo "uboot_defconfig=\${your_soc_name}_defconfig"
    echo "uboot_dt_name=\${your_board_name}"
    echo "uboot_programmer_dt_name=\${your_board_name}"
    echo "# TF-A dt name"
    echo "tfa_dt_name=\${your_board_name}"
    echo "tfa_dt_programmer_name=\${your_board_name}"
    echo "# Linux kernel dt name"
    echo "linux_dtb_name=\${your_board_name}"
    echo "# TF-M dt name"
    echo "tfm_dt_name=\${your_board_name}"
    echo ""
    echo "# Do not use externaldt suffix (Mx doesn't generate devicetree with sotorage suffix)"
    echo "use_externaldt_suffix=0"

}

function config_stm32mp2-m33td_security() {
    echo ""
    echo "##################################################################"
    echo "##################################################################"
    echo "### Signature / Encryption configuration"
    echo "##################################################################"
    echo "##################################################################"
    echo "#"
    echo "#          STM32MP2: CORTEX A35 A35TD               CORTEX M33"
    echo "#   ---------------------------------------    ---------------------"
    echo "#   |                   BL2               |   |       BL2          |"
    echo "#   |                 (TF-A)              |   |   (TF-M MCUBOOT)   |"
    echo "#   |                                     |   |                    |"
    echo "#   |    verified with  | decrypted with  |   | verified with      |"
    echo "#   |       OEM-KEY1    |       (*1)      |   |   OEM-KEY2         |"
    echo "#   |                                     |   |  (or OEM-KEY1)     |"
    echo "#   --------------------------------------    ----------------------"
    echo "#   |                 FIP                 |   |       DDR FW       |"
    echo "#   |        (FW-CONFIG - TF-A)           |   |    (TF-M)          |"
    echo "#   |               (optee)               |   |                    |"
    echo "#   [          (DDR fw - TF-A)            |   |   verified with    |"
    echo "#   |                                     |   |  FSBLM-DDR-FW-PKH  |"
    echo "#   |    verified with  | decrypted with  |   ----------------------"
    echo "#   |      OEM-KEY1     |     FIP-EDMK    |   |       M33 FW       |"
    echo "#    --------------------------------------   |                    |"
    echo "#                                             |   verified with    |"
    echo "#                                             |  FSBLM-M33-FW-PKH  |"
    echo "#                                             ---------------------- "
    echo "#"
    echo "# (*1):"
    echo "#   for stm32mp21 is EDMK1-128b"
    echo "#   for stm32mp23/25 is EDMK1"
    echo "#"
    echo "# WARNING: For FIP will be verified (and decrypted) by BL2, you need to compile"
    echo "#   TF-A with specific option for signature support:"
    echo "#     for all platform: TRUSTED_BOARD_BOOT=1 MBEDTLS_DIR=<path to mbedtls source code> BRANCH_PROTECTION=0"
    echo "#   TF-A with specific option for encryption support:"
    echo "#     for all platform: DECRYPTION_SUPPORT=aes_gcm ENCRYPT_BL32=1 MBEDTLS_DIR=<path to mbedtls source code>"
    echo "#"
    echo ""
    echo "# Activate signature for BL2 and FIP on cortex A35"
    echo "SIGN_ENABLE=0"
    echo ""
    echo "# Activate Encryption for BL2 and FIP on cortex A35"
    echo "ENCRYPT_ENABLE=0"
    echo ""
    echo "# Activate signature during for loading firmware on Cortex M33 (coprocessor)"
    echo "SIGN_COPRO_ENABLE=0"
    echo "# Activate encryption during for loading firmware on Cortex M33 (coprocessor)"
    echo "ENCRYPT_COPRO_ENABLE=0"
    echo ""
    echo "# Absolute path to signing tools"
    echo "SIGN_TOOL=\"<absolute path>/STM32CubeProgrammer-x.xx/bin/STM32_SigningTool_CLI\""
    echo ""
    echo "# Directory on which all key are available"
    echo "your_absolute_key_path=<absolute path to key directory>"
    echo ""
    echo "# For naming the output file, use the following suffix:"
    echo "signature_suffix=\"_Signed\""
    echo "encryption_suffix=\"_Encrypted\""
    echo ""

    echo "# ------ Key associated to OEM-KEY1 ---------------------"
    echo "# Private Key for signature (absolute path to key)"
    echo "SIGN_KEY=\"\${your_absolute_key_path}/a35_OEM-KEY1_privateKey00.pem\""
    echo "# List of Public key for signature"
    echo "# NOTE:"
    echo "# for stm32mp2 platform, 8 key pairs public/private are supported"
    echo "SIGN_KEY_PUB=\"\\"
    echo "    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey00.pem \\"
    echo "    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey01.pem \\"
    echo "    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey02.pem \\"
    echo "    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey03.pem \\"
    echo "    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey04.pem \\"
    echo "    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey05.pem \\"
    echo "    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey06.pem \\"
    echo "    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey07.pem \\"
    echo "\""
    echo "# Password (key pass) for all signature key"
    echo "# NOTE:"
    echo "# for stm32mp2 platform, up to 8 keys can be requested (means put all password according to key)"
    echo "SIGN_KEY_PASS=\"pass0 pass1 pass2 pass3 pass4 pass5 pass6 pass7\""
    echo ""
    echo "# Password key associated to private key selected via SIGN_KEY"
    echo "SIGN_KEY_PASS_FOR_SPECIFC_KEY=\"pass0\""
    echo ""
    echo "# ------ Key associated to EDMK1 or EDMK1-128b------------"
    echo "# encryption key use to decrypt Bl2 (TF-A) (128bits)"
    echo "ENCRYPT_FSBL_KEY=\"\${your_absolute_key_path}/a35_EDMK1-128b_key_128bits.bin\""
    echo ""
    echo "# ------ Key associated to OEM-KEY2 ---------------------"
    echo "# NOTE: if OEM-KEY1 is configured on OTP for verifying Cortex M33 BL2"
    echo "#     You need to configure the following variable as OEM-KEY1"
    echo ""
    echo "USE_SAME_KEY_FOR_OEM_KEY1_AND_OEM_KEY2=1"
    echo "if [ \${USE_SAME_KEY_FOR_OEM_KEY1_AND_OEM_KEY2} -eq 1 ]; then"
    echo "    # USING OEM-KEY1 for Cortex M33 BL2 verification"
    echo "    # Private Key for signature (absolute path to key)"
    echo "    SIGN_KEY_M=\"\${your_absolute_key_path}/a35_OEM-KEY1_privateKey00.pem\""
    echo "    # List of Public key for signature"
    echo "    # NOTE:"
    echo "    # for stm32mp2 platform, 8 key pairs public/private are supported"
    echo "    SIGN_KEY_PUB_M=\"\\"
    echo "        \${your_absolute_key_path}/a35_OEM-KEY1_publicKey00.pem \\"
    echo "        \${your_absolute_key_path}/a35_OEM-KEY1_publicKey01.pem \\"
    echo "        \${your_absolute_key_path}/a35_OEM-KEY1_publicKey02.pem \\"
    echo "        \${your_absolute_key_path}/a35_OEM-KEY1_publicKey03.pem \\"
    echo "        \${your_absolute_key_path}/a35_OEM-KEY1_publicKey04.pem \\"
    echo "        \${your_absolute_key_path}/a35_OEM-KEY1_publicKey05.pem \\"
    echo "        \${your_absolute_key_path}/a35_OEM-KEY1_publicKey06.pem \\"
    echo "        \${your_absolute_key_path}/a35_OEM-KEY1_publicKey07.pem \\"
    echo "    \""
    echo "    # Password (key pass) for all signature key"
    echo "    # NOTE:"
    echo "    # for stm32mp2 platform, up to 8 keys can be requested (means put all password according to key)"
    echo "    SIGN_KEY_PASS_M=\"pass0 pass1 pass2 pass3 pass4 pass5 pass6 pass7\""
    echo ""
    echo "    # Password key associated to private key selected via SIGN_KEY"
    echo "    SIGN_KEY_PASS_M_FOR_SPECIFC_KEY=\"pass0\""
    echo ""
    echo "    # ------ Key associated to EDMK1 or EDMK1-128b------------"
    echo "    # encryption key use to decrypt Bl2 (TF-A) (128bits)"
    echo "    ENCRYPT_FSBL_M_KEY=\"\${your_absolute_key_path}/a35_EDMK1-128b_key_128bits.bin\""
    echo ""
    echo "else"
    echo "    # USING two separate key, one for Cortex A35 one for CortexM33"
    echo "    # Private Key for signature (absolute path to key)"
    echo "    SIGN_KEY_M=\"\${your_absolute_key_path}/m33_OEM-KEY2_privateKey00.pem\""
    echo "    # List of Public key for signature"
    echo "    # NOTE:"
    echo "    # for stm32mp2 platform, 8 key pairs public/private are supported"
    echo "    SIGN_KEY_PUB_M=\"\\"
    echo "        \${your_absolute_key_path}/m33_OEM-KEY2_publicKey00.pem \\"
    echo "        \${your_absolute_key_path}/m33_OEM-KEY2_publicKey01.pem \\"
    echo "        \${your_absolute_key_path}/m33_OEM-KEY2_publicKey02.pem \\"
    echo "        \${your_absolute_key_path}/m33_OEM-KEY2_publicKey03.pem \\"
    echo "        \${your_absolute_key_path}/m33_OEM-KEY2_publicKey04.pem \\"
    echo "        \${your_absolute_key_path}/m33_OEM-KEY2_publicKey05.pem \\"
    echo "        \${your_absolute_key_path}/m33_OEM-KEY2_publicKey06.pem \\"
    echo "        \${your_absolute_key_path}/m33_OEM-KEY2_publicKey07.pem \\"
    echo "    \""
    echo "    # Password (key pass) for all signature key"
    echo "    # NOTE:"
    echo "    # for stm32mp2 platform, up to 8 keys can be requested (means put all password according to key)"
    echo "    SIGN_KEY_PASS_M=\"passM0 passM1 passM2 passM3 passM4 passM5 passM6 passM7\""
    echo ""
    echo "    # Password key associated to private key selected via SIGN_KEY"
    echo "    SIGN_KEY_PASS_M_FOR_SPECIFC_KEY=\"passM0\""
    echo ""
    echo "    # ------ Key associated to EDMK2 or EDMK2-128b------------"
    echo "    # encryption key use to decrypt Bl2 (TF-A) (128bits)"
    echo "    ENCRYPT_FSBL_M_KEY=\"\${your_absolute_key_path}/m33_EDMK2-128b_key_128bits.bin\""
    echo ""
    echo "fi"
    echo ""

    echo "# ------ Key associated to FSBLM-DDR-FW-PKH ---------------------"
    echo "SIGN_M33DDR_KEY=\"\${your_absolute_key_path}/m33_FSBLM-DDR-FW-PKH_privateKey.pem\""
    echo "SIGN_M33DDR_KEY_PASS=\"ddr_m_pass\""
    echo ""
    echo "# ------ Key associated to FSBLM-M33-FW-PKH ---------------------"
    echo "SIGN_M33FW_KEY=\"\${your_absolute_key_path}/m33_FSBLM-M33-FW-PKH_privateKey.pem\""
    echo "SIGN_M33FW_KEY_PASS=\"fw_m_pass\""
    echo ""
    echo "# ------ Key associated to FIP-EDMK------------"
    echo "# encryption key use to decrypt FIP component (256bits)"
    echo "ENCRYPT_FIP_KEY=\"\${your_absolute_key_path}/a35_FIP-EDMK_key_256bits.bin\""
    echo ""
    echo "# ------ Key associated to RPROC-FW-PKH for coprocessor signature----------"
    echo "# Type of key use for coprocessor signature:"
    echo "# (256bits symmetric key)"
    echo "#  RSA: Asymetric cryptography algorithm"
    echo "#  ECC: Elliptic curve cryptography"
    echo "SIGN_COPRO_TYPE=ecc"
    echo ""
    echo "# For RSA KEY"
    echo "SIGN_COPRO_RSA_INFOKEY=\"\""
    echo "SIGN_COPRO_RSA_PRIVKEY=\"\""
    echo "SIGN_COPRO_RSA_PASS=\"\""
    echo ""
    echo "# For ECC KEY"
    echo "SIGN_COPRO_ECC_INFOKEY=\"\${your_absolute_key_path}/m33_RPROC-FW-PKH_ecc_publicKey.der\""
    echo "SIGN_COPRO_ECC_PRIVKEY=\"\${your_absolute_key_path}/m33_RPROC-FW-PKH_ecc_privateKey.pem\""
    echo "SIGN_COPRO_ECC_PASS=\"passrproc\""
    echo ""
    echo "# ------ Key associated to RPROC-FW-ENC-KEY for coprocessor encryption-------"
    echo "# Encryption key (256bits)"
    echo "ENCRYPT_COPRO_KEY=\"\${your_absolute_key_path}/m33_RPROC-FW-ENC-KEY_key_256bits.bin\""
    echo ""
    echo "##################################################################"
    echo "##################################################################"
    echo "### End of Signature / Encryption configuration"
    echo "##################################################################"
    echo "##################################################################"
}

function config_stm32mp2-m33td() {
    local mx=$1
    local security=$2
    config_stm32mp2-m33td_common
    if [ $mx -eq 1 ]; then
        config_stm32mp2-m33td_mx
    else
        config_stm32mp2-m33td_externaldt
    fi
    config_stm32mp_debug
    config_stm32mp2_m33td_debug
    config_firmware_update
    echo ""
    echo "##################################################################"
    echo "##################################################################"
    echo "### End of general configuration"
    echo "##################################################################"
    echo "##################################################################"
    if [ $security -eq 1 ]; then
        config_stm32mp2-m33td_security
        config_security_restriction
    fi
}
function dump_stm32mp2-m33td_config() {
    local mx=$1
    echo "echo \"\""
    echo "echo \"Your configuration:\""
    echo "echo \"     sdk_absolute_path                 = \${sdk_absolute_path}\""
    echo "echo \"     sdk_env_file                      = \${sdk_env_file}\""
    echo "echo \"     your_board_name                   = \${your_board_name}\""
    echo "echo \"     your_soc_name                     = \${your_soc_name}\""
    echo "echo \"     your_storage_boot_scheme_security = \${your_storage_boot_scheme_security}\""
    echo "echo \"     your_storage_boot_scheme_cortex_a = \${your_storage_boot_scheme_cortex_a}\""
    if [ $mx -eq 1 ]; then
        echo "echo \"     your_storage_boot_scheme_cortex_m = \${your_storage_boot_scheme_cortex_m}\""
        echo "echo \"     your_m33_profile                  = \${your_m33_profile}\""
    fi
    echo "echo \"     your_deploy_dir_path              = \${your_deploy_dir_path}\""
    echo "echo \"     your_build_subdir_path            = \${your_build_subdir_path}\""
    if [ $mx -eq 1 ]; then
    echo "echo \"     your_cubemx_project_name          = \${your_cubemx_project_name}\""
    fi
    echo "echo \"     externaldt_path                   = \${externaldt_path}\""
    echo "echo \"     use_externaldt_suffix             = \${use_externaldt_suffix}\""
    echo "echo \"\""
}

function signature_encryption_stm32mp2-m33td_default_configuration() {
    echo "# add STM32 image information"
    echo "case \${your_soc_name} in"
    echo "    stm32mp21)"
    echo "        SIGN_HEADER_VERSION=\"2.3\""
    echo "        SIGN_TOOL_EXTRA=\"--header-version 2.3 -of 0x00000001\""
    echo "        SIGN_ENCRYPT_TOOL_EXTRA=\"--header-version 2.3 -of 0x80000003\""
    echo "        TF_A_ENCRYPT_DC=\"0x25205f0e\""
    echo "        ;;"
    echo "    stm32mp23)"
    echo "        SIGN_HEADER_VERSION=\"2.2\""
    echo "        SIGN_TOOL_EXTRA=\"--header-version 2.2 -of 0x00000001\""
    echo "        SIGN_ENCRYPT_TOOL_EXTRA=\"--header-version 2.2 -of 0x80000003\""
    echo "        TF_A_ENCRYPT_DC=\"0x25205f0e\""
    echo "        ;;"
    echo "    stm32mp25)"
    echo "        SIGN_HEADER_VERSION=\"2.2\""
    echo "        SIGN_TOOL_EXTRA=\"--header-version 2.2 -of 0x00000001\""
    echo "        SIGN_ENCRYPT_TOOL_EXTRA=\"--header-version 2.2 -of 0x80000003\""
    echo "        TF_A_ENCRYPT_DC=\"0x25205f0e\""
    echo "        ;;"
    echo "    *)"
    echo "       die 'PLATFORM not supported for SIGN_HEADER_VERSION'"
    echo "       ;;"
    echo "esac"
}

########################################################
function config_stm32mp() {
    local machine=$1
    local mx=$2
    local secu=$3
    case $machine in
    stm32mp1)
        config_stm32mp1 $mx $secu
        ;;
    stm32mp2)
        config_stm32mp2 $mx $secu
        ;;
    stm32mp2-m33td)
        config_stm32mp2-m33td $mx $secu
        ;;
    *)
        ;;
    esac
}
function dump_stm32mp_config() {
    local machine=$1
    local mx=$2
    case $machine in
    stm32mp1)
        echo ""
        dump_stm32mp1_config $mx
        echo "[ \$local_DRY_RUN -eq 1 ] &&  echo \"     ***DRY RUN***\""
        echo "[ \$local_DRY_RUN -eq 1 ] &&  echo \"\""
        ;;
    stm32mp2)
        echo ""
        dump_stm32mp2_config $mx
        echo "[ \$local_DRY_RUN -eq 1 ] &&  echo \"     ***DRY RUN***\""
        echo "[ \$local_DRY_RUN -eq 1 ] &&  echo \"\""
        ;;
    stm32mp2-m33td)
        echo ""
        dump_stm32mp2-m33td_config $mx
        echo "[ \$local_DRY_RUN -eq 1 ] &&  echo \"     ***DRY RUN***\""
        echo "[ \$local_DRY_RUN -eq 1 ] &&  echo \"\""
        ;;
    *)
        ;;
    esac
    echo ""
    echo "# Test if sdk if correctly set"
    echo "[ \$local_DRY_RUN -eq 0 ] && [ -d \${sdk_absolute_path} ] || die 'testing sdk path: sdk patch not correct'"
    echo "[ \$local_DRY_RUN -eq 0 ] && [ -e \${sdk_absolute_path}/\${sdk_env_file} ] || die  'testing sdk environment script: env script not set correclty'"

}
########################################################
SCRIPT_PATH=$(dirname ${BASH_SOURCE})

PRINT_DEBUG=${DEBUG:-0}
function debug() {
    if [ $PRINT_DEBUG -eq 1 ]; then
        >&2 echo "[DEBUG]: $@"
    fi
}
function error(){
    >&2 echo "[Error]: $@"
}
function info(){
    >&2 echo "[INFO]: $@"
}
function process_data() {
    localdata=$@
    # your_board_name
    if $(echo $localdata | grep -q your_board_name) ; then
        localdata=$(echo $localdata |sed "s|<your_board_name>|\${your_board_name}|g")
    fi
    # your_soc_name
    if $(echo $localdata | grep -q your_soc_name) ; then
        localdata=$(echo $localdata |sed "s|<your_soc_name>|\${your_soc_name}|g")
    fi
    # your_storage_boot_scheme_security
    if $(echo $localdata | grep -q your_storage_boot_scheme_security) ; then
        localdata=$(echo $localdata |sed "s|<your_storage_boot_scheme_security>|\${your_storage_boot_scheme_security}|g")
    fi
    # your_storage_boot_scheme_cortex_a
    if $(echo $localdata | grep -q your_storage_boot_scheme_cortex_a) ; then
        localdata=$(echo $localdata |sed "s|<your_storage_boot_scheme_cortex_a>|\${your_storage_boot_scheme_cortex_a}|g")
    fi
    # your_storage_boot_scheme_cortex_m
    if $(echo $localdata | grep -q your_storage_boot_scheme_cortex_m) ; then
        localdata=$(echo $localdata |sed "s|<your_storage_boot_scheme_cortex_m>|\${your_storage_boot_scheme_cortex_m}|g")
    fi
    # your_m33_profile
    if $(echo $localdata | grep -q your_m33_profile) ; then
        localdata=$(echo $localdata |sed "s|<your_m33_profile>|\${your_m33_profile}|g")
    fi
    # your_deploy_dir_path
    if $(echo $localdata | grep -q your_deploy_dir_path) ; then
        localdata=$(echo $localdata |sed "s|<your_deploy_dir_path>|\${your_deploy_dir_path}|g")
    fi
    # use_externaldt_suffix
    if $(echo $localdata | grep -q use_externaldt_suffix) ; then
        localdata=$(echo $localdata |sed "s|<use_externaldt_suffix>|\${use_externaldt_suffix}|g")
    fi
    # your build sub path
    if $(echo $localdata | grep -q your_build_subdir_path) ; then
        localdata=$(echo $localdata |sed "s|<your_build_subdir_path>|\${your_build_subdir_path}|g")
    fi

    # optee
    if $(echo $localdata | grep -q externaldt_path) ; then
        localdata=$(echo $localdata |sed "s|<externaldt_path>|\${externaldt_path}|g")
    fi

    # optee
    if $(echo $localdata | grep -q optee_dt_name) ; then
        localdata=$(echo $localdata |sed "s|<optee_dt_name>|\${optee_dt_name}|g")
    fi
    if $(echo $localdata | grep -q externaldt_optee_path) ; then
        localdata=$(echo $localdata |sed "s|<externaldt_optee_path>|\${externaldt_optee_path}|g")
    fi
    if $(echo $localdata | grep -q optee_dt_programmer_name) ; then
        localdata=$(echo $localdata |sed "s|<optee_dt_programmer_name>|\${optee_dt_programmer_name}|g")
    fi
    if $(echo $localdata | grep -q externaldt_optee_programmer_path) ; then
        localdata=$(echo $localdata |sed "s|<externaldt_optee_programmer_path>|\${externaldt_optee_programmer_path}|g")
    fi
    # u-boot
    if $(echo $localdata | grep -q uboot_defconfig) ; then
        localdata=$(echo $localdata |sed "s|<uboot_defconfig>|\${uboot_defconfig}|g")
    fi
        if $(echo $localdata | grep -q uboot_dt_name) ; then
        localdata=$(echo $localdata |sed "s|<uboot_dt_name>|\${uboot_dt_name}|g")
    fi
        if $(echo $localdata | grep -q externaldt_uboot_path) ; then
        localdata=$(echo $localdata |sed "s|<externaldt_uboot_path>|\${externaldt_uboot_path}|g")
    fi
    if $(echo $localdata | grep -q uboot_programmer_dt_name) ; then
        localdata=$(echo $localdata |sed "s|<uboot_programmer_dt_name>|\${uboot_programmer_dt_name}|g")
    fi
    if $(echo $localdata | grep -q externaldt_uboot_programmer_path) ; then
        localdata=$(echo $localdata |sed "s|<externaldt_uboot_programmer_path>|\${externaldt_uboot_programmer_path}|g")
    fi
    # tf-a
    if $(echo $localdata | grep -q tfa_dt_name) ; then
        localdata=$(echo $localdata |sed "s|<tfa_dt_name>|\${tfa_dt_name}|g")
    fi
    if $(echo $localdata | grep -q externaldt_tfa_path) ; then
        localdata=$(echo $localdata |sed "s|<externaldt_tfa_path>|\${externaldt_tfa_path}|g")
    fi
    if $(echo $localdata | grep -q tfa_dt_programmer_name) ; then
        localdata=$(echo $localdata |sed "s|<tfa_dt_programmer_name>|\${tfa_dt_programmer_name}|g")
    fi
    if $(echo $localdata | grep -q externaldt_tfa_programmer_path) ; then
        localdata=$(echo $localdata |sed "s|<externaldt_tfa_programmer_path>|\${externaldt_tfa_programmer_path}|g")
    fi
    if $(echo $localdata | grep -q sign_enable) ; then
        localdata=$(echo $localdata |sed "s|<sign_enable>|\${SIGN_ENABLE}|g")
    fi
    if $(echo $localdata | grep -q encrypt_enable) ; then
        localdata=$(echo $localdata |sed "s|<encrypt_enable>|\${ENCRYPT_ENABLE}|g")
    fi
    # tf-m
    if $(echo $localdata | grep -q externaldt_tfm_path) ; then
        localdata=$(echo $localdata |sed "s|<externaldt_tfm_path>|\${externaldt_tfm_path}|g")
    fi
    if $(echo $localdata | grep -q tfm_dt_name) ; then
        localdata=$(echo $localdata |sed "s|<tfm_dt_name>|\${tfm_dt_name}|g")
    fi
    if $(echo $localdata | grep -q externaldt_mcuboot_path) ; then
        localdata=$(echo $localdata |sed "s|<externaldt_mcuboot_path>|\${externaldt_mcuboot_path}|g")
    fi
    # linux
    if $(echo $localdata | grep -q externaldt_linux_path) ; then
        localdata=$(echo $localdata |sed "s|<externaldt_linux_path>|\${externaldt_linux_path}|g")
    fi
    if $(echo $localdata | grep -q kernel_fragment_list) ; then
        localdata=$(echo $localdata |sed "s|<kernel_fragment_list>|\${KERNEL_FRAGMENT_LIST}|g")
    fi
    # m33tdproject
    if $(echo $localdata | grep -q your_cube_m33td_project_name) ; then
        localdata=$(echo $localdata |sed "s|<your_cube_m33td_project_name>|\${your_cube_m33td_project_name}|g")
    fi
    if $(echo $localdata | grep -q your_cube_m33td_project_path) ; then
        localdata=$(echo $localdata |sed "s|<your_cube_m33td_project_path>|\${your_cube_m33td_project_path}|g")
    fi
    if $(echo $localdata | grep -q your_cube_m33td_bld_opts) ; then
        localdata=$(echo $localdata |sed "s|<your_cube_m33td_bld_opts>|\${your_cube_m33td_bld_opts}|g")
    fi

    # add parallel make for each make call
    if $(echo $localdata | grep -q make) ; then
        localdata=$(echo $localdata |sed "s|make|make \${PARALLEL_MAKE}|g")
    fi


    echo $localdata
}

function generate_action_all() {
    debug "** generate_action_all"
    for c in ${COMPONENTS};
    do
        echo "    ${c}_extract"
    done
    echo "    action_set"
    for c in ${COMPONENTS};
    do
        echo "    ${c}_configure"
        echo "    ${c}_compile"
        echo "    ${c}_deploy"
    done
}
function generate_action_extract() {
    debug "** generate_action_extract"
    for c in ${COMPONENTS};
    do
        echo "    ${c}_extract"
    done
}
function generate_action_compile() {
    debug "** generate_action_compile"
    echo "    action_set"
    for c in ${COMPONENTS};
    do
        echo "    ${c}_configure"
        echo "    ${c}_compile"
    done
}
function generate_action_compile_for_deploy() {
    debug "** generate_action_compile_for_deploy"
    echo "    action_set"
    for c in ${COMPONENTS_FIP};
    do
        echo "    ${c}_configure"
        echo "    ${c}_compile"
    done
}
function generate_action_compile_m33fw_for_deploy() {
    debug "** generate_action_compile_m33fw_for_deploy"
    echo "    action_set"
    for c in ${COMPONENTS_M33FW};
    do
        echo "    ${c}_configure"
        echo "    ${c}_compile"
    done
}
function generate_action_deploy_for_fip() {
    debug "** generate_action_deploy_for_fip"
    echo "    action_set"
    for c in ${COMPONENTS_FIP};
    do
        echo "    ${c}_deploy"
    done
}
function generate_action_deploy_for_m33fw() {
    debug "** generate_action_deploy_for_m33fw"
    echo "    action_set"
    for c in ${COMPONENTS_M33FW};
    do
        echo "    ${c}_deploy"
    done
}
function generate_action_deploy() {
    debug "** generate_action_deploy"
    echo "    action_set"
    for c in ${COMPONENTS};
    do
        echo "    ${c}_deploy"
    done
}
function generate_action_programmer() {
    debug "** generate_action_programmer"
    echo "    action_set"
    for c in ${COMPONENTS_FIP};
    do
        echo "    ${c}_programmer_compile"
    done
    for c in ${COMPONENTS_FIP};
    do
        echo "    ${c}_programmer_deploy"
    done
}
function generate_action_set() {
    debug "** generate_action_extract"
    echo "function action_set {"
    for c in ${COMPONENTS};
    do
        echo "    ${c}_set"
    done
    echo "}"

}
function generate_action_clean() {
    debug "** generate_action_deploy"
    for c in ${COMPONENTS};
    do
        echo "    ${c}_clean_build"
    done
}
function generate_action_cleanall() {
    debug "** generate_action_deploy"
    for c in ${COMPONENTS};
    do
        echo "    ${c}_clean_build"
        echo "    ${c}_clean_src_extracted"
    done
}

function generate_action_component() {
    debug "** generate_action_component"
    for c in ${COMPONENTS};
    do
        echo "${c})"
        echo "    ${c}_extract"
        echo "    action_set"
        echo "    ${c}_configure"
        echo "    ${c}_compile"
        echo "    ${c}_deploy"
        echo "    ;;"
        echo "${c}-extract)"
        echo "    ${c}_extract"
        echo "    ;;"
        echo "${c}-configure)"
        echo "    action_set"
        echo "    ${c}_extract"
        echo "    ${c}_configure"
        echo "    ;;"
        echo "${c}-compile)"
        echo "    action_set"
        echo "    ${c}_extract"
        echo "    ${c}_configure"
        echo "    ${c}_compile"
        echo "    ;;"
        echo "${c}-deploy)"
        echo "    action_set"
        echo "    ${c}_deploy"
        echo "    ;;"
        echo "${c}-help)"
        echo "    ${c}_extract"
        echo "    ${c}_help"
        echo "    ;;"

        echo "${c}-programmer-configure)"
        echo "    action_set"
        echo "    ${c}_programmer_configure"
        echo "    ;;"
        echo "${c}-programmer-compile)"
        echo "    action_set"
        echo "    ${c}_programmer_compile"
        echo "    ;;"
        echo "${c}-programmer-deploy)"
        echo "    action_set"
        echo "    ${c}_programmer_deploy"
        echo "    ;;"

        echo "${c}-clean)"
        echo "    ${c}_clean_build"
        echo "    ;;"

        echo "${c}-cleanall)"
        echo "    ${c}_clean_build"
        echo "    ${c}_clean_src_extracted"
        echo "    ;;"

        # case of linux-stm32mp
        if [ "${c}" = "linux-stm32mp" ]; then
            echo "${c}-dtb)"
            echo "    action_set"
            echo "    ${c}_dtb"
            echo "    ;;"
            echo "${c}-dtbs)"
            echo "    action_set"
            echo "    ${c}_dtbs"
            echo "    ;;"
            echo "${c}-images)"
            echo "    action_set"
            echo "    ${c}_images"
            echo "    ;;"
            echo "${c}-modules)"
            echo "    action_set"
            echo "    ${c}_modules"
            echo "    ;;"
            echo "${c}-menuconfig)"
            echo "    action_set"
            echo "    ${c}_menuconfig"
            echo "    ;;"
        fi
        # case of u-boot-stm32mp
        if [ "${c}" = "u-boot-stm32mp" ]; then
            echo "${c}-menuconfig)"
            echo "    action_set"
            echo "    ${c}_configure"
            echo "    ${c}_menuconfig"
            echo "    ;;"
        fi
    done
}
function generate_component_list() {
    debug "** generate_action_component_list"
    for c in ${COMPONENTS};
    do
        echo "    echo \"    ${c}\""
    done
}

function generate_sign_encrypt_action() {
    debug "** generate_sign_encrypt_action"
    if [ $SECURITY -eq 1 ]; then
        echo "sign-bl2)"
        echo "    sign_bl2"
        echo "    ;;"
        echo "sign-fip)"
        echo "    sign_fip"
        echo "    ;;"
        echo "sign-bl2-programmer)"
        echo "    sign_bl2_programmer"
        echo "    ;;"
        echo "sign-fip-programmer)"
        echo "    sign_fip_programmer"
        echo "    ;;"

        echo "sign-encrypt-bl2)"
        echo "    sign_encrypt_bl2"
        echo "    ;;"
        echo "sign-encrypt-fip)"
        echo "    sign_encrypt_fip"
        echo "    ;;"
        echo "sign-encrypt-bl2-programmer)"
        echo "    sign_encrypt_bl2_programmer"
        echo "    ;;"
        echo "sign-encrypt-fip-programmer)"
        echo "    sign_encrypt_fip_programmer"
        echo "    ;;"
    fi
    if $(echo $MACHINE  | grep -q stm32mp2-m33td) ; then
        echo "sign-m33-bl2)"
        echo "    sign_m33_bl2"
        echo "    ;;"
        echo "sign-m33-ddr)"
        echo "    sign_m33_ddr"
        echo "    ;;"
        echo "sign-m33-fw)"
        echo "    sign_m33_fw"
        echo "    ;;"
    fi
}

# Manage:
# @E> : extract
# @S> : set
# @C> : compile
# @c> : configure
# @F> : generate fip
# @M> : generate m33fw
# @MC>: module compile
# @PC>: compile programmer
# @PF>: generate programmer fip
function generate_component_function() {
    info "** generate_component_function"
    for c in $COMPONENTS;
    do
        info "** generate_component_function for ${c}"
        # search readme
        #debug "CMD: find ./${c}* ../${c}* ${SCRIPT_PATH}/../${c}* -name README.HOW_TO.txt.${MACHINE}"
        readme=$(find ./${c}* ../${c}* ${SCRIPT_PATH}/../${c}* -name README.HOW_TO.txt.${MACHINE} 2>/dev/null | head -n 1)
        #debug "CMD result: $readme"
        if [ -z "$readme" ]; then
            error "README.HOW_TO.txt for ${c} not found"
            continue
        fi
        debug "README: $readme"
        [ -e "$readme" ] || continue
        # extract info
        data=$(grep "\$@" ${readme})
        extract_nb=$(grep "\$@E>" ${readme} | wc -l)
        set_nb=$(grep "\$@S>" ${readme} | wc -l)
        compile_nb=$(grep "\$@C>" ${readme} | wc -l)
        fip_nb=$(grep "\$@F>" ${readme} | wc -l)
        m33fw_nb=$(grep "\$@M>" ${readme} | wc -l)
        programmer_compile_nb=$(grep "\$@PC>" ${readme} | wc -l)
        programmer_fip_nb=$(grep "\$@PF>" ${readme} | wc -l)
        configure_nb=$(grep "\$@c>" ${readme} | wc -l)
        debug "  extract_nb=$extract_nb"
        debug "  set_nb=$set_nb"
        debug "  compile_nb=$compile_nb"
        debug "  fip_nb=$fip_nb"
        debug "  m33fw_nb=$m33fw_nb"
        debug "  programmer_compile_nb=$programmer_compile_nb"
        debug "  programmer_fip_nb=$programmer_fip_nb"
        debug "  configure_nb=$configure_nb"
        echo "# --------------------------------------------------"
        echo "# ${c}"
        echo "# --------------------------------------------------"

        echo "function ${c}_help {"
        echo "    echo \"**** ${c}_help ****START****\""
        if [ $set_nb -gt 0 ]; then
            echo "    localpath=\$PWD"
            echo "    if [ ! -e source_code_extracted-${c} ]; then"
            echo "        ${c}_extract"
            echo "    fi"

            old_IFS=$IFS
            IFS=$'\n'
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@P>') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@P>||"
                    echo "${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@H') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@H>||"
                    echo "[ \$local_DRY_RUN -eq 0 ] && (${local_tmp_data})" | sed "s|[[:space:]]*\$@H>|   |"
                fi
            done
            IFS=$old_IFS
            echo "    cd \$localpath"
        fi
        echo "    echo \"**** ${c}_help ****END****\""
        echo "}"
        echo "function ${c}_extract {"
        echo "    echo \"**** ${c}_extract ****START****\""
        if [ $extract_nb -gt 0 ]; then
            echo "    localpath=\$PWD"
            echo "    if [ ! -e source_code_extracted-${c} ]; then"
            old_IFS=$IFS
            IFS=$'\n'
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@E') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@E>||"
                    echo "${local_tmp_data}" | sed "s|[[:space:]]*\$@E>|       |"
                fi
            done
            IFS=$old_IFS
            echo "        # create local fiel to indicate that source code are extracted"
            echo "        cd \$localpath"
            echo "        touch source_code_extracted-${c}"
            echo "    else"
            echo "        echo \"source code for ${c} already extracted\""
            echo "    fi"
            echo "    cd \$localpath"
        fi
        echo "    echo \"**** ${c}_extract ****END****\""
        echo "}"

        echo "function ${c}_set {"
        echo "    echo \"**** ${c}_set ****START****\""
        if [ $set_nb -gt 0 ]; then
            echo "    localpath=\$PWD"
            echo "    if [ ! -e source_code_extracted-${c} ]; then"
            echo "        ${c}_extract"
            echo "    fi"
            old_IFS=$IFS
            IFS=$'\n'
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@P>') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@P>||"
                    echo "${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            for d in ${data};
            do
                if $(echo ${c} | grep -q "external-dt") ; then
                    # set is not use and overwritten by externaldt_path
                    continue
                fi
                if $(echo ${d} | grep -q '@S') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@S>||"
                    echo "${local_tmp_data}" | sed "s|[[:space:]]*\$@S>|   |"
                fi
            done
            IFS=$old_IFS
            echo "    cd \$localpath"
        fi
        echo "    echo \"**** ${c}_set ****END****\""
        echo "}"

        echo "function ${c}_configure {"
        echo "    echo \"**** ${c}_configure ****START****\""
        if [ $configure_nb -gt 0 ]; then
            echo "    localpath=\$PWD"
            old_IFS=$IFS
            IFS=$'\n'
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@P>') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@P>||"
                    echo "${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            if $(echo ${c} | grep -q "linux-stm32mp") ; then
                echo "    if [ \"X\$KERNELRELEASE\" = \"X\" ]; then"
                echo "        make kernelversion && export VERBOSE_PARAM=\"\$VERBOSE_PARAM KERNELRELEASE=\$(make kernelversion)\""
                echo "        echo \"VERBOSE_PARAM=\$VERBOSE_PARAM\""
                echo "    fi"
            fi
            echo "    cmd    export BLD_PATH=\$(realpath \${your_build_subdir_path})"
            echo "    [ \$local_DRY_RUN -eq 0 ] && export BLD_PATH=\$(realpath \${your_build_subdir_path})"
            echo "    build_dir=\$(basename \${your_build_subdir_path})"
            if $(echo ${c} | grep -q "linux-stm32mp") ; then
                echo "    if [ ! -e ../source_code_configured-${c}-for-\${your_board_name}-\${build_dir} ]; then"
            fi
            if $(echo ${c} | grep -q "u-boot-stm32mp") ; then
                echo "    if [ ! -e ../source_code_configured-${c}-for-\${your_board_name}-\${build_dir} ]; then"
            fi
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@c') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@c>||"
                    echo "[ \$local_DRY_RUN -eq 0 ] && (${local_tmp_data}) || die ${c}" | sed "s|[[:space:]]*\$@c>|       |"
                fi
            done
            if $(echo ${c} | grep -q "linux-stm32mp") ; then
                echo "        [ \$local_DRY_RUN -eq 0 ] && touch ../source_code_configured-${c}-for-\${your_board_name}-\${build_dir}"
                echo "    fi"
            fi
            if $(echo ${c} | grep -q "u-boot-stm32mp") ; then
                echo "        [ \$local_DRY_RUN -eq 0 ] && touch ../source_code_configured-${c}-for-\${your_board_name}-\${build_dir}"
                echo "        if [ \"\$FIRMWARE_UPDATE_AB\" = \"1\" ]; then"
                echo "            if [ -n \"\$METADATA_IMAGE_NUMBER\" ]; then"
                echo "                sed -i \"s|^CONFIG_FWU_NUM_IMAGES_PER_BANK=.*$|CONFIG_FWU_NUM_IMAGES_PER_BANK=\${METADATA_IMAGE_NUMBER}|\" \${build_dir}/.config"
                echo "            fi"
                echo "        fi"
                echo "    fi"
            fi


            IFS=$old_IFS
            echo "    cd \$localpath"
        fi
        echo "    echo \"**** ${c}_configure ****END****\""
        echo "}"

        echo "function ${c}_compile {"
        echo "    echo \"**** ${c}_compile ****START****\""
        if [ $compile_nb -gt 0 ]; then
            echo "    localpath=\$PWD"
            old_IFS=$IFS
            IFS=$'\n'
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@P>') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@P>||"
                    echo "${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            echo "    cmd    export BLD_PATH=\$(realpath \${your_build_subdir_path})"
            echo "    [ \$local_DRY_RUN -eq 0 ] && export BLD_PATH=\$(realpath \${your_build_subdir_path})"
            if $(echo ${c} | grep -q "linux-stm32mp") ; then
                echo "    if [ \"X\$KERNELRELEASE\" = \"X\" ]; then"
                echo "        make kernelversion && export VERBOSE_PARAM=\"\$VERBOSE_PARAM KERNELRELEASE=\$(make kernelversion)\""
                echo "        echo \"VERBOSE_PARAM=\$VERBOSE_PARAM\""
                echo "    fi"
            fi
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@C') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@C.*>||"
                    echo "[ \$local_DRY_RUN -eq 0 ] && (${local_tmp_data}) || die ${c}" | sed "s|[[:space:]]*\$@C.*>|   |"
                fi
            done
            IFS=$old_IFS
            echo "    cd \$localpath"
        fi
        echo "    echo \"**** ${c}_compile ****END****\""
        echo "}"

        echo "function ${c}_deploy {"
        echo "    echo \"**** ${c}_deploy ****START****\""
        if [ $fip_nb -gt 0 ] || [ $m33fw_nb -gt 0 ]; then
            echo "    localpath=\$PWD"
            old_IFS=$IFS
            IFS=$'\n'
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@P>') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@P>||"
                    echo "${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            echo "    cmd    export BLD_PATH=\$(realpath \${your_build_subdir_path})"
            echo "    [ \$local_DRY_RUN -eq 0 ] && export BLD_PATH=\$(realpath \${your_build_subdir_path})"
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@F') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@F>||"
                    echo "[ \$local_DRY_RUN -eq 0 ] && (${local_tmp_data}) || die ${c}" | sed "s|[[:space:]]*\$@F>|   |"
                fi
            done
            echo "    cmd    export BLD_PATH=\$(realpath \${your_build_subdir_path})"
            echo "    [ \$local_DRY_RUN -eq 0 ] && export BLD_PATH=\$(realpath \${your_build_subdir_path})"
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@M>') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@M>||"
                    echo "[ \$local_DRY_RUN -eq 0 ] && (${local_tmp_data}) || die ${c}" | sed "s|[[:space:]]*\$@M>|   |"
                fi
            done
            IFS=$old_IFS
            echo "    cd \$localpath"
        fi
        echo "    echo \"**** ${c}_deploy ****END****\""
        echo "}"

        echo "function ${c}_programmer_configure {"
        echo "    echo \"**** ${c}_programmer_compile ****START****\""
        if [ $programmer_compile_nb -gt 0 ]; then
            echo "    localpath=\$PWD"
            old_IFS=$IFS
            IFS=$'\n'
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@P>') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@P>||"
                    echo "${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            echo "    cmd    export BLD_PATH=\$(realpath \${your_build_subdir_path}-programmer)"
            echo "    [ \$local_DRY_RUN -eq 0 ] && export BLD_PATH=\$(realpath \${your_build_subdir_path}-programmer)"
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@Pc') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@Pc>||"
                    echo "[ \$local_DRY_RUN -eq 0 ] && (${local_tmp_data}) || die ${c}" | sed "s|[[:space:]]*\$@PC>|   |"
                fi
            done
            IFS=$old_IFS
            echo "    cd \$localpath"
        fi
        echo "    echo \"**** ${c}_programmer_compile ****END****\""
        echo "}"

        echo "function ${c}_programmer_compile {"
        echo "    echo \"**** ${c}_programmer_compile ****START****\""
        if [ $programmer_compile_nb -gt 0 ]; then
            echo "    localpath=\$PWD"
            old_IFS=$IFS
            IFS=$'\n'
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@P>') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@P>||"
                    echo "${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            echo "    cmd    export BLD_PATH=\$(realpath \${your_build_subdir_path}-programmer)"
            echo "    [ \$local_DRY_RUN -eq 0 ] && export BLD_PATH=\$(realpath \${your_build_subdir_path}-programmer)"
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@PC') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@PC>||"
                    echo "[ \$local_DRY_RUN -eq 0 ] && (${local_tmp_data}) || die ${c}" | sed "s|[[:space:]]*\$@PC>|   |"
                fi
            done
            IFS=$old_IFS
            echo "    cd \$localpath"
        fi
        echo "    echo \"**** ${c}_programmer_compile ****END****\""
        echo "}"

        echo "function ${c}_programmer_deploy {"
        echo "    echo \"**** ${c}_programmer_deploy ****START****\""
        if [ $programmer_fip_nb -gt 0 ]; then
            echo "    localpath=\$PWD"
            old_IFS=$IFS
            IFS=$'\n'
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@P>') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@P>||"
                    echo "${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            echo "    cmd    export BLD_PATH=\$(realpath \${your_build_subdir_path}-programmer)"
            echo "    [ \$local_DRY_RUN -eq 0 ] && export BLD_PATH=\$(realpath \${your_build_subdir_path}-programmer)"
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@PF') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@PF>||"
                    echo "[ \$local_DRY_RUN -eq 0 ] && (${local_tmp_data}) || die ${c}" | sed "s|[[:space:]]*\$@PF>|   |"
                fi
            done
            IFS=$old_IFS
            echo "    cd \$localpath"
        fi
        echo "    echo \"**** ${c}_programmer_deploy ****END****\""
        echo "}"

        echo "function ${c}_clean_build {"
        echo "    echo \"**** ${c}_clean_buid ****START****\""
        if [ $extract_nb -gt 0 ]; then
            echo "    localpath=\$PWD"
            echo "    if [ -e source_code_extracted-${c} ]; then"
            old_IFS=$IFS
            IFS=$'\n'
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@P>') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "        cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@P>||"
                    echo "${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|       |"
                fi
            done
            IFS=$old_IFS
            if $(echo ${c} | grep -q "linux-stm32mp") ; then
                echo "        dir=\$(find .. -maxdepth 1 -type d | grep linux | tail -n 1)"
            else
                echo "        dir=\$(find .. -maxdepth 1 -type d | grep ${c} | tail -n 1)"
            fi
            echo "        # remove configured file step"
            echo "        build_dir=\$(basename \${your_build_subdir_path})"
            echo "        [ -e ../source_code_configured-${c}-for-\${your_board_name} ] && cmd rm ../source_code_configured-${c}-for-\${your_board_name}"
            echo "        [ -e ../source_code_configured-${c}-for-\${your_board_name}-\${build_dir} ] && cmd rm ../source_code_configured-${c}-for-\${your_board_name}-\${build_dir}"
            echo "        [ \$local_DRY_RUN -eq 0 ] && ([ -e ../source_code_configured-${c}-for-\${your_board_name} ] && rm ../source_code_configured-${c}-for-\${your_board_name})"
            echo "        [ \$local_DRY_RUN -eq 0 ] && ([ -e ../source_code_configured-${c}-for-\${your_board_name}-\${build_dir} ] && rm ../source_code_configured-${c}-for-\${your_board_name}-\${build_dir})"

            echo "        # remove build directory"
            echo "        [ -d \${your_build_subdir_path} ] && cmd rm -rf \${your_build_subdir_path}"
            echo "        [ \$local_DRY_RUN -eq 0 ] && ([ -d \${your_build_subdir_path} ] && rm -rf \${your_build_subdir_path})"
            echo "        [ -d \${your_build_subdir_path}-programmer ] && cmd rm -rf \${your_build_subdir_path}-programmer"
            echo "        [ \$local_DRY_RUN -eq 0 ] && ([ -d \${your_build_subdir_path}-programmer ] && rm -rf \${your_build_subdir_path}-programmer)"
            echo "    fi"
            echo "    cd \$localpath"
        fi
        echo "    echo \"**** ${c}_clean_build ****END****\""
        echo "}"

        echo "function ${c}_clean_src_extracted {"
        echo "    echo \"**** ${c}_clean_src_extracted ****START****\""
        if [ $extract_nb -gt 0 ]; then
            echo "    localpath=\$PWD"
            echo "    if [ -e source_code_extracted-${c} ]; then"
            old_IFS=$IFS
            IFS=$'\n'
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@P>') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "        cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@P>||"
                    echo "${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|       |"
                fi
            done
            IFS=$old_IFS
            echo "        cmd \"cd ..\""
            echo "        cd .."
            if $(echo ${c} | grep -q "linux-stm32mp") ; then
                echo "        dir=\$(find . -maxdepth 1 -type d | grep linux | tail -n 1)"
            else
                echo "        dir=\$(find . -maxdepth 1 -type d | grep ${c} | tail -n 1)"
            fi
            echo "        [ -d \$dir ] && cmd \"rm -rf \$dir\" ../source_code_extracted-${c}"
            echo "        [ \$local_DRY_RUN -eq 0 ] && ([ -d \$dir ] && rm -rf \$dir ../source_code_extracted-${c})"
            echo "    fi"
            echo "    cd \$localpath"
        fi
        echo "    echo \"**** ${c}_clean_src_extracted ****END****\""
        echo "}"

        # case of u-boot-stm32mp
        if [ "${c}" = "u-boot-stm32mp" ]; then
            echo "function ${c}_menuconfig {"
            echo "    echo \"**** ${c}_menuconfig ****START****\""
            echo "    localpath=\$PWD"
            old_IFS=$IFS
            IFS=$'\n'
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@P>') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@P>||"
                    echo "${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            IFS=$old_IFS
            local_tmp_data="    make O=\"\${your_build_subdir_path}/\${uboot_defconfig}\" menuconfig"
            echo "    cmd \"${local_tmp_data}\""
            echo "    [ \$local_DRY_RUN -eq 0 ] && (${local_tmp_data}) || die ${c}"
            echo "    build_dir=\$(basename \${your_build_subdir_path})"
            echo "    [ \$local_DRY_RUN -eq 0 ] && touch ../source_code_configured-${c}-for-\${your_board_name}-\${build_dir}"
            echo "    cd \$localpath"
            echo "    echo \"**** ${c}_menuconfig ****END****\""
            echo "}"
        fi
        # case of linux-stm32mp
        if [ "${c}" = "linux-stm32mp" ]; then
            echo "function ${c}_dtb {"
            echo "    echo \"**** ${c}_compile ****START****\""
            echo "    localpath=\$PWD"
            old_IFS=$IFS
            IFS=$'\n'
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@P>') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@P>||"
                    echo "${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            IFS=$old_IFS
            echo "    cmd    export BLD_PATH=\$(realpath \${your_build_subdir_path})"
            echo "    [ \$local_DRY_RUN -eq 0 ] && export BLD_PATH=\$(realpath \${your_build_subdir_path})"
            echo "    cmd  export OUTPUT_BUILD_DIR=\${your_build_subdir_path}"
            echo "    export OUTPUT_BUILD_DIR=\${your_build_subdir_path}"
            local_tmp_data="    make O=\"\$(realpath \${your_build_subdir_path})\" \${PARALLEL_MAKE} st/\${linux_dtb_name}.dtb KBUILD_EXTDTS=\${externaldt_path}/\${externaldt_linux_path}"
            echo "    cmd \"${local_tmp_data}\""
            echo "[ \$local_DRY_RUN -eq 0 ] && (${local_tmp_data}) || die ${c}"
            echo "    cd \$localpath"
            echo "    echo \"**** ${c}_dtb ****END****\""
            echo "}"

            echo "function ${c}_dtbs {"
            echo "    echo \"**** ${c}_dtbs ****START****\""
            echo "    localpath=\$PWD"
            old_IFS=$IFS
            IFS=$'\n'
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@P>') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@P>||"
                    echo "${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            IFS=$old_IFS
            echo "    cmd    export BLD_PATH=\$(realpath \${your_build_subdir_path})"
            echo "    [ \$local_DRY_RUN -eq 0 ] && export BLD_PATH=\$(realpath \${your_build_subdir_path})"
            echo "    cmd  export OUTPUT_BUILD_DIR=\${your_build_subdir_path}"
            echo "    export OUTPUT_BUILD_DIR=\${your_build_subdir_path}"
            local_tmp_data="    make O=\"\$(realpath \${your_build_subdir_path})\" \${PARALLEL_MAKE} dtbs KBUILD_EXTDTS=\${externaldt_path}/\${externaldt_linux_path}"
            echo "    cmd \"${local_tmp_data}\""
            echo "[ \$local_DRY_RUN -eq 0 ] && (${local_tmp_data}) || die ${c}"
            echo "    cd \$localpath"
            echo "    echo \"**** ${c}_dtbs ****END****\""
            echo "}"

            echo "function ${c}_modules {"
            echo "    echo \"**** ${c}_modules ****START****\""
            echo "    localpath=\$PWD"
            old_IFS=$IFS
            IFS=$'\n'
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@P>') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@P>||"
                    echo "${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            IFS=$old_IFS
            echo "    cmd    export BLD_PATH=\$(realpath \${your_build_subdir_path})"
            echo "    [ \$local_DRY_RUN -eq 0 ] && export BLD_PATH=\$(realpath \${your_build_subdir_path})"
            echo "    cmd  export OUTPUT_BUILD_DIR=\${your_build_subdir_path}"
            echo "    export OUTPUT_BUILD_DIR=\${your_build_subdir_path}"
            local_tmp_data="    make O=\"\${your_build_subdir_path}\" \${PARALLEL_MAKE} modules modules_install INSTALL_MOD_PATH=\"\${your_build_subdir_path}/install_artifact\" KBUILD_EXTDTS=\${externaldt_path}/\${externaldt_linux_path}"
            echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@C>||"
            echo "[ \$local_DRY_RUN -eq 0 ] && ${local_tmp_data} || die ${c}" | sed "s|[[:space:]]*\$@C>|   |"

            echo "    cmd \" mkdir -p \${your_deploy_dir_path}/kernel/modules\""
            echo "    [ \$local_DRY_RUN -eq 0 ] &&  mkdir -p \${your_deploy_dir_path}/kernel/modules || die linux-stm32mp"
            echo "    cmd \" mkdir -p \${your_deploy_dir_path}/kernel/modules_stripped\""
            echo "    [ \$local_DRY_RUN -eq 0 ] && mkdir -p \${your_deploy_dir_path}/kernel/modules_stripped || die linux-stm32mp"
            echo "    cmd \" rm \${your_build_subdir_path}/install_artifact/lib/modules/*/build\""
            echo "    [ \$local_DRY_RUN -eq 0 ] && rm \${your_build_subdir_path}/install_artifact/lib/modules/*/build || die linux-stm32mp"
            echo "    cmd \" cp -ar \${your_build_subdir_path}/install_artifact/* \${your_deploy_dir_path}/kernel/modules\""
            echo "    [ \$local_DRY_RUN -eq 0 ] && cp -ar \${your_build_subdir_path}/install_artifact/* \${your_deploy_dir_path}/kernel/modules || die linux-stm32mp"
            echo "    cmd \" cp -ar \${your_build_subdir_path}/install_artifact/* \${your_deploy_dir_path}/kernel/modules_stripped\""
            echo "    [ \$local_DRY_RUN -eq 0 ] && cp -ar \${your_build_subdir_path}/install_artifact/* \${your_deploy_dir_path}/kernel/modules_stripped || die linux-stm32mp"
            echo "    cmd \" find \${your_deploy_dir_path}/kernel/modules_stripped -name "*.ko" | xargs \$STRIP --strip-debug --remove-section=.comment --remove-section=.note --preserve-dates\""
            echo "    [ \$local_DRY_RUN -eq 0 ] && find \${your_deploy_dir_path}/kernel/modules_stripped -name \"*.ko\" | xargs \$STRIP --strip-debug --remove-section=.comment --remove-section=.note --preserve-dates || die linux-stm32mp"

            echo "    cd \$localpath"
            echo "    echo \"**** ${c}_modules ****END****\""
            echo "}"

            echo "function ${c}_images {"
            echo "    echo \"**** ${c}_images ****START****\""
            echo "    localpath=\$PWD"
            old_IFS=$IFS
            IFS=$'\n'
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@P>') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@P>||"
                    echo "${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            echo "    cmd    export BLD_PATH=\$(realpath \${your_build_subdir_path})"
            echo "    [ \$local_DRY_RUN -eq 0 ] && export BLD_PATH=\$(realpath \${your_build_subdir_path})"
            echo "    cmd  export OUTPUT_BUILD_DIR=\${your_build_subdir_path}"
            echo "    export OUTPUT_BUILD_DIR=\${your_build_subdir_path}"
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@CI') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@CI>||"
                    echo "[ \$local_DRY_RUN -eq 0 ] && (${local_tmp_data}) || die ${c}" | sed "s|[[:space:]]*\$@CI>|   |"
                fi
            done
            IFS=$old_IFS
            echo "    cd \$localpath"
            echo "    echo \"**** ${c}_images ****END****\""
            echo "}"

            echo "function ${c}_menuconfig {"
            echo "    echo \"**** ${c}_menuconfig ****START****\""
            echo "    localpath=\$PWD"
            old_IFS=$IFS
            IFS=$'\n'
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@P>') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@P>||"
                    echo "${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done

            echo "    cmd    export BLD_PATH=\$(realpath \${your_build_subdir_path})"
            echo "    [ \$local_DRY_RUN -eq 0 ] && export BLD_PATH=\$(realpath \${your_build_subdir_path})"
            echo "    cmd  export OUTPUT_BUILD_DIR=\${your_build_subdir_path}"
            echo "    export OUTPUT_BUILD_DIR=\${your_build_subdir_path}"
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@MC') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@MC>||"
                    echo "[ \$local_DRY_RUN -eq 0 ] && (${local_tmp_data}) || die ${c}" | sed "s|[[:space:]]*\$@MC>|   |"
                fi
            done
            IFS=$old_IFS
            echo "    build_dir=\$(basename \${your_build_subdir_path})"
            echo "    [ \$local_DRY_RUN -eq 0 ] && touch ../source_code_configured-${c}-for-\${your_board_name}-\${build_dir}"
            echo "    cd \$localpath"
            echo "    echo \"**** ${c}_menuconfig ****END****\""
            echo "}"

        fi
    done
}

# ------------------------------------------
#
# ------------------------------------------
function generate_signature_encryption_bl2_a_function() {
    echo "function _sign_bl2 {"
    echo "    if [ \$# -eq 0 ]; then"
    echo "        local local_board_name=\${your_board_name}"
    echo "    else"
    echo "        local local_board_name=\$1"
    echo "    fi"
    echo "    echo \"**** sign_bl2 ****START****\""
    echo "    if [ \${SIGN_ENABLE} -eq 0 ]; then"
    echo "        die \"[ERROR] You MUST set the flag SIGN_ENABLE to 1 to perform signature\""
    echo "    fi"
    echo "    if [ \${SIGN_ENABLE} -eq 1 -a \${ENCRYPT_ENABLE} -eq 1 ]; then"
    echo "        die \"[ERROR] This function only perform signature, you need to call 'sign-encrypt-bl2'\""
    echo "    fi"
    echo "    if [ ! -e \${SIGN_TOOL} ]; then"
    echo "        die \"[ERROR]: You MUST specifiy a correct path for 'SIGN_TOOL'\""
    echo "    fi"
    echo "    if [ \${SIGN_ENABLE} -eq 1 -a \${ENCRYPT_ENABLE} -eq 0 ]; then"
    echo "        # verify if bl2 binary to sign is present"
    echo "        local board_name=\${your_deploy_dir_path}/arm-trusted-firmware/tf-a-\${local_board_name}-\${your_storage_boot_scheme_cortex_a}"
    echo "        if [ ! -e \${your_deploy_dir_path}/arm-trusted-firmware/tf-a-\${local_board_name}-\${your_storage_boot_scheme_cortex_a}.stm32 ]; then"
    echo "            if [ -e \${your_deploy_dir_path}/arm-trusted-firmware/tf-a-\${local_board_name}.stm32 ]; then"
    echo "                  board_name=\${your_deploy_dir_path}/arm-trusted-firmware/tf-a-\${local_board_name}"
    echo "            else"
    echo "                  echo \"[ERROR] the file \${your_deploy_dir_path}/arm-trusted-firmware/tf-a-\${local_board_name}-\${your_storage_boot_scheme_cortex_a}.stm32 doesn't exist\""
    echo "                  die \"[ERROR] You need to compile it via tf-a or compile-for-fip\""
    echo "            fi"
    echo "        fi"
    echo "        if [ \$local_DRY_RUN -eq 1 ]; then"
    echo "          echo \${SIGN_TOOL} \\"
    echo "            -bin \${board_name}.stm32  \\"
    echo "            -o  \${board_name}\${signature_suffix}.stm32 \\"
    echo "            --password \${SIGN_KEY_PASS} \\"
    echo "            --public-key \${SIGN_KEY_PUB} \\"
    echo "            --private-key \${SIGN_KEY} \\"
    echo "            --type fsbl --silent \\"
    echo "            \${SIGN_TOOL_EXTRA}"
    echo "        else"
    echo "          \${SIGN_TOOL} \\"
    echo "            -bin \${board_name}.stm32  \\"
    echo "            -o  \${board_name}\${signature_suffix}.stm32 \\"
    echo "            --password \${SIGN_KEY_PASS} \\"
    echo "            --public-key \${SIGN_KEY_PUB} \\"
    echo "            --private-key \${SIGN_KEY} \\"
    echo "            --type fsbl --silent \\"
    echo "            \${SIGN_TOOL_EXTRA}"
    echo "        fi"
    echo "    fi"
    echo "    echo \"**** sign_bl2 ****END****\""
    echo "}"
    echo ""
    echo "function _sign_encrypt_bl2 {"
    echo "    if [ \$# -eq 0 ]; then"
    echo "        local local_board_name=\${your_board_name}"
    echo "    else"
    echo "        local local_board_name=\$1"
    echo "    fi"
    echo "    echo \"**** sign-encrypt-bl2 ****START****\""
    echo "    if [ \${SIGN_ENABLE} -eq 0 ]; then"
    echo "        die \"[ERROR] You MUST set the flag SIGN_ENABLE to 1 to perform signature and encryption\""
    echo "    fi"
    echo "    if [ \${SIGN_ENABLE} -eq 1 -a \${ENCRYPT_ENABLE} -eq 0 ]; then"
    echo "        [ \$local_DRY_RUN -eq 1 ] && echo \"[ERROR] This function only perform signature and encryption, you need to call 'sign-bl2'\""
    echo "        die \"[ERROR] This function only perform signature and encryption, you need to call 'sign-bl2'\""
    echo "    fi"
    echo "    if [ ! -e \${SIGN_TOOL} ]; then"
    echo "        die \"[ERROR]: You MUST specifiy a correct path for 'SIGN_TOOL'\""
    echo "    fi"
    echo "    if [ \${SIGN_ENABLE} -eq 1 -a \${ENCRYPT_ENABLE} -eq 1 ]; then"
    echo "        # verify if bl2 binary to sign is present"
    echo "        local board_name=\${your_deploy_dir_path}/arm-trusted-firmware/tf-a-\${local_board_name}-\${your_storage_boot_scheme_cortex_a}"
    echo "        if [ ! -e \${your_deploy_dir_path}/arm-trusted-firmware/tf-a-\${local_board_name}-\${your_storage_boot_scheme_cortex_a}.stm32 ]; then"
    echo "            if [ -e \${your_deploy_dir_path}/arm-trusted-firmware/tf-a-\${local_board_name}.stm32 ]; then"
    echo "                  board_name=\${your_deploy_dir_path}/arm-trusted-firmware/tf-a-\${local_board_name}"
    echo "            else"
    echo "                  echo \"[ERROR] the file \${your_deploy_dir_path}/arm-trusted-firmware/tf-a-\${local_board_name}-\${your_storage_boot_scheme_cortex_a}.stm32 doesn't exist\""
    echo "                  die \"[ERROR] You need to compile it via tf-a or compile-for-fip\""
    echo "            fi"
    echo "        fi"
    echo "        if [ \$local_DRY_RUN -eq 1 ]; then"
    echo "          echo \${SIGN_TOOL} \\"
    echo "            -bin \${board_name}.stm32  \\"
    echo "            -o \${board_name}\${encryption_suffix}\${signature_suffix}.stm32 \\"
    echo "            --password \${SIGN_KEY_PASS} \\"
    echo "            --public-key \${SIGN_KEY_PUB} \\"
    echo "            --private-key \${SIGN_KEY} \\"
    echo "            --type fsbl --silent \\"
    echo "            \${SIGN_ENCRYPT_TOOL_EXTRA} \\"
    echo "            --enc-key \${ENCRYPT_FSBL_KEY} \\"
    echo "            --enc-dc \${TF_A_ENCRYPT_DC} \\"
    echo "            --image-version 0"
    echo "        else"
    echo "          \${SIGN_TOOL} \\"
    echo "            -bin \${board_name}.stm32  \\"
    echo "            -o \${board_name}\${encryption_suffix}\${signature_suffix}.stm32 \\"
    echo "            --password \${SIGN_KEY_PASS} \\"
    echo "            --public-key \${SIGN_KEY_PUB} \\"
    echo "            --private-key \${SIGN_KEY} \\"
    echo "            --type fsbl --silent \\"
    echo "            \${SIGN_ENCRYPT_TOOL_EXTRA} \\"
    echo "            --enc-key \${ENCRYPT_FSBL_KEY} \\"
    echo "            --enc-dc \${TF_A_ENCRYPT_DC} \\"
    echo "            --image-version 0"
    echo "        fi"
    echo "    fi"
    echo "    echo \"**** sign-encrypt-bl2 ****END****\""
    echo "}"
}
function generate_signature_encryption_bl2_a_function_addons() {
    echo "function sign_bl2 {"
    echo "    _sign_bl2 \${your_board_name} ''"
    echo "}"
    echo "function sign_encrypt_bl2 {"
    echo "    _sign_encrypt_bl2 \${tfa_dt_name} ''"
    echo "}"
    echo "function sign_bl2_programmer {"
    echo "    _sign_bl2 \${your_board_name}-programmer-usb ''"
    echo "}"
    echo "function sign_encrypt_bl2_programmer {"
    echo "    _sign_encrypt_bl2 \${your_board_name}-programmer-usb ''"
    echo "}"
}

function generate_signature_encryption_fip_a_stm32mp1_function() {
    echo "function _sign_fip {"
    echo "    if [ \$# -eq 0 ]; then"
    echo "        local local_board_name=\${your_board_name}"
    echo "        local local_addons_parameters="
    echo "    else"
    echo "        local local_board_name=\$1"
    echo "        shift;"
    echo "        local local_addons_parameters=\$@"
    echo "    fi"
    echo "    echo \"**** sign_fip ****START****\""
    echo "    if [ \${SIGN_ENABLE} -eq 0 ]; then"
    echo "        die \"[ERROR] You MUST set the flag SIGN_ENABLE to 1 to perform signature\""
    echo "    fi"
    echo "    if [ \${SIGN_ENABLE} -eq 1 -a \${ENCRYPT_ENABLE} -eq 1 ]; then"
    echo "        [ \$local_DRY_RUN -eq 1 ] && echo \"[ERROR] This function only perform signature, you need to call 'sign-encrypt-fip'\""
    echo "        die \"[ERROR] This function only perform signature, you need to call 'sign-encrypt-fip'\""
    echo "    fi"
    echo "    if [ \${SIGN_ENABLE} -eq 1 -a \${ENCRYPT_ENABLE} -eq 0 ]; then"
    echo "        if [ \$local_DRY_RUN -eq 1 ]; then"
    echo "          echo FIP_DEPLOYDIR_ROOT=\${your_deploy_dir_path} ENCRYPT_SUFFIX=\${encryption_suffix} SIGN_SUFFIX=\${signature_suffix} create_st_fip_binary.sh \\"
    echo "            --sign \\"
    echo "            --signature-key \${SIGN_KEY} \\"
    echo "            --signature-key-pass \${SIGN_KEY_PASS_FOR_SPECIFC_KEY} \\"
    echo "            --search-secondary-config default:\${your_storage_boot_scheme_security} \\"
    echo "            --search-configuration \${your_storage_boot_scheme_cortex_a} \\"
    echo "            --search-devicetree \${local_board_name} \\"
    echo "            --search-soc-name \${your_soc_name} \\"
    echo "            \${local_addons_parameters} \\"
    echo "            --output \${your_deploy_dir_path}/fip"
    echo "       else"
    echo "          FIP_DEPLOYDIR_ROOT=\${your_deploy_dir_path} ENCRYPT_SUFFIX=\${encryption_suffix} SIGN_SUFFIX=\${signature_suffix} create_st_fip_binary.sh \\"
    echo "            --sign \\"
    echo "            --signature-key \${SIGN_KEY} \\"
    echo "            --signature-key-pass \${SIGN_KEY_PASS_FOR_SPECIFC_KEY} \\"
    echo "            --search-secondary-config default:\${your_storage_boot_scheme_security} \\"
    echo "            --search-configuration \${your_storage_boot_scheme_cortex_a} \\"
    echo "            --search-devicetree \${local_board_name} \\"
    echo "            --search-soc-name \${your_soc_name} \\"
    echo "            \${local_addons_parameters} \\"
    echo "            --output \${your_deploy_dir_path}/fip"
    echo "       fi"
    echo "    fi"
    echo "    echo \"**** sign_fip ****END****\""
    echo "}"

    echo ""
    echo "function _sign_encrypt_fip {"
    echo "    echo \"**** sign-encrypt-fip ****START****\""
    echo "     if [ \${your_soc_name} = \"stm32mp15\" ]; then"
    echo "        die \"[ERROR] This functionality is not supported for stm32mp15 platform\""
    echo "     fi"
    echo ""
    echo "    if [ \$# -eq 0 ]; then"
    echo "        local local_board_name=\${your_board_name}"
    echo "        local local_addons_parameters="
    echo "    else"
    echo "        local local_board_name=\$1"
    echo "        shift;"
    echo "        local local_addons_parameters=\$@"
    echo "    fi"
    echo "    if [ \${SIGN_ENABLE} -eq 0 ]; then"
    echo "        die \"[ERROR] You MUST set the flag SIGN_ENABLE to 1 to perform signature and encryption\""
    echo "    fi"
    echo "    if [ \${SIGN_ENABLE} -eq 1 -a \${ENCRYPT_ENABLE} -eq 0 ]; then"
    echo "        [ \$local_DRY_RUN -eq 1 ] && echo \"[ERROR] This function only perform signature and encryption, you need to call 'sign-fip'\""
    echo "        die \"[ERROR] This function only perform signature and encryption, you need to call 'sign-fip'\""
    echo "    fi"
    echo "    if [ \${SIGN_ENABLE} -eq 1 -a \${ENCRYPT_ENABLE} -eq 1 ]; then"
    echo "        # For FIP"
    echo "        if [ \$local_DRY_RUN -eq 1 ]; then"
    echo "          echo FIP_DEPLOYDIR_ROOT=\${your_deploy_dir_path} ENCRYPT_SUFFIX=\${encryption_suffix} SIGN_SUFFIX=\${signature_suffix} create_st_fip_binary.sh \\"
    echo "            --sign \\"
    echo "            --signature-key \${SIGN_KEY} \\"
    echo "            --signature-key-pass \${SIGN_KEY_PASS_FOR_SPECIFC_KEY} \\"
    echo "            --encrypt \${ENCRYPT_FIP_KEY} \\"
    echo "            --search-secondary-config default:\${your_storage_boot_scheme_security} \\"
    echo "            --search-configuration \${your_storage_boot_scheme_cortex_a} \\"
    echo "            --search-devicetree \${local_board_name} \\"
    echo "            --search-soc-name \${your_soc_name} \\"
    echo "            \${local_addons_parameters} \\"
    echo "            --output \${your_deploy_dir_path}/fip"
    echo "        else"
    echo "          FIP_DEPLOYDIR_ROOT=\${your_deploy_dir_path} ENCRYPT_SUFFIX=\${encryption_suffix} SIGN_SUFFIX=\${signature_suffix} create_st_fip_binary.sh \\"
    echo "            --sign \\"
    echo "            --signature-key \${SIGN_KEY} \\"
    echo "            --signature-key-pass \${SIGN_KEY_PASS_FOR_SPECIFC_KEY} \\"
    echo "            --encrypt \${ENCRYPT_FIP_KEY} \\"
    echo "            --search-secondary-config default:\${your_storage_boot_scheme_security} \\"
    echo "            --search-configuration \${your_storage_boot_scheme_cortex_a} \\"
    echo "            --search-devicetree \${local_board_name} \\"
    echo "            --search-soc-name \${your_soc_name} \\"
    echo "            \${local_addons_parameters} \\"
    echo "            --output \${your_deploy_dir_path}/fip"
    echo "       fi"
    echo "    fi"
    echo "    echo \"**** sign-encrypt-bl2 ****END****\""
    echo "}"
}
function generate_signature_encryption_fip_a_stm32mp2_function() {
    echo "function _sign_fip {"
    echo "    if [ \$# -eq 0 ]; then"
    echo "        local local_board_name=\${your_board_name}"
    echo "        local local_addons_parameters="
    echo "    else"
    echo "        local local_board_name=\$1"
    echo "        shift;"
    echo "        local local_addons_parameters=\$@"
    echo "    fi"
    echo "    echo \"**** sign_fip ****START****\""
    echo "    if [ \${SIGN_ENABLE} -eq 0 ]; then"
    echo "        die \"[ERROR] You MUST set the flag SIGN_ENABLE to 1 to perform signature\""
    echo "    fi"
    echo "    if [ \${SIGN_ENABLE} -eq 1 -a \${ENCRYPT_ENABLE} -eq 1 ]; then"
    echo "        [ \$local_DRY_RUN -eq 1 ] && echo \"[ERROR] This function only perform signature, you need to call 'sign-encrypt-fip'\""
    echo "        die \"[ERROR] This function only perform signature, you need to call 'sign-encrypt-fip'\""
    echo "    fi"
    echo "    if [ \${SIGN_ENABLE} -eq 1 -a \${ENCRYPT_ENABLE} -eq 0 ]; then"
    echo "        if [ \$local_DRY_RUN -eq 1 ]; then"
    echo "          # For FIP DDR"
    echo "          echo FIP_DEPLOYDIR_ROOT=\${your_deploy_dir_path} ENCRYPT_SUFFIX=\${encryption_suffix} SIGN_SUFFIX=\${signature_suffix} create_st_fip_binary.sh \\"
    echo "            --use-ddr --generate-only-ddr \\"
    echo "            --use-bl31 \\"
    echo "            --sign \\"
    echo "            --signature-key \${SIGN_KEY} \\"
    echo "            --signature-key-pass \${SIGN_KEY_PASS_FOR_SPECIFC_KEY} \\"
    echo "            --search-secondary-config default:\${your_storage_boot_scheme_security} \\"
    echo "            --search-configuration \${your_storage_boot_scheme_cortex_a} \\"
    echo "            --search-devicetree \${local_board_name} \\"
    echo "            --search-soc-name \${your_soc_name} \\"
    echo "            \${local_addons_parameters} \\"
    echo "            --output \${your_deploy_dir_path}/fip"
    echo "          # For FIP "
    echo "          echo FIP_DEPLOYDIR_ROOT=\${your_deploy_dir_path} ENCRYPT_SUFFIX=\${encryption_suffix} SIGN_SUFFIX=\${signature_suffix} create_st_fip_binary.sh \\"
    echo "            --use-bl31 \\"
    echo "            --use-ddr \\"
    echo "            --sign \\"
    echo "            --signature-key \${SIGN_KEY} \\"
    echo "            --signature-key-pass \${SIGN_KEY_PASS_FOR_SPECIFC_KEY} \\"
    echo "            --search-secondary-config default:\${your_storage_boot_scheme_security} \\"
    echo "            --search-configuration \${your_storage_boot_scheme_cortex_a} \\"
    echo "            --search-devicetree \${local_board_name} \\"
    echo "            --search-soc-name \${your_soc_name} \\"
    echo "            \${local_addons_parameters} \\"
    echo "            --output \${your_deploy_dir_path}/fip"
    echo "       else"
    echo "          # For FIP DDR"
    echo "          FIP_DEPLOYDIR_ROOT=\${your_deploy_dir_path} ENCRYPT_SUFFIX=\${encryption_suffix} SIGN_SUFFIX=\${signature_suffix} create_st_fip_binary.sh \\"
    echo "            --use-ddr --generate-only-ddr \\"
    echo "            --use-bl31 \\"
    echo "            --sign \\"
    echo "            --signature-key \${SIGN_KEY} \\"
    echo "            --signature-key-pass \${SIGN_KEY_PASS_FOR_SPECIFC_KEY} \\"
    echo "            --search-secondary-config default:\${your_storage_boot_scheme_security} \\"
    echo "            --search-configuration \${your_storage_boot_scheme_cortex_a} \\"
    echo "            --search-devicetree \${local_board_name} \\"
    echo "            --search-soc-name \${your_soc_name} \\"
    echo "            \${local_addons_parameters} \\"
    echo "            --output \${your_deploy_dir_path}/fip"
    echo "          # For FIP "
    echo "          FIP_DEPLOYDIR_ROOT=\${your_deploy_dir_path} ENCRYPT_SUFFIX=\${encryption_suffix} SIGN_SUFFIX=\${signature_suffix} create_st_fip_binary.sh \\"
    echo "            --use-bl31 \\"
    echo "            --use-ddr \\"
    echo "            --sign \\"
    echo "            --signature-key \${SIGN_KEY} \\"
    echo "            --signature-key-pass \${SIGN_KEY_PASS_FOR_SPECIFC_KEY} \\"
    echo "            --search-secondary-config default:\${your_storage_boot_scheme_security} \\"
    echo "            --search-configuration \${your_storage_boot_scheme_cortex_a} \\"
    echo "            --search-devicetree \${local_board_name} \\"
    echo "            --search-soc-name \${your_soc_name} \\"
    echo "            \${local_addons_parameters} \\"
    echo "            --output \${your_deploy_dir_path}/fip"
    echo "       fi"
    echo "    fi"
    echo "    echo \"**** sign_fip ****END****\""
    echo "}"
    echo ""
    echo "function _sign_encrypt_fip {"
    echo "    if [ \$# -eq 0 ]; then"
    echo "        local local_board_name=\${your_board_name}"
    echo "        local local_addons_parameters="
    echo "    else"
    echo "        local local_board_name=\$1"
    echo "        shift;"
    echo "        local local_addons_parameters=\$@"
    echo "    fi"

    echo "    echo \"**** sign-encrypt-fip ****START****\""
    echo "    if [ \${SIGN_ENABLE} -eq 0 ]; then"
    echo "        die \"[ERROR] You MUST set the flag SIGN_ENABLE to 1 to perform signature and encryption\""
    echo "    fi"
    echo "    if [ \${SIGN_ENABLE} -eq 1 -a \${ENCRYPT_ENABLE} -eq 0 ]; then"
    echo "        [ \$local_DRY_RUN -eq 1 ] && echo \"[ERROR] This function only perform signature and encryption, you need to call 'sign-fip'\""
    echo "        die \"[ERROR] This function only perform signature and encryption, you need to call 'sign-fip'\""
    echo "    fi"
    echo "    if [ \${SIGN_ENABLE} -eq 1 -a \${ENCRYPT_ENABLE} -eq 1 ]; then"
    echo "        if [ \$local_DRY_RUN -eq 1 ]; then"
    echo "          # For FIP DDR"
    echo "          echo FIP_DEPLOYDIR_ROOT=\${your_deploy_dir_path} ENCRYPT_SUFFIX=\${encryption_suffix} SIGN_SUFFIX=\${signature_suffix} create_st_fip_binary.sh \\"
    echo "            --use-ddr --generate-only-ddr \\"
    echo "            --use-bl31 \\"
    echo "            --sign \\"
    echo "            --signature-key \${SIGN_KEY} \\"
    echo "            --signature-key-pass \${SIGN_KEY_PASS_FOR_SPECIFC_KEY} \\"
    echo "            --encrypt \${ENCRYPT_FIP_KEY} \\"
    echo "            --search-secondary-config default:\${your_storage_boot_scheme_security} \\"
    echo "            --search-configuration \${your_storage_boot_scheme_cortex_a} \\"
    echo "            --search-devicetree \${local_board_name} \\"
    echo "            --search-soc-name \${your_soc_name} \\"
    echo "            \${local_addons_parameters} \\"
    echo "            --output \${your_deploy_dir_path}/fip"
    echo "          # For FIP"
    echo "          echo FIP_DEPLOYDIR_ROOT=\${your_deploy_dir_path} ENCRYPT_SUFFIX=\${encryption_suffix} SIGN_SUFFIX=\${signature_suffix} create_st_fip_binary.sh \\"
    echo "            --use-bl31 \\"
    echo "            --use-ddr \\"
    echo "            --sign \\"
    echo "            --signature-key \${SIGN_KEY} \\"
    echo "            --signature-key-pass \${SIGN_KEY_PASS_FOR_SPECIFC_KEY} \\"
    echo "            --encrypt \${ENCRYPT_FIP_KEY} \\"
    echo "            --search-secondary-config default:\${your_storage_boot_scheme_security} \\"
    echo "            --search-configuration \${your_storage_boot_scheme_cortex_a} \\"
    echo "            --search-devicetree \${local_board_name} \\"
    echo "            --search-soc-name \${your_soc_name} \\"
    echo "            \${local_addons_parameters} \\"
    echo "            --output \${your_deploy_dir_path}/fip"
    echo "       else"
    echo "          # For FIP DDR"
    echo "          FIP_DEPLOYDIR_ROOT=\${your_deploy_dir_path} ENCRYPT_SUFFIX=\${encryption_suffix} SIGN_SUFFIX=\${signature_suffix} create_st_fip_binary.sh \\"
    echo "            --use-ddr --generate-only-ddr \\"
    echo "            --use-bl31 \\"
    echo "            --sign \\"
    echo "            --signature-key \${SIGN_KEY} \\"
    echo "            --signature-key-pass \${SIGN_KEY_PASS_FOR_SPECIFC_KEY} \\"
    echo "            --encrypt \${ENCRYPT_FIP_KEY} \\"
    echo "            --search-secondary-config default:\${your_storage_boot_scheme_security} \\"
    echo "            --search-configuration \${your_storage_boot_scheme_cortex_a} \\"
    echo "            --search-devicetree \${local_board_name} \\"
    echo "            --search-soc-name \${your_soc_name} \\"
    echo "            \${local_addons_parameters} \\"
    echo "            --output \${your_deploy_dir_path}/fip"
    echo "          # For FIP"
    echo "          FIP_DEPLOYDIR_ROOT=\${your_deploy_dir_path} ENCRYPT_SUFFIX=\${encryption_suffix} SIGN_SUFFIX=\${signature_suffix} create_st_fip_binary.sh \\"
    echo "            --use-bl31 \\"
    echo "            --use-ddr \\"
    echo "            --sign \\"
    echo "            --signature-key \${SIGN_KEY} \\"
    echo "            --signature-key-pass \${SIGN_KEY_PASS_FOR_SPECIFC_KEY} \\"
    echo "            --encrypt \${ENCRYPT_FIP_KEY} \\"
    echo "            --search-secondary-config default:\${your_storage_boot_scheme_security} \\"
    echo "            --search-configuration \${your_storage_boot_scheme_cortex_a} \\"
    echo "            --search-devicetree \${local_board_name} \\"
    echo "            --search-soc-name \${your_soc_name} \\"
    echo "            \${local_addons_parameters} \\"
    echo "            --output \${your_deploy_dir_path}/fip"
    echo "       fi"
    echo "    fi"
    echo "    echo \"**** sign-encrypt-fip ****END****\""
    echo "}"
}
function generate_signature_encryption_fip_a_stm32mp12_function_addons() {
    echo "function sign_fip {"
    echo "    _sign_fip \${your_board_name} ''"
    echo "}"
    echo "function sign_encrypt_fip {"
    echo "    _sign_encrypt_fip \${your_board_name} ''"
    echo "}"

    echo "function sign_fip_programmer {"
    echo "    echo \"**** sign_fip programmer****START****\""
    echo "    _sign_fip \${your_board_name} --search-storage usb --search-secondary-config programmer:\${your_storage_boot_scheme_security}-programmer-usb:\${your_storage_boot_scheme_security}-programmer"
    echo "    echo \"**** sign_fip programmer****END****\""
    echo "}"
    echo "function sign_encrypt_fip_programmer {"
    echo "    echo \"**** sign-encrypt-fip programmer****START****\""
    echo "    _sign_encrypt_fip \${your_board_name} --search-storage usb --search-secondary-config programmer:\${your_storage_boot_scheme_security}-programmer-usb:\${your_storage_boot_scheme_security}-programmer"
    echo "    echo \"**** sign-encrypt-fip programmer****END****\""
    echo "}"
}

function generate_signature_encryption_bl2_a_stm32mp2-m33td_function_addons() {
    echo "function sign_bl2 {"
    echo "    _sign_bl2 \${tfa_dt_name} ''"
    echo "}"
    echo "function sign_encrypt_bl2 {"
    echo "    _sign_encrypt_bl2 \${tfa_dt_name} ''"
    echo "}"
    echo "function sign_bl2_programmer {"
    echo "    _sign_bl2 \${tfa_dt_programmer_name}-\${your_storage_boot_scheme_security}-programmer-usb ''"
    echo "}"
    echo "function sign_encrypt_bl2_programmer {"
    echo "    _sign_encrypt_bl2 \${tfa_dt_programmer_name}-\${your_storage_boot_scheme_security}-programmer-usb ''"
    echo "}"
}

function generate_signature_encryption_fip_a_stm32mp2-m33td_function() {
    echo "function _sign_fip {"
    echo "    if [ \$# -eq 0 ]; then"
    echo "        local local_board_name=\${your_board_name}"
    echo "        local local_addons_parameters="
    echo "    else"
    echo "        local local_board_name=\$1"
    echo "        shift;"
    echo "        local local_addons_parameters=\$@"
    echo "    fi"
    echo "    echo \"**** sign_fip ****START****\""
    echo "    if [ \${SIGN_ENABLE} -eq 0 ]; then"
    echo "        die \"[ERROR] You MUST set the flag SIGN_ENABLE to 1 to perform signature\""
    echo "    fi"
    echo "    if [ \${SIGN_ENABLE} -eq 1 -a \${ENCRYPT_ENABLE} -eq 1 ]; then"
    echo "        [ \$local_DRY_RUN -eq 1 ] && echo \"[ERROR] This function only perform signature, you need to call 'sign-encrypt-fip'\""
    echo "        die \"[ERROR] This function only perform signature, you need to call 'sign-encrypt-fip'\""
    echo "    fi"
    echo "    if [ \${SIGN_ENABLE} -eq 1 -a \${ENCRYPT_ENABLE} -eq 0 ]; then"
    echo "        if [ \$local_DRY_RUN -eq 1 ]; then"
    echo "          # For FIP"
    echo "          echo FIP_DEPLOYDIR_ROOT=\${your_deploy_dir_path} ENCRYPT_SUFFIX=\${encryption_suffix} SIGN_SUFFIX=\${signature_suffix} create_st_fip_binary.sh \\"
    echo "            --use-bl31 \\"
    echo "            --sign \\"
    echo "            --signature-key \${SIGN_KEY} \\"
    echo "            --signature-key-pass \${SIGN_KEY_PASS_FOR_SPECIFC_KEY} \\"
    echo "            --search-secondary-config default:\${your_storage_boot_scheme_security} \\"
    echo "            --search-configuration \${your_storage_boot_scheme_cortex_a} \\"
    echo "            --search-devicetree \${local_board_name} \\"
    echo "            --search-soc-name \${your_soc_name} \\"
    echo "            \${local_addons_parameters} \\"
    echo "            --output \${your_deploy_dir_path}/fip"
    echo "       else"
    echo "          # For FIP"
    echo "          FIP_DEPLOYDIR_ROOT=\${your_deploy_dir_path} ENCRYPT_SUFFIX=\${encryption_suffix} SIGN_SUFFIX=\${signature_suffix} create_st_fip_binary.sh \\"
    echo "            --use-bl31 \\"
    echo "            --sign \\"
    echo "            --signature-key \${SIGN_KEY} \\"
    echo "            --signature-key-pass \${SIGN_KEY_PASS_FOR_SPECIFC_KEY} \\"
    echo "            --search-secondary-config default:\${your_storage_boot_scheme_security} \\"
    echo "            --search-configuration \${your_storage_boot_scheme_cortex_a} \\"
    echo "            --search-devicetree \${local_board_name} \\"
    echo "            --search-soc-name \${your_soc_name} \\"
    echo "            \${local_addons_parameters} \\"
    echo "            --output \${your_deploy_dir_path}/fip"
    echo "       fi"
    echo "    fi"
    echo "    echo \"**** sign_fip ****END****\""
    echo "}"
    echo ""
    echo "function _sign_encrypt_fip {"
    echo "    if [ \$# -eq 0 ]; then"
    echo "        local local_board_name=\${your_board_name}"
    echo "        local local_addons_parameters="
    echo "    else"
    echo "        local local_board_name=\$1"
    echo "        shift;"
    echo "        local local_addons_parameters=\$@"
    echo "    fi"

    echo "    echo \"**** sign-encrypt-fip ****START****\""
    echo "    if [ \${SIGN_ENABLE} -eq 0 ]; then"
    echo "        die \"[ERROR] You MUST set the flag SIGN_ENABLE to 1 to perform signature and encryption\""
    echo "    fi"
    echo "    if [ \${SIGN_ENABLE} -eq 1 -a \${ENCRYPT_ENABLE} -eq 0 ]; then"
    echo "        [ \$local_DRY_RUN -eq 1 ] && echo \"[ERROR] This function only perform signature and encryption, you need to call 'sign-fip'\""
    echo "        die \"[ERROR] This function only perform signature and encryption, you need to call 'sign-fip'\""
    echo "    fi"
    echo "    if [ \${SIGN_ENABLE} -eq 1 -a \${ENCRYPT_ENABLE} -eq 1 ]; then"
    echo "        if [ \$local_DRY_RUN -eq 1 ]; then"
    echo "          # For FIP"
    echo "          echo FIP_DEPLOYDIR_ROOT=\${your_deploy_dir_path} ENCRYPT_SUFFIX=\${encryption_suffix} SIGN_SUFFIX=\${signature_suffix} create_st_fip_binary.sh \\"
    echo "            --use-bl31 \\"
    echo "            --sign \\"
    echo "            --signature-key \${SIGN_KEY} \\"
    echo "            --signature-key-pass \${SIGN_KEY_PASS_FOR_SPECIFC_KEY} \\"
    echo "            --encrypt \${ENCRYPT_FIP_KEY} \\"
    echo "            --search-secondary-config default:\${your_storage_boot_scheme_security} \\"
    echo "            --search-configuration \${your_storage_boot_scheme_cortex_a} \\"
    echo "            --search-devicetree \${local_board_name} \\"
    echo "            --search-soc-name \${your_soc_name} \\"
    echo "            \${local_addons_parameters} \\"
    echo "            --output \${your_deploy_dir_path}/fip"
    echo "       else"
    echo "          # For FIP"
    echo "          FIP_DEPLOYDIR_ROOT=\${your_deploy_dir_path} ENCRYPT_SUFFIX=\${encryption_suffix} SIGN_SUFFIX=\${signature_suffix} create_st_fip_binary.sh \\"
    echo "            --use-bl31 \\"
    echo "            --sign \\"
    echo "            --signature-key \${SIGN_KEY} \\"
    echo "            --signature-key-pass \${SIGN_KEY_PASS_FOR_SPECIFC_KEY} \\"
    echo "            --encrypt \${ENCRYPT_FIP_KEY} \\"
    echo "            --search-secondary-config default:\${your_storage_boot_scheme_security} \\"
    echo "            --search-configuration \${your_storage_boot_scheme_cortex_a} \\"
    echo "            --search-devicetree \${local_board_name} \\"
    echo "            --search-soc-name \${your_soc_name} \\"
    echo "            \${local_addons_parameters} \\"
    echo "            --output \${your_deploy_dir_path}/fip"
    echo "       fi"
    echo "    fi"
    echo "    echo \"**** sign-encrypt-fip ****END****\""
    echo "}"
}

function generate_signature_encryption_fip_a_stm32mp2-m33td_function_addons() {
    local mx=$1
    echo "function sign_fip {"
    echo "    # Take linux_dtb_name as name of devicetree to use for runtime"
    if [ $mx -eq 0 ]; then
        echo "    _sign_fip \${linux_dtb_name} ''"
    else
        echo "    local_storage=\$(echo \${your_storage_boot_scheme_cortex_a} | cut -d'-' -f2)"
        echo "    _sign_fip \${linux_dtb_name} --search-devicetree-suffix -\${local_storage}"
    fi
    echo "}"
    echo "function sign_encrypt_fip {"
    echo "    # Take linux_dtb_name as name of devicetree to use for runtime"
    if [ $mx -eq 0 ]; then
        echo "    _sign_encrypt_fip \${linux_dtb_name} ''"
    else
        echo "    local_storage=\$(echo \${your_storage_boot_scheme_cortex_a} | cut -d'-' -f2)"
        echo "    _sign_encrypt_fip \${linux_dtb_name} --search-devicetree-suffix -\${local_storage}"
    fi
    echo "}"

    echo "function sign_fip_programmer {"
    echo "    echo \"**** sign_fip programmer****START****\""
    echo "    # Take optee_dt_programmer_name as name of devicetree to use for programmer"
    echo "    _sign_fip \${tfa_dt_programmer_name} --use-ddr --search-storage usb --search-secondary-config programmer:\${your_storage_boot_scheme_security}-programmer-usb:\${your_storage_boot_scheme_security}-programmer"
    echo "    echo \"**** sign_fip programmer****END****\""
    echo "}"
    echo "function sign_encrypt_fip_programmer {"
    echo "    echo \"**** sign-encrypt-fip programmer****START****\""
    echo "    _sign_encrypt_fip \${tfa_dt_programmer_name} --use-ddr --search-storage usb --search-secondary-config programmer:\${your_storage_boot_scheme_security}-programmer-usb:\${your_storage_boot_scheme_security}-programmer"
    echo "    echo \"**** sign-encrypt-fip programmer****END****\""
    echo "}"
}

function generate_signature_encryption_bl2_m_function() {
    echo "function _sign_m33_bl2 {"
    echo "    if [ \$# -eq 1 ]; then"
    echo "        local local_board_name_addons=\"-\$1\""
    echo "    else"
    echo "        local local_board_name_addons=''"
    echo "    fi"

    echo "    echo \"**** sign_m33_bl2 ****START****\""
    echo "    if [ \${SIGN_ENABLE} -eq 0 ]; then"
    echo "        die \"[ERROR] You MUST set the flag SIGN_ENABLE to 1 to perform signature\""
    echo "    fi"
    echo "    if [ \${ENCRYPT_ENABLE} -eq 1 ]; then"
    echo "        [ \$local_DRY_RUN -eq 1 ] && echo \"[ERROR] ENCRYPTION is not supported on cortex M33\""
    echo "        die \"[ERROR] ENCRYPTION is not supported on cortex M33\""
    echo "    fi"
    echo "    if [ ! -e \${SIGN_TOOL} ]; then"
    echo "        die \"[ERROR]: You MUST specifiy a correct path for 'SIGN_TOOL'\""
    echo "    fi"
    echo "    if [ \${SIGN_ENABLE} -eq 1 -a \${ENCRYPT_ENABLE} -eq 0 ]; then"
    echo "        # verify if bl2 binary to sign is present"
    echo "        if [ ! -e \${your_deploy_dir_path}/arm-trusted-firmware-m/bl2/bl2-\${tfm_dt_name}\${local_board_name_addons}.stm32 ]; then"
    echo "            echo \"[ERROR] the file \${your_deploy_dir_path}/arm-trusted-firmware-m/bl2/bl2-\${tfm_dt_name}\${local_board_name_addons}.stm32 doesn't exist\""
    echo "            die \"[ERROR] You need to compile it via tf-a or compile-for-fip\""
    echo "        fi"
    echo "        if [ \$local_DRY_RUN -eq 1 ]; then"
    echo "          echo \${SIGN_TOOL} \\"
    echo "            -bin \${your_deploy_dir_path}/arm-trusted-firmware-m/nl2/bl2-\${tfm_dt_name}\${local_board_name_addons}.stm32  \\"
    echo "            -o \${your_deploy_dir_path}/arm-trusted-firmware-m/bl2/bl2-\${tfm_dt_name}\${local_board_name_addons}\${signature_suffix}.stm32 \\"
    echo "            --password \${SIGN_KEY_PASS_M} \\"
    echo "            --public-key \${SIGN_KEY_PUB_M} \\"
    echo "            --private-key \${SIGN_KEY_M} \\"
    echo "            --type cm33 --silent \\"
    echo "            \${SIGN_TOOL_EXTRA}"
    echo "       else"
    echo "          \${SIGN_TOOL} \\"
    echo "            -bin \${your_deploy_dir_path}/arm-trusted-firmware-m/bl2/bl2-\${tfm_dt_name}\${local_board_name_addons}.stm32  \\"
    echo "            -o \${your_deploy_dir_path}/arm-trusted-firmware-m/bl2/bl2-\${tfm_dt_name}\${local_board_name_addons}\${signature_suffix}.stm32 \\"
    echo "            --password \${SIGN_KEY_PASS_M} \\"
    echo "            --public-key \${SIGN_KEY_PUB_M} \\"
    echo "            --private-key \${SIGN_KEY_M} \\"
    echo "            --type cm33 --silent \\"
    echo "            \${SIGN_TOOL_EXTRA}"
    echo "       fi"
    echo "    fi"
    echo "    echo \"**** sign_m33_bl2 ****END****\""
    echo "}"
    echo ""
}

function generate_signature_encryption_ddr_fw_m_function() {
    echo "function _sign_m33_ddr_fw {"
    echo "    sign_version=\"\""
    echo "    if [ \$# -eq 1 ]; then"
    echo "        local local_type=\"\$1\""
    echo "        if [ "\${local_type}" = "--ddrfw" ]; then"
    echo "            [ -z "\${M33FW_SIGN_VERSION_DDR}" ] || sign_version=\"--sign-version \${M33FW_SIGN_VERSION_DDR}\""
    echo "            [ -z "\${SIGN_M33DDR_KEY}" ] || sign_key=\"--signature-key \${SIGN_M33DDR_KEY}\""
    echo "            [ -z "\${SIGN_M33DDR_KEY_PASS}" ] || sign_key_pass=\"--signature-key-pass \${SIGN_M33DDR_KEY_PASS}\""
    echo "            local_m33_storage=\$(echo \${your_storage_boot_scheme_cortex_m} | cut -d '_' -f2 | cut -d'-' -f1)"
    echo "        else"
    echo "            [ -z "\${M33FW_SIGN_VERSION}" ] || sign_version=\"--sign-version \${M33FW_SIGN_VERSION}\""
    echo "            [ -z "\${SIGN_M33FW_KEY}" ] || sign_key=\"--signature-key \${SIGN_M33FW_KEY}\""
    echo "            [ -z "\${SIGN_M33FW_KEY_PASS}" ] || sign_key_pass=\"--signature-key-pass \${SIGN_M33FW_KEY_PASS}\""
    echo "            local_m33_storage=\$(echo \${your_storage_boot_scheme_cortex_m} | cut -d '_' -f2-)"
    echo "        fi"
    echo "    else"
    echo "        die \"[ERROR] the type of firmware to sign must be specified [--ddrfw | --m33fw ]\""
    echo "    fi"
    echo "    echo \"**** _sign_m33_ddr_fw ****START****\""
    echo "    if [ \${SIGN_ENABLE} -eq 0 ]; then"
    echo "        die \"[ERROR] You MUST set the flag SIGN_ENABLE to 1 to perform signature\""
    echo "    fi"
    echo "    if [ \${ENCRYPT_ENABLE} -eq 1 ]; then"
    echo "        [ \$local_DRY_RUN -eq 1 ] && echo \"[ERROR] ENCRYPTION is not supported on cortex M33\""
    echo "        die \"[ERROR] ENCRYPTION is not supported on cortex M33\""
    echo "    fi"
    echo "    if [ ! -e \${SIGN_TOOL} ]; then"
    echo "        die \"[ERROR]: You MUST specifiy a correct path for 'SIGN_TOOL'\""
    echo "    fi"
    echo "    image_confirm=\"--image-confirm\""
    echo "    [ \"\${M33FW_IMAGE_CONFIRM}\" -eq 1 ] || image_confirm=\"\""
    echo ""
    echo "    if [ \${SIGN_ENABLE} -eq 1 -a \${ENCRYPT_ENABLE} -eq 0 ]; then"
    echo "        if [ \$local_DRY_RUN -eq 1 ]; then"
    echo "          echo M33FW_DEPLOYDIR_ROOT=\${your_deploy_dir_path} \${OECORE_NATIVE_SYSROOT}/usr/bin/create_st_m33fw_binary.sh \\"
    echo "            \${local_type} \\"
    echo "            --devicetree \${tfm_dt_name} \\"
    echo "            --bootsuffix-m33 -\${local_m33_storage} \\"
    echo "            \${sign_key} \\"
    echo "            \${sign_key_pass} \\"
    echo "            \${sign_version} \\"
    echo "            \${image_confirm} \\"
    echo "            --output \${your_deploy_dir_path}/m33-firmware/"
    echo "        else"
    echo "          M33FW_DEPLOYDIR_ROOT=\${your_deploy_dir_path} \${OECORE_NATIVE_SYSROOT}/usr/bin/create_st_m33fw_binary.sh \\"
    echo "            \${local_type} \\"
    echo "            --devicetree \${tfm_dt_name} \\"
    echo "            --bootsuffix-m33 -\${local_m33_storage} \\"
    echo "            \${sign_key} \\"
    echo "            \${sign_key_pass} \\"
    echo "            \${sign_version} \\"
    echo "            \${image_confirm} \\"
    echo "            --output \${your_deploy_dir_path}/m33-firmware/"
    echo "        fi"
    echo "    fi"
    echo "    echo \"**** _sign_m33_ddr_fw ****END****\""
    echo "}"
    echo ""
}


function generate_signature_encryption_m_function_addons() {
    local mx=$1
    echo "function sign_m33_bl2 {"
    if [ $mx -eq 0 ]; then
        echo "    local_m33_storage=\$(echo \${your_storage_boot_scheme_cortex_m} | cut -d '_' -f2 | cut -d'-' -f1)"
        echo "    _sign_m33_bl2 \${local_m33_storage}"
    else
        echo "    _sign_m33_bl2"
    fi
    echo "}"

    echo "function sign_m33_ddr {"
        echo "    _sign_m33_ddr_fw --ddrfw"
    echo "}"

    echo "function sign_m33_fw {"
    echo "    _sign_m33_ddr_fw --m33fw"
    echo "}"
}


function generate_signature_encryption_function() {
    local machine=$1
    local mx=$2
    local security=$3
    [ $security -eq 1 ] || return
    # signature/encryption of bl2 on Cortex A
    generate_signature_encryption_bl2_a_function
    # signature/encryption of FIP on Cortex A
    case $machine in
    stm32mp1*)
        generate_signature_encryption_bl2_a_function_addons
        generate_signature_encryption_fip_a_stm32mp1_function
        generate_signature_encryption_fip_a_stm32mp12_function_addons
        ;;
    stm32mp2)
        generate_signature_encryption_bl2_a_function_addons
        generate_signature_encryption_fip_a_stm32mp2_function
        generate_signature_encryption_fip_a_stm32mp12_function_addons
        ;;
    stm32mp2-mx)
        generate_signature_encryption_bl2_a_function_addons
        generate_signature_encryption_fip_a_stm32mp2_function
        generate_signature_encryption_fip_a_stm32mp2_function_addons
        ;;
    stm32mp2-m33td*)
        generate_signature_encryption_bl2_a_stm32mp2-m33td_function_addons
        generate_signature_encryption_fip_a_stm32mp2-m33td_function
        generate_signature_encryption_fip_a_stm32mp2-m33td_function_addons $mx
        ;;
    esac
    # signature/encryption of bl2, ddr and fw on Cortex M
    case $machine in
    stm32mp2-m33td*)
        generate_signature_encryption_bl2_m_function
        generate_signature_encryption_ddr_fw_m_function
        generate_signature_encryption_m_function_addons $mx
        ;;
    esac

    echo "# --------------------------------------------------"
    echo "# Signature / Encryption"
    echo "# --------------------------------------------------"
    case $machine in
    stm32mp1*)
        signature_encryption_stm32mp1_default_configuration
        ;;
    stm32mp2)
        signature_encryption_stm32mp2_default_configuration
        ;;
    stm32mp2-mx)
        signature_encryption_stm32mp2_default_configuration
        ;;
    stm32mp2-m33td*)
        signature_encryption_stm32mp2-m33td_default_configuration
        ;;
    *)
        ;;
    esac
    echo ""
    echo "if [ \${SIGN_ENABLE} -eq 0 -a \${SIGN_ENABLE} -eq 1 ]; then"
    echo "    echo \"ONLY SIGNATURE/ENCRYPTION IS SUPPORTED\""
    echo "    echo \"FOR USING ENCRYPTION YOU NEED TO ACTIVATE SIGNATURE\""
    echo "    die 'signature/encryption configuration issue'"
    echo "fi"
    echo "if [ \${SIGN_COPRO_ENABLE} -eq 0 -a \${ENCRYPT_COPRO_ENABLE} -eq 1 ]; then"
    echo "    echo \"ONLY SIGNATURE/ENCRYPTION IS SUPPORTED FOR COPROCESSOR\""
    echo "    echo \"FOR USING ENCRYPTION YOU NEED TO ACTIVATE SIGNATURE\""
    echo "    die 'signature/encryption configuration issue on coprocessor'"
    echo "fi"
    echo ""
    echo "# --------------------------------------------------"
    echo "# --------------------------------------------------"
}
# ------------------------------------------
function usage() {
    echo "generated_build_script <machine name> [--security]"
    echo "  <machine name>: name of the machine which can be use for sdk compilation"
    echo "      stm32mp1 or stm32mp1-mx"
    echo "      stm32mp2 or stm32mp2-mx"
    echo "      stm32mp2-m33td or stm32mp2-m33td-mx"
    echo "  --security: generate configuration for signature/encryption"
}
# ------------------------------------------
# Main
# ------------------------------------------
# parse argument
if [ $# -eq 0 ]; then
    usage
    exit 1
fi
GLOBAL_CONFIGURATION=""
MX_NAME=""
SECURITY_NAME=""
OVERWRITE_GENERATED_FILE=0
while [[ $# -gt 0 ]]; do
  case $1 in
    help)
      usage
      exit 0
      ;;
    stm32mp1-mx)
      MACHINE=stm32mp1
      MX=1
      MX_NAME="-mx"
      GLOBAL_CONFIGURATION=$1
      shift
      ;;
    stm32mp1)
      MACHINE=stm32mp1
      MX=0
      GLOBAL_CONFIGURATION=$1
      shift;
      ;;
    stm32mp2-mx)
      MACHINE=stm32mp2
      MX=1
      MX_NAME="-mx"
      GLOBAL_CONFIGURATION=$1
      shift
      ;;
    stm32mp2)
      MACHINE=stm32mp2
      MX=0
      GLOBAL_CONFIGURATION=$1
      shift
      ;;
    stm32mp2-m33td-mx)
      MACHINE=stm32mp2-m33td
      MX=1
      MX_NAME="-mx"
      GLOBAL_CONFIGURATION=$1
      shift
      ;;
    stm32mp2-m33td)
      MACHINE=stm32mp2-m33td
      MX=0
      GLOBAL_CONFIGURATION=$1
      shift
      ;;
   --secu*)
       SECURITY=1
       SECURITY_NAME="-sign_encrypt"
       shift
       ;;
   -sec*)
       SECURITY=1
       SECURITY_NAME="-sign_encrypt"
       shift
       ;;
    --overwrite)
      OVERWRITE_GENERATED_FILE=1
      shift
      ;;
    *)
      echo "[ERROR] parameters not supported: $@"
      usage
      exit 1
      ;;
  esac
done

if [ "X$GLOBAL_CONFIGURATION" = "X" ]; then
    usage
    exit 1
fi
if [ $MX -eq 1 ]; then
    info "MACHINE=$MACHINE MX"
else
    info "MACHINE=$MACHINE"
fi
if [ $SECURITY -eq 1 ]; then
    info "with security configuration generation activated (Signature / Encryption)"
fi

# generate list of components
COMPONENTS=$(cat ${SCRIPT_PATH}/README.HOW_TO.txt.${MACHINE} | grep ^@ | sed "s/^@[A-Z]* //g")
debug "COMPONENT LIST=>$COMPONENTS<"
# generate list of components used for fip generation
COMPONENTS_FIP=$(cat ${SCRIPT_PATH}/README.HOW_TO.txt.${MACHINE} | grep ^@F | sed "s/^@[A-Z]* //g")
# generate list of components used for fip generation
COMPONENTS_M33FW=$(cat ${SCRIPT_PATH}/README.HOW_TO.txt.${MACHINE} | grep ^@M | sed "s/^@[A-Z]* //g")

# create file name for output script file
OUTPUT_SCRIPT=${SCRIPT_PATH}/../sdk_compilation-${MACHINE}${MX_NAME}${SECURITY_NAME}-my-custom-board.sh

COMMON_SCRIPT_NAME=sdk_action-common-for-${GLOBAL_CONFIGURATION}.source
if [ $SECURITY -eq 1 ]; then
    COMMON_SCRIPT_NAME=sdk_action-common-for-${GLOBAL_CONFIGURATION}-with-security.source
fi

OUTPUT_COMMON_SCRIPT=${SCRIPT_PATH}/../${COMMON_SCRIPT_NAME}

# Verify if a precedent version of script generated exist
if [ ${OVERWRITE_GENERATED_FILE} -eq 0 -a -f "${OUTPUT_COMMON_SCRIPT}" ];
then
    echo ""
    echo "[WARNING]: A previous version of generated script exist on directory"
    echo "[WARNING]:    '${OUTPUT_COMMON_SCRIPT}'"
    echo "[WARNING]: would you like to continue and overwrite it: [Y/n]"
    read -r answer
    if [[ "$answer" =~ ^[Yy]+[ESes]* ]]; then
        echo "[INFO] Overwrite previous version of ${OUTPUT_COMMON_SCRIPT}"
        echo ""
    elif [ "${answer}" = "" ]; then
        echo "[INFO] Overwrite previous version of ${OUTPUT_COMMON_SCRIPT}"
        echo ""
    else
        echo "[INFO] Please save or rename generated script before to relaunch script"
        exit 0
    fi
fi

debug "Machine $MACHINE"
debug "is MX machine = $MX"
debug "is security activated = $SECURITY (Signature / Encryption)"

cat << EOF > $OUTPUT_SCRIPT
#!/bin/bash

$(config_stm32mp $MACHINE $MX $SECURITY)

source $COMMON_SCRIPT_NAME
EOF

cat << EOF > $OUTPUT_COMMON_SCRIPT
#!/bin/bash

# do not execute the command, only display command if equal to 1
# > DRY_RUN=1 <script_name> <action>
local_DRY_RUN=\${DRY_RUN:=0}

# ----------------------------------------
die() {
    if [ \$local_DRY_RUN -eq 0 ]; then
        >&2 echo "FAILED > BUILD ISSUE on \$*"
        exit 1
    else
        >&2 echo ""
    fi
}
cmd(){
    echo "[CMD]:> \$@"
}
# -----------------------------------------
# addons configuration management
if [ -n "\${FIRMWARE_UPDATE_AB}" ]; then
    if [ \${FIRMWARE_UPDATE_AB} -eq 1 ]; then
        export TF_A_FIRMWARE_UPDATE_AB=1
    fi
fi
# -----------------------------------------
$(generate_component_function)

$(generate_action_set)

$(generate_signature_encryption_function $MACHINE $MX $SECURITY)

# -----------------------------------------
#              MAIN
# -----------------------------------------
if [ \$# -ne 1 ];
then
    if [ "\$1" = "--dry-run" ]; then
        local_DRY_RUN=1
        shift
        if [ \$# -ne 1 ];
        then
            action=help
        else
            action=\$1
        fi
    else
        action=\$1
    fi
else
    action=\$1
fi

$(dump_stm32mp_config $MACHINE $MX)

# source sdk environment
source \${sdk_absolute_path}/\${sdk_env_file}

case \$action in
all)
$(generate_action_all)
    ;;
extract)
$(generate_action_extract)
    ;;
compile)
$(generate_action_compile)
    ;;
compile-for-fip)
$(generate_action_compile_for_deploy)
    ;;
compile-for-m33fw)
$(generate_action_compile_m33fw_for_deploy)
    ;;
deploy-for-fip)
$(generate_action_deploy_for_fip)
    ;;
deploy-for-m33fw)
$(generate_action_deploy_for_m33fw)
    ;;
deploy)
$(generate_action_deploy)
    ;;
programmer)
$(generate_action_programmer)
    ;;
clean)
$(generate_action_clean)
    ;;
cleanall)
$(generate_action_cleanall)
    ;;
$(generate_action_component)
$(generate_sign_encrypt_action)
*)
    echo "Help:"
    echo "\$0 [extract|compile|compile-for-fip|deploy|programmer|<component>-<action>]"
    echo "action:"
    echo "    extract: extract the source for all components"
    echo "    compile-for-fip: compile all the component needed to generate fip"
    echo "    deploy-for-fip:  deploy all the component needed to generate fip and generate fip"
    echo "    compile-for-m33fw: compile all the component needed to generate m33fw"
    echo "    deploy-for-m33fw:  deploy all the component needed to generate m33fw and generate m33fw"
    echo "    compile:     build all the components for runtime"
    echo "    deploy:     generate the deploy with input of each component (need to have made compile of component before)"
    echo "    programmer: binaries for programmer usage"
    echo "    clean:      clean all components (remove all build directory, preserve source code)"
    echo "    cleanall:   clean all components (remove all build directory and extracted source code)"

    echo ""
    echo "    <component>: make all step for a specific component (if is needed)"
    echo "         -extract"
    echo "         -set"
    echo "         -configure"
    echo "         -compile"
    echo "         -deploy"
    echo "         -programmer-compile"
    echo "         -programmer-deploy"
    echo "         -clean"
    echo "         -cleanall"
    echo "    for linux-stm32mp there is to more possible action (already included on compile)"
    echo "         -dtb"
    echo "         -dtbs"
    echo "         -images"
    echo "         -modules"
    echo "         -menuconfig"
    echo "    for u-boot-stm32mp there is to more possible action (already included on compile)"
    echo "         -menuconfig"
    echo "component:"
$(generate_component_list)
EOF
    if $(echo $MACHINE  | grep -q stm32mp2-m33td) ; then
cat << EOF >> $OUTPUT_COMMON_SCRIPT
    echo "   For stm32mp2-m33td machine, the M33 part"
    echo "    tf-m-stm32mp"
    echo "    m33tdprojects-starter-stm32mp2"
EOF
    fi

cat << EOF >> $OUTPUT_COMMON_SCRIPT
    echo ""
    echo "Recommended step:"
    echo "    \$0  extract"
    echo "    \$0  compile-for-fip"
    echo "    \$0  deploy-for-fip"
    echo "   for flashing:"
    echo "    \$0  programmer"
    echo "   kernel"
    echo "    \$0  linux-stm32mp"
    echo "   if your have a gpu on soc"
    echo "    \$0  gcnano-driver-stm32mp"
EOF

    if $(echo $MACHINE  | grep -q stm32mp2-m33td) ; then
cat << EOF >> $OUTPUT_COMMON_SCRIPT
    echo "   For stm32mp2-m33td machine, the M33 part:"
    echo "    \$0  compile-for-m33fw"
    echo "    \$0  deploy-for-m33fw"
EOF
    fi

    if [ $SECURITY -eq 1 ]; then
cat << EOF >> $OUTPUT_COMMON_SCRIPT
    echo "Addons step:"
    echo "   For Signature:"
    echo "    \$0  sign-bl2"
    echo "    \$0  sign-fip"
    echo "    \$0  sign-bl2-programmer"
    echo "    \$0  sign-fip-programmer"

    echo "   For Signature and encryption:"
    echo "    \$0  sign-encrypt-bl2"
    echo "    \$0  sign-encrypt-fip"
    echo "    \$0  sign-encrypt-bl2-programmer"
    echo "    \$0  sign-encrypt-fip-programmer"

EOF

    if $(echo $MACHINE  | grep -q stm32mp2-m33td) ; then
cat << EOF >> $OUTPUT_COMMON_SCRIPT
    echo "   For Signature on M33_TD:"
    echo "    \$0  sign-m33-bl2"
    echo "    \$0  sign-m33-ddr"
    echo "    \$0  sign-m33-fw"
EOF
        fi
    fi

cat << EOF >> $OUTPUT_COMMON_SCRIPT
    ;;
esac
EOF

info ""
info "******************************************"
info "This script generates two files:"
info "* one with all the command for compiling the components: $OUTPUT_COMMON_SCRIPT"
info "* one for your custom configuration: $OUTPUT_SCRIPT"
info ""
info "You MUST fill the information on the custom file to match your needs "
info "and it is preferable to rename the script according with your project name"
info " (and not to be overwrited by a new launch of this script $0"
info "******************************************"

chmod +x $OUTPUT_SCRIPT
