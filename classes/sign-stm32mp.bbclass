EXTERNAL_KEY_CONF ??= "0"

ENCRYPT_ENABLE ??= "0"
ENCRYPT_FIP_KEY ??= ""
ENCRYPT_FSBL_KEY ??= ""
ENCRYPT_SUFFIX ??= "_Encrypted"

SIGN_ENABLE ??= "0"
SIGN_KEY ??=""
SIGN_KEY_PASS ??= ""
SIGN_SUFFIX ??= "_Signed"

SIGN_TOOL ??= ""

def search_path(file_search, d):
    """
    Check for <file_search> path availability from BBPATH
    And return the <file_search> absolute path
    """
    search_path = d.getVar("BBPATH").split(":")
    for p in search_path:
        file_path = os.path.join(p, file_search)
        if os.path.isfile(file_path):
            return file_path
    bbpaths = d.getVar('BBPATH').replace(':','\n\t')
    bb.fatal('\n[sign-stm32mp] Not able to find "%s" path from current BBPATH var:\n\t%s.' % (file_search, bbpaths))

def init_keylist_from(keylist, keyinput, soclist, d):
    """
    Build the <keylist> var as a coma separated list of values,
    Using either the default <keyinput> var value
    or any defined <keyinput>_socname var value
    (with 'socname' item comming from <soclist> var value list)
    """
    # Init soc name list
    socname_list = (d.getVar(soclist) or "").split()
    # Init key from keyinput var value
    key = d.getVar(keyinput) or ""
    if key:
        # Check first if keyinput_<soc> is defined to use it
        if len(socname_list) > 0:
            # Configure keylist according to STM32MP_SOC_NAME list
            d.setVar(keylist, '')
            for socname in socname_list:
                key = d.getVar(keyinput + '_' + socname) or ""
                if key:
                    if d.getVar('EXTERNAL_KEY_CONF') == '1':
                        key = search_path(key, d)
                    bb.debug(1, "[sign-stm32mp] Append '%s' path to %s (socname %s)." % (key, keylist, socname))
                    d.appendVar(keylist, key + ',')
                else:
                    bb.fatal("[sign-stm32mp] Please make sure to configure \"%s_%s\" var to key file." % (keyinput, socname))
        else:
            # Default to keyinput value setting
            if d.getVar('EXTERNAL_KEY_CONF') == '1':
                key = search_path(key, d)
                bb.debug(1, "[sign-stm32mp] Set %s to '%s' path." % (keylist, key))
                d.setVar(keylist, key)
            else:
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
                    if d.getVar('EXTERNAL_KEY_CONF') == '1':
                        key = search_path(key, d)
                    bb.debug(1, "[sign-stm32mp] Append '%s' path to %s (socname %s)." % (key, keylist, socname))
                    d.appendVar(keylist, key + ',')
                else:
                    bb.fatal("[sign-stm32mp] Please make sure to configure \"%s_%s\" var to key file." % (keyinput, socname))
        else:
            bb.fatal("[sign-stm32mp] Please make sure to configure \"%s\" var to key file." % keyinput)

python __anonymous() {
    if d.getVar('SIGN_ENABLE') == "1" or d.getVar('ENCRYPT_ENABLE') == "1":

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
            # Check for internal use of SIGN_KEY_PATH_LIST
            signingkey_list = d.getVar('SIGN_KEY_PATH_LIST')
            if signingkey_list:
                raise bb.parse.SkipRecipe("[sign-stm32mp] You cannot use SIGN_KEY_PATH_LIST as it is internal to sign-stm32mp.bbclass.")
            # Init SIGN_KEY_PATH_LIST from SIGN_KEY settings
            init_keylist_from('SIGN_KEY_PATH_LIST', 'SIGN_KEY', 'STM32MP_SOC_NAME', d)

        if d.getVar('ENCRYPT_ENABLE') == "1":
            if d.getVar('SIGN_ENABLE') == "0":
                bb.fatal("[sign-stm32mp] You need to set 'SIGN_ENABLE = 1' to encrypt and sign binaries at once.")

            # Check for internal use of ENCRYPT_FSBL_KEY_PATH_LIST
            fsbl_encryptkey_list = d.getVar('ENCRYPT_FSBL_KEY_PATH_LIST')
            if fsbl_encryptkey_list:
                raise bb.parse.SkipRecipe("[sign-stm32mp] You cannot use ENCRYPT_FSBL_KEY_PATH_LIST as it is internal to sign-stm32mp.bbclass.")
            # Init ENCRYPT_KEY_PATH_LIST from ENCRYPT_KEY settings
            init_keylist_from('ENCRYPT_FSBL_KEY_PATH_LIST', 'ENCRYPT_FSBL_KEY', 'STM32MP_ENCRYPT_SOC_NAME', d)

            # Check for internal use of ENCRYPT_FIP_KEY_PATH_LIST
            fip_encryptkey_list = d.getVar('ENCRYPT_FIP_KEY_PATH_LIST')
            if fip_encryptkey_list:
                raise bb.parse.SkipRecipe("[sign-stm32mp] You cannot use ENCRYPT_FIP_KEY_PATH_LIST as it is internal to sign-stm32mp.bbclass.")
            # Init ENCRYPT_KEY_PATH_LIST from ENCRYPT_KEY settings
            init_keylist_from('ENCRYPT_FIP_KEY_PATH_LIST', 'ENCRYPT_FIP_KEY', 'STM32MP_ENCRYPT_SOC_NAME', d)

}
