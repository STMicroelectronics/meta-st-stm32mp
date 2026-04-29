EXTERNAL_KEY_CONF ??= "0"

ENCRYPT_ENABLE ??= "0"
ENCRYPT_FIP_KEY ??= ""
ENCRYPT_FSBL_KEY ??= ""
ENCRYPT_OFFSET ??= "0x80000003"
ENCRYPT_SUFFIX ??= "_Encrypted"

ENCRYPT_COPRO_ENABLE ??= "0"
ENCRYPT_COPRO_KEY ??= ""

SIGN_ENABLE ??= "0"
SIGN_HEADER_VERSION ??= ""
SIGN_KEY ??= ""
SIGN_KEY_PASS ??= ""
SIGN_KEY_PUB ??= ""
SIGN_M33DDR_KEY ??= ""
SIGN_M33DDR_KEY_PASS ??= ""
SIGN_M33FW_KEY ??= ""
SIGN_M33FW_KEY_PASS ??= ""
SIGN_OFFSET ??= "0x00000001"
SIGN_SUFFIX ??= "_Signed"

SIGN_COPRO_DEFAULT ??= "0"
SIGN_COPRO_ENABLE ??= "0"
SIGN_COPRO_ECC ??= "1"
SIGN_COPRO_ECC_PRIVKEY ??= ""
SIGN_COPRO_ECC_INFOKEY ??= ""
SIGN_COPRO_ECC_PASS ??= ""
SIGN_COPRO_RSA ??= "0"
SIGN_COPRO_RSA_PRIVKEY ??= ""
SIGN_COPRO_RSA_INFOKEY ??= ""

SIGN_TOOL ??= ""

def search_path(file_search, d):
    """
    Check for <file_search> path availability from BBPATH
    And return the <file_search> absolute path
    """
    # Init file search list
    file_list = file_search.split()
    file_path_list = []
    # Run search from BBPATH
    search_path = d.getVar("BBPATH").split(":")
    search_count = 0
    for p in search_path:
        for item in file_list:
            file_path = os.path.join(p, item)
            if os.path.isfile(file_path):
                file_path_list.append(file_path)
                search_count += 1
        if search_count == len(file_list):
            break
    if search_count == len(file_list):
        return " ".join(file_path_list)
    bbpaths = d.getVar('BBPATH').replace(':','\n\t')
    bb.fatal('\n[sign-stm32mp] Not able to find "%s" path from current BBPATH var:\n\t%s.' % (file_search, bbpaths))

def init_keylist_from(keylist, keyinput, soclist, d, getPath='1'):
    """
    Build the <keylist> var as a coma separated list of values,
    Using either the default <keyinput> var value
    or any defined <keyinput>_socname var value
    (with 'socname' item comming from <soclist> var value list)
    """
    # Init path resolution
    if getPath == '1' and d.getVar('EXTERNAL_KEY_CONF') == '1':
        resolve_path = '1'
    else:
        resolve_path = '0'
    # Init soc name list
    socname_list = (d.getVar(soclist) or "").split()
    # Init key from keyinput var value
    key = d.getVar(keyinput) or ""
    if key:
        # Check first if keyinput_<soc> is defined to use it
        if len(socname_list) > 0:
            # Configure keylist according to <soclist> list
            d.setVar(keylist, '')
            for socname in socname_list:
                key_soc = d.getVar(keyinput + '_' + socname) or ""
                if key_soc:
                    if resolve_path == '1':
                        key_soc = search_path(key_soc, d)
                    bb.debug(1, "[sign-stm32mp] Append '%s' path to %s (socname %s)." % (key_soc, keylist, socname))
                    d.appendVar(keylist, key_soc + ',')
                else:
                    if resolve_path == '1':
                        key = search_path(key, d)
                    bb.debug(1, "[sign-stm32mp] Append generic '%s' path to %s (socname %s)." % (key, keylist, socname))
                    d.appendVar(keylist, key + ',')
        else:
            # Default to keyinput value setting
            if resolve_path == '1':
                key = search_path(key, d)
            bb.debug(1, "[sign-stm32mp] Set %s to '%s' path." % (keylist, key))
            d.setVar(keylist, key)
    else:
        # Check first if keyinput_<soc> is defined to use it
        if len(socname_list) > 0:
            # Configure keylist according to STM32MP_SOC_NAME list
            d.setVar(keylist, '')
            for socname in socname_list:
                key = d.getVar(keyinput + '_' + socname)
                if key:
                    if resolve_path == '1':
                        key = search_path(key, d)
                    bb.debug(1, "[sign-stm32mp] Append '%s' path to %s (socname %s)." % (key, keylist, socname))
                    d.appendVar(keylist, key + ',')
                else:
                    bb.fatal("[sign-stm32mp] Please make sure to configure \"%s_%s\" var to key file." % (keyinput, socname))
        else:
            bb.fatal("[sign-stm32mp] Please make sure to configure \"%s\" var to key file." % keyinput)

python __anonymous() {
    # Check for configuration needs
    if d.getVar('SIGN_ENABLE') == "0" and d.getVar('SIGN_COPRO_ENABLE') == "0" and d.getVar('ENCRYPT_ENABLE') == "0" and d.getVar('ENCRYPT_COPRO_ENABLE') == "0":
        return

    # Signing process is dedicated to "target" recipe only:
    # Make sure to discard native and nativesdk
    for native_class in ['native', 'nativesdk']:
        if bb.data.inherits_class(native_class, d):
            return

    # Check for SIGN_TOOL configuration
    signtool = d.getVar('SIGN_TOOL') or ""
    if not signtool:
        bb.fatal("[sign-stm32mp] Please make sure to configure \"SIGN_TOOL\" var to signing tool.")
    # Check for SIGN_TOOL is present in PATH environment variable
    if not bb.utils.which(d.getVar('PATH'), signtool):
        bb.debug(1, "[sign-stm32mp] %s binary is not found in PATH." % signtool)
        signtool_path = search_path(signtool, d)
        bb.debug(1, "[sign-stm32mp] Set SIGN_TOOL to '%s' path." % signtool_path)
        d.setVar('SIGN_TOOL', signtool_path)

    if d.getVar('SIGN_ENABLE') == "1":
        # Check for internal use of SIGN_HEADER_VERSION_LIST
        if d.getVar('SIGN_HEADER_VERSION_LIST'):
            raise bb.parse.SkipRecipe("[sign-stm32mp] You cannot use SIGN_HEADER_VERSION_LIST as it is internal to sign-stm32mp.bbclass.")
        # Init SIGN_HEADER_VERSION_LIST from SIGN_HEADER_VERSION settings
        init_keylist_from('SIGN_HEADER_VERSION_LIST', 'SIGN_HEADER_VERSION', 'STM32MP_SOC_NAME', d, getPath='0')

        # Check for internal use of SIGN_KEY_PATH_LIST
        if d.getVar('SIGN_KEY_PATH_LIST'):
            raise bb.parse.SkipRecipe("[sign-stm32mp] You cannot use SIGN_KEY_PATH_LIST as it is internal to sign-stm32mp.bbclass.")
        # Init SIGN_KEY_PATH_LIST from SIGN_KEY settings
        init_keylist_from('SIGN_KEY_PATH_LIST', 'SIGN_KEY', 'STM32MP_SOC_NAME', d)

        # Check for internal use of SIGN_KEY_PUB_PATH_LIST
        if d.getVar('SIGN_KEY_PUB_PATH_LIST'):
            raise bb.parse.SkipRecipe("[sign-stm32mp] You cannot use SIGN_KEY_PUB_PATH_LIST as it is internal to sign-stm32mp.bbclass.")
        # Init SIGN_KEY_PUB_PATH_LIST from SIGN_KEY_PUB settings
        init_keylist_from('SIGN_KEY_PUB_PATH_LIST', 'SIGN_KEY_PUB', 'STM32MP_SOC_NAME', d)

        # Check for internal use of SIGN_KEY_PASS_LIST
        if d.getVar('SIGN_KEY_PASS_LIST'):
            raise bb.parse.SkipRecipe("[sign-stm32mp] You cannot use SIGN_KEY_PASS_LIST as it is internal to sign-stm32mp.bbclass.")
        # Init SIGN_KEY_PASS_LIST from SIGN_KEY settings
        init_keylist_from('SIGN_KEY_PASS_LIST', 'SIGN_KEY_PASS', 'STM32MP_SOC_NAME', d, getPath='0')

        if 'm33td' in d.getVar('MACHINE_FEATURES').split() or 'm33copro' in  d.getVar('MACHINE_FEATURES').split():
            if d.getVar('SIGN_M33DDR_KEY_PATH_LIST'):
                raise bb.parse.SkipRecipe("[sign-stm32mp] You cannot use SIGN_M33DDR_KEY_PATH_LIST as it is internal to sign-stm32mp.bbclass.")
            # Init SIGN_M33DDR_KEY_PATH_LIST from SIGN_M33DDR_KEY settings
            init_keylist_from('SIGN_M33DDR_KEY_PATH_LIST', 'SIGN_M33DDR_KEY', 'STM32MP_SOC_NAME', d)

            if d.getVar('SIGN_M33DDR_KEY_PASS_LIST'):
                raise bb.parse.SkipRecipe("[sign-stm32mp] You cannot use SIGN_M33DDR_KEY_PASS_LIST as it is internal to sign-stm32mp.bbclass.")
            # Init SIGN_M33DDR_KEY_PASS_LIST from SIGN_M33DDR_KEY_PASS settings
            init_keylist_from('SIGN_M33DDR_KEY_PASS_LIST', 'SIGN_M33DDR_KEY_PASS', 'STM32MP_SOC_NAME', d, getPath='0')

            if d.getVar('SIGN_M33FW_KEY_PATH_LIST'):
                raise bb.parse.SkipRecipe("[sign-stm32mp] You cannot use SIGN_M33FW_KEY_PATH_LIST as it is internal to sign-stm32mp.bbclass.")
            # Init SIGN_M33FW_KEY_PATH_LIST from SIGN_M33FW_KEY settings
            init_keylist_from('SIGN_M33FW_KEY_PATH_LIST', 'SIGN_M33FW_KEY', 'STM32MP_SOC_NAME', d)

            if d.getVar('SIGN_M33FW_KEY_PASS_LIST'):
                raise bb.parse.SkipRecipe("[sign-stm32mp] You cannot use SIGN_M33FW_KEY_PASS_LIST as it is internal to sign-stm32mp.bbclass.")
            # Init SIGN_M33FW_KEY_PASS_LIST from SIGN_M33FW_KEY_PASS settings
            init_keylist_from('SIGN_M33FW_KEY_PASS_LIST', 'SIGN_M33FW_KEY_PASS', 'STM32MP_SOC_NAME', d, getPath='0')

        # If signature are activated, for winning space, the debug parameter will be remove and level of trace decrease
        if (d.getVar('ST_TF_A_DEBUG_TRACE') or "0") == '1':
            # Reduce configuration to TF-A recipe only
            if (d.getVar('PN') or "") == 'tf-a-stm32mp':
                bb.warn("TF-A SIGNATURE: force ST_TF_A_DEBUG_TRACE to '0' to disable DEBUG and decrease log level")
                d.setVar('ST_TF_A_DEBUG_TRACE', "0")

    if d.getVar('SIGN_COPRO_ENABLE') == "1":
        if d.getVar('SIGN_COPRO_ECC') == "0" and d.getVar('SIGN_COPRO_RSA') == "0":
            bb.fatal("[sign-stm32mp] You need to set 'SIGN_COPRO_ECC = 1' or 'SIGN_COPRO_RSA = 1' to configure signature settings.")

        if d.getVar('SIGN_COPRO_ECC') == "1":
            # Check for internal use of SIGN_COPRO_ECC_PRIV_KEY_PATH_LIST
            if d.getVar('SIGN_COPRO_ECC_PRIV_KEY_PATH_LIST'):
                raise bb.parse.SkipRecipe("[sign-stm32mp] You cannot use SIGN_COPRO_ECC_PRIV_KEY_PATH_LIST as it is internal to sign-stm32mp.bbclass.")
            # Init SIGN_COPRO_ECC_PRIV_KEY_PATH_LIST from SIGN_COPRO_ECC_PRIVKEY settings
            init_keylist_from('SIGN_COPRO_ECC_PRIV_KEY_PATH_LIST', 'SIGN_COPRO_ECC_PRIVKEY', 'STM32MP_ENCRYPT_COPRO_SOC_NAME', d)
            # Check for internal use of SIGN_COPRO_ECC_INFO_KEY_PATH_LIST
            if d.getVar('SIGN_COPRO_ECC_INFO_KEY_PATH_LIST'):
                raise bb.parse.SkipRecipe("[sign-stm32mp] You cannot use SIGN_COPRO_ECC_INFO_KEY_PATH_LIST as it is internal to sign-stm32mp.bbclass.")
            # Init SIGN_COPRO_ECC_INFO_KEY_PATH_LIST from SIGN_COPRO_ECC_INFOKEY settings
            init_keylist_from('SIGN_COPRO_ECC_INFO_KEY_PATH_LIST', 'SIGN_COPRO_ECC_INFOKEY', 'STM32MP_ENCRYPT_COPRO_SOC_NAME', d)
            # Check for internal use of SIGN_COPRO_ECC_PASS_KEY_LIST
            if d.getVar('SIGN_COPRO_ECC_PASS_KEY_LIST'):
                raise bb.parse.SkipRecipe("[sign-stm32mp] You cannot use SIGN_COPRO_ECC_PASS_KEY_LIST as it is internal to sign-stm32mp.bbclass.")
             # Init SIGN_COPRO_ECC_PASS_KEY_LIST from SIGN_COPRO_ECC_PASS settings
            init_keylist_from('SIGN_COPRO_ECC_PASS_KEY_LIST', 'SIGN_COPRO_ECC_PASS', 'STM32MP_ENCRYPT_COPRO_SOC_NAME', d, getPath='0')

        if d.getVar('SIGN_COPRO_RSA') == "1":
            # Check for internal use of SIGN_COPRO_RSA_PRIV_KEY_PATH_LIST
            if d.getVar('SIGN_COPRO_RSA_PRIV_KEY_PATH_LIST'):
                raise bb.parse.SkipRecipe("[sign-stm32mp] You cannot use SIGN_COPRO_RSA_PRIV_KEY_PATH_LIST as it is internal to sign-stm32mp.bbclass.")
            # Init SIGN_COPRO_RSA_PRIV_KEY_PATH_LIST from SIGN_COPRO_RSA_PRIVKEY settings
            init_keylist_from('SIGN_COPRO_RSA_PRIV_KEY_PATH_LIST', 'SIGN_COPRO_RSA_PRIVKEY', 'STM32MP_ENCRYPT_COPRO_SOC_NAME', d)
            # Check for internal use of SIGN_COPRO_RSA_INFO_KEY_PATH_LIST
            if d.getVar('SIGN_COPRO_RSA_INFO_KEY_PATH_LIST'):
                raise bb.parse.SkipRecipe("[sign-stm32mp] You cannot use SIGN_COPRO_RSA_INFO_KEY_PATH_LIST as it is internal to sign-stm32mp.bbclass.")
            # Init SIGN_COPRO_RSA_INFO_KEY_PATH_LIST from SIGN_COPRO_RSA_INFOKEY settings
            init_keylist_from('SIGN_COPRO_RSA_INFO_KEY_PATH_LIST', 'SIGN_COPRO_RSA_INFOKEY', 'STM32MP_ENCRYPT_COPRO_SOC_NAME', d)

    if d.getVar('ENCRYPT_ENABLE') == "1":
        if d.getVar('SIGN_ENABLE') == "0":
            bb.fatal("[sign-stm32mp] You need to set 'SIGN_ENABLE = 1' to encrypt and sign binaries at once.")

        # Check for internal use of ENCRYPT_FSBL_KEY_PATH_LIST
        if d.getVar('ENCRYPT_FSBL_KEY_PATH_LIST'):
            raise bb.parse.SkipRecipe("[sign-stm32mp] You cannot use ENCRYPT_FSBL_KEY_PATH_LIST as it is internal to sign-stm32mp.bbclass.")
        # Init ENCRYPT_FSBL_KEY_PATH_LIST from ENCRYPT_FSBL_KEY settings
        init_keylist_from('ENCRYPT_FSBL_KEY_PATH_LIST', 'ENCRYPT_FSBL_KEY', 'STM32MP_ENCRYPT_SOC_NAME', d)

        # Check for internal use of ENCRYPT_FIP_KEY_PATH_LIST
        if d.getVar('ENCRYPT_FIP_KEY_PATH_LIST'):
            raise bb.parse.SkipRecipe("[sign-stm32mp] You cannot use ENCRYPT_FIP_KEY_PATH_LIST as it is internal to sign-stm32mp.bbclass.")
        # Init ENCRYPT_FIP_KEY_PATH_LIST from ENCRYPT_FIP_KEY settings
        init_keylist_from('ENCRYPT_FIP_KEY_PATH_LIST', 'ENCRYPT_FIP_KEY', 'STM32MP_ENCRYPT_SOC_NAME', d)

    if d.getVar('ENCRYPT_COPRO_ENABLE') == "1":
        # Check for internal use of ENCRYPT_COPRO_KEY_PATH_LIST
        if d.getVar('ENCRYPT_COPRO_KEY_PATH_LIST'):
            raise bb.parse.SkipRecipe("[sign-stm32mp] You cannot use ENCRYPT_COPRO_KEY_PATH_LIST as it is internal to sign-stm32mp.bbclass.")
        # Init ENCRYPT_COPRO_KEY_PATH_LIST from ENCRYPT_COPRO_KEY settings
        init_keylist_from('ENCRYPT_COPRO_KEY_PATH_LIST', 'ENCRYPT_COPRO_KEY', 'STM32MP_ENCRYPT_SOC_NAME', d)
}

#
# Handle sign and encrypt process
#
sign_stm32mp_fw() {
    local input_path=$1
    local input_type=$2
    local soc_conf=$3
    local output=$4
    # Configure signature key settings
    local offset=""
    local pub_sign_key=""
    local prv_sign_key=""
    local pwd_sign_key=""
    if [ "${SIGN_ENABLE}" = "1" ]; then
        offset="${SIGN_OFFSET}"
        local k=0
        for soc in ${STM32MP_SOC_NAME}; do
            k=$(expr $k + 1)
            if [ "${soc}" = "${soc_conf}" ] ; then
                header_version=$(echo ${SIGN_HEADER_VERSION_LIST} | cut -d',' -f${k})
                pub_sign_key=$(echo ${SIGN_KEY_PUB_PATH_LIST} | cut -d',' -f${k})
                prv_sign_key=$(echo ${SIGN_KEY_PATH_LIST} | cut -d',' -f${k})
                pwd_sign_key=$(echo ${SIGN_KEY_PASS_LIST} | cut -d',' -f${k})
                break
            fi
        done
    fi
#    # Configure encrypt key settings
#    if [ "${ENCRYPT_ENABLE}" = "1" ]; then
#        offset="${ENCRYPT_OFFSET}"
#    fi

    if [ -n "${prv_sign_key}" ]; then
        bbnote "[sign_stm32mp_fw] Generate sign firwmare:"
        echo "[SIGNING CMD] ${SIGN_TOOL} \\
            -bin ${input_path} \\
            -o ${output} \\
            -of ${offset} \\
            --public-key ${pub_sign_key} \\
            --password ${pwd_sign_key} \\
            --private-key ${prv_sign_key} \\
            --header-version ${header_version} \\
            --type ${input_type} \\
            --silent"
        ${SIGN_TOOL} \
            -bin "${input_path}" \
            -o "${output}" \
            -of ${offset} \
            --public-key ${pub_sign_key} \
            --password ${pwd_sign_key} \
            --private-key ${prv_sign_key} \
            --header-version ${header_version} \
            --type ${input_type} \
            --silent
    fi
}

#
# Handle copro firmware signature process
#
DEPENDS += "${@bb.utils.contains('MACHINE_FEATURES', 'm33copro', 'optee-os-stm32mp-scripts-native', '', d)}"

SIGN_COPRO_TA_DEV_KIT_DIR ?= "${STAGING_DATADIR_NATIVE}/optee"
SIGN_COPRO_TOOL           ?= "${STAGING_BINDIR_NATIVE}/st_copro_firmware_signature.sh"

sign_copro_fw_m33() {
    local soc_conf=$1
    local output=$2
    local inputns=$3
    local inputs=$4
    # Configure input settings
    local input_nsecure=""
    local input_secure=""
    [ -z "${inputns}" ] || input_nsecure="--input-nsecure ${inputns}"
    [ -z "${inputs}" ] || input_secure="--input-secure ${inputs}"
    # Configure signature key settings
    local m33_priv_ecc_sign_key=""
    local m33_info_ecc_sign_key=""
    local m33_pass_ecc_sign_key=""
    local m33_priv_rsa_sign_key=""
    local m33_info_rsa_sign_key=""
    if [ "${SIGN_COPRO_ENABLE}" = "1" ]; then
        local k=0
        for soc in ${STM32MP_SOC_NAME}; do
            k=$(expr $k + 1)
            if [ "${soc}" = "${soc_conf}" ] ; then
                m33_priv_ecc_sign_key=$(echo ${SIGN_COPRO_ECC_PRIV_KEY_PATH_LIST} | cut -d',' -f${k})
                m33_info_ecc_sign_key=$(echo ${SIGN_COPRO_ECC_INFO_KEY_PATH_LIST} | cut -d',' -f${k})
                m33_pass_ecc_sign_key=$(echo ${SIGN_COPRO_ECC_PASS_KEY_LIST} | cut -d',' -f${k})
                m33_priv_rsa_sign_key=$(echo ${SIGN_COPRO_RSA_PRIV_KEY_PATH_LIST} | cut -d',' -f${k})
                m33_info_rsa_sign_key=$(echo ${SIGN_COPRO_RSA_INFO_KEY_PATH_LIST} | cut -d',' -f${k})
                break
            fi
        done
    fi
    # Configure encrypt key settings
    local m33_encrypt_key=""
    if [ "${ENCRYPT_COPRO_ENABLE}" = "1" ]; then
        local l=0
        for soc in ${STM32MP_ENCRYPT_SOC_NAME}; do
            l=$(expr $l + 1)
            if [ "${soc}" = "${soc_conf}" ] ; then
                m33_encrypt_key=$(echo ${ENCRYPT_COPRO_KEY_PATH_LIST} | cut -d',' -f${l})
                break
            fi
        done
    fi
    # Override default env for SIGN_COPRO_TOOL
    [ -z "${SIGN_COPRO_TA_DEV_KIT_DIR}" ] || export TA_DEV_KIT_DIR="${SIGN_COPRO_TA_DEV_KIT_DIR}"

    bbnote "[sign_copro_fw_m33] Generate sign firwmare:"
    if [ -n "${m33_priv_ecc_sign_key}" ]; then
        bbnote "[sign_copro_fw_m33] Use ECC signature"
        ${SIGN_COPRO_TOOL} \
                ${input_nsecure} \
                ${input_secure} \
                --signature-key ${m33_priv_ecc_sign_key} --sign-info-key ${m33_info_ecc_sign_key} --sign-pass ${m33_pass_ecc_sign_key} \
                --sign-ecc \
                --output ${output} || bbwarn "[sign_copro_fw_m33][ECC key] Failed to generate $(basename "${output}")"
        if [ -n "${m33_encrypt_key}" ]; then
            bbnote "[sign_copro_fw_m33] Use ECC key and encrypt firwmare"
            ${SIGN_COPRO_TOOL} \
                    ${input_nsecure} \
                    ${input_secure} \
                    --signature-key ${m33_priv_ecc_sign_key} --sign-info-key ${m33_info_ecc_sign_key} --sign-pass ${m33_pass_ecc_sign_key} \
                    --sign-ecc \
                    --encrypt-key ${m33_encrypt_key} \
                    --output ${output} || bbwarn "[sign_copro_fw_m33][ECC key and encrypt] Failed to generate $(basename "${output}")"
        fi
    fi
    if [ -n "${m33_priv_rsa_sign_key}" ]; then
        bbnote "[sign_copro_fw_m33] Use RSA signature"
        ${SIGN_COPRO_TOOL} \
                ${input_nsecure} \
                ${input_secure} \
                --signature-key ${m33_priv_rsa_sign_key} --sign-info-key ${m33_info_rsa_sign_key} \
                --sign-rsa \
                --output ${output} || bbwarn "[sign_copro_fw_m33][RSA key] Failed to generate $(basename "${output}")"
        if [ -n "${m33_encrypt_key}" ]; then
            bbnote "[sign_copro_fw_m33] Use RSA key and encrypt firwmare"
            ${SIGN_COPRO_TOOL} \
                    ${input_nsecure} \
                    ${input_secure} \
                    --signature-key ${m33_priv_rsa_sign_key} --sign-info-key ${m33_info_rsa_sign_key} \
                    --sign-rsa \
                    --encrypt-key ${m33_encrypt_key} \
                    --output ${output} || bbwarn "[sign_copro_fw_m33][RSA key and encrypt] Failed to generate $(basename "${output}")"
        fi
    fi
    if [ "${SIGN_COPRO_ENABLE}" = "0" ] || [ "${SIGN_COPRO_DEFAULT}" = "1" ]; then
        bbnote "[sign_copro_fw_m33] Use default signature"
        ${SIGN_COPRO_TOOL} \
                ${input_nsecure} \
                ${input_secure} \
                --output ${output} || bbwarn "[sign_copro_fw_m33][default] Failed to generate $(basename "${output}")"
        if [ -n "${m33_encrypt_key}" ]; then
            bbnote "[sign_copro_fw_m33] Use default key and encrypt firwmare"
            ${SIGN_COPRO_TOOL} \
                    ${input_nsecure} \
                    ${input_secure} \
                    --encrypt-key ${m33_encrypt_key} \
                    --output ${output} || bbwarn "[sign_copro_fw_m33][default key and encrypt] Failed to generate $(basename "${output}")"
        fi
    fi
}
