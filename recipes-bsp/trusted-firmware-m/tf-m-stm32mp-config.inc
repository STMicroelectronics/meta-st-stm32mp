# -----------------------------------------------
# Handle TF-M config and set internal vars
#   TF_M_EXTRA_OPTFLAGS
#   TF_M_BOOTDEV
#   TF_M_DT_BL2_SUFFIX
#   TF_M_DT_TFM_SUFFIX
python () {
    if bb.utils.contains('MACHINE_FEATURES', 'm33td', True, False, d):
        if (d.getVar('M33PROJECT_TF_M_TRUSTZONE') or "0") == "1":
            raise bb.parse.SkipRecipe("You cannot configure M33PROJECT_TF_M_TRUSTZONE to '1' and enable 'm33td' in MACHINE_FEATURES: please clarify your machine configuration.")
    else:
        if (d.getVar('M33PROJECT_TF_M_TRUSTZONE') or "0") == "0":
            return

    tfmconfigflags = d.getVarFlags('TF_M_CONFIG') or ""
    # The "doc" varflag is special, we don't want to see it here
    tfmconfigflags.pop('doc', None)
    m33fwconfig = (d.getVar('M33FW_CONFIG') or "").split()

    if not m33fwconfig:
        raise bb.parse.SkipRecipe("M33FW_CONFIG must be set in the %s machine configuration." % d.getVar("MACHINE"))

    if (d.getVar('TF_M_EXTRA_OPTFLAGS') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_M_EXTRA_OPTFLAGS as it is internal to TF_M_CONFIG var expansion.")
    if (d.getVar('TF_M_BOOTDEV') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_M_BOOTDEV as it is internal to TF_M_CONFIG var expansion.")
    if (d.getVar('TF_M_DT_BL2_SUFFIX') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_M_DT_BL2_SUFFIX as it is internal to TF_M_CONFIG var expansion.")
    if (d.getVar('TF_M_DT_TFM_SUFFIX') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_M_DT_TFM_SUFFIX as it is internal to TF_M_CONFIG var expansion.")

    if len(m33fwconfig) > 0:
        for config in m33fwconfig:
            found = False
            for f, v in tfmconfigflags.items():
                if config == f:
                    found = True
                    # Make sure to get var flag properly expanded
                    v = d.getVarFlag('TF_M_CONFIG', config)
                    if not v.strip():
                        bb.fatal('[TF_M_CONFIG] Empty configuration for %s config' % config)
                    items = v.split(',')
                    if items[0] and len(items) > 5:
                        raise bb.parse.SkipRecipe('Only <EXTRA_OPTFLAGS>,<BOOTDEV>,<DT_BL2_SUFFIX>,<DT_TFM_SUFFIX> can be specified! (%s: %s, %s)' % (config,items[0],len(items)))
                    # Set internal vars
                    if items[0]:
                        bb.debug(1, "Appending '%s' to TF_M_EXTRA_OPTFLAGS." % items[0])
                        d.appendVar('TF_M_EXTRA_OPTFLAGS', items[0] + ',')
                    else:
                        d.appendVar('TF_M_EXTRA_OPTFLAGS', '' + ',')
                    if len(items) > 1 and items[1]:
                        bb.debug(1, "Appending '%s' to TF_M_BOOTDEV." % items[1])
                        d.appendVar('TF_M_BOOTDEV', items[1].strip() + ',')
                    else:
                        d.appendVar('TF_M_BOOTDEV', '' + ',')
                    if len(items) > 2 and items[2]:
                        bb.debug(1, "Appending '%s' to TF_M_DT_BL2_SUFFIX." % items[2])
                        d.appendVar('TF_M_DT_BL2_SUFFIX', items[2].strip() + ',')
                    else:
                        d.appendVar('TF_M_DT_BL2_SUFFIX', '' + ',')
                    if len(items) > 3 and items[3]:
                        bb.debug(1, "Appending '%s' to TF_M_DT_TFM_SUFFIX." % items[3])
                        d.appendVar('TF_M_DT_TFM_SUFFIX', items[3].strip() + ',')
                    else:
                        d.appendVar('TF_M_DT_TFM_SUFFIX', '' + ',')
                    break
            if not found:
                raise bb.parse.SkipRecipe('[TF_M_CONFIG] The selected TF_M_CONFIG key %s has no match in %s' % (config, tfmconfigflags.keys()))
}

# -----------------------------------------------
# Define config for each TF_M_CONFIG
#   TF_M_CONFIG[config] ?= "
#                           <extra opt flags>,
#                           <m33 boot device>,
#                           <dt bl2 suffix>,
#                           <dt tfm suffix>
#                          "
#

TF_M_CONFIG_OPTS_copro += "-DSTM32_M33TDCID=OFF"
TF_M_CONFIG_OPTS_copro += "-DNS=OFF"
TF_M_CONFIG_OPTS_m33td += "-DSTM32_M33TDCID=ON"
TF_M_CONFIG_OPTS_m33td += "-DNS=ON"

# Config for slave boot
# tf-m elf for m33 copro signature (only requires devicetree and platform list)
TF_M_CONFIG[m33copro] = "\
    ${TF_M_CONFIG_OPTS_copro} \
    "
# Config for main boot
TF_M_CONFIG[m33td_emmc] = "\
    ${TF_M_CONFIG_OPTS_m33td}, \
    sdmmc2, \
    ${EXTDT_SUFFIX_EMMC}, \
    ${EXTDT_SUFFIX_EMMC} \
    "
TF_M_CONFIG[m33td_nor] = "\
    ${TF_M_CONFIG_OPTS_m33td} -DTFM_PARTITION_PROTECTED_STORAGE=OFF, \
    ospi, \
    ${EXTDT_SUFFIX_NOR}, \
    ${EXTDT_SUFFIX_NOR} \
    "
TF_M_CONFIG[m33td_nor-emmc] = "\
    ${TF_M_CONFIG_OPTS_m33td}, \
    ospi, \
    ${EXTDT_SUFFIX_NOR}, \
    ${EXTDT_SUFFIX_EMMC} \
    "
TF_M_CONFIG[m33td_nor-sdcard] = "\
    ${TF_M_CONFIG_OPTS_m33td}, \
    ospi, \
    ${EXTDT_SUFFIX_NOR}, \
    ${EXTDT_SUFFIX_SDCARD} \
    "
TF_M_CONFIG[m33td_sdcard] = "\
    ${TF_M_CONFIG_OPTS_m33td}, \
    sdmmc1, \
    ${EXTDT_SUFFIX_SDCARD}, \
    ${EXTDT_SUFFIX_SDCARD} \
    "
