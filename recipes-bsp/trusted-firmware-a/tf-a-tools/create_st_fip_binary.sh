#!/bin/bash -
#===============================================================================
#  ORGANIZATION: STMicroelectronics
#     COPYRIGHT: Copyright (C) 2024, STMicroelectronics - All Rights Reserved
#       License: BSD 3 Claused
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

# set environment variable needed by script
DEFAULT_ENCRYPT_SUFFIX=${ENCRYPT_SUFFIX:-_Encrypted}
DEFAULT_FIP_ENCRYPT_NONCE=${FIP_ENCRYPT_NONCE:-1234567890abcdef12345678}
DEFAULT_SIGN_SUFFIX=${SIGN_SUFFIX:-_Signed}

TOOLS_FIPTOOL=${FIPTOOL:-fiptool}
TOOLS_CERTTOOL=${CERTTOOL:-cert_create}
TOOLS_ENCTOOL=${ENCTOOL:-encrypt_fw}

# Configure default folder path for binaries to package
FIP_DEPLOYDIR_ROOT="${FIP_DEPLOYDIR_ROOT:-}"
FIP_DEPLOYDIR_BL31="${FIP_DEPLOYDIR_BL31:-$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware/bl31}"
FIP_DEPLOYDIR_FWDDR="${FIP_DEPLOYDIR_FWDDR:-$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware/ddr}"
FIP_DEPLOYDIR_FWCONF="${FIP_DEPLOYDIR_FWCONF:-$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware/fwconfig}"
FIP_DEPLOYDIR_TFA="${FIP_DEPLOYDIR_TFA:-$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware/bl32}"
FIP_DEPLOYDIR_OPTEE="${FIP_DEPLOYDIR_OPTEE:-$FIP_DEPLOYDIR_ROOT/optee}"
FIP_DEPLOYDIR_UBOOT="${FIP_DEPLOYDIR_UBOOT:-$FIP_DEPLOYDIR_ROOT/u-boot}"

FIP_DEPLOYDIR_FIP="${FIP_DEPLOYDIR_FIP:-$FIP_DEPLOYDIR_ROOT/fip}"

# Variable
DRY_RUN=0
NEED_TO_SIGN=0
NEED_TO_ENCRYPT=0
ENCRYPT_FILE_KEY=""
SIGN_KEY_FILE=""
SIGN_KEY_PASS=""
USE_BL31=0
# USE_BL32:  use TF-A as bl32 instead of optee
USE_BL32=0
USE_DDR=0
USE_ONLY_DDR=0
OUTPUT_DIR=""
INPUT_DIR=""
SEARCH_CONFIG="NULL"
SEARCH_STORAGE="NULL"
SEARCH_DTB="NULL"
SEARCH_DTB_SUFFIX="NULL"
SEARCH_SOC_NAME="NULL"
SEARCH_SECONDARY_CONF="NULL"

# -------------------------------------------------------------
function die() {
    echo "[TOOLS ERROR]: $@"
    exit 200
}

# -------------------------------------------------------------

function usage() {
    ret=$1
    echo ""
    echo "Help:"
    echo "   $0 [options] [-h|--help]"
    echo ""
    echo "This script generate a fip binary"
    echo ""
    echo "Environment variable used: "
    echo "    FIP_DEPLOYDIR_ROOT: path to default input folder tree (default: ${FIP_DEPLOYDIR_ROOT})"
    echo "  Input dirs:"
    echo "    FIP_DEPLOYDIR_BL31:   path for bl31 file dir"
    echo "      default: \$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware/bl31"
    echo "      current: ${FIP_DEPLOYDIR_BL31}"
    echo "    FIP_DEPLOYDIR_FWDDR:  path for DDR firwmare file dir"
    echo "      default: \$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware/ddr"
    echo "      current: ${FIP_DEPLOYDIR_FWDDR}"
    echo "    FIP_DEPLOYDIR_FWCONF: path for FWCONF file dir"
    echo "      default: \$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware/fwconfig"
    echo "      current: ${FIP_DEPLOYDIR_FWCONF}"
    echo "    FIP_DEPLOYDIR_TFA:    path for bl32 file dir"
    echo "      default: \$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware/bl32"
    echo "      current: ${FIP_DEPLOYDIR_TFA}"
    echo "    FIP_DEPLOYDIR_OPTEE:  path for Optee file dir"
    echo "      default: \$FIP_DEPLOYDIR_ROOT/optee"
    echo "      current: ${FIP_DEPLOYDIR_OPTEE}"
    echo "    FIP_DEPLOYDIR_UBOOT:  path for U-boot file dir"
    echo "      default: \$FIP_DEPLOYDIR_ROOT/u-boot"
    echo "      current: ${FIP_DEPLOYDIR_UBOOT}"
    echo "  Output dir:"
    echo "    FIP_DEPLOYDIR_FIP: path for FIP file output dir"
    echo "      default: \$FIP_DEPLOYDIR_ROOT/fip"
    echo "      current: ${FIP_DEPLOYDIR_FIP}"
    echo ""
    echo "    ENCRYPT_SUFFIX:  suffix use to name the encrypted file (default: $DEFAULT_ENCRYPT_SUFFIX)"
    echo "    SIGN_SUFFIX:  suffix use to name the signed file (default: $DEFAULT_SIGN_SUFFIX)"
    echo "    FIP_ENCRYPT_NONCE: nonce for encryption (default: $DEFAULT_FIP_ENCRYPT_NONCE)"
    echo ""
    echo "    FIPTOOL: fiptool tool (default: $TOOLS_FIPTOOL)"
    echo "    CERTTOOL: cert_create  tool: (default: $TOOLS_CERTTOOL)"
    echo "    ENCTOOL: encrypt_fw tool (default: $TOOLS_ENCTOOL)"
    echo ""
    echo "Parameters:"
    echo "    -h | --help: this help"
    echo "    -b | --use-bl31: use BL31 binary"
    echo "    -c | --use-bl32: use BL32 binary instead of optee binary"
    echo "    -d | --use-ddr: add ddr firmware on FIP binary"
    echo "    -I <INPUT_DIR>| --input <INPUT_DIR>: directory prefix for search input binaries and fip output dir"
    echo "    -O <FIP_DEPLOYDIR_FIP>| --output <FIP_DEPLOYDIR_FIP>: FIP output directory"
    echo "    Search parameters:"
    echo "    -C <configuration>| --search-configuration <configuration>: configuration name for finding file"
    echo "    -D <devicetree>| --search-devicetree <devicetree>: devicetree name for finding file"
    echo "    -DS <devicetree_suffix>| --search-devicetree-suffix <devicetree_suffix>: devicetree suffix for finding file"
    echo "    -S <storage>| --search-storage <storage>: storage name for finding file"
    echo "    -T <soc name>| --search-soc-name <soc name>: soc name for finding file"
    echo "    -U <configuration>| --search-secondary-config <configuration>: second configuration name for finding file"
    echo "         configuration: list of configuration separated by ':'"
    echo "            ex.: --search-secondary-config default:optee"
    echo "                 --search-secondary-config fastboot-emmc:optee"
    echo "                 --search-secondary-config dafault:opteemin"
    echo "    DDR parameters:"
    echo "    -g | --generate-only-ddr: generate only FIP ddr firmware"
    echo "    Signature parameters:"
    echo "    -s | --sign: sign binaries"
    echo "    -K <signature key file>| --signature-key <signature key file>: signature key file for signature"
    echo "    -P <signature pass>| --signature-key-pass <signature pass>: signature key pass for signature"
    echo "    Encryption parameters:"
    echo "    -E <encrypt file key name>| --encrypt <encrypt file key name>: use encryption and use specific key file"
    echo ""
    echo "Examples:"
    echo "  Generate only fip ddr binary for stm32mp257f-dk"
    echo "  $0 -O deploy/images/stm32mp2/fip --use-ddr --generate-only-ddr -C optee-emmc -D stm32mp257f-dk --search-secondary-config default:optee"
    echo "  Generate fip binary for stm32mp257f-dk and sdcard storage"
    echo "  $0 -O deploy/images/stm32mp2/fip --use-ddr -C optee-sdcard -D stm32mp257f-dk -T stm32mp25 --search-secondary-config default:optee"
    echo "  Generate fip binary for stm32mp157f-ev1 and emmc storage (no ddr used with stm32mp1)"
    echo "  $0 -O deploy/images/stm32mp2/fip -C optee-emmc -D stm32mp157f-ev1 -T stm32mp15 --search-secondary-config default:opteemin"
    echo "  Generate fip binary for stm32mp257f-dk and sdcard storage and signed and encrypted"
    echo "  $0 -O deploy/images/stm32mp2/fip --use-ddr -C optee-sdcard -D stm32mp257f-dk -T stm32mp25 --search-secondary-config default:optee \\"
    echo "               -s -K key/stm32mp25/privateKey00.pem -P <my pass> -E key/stm32mp25/edmk-fip.bin"
    echo ""
    echo "Examples per boards and use cases:"

    echo "STM32MP157F-EV1 opteemin sdcard" 
    echo "$0 --search-configuration opteemin-sdcard --search-secondary-config default:opteemin --search-devicetree stm32mp157f-ev1 \\"
    echo "   --search-soc-name stm32mp15 \\"
    echo "   --output deploy-dir/fip"
    echo "STM32MP157F-EV1 opteemin emmc" 
    echo "$0 --search-configuration opteemin-sdcard --search-secondary-config default:opteemin --search-devicetree stm32mp157f-ev1 \\"
    echo "   --search-soc-name stm32mp15 \\"
    echo "   --output deploy-dir/fip"
    echo "STM32MP137F-EV1 opteemin sdcard signed"
    echo "$0 --search-configuration opteemin-sdcard --search-secondary-config default:opteemin--search-devicetree stm32mp157f-ev1 \\"
    echo "   --search-soc-name stm32mp15 \\"
    echo "    -s -K key/stm32mp15/privateKey00.pem -P <my pass> \\"
    echo "   --output deploy-dir/fip"
    echo ""
    echo "STM32MP135F-DK optee sdcard" 
    echo "$0 --search-configuration optee-sdcard --search-secondary-config default:optee --search-devicetree stm32mp135f-dk \\"
    echo "   --search-soc-name stm32mp13  \\"
    echo "   --output deploy-dir/fip"
    echo "STM32MP135F-DK optee sdcard signed" 
    echo "$0 --search-configuration optee-sdcard --search-secondary-config default:optee --search-devicetree stm32mp135f-dk \\"
    echo "   --search-soc-name stm32mp13 \\"
    echo "   -s -K key/stm32mp13/privateKey00.pem -P <my pass>  \\"
    echo "   --output deploy-dir/fip"
    echo "STM32MP135F-DK optee sdcard signed encrypted" 
    echo "$0 --search-configuration optee-sdcard --search-secondary-config default:optee --search-devicetree stm32mp135f-dk \\"
    echo "   --search-soc-name stm32mp13 \\"
    echo "   -s -K key/stm32mp13/privateKey00.pem -P <my pass> -E key/stm32mp13/encryption_key.bin \\"
    echo "   --output deploy-dir/fip"
    echo ""
    echo "STM32MP257F-EV1 optee sdcard" 
    echo "$0 --use-ddr --generate-only-ddr --use-bl31 \\"
    echo "   --search-configuration optee-sdcard --search-storage optee-sdcard \\"
    echo "   --search-devicetree stm32mp257f-ev1 --search-soc-name stm32mp25 \\"
    echo "   --output deploy-dir/fip"
    echo "$0 --use-ddr --use-bl31 \\"
    echo "   --search-configuration optee-sdcard --search-secondary-config default:optee --search-storage optee-sdcard \\"
    echo "   --search-devicetree stm32mp257f-ev1 --search-soc-name stm32mp25 \\"
    echo "   --output deploy-dir/fip"
    echo "STM32MP257F-EV1 optee emmc" 
    echo "$0 --use-ddr --generate-only-ddr --use-bl31 \\"
    echo "   --search-configuration optee-emmc --search-storage optee-emmc --search-storage optee-emmc \\"
    echo "   --search-devicetree stm32mp257f-ev1 --search-soc-name stm32mp25 \\"
    echo "   --output deploy-dir/fip"
    echo "$0 --use-ddr --use-bl31 \\"
    echo "   --search-configuration optee-emmc --search-secondary-config default:optee \\"
    echo "   --search-devicetree stm32mp257f-ev1 --search-soc-name stm32mp25 \\"
    echo "   --output deploy-dir/fip"
    echo "STM32MP257F-EV1 optee sdcard signed" 
    echo "$0 --use-ddr --generate-only-ddr --use-bl31 \\"
    echo "   --search-configuration optee-sdcard --search-storage optee-sdcard \\"
    echo "   --search-devicetree stm32mp257f-ev1 --search-soc-name stm32mp25 \\"
    echo "   --sign --signature-key key/stm32mp25/edmk-fip.bin \\"
    echo "   --output deploy-dir/fip"
    echo "$0 --use-ddr --use-bl31 \\"
    echo "   --search-configuration optee-sdcard --search-secondary-config default:optee --search-storage optee-sdcard \\"
    echo "   --search-devicetree stm32mp257f-ev1 --search-soc-name stm32mp25\\"
    echo "   --sign --signature-key key/stm32mp25/edmk-fip.bin \\"
    echo "   --output deploy-dir/fip"
    echo "STM32MP257F-EV1 optee sdcard signed encrypted" 
    echo "$0 --use-ddr --generate-only-ddr --use-bl31 \\"
    echo "   --search-configuration optee-sdcard --search-storage optee-sdcard \\"
    echo "   --search-devicetree stm32mp257f-ev1 --search-soc-name stm32mp25 \\"
    echo "   --sign --signature-key key/stm32mp25/edmk-fip.bin -E key/stm32mp25/edmk.bin \\"
    echo "   --output deploy-dir/fip"
    echo "$0 --use-ddr --use-bl31 \\"
    echo "   --search-configuration optee-sdcard --search-secondary-config default:optee --search-storage optee-sdcard \\"
    echo "   --search-devicetree stm32mp257f-ev1 --search-soc-name stm32mp25 \\"
    echo "   --sign --signature-key key/stm32mp25/edmk-fip.bin -E key/stm32mp25/edmk.bin\\"
    echo "   --output deploy-dir/fip"
    echo "STM32MP257F-EV1 fastboot sdcard" 
    echo "$0 --use-ddr --generate-only-ddr --use-bl31 \\"
    echo "   --search-configuration fastboot-sdcard --search-storage optee-sdcard \\"
    echo "   --search-devicetree stm32mp257f-ev1 --search-soc-name stm32mp25 \\"
    echo "   --output deploy-dir/fip"
    echo "$0 --use-ddr --use-bl31 \\"
    echo "   --search-configuration fastboot-sdcard --search-secondary-config fastboot-sdcard:optee --search-storage optee-sdcard \\"
    echo "   --search-devicetree stm32mp257f-ev1 --search-soc-name stm32mp25 \\"
    echo "   --output deploy-dir/fip"

    exit $ret
}
function dump_input_dir_presence() {
    echo "Presence of directories for generating FIP file:"
    local dir=""
    # BL31
    dir="present"
    [ -d "${FIP_DEPLOYDIR_BL31}" ] || dir="NOT Present"
    [ $USE_BL31 -eq 1 ] && printf "%-8s [%10s] [%s]\n" "BL31" "$dir" "${FIP_DEPLOYDIR_BL31}"
    # DDR Firmware
    dir="present"
    [ -d "${FIP_DEPLOYDIR_FWDDR}" ] || dir="NOT Present"
    [ $USE_DDR -eq 1 ] && printf "%-8s [%10s] [%s]\n" "DDR_fw" "$dir" "${FIP_DEPLOYDIR_FWDDR}"
    # FWCONF
    dir="present"
    [ -d "${FIP_DEPLOYDIR_FWCONF}" ] || dir="NOT Present"
    printf "%-8s [%10s] [%s]\n" "FW_conf" "$dir" "${FIP_DEPLOYDIR_FWCONF}"
    # U-boot
    dir="present"
    [ -d "${FIP_DEPLOYDIR_UBOOT}" ] || dir="NOT Present"
    printf "%-8s [%10s] [%s]\n" "U-Boot" "$dir" "${FIP_DEPLOYDIR_UBOOT}"
    # BL32
    dir="present"
    [ -d "${FIP_DEPLOYDIR_TFA}" ] || dir="NOT Present"
    [ $USE_BL32 -eq 1 ] && printf "%-8s [%10s] [%s]\n" "BL32" "$dir" "${FIP_DEPLOYDIR_TFA}"
    # Optee
    dir="present"
    [ -d "${FIP_DEPLOYDIR_OPTEE}" ] || dir="NOT Present"
    [ $USE_BL32 -eq 0 ] && printf "%-8s [%10s] [%s]\n" "OPTEE" "$dir" "${FIP_DEPLOYDIR_OPTEE}"
    echo
    echo "[ERROR][PARAMETER] You MUST specify a valid directory which contains the binaries for FIP generation"
    echo
    usage 10
}
function verify_input_dir() {
    # Init dirs
    if [ -n "${INPUT_DIR:-}" ]; then
        FIP_DEPLOYDIR_BL31=${INPUT_DIR}/arm-trusted-firmware/bl31
        FIP_DEPLOYDIR_FWDDR=${INPUT_DIR}/arm-trusted-firmware/ddr
        FIP_DEPLOYDIR_FWCONF=${INPUT_DIR}/arm-trusted-firmware/fwconfig
        FIP_DEPLOYDIR_TFA=${INPUT_DIR}/arm-trusted-firmware/bl32
        FIP_DEPLOYDIR_OPTEE=${INPUT_DIR}/optee
        FIP_DEPLOYDIR_UBOOT=${INPUT_DIR}/u-boot
        FIP_DEPLOYDIR_FIP=${FIP_DEPLOYDIR_FIP:-${INPUT_DIR}/fip}
    fi

    # Verify input dirs
    if [ $USE_BL31 -eq 1 ]; then
        [ -d "${FIP_DEPLOYDIR_BL31}" ] || dump_input_dir_presence
    fi
    if [ $USE_DDR -eq 1 ]; then
        [ -d "${FIP_DEPLOYDIR_FWDDR}" ] || dump_input_dir_presence
    fi
    [ -d "${FIP_DEPLOYDIR_FWCONF}" ] || dump_input_dir_presence
    [ -d "${FIP_DEPLOYDIR_UBOOT}" ] || dump_input_dir_presence
    if [ $USE_BL32 -eq 1 ]; then
        [ -d "${FIP_DEPLOYDIR_TFA}" ] || dump_input_dir_presence
    else
        [ -d "${FIP_DEPLOYDIR_OPTEE}" ] || dump_input_dir_presence
    fi
}
function verify_signature_parameters() {
    if [ "X$NEED_TO_SIGN" = "X1" ]; then
        if [ "X$SIGN_KEY_FILE" = "X" ]; then
            echo "[ERROR][PARAMETER]: You MUST specify a signature key file"
            echo ""
            usage 20
        fi
    fi
}
function verify_encryption_parameters() {
    if [ "X$NEED_TO_ENCRYPT" = "X1" ]; then
        if [ "X$ENCRYPT_FILE_KEY" = "X" ]; then
            echo "[ERROR][PARAMETER]: You MUST specify an encryption key file"
            echo ""
            usage 30
        fi
    fi
}
function verify_configuration_parameter() {
    if [ "X$SEARCH_CONFIG" = "XNULL" ]; then
        echo "[ERROR][PARAMETER]: You MUST specify a configuration (like optee)"
        echo ""
        usage 2
    fi
}
function verify_dtb_parameter() {
    if [ "X$SEARCH_DTB" = "XNULL" ]; then
        echo "[ERROR][PARAMETER]: You MUST specify a devicetree"
        echo ""
        usage 3
    fi
}
function verify_dtb_suffix_parameter() {
    if [ "X$SEARCH_DTB_SUFFIX" = "XNULL" ]; then
        echo "[ERROR][PARAMETER]: You MUST specify a devicetree suffix (like emmc, sdcard, snor, ...)"
        echo ""
        usage 3
    fi
}
function verify_storage_parameter() {
    if [ "X$SEARCH_STORAGE" = "XNULL" ]; then
        echo "[ERROR][PARAMETER]: You MUST specify a storage (like emmc, sdcard, nor-sdcard, ...)"
        echo ""
        usage 4
    fi
}
function verify_storage_parameter() {
    if [ "X$SEARCH_SOC_NAME" = "XNULL" ]; then
        echo "[ERROR][PARAMETER]: You MUST specify a soc name (like stm32mp15, stm32mp13, stm32mp25,...)"
        echo ""
        usage 5
    fi
}
function verify_bl32_parameter() {
    if [ $USE_BL32 -eq 1 ] && [ $USE_BL31 -eq 1 ]; then
        echo "[ERROR][PARAMETER]: It's not possible to mix BL31 usage and BL32 usage"
        echo ""
        usage 6
    fi
}
function verify_secondary_configuration_parameter() {
    if [ "X$SEARCH_SECONDARY_CONF" = "X1" ]; then
        echo "[ERROR][PARAMETER]: You MUST specify a secondary configuration (like default)"
        echo ""
        usage 7
    fi
}
# -------------------------------------------------------------
function search_specific_storage_in_name() {
    local path=$1
    echo $path | grep -q $SEARCH_STORAGE
    echo $?
    return
}
# -------------------------------------------------------------
# found file
# param1 path of componant
# param2 prefix file name
# param3 extension file
function found_file(){
    local path_search=$1
    local file_prefix=$2
    local file_extension=$3

    # Strategy:
    # file_prefix-<DTB><DTB_SUFFIX>-<STORAGE>.extension
    # file_prefix-<DTB><DTB_SUFFIX>-<CONFIG>.extension
    # file_prefix-<DTB><DTB_SUFFIX>-<SECONDARY_CONFIG>-<STORAGE>.extension
    # file_prefix-<DTB><DTB_SUFFIX>-<SECONDARY_CONFIG>.extension
    # file_prefix-<DTB><DTB_SUFFIX>.extension
    # file_prefix-<DTB>-<STORAGE>.extension
    # file_prefix-<DTB>-<CONFIG>.extension
    # file_prefix-<DTB>-<SECONDARY_CONFIG>-<STORAGE>.extension
    # file_prefix-<DTB>-<SECONDARY_CONFIG>.extension
    # file_prefix-<DTB>.extension
    # file_prefix-<STORAGE>.extension
    # file_prefix-<CONFIG>.extension
    # file_prefix-<SECONDARY_CONFIG>.extension
    # file_prefix-<SOC NAME>-<STORAGE>.extension
    # file_prefix-<SOC NAME>-<CONFIG>.extension
    # file_prefix-<SOC NAME>-<SECONDARY_CONFIG>-<STORAGE>.extension
    # file_prefix-<SOC NAME>-<SECONDARY_CONFIG>.extension
    # file_prefix-<SOC NAME>.extension

    if [ "X${SEARCH_DTB_SUFFIX}" != "XNULL" ]; then
        [ -e "$path_search/$file_prefix-$SEARCH_DTB${SEARCH_DTB_SUFFIX}-$SEARCH_STORAGE.$file_extension" ] && echo "$path_search/$file_prefix-$SEARCH_DTB${SEARCH_DTB_SUFFIX}-$SEARCH_STORAGE.$file_extension" && return
        [ -e "$path_search/$file_prefix-$SEARCH_DTB${SEARCH_DTB_SUFFIX}-$SEARCH_CONFIG.$file_extension" ] && echo "$path_search/$file_prefix-$SEARCH_DTB${SEARCH_DTB_SUFFIX}-$SEARCH_CONFIG.$file_extension" && return
        for item in $(echo ${SEARCH_SECONDARY_CONF} | tr ':' ' '); do
            [ -e "$path_search/$file_prefix-$SEARCH_DTB${SEARCH_DTB_SUFFIX}-$item-$SEARCH_STORAGE.$file_extension" ] && echo "$path_search/$file_prefix-$SEARCH_DTB${SEARCH_DTB_SUFFIX}-$item-$SEARCH_STORAGE.$file_extension" && return
            [ -e "$path_search/$file_prefix-$SEARCH_DTB${SEARCH_DTB_SUFFIX}-$item.$file_extension" ] && echo "$path_search/$file_prefix-$SEARCH_DTB${SEARCH_DTB_SUFFIX}-$item.$file_extension" && return
        done
        [ -e "$path_search/$file_prefix-$SEARCH_DTB${SEARCH_DTB_SUFFIX}.$file_extension" ] && echo "$path_search/$file_prefix-$SEARCH_DTB${SEARCH_DTB_SUFFIX}.$file_extension" && return
    fi
    [ -e "$path_search/$file_prefix-$SEARCH_DTB-$SEARCH_STORAGE.$file_extension" ] && echo "$path_search/$file_prefix-$SEARCH_DTB-$SEARCH_STORAGE.$file_extension" && return
    [ -e "$path_search/$file_prefix-$SEARCH_DTB-$SEARCH_CONFIG.$file_extension" ] && echo "$path_search/$file_prefix-$SEARCH_DTB-$SEARCH_CONFIG.$file_extension" && return
    for item in $(echo ${SEARCH_SECONDARY_CONF} | tr ':' ' '); do
        [ -e "$path_search/$file_prefix-$SEARCH_DTB-$item-$SEARCH_STORAGE.$file_extension" ] && echo "$path_search/$file_prefix-$SEARCH_DTB-$item-$SEARCH_STORAGE.$file_extension" && return
        [ -e "$path_search/$file_prefix-$SEARCH_DTB-$item.$file_extension" ] && echo "$path_search/$file_prefix-$SEARCH_DTB-$item.$file_extension" && return
    done
    [ -e "$path_search/$file_prefix-$SEARCH_DTB.$file_extension" ] && echo "$path_search/$file_prefix-$SEARCH_DTB.$file_extension" && return
    [ -e "$path_search/$file_prefix-$SEARCH_STORAGE.$file_extension" ] && echo "$path_search/$file_prefix-$SEARCH_STORAGE.$file_extension" && return
    [ -e "$path_search/$file_prefix-$SEARCH_CONFIG.$file_extension" ] && echo "$path_search/$file_prefix-$SEARCH_CONFIG.$file_extension" && return
    for item in $(echo ${SEARCH_SECONDARY_CONF} | tr ':' ' '); do
        [ -e "$path_search/$file_prefix-$item.$file_extension" ] && echo "$path_search/$file_prefix-$item.$file_extension" && return
    done
    [ -e "$path_search/$file_prefix-$SEARCH_SOC_NAME-$SEARCH_STORAGE.$file_extension" ] && echo "$path_search/$file_prefix-$SEARCH_SOC_NAME-$SEARCH_STORAGE.$file_extension" && return
    [ -e "$path_search/$file_prefix-$SEARCH_SOC_NAME-$SEARCH_CONFIG.$file_extension" ] && echo "$path_search/$file_prefix-$SEARCH_SOC_NAME-$SEARCH_CONFIG.$file_extension" && return
    for item in $(echo ${SEARCH_SECONDARY_CONF} | tr ':' ' '); do
        [ -e "$path_search/$file_prefix-$SEARCH_SOC_NAME-$item-$SEARCH_STORAGE.$file_extension" ] && echo "$path_search/$file_prefix-$SEARCH_SOC_NAME-$item-$SEARCH_STORAGE.$file_extension" && return
        [ -e "$path_search/$file_prefix-$SEARCH_SOC_NAME-$item.$file_extension" ] && echo "$path_search/$file_prefix-$SEARCH_SOC_NAME-$item.$file_extension" && return
    done
    [ -e "$path_search/$file_prefix-$SEARCH_SOC_NAME.$file_extension" ] && echo "$path_search/$file_prefix-$SEARCH_SOC_NAME.$file_extension" && return

    echo "NOTFOUND"
}
# -------------------------------------------------------------

function process_args() {
    # check opt args
    while test $# != 0
    do
        #echo "[DEBUG][PARAM] >$1<"
        case "$1" in
        -h|--help)
            usage 0
            ;;
        -b|--use-bl31)
            USE_BL31=1
            ;;
        -c|--use-bl32)
            USE_BL32=1
            ;;
        -C|--search-configuration)
            SEARCH_CONFIG=1
            if [ $# -gt 1 ]; then
                SEARCH_CONFIG=$2
                shift
            fi
            ;;
        -d|--use-ddr)
            USE_DDR=1
            ;;
        -D|--search-devicetree)
            SEARCH_DTB=1
            if [ $# -gt 1 ]; then
                SEARCH_DTB=$2
                shift
            fi
            ;;
        -DS|--search-devicetree-suffix)
            SEARCH_DTB_SUFFIX=1
            if [ $# -gt 1 ]; then
                SEARCH_DTB_SUFFIX=$2
                shift
            fi
            ;;
        -E|--encrypt)
            NEED_TO_ENCRYPT=1
            if [ $# -gt 1 ]; then
                ENCRYPT_FILE_KEY=$2
                shift
            fi
            ;;
        -g|--generate-only-ddr)
            USE_ONLY_DDR=1
            ;;
        -I|--input)
            if [ $# -gt 1 ]; then
                INPUT_DIR=$2
                shift
            fi
            ;;
        -K|--signature-key)
            if [ $# -gt 1 ]; then
                SIGN_KEY_FILE=$2
                shift
            fi
            ;;
        -n|--dry-run)
            DRY_RUN=1
            ;;
        -O|--output)
            if [ $# -gt 1 ]; then
                FIP_DEPLOYDIR_FIP=$2
                shift
            fi
            ;;
        -P|--signature-key-pass)
            if [ $# -gt 1 ]; then
                SIGN_KEY_PASS=$2
                shift
            fi
            ;;
        -s|--sign)
            NEED_TO_SIGN=1
            ;;
        -S|--search-storage)
            SEARCH_STORAGE=1
            if [ $# -gt 1 ]; then
                SEARCH_STORAGE=$2
                shift
            fi
            ;;
        -T|--search-soc-name)
            SEARCH_SOC_NAME=1
            if [ $# -gt 1 ]; then
                SEARCH_SOC_NAME=$2
                shift
            fi
            ;;
        -U|--search-secondary-config)
            SEARCH_SECONDARY_CONF=1
            if [ $# -gt 1 ]; then
                SEARCH_SECONDARY_CONF=$2
                shift
            fi
            ;;
        -*)
            echo "Wrong parameter: $1"
            usage 1
            ;;
        esac
        shift
    done
}

# ==============================================
#                    MAIN
# ==============================================
process_args $@
verify_input_dir
verify_dtb_parameter
verify_configuration_parameter
verify_secondary_configuration_parameter
if [ $USE_ONLY_DDR -eq 0 ]; then
    verify_storage_parameter
    verify_signature_parameters
    verify_encryption_parameters
    verify_bl32_parameter
fi
#-----------------------------------------------------------
#-----------------------------------------------------------
#---- get all file and set fiptool option
file_error=0
file_contains_storage=0
file_ddr_error=0
fiptool_opt=" "
certool_opt=" "
fip_certconf_opt=""
fip_certconf_opt_addons=""

# TF-A firmware: FW config
fw_config_file=$(found_file "${FIP_DEPLOYDIR_FWCONF}" "$SEARCH_DTB${SEARCH_DTB_SUFFIX}-fw-config" "dtb")
[ "$fw_config_file" = "NOTFOUND" ] && fw_config_file=$(found_file "${FIP_DEPLOYDIR_FWCONF}" "$SEARCH_DTB-fw-config" "dtb")
fw_config_file_light=$(echo $fw_config_file | sed "s|${FIP_DEPLOYDIR_ROOT}|<PATH>|")
[ "$fw_config_file" = "NOTFOUND" ] && file_error=1
[ $file_error -eq 0 ] && fiptool_opt="$fiptool_opt --fw-config $fw_config_file"
[ $file_error -eq 0 ] && certool_opt="$certool_opt --fw-config $fw_config_file"
store=$(search_specific_storage_in_name $fw_config_file)
[ $store -eq 0 ] && file_contains_storage=1

# TF-A firmware: BL31 for arm64
if [ $USE_BL31 -eq 1 ]; then
    bl3x_fw_file=$(found_file "${FIP_DEPLOYDIR_BL31}" "tf-a-bl31" "bin")
    bl3x_fw_file_light=$(echo $bl3x_fw_file | sed "s|${FIP_DEPLOYDIR_ROOT}|<PATH>|")
    bl3x_dtb_file=$(found_file "${FIP_DEPLOYDIR_BL31}" "$SEARCH_DTB${SEARCH_DTB_SUFFIX}-bl31" "dtb")
    [ "$bl3x_dtb_file" = "NOTFOUND" ] && bl3x_dtb_file=$(found_file "${FIP_DEPLOYDIR_BL31}" "$SEARCH_DTB-bl31" "dtb")
    bl3x_dtb_file_light=$(echo $bl3x_dtb_file | sed "s|${FIP_DEPLOYDIR_ROOT}|<PATH>|")
    [ "$bl3x_fw_file" = "NOTFOUND" ] && file_error=1
    [ "$bl3x_dtb_file" = "NOTFOUND" ] && file_error=1
    store=$(search_specific_storage_in_name $bl3x_dtb_file)
    [ $store -eq 0 ] && file_contains_storage=1
    if [ $NEED_TO_ENCRYPT -eq 1 ]; then
        if [ $file_error -eq 0 ]; then
            bl3x_fw_encrypted_file=$(echo $bl3x_fw_file | sed "s|.bin|$DEFAULT_ENCRYPT_SUFFIX.bin|")
            bl3x_dtb_encrypted_file=$(echo $bl3x_dtb_file | sed "s|.dtb|$DEFAULT_ENCRYPT_SUFFIX.dtb|")
            bl3x_fw_encrypted_file_light=$(echo $bl3x_fw_encrypted_file | sed "s|${FIP_DEPLOYDIR_ROOT}|<PATH>|")
            bl3x_dtb_encrypted_file_light=$(echo $bl3x_dtb_encrypted_file | sed "s|${FIP_DEPLOYDIR_ROOT}|<PATH>|")

            fiptool_opt="$fiptool_opt --soc-fw $bl3x_fw_encrypted_file"
            fiptool_opt="$fiptool_opt --soc-fw-config $bl3x_dtb_encrypted_file"
            certool_opt="$certool_opt --soc-fw $bl3x_fw_file"
            certool_opt="$certool_opt --soc-fw-config $bl3x_dtb_file"
        fi
    else
        if [ $file_error -eq 0 ]; then
            fiptool_opt="$fiptool_opt --soc-fw $bl3x_fw_file"
            fiptool_opt="$fiptool_opt --soc-fw-config $bl3x_dtb_file"
            certool_opt="$certool_opt --soc-fw $bl3x_fw_file"
            certool_opt="$certool_opt --soc-fw-config $bl3x_dtb_file"
        fi
    fi
fi

# TF-A firmware: ddr
if [ $USE_DDR -eq 1 ]; then
    ddr_fw_file=$(found_file "${FIP_DEPLOYDIR_FWDDR}" "ddr_pmu" "bin")
    [ "$ddr_fw_file" = "NOTFOUND" ] && file_error=1
    [ "$ddr_fw_file" = "NOTFOUND" ] && file_ddr_error=1
    [ $file_error -eq 0 ] && fiptool_opt="$fiptool_opt --ddr-fw $ddr_fw_file"
    [ $file_error -eq 0 ] && certool_opt="$certool_opt --ddr-fw $ddr_fw_file"
    store=$(search_specific_storage_in_name $ddr_fw_file)
    [ $store -eq 0 ] && file_contains_storage=1
    ddr_fw_file_light=$(echo $ddr_fw_file | sed "s|${FIP_DEPLOYDIR_ROOT}|<PATH>|")
fi

# OPTEE: header
optee_header_file=$(found_file "${FIP_DEPLOYDIR_OPTEE}" "tee-header_v2" "bin")
optee_header_file_light=$(echo $optee_header_file | sed "s|${FIP_DEPLOYDIR_ROOT}|<PATH>|")
[ "$optee_header_file" = "NOTFOUND" ] && file_error=1
# OPTEE: pager
optee_pager_file=$(found_file "${FIP_DEPLOYDIR_OPTEE}" "tee-pager_v2" "bin")
optee_pager_file_light=$(echo $optee_pager_file | sed "s|${FIP_DEPLOYDIR_ROOT}|<PATH>|")
[ "$optee_pager_file" = "NOTFOUND" ] && file_error=1
# OPTEE: pageable
optee_pageable_file=$(found_file "${FIP_DEPLOYDIR_OPTEE}" "tee-pageable_v2" "bin")
optee_pageable_file_light=$(echo $optee_pageable_file | sed "s|${FIP_DEPLOYDIR_ROOT}|<PATH>|")
[ "$optee_pageable_file" = "NOTFOUND" ] && file_error=1
store=$(search_specific_storage_in_name $optee_pager_file)
[ $store -eq 0 ] && file_contains_storage=1

if [ $USE_BL32 -eq 1 ]; then
    # TF-A: BL32
    bl3x_fw_file=$(found_file "${FIP_DEPLOYDIR_TFA}" "tf-a-bl32" "bin")
    bl3x_fw_file_light=$(echo $bl3x_fw_file | sed "s|${FIP_DEPLOYDIR_ROOT}|<PATH>|")
    [ "$bl3x_fw_file" = "NOTFOUND" ] && file_error=1
    bl3x_dtb_file=$(found_file "${FIP_DEPLOYDIR_TFA}" "$SEARCH_DTB${SEARCH_DTB_SUFFIX}-bl32" "dtb")
    [ "$bl3x_dtb_file" = "NOTFOUND" ] && bl3x_dtb_file=$(found_file "${FIP_DEPLOYDIR_TFA}" "$SEARCH_DTB-bl32" "dtb")
    bl3x_dtb_file_light=$(echo $bl3x_dtb_file | sed "s|${FIP_DEPLOYDIR_ROOT}|<PATH>|")
    [ "$bl3x_fw_file" = "NOTFOUND" ] && file_error=1
    [ $file_error -eq 0 ] && fiptool_opt="$fiptool_opt --tos-fw $bl3x_fw_file"
    [ $file_error -eq 0 ] && fiptool_opt="$fiptool_opt --tos-fw-config $bl3x_dtb_file"
else
    if [ $NEED_TO_ENCRYPT -eq 1 ]; then
        if [ $file_error -eq 0 ]; then
            optee_header_encrypted_file=$(echo $optee_header_file | sed "s|.bin|$DEFAULT_ENCRYPT_SUFFIX.bin|")
            optee_pager_encrypted_file=$(echo $optee_pager_file | sed "s|.bin|$DEFAULT_ENCRYPT_SUFFIX.bin|")
            optee_pageable_encrypted_file=$(echo $optee_pageable_file | sed "s|.bin|$DEFAULT_ENCRYPT_SUFFIX.bin|")
            optee_header_encrypted_file_light=$(echo $optee_header_encrypted_file | sed "s|${FIP_DEPLOYDIR_ROOT}|<PATH>|")
            optee_pager_encrypted_file_light=$(echo $optee_pager_encrypted_file | sed "s|${FIP_DEPLOYDIR_ROOT}|<PATH>|")
            optee_pageable_encrypted_file_light=$(echo $optee_pageable_encrypted_file | sed "s|${FIP_DEPLOYDIR_ROOT}|<PATH>|")

            fiptool_opt="$fiptool_opt --tos-fw $optee_header_encrypted_file"
            fiptool_opt="$fiptool_opt --tos-fw-extra1 $optee_pager_encrypted_file"
            fiptool_opt="$fiptool_opt --tos-fw-extra2 $optee_pageable_encrypted_file"
            certool_opt="$certool_opt --tos-fw $optee_header_file"
            certool_opt="$certool_opt --tos-fw-extra1 $optee_pager_file"
            certool_opt="$certool_opt --tos-fw-extra2 $optee_pageable_file"
        fi
    else
        if [ $file_error -eq 0 ]; then
            fiptool_opt="$fiptool_opt --tos-fw $optee_header_file"
            fiptool_opt="$fiptool_opt --tos-fw-extra1 $optee_pager_file"
            fiptool_opt="$fiptool_opt --tos-fw-extra2 $optee_pageable_file"
            certool_opt="$certool_opt --tos-fw $optee_header_file"
            certool_opt="$certool_opt --tos-fw-extra1 $optee_pager_file"
            certool_opt="$certool_opt --tos-fw-extra2 $optee_pageable_file"
        fi
    fi
fi

# U-BOOT: no-dtb.bin
u_boot_fw_file=$(found_file "${FIP_DEPLOYDIR_UBOOT}" "u-boot-nodtb" "bin")
u_boot_fw_file_light=$(echo $u_boot_fw_file | sed "s|${FIP_DEPLOYDIR_ROOT}|<PATH>|")
[ "$u_boot_fw_file" = "NOTFOUND" ] && file_error=1
[ $file_error -eq 0 ] && fiptool_opt="$fiptool_opt --nt-fw $u_boot_fw_file"
[ $file_error -eq 0 ] && certool_opt="$certool_opt --nt-fw $u_boot_fw_file"
store=$(search_specific_storage_in_name $u_boot_fw_file)
[ $store -eq 0 ] && file_contains_storage=1

# U-BOOT:dtb
u_boot_dtb_file=$(found_file "${FIP_DEPLOYDIR_UBOOT}" "u-boot" "dtb")
u_boot_dtb_file_light=$(echo $u_boot_dtb_file | sed "s|${FIP_DEPLOYDIR_ROOT}|<PATH>|")
[ "$u_boot_dtb_file" = "NOTFOUND" ] && file_error=1
[ $file_error -eq 0 ] && fiptool_opt="$fiptool_opt --hw-config $u_boot_dtb_file"
[ $file_error -eq 0 ] && certool_opt="$certool_opt --hw-config $u_boot_dtb_file"

# dump information about files
echo "------------------------------------"
echo "File found:"
fip_addons_name="-$SEARCH_CONFIG"
if [ $USE_ONLY_DDR -eq 1 ]; then
    printf " %-8s\n" "arm-trusted-firmware"
    [ $USE_DDR -eq 1 ]  && printf "%16s : %s\n" DDR_fw $ddr_fw_file
    # reset fiptool_opt to have only ddr firmware
    fiptool_opt=" "
    [ $file_ddr_error -eq 0 ] && fiptool_opt="$fiptool_opt --ddr-fw $ddr_fw_file"
    if [ $file_ddr_error -gt 0 ]; then
        echo "[ERROR] some files are not present, please provide it or change the parameters"
        echo ""
        exit 100
    fi
    fip_addons_name="-ddr-$SEARCH_CONFIG"
else
    printf " %-8s\n" "arm-trusted-firmware"
    printf "%16s : %s\n" FW_config $fw_config_file
    [ $USE_BL31 -eq 1 ] && printf "%16s : %s\n" BL31_fw $bl3x_fw_file
    [ $USE_BL31 -eq 1 ] && printf "%16s : %s\n" BL31_dtb $bl3x_dtb_file
    [ $USE_BL32 -eq 1 ] && printf "%16s : %s\n" BL32_fw $bl3x_fw_file
    [ $USE_BL32 -eq 1 ] && printf "%16s : %s\n" BL32_dtb $bl3x_dtb_file
    [ $USE_DDR -eq 1 ]  && printf "%16s : %s\n" DDR_fw $ddr_fw_file
    [ $USE_BL32 -eq 0 ] && printf " %-8s\n" "optee-os"
    [ $USE_BL32 -eq 0 ] && printf "%16s : %s\n" Optee_header $optee_header_file
    [ $USE_BL32 -eq 0 ] && printf "%16s : %s\n" Optee_pager $optee_pager_file
    [ $USE_BL32 -eq 0 ] && printf "%16s : %s\n" Optee_pageable $optee_pageable_file
    printf " %-8s\n" "U-Boot"
    printf "%16s : %s\n" U-BOOT_FW $u_boot_fw_file
    printf "%16s : %s\n" U-BOOT_dtb $u_boot_dtb_file
    if [ $file_error -gt 0 ]; then
        echo "[ERROR] some files are not present, please provide it or change the parameters"
        echo ""
        exit 100
    fi
fi

# ---------------------------------------------------
# Signature
if [ $NEED_TO_SIGN -eq 1 ]; then
    sign_key="${SIGN_KEY_FILE}"
    sign_single_key_pass="${SIGN_KEY_PASS}"
fi
# ---------------------------------------------------

echo "------------------------------------"
fiptool_output_file_suffix=""
if [ $NEED_TO_ENCRYPT -eq 1 ]; then
    fiptool_output_file_suffix="$DEFAULT_ENCRYPT_SUFFIX"
fi
if [ $NEED_TO_SIGN -eq 1 ]; then
    fiptool_output_file_suffix="$fiptool_output_file_suffix$DEFAULT_SIGN_SUFFIX"
    fiptool_output_temp_file=${FIP_DEPLOYDIR_FIP}/tmp-fip-$SEARCH_DTB-$SEARCH_STORAGE
    mkdir -p $fiptool_output_temp_file
fi
if [ $USE_ONLY_DDR -eq 1 ]; then
    # Signature for ddr fw
    if [ $NEED_TO_SIGN -eq 1 ]; then
        fip_certconf_opt="$fip_certconf_opt --stm32mp-cfg-cert $fiptool_output_temp_file/stm32mp_cfg_cert_ddr.crt"
        echo "CMD> $TOOLS_CERTTOOL -n --tfw-nvctr 0 --rot-key ${sign_key}" \
            "--rot-key-pwd ${sign_single_key_pass}" \
            "$fip_certconf_opt" \
            "--ddr-fw $ddr_fw_file" | sed "s|--|\\\ \n\t--|g"
        if [ $DRY_RUN -eq 0 ]; then
            $TOOLS_CERTTOOL \
             -n --tfw-nvctr 0 \
            --rot-key ${sign_key} \
            --rot-key-pwd ${sign_single_key_pass} \
            $fip_certconf_opt \
            --ddr-fw $ddr_fw_file  || die "CERTOOL error"
        fi
    fi
else
    if [ $NEED_TO_ENCRYPT -eq 1 ]; then
        # use optee instead of TF-A bl32
        encrypt_key="$ENCRYPT_FILE_KEY"
        if [ "$(file "${encrypt_key}" | sed 's#.*: \(.*\)$#\1#')" = "ASCII text" ]; then
            # The encryption key is already available in hexadecimal format, so just extract it from file
            encrypt_key="$(cat ${encrypt_key})"
        else
            encrypt_key="$(hexdump -e '/1 "%02x"' ${encrypt_key})"
        fi

        if [ $USE_BL32 -eq 0 ]; then
            # encrypt Optee header
            echo "CMD> $TOOLS_ENCTOOL --key ${encrypt_key} --nonce $DEFAULT_FIP_ENCRYPT_NONCE --fw-enc-status 0" \
                "--in $optee_header_file --out $optee_header_encrypted_file " | sed "s|--|\\\ \n\t--|g"
            if [ $DRY_RUN -eq 0 ]; then
                $TOOLS_ENCTOOL --key ${encrypt_key} --nonce $DEFAULT_FIP_ENCRYPT_NONCE --fw-enc-status 0 \
                    --in $optee_header_file --out $optee_header_encrypted_file  || die "ENCTOOL optee header error"
            fi
            echo "CMD> $TOOLS_ENCTOOL --key ${encrypt_key} --nonce $DEFAULT_FIP_ENCRYPT_NONCE --fw-enc-status 0" \
                "--in $optee_pager_file --out $optee_pager_encrypted_file" | sed "s|--|\\\ \n\t--|g"
            if [ $DRY_RUN -eq 0 ]; then
                $TOOLS_ENCTOOL --key ${encrypt_key} --nonce $DEFAULT_FIP_ENCRYPT_NONCE --fw-enc-status 0 \
                    --in $optee_pager_file --out $optee_pager_encrypted_file  || die "ENCTOOL optee pager error"
            fi
            echo "CMD> $TOOLS_ENCTOOL --key ${encrypt_key} --nonce $DEFAULT_FIP_ENCRYPT_NONCE --fw-enc-status 0" \
                "--in  $optee_pageable_file --out $optee_pageable_encrypted_file " | sed "s|--|\\\ \n\t--|g"
            if [ $DRY_RUN -eq 0 ]; then
                $TOOLS_ENCTOOL --key ${encrypt_key} --nonce $DEFAULT_FIP_ENCRYPT_NONCE --fw-enc-status 0 \
                    --in  $optee_pageable_file --out $optee_pageable_encrypted_file  || die "ENCTOOL optee pageable error"
            fi
        fi

        if [ $USE_BL31 -eq 1 ]; then
            echo "CMD> $TOOLS_ENCTOOL --key ${encrypt_key} --nonce $DEFAULT_FIP_ENCRYPT_NONCE --fw-enc-status 0" \
                "--in $bl3x_fw_file --out $bl3x_fw_encrypted_file " | sed "s|--|\\\ \n\t--|g"
            if [ $DRY_RUN -eq 0 ]; then
                $TOOLS_ENCTOOL --key ${encrypt_key} --nonce $DEFAULT_FIP_ENCRYPT_NONCE --fw-enc-status 0 \
                    --in $bl3x_fw_file --out $bl3x_fw_encrypted_file || die "ENCTOOL bl3x fw error"
            fi
            echo "CMD> $TOOLS_ENCTOOL --key ${encrypt_key} --nonce $DEFAULT_FIP_ENCRYPT_NONCE --fw-enc-status 0" \
                "--in $bl3x_dtb_file --out $bl3x_dtb_encrypted_file " | sed "s|--|\\\ \n\t--|g"
            if [ $DRY_RUN -eq 0 ]; then
                $TOOLS_ENCTOOL --key ${encrypt_key} --nonce $DEFAULT_FIP_ENCRYPT_NONCE --fw-enc-status 0 \
                    --in $bl3x_dtb_file --out $bl3x_dtb_encrypted_file || die "ENCTOOL bl3x dtb error"
            fi
        fi
    fi
    if [ $NEED_TO_SIGN -eq 1 ]; then
        fip_certconf_opt=""
        fip_certconf_opt_addons=""
        # certificat for Trusted Boot Firmware image file
        fip_certconf_opt="$fip_certconf_opt --tb-fw-cert $fiptool_output_temp_file/tb_fw.crt"
        fip_certconf_opt="$fip_certconf_opt --trusted-key-cert  $fiptool_output_temp_file/trusted_key.crt"
        # certificat for u-boot.bin and devicetree
        fip_certconf_opt="$fip_certconf_opt --nt-fw-cert  $fiptool_output_temp_file/nt_fw_content.crt"
        fip_certconf_opt="$fip_certconf_opt --nt-fw-key-cert  $fiptool_output_temp_file/nt_fw_key.crt"
        # certificat for BL32 or optee header and optee pager
        fip_certconf_opt="$fip_certconf_opt --tos-fw-cert  $fiptool_output_temp_file/tos_fw_content.crt"
        fip_certconf_opt="$fip_certconf_opt --tos-fw-key-cert  $fiptool_output_temp_file/tos_fw_key.crt"
        # certificat for STM32MP Config Certificate
        fip_certconf_opt="$fip_certconf_opt --stm32mp-cfg-cert $fiptool_output_temp_file/stm32mp_cfg_cert.crt"
        if [ $USE_BL31 -eq 1 ]; then
            # certificat for BL31
            fip_certconf_opt_addons="$fip_certconf_opt_addons --soc-fw-cert  $fiptool_output_temp_file/soc_fw_content.crt"
            fip_certconf_opt_addons="$fip_certconf_opt_addons --soc-fw-key-cert $fiptool_output_temp_file/soc_fw_key.crt"
        fi
        # Need fake bl2 binary to generate certificates
        touch $fiptool_output_temp_file/bl2-fake.bin
        echo "CMD> ${TOOLS_CERTTOOL} -n --tfw-nvctr 0 --ntfw-nvctr 0 --key-alg ecdsa --hash-alg sha256" \
            "--rot-key ${sign_key}" \
            "--rot-key-pwd $sign_single_key_pass" \
            "$certool_opt" \
            "$fip_certconf_opt" \
            "$fip_certconf_opt_addons" \
            "--tb-fw $fiptool_output_temp_file/bl2-fake.bin" | sed "s|--|\\\ \n\t--|g"

        if [ $DRY_RUN -eq 0 ]; then
            ${TOOLS_CERTTOOL} -n --tfw-nvctr 0 --ntfw-nvctr 0 --key-alg ecdsa --hash-alg sha256 \
                --rot-key ${sign_key} \
                --rot-key-pwd $sign_single_key_pass \
                $certool_opt \
                $fip_certconf_opt \
                $fip_certconf_opt_addons \
                --tb-fw $fiptool_output_temp_file/bl2-fake.bin || die "CERTOOL error"
        fi
    fi
fi

echo "------------------------------------"
fiptool_output_file="${FIP_DEPLOYDIR_FIP}/fip-$SEARCH_DTB${fip_addons_name}$fiptool_output_file_suffix.bin"
fiptool_output_file_dump="${FIP_DEPLOYDIR_FIP}/fip-$SEARCH_DTB${fip_addons_name}$fiptool_output_file_suffix.txt"

# dump information about files
echo "------------------------------------"
echo "List of files: " > $fiptool_output_file_dump
if [ $USE_ONLY_DDR -eq 1 ]; then
    printf " %-8s\n" "arm-trusted-firmware" >> $fiptool_output_file_dump
    [ $USE_DDR -eq 1 ]  && printf "%16s : %s\n" DDR_fw $ddr_fw_file_light >> $fiptool_output_file_dump
else
    printf " %-8s\n" "arm-trusted-firmware" >> $fiptool_output_file_dump
    printf "%16s : %s\n" FW_config $fw_config_file_light >> $fiptool_output_file_dump
    [ $USE_BL31 -eq 1 ] && printf "%16s : %s\n" BL31_fw $bl3x_fw_file_light >> $fiptool_output_file_dump
    [ $USE_BL31 -eq 1 ] && printf "%16s : %s\n" BL31_dtb $bl3x_dtb_file_light >> $fiptool_output_file_dump
    [ $USE_BL32 -eq 1 ] && printf "%16s : %s\n" BL32_fw $bl3x_fw_file_light >> $fiptool_output_file_dump
    [ $USE_BL32 -eq 1 ] && printf "%16s : %s\n" BL32_dtb $bl3x_dtb_file_light >> $fiptool_output_file_dump
    [ $USE_DDR -eq 1 ]  && printf "%16s : %s\n" DDR_fw $ddr_fw_file_light >> $fiptool_output_file_dump
    [ $USE_BL32 -eq 0 ] && printf " %-8s\n" "optee-os" >> $fiptool_output_file_dump
    [ $USE_BL32 -eq 0 ] && printf "%16s : %s\n" Optee_header $optee_header_file_light >> $fiptool_output_file_dump
    [ $USE_BL32 -eq 0 ] && printf "%16s : %s\n" Optee_pager $optee_pager_file_light >> $fiptool_output_file_dump
    [ $USE_BL32 -eq 0 ] && printf "%16s : %s\n" Optee_pageable $optee_pageable_file_light >> $fiptool_output_file_dump
    printf " %-8s\n" "U-Boot" >> $fiptool_output_file_dump
    printf "%16s : %s\n" U-BOOT_FW $u_boot_fw_file_light >> $fiptool_output_file_dump
    printf "%16s : %s\n" U-BOOT_dtb $u_boot_dtb_file_light >> $fiptool_output_file_dump
fi

echo "Fip tool command:"
echo "Fip tool command:"  >> $fiptool_output_file_dump
[ -d "${FIP_DEPLOYDIR_FIP}" ] || mkdir -p "${FIP_DEPLOYDIR_FIP}"
echo "CMD> $TOOLS_FIPTOOL create $fiptool_opt $fip_certconf_opt_addons $fip_certconf_opt \\ " | sed "s|--|\\\ \n\t--|g"
echo -e "\t$fiptool_output_file"
echo "CMD> $TOOLS_FIPTOOL create $fiptool_opt $fip_certconf_opt_addons $fip_certconf_opt \\ " | sed "s|--|\\\ \n\t--|g" | sed "s|${FIP_DEPLOYDIR_ROOT}|<PATH>|g" >> $fiptool_output_file_dump
echo -e "\t$fiptool_output_file" | sed "s|${FIP_DEPLOYDIR_FIP}|<PATH>/fip|g" >> $fiptool_output_file_dump

if [ $DRY_RUN -eq 0 ]; then
    $TOOLS_FIPTOOL create $fiptool_opt  $fip_certconf_opt $fip_certconf_opt_addons $fiptool_output_file || die "FIPTOOL error"
fi

if [ $NEED_TO_SIGN -eq 1 ]; then
    # cleanup
    rm -rf $fiptool_output_temp_file
fi
