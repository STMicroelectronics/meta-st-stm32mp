#!/bin/bash -
#===============================================================================
#  ORGANIZATION: STMicroelectronics
#     COPYRIGHT: Copyright (C) 2024, STMicroelectronics - All Rights Reserved
#       License: BSD 3 Claused
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

# set environment variable needed by script
DEFAULT_M33FW_SUBDIR_CUBE="${M33FW_SUBDIR_CUBE:-m33-projects}"
DEFAULT_M33FW_SUBDIR_FWDDR="${M33FW_SUBDIR_FWDDR:-arm-trusted-firmware-m/ddr}"
DEFAULT_M33FW_SUBDIR_TFM="${M33FW_SUBDIR_TFM:-arm-trusted-firmware-m}"
DEFAULT_M33FW_SUBDIR_M33FW="${M33FW_SUBDIR_CUBE:-m33-firmware}"
DEFAULT_SIGN_SUFFIX=${M33FW_SIGN_SUFFIX:-_Signed}

TOOLS_M33FWTOOL="${M33FWTOOL:-st_m33td_firmware_signature.sh}"

M33FW_DDR_PREFIX="${M33FW_DDR_PREFIX:-ddr_phy}"
M33FW_DDR_SUFFIX="${M33FW_DDR_SUFFIX:-bin}"
M33FW_LAYOUT_SUFFIX="${M33FW_LAYOUT_SUFFIX:-signing_layout}"
M33FW_NS_TYPE="${M33FW_NS_TYPE:-_ns}"
M33FW_S_PREFIX="${M33FW_S_PREFIX:-tfm}"
M33FW_S_TYPE="${M33FW_S_TYPE:-_s}"
M33FW_TYPE="${M33FW_TYPE:-${M33FW_S_TYPE}${M33FW_NS_TYPE}}"
M33FW_SUFFIX="${M33FW_SUFFIX:-bin}"

# Configure default folder path for binaries to package
M33FW_DEPLOYDIR_ROOT="${M33FW_DEPLOYDIR_ROOT:-}"
M33FW_DEPLOYDIR_CUBE="${M33FW_DEPLOYDIR_CUBE:-$M33FW_DEPLOYDIR_ROOT/${DEFAULT_M33FW_SUBDIR_CUBE}}"
M33FW_DEPLOYDIR_FWDDR="${M33FW_DEPLOYDIR_FWDDR:-$M33FW_DEPLOYDIR_ROOT/${DEFAULT_M33FW_SUBDIR_FWDDR}}"
M33FW_DEPLOYDIR_TFM="${M33FW_DEPLOYDIR_TFM:-$M33FW_DEPLOYDIR_ROOT/${DEFAULT_M33FW_SUBDIR_TFM}}"

M33FW_DEPLOYDIR_M33FW="${M33FW_DEPLOYDIR_M33FW:-$M33FW_DEPLOYDIR_ROOT/${DEFAULT_M33FW_SUBDIR_M33FW}}"

# Variable
DDR_FW="0"
DRY_RUN="0"
DT_CONFIG=""
INPUT_DIR=""
SIGN_KEY_FILE=""
SIGN_KEY_PASS=""
SUFFIX_BOOTA35=""
SUFFIX_BOOTM33=""

# -------------------------------------------------------------
function die() {
    echo "[TOOLS ERROR]: $@"
    exit 200
}

# -------------------------------------------------------------
function usage() {
    local ret=$1
    echo ""
    echo "Help:"
    echo "   $0 [options] [-h|--help]"
    echo ""
    echo "This script generate assembled and signed firmwares for CM33"
    echo ""
    echo "Environment variable used: "
    echo "    M33FW_DEPLOYDIR_ROOT:  path to default input folder tree (default: ${M33FW_DEPLOYDIR_ROOT})"
    echo "  Input dirs:"
    echo "    M33FW_DEPLOYDIR_CUBE:  path for CM33 NS firmware file dir"
    echo "      default: \$M33FW_DEPLOYDIR_ROOT/${DEFAULT_M33FW_SUBDIR_CUBE}"
    echo "      current: ${M33FW_DEPLOYDIR_CUBE}"
    echo "    M33FW_DEPLOYDIR_FWDDR: path for DDR firwmare file dir"
    echo "      default: \$M33FW_DEPLOYDIR_ROOT/${DEFAULT_M33FW_SUBDIR_FWDDR}"
    echo "      current: ${M33FW_DEPLOYDIR_FWDDR}"
    echo "    M33FW_DEPLOYDIR_TFM:   path for CM33 S firmware file dir"
    echo "      default: \$M33FW_DEPLOYDIR_ROOT/${DEFAULT_M33FW_SUBDIR_TFM}"
    echo "      current: ${M33FW_DEPLOYDIR_TFM}"
    echo "  Output dir:"
    echo "    M33FW_DEPLOYDIR_M33FW: path for CM33 assembled and signed firmware file output dir"
    echo "      default: \$M33FW_DEPLOYDIR_ROOT/${DEFAULT_M33FW_SUBDIR_M33FW}"
    echo "      current: ${M33FW_DEPLOYDIR_M33FW}"
    echo ""
    echo "    M33FW_SIGN_SUFFIX: suffix use to name the signed file (default: $DEFAULT_SIGN_SUFFIX)"
    echo ""
    echo "    M33FWTOOL: m33fwtool tool (default: ${TOOLS_M33FWTOOL})"
    echo ""
    echo "Parameters:"
    echo "    -h | --help:"
    echo "          this help"
    echo "    -d | --ddrfw:"
    echo "          enable ddr firmware generation"
    echo "    -m | --m33fw:"
    echo "          enable m33 assembled and signed firmware generation"
    echo "    -n | --dry-run:"
    echo "          execute script without running m33fwtool"
    echo "          (i.e. output dir is not populated with firmware files)"
    echo "    -I <INPUT_DIR> | --input <INPUT_DIR>:"
    echo "          directory prefix for search input files and CM33 firmware output dir"
    echo "    -O <M33FW_DEPLOYDIR_M33FW> | --output <M33FW_DEPLOYDIR_M33FW>:"
    echo "          CM33 firmware output directory"
    echo "    Signature parameters:"
    echo "    -K <signature key file> | --signature-key <signature key file>:"
    echo "          signature key file for signature"
    echo "    -P <signature pass> | --signature-key-pass <signature pass>:"
    echo "          signature key pass for signature"
    echo "    Search parameters:"
    echo "    -A <boot suffix for a35> | --bootsuffix-a35 <boot suffix for a35>"
    echo "          boot suffix for a35 for finding file"
    echo "    -D <devicetree> | --devicetree <devicetree>:"
    echo "          devicetree name for finding file"
    echo "    -M <boot suffix for m33> |--bootsuffix-m33 <boot suffix for m33>"
    echo "          boot suffix for m33 for finding file"
    echo ""
    echo "Examples:"
    echo "  Generate only CM33 ddr firmware for stm32mp257f-dk and sdcard as m33 boot device"
    echo "  $0 -O deploy/images/stm32mp2/m33-firmware --ddrfw -D stm32mp257f-dk-cm33tdcid-ostl -M -sdcard"
    echo "  Generate CM33 firmware for stm32mp257f-dk and sdcard as a35 and m33 boot device"
    echo "  $0 -O deploy/images/stm32mp2/m33-firmware --m33fw -D stm32mp257f-dk-cm33tdcid-ostl -A -sdcard -M -sdcard"
    echo "  Generate CM33 firmware for stm32mp257f-dk and sdcard as a35 and m33 boot device signed"
    echo "  $0 -O deploy/images/stm32mp2/m33-firmware --m33fw -D stm32mp257f-dk-cm33tdcid-ostl -A -sdcard -M -sdcard \\"
    echo "               -K key/stm32mp25/m33fw_privateKey.pem -P <my pass> "
    echo ""

    exit $ret
}
function dump_input_dir_presence() {
    local ret_code="${1}"
    echo "------------------------------------"
    echo "Presence of directories for generating M33 firmwares:"
    local dir=""

    if [ -n "${M33FW_DEPLOYDIR_ROOT}" ]; then
        # M33 Firmware root directory
        dir="present"
        [ -d "${M33FW_DEPLOYDIR_ROOT}" ] || dir="NOT Present"
        printf " %-20s [%10s] [%s]\n" "${!M33FW_DEPLOYDIR_ROOT@}" "$dir" "${M33FW_DEPLOYDIR_ROOT}"
    fi
    if [ "${DDR_FW}" -eq 1 ]; then
        # DDR Firmware
        dir="present"
        [ -d "${M33FW_DEPLOYDIR_FWDDR}" ] || dir="NOT Present"
        printf " %-20s [%10s] [%s]\n" "${!M33FW_DEPLOYDIR_FWDDR@}" "$dir" "${M33FW_DEPLOYDIR_FWDDR}"
    else
        # CUBE Firmware
        dir="present"
        [ -d "${M33FW_DEPLOYDIR_CUBE}" ] || dir="NOT Present"
        printf " %-20s [%10s] [%s]\n" "${!M33FW_DEPLOYDIR_CUBE@}" "$dir" "${M33FW_DEPLOYDIR_CUBE}"
        # TFM Firmware
        dir="present"
        [ -d "${M33FW_DEPLOYDIR_TFM}" ] || dir="NOT Present"
        printf " %-20s [%10s] [%s]\n" "${!M33FW_DEPLOYDIR_TFM@}" "$dir" "${M33FW_DEPLOYDIR_TFM}"
    fi
    if [ "${ret_code}" -gt 0 ]; then
        echo
        echo "[ERROR][PARAMETER] You MUST specify a valid directory which contains the binaries for M33 firmwares generation"
        echo
        usage 10
    fi
}
function verify_input_dir() {
    local ret_code=0
    # Init dirs
    if [ -n "${INPUT_DIR:-}" ]; then
        M33FW_DEPLOYDIR_ROOT="${M33FW_DEPLOYDIR_ROOT:-${INPUT_DIR}}"
        M33FW_DEPLOYDIR_FWDDR="${M33FW_DEPLOYDIR_FWDDR:-${INPUT_DIR}/${DEFAULT_M33FW_SUBDIR_FWDDR}}"
        M33FW_DEPLOYDIR_CUBE="${M33FW_DEPLOYDIR_CUBE:-${INPUT_DIR}/${DEFAULT_M33FW_SUBDIR_CUBE}}"
        M33FW_DEPLOYDIR_TFM="${M33FW_DEPLOYDIR_TFM:-${INPUT_DIR}/${DEFAULT_M33FW_SUBDIR_TFM}}"
        M33FW_DEPLOYDIR_M33FW=${M33FW_DEPLOYDIR_M33FW:-${INPUT_DIR}/${DEFAULT_M33FW_SUBDIR_M33FW}}
    fi
    if [ "${DDR_FW}" -eq 1 ]; then
        [ -d "${M33FW_DEPLOYDIR_FWDDR}" ] || ret_code=1
    else
        [ -d "${M33FW_DEPLOYDIR_CUBE}" ] || ret_code=1
        [ -d "${M33FW_DEPLOYDIR_TFM}" ] || ret_code=1
    fi
    dump_input_dir_presence "${ret_code}"
}

# -------------------------------------------------------------
# found file
# param1 path of component
# param2 prefix file name
# param3 extension file
function found_file(){
    local path_search=$1
    local file_prefix=$2
    local file_extension=$3

    if [ -e "${path_search}/${file_prefix}.${file_extension}" ]; then
        echo "${path_search}/${file_prefix}.${file_extension}"
    else
        local file_list=""
        file_list=$(find "${path_search}" -name "${file_prefix}.${file_extension}")
        if [ -n "${file_list}" ]; then
            echo "${file_list}"
        else
            echo "NOTFOUND"
        fi
    fi
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
        -d|--ddrfw)
            DDR_FW=1
            ;;
        -m|--m33fw)
            DDR_FW=0
            ;;
        -n|--dry-run)
            DRY_RUN=1
            ;;
        -A|--bootsuffix-a35)
            if [ $# -gt 1 ]; then
                SUFFIX_BOOTA35=$2
                shift
            fi
            ;;
        -D|--devicetree)
            if [ $# -gt 1 ]; then
                DT_CONFIG=$2
                shift
            fi
            ;;
        -K|--signature-key)
            if [ $# -gt 1 ]; then
                SIGN_KEY_FILE=$2
                shift
            fi
            ;;
        -I|--input)
            if [ $# -gt 1 ]; then
                INPUT_DIR=$2
                shift
            fi
            ;;
        -M|--bootsuffix-m33)
            if [ $# -gt 1 ]; then
                SUFFIX_BOOTM33=$2
                shift
            fi
            ;;
        -O|--output)
            if [ $# -gt 1 ]; then
                M33FW_DEPLOYDIR_M33FW=$2
                shift
            fi
            ;;
        -P|--signature-key-pass)
            if [ $# -gt 1 ]; then
                SIGN_KEY_PASS=$2
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

file_error="0"
m33fw_output_prefix=""

if [ "${DDR_FW}" -eq 1 ]; then
    fwddr_dir_display="$(echo "${M33FW_DEPLOYDIR_FWDDR}" | sed 's|'"${M33FW_DEPLOYDIR_ROOT}"'|<M33FW_DEPLOYDIR_ROOT>|')"
    fwddr_file=$(found_file "${M33FW_DEPLOYDIR_FWDDR}" "${M33FW_DDR_PREFIX}-${DT_CONFIG}${SUFFIX_BOOTM33}" "${M33FW_DDR_SUFFIX}")
    fwddr_file_display=$(echo "${fwddr_file}" | sed "s|^|DDR_fw |;s|${M33FW_DEPLOYDIR_FWDDR}|<${!M33FW_DEPLOYDIR_FWDDR@}>|")
    [ "${fwddr_file}" = "NOTFOUND" ] && file_error=1
    fwddr_layout_file=$(found_file "${M33FW_DEPLOYDIR_FWDDR}" "${M33FW_DDR_PREFIX}-${DT_CONFIG}${SUFFIX_BOOTM33}" "${M33FW_LAYOUT_SUFFIX}")
    fwddr_layout_file_display=$(echo "${fwddr_layout_file}" | sed "s|^|DDR_layout |;s|${M33FW_DEPLOYDIR_FWDDR}|<${!M33FW_DEPLOYDIR_FWDDR@}>|")
    [ "${fwddr_layout_file}" = "NOTFOUND" ] && file_error=1

    # Init ouput file prefix
    m33fw_output_prefix="${M33FW_DDR_PREFIX}"

else
    fw_s_dir_display="$(echo "${M33FW_DEPLOYDIR_TFM}" | sed 's|'"${M33FW_DEPLOYDIR_ROOT}"'|<M33FW_DEPLOYDIR_ROOT>|')"
    fw_s_file=$(found_file "${M33FW_DEPLOYDIR_TFM}" "${M33FW_S_PREFIX}-${DT_CONFIG}${SUFFIX_BOOTM33}${SUFFIX_BOOTA35}${M33FW_S_TYPE}" "${M33FW_SUFFIX}")
    fw_s_file_display=$(echo "${fw_s_file}" | sed "s|^|M33_S_fw |;s|${M33FW_DEPLOYDIR_TFM}|<${!M33FW_DEPLOYDIR_TFM@}>|")
    [ "${fw_s_file}" = "NOTFOUND" ] && file_error=1
    fw_s_layout_file=$(found_file "${M33FW_DEPLOYDIR_TFM}" "${M33FW_S_PREFIX}-${DT_CONFIG}${SUFFIX_BOOTM33}${SUFFIX_BOOTA35}${M33FW_S_TYPE}" "${M33FW_LAYOUT_SUFFIX}")
    fw_s_layout_file_display=$(echo "${fw_s_layout_file}" | sed "s|^|M33_S_layout |;s|${M33FW_DEPLOYDIR_TFM}|<${!M33FW_DEPLOYDIR_TFM@}>|")
    [ "${fw_s_layout_file}" = "NOTFOUND" ] && file_error=1

    fw_ns_dir_display="$(echo "${M33FW_DEPLOYDIR_CUBE}" | sed 's|'"${M33FW_DEPLOYDIR_ROOT}"'|<M33FW_DEPLOYDIR_ROOT>|')"
    fw_ns_file=$(found_file "${M33FW_DEPLOYDIR_CUBE}" "*-${DT_CONFIG}${SUFFIX_BOOTM33}${SUFFIX_BOOTA35}${M33FW_NS_TYPE}" "${M33FW_SUFFIX}")
    fw_ns_file_display=$(echo "${fw_ns_file}" | sed "s|^|M33_NS_fw |;s|${M33FW_DEPLOYDIR_CUBE}|<${!M33FW_DEPLOYDIR_CUBE@}>|")
    [ "${fw_ns_file}" = "NOTFOUND" ] && file_error=1

    # Init ouput file prefix
    m33fw_output_prefix=$(echo "${fw_ns_file}" | sed 's|^.*/\([^/]*\)|\1|;s|-'"${DT_CONFIG}${SUFFIX_BOOTM33}${SUFFIX_BOOTA35}${M33FW_NS_TYPE}.${M33FW_SUFFIX}"'||')
fi

# dump information about files
echo "------------------------------------"
echo "File found:"
if [ "${DDR_FW}" -eq 1 ]; then
    printf " %-8s\n" "${fwddr_dir_display}"
    printf "%16s : %s\n" ${fwddr_file_display}
    printf "%16s : %s\n" ${fwddr_layout_file_display}
else
    printf " %-8s\n" "${fw_s_dir_display}"
    printf "%16s : %s\n" ${fw_s_file_display}
    printf "%16s : %s\n" ${fw_s_layout_file_display}
    printf " %-8s\n" "${fw_ns_dir_display}"
    printf "%16s : %s\n" ${fw_ns_file_display}
fi
if [ "${file_error}" -gt 0 ]; then
    echo "[ERROR] some files are not present, please provide it or change the parameters"
    echo ""
    exit 100
fi

echo "------------------------------------"
[ -d "${M33FW_DEPLOYDIR_M33FW}" ] || mkdir -p "${M33FW_DEPLOYDIR_M33FW}"

for output_prefix in ${m33fw_output_prefix}; do
    # Init TOOLS_M33FWTOOL options
    m33fwtool_opt=""
    if [ "${DDR_FW}" -eq 1 ]; then
        # Configure m33fwtool_opt for DDR
        m33fwtool_opt="${m33fwtool_opt} --signing ${fwddr_file}"
        m33fwtool_opt="${m33fwtool_opt} --layout ${fwddr_layout_file}"
        # Init output files
        m33fw_output_file="${M33FW_DEPLOYDIR_M33FW}/${output_prefix}-${DT_CONFIG}${SUFFIX_BOOTM33}${DEFAULT_SIGN_SUFFIX}.${M33FW_DDR_SUFFIX}"
        m33fw_output_file_dump="${M33FW_DEPLOYDIR_M33FW}/${output_prefix}-${DT_CONFIG}${SUFFIX_BOOTM33}${DEFAULT_SIGN_SUFFIX}.txt"
    else
        # Configure single FW NS file
        fw_ns_file_single=$(echo "${fw_ns_file}" | grep ${output_prefix}-${DT_CONFIG}${SUFFIX_BOOTM33}${SUFFIX_BOOTA35}${M33FW_NS_TYPE})
        fw_ns_file_single_display=$(echo "${fw_ns_file_single}" | sed "s|^|M33_NS_fw |;s|${M33FW_DEPLOYDIR_CUBE}|<${!M33FW_DEPLOYDIR_CUBE@}>|")
        # Configure m33fwtool_opt for M33 FW
        m33fwtool_opt="${m33fwtool_opt} --input-nsecure ${fw_ns_file_single}"
        m33fwtool_opt="${m33fwtool_opt} --input-secure ${fw_s_file}"
        m33fwtool_opt="${m33fwtool_opt} --layout ${fw_s_layout_file}"
        # Init output files
        m33fw_output_file="${M33FW_DEPLOYDIR_M33FW}/${M33FW_S_PREFIX}-${output_prefix}-${DT_CONFIG}${SUFFIX_BOOTM33}${SUFFIX_BOOTA35}${M33FW_TYPE}${DEFAULT_SIGN_SUFFIX}.${M33FW_SUFFIX}"
        m33fw_output_file_dump="${M33FW_DEPLOYDIR_M33FW}/${M33FW_S_PREFIX}-${output_prefix}-${DT_CONFIG}${SUFFIX_BOOTM33}${SUFFIX_BOOTA35}${M33FW_TYPE}${DEFAULT_SIGN_SUFFIX}.txt"
    fi
    [ ! -e "${m33fw_output_file}" ] || rm "${m33fw_output_file}"
    [ ! -e "${m33fw_output_file_dump}" ] || rm "${m33fw_output_file_dump}"

    # Handle signature configuration
    [ -z "${SIGN_KEY_FILE}" ] || m33fwtool_opt="${m33fwtool_opt} --signature-key ${SIGN_KEY_FILE}"
    [ -z "${SIGN_KEY_PASS}" ] || m33fwtool_opt="${m33fwtool_opt} --signature-pass ${SIGN_KEY_PASS}"

    # dump information about files
    echo "------------------------------------"
    echo "List of files: " > "${m33fw_output_file_dump}"
    if [ "${DDR_FW}" -eq 1 ]; then
        printf " %-8s\n" "${fwddr_dir_display}" >> "${m33fw_output_file_dump}"
        printf "%16s : %s\n" ${fwddr_file_display} >> "${m33fw_output_file_dump}"
        printf "%16s : %s\n" ${fwddr_layout_file_display} >> "${m33fw_output_file_dump}"
    else
        printf " %-8s\n" "${fw_s_dir_display}" >> "${m33fw_output_file_dump}"
        printf "%16s : %s\n" ${fw_s_file_display} >> "${m33fw_output_file_dump}"
        printf "%16s : %s\n" ${fw_s_layout_file_display} >> "${m33fw_output_file_dump}"
        printf " %-8s\n" "${fw_ns_dir_display}" >> "${m33fw_output_file_dump}"
        printf "%16s : %s\n" ${fw_ns_file_single_display} >> "${m33fw_output_file_dump}"
    fi

    echo "M33FW tool command:" | tee -a "${m33fw_output_file_dump}"
    echo "CMD> ${TOOLS_M33FWTOOL} ${m33fwtool_opt} --output ${m33fw_output_file}" | sed "s|--|\\\ \n\t--|g" | tee -a "${m33fw_output_file_dump}"
    sed -i "s|${M33FW_DEPLOYDIR_ROOT}|<${!M33FW_DEPLOYDIR_ROOT@}>|g" "${m33fw_output_file_dump}"

    if [ "${DRY_RUN}" -eq 0 ]; then
        ${TOOLS_M33FWTOOL} ${m33fwtool_opt} --output "${m33fw_output_file}" || die "[$(basename "${TOOLS_M33FWTOOL}")] Failed to generate $(basename "${m33fw_output_file}")"
    fi
done
