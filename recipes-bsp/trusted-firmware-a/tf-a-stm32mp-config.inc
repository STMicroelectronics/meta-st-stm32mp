# -----------------------------------------------
# Handle TF-A config and set internal vars
#   TF_A_DEVICETREE
#   TF_A_EXTRA_OPTFLAGS
python () {
    import re

    tfaconfigflags = d.getVarFlags('TF_A_CONFIG') or ""
    # The "doc" varflag is special, we don't want to see it here
    tfaconfigflags.pop('doc', None)
    tfaconfig = (d.getVar('TF_A_CONFIG') or "").split()
    tfabasename = d.getVar('TF_A_BASENAME') or ""

    if not tfaconfig:
        raise bb.parse.SkipRecipe("TF_A_CONFIG must be set in the %s machine configuration." % d.getVar("MACHINE"))
    if (d.getVar('TF_A_DEVICETREE') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_A_DEVICETREE as it is internal to TF_A_CONFIG var expansion.")
    if (d.getVar('TF_A_EXTRA_OPTFLAGS') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_A_EXTRA_OPTFLAGS as it is internal to TF_A_CONFIG var expansion.")
    if (d.getVar('TF_A_BINARIES') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_A_BINARIES as it is internal to TF_A_CONFIG var expansion.")
    if (d.getVar('TF_A_MAKE_TARGET') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_A_MAKE_TARGET as it is internal to TF_A_CONFIG var expansion.")
    if (d.getVar('TF_A_FILES') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_A_FILES as it is internal to TF_A_CONFIG var expansion.")

    if len(tfaconfig) > 0:
        for config in tfaconfig:
            for f, v in tfaconfigflags.items():
                if config == f:
                    # Make sure to get var flag properly expanded
                    v = d.getVarFlag('TF_A_CONFIG', config)
                    if not v.strip():
                        bb.fatal('[TF_A_CONFIG] Missing configuration for %s config' % config)
                    items = v.split(',')
                    if items[0] and len(items) > 5:
                        raise bb.parse.SkipRecipe('Only <DEVICETREE>,<EXTRA_OPTFLAGS>,<BINARY_BASENAME>,<MAKE_TARGET>,<FILES TYPE> can be specified!')
                    # Set internal vars
                    bb.debug(1, "Appending '%s' to TF_A_DEVICETREE" % items[0])
                    d.appendVar('TF_A_DEVICETREE', items[0] + ',')
                    if len(items) > 1 and items[1]:
                        bb.debug(1, "Appending '%s' to TF_A_EXTRA_OPTFLAGS." % items[1])
                        d.appendVar('TF_A_EXTRA_OPTFLAGS', items[1] + ',')
                    else:
                        d.appendVar('TF_A_EXTRA_OPTFLAGS', '' + ',')
                    if len(items) > 2 and items[2]:
                        bb.debug(1, "Appending '%s' to TF_A_BINARIES." % items[2])
                        d.appendVar('TF_A_BINARIES', items[2].strip() + ',')
                    else:
                        bb.debug(1, "Appending '%s' to TF_A_BINARIES." % tfabasename)
                        d.appendVar('TF_A_BINARIES', tfabasename + ',')
                    if len(items) > 3 and items[3]:
                        bb.debug(1, "Appending '%s' to TF_A_MAKE_TARGET." % items[3])
                        d.appendVar('TF_A_MAKE_TARGET', items[3] + ',')
                    else:
                        d.appendVar('TF_A_MAKE_TARGET', 'all' + ',')
                    if len(items) > 4 and items[4]:
                        bb.debug(1, "Appending '%s' to TF_A_FILES." % items[4])
                        d.appendVar('TF_A_FILES', items[4] + ',')
                    else:
                        d.appendVar('TF_A_FILES', 'bl2' + ',')
                    break
}

# -----------------------------------------------
# Define config for each TF_A_CONFIG
#   TF_A_CONFIG[config] ?= "
#                           <list of devicetree>,
#                           <extra opt flags>,
#                           <binary basename (default: 'tf-a')>,
#                           <make target (default: 'all')>,
#                           <type of binary to deploy: [bl2 bl31 bl32 fwconfig] (default 'bl2')>
#                          "
#
TF_A_CONFIG_OPTS_fwupdate ?= "${@bb.utils.contains('MACHINE_FEATURES', 'fw-update', 'PSA_FWU_SUPPORT=1', '', d)}"
TF_A_CONFIG_OPTS_optee    ?= "${@bb.utils.contains('TUNE_FEATURES', 'aarch64', 'SPD=opteed', 'AARCH32_SP=optee', d)}"
TF_A_CONFIG_BASENAME_BIN ?= "${TF_A_BASENAME}"
TF_A_CONFIG_MAKE_TARGET ?= "all"
TF_A_CONFIG_MAKE_EXTRAS ?= "${@bb.utils.contains('FIP_BL31_ENABLE', '1', 'dtbs bl31', 'dtbs', d)}"
TF_A_CONFIG_DEPLOY_FTYPE ?= "bl2"
TF_A_CONFIG_DEPLOY_EXTRA ?= "${@bb.utils.contains('FIP_BL31_ENABLE', '1', 'fwconfig bl31', 'fwconfig', d)}"

TF_A_CONFIG[optee-emmc] ?= "\
    ${STM32MP_DT_FILES_EMMC},\
    ${TF_A_CONFIG_OPTS_optee} ${TF_A_CONFIG_OPTS_fwupdate} STM32MP_EMMC=1,\
    ${TF_A_CONFIG_BASENAME_BIN},\
    ${TF_A_CONFIG_MAKE_TARGET} ${TF_A_CONFIG_MAKE_EXTRAS},\
    ${TF_A_CONFIG_DEPLOY_FTYPE} ${TF_A_CONFIG_DEPLOY_EXTRA}"
TF_A_CONFIG[optee-nand] ?= "\
    ${STM32MP_DT_FILES_NAND},\
    ${TF_A_CONFIG_OPTS_optee} ${TF_A_CONFIG_OPTS_fwupdate} STM32MP_RAW_NAND=1 ${@'STM32MP_FORCE_MTD_START_OFFSET=${TF_A_MTD_START_OFFSET_NAND}' if ${TF_A_MTD_START_OFFSET_NAND} else ''},\
    ${TF_A_CONFIG_BASENAME_BIN},\
    ${TF_A_CONFIG_MAKE_TARGET} ${TF_A_CONFIG_MAKE_EXTRAS},\
    ${TF_A_CONFIG_DEPLOY_FTYPE} ${TF_A_CONFIG_DEPLOY_EXTRA}"
TF_A_CONFIG[optee-nor] ?= "\
    ${STM32MP_DT_FILES_NOR},\
    ${TF_A_CONFIG_OPTS_optee} ${TF_A_CONFIG_OPTS_fwupdate} STM32MP_SPI_NOR=1 ${@'STM32MP_FORCE_MTD_START_OFFSET=${TF_A_MTD_START_OFFSET_NOR}' if ${TF_A_MTD_START_OFFSET_NOR} else ''},\
    ${TF_A_CONFIG_BASENAME_BIN},\
    ${TF_A_CONFIG_MAKE_TARGET} ${TF_A_CONFIG_MAKE_EXTRAS},\
    ${TF_A_CONFIG_DEPLOY_FTYPE} ${TF_A_CONFIG_DEPLOY_EXTRA}"
TF_A_CONFIG[optee-sdcard] ?= "\
    ${STM32MP_DT_FILES_SDCARD},\
    ${TF_A_CONFIG_OPTS_optee} ${TF_A_CONFIG_OPTS_fwupdate} STM32MP_SDMMC=1,\
    ${TF_A_CONFIG_BASENAME_BIN},\
    ${TF_A_CONFIG_MAKE_TARGET} ${TF_A_CONFIG_MAKE_EXTRAS},\
    ${TF_A_CONFIG_DEPLOY_FTYPE} ${TF_A_CONFIG_DEPLOY_EXTRA}"
TF_A_CONFIG[optee-spinand] ?= "\
    ${STM32MP_DT_FILES_SPINAND},\
    ${TF_A_CONFIG_OPTS_optee} ${TF_A_CONFIG_OPTS_fwupdate} STM32MP_SPI_NAND=1 ${@'STM32MP_FORCE_MTD_START_OFFSET=${TF_A_MTD_START_OFFSET_SPINAND}' if ${TF_A_MTD_START_OFFSET_SPINAND} else ''},\
    ${TF_A_CONFIG_BASENAME_BIN},\
    ${TF_A_CONFIG_MAKE_TARGET} ${TF_A_CONFIG_MAKE_EXTRAS},\
    ${TF_A_CONFIG_DEPLOY_FTYPE} ${TF_A_CONFIG_DEPLOY_EXTRA}"

TF_A_CONFIG[optee-programmer] ?= "\
    ${STM32MP_DEVICETREE_PROGRAMMER},\
    ${TF_A_CONFIG_OPTS_optee},\
    ${TF_A_CONFIG_BASENAME_BIN},\
    ${TF_A_CONFIG_MAKE_EXTRAS},\
    ${TF_A_CONFIG_DEPLOY_EXTRA}"

TF_A_CONFIG[uart] ?= "\
    ${@bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', '${STM32MP_DEVICETREE_PROGRAMMER}', '${STM32MP_DEVICETREE}', d)},\
    STM32MP_UART_PROGRAMMER=1,\
    ${TF_A_CONFIG_BASENAME_BIN},\
    ${TF_A_CONFIG_MAKE_TARGET},\
    ${TF_A_CONFIG_DEPLOY_FTYPE}"
TF_A_CONFIG[usb]  ?= "\
    ${@bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', '${STM32MP_DEVICETREE_PROGRAMMER}', '${STM32MP_DEVICETREE}', d)},\
    STM32MP_USB_PROGRAMMER=1,\
    ${TF_A_CONFIG_BASENAME_BIN},\
    ${TF_A_CONFIG_MAKE_TARGET},\
    ${TF_A_CONFIG_DEPLOY_FTYPE}"
# Define configuration for SSP
TF_A_CONFIG[uart-ssp] ?= "\
    ${@bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', '${STM32MP_DEVICETREE_PROGRAMMER}', '${STM32MP_DEVICETREE}', d)},\
    STM32MP_UART_PROGRAMMER=1 STM32MP_SSP=1,\
    ${TF_A_CONFIG_BASENAME_BIN}-ssp,\
    ${TF_A_CONFIG_MAKE_TARGET},\
    ${TF_A_CONFIG_DEPLOY_FTYPE}"
TF_A_CONFIG[usb-ssp]  ?= "\
    ${@bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', '${STM32MP_DEVICETREE_PROGRAMMER}', '${STM32MP_DEVICETREE}', d)},\
    STM32MP_USB_PROGRAMMER=1 STM32MP_SSP=1,\
    ${TF_A_CONFIG_BASENAME_BIN}-ssp,\
    ${TF_A_CONFIG_MAKE_TARGET},\
    ${TF_A_CONFIG_DEPLOY_FTYPE}"
