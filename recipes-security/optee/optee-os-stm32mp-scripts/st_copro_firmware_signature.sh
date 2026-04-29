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
DEFAULT_SBOOTADDR=${SBOOTADDR:-0x80000000}
DEFAULT_NSBOOTADDR=${NSBOOTADDR:-0x80100000}
DEFAULT_SGN_RADIX=${SGN_RADIX:-_sign}
DEFAULT_ENC_RADIX=${ENC_RADIX:-_enc}
DEFAULT_ECC_RADIX=${ECC_RADIX:-_ecc}
DEFAULT_RSA_RADIX=${RSA_RADIX:-_rsa}
DEFAULT_SEC_RADIX=${SEC_RADIX:-_tfm}
DEFAULT_EXT_RADIX=${EXT_RADIX:-.bin}

TA_DEV_KIT_DIR="${TA_DEV_KIT_DIR:-}"
SIGN_RPROC="${SIGN_RPROC:-$TA_DEV_KIT_DIR/scripts/sign_rproc_fw.py}"
OPTEE_KEYS="${OPTEE_KEYS:-$TA_DEV_KIT_DIR/keys/default.pem}"

# Variable
INPUT_NSECURE=""
OUTPUT_NSECURE=""
OUTPUT_NSECURE_STRIPPED=0
OUTPUT_SIGNATURE=""
INPUT_SECURE=""
OUTPUT_SECURE=""
OUTPUT_SECURE_STRIPPED=0
ENCRYPT_KEY=""
SIGN_ECC=0
SIGN_RSA=0
SIGN_XXX_PASS_KEY=""
SIGNATURE_KEY=""
SIGN_XXX_INFO_KEY=""

SIGNATURE_KEY=""
OUTPUT_FILE=""

function usage() {
    echo ""
    echo "Help:"
    echo "   $0 [options] [-h|--help]"
    echo ""
    echo "This script generate a firmware ready to load on CoPro"
    echo ""
    echo "Environment variable used: "
    echo "    SBOOTADDR : Secure load (default: $DEFAULT_SBOOTADDR)"
    echo "    NSBOOTADDR: Non Secure load (default: $DEFAULT_NSBOOTADDR)"
    echo "    SEC_RADIX : Radix to add to output file name (default: $DEFAULT_SEC_RADIX)"
    echo "    SGN_RADIX : Radix to add to output file name (default: $DEFAULT_SGN_RADIX)"
    echo "    ECC_RADIX : Radix to add to output file name (default: $DEFAULT_ECC_RADIX)"
    echo "    RSA_RADIX : Radix to add to output file name (default: $DEFAULT_RSA_RADIX)"
    echo "    ENC_RADIX : Radix to add to output file name (default: $DEFAULT_ENC_RADIX)"
    echo "    EXT_RADIX : Radix to add to output file name (default: $DEFAULT_EXT_RADIX)"
    echo ""
    echo "Parameters:"
    echo "    -h|--help: this help"
    echo "    -i|--input-nsecure <elf file>   : File to load on non secure address"
    echo "    -I|--input-secure <elf file>    : File to load on secure address"
    echo "    -k|--signature-key <key file>   : Key use to sign firmware"
    echo "    -E|--sign-ecc                   : Sign with ecc key"
    echo "    -R|--sign-rsa                   : Sign with rsa key"
    echo "    -P|--sign-info-key <info key>   : Key info use to sign (ECC and RSA only)"
    echo "    -p|--sign-pass <pass>           : Key pass use to sign (ECC only)"
    echo "    -e|--encrypt-key <key file      : Key use to encrypt"
    echo "    -o|--output <output file prefix>: Output file prefix (path and prefix)"
    echo "    -S|--output-signature <sign suffix>: Append suffix to output file prefix (<output_file_prefix><sign_suffix>)"
    echo ""
    echo "NOTE: Default output filename is built with format:"
    echo "  <output_file_prefix>[<SEC_RADIX>]<SIGN_RADIX>[<ECC_RADIX>|<RSA_RADIX>][<ENC_RADIX>]<EXT_RADIX>"
    echo "  or"
    echo "  <output_file_prefix><sign_suffix>"
    echo ""
    echo "Example:"
    echo "  $0 -i OpenAMP_TTY_echo_CM33_NonSecure.elf -o OpenAMP_TTY_echo_CM33"
    echo "      -> generate OpenAMP_TTY_echo_CM33_${DEFAULT_SGN_RADIX}${DEFAULT_EXTENSION}"
    echo "  $0 --input-nsecure OpenAMP_TTY_echo_CM33_NonSecure.elf --input-secure tfm_s_ipcc.elf -o OpenAMP_TTY_echo_CM33"
    echo "      -> generate OpenAMP_TTY_echo_CM33_${DEFAULT_SEC_RADIX}${DEFAULT_SGN_RADIX}${DEFAULT_EXTENSION}"
}

function verify_parameters() {
    #verify output
    if [ "X$OUTPUT_FILE" = "X" ]; then
        echo "[ERROR]: need to specify output file"
        usage
        exit 1
    else
        if [ -e $OUTPUT_FILE ]; then
            echo "[ERROR]: the output file exist, please change the name of output file or erase it"
            echo ""
            exit 2
        fi
    fi
}
function verify_nsecure_parameters() {
    if [ "X$INPUT_NSECURE" = "X" ]; then
        echo "[ERROR]: need to specify input for signature"
        usage
        exit 11
    else
        if [ -e $INPUT_NSECURE ]; then
            # verify elf is stripped
            if echo "$INPUT_NSECURE" | grep -q "stripped"; then
                OUTPUT_NSECURE=$INPUT_NSECURE
            else
                echo "NSECURE [$INPUT_NSECURE] is not stripped"
                OUTPUT_NSECURE="/tmp/$(basename "$INPUT_NSECURE" .elf).stripped.elf"
                $OBJCOPY -S $INPUT_NSECURE $OUTPUT_NSECURE
                echo "NSECURE [$INPUT_NSECURE] stripped [$OUTPUT_NSECURE]"
                OUTPUT_NSECURE_STRIPPED=1
            fi
        else
            echo "[ERROR]: the input file to sign is not present"
            echo ""
            exit 12
        fi
    fi
}
function verify_secure_parameters() {
    if [ "X$INPUT_SECURE" = "X" ]; then
        echo "[ERROR]: need to specify input for signature"
        usage
        exit 21
    else
        if [ -e $INPUT_SECURE ]; then
            # verify elf is stripped
            if echo "$INPUT_SECURE" | grep -q "stripped"; then
                OUTPUT_SECURE=$INPUT_SECURE
            else
                echo "SECURE [$INPUT_SECURE] is not stripped"
                OUTPUT_SECURE="/tmp/$(basename "$INPUT_SECURE" .elf).stripped.elf"
                $OBJCOPY -S $INPUT_SECURE $OUTPUT_SECURE
                echo "SECURE [$INPUT_SECURE] stripped [$OUTPUT_SECURE]"
                OUTPUT_SECURE_STRIPPED=1
            fi
        else
            echo "[ERROR]: the input file to load on secure address is not present"
            echo ""
            exit 22
        fi
    fi
}
function verify_optee_presence() {
    if [ "X$TA_DEV_KIT_DIR" = "X" ]; then
        echo "[ERROR]: NEED to have environment variable for OPTEE: TA_DEV_KIT_DIR"
        echo "  TA_DEV_KIT_DIR=<path to optee sdk>/export-user_ta[_arm32|_arm64]"
        echo ""
        exit 31
    fi
    if [ ! -e "${SIGN_RPROC}" ]; then
        echo "[ERROR]: The Optee script $(basename "${SIGN_RPROC}") is not present"
        echo ""
        exit 32
    fi
}
function verify_encrypt_key() {
    if [ -z "$ENCRYPT_KEY" ] ; then
        echo "[ERROR]: the key \"ENCRYPT_KEY\" is empty."
        echo "[ERROR]: please specify a valid encryption key."
        usage
        exit 41
    fi
    # verify if key exist
    if [ ! -e $ENCRYPT_KEY ]; then
        echo "[ERROR]: the key \"$ENCRYPT_KEY\" doesn't exist."
        echo "[ERROR]: please specify a valid encryption key."
        usage
        exit 42
    fi
}
function verify_xxx_key() {
    if [ "${SIGN_ECC}" = "0" ] && [ "${SIGN_RSA}" = "0" ]; then
        if [ -z "$SIGNATURE_KEY" ] ; then
            echo "[INFO]: the key \"SIGNATURE_KEY\" is empty."
            echo "[INFO]: using default optee key as signature key ($(basename "${OPTEE_KEYS}"))."
            SIGNATURE_KEY=${OPTEE_KEYS}
        fi
    fi
    # Default signature setting
    if [ -z "$SIGNATURE_KEY" ] ; then
        echo "[ERROR]: the key \"SIGNATURE_KEY\" is empty."
        echo "[ERROR]: please specify a valid signature key."
        usage
        exit 51
    fi
    # verify if key exist
    if [ ! -e $SIGNATURE_KEY ]; then
        echo "[ERROR]: the \"$SIGNATURE_KEY\" doesn't exist."
        echo "[ERROR]: please specify a valid signature key."
        usage
        exit 52
    fi
    # Extra signature info
    if [ "${SIGN_ECC}" = "1" ] || [ "${SIGN_RSA}" = "1" ]; then
        if [ -z "$SIGN_XXX_INFO_KEY" ] ; then
            echo "[ERROR]: the key \"SIGN_XXX_INFO_KEY\" is empty."
            echo "[ERROR]: please specify a valid key information."
            usage
            exit 53
        fi
        # verify if key exist
        if [ ! -e $SIGN_XXX_INFO_KEY ]; then
            echo "[ERROR]: the \"$SIGN_XXX_INFO_KEY\" doesn't exist."
            echo "[ERROR]: please specify a valid key information."
            usage
            exit 54
        fi
    fi
    # Signature password
    if [ "${SIGN_ECC}" = "1" ] ; then
        if [ -z "$SIGN_XXX_PASS_KEY" ] ; then
            echo "[ERROR]: the password \"SIGN_XXX_PASS_KEY\" is empty."
            echo "[ERROR]: please specify a valid password."
            usage
            exit 55
        fi
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
        -e|--encrypt-key)
            if [ $# -gt 1 ]; then
                ENCRYPT_KEY=$2
                shift
            fi
            ;;
        -E|--sign-ecc)
            SIGN_ECC=1
            ;;
        -R|--sign-rsa)
            SIGN_RSA=1
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
        -o|--output)
            if [ $# -gt 1 ]; then
                OUTPUT_FILE=$2
                shift
            fi
            ;;
        -S|--ouput-signature)
            if [ $# -gt 1 ]; then
                OUTPUT_SIGNATURE=$2
                shift
            fi
            ;;
        -k|--signature-key)
            if [ $# -gt 1 ]; then
                SIGNATURE_KEY=$2
                shift
            fi
            ;;
        -p|--sign-pass)
            if [ $# -gt 1 ]; then
                SIGN_XXX_PASS_KEY=$2
                shift
            fi
            ;;
        -P|--sign-info-key)
            if [ $# -gt 1 ]; then
                SIGN_XXX_INFO_KEY=$2
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
verify_nsecure_parameters
verify_optee_presence
verify_xxx_key

# ----------------------------
# choose what we can generate

# UC 1: signature (and encryption) / Non Secure firmware only (no Secure firmware)
# scripts/sign_rproc_fw.py  --in OpenAMP_TTY_echo_CM33_NonSecure.elf -out sign.bin --key keys/default.pem --plat-tlv BOOTADDR 0x80100000
# if using ECC key:
# scripts/sign_rproc_fw.py  --in OpenAMP_TTY_echo_CM33_NonSecure.elf -out ecc.stm32 --key ./keys/privateKey.pem --key_pwd="azerty" --key_type=ECC --key_info ./keys/publicKey.der --plat-tlv BOOTADDR 0x80100000

# UC 2: signature (and encryption) / Secure and Non Secure firmwares
# scripts/sign_rproc_fw.py  --in OpenAMP_TTY_echo_CM33_NonSecure.elf --in tfm_s_ipcc.elf --out tfm_sign.bin --key keys/default.pem --plat-tlv BOOTADDR 0x80000000  --plat-tlv BOOTSEC 0x01


# signature command
if [ "${SIGN_ECC}" = "1" ]; then
    CMD_SIGN="--key ${SIGNATURE_KEY} --key_info ${SIGN_XXX_INFO_KEY} --key_pwd=${SIGN_XXX_PASS_KEY} --key_type=ECC"
    SIGN_RADIX="${DEFAULT_SGN_RADIX}${DEFAULT_ECC_RADIX}"
elif [ "${SIGN_RSA}" = "1" ]; then
    CMD_SIGN="--key ${SIGNATURE_KEY} --key_info ${SIGN_XXX_INFO_KEY}"
    SIGN_RADIX="${DEFAULT_SGN_RADIX}${DEFAULT_RSA_RADIX}"
else
    CMD_SIGN="--key ${SIGNATURE_KEY}"
    SIGN_RADIX="${DEFAULT_SGN_RADIX}"
fi
# encryption command
if [ -z "${ENCRYPT_KEY}" ] ; then
    CMD_ENC=""
    ENC_RADIX=""
else
    verify_encrypt_key
    CMD_ENC="--enc_key ${ENCRYPT_KEY}"
    ENC_RADIX="${DEFAULT_ENC_RADIX}"
fi

if [ -z "${INPUT_SECURE}" ]; then
    # UC1 signature (and encryption) / Non Secure firmware only (no Secure firmware)
    filename="${OUTPUT_FILE}${SIGN_RADIX}${ENC_RADIX}${DEFAULT_EXT_RADIX}"
    [ -z "${OUTPUT_SIGNATURE}" ] || filename="${OUTPUT_FILE}${OUTPUT_SIGNATURE}"
    echo "[COPRO CMD] ${SIGN_RPROC} \\
            ${CMD_SIGN} \\
            ${CMD_ENC} \\
            --in $OUTPUT_NSECURE \\
            --plat-tlv BOOTADDR $DEFAULT_NSBOOTADDR \\
            --out $filename"
    ${SIGN_RPROC} \
            ${CMD_SIGN} \
            ${CMD_ENC} \
            --in $OUTPUT_NSECURE \
            --plat-tlv BOOTADDR $DEFAULT_NSBOOTADDR \
            --out $filename || { EXIT_STATUS=$? ; echo "[ERROR]: failed to generate $filename" ; }
else
    verify_secure_parameters
    # UC2 signature (and encryption) / Secure and Non Secure firmwares
    filename="${OUTPUT_FILE}${DEFAULT_SEC_RADIX}${SIGN_RADIX}${ENC_RADIX}${DEFAULT_EXT_RADIX}"
    [ -z "${OUTPUT_SIGNATURE}" ] || filename="${OUTPUT_FILE}${OUTPUT_SIGNATURE}"
    echo "[COPRO SECURE CMD] ${SIGN_RPROC} \\
            ${CMD_SIGN} \\
            ${CMD_ENC} \\
            --in $OUTPUT_NSECURE \\
            --in $OUTPUT_SECURE \\
            --plat-tlv BOOTADDR $DEFAULT_SBOOTADDR \\
            --plat-tlv BOOTSEC 0x01 \\
            --out $filename"
    ${SIGN_RPROC} \
            ${CMD_SIGN} \
            ${CMD_ENC} \
            --in $OUTPUT_NSECURE \
            --in $OUTPUT_SECURE  \
            --plat-tlv BOOTADDR $DEFAULT_SBOOTADDR \
            --plat-tlv BOOTSEC 0x01 \
            --out $filename || { EXIT_STATUS=$? ; echo "[ERROR]: failed to generate $filename" ; }
fi

#--------------------------------
# clean temporary file
[ "$OUTPUT_NSECURE_STRIPPED" -ne 1 ] || rm -f "$OUTPUT_NSECURE"
[ "$OUTPUT_SECURE_STRIPPED" -ne 1 ]  || rm -f "$OUTPUT_SECURE"

exit ${EXIT_STATUS:-0}
