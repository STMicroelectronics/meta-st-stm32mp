#!/bin/bash - 
#===============================================================================
#  ORGANIZATION: STMicroelectronics
#     COPYRIGHT: Copyright (C) 2025, STMicroelectronics - All Rights Reserved
#       CREATED: 03/19/2025 16:57
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

########################################################
# local variable for generation script
MACHINE=
MX=0
MX_NAME=""
OUTPUT_SCRIPT=
COMPONENTS=
COMPONENTS_FIP=
#######################################################

########################################################
#           STM32MP1
function config_stm32mp1_common() {
    echo "##################################################################"
    echo "##################################################################"
    echo "### General configuration"
    echo "##################################################################"
    echo "##################################################################"

    echo "# Configuration for building stm32mp1x"
    echo "your_board_name=\"<Your board name>\""
    echo ""
    echo "# your_soc_name can be stm32mp15 or stm32mp13"
    echo "your_soc_name=\"<Your soc name>\""
    echo ""
    echo "# Type of security can be 'optee' or 'opteemin' (opteemin is preconize for stm32mp15)"
    echo "your_storage_boot_scheme_security=\"<your_storage_boot_scheme_security>\""
    echo ""
    echo "# Type of boot scheme storage used for A35 part, ex.:"
    echo "#    optee-emmc"
    echo "#    optee-sdcard"
    echo "#    optee-nor"
    echo "#    opteemin-emmc"
    echo "#    opteemin-sdcard"
    echo "#    opteemin-nor"
    echo "#    ... (for other boot storage see ST Yocto BSP)"
    echo "your_storage_boot_scheme_cortex_a=\"<your_storage_boot_scheme_cortex_a>\""
    echo ""
    echo "# Location to store all the binaries generated"
    echo "your_deploy_dir_path=\"<your_deploy_dir_path>\""
    echo ""
    echo "# Define sub build directory (by default set to ../build)"
    echo "your_build_subdir_path=\"<your_build_subdir_path>\""
    echo ""

    echo "PARALLEL_MAKE="
    echo "# paralelle make for linux kernel:"
    echo "# ex.: "
    echo "# PARALLEL_MAKE=-j8"
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
    echo "# force external dt path to externaldt env path"
    echo "externaldt_path=\"<external dt path>\""
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
    echo "optee_dt_name=\$your_board_name"
    echo "optee_dt_programmer_name=\$your_board_name"
    echo "# U-boot dt name"
    echo "uboot_defconfig=\${your_soc_name}_defconfig"
    echo "uboot_dt_name=\$your_board_name"
    echo "uboot_programmer_dt_name=your_board_name"
    echo "# TF-A dt name"
    echo "tfa_dt_name=\$your_board_name"
    echo "tfa_dt_programmer_name=\$your_board_name"
    echo "# Linux kernel dt name"
    echo "# linux"
    echo "linux_dtb_name=\$your_board_name"
}
function config_stm32mp1_mx() {
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
    echo "#  <CUBMEMX project path>/CA7/DeviceTree/<your_cubemx_project_name/"
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
    echo "optee_dt_name=\$your_board_name"
    echo "optee_dt_programmer_name=\$your_board_name"
    echo "# U-boot dt name"
    echo "uboot_defconfig=\${your_soc_name}_defconfig"
    echo "uboot_dt_name=\$your_board_name"
    echo "uboot_programmer_dt_name=your_board_name"
    echo "# TF-A dt name"
    echo "tfa_dt_name=\$your_board_name"
    echo "tfa_dt_programmer_name=\$your_board_name"
    echo "# Linux kernel dt name"
    echo "# linux"
    echo "linux_dtb_name=\$your_board_name"
}
function config_stm32mp1() {
    local mx=$1
    config_stm32mp1_common
    if [ $mx -eq 1 ]; then
        config_stm32mp1_mx
    else
        config_stm32mp1_externaldt
    fi
    echo "##################################################################"
    echo "##################################################################"
    echo "##################################################################"
    echo "##################################################################"
}
function dump_stm32mp1_config() {
    local mx=$1
    echo "echo \"Your configuration:\""
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
########################################################
#           STM32MP2
function config_stm32mp2_common() {
    echo "##################################################################"
    echo "##################################################################"
    echo "### General configuration"
    echo "##################################################################"
    echo "##################################################################"


    echo "# Configuration for building stm32mp2x"
    echo "your_board_name=\"<Your board name>\""
    echo ""
    echo "# your_soc_name can be stm32mp21, stm32mp23, stm32mp25"
    echo "your_soc_name=\"<Your soc name>\""
    echo ""
    echo "# Type of securityType of security can be 'optee' or 'opteemin' (optee is preconized)"
    echo "your_storage_boot_scheme_security=\"<your_storage_boot_scheme_security>\""
    echo ""
    echo "# Type of boot scheme storage used for A35 part, ex.:"
    echo "#    optee-emmc"
    echo "#    optee-sdcard"
    echo "#    optee-nor"
    echo "#    ... (for other boot storage see ST Yocto BSP)"
    echo "your_storage_boot_scheme_cortex_a=\"<your_storage_boot_scheme_cortex_a>\""
    echo ""
    echo "# Location to store all the binaries generated"
    echo "your_deploy_dir_path=\"<your_deploy_dir_path>\""
    echo ""
    echo "# Define sub build directory  (by default set to ../build)"
    echo "your_build_subdir_path=\"<your_build_subdir_path>\""
    echo ""
    echo "PARALLEL_MAKE="
    echo "# paralelle make for linux kernel:"
    echo "# ex.: "
    echo "# PARALLEL_MAKE=-j8"
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
    echo "# force external dt path to externaldt env path"
    echo "externaldt_path=\"<external dt path>\""
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
    echo "optee_dt_name=\$your_board_name"
    echo "optee_dt_programmer_name=\$your_board_name"
    echo "# U-boot dt name"
    echo "uboot_defconfig=\${your_soc_name}_defconfig"
    echo "uboot_dt_name=\$your_board_name"
    echo "uboot_programmer_dt_name=your_board_name"
    echo "# TF-A dt name"
    echo "tfa_dt_name=\$your_board_name"
    echo "tfa_dt_programmer_name=\$your_board_name"
    echo "# Linux kernel dt name"
    echo "# linux"
    echo "linux_dtb_name=\$your_board_name"
}
function config_stm32mp2_mx() {
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
    echo "optee_dt_name=\$your_board_name"
    echo "optee_dt_programmer_name=\$your_board_name"
    echo "# U-boot dt name"
    echo "uboot_defconfig=\${your_soc_name}_defconfig"
    echo "uboot_dt_name=\$your_board_name"
    echo "uboot_programmer_dt_name=your_board_name"
    echo "# TF-A dt name"
    echo "tfa_dt_name=\$your_board_name"
    echo "tfa_dt_programmer_name=\$your_board_name"
    echo "# Linux kernel dt name"
    echo "# linux"
    echo "linux_dtb_name=\$your_board_name"
}
function config_stm32mp2() {
    local mx=$1
    config_stm32mp2_common
    if [ $mx -eq 1 ]; then
        config_stm32mp2_mx
    else
        config_stm32mp2_externaldt
    fi
    echo "##################################################################"
    echo "##################################################################"
    echo "##################################################################"
    echo "##################################################################"
}
function dump_stm32mp2_config() {
    local mx=$1
    echo "echo \"Your configuration:\""
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

    echo "# Configuration for building stm32mp2x-M33TD"
    echo "your_board_name=\"<Your board name>\""
    echo ""
    echo "# your_soc_name can be stm32mp21, stm32mp25"
    echo "your_soc_name=\"<Your soc name>\""
    echo ""
    echo "# Type of securityType of security can be 'optee' or 'opteemin' (optee is preconized)"
    echo "your_storage_boot_scheme_security=\"<your_storage_boot_scheme_security>\""
    echo ""
    echo "# Type of boot scheme storage used for A35 part, ex.:"
    echo "#    optee-emmc"
    echo "#    optee-sdcard"
    echo "#    optee-nor"
    echo "#    ... (for other boot storage see ST Yocto BSP)"
    echo "your_storage_boot_scheme_cortex_a=\"<your_storage_boot_scheme_cortex_a>\""
    echo ""
    echo "# Type of boot scheme storage used for M33 part, ex.:"
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
    echo "your_m33_profile=\"<your_m33_profile>\""
    echo ""
    echo "# Name of project used on M33 processor, ex.:"
    echo "#       Projects/STM32MP215F-DK/Demonstrations/StarterApp_M33TD"
    echo "#       Projects/STM32MP257F-EV1/Demonstrations/StarterApp_M33TD"
    echo "your_cube_m33td_project=\"<your_cube_m33td_project>\""
    echo ""
    echo "# Location to store all the binaries generated"
    echo "your_deploy_dir_path=\"<your_deploy_dir_path>\""
    echo ""
    echo "# Define sub build directory (by default set to ../build)"
    echo "your_build_subdir_path=\"<your_build_subdir_path>\""
    echo ""
    echo "PARALLEL_MAKE="
    echo "# paralelle make for linux kernel:"
    echo "# ex.: "
    echo "# PARALLEL_MAKE=-j8"
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
    echo "tfa_dt_name=\${your_board_name}-cm33tdcid-ostl"
    echo "tfa_dt_programmer_name=\${your_board_name}-cm33tdcid-ostl-serial-ca35tdcid"
    echo "# TF-M dt name"
    echo "tfm_dt_name=\${your_board_name}-cm33tdcid-ostl"
}
function config_stm32mp2-m33td_mx() {
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
    echo "externaldt_tfm_path=CM33/DeviceTree/\${your_cubemx_project_name}/tfm"
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
    echo "# TF-M dt name"
    echo "tfm_dt_name=\${your_board_name}"
}
function config_stm32mp2-m33td() {
    local mx=$1
    config_stm32mp2-m33td_common
    if [ $mx -eq 1 ]; then
        config_stm32mp2-m33td_mx
    else
        config_stm32mp2-m33td_externaldt
    fi
    echo "##################################################################"
    echo "##################################################################"
    echo "##################################################################"
    echo "##################################################################"
}
function dump_stm32mp2-m33td_config() {
    local mx=$1
    echo "echo \"Your configuration:\""
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

    echo "echo \"\""
}

########################################################
function config_stm32mp() {
    local machine=$1
    local mx=$2
    case $machine in
    stm32mp1)
        config_stm32mp1 $mx
        ;;
    stm32mp2)
        config_stm32mp2 $mx
        ;;
    stm32mp2-m33td)
        config_stm32mp2-m33td $mx
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
        dump_stm32mp1_config $mx
        echo "[ \$DRY_RUN -eq 1 ] &&  echo \"     ***DRY RUN***\""
        echo "[ \$DRY_RUN -eq 1 ] &&  echo \"\""
        ;;
    stm32mp2)
        dump_stm32mp2_config $mx
        echo "[ \$DRY_RUN -eq 1 ] &&  echo \"     ***DRY RUN***\""
        echo "[ \$DRY_RUN -eq 1 ] &&  echo \"\""
        ;;
    stm32mp2-m33td)
        dump_stm32mp2-m33td_config $mx
        echo "[ \$DRY_RUN -eq 1 ] &&  echo \"     ***DRY RUN***\""
        echo "[ \$DRY_RUN -eq 1 ] &&  echo \"\""
        ;;
    *)
        ;;
    esac
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
    # your_m33_profile
    if $(echo $localdata | grep -q your_deploy_dir_path) ; then
        localdata=$(echo $localdata |sed "s|<your_deploy_dir_path>|\${your_deploy_dir_path}|g")
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
    # m33tdproject
    if $(echo $localdata | grep -q your_cube_m33td_project) ; then
        localdata=$(echo $localdata |sed "s|<your_cube_m33td_project>|\${your_cube_m33td_project}|g")
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
function generate_action_deploy_for_fip() {
    debug "** generate_action_deploy_for_fip"
    echo "    action_set"
    for c in ${COMPONENTS_FIP};
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
        echo "    ${c}_configure"
        echo "    ;;"
        echo "${c}-compile)"
        echo "    action_set"
        echo "    ${c}_compile"
        echo "    ;;"
        echo "${c}-deploy)"
        echo "    action_set"
        echo "    ${c}_deploy"
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
            echo "${c}-modules)"
            echo "    action_set"
            echo "    ${c}_modules"
            echo "    ;;"

        fi
    done
}
function generate_component_list() {
    debug "** generate_action_compenent_list"
    for c in ${COMPONENTS};
    do
        echo "    echo \"    ${c}\""
    done
}

# Manage:
# @E> : extract
# @S> : set
# @C> : compile
# @c> : compile
# @F> : generate fip
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
        programmer_compile_nb=$(grep "\$@PC>" ${readme} | wc -l)
        programmer_fip_nb=$(grep "\$@PF>" ${readme} | wc -l)
        configure_nb=$(grep "\$@c>" ${readme} | wc -l)
        debug "  extract_nb=$extract_nb"
        debug "  set_nb=$set_nb"
        debug "  compile_nb=$compile_nb"
        debug "  fip_nb=$fip_nb"
        debug "  programmer_compile_nb=$programmer_compile_nb"
        debug "  programmer_fip_nb=$programmer_fip_nb"
        debug "  configure_nb=$configure_nb"
        echo "# --------------------------------------------------"
        echo "# ${c}"
        echo "# --------------------------------------------------"
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
            echo "        echo \"source code for ${c} not extracted\""
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
                    echo "[ \$DRY_RUN -eq 0 ] && ${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            for d in ${data};
            do
                if $(echo ${c} | grep -q "external-dt") ; then
                    # set is not use and overwrited by externaldt_path
                    continue
                fi
                if $(echo ${d} | grep -q '@S') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@S>||"
                    echo "[ \$DRY_RUN -eq 0 ] && ${local_tmp_data}" | sed "s|[[:space:]]*\$@S>|   |"
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
                    echo "[ \$DRY_RUN -eq 0 ] && ${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            echo "    cmd    export BLD_PATH=\${your_build_subdir_path}"
            echo "    [ \$DRY_RUN -eq 0 ] && export BLD_PATH=\${your_build_subdir_path}"
            if $(echo ${c} | grep -q "linux-stm32mp") ; then
                echo "    if [ ! -e ../source_code_configured-${c}-for-\${your_board_name} ]; then"
            fi
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@c') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "[ \$DRY_RUN -eq 0 ] && ${local_tmp_data} || die ${c}" | sed "s|[[:space:]]*\$@c>|       |"
                fi
            done
            if $(echo ${c} | grep -q "linux-stm32mp") ; then
                echo "        [ \$DRY_RUN -eq 0 ] && touch ../source_code_configured-${c}-for-\${your_board_name}"
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
                    echo "[ \$DRY_RUN -eq 0 ] && ${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            echo "    cmd    export BLD_PATH=\${your_build_subdir_path}"
            echo "    [ \$DRY_RUN -eq 0 ] && export BLD_PATH=\${your_build_subdir_path}"
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@C') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@C>||"
                    echo "[ \$DRY_RUN -eq 0 ] && ${local_tmp_data} || die ${c}" | sed "s|[[:space:]]*\$@C>|   |"
                fi
            done
            IFS=$old_IFS
            echo "    cd \$localpath"
        fi
        echo "    echo \"**** ${c}_compile ****END****\""
        echo "}"

        echo "function ${c}_deploy {"
        echo "    echo \"**** ${c}_deploy ****START****\""
        if [ $fip_nb -gt 0 ]; then
            echo "    localpath=\$PWD"
            old_IFS=$IFS
            IFS=$'\n'
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@P>') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@P>||"
                    echo "[ \$DRY_RUN -eq 0 ] && ${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            echo "    cmd    export BLD_PATH=\${your_build_subdir_path}"
            echo "    [ \$DRY_RUN -eq 0 ] && export BLD_PATH=\${your_build_subdir_path}"
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@F') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@F>||"
                    echo "[ \$DRY_RUN -eq 0 ] && ${local_tmp_data} || die ${c}" | sed "s|[[:space:]]*\$@F>|   |"
                fi
            done
            IFS=$old_IFS
            echo "    cd \$localpath"
        fi
        echo "    echo \"**** ${c}_deploy ****END****\""
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
                    echo "[ \$DRY_RUN -eq 0 ] && ${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            echo "    cmd    export BLD_PATH=\${your_build_subdir_path}-programmer"
            echo "    [ \$DRY_RUN -eq 0 ] && export BLD_PATH=\${your_build_subdir_path}-programmer"
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@PC') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@PC>||"
                    echo "[ \$DRY_RUN -eq 0 ] && ${local_tmp_data} || die ${c}" | sed "s|[[:space:]]*\$@PC>|   |"
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
                    echo "[ \$DRY_RUN -eq 0 ] && ${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            echo "    cmd    export BLD_PATH=\${your_build_subdir_path}-programmer"
            echo "    [ \$DRY_RUN -eq 0 ] && export BLD_PATH=\${your_build_subdir_path}-programmer"
            for d in ${data};
            do
                if $(echo ${d} | grep -q '@PF') ; then
                    local_tmp_data=$(process_data ${d})
                    echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@PF>||"
                    echo "[ \$DRY_RUN -eq 0 ] && ${local_tmp_data} || die ${c}" | sed "s|[[:space:]]*\$@PF>|   |"
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
                    echo "[ \$DRY_RUN -eq 0 ] && ${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|       |"
                fi
            done
            IFS=$old_IFS
            if $(echo ${c} | grep -q "linux-stm32mp") ; then
                echo "        dir=\$(find .. -maxdepth 1 -type d | grep linux | tail -n 1)"
            else
                echo "        dir=\$(find .. -maxdepth 1 -type d | grep ${c} | tail -n 1)"
            fi
            echo "        # remove configured file step"
            echo "        [ -e ../source_code_configured-${c}-for-\${your_board_name} ] && cmd rm ../source_code_configured-${c}-for-\${your_board_name}"
            echo "        [ \$DRY_RUN -eq 0 ] && [ -e ../source_code_configured-${c}-for-\${your_board_name} ] && rm ../source_code_configured-${c}-for-\${your_board_name}"

            echo "        # remove build directory"
            echo "        [ -d \${your_build_subdir_path} ] && cmd rm -rf \${your_build_subdir_path}"
            echo "        [ \$DRY_RUN -eq 0 ] && [ -d \${your_build_subdir_path} ] && rm -rf \${your_build_subdir_path}"
            echo "        [ -d \${your_build_subdir_path}-programmer ] && cmd rm -rf \${your_build_subdir_path}-programmer"
            echo "        [ \$DRY_RUN -eq 0 ] && [ -d \${your_build_subdir_path}-programmer ] && rm -rf \${your_build_subdir_path}-programmer"
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
                    echo "[ \$DRY_RUN -eq 0 ] && ${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|       |"
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
            echo "        [ \$DRY_RUN -eq 0 ] && [ -d \$dir ] && rm -rf \$dir ../source_code_extracted-${c}"
            echo "    fi"
            echo "    cd \$localpath"
        fi
        echo "    echo \"**** ${c}_clean_src_extracted ****END****\""
        echo "}"

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
                    echo "[ \$DRY_RUN -eq 0 ] && ${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            IFS=$old_IFS
            echo "    cmd    export BLD_PATH=\${your_build_subdir_path}"
            echo "    [ \$DRY_RUN -eq 0 ] && export BLD_PATH=\${your_build_subdir_path}"
            echo "    cmd  export OUTPUT_BUILD_DIR=\$PWD/../build"
            echo "    [ \$DRY_RUN -eq 0 ] && export OUTPUT_BUILD_DIR=\$PWD/../build"
            local_tmp_data="    make O=\"\${OUTPUT_BUILD_DIR}\" \${PARALLEL_MAKE} st/\${linux_dtb_name}.dtb KBUILD_EXTDTS=\${externaldt_path}/\${externaldt_linux_path}"
            echo "    cmd \"${local_tmp_data}\""
            echo "[ \$DRY_RUN -eq 0 ] && ${local_tmp_data} || die ${c}"
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
                    echo "[ \$DRY_RUN -eq 0 ] && ${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            IFS=$old_IFS
            echo "    cmd    export BLD_PATH=\${your_build_subdir_path}"
            echo "    [ \$DRY_RUN -eq 0 ] && export BLD_PATH=\${your_build_subdir_path}"
            echo "    cmd  export OUTPUT_BUILD_DIR=\$PWD/../build"
            echo "    [ \$DRY_RUN -eq 0 ] && export OUTPUT_BUILD_DIR=\$PWD/../build"
            local_tmp_data="    make O=\"\${OUTPUT_BUILD_DIR}\" \${PARALLEL_MAKE} dtbs KBUILD_EXTDTS=\${externaldt_path}/\${externaldt_linux_path}"
            echo "    cmd \"${local_tmp_data}\""
            echo "[ \$DRY_RUN -eq 0 ] && {local_tmp_data} || die ${c}"
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
                    echo "[ \$DRY_RUN -eq 0 ] &&  ${local_tmp_data}" | sed "s|[[:space:]]*\$@P>|   |"
                fi
            done
            IFS=$old_IFS
            echo "    cmd    export BLD_PATH=\${your_build_subdir_path}"
            echo "    [ \$DRY_RUN -eq 0 ] && export BLD_PATH=\${your_build_subdir_path}"
            echo "    cmd  export OUTPUT_BUILD_DIR=\$PWD/../build"
            echo "    [ \$DRY_RUN -eq 0 ] && export OUTPUT_BUILD_DIR=\$PWD/../build"
            local_tmp_data="    make O=\"\${OUTPUT_BUILD_DIR}\" \${PARALLEL_MAKE} modules modules_install INSTALL_MOD_PATH=\"\${OUTPUT_BUILD_DIR}/install_artifact\" KBUILD_EXTDTS=\${externaldt_path}/\${externaldt_linux_path}"
            echo "    cmd \"${local_tmp_data}\"" | sed "s|[[:space:]]*\$@C>||"
            echo "[ \$DRY_RUN -eq 0 ] && ${local_tmp_data} || die ${c}" | sed "s|[[:space:]]*\$@C>|   |"

            echo "    cmd \" mkdir -p \${FIP_DEPLOYDIR_ROOT}/kernel/modules\""
            echo "    [ \$DRY_RUN -eq 0 ] &&  mkdir -p \${FIP_DEPLOYDIR_ROOT}/kernel/modules || die linux-stm32mp"
            echo "    cmd \" mkdir -p \${FIP_DEPLOYDIR_ROOT}/kernel/modules_stripped\""
            echo "    [ \$DRY_RUN -eq 0 ] && mkdir -p \${FIP_DEPLOYDIR_ROOT}/kernel/modules_stripped || die linux-stm32mp"
            echo "    cmd \" rm \${OUTPUT_BUILD_DIR}/install_artifact/lib/modules/*/build\""
            echo "    [ \$DRY_RUN -eq 0 ] && rm \${OUTPUT_BUILD_DIR}/install_artifact/lib/modules/*/build || die linux-stm32mp"
            echo "    cmd \" cp -ar \${OUTPUT_BUILD_DIR}/install_artifact/* \${FIP_DEPLOYDIR_ROOT}/kernel/modules\""
            echo "    [ \$DRY_RUN -eq 0 ] && cp -ar \${OUTPUT_BUILD_DIR}/install_artifact/* \${FIP_DEPLOYDIR_ROOT}/kernel/modules || die linux-stm32mp"
            echo "    cmd \" cp -ar \${OUTPUT_BUILD_DIR}/install_artifact/* \${FIP_DEPLOYDIR_ROOT}/kernel/modules_stripped\""
            echo "    [ \$DRY_RUN -eq 0 ] && cp -ar \${OUTPUT_BUILD_DIR}/install_artifact/* \${FIP_DEPLOYDIR_ROOT}/kernel/modules_stripped || die linux-stm32mp"
            echo "    cmd \" find \${FIP_DEPLOYDIR_ROOT}/kernel/modules_stripped -name "*.ko" | xargs \$STRIP --strip-debug --remove-section=.comment --remove-section=.note --preserve-dates\""
            echo "    [ \$DRY_RUN -eq 0 ] && find \${FIP_DEPLOYDIR_ROOT}/kernel/modules_stripped -name \"*.ko\" | xargs \$STRIP --strip-debug --remove-section=.comment --remove-section=.note --preserve-dates || die linux-stm32mp"

            echo "    cd \$localpath"
            echo "    echo \"**** ${c}_modules ****END****\""
            echo "}"

        fi
    done
}

function usage() {
    echo "generated_build_script <machine name>"
    echo "  <machine name>: name of the machine which can be use for sdk compilation"
    echo "      stm32mp1 or stm32mp1-mx"
    echo "      stm32mp2 or stm32mp2-mx"
    echo "      stm32mp2-m33td or stm32mp2-m33td-mx"
}
# ------------------------------------------
# Main
# ------------------------------------------
# parse argument
if [ $# -eq 0 ]; then
    usage
    exit 1
fi
MX_NAME=""
while [[ $# -gt 0 ]]; do
  GLOBAL_CONFIGURATION=$1
  case $1 in
    help)
      usage
      exit 0
      ;;
    stm32mp1-mx)
      MACHINE=stm32mp1
      MX=1
      MX_NAME="-mx"
      shift
      ;;
    stm32mp1)
      MACHINE=stm32mp1
      MX=0
      shift;
      ;;
    stm32mp2-mx)
      MACHINE=stm32mp2
      MX=1
      MX_NAME="-mx"
      shift
      ;;
    stm32mp2)
      MACHINE=stm32mp2
      MX=0
      shift
      ;;
    stm32mp2-m33td-mx)
      MACHINE=stm32mp2-m33td
      MX=1
      MX_NAME="-mx"
      shift
      ;;
    stm32mp2-m33td)
      MACHINE=stm32mp2-m33td
      MX=0
      shift
      ;;
    *)
      usage
      exit 1
      ;;
  esac
done

if [ $MX -eq 1 ]; then
    info "MACHINE=$MACHINE MX"
else
    info "MACHINE=$MACHINE"
fi

# generate list of components
COMPONENTS=$(cat ${SCRIPT_PATH}/README.HOW_TO.txt.${MACHINE} | grep ^@ | sed "s/^@[A-Z]* //g")
debug "COMPONENT LIST=>$COMPONENTS<"
# generate list of components used for fip generation
COMPONENTS_FIP=$(cat ${SCRIPT_PATH}/README.HOW_TO.txt.${MACHINE} | grep ^@F | sed "s/^@[A-Z]* //g")
# create file name for outpu script file

OUTPUT_SCRIPT=${SCRIPT_PATH}/../sdk_compilation-${MACHINE}${MX_NAME}-my-custom-board.sh

COMMON_SCRIPT_NAME=sdk_action-common-for-${GLOBAL_CONFIGURATION}.source
OUTPUT_COMMON_SCRIPT=${SCRIPT_PATH}/../${COMMON_SCRIPT_NAME}

debug "Machine $MACHINE"
debug "is MX machine = $MX"


cat << EOF > $OUTPUT_SCRIPT
#!/bin/bash

$(config_stm32mp $MACHINE $MX)

source $COMMON_SCRIPT_NAME
EOF

cat << EOF > $OUTPUT_COMMON_SCRIPT
#!/bin/bash

# do not execute the command, only display command if equal to 1
DRY_RUN=0

# ----------------------------------------
die() {
    if [ \$DRY_RUN -eq 0 ]; then
        >&2 echo "FAILED > BUILD ISSUE on \$*"
        exit 1
    fi
}
cmd(){
    echo "[CMD]:> \$@"
}

# -----------------------------------------
$(generate_component_function)

$(generate_action_set)

# -----------------------------------------
#              MAIN
# -----------------------------------------
if [ \$# -ne 1 ];
then
    if [ "\$1" = "--dry-run" ]; then
        DRY_RUN=1
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
deploy-for-fip)
$(generate_action_deploy_for_fip)
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
*)
    echo "Help:"
    echo "\$0 [extract|compile|compile-for-fip|deploy|programmer|<component>-<action>]"
    echo "action:"
    echo "    extract: extract the source for all components"
    echo "    compile-for-fip: compile all the component needed to generated fip"
    echo "    deploy-for-fip:  deploy all the component needed to generated fip and generate fip"
    echo "    compile:     compile all the component for runtime"
    echo "    deploy:     generate the deploy with input of each components (need to have made compile of component before)"
    echo "    programmer: binaries for programmer usage"
    echo "    clean:      clean all components (remove all build directory, keep extracted source code)"
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
    echo "    for linux-stm32mp there is to more possible action (already included on compil)"
    echo "         -dtb"
    echo "         -dtbs"
    echo "         -modules"
    echo "component:"
$(generate_component_list)
    echo ""
    echo "Preconize step:"
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
    echo "   For stm32mp2-m33td machine, the M33 part"
    echo "    \$0  tf-m-stm32mp"
    echo "    \$0  m33tdprojects-starter-stm32mp2"
EOF
    fi

cat << EOF >> $OUTPUT_COMMON_SCRIPT
    ;;
esac
EOF

info ""
info "******************************************"
info "This script generated two files:"
info "* one with all the command for compiling the components: $OUTPUT_COMMON_SCRIPT"
info "* one for your custom configuration: $OUTPUT_SCRIPT"
info ""
info "You MUST fill the information on the custom file to match with your need "
info "and it is preferable to rename the file to match with your need"
info " (and not to be overwrited by a new launch of this script $0"
info "******************************************"

chmod +x $OUTPUT_SCRIPT
