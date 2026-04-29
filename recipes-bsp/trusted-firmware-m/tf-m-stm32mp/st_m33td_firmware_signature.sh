#!/usr/bin/env bash
#===============================================================================
#  ORGANIZATION: STMicroelectronics
#     COPYRIGHT: Copyright (C) 2022, STMicroelectronics - All Rights Reserved
#       CREATED: 09/28/2022 11:45
#       License: BSD 3 Claused
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

# set environment variable needed by script
DEFAULT_PUBKEY_FORMAT="${DEFAULT_PUBKEY_FORMAT:-full}"
DEFAULT_ENCKEY_LENGHT="${DEFAULT_ENCKEY_LENGHT:-128}"
DEFAULT_SECURITYCOUNT="${DEFAULT_SECURITYCOUNT:-1}"
DEFAULT_HEADER_SIZE="${DEFAULT_HEADER_SIZE:-0x800}"
DEFAULT_PADDINGOPTS="${DEFAULT_PADDINGOPTS:---align 1 --pad --pad-header}"

SIGN_VERSION=${SIGN_VERSION:-0.1}

TFM_DEV_KIT_DIR="${TFM_DEV_KIT_DIR:-}"
M33FW_ASMB="${M33FW_ASMB:-${TFM_DEV_KIT_DIR}/scripts/assemble.py}"
M33FW_SIGN="${M33FW_SIGN:-${TFM_DEV_KIT_DIR}/scripts/wrapper/wrapper.py}"
TFM_KEYS_FRST="${TFM_KEYS_FRST:-$TFM_DEV_KIT_DIR/keys/root-ec-p256.pem}"
TFM_KEYS_SCND="${TFM_KEYS_SCND:-$TFM_DEV_KIT_DIR/keys/root-ec-p256_1.pem}"

# Variable
DEFAULT_KEYS="${TFM_KEYS_FRST}"
INPUT_SIGNING=""
INPUT_NSECURE=""
INPUT_SECURE=""
LAYOUT_FILE=""
OUTPUT_FILE=""
SIGNATURE_KEY=""
SIGNATURE_KEY_PASS=""
SIGNING=1


function usage() {
    echo ""
    echo "Help:"
    echo "   $0 [options] [-h|--help]"
    echo ""
    echo "This script generate a firmware ready for M33TD usage"
    echo ""
    echo "Parameters:"
    echo "    -h|--help: this help"
    echo "    -l|--layout <file>            : Layout file needed to assemble and sign"
    echo "    -i|--input-nsecure <file>     : NonSecure file for firmware assembly"
    echo "    -I|--input-secure <file>      : Secure for firmware assembly"
    echo "    -s|--signing <file>           : File to sign"
    echo "  NB: '--input-nsecure' and '--input-secure' cannot be used with '--signing'"
    echo "    -k|--signature-key <key file> : Key use to sign firmware"
    echo "    -p|--signature-pass <pass>    : Key pass use to sign"
    echo "    -n|--no-signing               : disable signing mechanism"
    echo "    -o|--output <output filepath> : Output filename and path"
    echo ""
    echo "Optional configs:"
    echo "    --signing-version <version>   : Specific signing version to use (default: ${SIGN_VERSION})"
    echo "    --public-key-format <format>  : Format of the public key embedded (default: ${DEFAULT_PUBKEY_FORMAT})"
    echo "    --header-size <hex size>      : Specify the header size (default: ${DEFAULT_HEADER_SIZE})"
    echo "    --security-counter <value>    : Specify the value of security counter (default: ${DEFAULT_SECURITYCOUNT})"
    echo "    --encrypt-lenght <size>       : Specify the value of encrypt key length (default: ${DEFAULT_ENCKEY_LENGHT})"
    echo "    --padding-opts <OPTS>         : Options for alignment and padding (default: ${DEFAULT_PADDINGOPTS})"
    echo ""
    echo "Example:"
    echo "  $0 --layout tfm_s.signing_layout \
               --input-nsecure starterapp_CM33_NonSecure.bin \
               --input-secure tfm-_s.bin \
               --signature-key privateKey.pem \
               --signature-pass root \
               --output tfm-starterapp_s_ns_signed.bin"
    echo "  -> generate tfm-starterapp_s_ns_signed.bin"
    echo "  $0 --layout ddr_phy.signing_layout \
               --signing ddr_phy.bin \
               --signature-key privateKey.pem \
               --signature-pass root \
               --output ddr_phy_signed.bin"
    echo "  -> generate ddr_phy_signed.bin"
}

function verify_parameters() {
    # Verify output
    if [ "X${OUTPUT_FILE}" = "X" ]; then
        echo "[ERROR]: need to specify output file"
        usage
        exit 1
    else
        if [ -e "${OUTPUT_FILE}" ]; then
            echo "[ERROR]: the output file exist, please change the name of output file or erase it"
            echo ""
            exit 2
        fi
    fi
    # Verify layout
    if [ "X${LAYOUT_FILE}" = "X" ]; then
        echo "[ERROR]: need to specify layout file"
        usage
        exit 3
    else
        if ! [ -e "${LAYOUT_FILE}" ]; then
            echo "[ERROR]: the layout file is not present"
            echo ""
            exit 4
        fi
    fi
    # Verify signing version
    if [ "X${SIGN_VERSION}" = "X" ]; then
        echo "[ERROR]: need to specify signing version"
        usage
        exit 5
    fi
    # Verify compatibible option
    if ( [ -n "${INPUT_NSECURE}" ] || [ -n "${INPUT_SECURE}" ] ) && [ -n "${INPUT_SIGNING}" ]; then
        echo "[ERROR]: ASSEMBLE and NSECURE/SECURE binaries can not be specified together"
        echo ""
        exit 6
    fi
}
function verify_nsecure_parameters() {
    if [ "X${INPUT_NSECURE}" = "X" ]; then
        echo "[ERROR]: need to specify non-secure input assembly"
        usage
        exit 11
    else
        if ! [ -e "${INPUT_NSECURE}" ]; then
            echo "[ERROR]: the non-secure input file to assemble is not present"
            echo ""
            exit 12
        fi
    fi
}
function verify_secure_parameters() {
    if [ "X${INPUT_SECURE}" = "X" ]; then
        echo "[ERROR]: need to specify secure input for assembly"
        usage
        exit 21
    else
        if ! [ -e "${INPUT_SECURE}" ]; then
            echo "[ERROR]: the secure input file to assemble is not present"
            echo ""
            exit 22
        fi
    fi
}
function verify_tfm_sdk_presence() {
    if [ "X$TFM_DEV_KIT_DIR" = "X" ]; then
        echo "[ERROR]: NEED to have environment variable for TFM_DEV_KIT_DIR"
        echo "  TFM_DEV_KIT_DIR=<path to tf-m sdk>"
        echo ""
        exit 31
    fi
    # Verify assemble script
    if [ ! -e "${M33FW_ASMB}" ]; then
        echo "[ERROR]: The TF-M script $(basename "${M33FW_ASMB}") is not present"
        echo ""
        exit 32
    fi
}
function verify_signing() {
    # Verify signing script
    if [ ! -e "${M33FW_SIGN}" ]; then
        echo "[ERROR]: The TF-M script $(basename "${M33FW_SIGN}") is not present"
        echo ""
        exit 41
    fi
    if [ -z "${SIGNATURE_KEY}" ]; then
        echo "[INFO]: the key \"SIGNATURE_KEY\" is empty."
        echo "[INFO]: using default key as signature key ($(basename "${DEFAULT_KEYS}"))."
        SIGNATURE_KEY=${DEFAULT_KEYS}
    fi
    # Verify if key exist
    if [ ! -e "${SIGNATURE_KEY}" ]; then
        echo "[ERROR]: the \"$SIGNATURE_KEY\" doesn't exist."
        echo "[ERROR]: please specify a valid signature key."
        usage
        exit 42
    fi
}

function process_args() {
    # check opt args
    while test $# != 0
    do
        case "$1" in
        -h|--help)
            usage
            return 0
            ;;
        -i|--input-nsecure)
            if [ $# -gt 1 ]; then
                INPUT_NSECURE=$2
                shift
            fi
            ;;
        -I|--input-secure)
            if [ $# -gt 1 ]; then
                INPUT_SECURE=$2
                shift
            fi
            ;;
        -l|--layout)
            if [ $# -gt 1 ]; then
                LAYOUT_FILE=$2
                shift
            fi
            ;;
        -o|--output)
            if [ $# -gt 1 ]; then
                OUTPUT_FILE=$2
                shift
            fi
            ;;
        -s|--signing)
            if [ $# -gt 1 ]; then
                INPUT_SIGNING=$2
                shift
            fi
            ;;
        -k|--signature-key)
            if [ $# -gt 1 ]; then
                SIGNATURE_KEY=$2
                shift
            fi
            ;;
        -p|--signature-pass)
            if [ $# -gt 1 ]; then
                SIGNATURE_KEY_PASS=$2
                shift
            fi
            ;;
        -n|--no-signing)
            SIGNING=0
            ;;
        --signing-version)
            if [ $# -gt 1 ]; then
                SIGN_VERSION=$2
                shift
            fi
            ;;
        --public-key-format)
            if [ $# -gt 1 ]; then
                DEFAULT_PUBKEY_FORMAT=$2
                shift
            fi
            ;;
        --header-size)
            if [ $# -gt 1 ]; then
                DEFAULT_HEADER_SIZE=$2
                shift
            fi
            ;;
        --security-counter)
            if [ $# -gt 1 ]; then
                DEFAULT_SECURITYCOUNT=$2
                shift
            fi
            ;;
        --encrypt-lenght)
            if [ $# -gt 1 ]; then
                DEFAULT_ENCKEY_LENGHT=$2
                shift
            fi
            ;;
        --padding-opts)
            if [ $# -gt 1 ]; then
                DEFAULT_PADDINGOPTS=$2
                shift
            fi
            ;;
        -*)
            echo "Wrong parameter: $1"
            usage
            return 1
            ;;
        esac
        shift
    done
}

# ==============================================
#                    MAIN
# ==============================================

# Display scirpt command launched
echo "[CMD] $0 $@"

process_args $@
verify_parameters
verify_tfm_sdk_presence

if [ -z "${INPUT_SIGNING}" ]; then
    verify_nsecure_parameters
    verify_secure_parameters
    # Init assemble filename
    filename=$(mktemp)
    echo "[M33FW ASSEMBLE CMD] ${M33FW_ASMB} \\
        --layout ${LAYOUT_FILE} \\
        --secure ${INPUT_SECURE} \\
        --non_secure {INPUT_NSECURE} \\
        --output ${filename}"
    ${M33FW_ASMB} \
        --layout "${LAYOUT_FILE}" \
        --secure "${INPUT_SECURE}" \
        --non_secure "${INPUT_NSECURE}" \
        --output "${filename}" || { EXIT_STATUS=$? ; echo "[ERROR]: failed to generate ${filename}" ; }
else
    filename="${INPUT_SIGNING}"
    # Make sure to sign firmware
    SIGNING=1
    # Default use of secondary key
    DEFAULT_KEYS="${TFM_KEYS_SCND}"
fi

if [ "${SIGNING}" -eq 1 ]; then
    verify_signing
    # Manage SIGNATURE_KEY_PASS
    signature_key_pass=""
    [ -z "${SIGNATURE_KEY_PASS}" ] || signature_key_pass="--key-pswd ${SIGNATURE_KEY_PASS}"

    echo "[M33FW SIGNING CMD] ${M33FW_SIGN} \\
        --version "${SIGN_VERSION}" \\
        --layout "${LAYOUT_FILE}" \\
        --key ${SIGNATURE_KEY} \\
        ${signature_key_pass} \\
        --public-key-format ${DEFAULT_PUBKEY_FORMAT} \\
        -H ${DEFAULT_HEADER_SIZE} \\
        -s ${DEFAULT_SECURITYCOUNT} \\
        -L ${DEFAULT_ENCKEY_LENGHT} \\
        ${DEFAULT_PADDINGOPTS} \\
        --measured-boot-record ${filename} \\
        ${OUTPUT_FILE}"
    ${M33FW_SIGN} \
        --version "${SIGN_VERSION}" \
        --layout "${LAYOUT_FILE}" \
        --key ${SIGNATURE_KEY} \
        ${signature_key_pass} \
        --public-key-format "${DEFAULT_PUBKEY_FORMAT}" \
        -H "${DEFAULT_HEADER_SIZE}" \
        -s "${DEFAULT_SECURITYCOUNT}" \
        -L "${DEFAULT_ENCKEY_LENGHT}" \
        ${DEFAULT_PADDINGOPTS} \
        --measured-boot-record "${filename}" \
        "${OUTPUT_FILE}" || { EXIT_STATUS=$? ; echo "[ERROR]: failed to generate ${OUTPUT_FILE}" ; }
else
    echo "[INFO]: No 'signed' firwmare generated as signing is disabled."
    # No signing configure: move temporary file to OUTPUT_FILE
    mv "${filename}" "${OUTPUT_FILE}"
fi

exit ${EXIT_STATUS:-0}
