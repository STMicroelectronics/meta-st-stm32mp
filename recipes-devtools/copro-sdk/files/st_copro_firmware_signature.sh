#!/bin/bash -
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
DEFAULT_UC1_RADIX=${UC1_RADIX:-sign.bin}
DEFAULT_UC2_RADIX=${UC2_RADIX:-tfm_sign.bin}
DEFAULT_UC3_RADIX=${UC1_RADIX:-sign_enc.bin}

# Variable
NEED_TO_SIGN=0
NEED_TO_SECURE=0
INPUT_NSECURE=""
OUTPUT_NSECURE=""
OUTPUT_NSECURE_STRIPPED=0
OUTPUT_SIGNATURE=""
INPUT_SECURE=""
OUTPUT_SECURE=""
OUTPUT_SECURE_STRIPPED=0
ENCRYPT_KEY=""
SIGN_ECC=0
SIGN_ECC_PASS_KEY=""
SIGN_ECC_PRIV_KEY=""
SIGN_ECC_INFO_KEY=""

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
    echo "    SBOOTADDR:  Secure load (default: $DEFAULT_SBOOTADDR)"
    echo "    NSBOOTADDR: Non Secure load (default: $DEFAULT_NSBOOTADDR)"
    echo "    UC1_RADIX:  Radix to add to output file name (default: $DEFAULT_UC1_RADIX)"
    echo "    UC2_RADIX:  Radix to add to output file name (default: $DEFAULT_UC2_RADIX)"
    echo "    UC3_RADIX:  Radix to add to output file name (default: $DEFAULT_UC3_RADIX)"
    echo "Parameters:"
    echo "    -h | --help: this help"
    echo "    -i <elf file>| --input-nsecure <elf file>: File to load on non secure address"
    echo "    -I <elf file>| --input-secure <elf file>: File to load on secure address"
    echo "    -k <key file>| --signature-key <key file>: Key use to sign firmware"
    echo "    -o <output file prefix>| --output <output file prefix>: output file prefix (path + prefix)"
    echo "    -e <key file>| --encrypt-key <key file: key use to encrypt"
    echo "    -E -p <ecc priv key> -P <ecc info key> [-s <eec passwd>]| "
    echo "        --sign-ecc --sign-ecc-priv-key <ecc private key> --sign-ecc-info-key <ecc info key> [--sign-ecc-pass <ecc pass>]:"
    echo "        sign with ecc key"

    echo "Example:"
    echo "  $0 -i OpenAMP_TTY_echo_CM33_NonSecure.elf -o OpenAMP_TTY_echo_CM33"
    echo "      -> generate OpenAMP_TTY_echo_CM33-$DEFAULT_UC1_RADIX"
    echo "  $0 --input-nsecure OpenAMP_TTY_echo_CM33_NonSecure.elf --input-secure tfm_s_ipcc.elf -o OpenAMP_TTY_echo_CM33"
    echo "      -> generate OpenAMP_TTY_echo_CM33-$DEFAULT_UC1_RADIX"
    echo "      -> generate OpenAMP_TTY_echo_CM33-$DEFAULT_UC2_RADIX"
    exit 1
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
        exit 3
    else
        if [ ! -e $INPUT_NSECURE ]; then
            echo "[ERROR]: the input file to sign is not present"
            echo ""
            exit 4
        fi
    fi
}
function verify_secure_parameters() {
    if [ "X$INPUT_SECURE" = "X" ]; then
        echo "[ERROR]: need to specify input for signature"
        usage
        exit 3
    else
        if [ ! -e $INPUT_SECURE ]; then
            echo "[ERROR]: the input file to load on secure address is not present"
            echo ""
            exit 4
        fi
    fi
}
function verify_optee_presence() {
    if [ "X$TA_DEV_KIT_DIR" = "X" ]; then
        echo "[ERROR]: NEED to have environment variable for OPTEE: TA_DEV_KIT_DIR"
        echo "  TA_DEV_KIT_DIR=<path to optee sdk>/export-user_ta[_arm32|_arm64]"
        echo ""
        exit 5
    fi
    if [ ! -e "$TA_DEV_KIT_DIR/scripts/sign_rproc_fw.py" ]; then
        echo "[ERROR]: The Optee script sign_rproc_fw.py are not present"
        echo ""
        exit 7

    fi
}

function verify_optee_key() {
    if [ -z "$SIGNATURE_KEY" ] ; then
        SIGNATURE_KEY=$TA_DEV_KIT_DIR/keys/default.pem
    fi
    # verify if key exist
    if [ ! -e $SIGNATURE_KEY ]; then
        echo "[ERROR]: the key \"$SIGNATURE_KEY\" doesn't exist."
        echo "[ERROR]: please specify a valid signature key."
        usage
        exit 6
    fi
}
function verify_encrypt_key() {
    if [ -z "$ENCRYPT_KEY" ] ; then
        echo "[ERROR]: the key \"ENCRYPT_KEY\" is empty."
        echo "[ERROR]: please specify a valid encryption key."
        usage
        exit 7
    fi
    # verify if key exist
    if [ ! -e $ENCRYPT_KEY ]; then
        echo "[ERROR]: the key \"$ENCRYPT_KEY\" doesn't exist."
        echo "[ERROR]: please specify a valid encryption key."
        usage
        exit 8
    fi
}
function verify_ecc_key() {
    if [ -z "$SIGN_ECC_PRIV_KEY" ] ; then
        echo "[ERROR]: the ECC key \"SIGN_ECC_PRIV_KEY\" is empty."
        echo "[ERROR]: please specify a valid ecc signature key."
        usage
        exit 9
    fi
    # verify if key exist
    if [ ! -e $SIGN_ECC_PRIV_KEY ]; then
        echo "[ERROR]: the key \"$SIGN_ECC_PRIV_KEY\" doesn't exist."
        echo "[ERROR]: please specify a valid ecc signature key."
        usage
        exit 10
    fi
    if [ -z "$SIGN_ECC_INFO_KEY" ] ; then
        echo "[ERROR]: the ECC key \"SIGN_ECC_INFO_KEY\" is empty."
        echo "[ERROR]: please specify a valid ecc key information."
        usage
        exit 11
    fi
    # verify if key exist
    if [ ! -e $SIGN_ECC_INFO_KEY ]; then
        echo "[ERROR]: the key \"$SIGN_ECC_INFO_KEY\" doesn't exist."
        echo "[ERROR]: please specify a valid ecc key information."
        usage
        exit 12
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
        -k|--signature-key)
            if [ $# -gt 1 ]; then
                SIGNATURE_KEY=$2
                shift
            fi
            ;;
        -s|--sign-ecc-pass)
            if [ $# -gt 1 ]; then
                SIGN_ECC_PASS_KEY=$2
                shift
            fi
            ;;
        -p|--sign-ecc-priv-key)
            if [ $# -gt 1 ]; then
                SIGN_ECC_PRIV_KEY=$2
                shift
            fi
            ;;
        -P|--sign-ecc-info-key)
            if [ $# -gt 1 ]; then
                SIGN_ECC_INFO_KEY=$2
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
process_args $@
verify_parameters
verify_optee_presence
verify_optee_key

# Strip elf file
if [ -n "$INPUT_NSECURE" ]; then
    if ! $(echo "$INPUT_NSECURE" | grep -q "stripped") ; then
        echo "NSECURE [$INPUT_NSECURE] is not stripped"
        # need to strip file
        temp_file_name=$(basename $INPUT_NSECURE | sed "s/elf/stripped.elf/")
        OUTPUT_NSECURE=$(echo "/tmp/$temp_file_name")
        $OBJCOPY -S $INPUT_NSECURE $OUTPUT_NSECURE
        echo "NSECURE [$INPUT_NSECURE] stripped [OUTPUT_NSECURE]"
        OUTPUT_NSECURE_STRIPPED=1
    else
        OUTPUT_NSECURE=$INPUT_NSECURE
    fi
fi
if [ -n "$INPUT_SECURE" ]; then
    if ! $(echo "$INPUT_SECURE" | grep -q "stripped") ; then
        echo "SECURE [$INPUT_SECURE] is not stripped"
        # need to strip file
        temp_file_name=$(basename $INPUT_SECURE | sed "s/elf/stripped.elf/")
        OUTPUT_SECURE=$(echo "/tmp/$temp_file_name")
        $OBJCOPY -S $INPUT_SECURE $OUTPUT_SECURE
        echo "SECURE [$INPUT_SECURE] stripped [$OUTPUT_SECURE]"
        OUTPUT_SECURE_STRIPPED=1
    else
        OUTPUT_SECURE=$INPUT_SECURE
    fi
fi

# ----------------------------
# choose what we can generate

# UC 1: signature / non secure (and no Secure firmware)
# scripts/sign_rproc_fw.py  --in OpenAMP_TTY_echo_CM33_NonSecure.elf -out sign.bin --key keys/default.pem --plat-tlv BOOTADDR 0x80100000
# if using ECC key:
# scripts/sign_rproc_fw.py  --in OpenAMP_TTY_echo_CM33_NonSecure.elf -out ecc.stm32 --key ./keys/privateKey.pem --key_pwd="azerty" --key_type=ECC --key_info ./keys/publicKey.der --plat-tlv BOOTADDR 0x80100000


# UC 2: signature / secure and non secure firmware
# scripts/sign_rproc_fw.py  --in OpenAMP_TTY_echo_CM33_NonSecure.elf --in tfm_s_ipcc.elf --out tfm_sign.bin --key keys/default.pem --plat-tlv BOOTADDR 0x80000000  --plat-tlv BOOTSEC 0x01

# UC 1: signature / non secure (and no Secure firmware)
if [ -n "$INPUT_NSECURE"  -a -z "$INPUT_SECURE"]; then
    # signature command
    if [ "$SIGN_ECC" = "1" ]; then
        verify_ecc_key
        CMD_SIGN="--key $SIGN_ECC_PRIV_KEY --key_pwd=${SIGN_ECC_PASS_KEY} --key_type=ECC --key_info $SIGN_ECC_INFO_KEY"
    else
        CMD_SIGN="--key $SIGNATURE_KEY"
    fi

    if [ -z "$ENCRYPT_KEY" ] ; then
        # signature / non secure (and no Secure firmware)
        filename=$(echo $OUTPUT_FILE"_"$DEFAULT_UC1_RADIX)
        echo "[COPRO CMD] $TA_DEV_KIT_DIR/scripts/sign_rproc_fw.py $CMD_SIGN \\
                --in $OUTPUT_NSECURE \\
                --plat-tlv BOOTADDR $DEFAULT_NSBOOTADDR \\
                --out $filename"
        $TA_DEV_KIT_DIR/scripts/sign_rproc_fw.py $CMD_SIGN \
                --in $OUTPUT_NSECURE \
                --plat-tlv BOOTADDR $DEFAULT_NSBOOTADDR \
                --out $filename
    else
        verify_encrypt_key
        # signature encryption / non secure (and no Secure firmware)
        filename=$(echo $OUTPUT_FILE"_"$DEFAULT_UC3_RADIX)
        echo "[COPRO CMD] $TA_DEV_KIT_DIR/scripts/sign_rproc_fw.py \\
                --in $OUTPUT_NSECURE $CMD_SIGN \\
                --enc_key $ENCRYPT_KEY \\
                --plat-tlv BOOTADDR $DEFAULT_NSBOOTADDR \\
                --out $filename"
        $TA_DEV_KIT_DIR/scripts/sign_rproc_fw.py \
                --in $OUTPUT_NSECURE $CMD_SIGN \
                --plat-tlv BOOTADDR $DEFAULT_NSBOOTADDR \
                --out $filename

    fi
fi
# UC 2: signature / secure and non secure firmware
if [ -n "$INPUT_NSECURE" -a -n  "$INPUT_SECURE" ]; then
    #  signature / secure and non secure firmware
    filename=$(echo $OUTPUT_FILE"_"$DEFAULT_UC2_RADIX)
    echo "[COPRO SECURE CMD] $TA_DEV_KIT_DIR/scripts/sign_rproc_fw.py --key $SIGNATURE_KEY \\
            --in $OUTPUT_NSECURE \\
            --in $OUTPUT_SECURE  \\
            --plat-tlv BOOTADDR $DEFAULT_SBOOTADDR \\
            --plat-tlv BOOTSEC 0x01 \\
            --out $filename"
    $TA_DEV_KIT_DIR/scripts/sign_rproc_fw.py --key $SIGNATURE_KEY \
            --in $OUTPUT_NSECURE \
            --in $OUTPUT_SECURE  \
            --plat-tlv BOOTADDR $DEFAULT_SBOOTADDR \
            --plat-tlv BOOTSEC 0x01 \
            --out $filename
fi

#--------------------------------
# clean temporary file
if [ -e $OUTPUT_NSECURE ]; then
    if [ $OUTPUT_NSECURE_STRIPPED -eq 1 ]; then
        rm -f $OUTPUT_NSECURE
    fi
fi
if [ -e $OUTPUT_SECURE ]; then
    if [ $OUTPUT_SECURE_STRIPPED -eq 1 ]; then
        rm -f $OUTPUT_SECURE
    fi
fi
