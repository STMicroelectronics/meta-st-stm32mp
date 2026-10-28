#!/bin/bash -
#===============================================================================
#  ORGANIZATION: STMicroelectronics
#     COPYRIGHT: Copyright (C) 2026, STMicroelectronics - All Rights Reserved
#       CREATED: 03/18/2026 09:36
#      REVISION:  ---
#===============================================================================

set -o nounset                              # Treat unset variables as an error

########################################################
# local variable for generation script
MACHINE=
M33=0
OUTPUT=""
PASS_KEY1=""
PASS_KEY2=""
PASS_RPROC=""
PASS_M33_DDR=""
PASS_M33_FW=""

#######################################################
TOOLS_KEYGEN=${KEY_GEN:-STM32_KeyGen_CLI}

# ------------------------------------------
function usage() {
    echo "create_signature_encryption_key.sh <soc name> <OPTIONS>"
    echo "  <soc name>: name of the ST soc"
    echo "      stm32mp13"
    echo "      stm32mp15"
    echo "      stm32mp21"
    echo "      stm32mp23"
    echo "      stm32mp25"
    echo "  <OPTIONS>: list of options"
    echo "  --m33: generate key for Cortex M33 (for stm32mp2 soc)"
    echo "  --passkey1 <pass>: password for bl2 signature on Cortex A (key1) (abrev: -pk1)"
    echo "         pass: several pass are accepted (1 for stm32mp15 but up to 8 for other soc)"
    echo "  --passkey2 <pass>: password for bl2 signature on Cortex A (key2) (abrev: -pk2) [For Cortex M33]"
    echo "         pass: several pass are accepted (1 for stm32mp15 but up to 8 for other soc)"
    echo "  --passrproc <pass>: password for remoteproc signature (rproc) (abrev: -prproc) "
    echo "  --passm33ddr <pass>: password for ddr signature on Cortex M33 (M33 ddr) (abrev: -pddr) [For Cortex M33]"
    echo "  --passm33fw <pass>: password for fw signature on Cortex M33 (M33 fw) (abrev: -pfw) [For Cortex M33]"
    echo "  --output: location on which the key must be create (create directory if not exist)"
    echo ""
    echo "  ENVIRONMENT variable"
    echo "  KEY_GEN=<Aboslute path to STM32_KeyGen_CLI tools>"
    echo "    ex.: KEY_GEN=\"<absolute path>/STM32CubeProgrammer-x.xx/bin/STM32_KeyGen_CLI\""
    echo ""
    echo "Example:"
    echo "  for stm32mp13:"
    echo "    KEY_GEN=<path to STM32CubeProgrammer-x.xx>/bin/STM32_KeyGen_CLI ./create_signature_encryption_key.sh \\"
    echo "              stm32mp13 \\"
    echo "              -pk1 pass0 pass1 pass2 pass3 pass4 pass5 pass6 pass7 \\"
    echo "              -o <output path>"
    echo ""
    echo "  for stm32mp15:"
    echo "    KEY_GEN=<path to STM32CubeProgrammer-x.xx>/bin/STM32_KeyGen_CLI ./create_signature_encryption_key.sh \\"
    echo "              stm32mp15 \\"
    echo "              -pk1 pass \\"
    echo "              -o <output path>"
    echo ""
    echo "  for stm32mp21/stm32mp23/stm32mp25:"
    echo "    KEY_GEN=<path to STM32CubeProgrammer-x.xx>/bin/STM32_KeyGen_CLI ./create_signature_encryption_key.sh \\"
    echo "             stm32mp21 \\"
    echo "             -pk1 pass0 pass1 pass2 pass3 pass4 pass5 pass6 pass7 \\"
    echo "             --passrproc passrproc \\"
    echo "              -o <output path>"
    echo ""
    echo "  for stm32mp21/stm32mp23/stm32mp25 with Cortex M33 support"
    echo "    KEY_GEN=<path to STM32CubeProgrammer-x.xx>/bin/STM32_KeyGen_CLI ./create_signature_encryption_key.sh \\"
    echo "             stm32mp25 \\"
    echo "             --m33 \\"
    echo "             -pk1 pass0 pass1 pass2 pass3 pass4 pass5 pass6 pass7 \\"
    echo "             --passrproc passrproc \\"
    echo "             -pk2 passM0 passM1 passM2 passM3 passM4 passM5 passM6 passM7 \\"
    echo "             --passm33ddr passddr \\"
    echo "             --passm33fw passfw \\"
    echo "              -o <output path>"
    echo ""
    echo "Note:"
    echo "  The script generate on 'output' directory:"
    echo "    * all the keys"
    echo "    * 'infos_keys.txt' files which indicated all command used to generate key and list file generated"
    echo "    * 'distro_configuration_for_signature_encryption_'MACHINE'.txt' files which is a \\"
    echo "        dump of configuration to use with sdk-info"
    echo "    * 'distro_configuration_for_signature_encryption_'MACHINE'-m33td.txt' files which is a \\"
    echo "        dump of configuration to use with sdk-info for m33td configuration"
    echo ""
}
# ------------------------------------------
function dump_parameters() {
    echo "###########################################################"
    echo "Parameters:"
    echo "  Machine: ${MACHINE}"
    case ${MACHINE} in
    stm32mp2*)
        echo "  Generate key for Cortex M33 (OEM-KEY2, M33 DDR, M33 FW): ${M33}"
        ;;
    esac
    echo "  Output directory: ${OUTPUT}"
    echo ""
    echo "  Password:"
    echo -n "     for bl2 on cortex A (key1):"
    if [ -n "${PASS_KEY1}" ]; then
        echo " ${PASS_KEY1}"
    else
        echo " <empty password>"
    fi
    case ${MACHINE} in
    stm32mp2*)
        echo -n "     for remoteproc (rproc):"
            if [ -n "${PASS_RPROC}" ]; then
                echo " ${PASS_RPROC}"
            else
                echo " <empty password>"
            fi
        if [ $M33 -eq 1 ]; then
            echo -n "     for bl2 on cortex M33 (key2):"
            if [ -n "${PASS_KEY2}" ]; then
                echo " ${PASS_KEY2}"
            else
                echo " <empty password>"
            fi
            echo -n "     for M33 DDR:"
            if [ -n "${PASS_M33_DDR}" ]; then
                echo " ${PASS_M33_DDR}"
            else
                echo " <empty password>"
            fi
            echo -n "     for M33 FW:"
            if [ -n "${PASS_M33_FW}" ]; then
                echo " ${PASS_M33_FW}"
            else
                echo " <empty password>"
            fi
        else
            if [ -n "${PASS_RPROC}" ]; then
                echo " ${PASS_RPROC}"
            else
                echo " <empty password>"
            fi
        fi
        ;;
    esac
    echo "###########################################################"
    echo ""
}
# ------------------------------------------

INFO_FILE=infos_keys.txt
DISTRO_FILE=distro_configuration_for_signature_encryption

function debug_cmd() {
    output="$1"
    shift
    data=$@
    output_file="${output}/infos_keys.txt"
    >&2 echo "${data}"
    echo "${data}" >> "${output_file}"

}
function echo_info_s() {
    output="$1"
    shift
    data=$@
    output_file="${output}/infos_keys.txt"
    echo "${data}" >> "${output_file}"
}

# display information on stdout and info file (Dump And Store)
function echo_info_das() {
    output="$1"
    shift
    data=$@
    output_file="${output}/infos_keys.txt"
    echo "${data}"
    echo "${data}" >> "${output_file}"
}
function print_info_das() {
    output="$1"
    shift
    output_file="${output}/infos_keys.txt"
    printf "$@"
    printf "$@" >> "${output_file}"
}

# ----------------------------------
function signature_key() {
    key=$1
    cortex=$2
    output=$3
    number=$4
    password=$5

    mkdir -p "${output}"/tmp_key_${key}
    echo_info_s "${output}" "##########################################################"
    echo_info_s "${output}" "# Generate Signature Key: ${key}"
    echo_info_s "${output}" ""
    debug_cmd "${output}" "${TOOLS_KEYGEN} -abs ${output}/tmp_key_${key} -pwd ${password} -n ${number}"
    ${TOOLS_KEYGEN} -abs "${output}"/tmp_key_${key} -pwd ${password} -n ${number}
    # rename files
    max=$((${number} -1))
    for i in $(seq 0 ${max})
    do
        # rename privateKey0X.pem
        mv -f "${output}"/tmp_key_${key}/privateKey0${i}.pem "${output}"/tmp_key_${key}/${cortex}_${key}_privateKey0${i}.pem
        # rename publicKey0X.pem
        mv -f "${output}"/tmp_key_${key}/publicKey0${i}.pem "${output}"/tmp_key_${key}/${cortex}_${key}_publicKey0${i}.pem
        # rename publicKeyHash0X.bin
        mv -f "${output}"/tmp_key_${key}/publicKeyHash0${i}.bin "${output}"/tmp_key_${key}/${cortex}_${key}_publicKeyHash0${i}.bin
    done
    # rename publicKeysHashHashes.bin
    mv "${output}"/tmp_key_${key}/publicKeysHashHashes.bin "${output}"/tmp_key_${key}/${cortex}_${key}_publicKeysHashHashes.bin
    # move file on output directory
    if [ -e "${output}"/${cortex}_${key}_publicKeysHashHashes.bin ]; then
        # key file already: ask if you would like to overwrite it
        echo "[WARNING]: A previous version of KEY ${key} exist on this directory"
        echo "[WARNING]:    ${output}"
        echo "[WARNING]: Would you like to overwrite it: [Y/n]"
        read -r answer
        if [[ "$answer" =~ ^[Yy]+[ESes]* ]]; then
            mv -f "${output}"/tmp_key_${key}/${cortex}_${key}_* "${output}/"
        fi
    else
       mv -f "${output}"/tmp_key_${key}/${cortex}_${key}_* "${output}/"
    fi
    # remove temporary output key
    rm -rf "${output}"/tmp_key_${key}
    echo_info_s "${output}" ""
}

function encryption_key() {
    key_file_name=$1
    size=$2
    output=$3

    mkdir -p "${output}"/tmp_enc_key
    echo_info_s "${output}" "##########################################################"
    echo_info_s "${output}" "# Generate Encryption Key: ${key}"
    echo_info_s "${output}" ""
    debug_cmd "${output}" "${TOOLS_KEYGEN} -rand ${size} "${output}"/tmp_enc_key/${key_file_name}"
    ${TOOLS_KEYGEN} -rand ${size} "${output}"/tmp_enc_key/${key_file_name}
    # move file on output directory
    if [ -e "${output}"/${key_file_name} ]; then
        # key file already: ask if you would like to overwrite it
        echo "[WARNING]: A previous version of file ${key_file_name} exist on this directory"
        echo "[WARNING]:    ${output}"
        echo "[WARNING]: would you like to overwrite it: [Y/n]"
        read -r answer
        if [[ "$answer" =~ ^[Yy]+[ESes]* ]]; then
            mv -f "${output}"/tmp_enc_key/${key_file_name} "${output}/"
        fi
    else
        mv -f "${output}"/tmp_enc_key/${key_file_name} "${output}/"
    fi
    # remove temporary output key
    rm -rf "${output}"/tmp_enc_key
    echo_info_s "${output}" ""
}

function signature_rproc_key() {
    key=$1
    cortex=$2
    output=$3
    password=$4

    mkdir -p ${output}/tmp_key_${key}
    echo_info_s "${output}" "##########################################################"
    echo_info_s "${output}" "# Generate Remote Proc Key: ${key}"
    echo_info_s "${output}" ""
    debug_cmd "${output}" "${TOOLS_KEYGEN} -abs ${output}/tmp_key_${key} -pwd ${password}"
    ${TOOLS_KEYGEN} -abs ${output}/tmp_key_${key} -pwd ${password}
    # generate der file from private key

    debug_cmd "${output}" "openssl ec -pubin -in ${output}/tmp_key_${key}/publicKey.pem -outform DER -pubout -out ${output}/tmp_key_${key}/${cortex}_${key}_ecc_publicKey.der"
    openssl ec -pubin -in ${output}/tmp_key_${key}/publicKey.pem -outform DER -pubout -out ${output}/tmp_key_${key}/${cortex}_${key}_ecc_publicKey.der

    # rename privateKey.pem
    mv -f ${output}/tmp_key_${key}/privateKey.pem ${output}/tmp_key_${key}/${cortex}_${key}_ecc_privateKey.pem
    # rename publicKey.pem
    mv -f ${output}/tmp_key_${key}/publicKey.pem ${output}/tmp_key_${key}/${cortex}_${key}_ecc_publicKey.pem
    # rename publicKeyhash.bin
    mv -f ${output}/tmp_key_${key}/publicKeyhash.bin ${output}/tmp_key_${key}/${cortex}_${key}_ecc_publicKeyhash.bin
    # move file on output directory
    if [ -e ${output}/tmp_key_${key}/${cortex}_${key}_publicKeyhash.bin ]; then
        # key file already: ask if you would like to overwrite it
        echo "[WARNING]: A previous version of KEY ${key} exist on this directory"
        echo "[WARNING]:    ${output}"
        echo "[WARNING]: would you like to overwrite it: [Y/n]"
        read -r answer
        if [[ "$answer" =~ ^[Yy]+[ESes]* ]]; then
            mv -f "${output}"/tmp_key_${key}/${cortex}_${key}_* "${output}/"
        fi
    else
        mv -f "${output}"/tmp_key_${key}/${cortex}_${key}_* "${output}/"
    fi
    # remove temporary output key
    rm -rf ${output}/tmp_key_${key}
    echo_info_s "${output}" ""
}

function signature_asymetric_key() {
    key=$1
    cortex=$2
    output=$3
    password=$4

    mkdir -p ${output}/tmp_key_${key}
    echo_info_s "${output}" "##########################################################"
    echo_info_s "${output}" "# Generate Asymetric Key: ${key}"
    echo_info_s "${output}" ""
    debug_cmd "${output}" "${TOOLS_KEYGEN} -abs ${output}/tmp_key_${key} -pwd ${password}"
    ${TOOLS_KEYGEN} -abs ${output}/tmp_key_${key} -pwd ${password}

    # The hash are not usable in this format, need to regenerate it with the help of private key
    # Extract public key in DER format
    debug_cmd "${output}" "openssl ec -pubin -in ${output}/tmp_key_${key}/privateKey.pem -pubout -outform DER -out ${output}/tmp_key_${key}/${cortex}_${key}_publicKey.der"
    openssl ec -pubin -in ${output}/tmp_key_${key}/publicKey.pem -pubout -outform DER -out ${output}/tmp_key_${key}/${cortex}_${key}_publicKey.der
    # Hash the key and store the result in a file
    debug_cmd "${output}" "openssl dgst -sha256 -binary ${output}/tmp_key_${key}/public_key.der > ${output}/tmp_key_${key}/${cortex}_${key}_hash.bin"
    openssl dgst -sha256 -binary ${output}/tmp_key_${key}/${cortex}_${key}_publicKey.der > ${output}/tmp_key_${key}/${cortex}_${key}_hash.bin

    # rename privateKey00.pem
    mv -f ${output}/tmp_key_${key}/privateKey.pem ${output}/tmp_key_${key}/${cortex}_${key}_privateKey.pem
    # rename publicKey00.pem
    mv -f ${output}/tmp_key_${key}/publicKey.pem ${output}/tmp_key_${key}/${cortex}_${key}_publicKey.pem
    # rename hash00.bin
    rm -f  ${output}/tmp_key_${key}/publicKeyhash.bin
    # move file on output directory
    if [ -e ${output}/${cortex}_${key}_publicKeyhash.bin ]; then
        # key file already: ask if you would like to overwrite it
        echo "[WARNING]: A previous version of KEY ${key} exist on this directory"
        echo "[WARNING]:    ${output}"
        echo "[WARNING]: would you like to overwrite it: [Y/n]"
        read -r answer
        if [[ "$answer" =~ ^[Yy]+[ESes]* ]]; then
            mv -f "${output}"/tmp_key_${key}/${cortex}_${key}_* "${output}/"
        fi
    else
        mv -f "${output}"/tmp_key_${key}/${cortex}_${key}_* "${output}/"
    fi
    # remove temporary output key
    rm -rf ${output}/tmp_key_${key}
    echo_info_s "${output}" ""
}

# -------------------------------------
# STM32MP13
function create_signature_key_for_stm32mp13() {
    output="$1"
    pass="$2"
    signature_key PKHTH a7 "${output}" 8 "${pass}"
}
function create_encryption_key_for_stm32mp13() {
    output="$1"
    encryption_key a7_EDMK_key_128bits.bin 16 "${output}"
    # create text version of encryption key
    debug_cmd "${output}" "hexdump -ve '1/1 \"%.2x\"' \"${output}\"/a7_EDMK_key_128bits.bin > \"${output}\"/a7_EDMK_key_256bits.txt"
    debug_cmd "${output}" "hexdump -ve '1/1 \"%.2x\"' \"${output}\"/a7_EDMK_key_128bits.bin >> \"${output}\"/a7_EDMK_key_256bits.txt"
    hexdump -ve '1/1 "%.2x"' "${output}"/a7_EDMK_key_128bits.bin > "${output}"/a7_EDMK_key_256bits.txt
    hexdump -ve '1/1 "%.2x"' "${output}"/a7_EDMK_key_128bits.bin >> "${output}"/a7_EDMK_key_256bits.txt
}

function dump_key_stm32mp13() {
    output="$1"
    echo_info_das "${output}" "###########################################################"
    echo_info_das "${output}" "Summary of key creation for stm32mp13:"
    echo_info_das "${output}" "Location: ${output}"
    echo_info_das "${output}" "Signature:"
    print_info_das "${output}" "[%16s] %0s (To FUSE ON BOARD)\n"  "PKHTH" "a7_PKHTH_publicKeysHashHashes.bin"
    for i in $(seq 0 7); do
        print_info_das "${output}" "%18s %0s\n" " " "a7_PKHTH_privateKey0${i}.pem"
    done
    for i in $(seq 0 7); do
        print_info_das "${output}" "%18s %0s\n" " " "a7_PKHTH_publicKey0${i}.pem"
    done
    for i in $(seq 0 7); do
        print_info_das "${output}" "%18s %0s\n" " " "a7_PKHTH_publicKeyHash0${i}.pem"
    done
    echo_info_das "${output}" ""
    echo_info_das "${output}" "Encryption:"
    print_info_das "${output}" "[%16s] %0s (To FUSE ON BOARD)\n" "EDMK" "a7_EDMK_key_128bits.bin"
    print_info_das "${output}" "%18s %0s\n" " " "a7_EDMK_key_256bits.txt"
    echo_info_das "${output}" ""
    echo_info_das "${output}" "###########################################################"
}
function dump_key_for_distribution_stm32mp13() {
    output="$1"
    first_pass=$(echo ${PASS_KEY1} | cut -d' ' -f 1)

    cat << EOF > ${output}/${DISTRO_FILE}_${MACHINE}.txt
##################################################################
##################################################################
### Signature / Encryption configuration
##################################################################
##################################################################
#
#          STM32MP13: CORTEX A7
#   ---------------------------------------
#   |                   BL2               |
#   |                 (TF-A)              |
#   |                                     |
#   |    verified with  | decrypted with  |
#   |       PKHTH       |       EDMK      |
#   ---------------------------------------
#   |                 FIP                 |
#   |        (FW-CONFIG - TF-A)           |
#   |               (optee)               |
#   |                                     |
#   |    verified with  | decrypted with  |
#   |      PKHTH        |       EDMK      |
#   ---------------------------------------
# WARNING: For FIP will be verified (and decrypted) by BL2, you need to compile
#   TF-A with specific option for signature support:
#     for all platform: TRUSTED_BOARD_BOOT=1 MBEDTLS_DIR=<path to mbedtls source code>
#   TF-A with specific option for encryption support:
#     for all platform: DECRYPTION_SUPPORT=aes_gcm ENCRYPT_BL32=1 MBEDTLS_DIR=<path to mbedtls source code>
#            ENCRYPT_BL31=1
#
#
# Activate signature for BL2 and FIP on cortex A7
SIGN_ENABLE=0

# Activate Encryption for BL2 and FIP on cortex A7
ENCRYPT_ENABLE=0

# Absolute path to signing tools
SIGN_TOOL="<absolute path>/STM32CubeProgrammer-x.xx/bin/STM32_SigningTool_CLI"

# Directory on which all key are available
your_absolute_key_path=${output}

# For naming the output file, use the following suffix:
signature_suffix="_Signed"
encryption_suffix="_Encrypted"

# ------ Key associated to PKHTH ---------------------
# Private Key for signature (absolute path to key)
SIGN_KEY="\${your_absolute_key_path}/a7_PKHTH_privateKey00.pem"
# List of Public key for signature
SIGN_KEY_PUB="\\
    \${your_absolute_key_path}/a7_PKHTH_publicKey00.pem \\
    \${your_absolute_key_path}/a7_PKHTH_publicKey01.pem \\
    \${your_absolute_key_path}/a7_PKHTH_publicKey02.pem \\
    \${your_absolute_key_path}/a7_PKHTH_publicKey03.pem \\
    \${your_absolute_key_path}/a7_PKHTH_publicKey04.pem \\
    \${your_absolute_key_path}/a7_PKHTH_publicKey05.pem \\
    \${your_absolute_key_path}/a7_PKHTH_publicKey06.pem \\
    \${your_absolute_key_path}/a7_PKHTH_publicKey07.pem \\
"
# Password (key pass) for all signature key
SIGN_KEY_PASS="${PASS_KEY1}"

# Password key associated to private key selected via SIGN_KEY
SIGN_KEY_PASS_FOR_SPECIFC_KEY="${first_pass}"

# ------ Key associated to EDMK---------------------------
# encryption key use to decrypt Bl2 (TF-A) (128bits)
ENCRYPT_FSBL_KEY="\${your_absolute_key_path}/a7_EDMK_key_128bits.bin"

# ------ Key associated to EDMK ---------------------------
# Encryption key use for fip content (256bits on txt)
ENCRYPT_FIP_KEY="\${your_absolute_key_path}/a7_EDMK_key_256bits.txt"

##################################################################
##################################################################
### End of Signature / Encryption
##################################################################
##################################################################
EOF
}

# -------------------------------------
# STM32MP15
function create_signature_key_for_stm32mp15() {
    output="$1"
    pass="$2"
    signature_key PKH a7 "${output}" 1 "${pass}"
}
function dump_key_stm32mp15() {
    output="$1"
    echo_info_das "${output}" "###########################################################"
    echo_info_das "${output}" "Summary of key creation for stm32mp15:"
    echo_info_das "${output}" "Location: ${output}"
    echo_info_das "${output}" "Signature:"
    print_info_das "${output}" "[%16s] %0s (To FUSE ON BOARD)\n" "PKH" "a7_PKH_publicKeysHashHashes.bin"
    print_info_das "${output}" "%18s %0s\n" " " "a7_PKH_privateKey00.pem"
    print_info_das "${output}" "%18s %0s\n" " " "a7_PKH_publicKey00.pem"
    print_info_das "${output}" "%18s %0s\n" " " "a7_PKH_publicKeyHash00.pem"
    echo_info_das "${output}" ""
    echo_info_das "${output}" "###########################################################"
}
function dump_key_for_distribution_stm32mp15() {
    output="$1"
    first_pass=$(echo ${PASS_KEY1} | cut -d' ' -f 1)

    cat << EOF > ${output}/${DISTRO_FILE}_${MACHINE}.txt
##################################################################
##################################################################
### Signature / Encryption configuration
##################################################################
##################################################################
#
#    STM32MP15: CORTEX A7
#    ---------------------
#   |           BL2      |
#   |        (TF-A)      |
#   |                    |
#   |   verified with    |
#   |        PKHTH       |
#   ----------------------
#   |         FIP        |
#   | (FW-CONFIG - TF-A) |
#   |    (optee)         |
#   |                    |
#   |   verified with    |
#   |       PKHTH        |
#   ---------------------
# WARNING: For FIP will be verified (and decrypted) by BL2, you need to compile
#   TF-A with specific option for signature support:
#     for all platform: TRUSTED_BOARD_BOOT=1 MBEDTLS_DIR=<path to mbedtls source code>
#   TF-A with specific option for encryption support:
#     for all platform: DECRYPTION_SUPPORT=aes_gcm ENCRYPT_BL32=1 MBEDTLS_DIR=<path to mbedtls source code>
#            ENCRYPT_BL31=1
#
#
# Activate signature for BL2 and FIP on cortex A7
SIGN_ENABLE=0

# Activate Encryption for BL2 and FIP on cortex A7
# WARNING: encryption is not supported by stm32mp15 platform
# ENCRYPT_ENABLE MUST be disabled
ENCRYPT_ENABLE=0

# Absolute path to signing tools
SIGN_TOOL="<absolute path>/STM32CubeProgrammer-x.xx/bin/STM32_SigningTool_CLI"

# Directory on which all key are available
your_absolute_key_path=\${output}

# For naming the output file, use the following suffix:
signature_suffix="_Signed"
encryption_suffix="_Encrypted"

# ------ Key associated to PKH ---------------------
# Private Key for signature (absolute path to key)
SIGN_KEY="\${your_absolute_key_path}/a7_PKH_privateKey00.pem"
# List of Public key for signature
SIGN_KEY_PUB="\\
    \${your_absolute_key_path}/a7_PKH_publicKey00.pem \\
"
# Password (key pass) for all signature key
SIGN_KEY_PASS="${PASS_KEY1}"

# Password key associated to private key selected via SIGN_KEY
SIGN_KEY_PASS_FOR_SPECIFC_KEY="${first_pass}"

##################################################################
##################################################################
### End of Signature / Encryption
##################################################################
##################################################################
EOF
}


# --------------------------------------
# STM32MP2
function create_signature_key_for_stm32mp2() {
    output="$1"
    pass_key1="$2"
    pass_key2="$3"
    pass_rproc="$4"
    pass_m33_ddr="$5"
    pass_m33_fw="$5"
    signature_key OEM-KEY1 a35 "${output}" 8 "${pass_key1}"
    signature_rproc_key RPROC-FW-PKH m33 "${output}" "${pass_rproc}"
    if [ $M33 -eq 1 ]; then
        signature_key OEM-KEY2 m33 "${output}" 8 "${pass_key2}"
        signature_asymetric_key FSBLM-DDR-FW-PKH m33 "${output}" "${pass_m33_ddr}"
        signature_asymetric_key FSBLM-M33-FW-PKH m33 "${output}" "${pass_m33_fw}"
        encryption_key m33_TFM-IAK_key_256bits.bin 32 "${output}"
    fi
}
# -------------------------------------
# STM32MP21
function create_encryption_key_for_stm32mp21() {
    output="$1"
    encryption_key a35_EDMK1-128b_key_128bits.bin 16 "${output}"
    encryption_key a35_FIP-EDMK_key_256bits.bin 32 "${output}"
    encryption_key m33_RPROC-FW-ENC-KEY_key_256bits.bin 32 "${output}"
    if [ $M33 -eq 1 ]; then
        encryption_key m33_EDMK2-128b_key_128bits.bin 16 "${output}"
    fi
}
function dump_key_stm32mp2() {
    output="$1"
    echo_info_das "${output}" "###########################################################"
    echo_info_das "${output}" "Summary of key creation for stm32mp21:"
    echo_info_das "${output}" "Location: ${output}"
    echo_info_das "${output}" "Signature:"
    # OEM-KEY1
    printf "[%16s] %0s (To PUT BOARD)\n" "OEM-KEY1" "a35_OEM-KEY1_publicKeysHashHashes.bin"
    for i in $(seq 0 8); do
        print_info_das "${output}" "%18s %0s\n" " " "a35_OEM-KEY1_privateKey0${i}.pem"
    done
    for i in $(seq 0 8); do
        print_info_das "${output}" "%18s %0s\n" " " "a35_OEM-KEY1_publicKey0${i}.pem"
    done
    for i in $(seq 0 8); do
        print_info_das "${output}" "%18s %0s\n" " " "a35_OEM-KEY1_publicKeyHash0${i}.pem"
    done
    # RPROC-FW-PKH
    print_info_das "${output}" "[%16s] %0s (To FUSE ON BOARD)\n" "RPROC-FW-PKH" "m33_RPROC-FW-PKH_ecc_publicKeyhash.bin"
    print_info_das "${output}" "%18s %0s\n" " " "m33_RPROC-FW-PKH_ecc_privateKey.pem"
    print_info_das "${output}" "%18s %0s\n" " " "m33_RPROC-FW-PKH_ecc_publicKey.pem"
    print_info_das "${output}" "%18s %0s\n" " " "m33_RPROC-FW-PKH_ecc_publicKey.der"
    print_info_das "${output}" "%18s %0s\n" " " "m33_RPROC-FW-PKH_ecc_publicKeyhash.bin"
    if [ $M33 -eq 1 ]; then
        # OEM-KEY2
        print_info_das "${output}" "[%16s] %0s (To FUSE ON BOARD)\n" "OEM-KEY2" "m33_OEM-KEY2_publicKeysHashHashes.bin"
        for i in $(seq 0 8); do
            print_info_das "${output}" "%18s %0s\n" " " "m33_OEM-KEY2_privateKey0${i}.pem"
        done
        for i in $(seq 0 8); do
            print_info_das "${output}" "%18s %0s\n" " " "m33_OEM-KEY2_publicKey0${i}.pem"
        done
        for i in $(seq 0 8); do
            print_info_das "${output}" "%18s %0s\n" " " "m33_OEM-KEY2_publicKeyHash0${i}.pem"
        done
        # FSBLM-DDR-FW-PKH
        print_info_das "${output}" "[%16s] %0s (To FUSE ON BOARD)\n" "FSBLM-DDR-FW-PKH" "m33_FSBLM-DDR-FW-PKH_hash.bin"
        print_info_das "${output}" "%18s %0s\n" " " "m33_FSBLM-DDR-FW-PKH_privateKey.pem"
        print_info_das "${output}" "%18s %0s\n" " " "m33_FSBLM-DDR-FW-PKH_publicKey.pem"
        print_info_das "${output}" "%18s %0s\n" " " "m33_FSBLM-DDR-FW-PKH_publicKey.der"
        # FSBLM-M33-FW-PKH
        print_info_das "${output}" "[%16s] %0s (To FUSE ON BOARD)\n" "FSBLM-M33-FW-PKH" "m33_FSBLM-M33-FW-PKH_hash.bin"
        print_info_das "${output}" "%18s %0s\n" " " "m33_FSBLM-M33-FW-PKH_privateKey.pem"
        print_info_das "${output}" "%18s %0s\n" " " "m33_FSBLM-M33-FW-PKH_publicKey.pem"
        print_info_das "${output}" "%18s %0s\n" " " "m33_FSBLM-M33-FW-PKH_publicKey.der"
        # TFM-IAK
        print_info_das "${output}" "[%16s] %0s (To FUSE ON BOARD)\n" "TFM-IAK" "m33_TFM-IAK_key_256bits.bin"
    fi
    echo_info_das "${output}" ""
    echo_info_das "${output}" "Encryption:"
    case ${MACHINE} in
    stm32mp21)
        print_info_das "${output}" "[%16s] %0s (To FUSE ON BOARD)\n" "EDMK1-128b" "a35_EDMK1-128b_key_128bits.bin"
        ;;
    stm32mp23|stm32mp25)
        print_info_das "${output}" "[%16s] %0s (To FUSE ON BOARD)\n" "EDMK1" "a35_EDMK1_key_128bits.bin"
    esac
    print_info_das "${output}" "[%16s] %0s (To FUSE ON BOARD)\n" "FIP-EDMK" "a35_FIP-EDMK_key_256bits.bin"
    print_info_das "${output}" "[%16s] %0s (To FUSE ON BOARD)\n" "RPROC-FW-ENC-KEY" "m33_RPROC-FW-ENC-KEY_key_256bits.bin"
    if [ $M33 -eq 1 ]; then
        case ${MACHINE} in
        stm32mp21)
            print_info_das "${output}" "[%16s] %0s (To FUSE ON BOARD)\n" "EDMK2-128b" "m335_EDMK2-128b_key_128bits.bin"
            ;;
        stm32mp23|stm32mp25)
            print_info_das "${output}" "[%16s] %0s (To FUSE ON BOARD)\n" "EDMK2" "a35_EDMK2_key_128bits.bin"
        esac
    fi
    if [ $M33 -eq 1 ]; then
        echo_info_das "${output}" ""
        case ${MACHINE} in
        stm32mp21)
            echo_info_das "${output}" "[WARNING] About OEM-KEY2 and EDMK2-128b:"
            echo_info_das "${output}" "[WARNING]    If you fuse this two keys, at runtime "
            echo_info_das "${output}" "[WARNING]    OEM-KEY2 will be used to verify first boot binary on Cortex M33 and "
            echo_info_das "${output}" "[WARNING]    EDMK2-128b will be used to decrypt verify first boot binary on Cortex M33."
            echo_info_das "${output}" "[WARNING]"
            echo_info_das "${output}" "[WARNING]    It's no more possible to use OEM-KEY1 EDMK1-128b instead for Cortex M33."
            ;;
        stm32mp23|stm32mp25)
            echo_info_das "${output}" "[WARNING] About OEM-KEY2 and EDMK2:"
            echo_info_das "${output}" "[WARNING]    If you fuse this two keys, at runtime "
            echo_info_das "${output}" "[WARNING]    OEM-KEY2 will be used to verify first boot binary on Cortex M33 and "
            echo_info_das "${output}" "[WARNING]    EDMK2 will be used to decrypt verify first boot binary on Cortex M33."
            echo_info_das "${output}" "[WARNING]"
            echo_info_das "${output}" "[WARNING]    It's no more possible to use OEM-KEY1 EDMK1 instead for Cortex M33."
        esac
    fi

    echo_info_das "${output}" ""
    echo_info_das "${output}" "###########################################################"
}

function dump_key_for_distribution_stm32mp2() {
    output="$1"
    first_pass=$(echo ${PASS_KEY1} | cut -d' ' -f 1)

    first_pass_key1=$(echo ${PASS_KEY1} | cut -d' ' -f 1)
    first_pass_key2=$(echo ${PASS_KEY2} | cut -d' ' -f 1)

    case ${MACHINE} in
    stm32mp21)
        encrypt_key_fsbl_token="-128b"
        ;;
    *)
        encrypt_key_fsbl_token=""
        ;;
    esac

    cat << EOF > ${output}/${DISTRO_FILE}_${MACHINE}.txt
##################################################################
##################################################################
### Signature / Encryption configuration
##################################################################
##################################################################
#
#          STM32MP2: CORTEX A35 A35TD               CORTEX M33
#   ---------------------------------------    ---------------------
#   |                   BL2               |   |                    |
#   |                 (TF-A)              |   |                    |
#   |                                     |   |                    |
#   |    verified with  | decrypted with  |   |                    |
#   |       OEM-KEY1    |     *(1)        |   |                    |
#   --------------------------------------    ----------------------
#   |                 FIP                 |   |  co-processor FW   |
#   |        (FW-CONFIG - TF-A)           |   |    (M33Project)    |
#   |               (optee)               |   |                    |
#   [          (DDR fw - TF-A)            |   |   verified with    |
#   |                                     |   |     RPROC-FW-PKH   |
#   |    verified with  | decrypted with  |   |   decrypted with   |
#   |      OEM-KEY1     |     FIP-EDMK    |   |   RPROC-FW-ENC-KEY |
#    --------------------------------------   ----------------------
#
# (*1):
#   for stm32mp21 is EDMK1-128b
#   for stm32mp23/25 is EDMK1
#
# WARNING: For FIP will be verified (and decrypted) by BL2, you need to compile
#   TF-A with specific option for signature support:
#     for all platform: TRUSTED_BOARD_BOOT=1 MBEDTLS_DIR=<path to mbedtls source code> BRANCH_PROTECTION=0
#   TF-A with specific option for encryption support:
#     for all platform: DECRYPTION_SUPPORT=aes_gcm ENCRYPT_BL32=1 MBEDTLS_DIR=<path to mbedtls source code>
#

# Activate signature for BL2 and FIP on cortex A35
SIGN_ENABLE=0

# Activate Encryption for BL2 and FIP on cortex A35
ENCRYPT_ENABLE=0

# Activate signature during for loading firmware on Cortex M33 (coprocessor)
SIGN_COPRO_ENABLE=0
# Activate encryption during for loading firmware on Cortex M33 (coprocessor)
ENCRYPT_COPRO_ENABLE=0

# Absolute path to signing tools
SIGN_TOOL="<absolute path>/STM32CubeProgrammer-x.xx/bin/STM32_SigningTool_CLI"

# Directory on which all key are available
your_absolute_key_path=${output}

# For naming the output file, use the following suffix:
signature_suffix="_Signed"
encryption_suffix="_Encrypted"

# ------ Key associated to OEM-KEY1 ---------------------
# Private Key for signature (absolute path to key)
SIGN_KEY="\${your_absolute_key_path}/a35_OEM-KEY1_privateKey00.pem"
# List of Public key for signature
# NOTE:
# for stm32mp2 platform, 8 key pairs public/private are supported
SIGN_KEY_PUB="\\
    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey00.pem \\
    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey01.pem \\
    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey02.pem \\
    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey03.pem \\
    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey04.pem \\
    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey05.pem \\
    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey06.pem \\
    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey07.pem \\
"
# Password (key pass) for all signature key
# NOTE:
# for stm32mp2 platform, up to 8 keys can be requested (means put all password according to key)
SIGN_KEY_PASS="${PASS_KEY1}"

# Password key associated to private key selected via SIGN_KEY
SIGN_KEY_PASS_FOR_SPECIFC_KEY="${first_pass}"

# ------ Key associated to EDMK1${encrypt_key_fsbl_token}------------
# encryption key use to decrypt Bl2 (TF-A) (128bits)
ENCRYPT_FSBL_KEY="\${your_absolute_key_path}/a35_EDMK1${encrypt_key_fsbl_token}_key_128bits.bin"

# ------ Key associated to FIP-EDMK------------
# encryption key use to decrypt FIP component (256bits)
ENCRYPT_FIP_KEY="\${your_absolute_key_path}/a35_FIP-EDMK_key_256bits.bin"

# ------ Key associated to RPROC-FW-PKH for coprocessor signature----------
# Type of key use for coprocessor signature:
# (256bits symmetric key)
#  RSA: Asymetric cryptography algorithm
#  ECC: Elliptic curve cryptography
SIGN_COPRO_TYPE=ecc

# For RSA KEY
SIGN_COPRO_RSA_INFOKEY=""
SIGN_COPRO_RSA_PRIVKEY=""
SIGN_COPRO_RSA_PASS=""

# For ECC KEY
SIGN_COPRO_ECC_INFOKEY="\${your_absolute_key_path}/m33_RPROC-FW-PKH_ecc_publicKey.der"
SIGN_COPRO_ECC_PRIVKEY="\${your_absolute_key_path}/m33_RPROC-FW-PKH_ecc_privateKey.pem"
SIGN_COPRO_ECC_PASS="passrproc"

# ------ Key associated to RPROC-FW-ENC-KEY for coprocessor encryption-------
# Encryption key (256bits)
ENCRYPT_COPRO_KEY="\${your_absolute_key_path}/m33_RPROC-FW-ENC-KEY_key_256bits.bin"

##################################################################
##################################################################
### End of Signature / Encryption configuration
##################################################################
##################################################################
EOF
}

function dump_key_for_distribution_stm32mp2_m33() {
    output="$1"
    first_pass_key1=$(echo ${PASS_KEY1} | cut -d' ' -f 1)
    first_pass_key2=$(echo ${PASS_KEY2} | cut -d' ' -f 1)

    case ${MACHINE} in
    stm32mp21)
        encrypt_key_fsbl_token="-128b"
        ;;
    *)
        encrypt_key_fsbl_token=""
        ;;
    esac

    cat << EOF > ${output}/${DISTRO_FILE}_${MACHINE}-m33td.txt
##################################################################
##################################################################
### Signature / Encryption configuration
##################################################################
##################################################################
#
#          STM32MP2: CORTEX A35 A35TD               CORTEX M33
#   ---------------------------------------    ---------------------
#   |                   BL2               |   |       BL2          |
#   |                 (TF-A)              |   |   (TF-M MCUBOOT)   |
#   |                                     |   |                    |
#   |    verified with  | decrypted with  |   | verified with      |
#   |       OEM-KEY1    |       (*1)      |   |   OEM-KEY2         |
#   |                                     |   |  (or OEM-KEY1)     |
#   --------------------------------------    ----------------------
#   |                 FIP                 |   |       DDR FW       |
#   |        (FW-CONFIG - TF-A)           |   |    (TF-M)          |
#   |               (optee)               |   |                    |
#   [          (DDR fw - TF-A)            |   |   verified with    |
#   |                                     |   |  FSBLM-DDR-FW-PKH  |
#   |    verified with  | decrypted with  |   ----------------------
#   |      OEM-KEY1     |     FIP-EDMK    |   |       M33 FW       |
#    --------------------------------------   |                    |
#                                             |   verified with    |
#                                             |  FSBLM-M33-FW-PKH  |
#                                             ----------------------
#
# (*1):
#   for stm32mp21 is EDMK1-128b
#   for stm32mp23/25 is EDMK1
#
# WARNING: For FIP will be verified (and decrypted) by BL2, you need to compile
#   TF-A with specific option for signature support:
#     for all platform: TRUSTED_BOARD_BOOT=1 MBEDTLS_DIR=<path to mbedtls source code> BRANCH_PROTECTION=0
#   TF-A with specific option for encryption support:
#     for all platform: DECRYPTION_SUPPORT=aes_gcm ENCRYPT_BL32=1 MBEDTLS_DIR=<path to mbedtls source code>
#

# Activate signature for BL2 and FIP on cortex A35
SIGN_ENABLE=0

# Activate Encryption for BL2 and FIP on cortex A35
ENCRYPT_ENABLE=0

# Activate signature during for loading firmware on Cortex M33 (coprocessor)
SIGN_COPRO_ENABLE=0
# Activate encryption during for loading firmware on Cortex M33 (coprocessor)
ENCRYPT_COPRO_ENABLE=0

# Absolute path to signing tools
SIGN_TOOL="<absolute path>/STM32CubeProgrammer-x.xx/bin/STM32_SigningTool_CLI"

# Directory on which all key are available
your_absolute_key_path=${output}

# For naming the output file, use the following suffix:
signature_suffix="_Signed"
encryption_suffix="_Encrypted"

# ------ Key associated to OEM-KEY1 ---------------------
# Private Key for signature (absolute path to key)
SIGN_KEY="\${your_absolute_key_path}/a35_OEM-KEY1_privateKey00.pem"
# List of Public key for signature
# NOTE:
# for stm32mp2 platform, 8 key pairs public/private are supported
SIGN_KEY_PUB="\\
    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey00.pem \\
    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey01.pem \\
    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey02.pem \\
    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey03.pem \\
    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey04.pem \\
    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey05.pem \\
    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey06.pem \\
    \${your_absolute_key_path}/a35_OEM-KEY1_publicKey07.pem \\
"
# Password (key pass) for all signature key
# NOTE:
# for stm32mp2 platform, up to 8 keys can be requested (means put all password according to key)
SIGN_KEY_PASS="${PASS_KEY1}"

# Password key associated to private key selected via SIGN_KEY
SIGN_KEY_PASS_FOR_SPECIFC_KEY="${first_pass_key1}"

# ------ Key associated to EDMK1${encrypt_key_fsbl_token}------------
# encryption key use to decrypt Bl2 (TF-A) (128bits)
ENCRYPT_FSBL_KEY="\${your_absolute_key_path}/a35_EDMK1${encrypt_key_fsbl_token}_key_128bits.bin"

# ------ Key associated to OEM-KEY2 ---------------------
# NOTE: if OEM-KEY1 is configured on OTP for verifying Cortex M33 BL2
#     You need to configure the following variable as OEM-KEY1

USE_SAME_KEY_FOR_OEM_KEY1_AND_OEM_KEY2=1
if [ \${USE_SAME_KEY_FOR_OEM_KEY1_AND_OEM_KEY2} -eq 1 ]; then
    # USING OEM-KEY1 for Cortex M33 BL2 verification
    # Private Key for signature (absolute path to key)
    SIGN_KEY_M="\${your_absolute_key_path}/a35_OEM-KEY1_privateKey00.pem"
    # List of Public key for signature
    # NOTE:
    # for stm32mp2 platform, 8 key pairs public/private are supported
    SIGN_KEY_PUB_M="\\
        \${your_absolute_key_path}/a35_OEM-KEY1_publicKey00.pem \\
        \${your_absolute_key_path}/a35_OEM-KEY1_publicKey01.pem \\
        \${your_absolute_key_path}/a35_OEM-KEY1_publicKey02.pem \\
        \${your_absolute_key_path}/a35_OEM-KEY1_publicKey03.pem \\
        \${your_absolute_key_path}/a35_OEM-KEY1_publicKey04.pem \\
        \${your_absolute_key_path}/a35_OEM-KEY1_publicKey05.pem \\
        \${your_absolute_key_path}/a35_OEM-KEY1_publicKey06.pem \\
        \${your_absolute_key_path}/a35_OEM-KEY1_publicKey07.pem \\
    "
    # Password (key pass) for all signature key
    # NOTE:
    # for stm32mp2 platform, up to 8 keys can be requested (means put all password according to key)
    SIGN_KEY_PASS_M="${PASS_KEY1}"

    # Password key associated to private key selected via SIGN_KEY
    SIGN_KEY_PASS_M_FOR_SPECIFC_KEY="${first_pass_key1}"

    # ------ Key associated to EDMK1${encrypt_key_fsbl_token}------------
    # encryption key use to decrypt Bl2 (TF-A) (128bits)
    ENCRYPT_FSBL_M_KEY="\${your_absolute_key_path}/a35_EDMK1${encrypt_key_fsbl_token}_key_128bits.bin"

else
    # USING two separate key, one for Cortex A35 one for CortexM33
    # Private Key for signature (absolute path to key)
    SIGN_KEY_M="\${your_absolute_key_path}/m33_OEM-KEY2_privateKey00.pem"
    # List of Public key for signature
    # NOTE:
    # for stm32mp2 platform, 8 key pairs public/private are supported
    SIGN_KEY_PUB_M="\\
        \${your_absolute_key_path}/m33_OEM-KEY2_publicKey00.pem \\
        \${your_absolute_key_path}/m33_OEM-KEY2_publicKey01.pem \\
        \${your_absolute_key_path}/m33_OEM-KEY2_publicKey02.pem \\
        \${your_absolute_key_path}/m33_OEM-KEY2_publicKey03.pem \\
        \${your_absolute_key_path}/m33_OEM-KEY2_publicKey04.pem \\
        \${your_absolute_key_path}/m33_OEM-KEY2_publicKey05.pem \\
        \${your_absolute_key_path}/m33_OEM-KEY2_publicKey06.pem \\
        \${your_absolute_key_path}/m33_OEM-KEY2_publicKey07.pem \\
    "
    # Password (key pass) for all signature key
    # NOTE:
    # for stm32mp2 platform, up to 8 keys can be requested (means put all password according to key)
    SIGN_KEY_PASS_M="${PASS_KEY2}"

    # Password key associated to private key selected via SIGN_KEY
    SIGN_KEY_PASS_M_FOR_SPECIFC_KEY="${first_pass_key2}"

    # ------ Key associated to EDMK2${encrypt_key_fsbl_token}------------
    # encryption key use to decrypt Bl2 (TF-A) (128bits)
    ENCRYPT_FSBL_M_KEY="\${your_absolute_key_path}/m33_EDMK2${encrypt_key_fsbl_token}_key_128bits.bin"

fi

# ------ Key associated to FSBLM-DDR-FW-PKH ---------------------
SIGN_M33DDR_KEY="\${your_absolute_key_path}/m33_FSBLM-DDR-FW-PKH_privateKey.pem"
SIGN_M33DDR_KEY_PASS="${PASS_M33_DDR}"

# ------ Key associated to FSBLM-M33-FW-PKH ---------------------
SIGN_M33FW_KEY="\${your_absolute_key_path}/m33_FSBLM-M33-FW-PKH_privateKey.pem"
SIGN_M33FW_KEY_PASS="${PASS_M33_FW}"

# ------ Key associated to FIP-EDMK------------
# encryption key use to decrypt FIP component (256bits)
ENCRYPT_FIP_KEY="\${your_absolute_key_path}/a35_FIP-EDMK_key_256bits.bin"

# ------ Key associated to RPROC-FW-PKH for coprocessor signature----------
# Type of key use for coprocessor signature:
# (256bits symmetric key)
#  RSA: Asymetric cryptography algorithm
#  ECC: Elliptic curve cryptography
SIGN_COPRO_TYPE=ecc

# For RSA KEY
SIGN_COPRO_RSA_INFOKEY=""
SIGN_COPRO_RSA_PRIVKEY=""
SIGN_COPRO_RSA_PASS=""

# For ECC KEY
SIGN_COPRO_ECC_INFOKEY="\${your_absolute_key_path}/m33_RPROC-FW-PKH_ecc_publicKey.der"
SIGN_COPRO_ECC_PRIVKEY="\${your_absolute_key_path}/m33_RPROC-FW-PKH_ecc_privateKey.pem"
SIGN_COPRO_ECC_PASS="${PASS_RPROC}"

# ------ Key associated to RPROC-FW-ENC-KEY for coprocessor encryption-------
# Encryption key (256bits)
ENCRYPT_COPRO_KEY="\${your_absolute_key_path}/m33_RPROC-FW-ENC-KEY_key_256bits.bin"

##################################################################
##################################################################
### End of Signature / Encryption configuration
##################################################################
##################################################################
EOF
}
# -------------------------------------
# STM32MP23 / STM32MP25
function create_encryption_key_for_stm32mp25() {
    output="$1"
    encryption_key a35_EDMK1_key_128bits.bin 16 "${output}"
    encryption_key a35_FIP-EDMK_key_256bits.bin 32 "${output}"
    encryption_key a35_RPROC-FW-ENC-KEY_key_256bits.bin 32 "${output}"
    if [ $M33 -eq 1 ]; then
        encryption_key m33_EDMK2_key_128bits.bin 16 "${output}"
    fi
}

function create_key() {
    output="$1"
    pass_key1="$2"
    pass_key2="$3"
    pass_rproc="$4"
    pass_m33_ddr="$5"
    pass_m33_fw="$5"

    case ${MACHINE} in
    stm32mp13)
        create_signature_key_for_stm32mp13 "${output}" "${pass_key1}"
        create_encryption_key_for_stm32mp13 "${output}"
        dump_key_stm32mp13 "${output}"
        dump_key_for_distribution_stm32mp13 "${output}"
        ;;
    stm32mp15)
        create_signature_key_for_stm32mp15 "${output}" "${pass_key1}"
        dump_key_stm32mp15 "${output}"
        dump_key_for_distribution_stm32mp15  "${output}"
        ;;
    stm32mp21)
        create_signature_key_for_stm32mp2 "${output}" "${pass_key1}" "${pass_key2}" "${pass_rproc}" "${pass_m33_ddr}" "${pass_m33_fw}"
        create_encryption_key_for_stm32mp21 "${output}"
        dump_key_stm32mp2 "${output}"
        dump_key_for_distribution_stm32mp2 "${output}"
        [ $M33 -eq 0 ] || dump_key_for_distribution_stm32mp2_m33  "${output}"
        ;;
    stm32mp23)
        create_signature_key_for_stm32mp2 "${output}" "${pass_key1}" "${pass_key2}" "${pass_rproc}" "${pass_m33_ddr}" "${pass_m33_fw}"
        create_encryption_key_for_stm32mp25 "${output}"
        dump_key_stm32mp2 "${output}"
        dump_key_for_distribution_stm32mp2 "${output}"
        [ $M33 -eq 0 ] || dump_key_for_distribution_stm32mp2_m33 "${output}"
        ;;
    stm32mp25)
        create_signature_key_for_stm32mp2 "${output}" "${pass_key1}" "${pass_key2}" "${pass_rproc}" "${pass_m33_ddr}" "${pass_m33_fw}"
        create_encryption_key_for_stm32mp25 "${output}"
        dump_key_stm32mp2 "${output}"
        dump_key_for_distribution_stm32mp2 "${output}"
        [ $M33 -eq 0 ] || dump_key_for_distribution_stm32mp2_m33 "${output}"
        ;;
    esac
}

# ------------------------------------------
# test
# ------------------------------------------
function check_pass() {
    name="$1"
    data="$2"
    if [ -z "${data}" ]; then
        echo ""
        echo "[ERROR]: You must specify the signature key password for ${name}"
        echo ""
        usage
        exit 5
    fi
}
# ------------------------------------------
# Main
# ------------------------------------------
# parse argument
if [ $# -eq 0 ]; then
    usage
    exit 1
fi
while [[ $# -gt 0 ]]; do
    case $1 in
    help|--help|-h)
        usage
        exit 0
        ;;
    stm32mp13)
        MACHINE=stm32mp13
        M33=0
        shift
        ;;
    stm32mp15)
        MACHINE=stm32mp15
        M33=0
        shift;
        ;;
    stm32mp21)
        MACHINE=stm32mp21
        shift
        ;;
    stm32mp23)
        MACHINE=stm32mp23
        shift
        ;;
    stm32mp25)
        MACHINE=stm32mp25
        shift
        ;;
   --m33|-m33)
        M33=1
        shift
        ;;
   --output|-o)
        shift
        OUTPUT=$1
        shift
        ;;
   --passkey1|-pk1)
        shift
        tmp_data=$1
        if [ "${tmp_data:0:1}" = "-" ]; then
            PASS_KEY1=""
        else
            while [ ! "${tmp_data:0:1}" = "-" ]; do
                PASS_KEY1="${PASS_KEY1} ${tmp_data}"
                shift
                if [ $# -gt 0 ]; then
                    tmp_data=$1
                else
                    break
                fi
            done
        fi
        ;;
   --passkey2|-pk2)
        shift
        tmp_data=$1
        if [ "${tmp_data:0:1}" = "-" ]; then
            PASS_KEY2=""
        else
            while [ ! "${tmp_data:0:1}" = "-" ]; do
                PASS_KEY2="${PASS_KEY2} ${tmp_data}"
                shift
                if [ $# -gt 0 ]; then
                    tmp_data=$1
                else
                    break
                fi
           done
        fi
        ;;
   --passrproc|-prproc)
        shift
        tmp_data=$1
        if [ ! "${tmp_data:0:1}" = "-" ]; then
           PASS_RPROC=$1
           shift
        else
            PASS_RPROC=""
        fi
       ;;
   --passm33ddr|-pddr)
        shift
        tmp_data=$1
        if [ ! "${tmp_data:0:1}" = "-" ]; then
           PASS_M33_DDR=$1
           shift
        else
            PASS_M33_DDR=""
        fi
        ;;
   --passm33fw|-pfw)
        shift
        tmp_data=$1
        if [ ! "${tmp_data:0:1}" = "-" ]; then
           PASS_M33_FW=$1
           shift
        else
            PASS_M33_FW=""
        fi
        ;;
    *)
        echo ""
        echo "[ERROR] unknown parameter: $@"
        usage
        exit 1
        ;;
  esac
done

# test output
if [ -z "${OUTPUT}" ]; then
    echo "[ERROR]: You must specify an output directory n which the kye will be stored"
    usage
    exit 2
fi
if [ -z "${MACHINE}" ]; then
    echo "[ERROR]: You must specify an soc name"
    usage
    exit 3
fi
# check password
case ${MACHINE} in
stm32mp13)
    PASS_KEY1=$(echo "${PASS_KEY1}" | sed 's/^[[:space:]]*//')
    check_pass "key1" "${PASS_KEY1}"
    ;;
stm32mp15)
    PASS_KEY1=$(echo "${PASS_KEY1}" | sed 's/^[[:space:]]*//')
    check_pass "key1" "${PASS_KEY1}"
    ;;
stm32mp21|stm32mp23|stm32mp25)
    PASS_KEY1=$(echo "${PASS_KEY1}" | sed 's/^[[:space:]]*//')
    PASS_RPROC=$(echo "${PASS_RPROC}" | sed 's/^[[:space:]]*//')
    check_pass "key1" "${PASS_KEY1}"
    check_pass "rproc" "${PASS_RPROC}"
    if [ $M33 -eq 1 ]; then
        PASS_KEY2=$(echo "${PASS_KEY2}" | sed 's/^[[:space:]]*//')
        PASS_M33_DDR=$(echo "${PASS_M33_DDR}" | sed 's/^[[:space:]]*//')
        PASS_M33_FW=$(echo "${PASS_M33_FW}" | sed 's/^[[:space:]]*//')
        check_pass "key2" "${PASS_KEY2}"
        check_pass "M33 ddr" "${PASS_M33_DDR}"
        check_pass "M33 fw" "${PASS_M33_FW}"
    fi
    ;;
esac
# check output
if [ ! -d "${OUTPUT}" ]; then
    echo "[INFO]: The output directory doesn't exist"
    echo "[INFO]:    ${OUTPUT}"
    echo "[INFO]: Would you like to create it: [Y/n]"
        read -r answer
        if [[ "$answer" =~ ^[Yy]+[ESes]* ]]; then
            mkdir -p "${OUTPUT}/"
        else
            usage
            exit 4
        fi
fi
# test KEY_GEN
if [ -n ${TOOLS_KEYGEN} ]; then
    ${TOOLS_KEYGEN} --version 2> /dev/null > /dev/null
    if [ $? -gt 0 ]; then
        echo "[ERROR] you must specify the valid path to STM32_KeyGen_CLI via KEY_GEN"
        usage
        exit 5
    fi
else
    echo ""
    echo "[ERROR] you must specify the path to STM32_KeyGen_CLI via KEY_GEN"
    usage
    exit 6
fi
# test openssl
openssl version  2> /dev/null > /dev/null
if [ $? -gt 0 ]; then
    echo ""
    echo "[ERROR] you must have openssl tools available on your PATH"
    echo "[ERROR]   'openssl ec' command is used by this script"
    echo "[ERROR] Please add 'openssl' your PATH"
    exit 7
fi

dump_parameters
create_key "${OUTPUT}" "${PASS_KEY1}" "${PASS_KEY2}" "${PASS_RPROC}" "${PASS_M33_DDR}" "${PASS_M33_FW}"
