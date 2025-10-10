# -----------------------------------------------
# Handle TF-M config and set internal vars
#   TF_M_DEVICETREE
#   TF_M_DEVICETREE_INTERNAL
#   TF_M_DEVICETREE_EXTERNAL
#   TF_M_PLATFORM
#   TF_M_EXTRA_OPTFLAGS
#   TF_M_BOOTDEV
#   TF_M_BOOT_SUFFIX_M33
#   TF_M_BOOT_SUFFIX_A35
#   TF_M_DT_BL2_SUFFIX
#   TF_M_DT_TFM_SUFFIX
python () {
    import re

    if bb.utils.contains('MACHINE_FEATURES', 'm33td', True, False, d):
        if (d.getVar('M33PROJECT_TF_M_TRUSTZONE') or "0") == "1":
            raise bb.parse.SkipRecipe("You cannot configure M33PROJECT_TF_M_TRUSTZONE to '1' and enable 'm33td' in MACHINE_FEATURES: please clarify your machine configuration.")
    else:
        if (d.getVar('M33PROJECT_TF_M_TRUSTZONE') or "0") == "0":
            return

    tfmconfigflags = d.getVarFlags('TF_M_CONFIG') or ""
    # The "doc" varflag is special, we don't want to see it here
    tfmconfigflags.pop('doc', None)
    tfmconfig = (d.getVar('TF_M_CONFIG') or "").split()

    if not tfmconfig:
        raise bb.parse.SkipRecipe("TF_M_CONFIG must be set in the %s machine configuration." % d.getVar("MACHINE"))
    if (d.getVar('TF_M_DEVICETREE') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_M_DEVICETREE as it is internal to TF_M_CONFIG var expansion.")
    if (d.getVar('TF_M_DEVICETREE_EXTERNAL') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_M_DEVICETREE_EXTERNAL as it is internal for var expansion.")
    if (d.getVar('TF_M_DEVICETREE_INTERNAL') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_M_DEVICETREE_INTERNAL as it is internal for var expansion.")
    if (d.getVar('TF_M_PLATFORM') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_M_PLATFORM as it is internal to TF_M_CONFIG var expansion.")
    if (d.getVar('TF_M_EXTRA_OPTFLAGS') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_M_EXTRA_OPTFLAGS as it is internal to TF_M_CONFIG var expansion.")
    if (d.getVar('TF_M_BOOTDEV') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_M_BOOTDEV as it is internal to TF_M_CONFIG var expansion.")
    if (d.getVar('TF_M_BOOT_SUFFIX_M33') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_M_BOOT_SUFFIX_M33 as it is internal to TF_M_CONFIG var expansion.")
    if (d.getVar('TF_M_BOOT_SUFFIX_A35') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_M_BOOT_SUFFIX_A35 as it is internal to TF_M_CONFIG var expansion.")
    if (d.getVar('TF_M_DT_BL2_SUFFIX') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_M_DT_BL2_SUFFIX as it is internal to TF_M_CONFIG var expansion.")
    if (d.getVar('TF_M_DT_TFM_SUFFIX') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_M_DT_TFM_SUFFIX as it is internal to TF_M_CONFIG var expansion.")

    if (d.getVar('EXTERNAL_DT_ENABLED') or "0") == "1":
        localdata = bb.data.createCopy(d)
        localdata.setVar('EXTERNAL_DT_ENABLED', '0')

    if len(tfmconfig) > 0:
        for config in tfmconfig:
            found = False
            for f, v in tfmconfigflags.items():
                if config == f:
                    found = True
                    # Make sure to get var flag properly expanded
                    v = d.getVarFlag('TF_M_CONFIG', config)
                    if not v.strip():
                        bb.fatal('[TF_M_CONFIG] Empty configuration for %s config' % config)
                    items = v.split(',')
                    if items[0] and len(items) > 8:
                        raise bb.parse.SkipRecipe('Only <DEVICETREE>,<PLATFORM>,<EXTRA_OPTFLAGS>,<BOOTDEV>,<BOOT_SUFFIX_M33>,<BOOT_SUFFIX_A35>,<DT_BL2_SUFFIX>,<DT_TFM_SUFFIX> can be specified! (%s: %s, %s)' % (config,items[0],len(items)))
                    # Set internal vars
                    bb.debug(1, "Appending '%s' to TF_M_DEVICETREE" % items[0])
                    d.appendVar('TF_M_DEVICETREE', items[0] + ',')

                    if (d.getVar('EXTERNAL_DT_ENABLED') or "0") == "1":
                        internal_devicetree = localdata.getVarFlag('TF_M_CONFIG', config).split(',')[0]
                        external_devicetree = ' '.join([dt for dt in items[0].split() if dt not in internal_devicetree.split()])
                    else:
                        internal_devicetree = items[0]
                        external_devicetree = ''
                    bb.debug(1, "Appending '%s' to TF_M_DEVICETREE_INTERNAL" % internal_devicetree)
                    d.appendVar('TF_M_DEVICETREE_INTERNAL', internal_devicetree + ',')
                    bb.debug(1, "Appending '%s' to TF_M_DEVICETREE_EXTERNAL" % external_devicetree)
                    d.appendVar('TF_M_DEVICETREE_EXTERNAL', external_devicetree + ',')

                    if len(items) > 1 and items[1]:
                        bb.debug(1, "Appending '%s' to TF_M_PLATFORM." % items[1])
                        d.appendVar('TF_M_PLATFORM', items[1] + ',')
                        # Manage TF_M_PLATFORM_BASENAME_LIST
                        for pltf in items[1].split():
                            d.appendVar('TF_M_PLATFORM_BASENAME_LIST', os.path.basename(pltf) + ' ')
                    else:
                        d.appendVar('TF_M_PLATFORM', '' + ',')
                    if len(items) > 2 and items[2]:
                        bb.debug(1, "Appending '%s' to TF_M_EXTRA_OPTFLAGS." % items[2])
                        d.appendVar('TF_M_EXTRA_OPTFLAGS', items[2] + ',')
                    else:
                        d.appendVar('TF_M_EXTRA_OPTFLAGS', '' + ',')
                    if len(items) > 3 and items[3]:
                        bb.debug(1, "Appending '%s' to TF_M_BOOTDEV." % items[3])
                        d.appendVar('TF_M_BOOTDEV', items[3].strip() + ',')
                    else:
                        d.appendVar('TF_M_BOOTDEV', '' + ',')
                    if len(items) > 4 and items[4]:
                        bb.debug(1, "Appending '%s' to TF_M_BOOT_SUFFIX_M33." % items[4])
                        d.appendVar('TF_M_BOOT_SUFFIX_M33', items[4].strip() + ',')
                    else:
                        d.appendVar('TF_M_BOOT_SUFFIX_M33', '' + ',')
                    if len(items) > 5 and items[5]:
                        bb.debug(1, "Appending '%s' to TF_M_BOOT_SUFFIX_A35." % items[5])
                        d.appendVar('TF_M_BOOT_SUFFIX_A35', items[5].strip() + ',')
                    else:
                        d.appendVar('TF_M_BOOT_SUFFIX_A35', '' + ',')

                    if len(items) > 6 and items[6]:
                        bb.debug(1, "Appending '%s' to TF_M_DT_BL2_SUFFIX." % items[6])
                        d.appendVar('TF_M_DT_BL2_SUFFIX', items[6].strip() + ',')
                    else:
                        d.appendVar('TF_M_DT_BL2_SUFFIX', '' + ',')
                    if len(items) > 7 and items[7]:
                        bb.debug(1, "Appending '%s' to TF_M_DT_TFM_SUFFIX." % items[7])
                        d.appendVar('TF_M_DT_TFM_SUFFIX', items[7].strip() + ',')
                    else:
                        d.appendVar('TF_M_DT_TFM_SUFFIX', '' + ',')
                    break
            if not found:
                raise bb.parse.SkipRecipe('[TF_M_CONFIG] The selected TF_M_CONFIG key %s has no match in %s' % (config, tfmconfigflags.keys()))
}

# -----------------------------------------------
# Define plateform extra flags for build
#   TF_M_PLATFORM_OPTFLAGS_plateform ?= "<list of extra opt flags>"
#
TF_M_PLATFORM_OPTFLAGS_stm32mp215f_dk  = "-DTFM_PARTITION_PLATFORM=ON -DTFM_PARTITION_PROTECTED_STORAGE=OFF"
TF_M_PLATFORM_OPTFLAGS_stm32mp257f_ev1 = ""

# -----------------------------------------------
# Define config for each TF_M_CONFIG
#   TF_M_CONFIG[config] ?= "
#                           <list of devicetree>,
#                           <list of platform>,
#                           <extra opt flags>,
#                           <m33 boot device>,
#                           <m33 boot suffix>,
#                           <a35 boot suffix>,
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
    ${STM32MP_DEVICETREE}, \
    ${TFM_STM_PLATFORM}, \
    ${TF_M_CONFIG_OPTS_copro} \
    "
# Config for master boot
TF_M_CONFIG[m33td_emmc] = "\
    ${STM32MP_DT_FILES_EMMC}, \
    ${TFM_STM_PLATFORM}, \
    ${TF_M_CONFIG_OPTS_m33td}, \
    sdmmc2, \
    emmc, \
    emmc, \
    ${EXTDT_SUFFIX_EMMC}, \
    ${EXTDT_SUFFIX_EMMC} \
    "
TF_M_CONFIG[m33td_nor] = "\
    ${STM32MP_DT_FILES_NOR_mb_sb}, \
    ${TFM_STM_PLATFORM}, \
    ${TF_M_CONFIG_OPTS_m33td} -DTFM_PARTITION_PROTECTED_STORAGE=OFF, \
    ospi, \
    nor, \
    nor, \
    ${EXTDT_SUFFIX_NOR}, \
    ${EXTDT_SUFFIX_NOR} \
    "
TF_M_CONFIG[m33td_nor-emmc] = "\
    ${@' '.join(dt for dt in (d.getVar('STM32MP_DT_FILES_NOR_mb') or '').split() if dt in (d.getVar('STM32MP_DT_FILES_EMMC') or '').split())}, \
    ${TFM_STM_PLATFORM}, \
    ${TF_M_CONFIG_OPTS_m33td}, \
    ospi, \
    nor, \
    emmc, \
    ${EXTDT_SUFFIX_NOR}, \
    ${EXTDT_SUFFIX_EMMC} \
    "
TF_M_CONFIG[m33td_nor-sdcard] = "\
    ${@' '.join(dt for dt in (d.getVar('STM32MP_DT_FILES_NOR_mb') or '').split() if dt in (d.getVar('STM32MP_DT_FILES_SDCARD') or '').split())}, \
    ${TFM_STM_PLATFORM}, \
    ${TF_M_CONFIG_OPTS_m33td}, \
    ospi, \
    nor, \
    sdcard, \
    ${EXTDT_SUFFIX_NOR}, \
    ${EXTDT_SUFFIX_SDCARD} \
    "
TF_M_CONFIG[m33td_sdcard] = "\
    ${STM32MP_DT_FILES_SDCARD}, \
    ${TFM_STM_PLATFORM}, \
    ${TF_M_CONFIG_OPTS_m33td}, \
    sdmmc1, \
    sdcard, \
    sdcard, \
    ${EXTDT_SUFFIX_SDCARD}, \
    ${EXTDT_SUFFIX_SDCARD} \
    "
