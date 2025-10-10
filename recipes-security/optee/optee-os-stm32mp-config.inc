# -----------------------------------------------
# Define config for each OPTEE_CONFIG
#   OPTEE_CONFIG[config] ?= "
#                           <list of devicetree>,
#                           <extra opt flags>,
#                          "
#
python () {
    import re

    opteeconfigflags = d.getVarFlags('OPTEE_CONFIG') or ""
    # The "doc" varflag is special, we don't want to see it here
    opteeconfigflags.pop('doc', None)
    opteeconfig = (d.getVar('OPTEE_CONFIG') or "").split()
    opteebasename = d.getVar('OPTEE_BASENAME') or ""

    if not opteeconfig:
        raise bb.parse.SkipRecipe("OPTEE_CONFIG must be set in the %s machine configuration." % d.getVar("MACHINE"))
    if (d.getVar('OPTEE_DEVICETREE') or "").split():
        raise bb.parse.SkipRecipe("You cannot use OPTEE_DEVICETREE as it is internal to OPTEE_CONFIG var expansion.")
    if (d.getVar('OPTEE_EXTRA_OPTFLAGS') or "").split():
        raise bb.parse.SkipRecipe("You cannot use OPTEE_EXTRA_OPTFLAGS as it is internal to OPTEE_CONFIG var expansion.")

    if (d.getVar('OPTEE_DEVICETREE_INTERNAL') or "").split():
        raise bb.parse.SkipRecipe("You cannot use OPTEE_DEVICETREE_INTERNAL as it is internal for var expansion.")
    if (d.getVar('OPTEE_DEVICETREE_EXTERNAL') or "").split():
        raise bb.parse.SkipRecipe("You cannot use OPTEE_DEVICETREE_EXTERNAL as it is internal for var expansion.")

    if (d.getVar('EXTERNAL_DT_ENABLED') or "0") == "1":
        localdata = bb.data.createCopy(d)
        localdata.setVar('EXTERNAL_DT_ENABLED', '0')

    if len(opteeconfig) > 0:
        for config in opteeconfig:
            for f, v in opteeconfigflags.items():
                if config == f:
                    # Make sure to get var flag properly expanded
                    v = d.getVarFlag('OPTEE_CONFIG', config)
                    if not v.strip():
                        bb.fatal('[OPTEE_CONFIG] Missing configuration for %s config' % config)
                    items = v.split(',')
                    if items[0] and len(items) > 5:
                        raise bb.parse.SkipRecipe('Only <DEVICETREE>,<EXTRA_OPTFLAGS> can be specified!')
                    # Set internal vars
                    bb.debug(1, "Appending '%s' to OPTEE_DEVICETREE" % items[0])
                    d.appendVar('OPTEE_DEVICETREE', items[0] + ',')

                    if (d.getVar('EXTERNAL_DT_ENABLED') or "0") == "1":
                        internal_devicetree = localdata.getVarFlag('OPTEE_CONFIG', config).split(',')[0]
                        external_devicetree = ' '.join([dt for dt in items[0].split() if dt not in internal_devicetree.split()])
                    else:
                        internal_devicetree = items[0]
                        external_devicetree = ''
                    d.appendVar('OPTEE_DEVICETREE_INTERNAL', internal_devicetree + ',')
                    d.appendVar('OPTEE_DEVICETREE_EXTERNAL', external_devicetree + ',')

                    if len(items) > 1 and items[1]:
                        bb.debug(1, "Appending '%s' to OPTEE_EXTRA_OPTFLAGS." % items[1])
                        d.appendVar('OPTEE_EXTRA_OPTFLAGS', items[1] + ',')
                    else:
                        d.appendVar('OPTEE_EXTRA_OPTFLAGS', '' + ',')
                    break
}

OPTEE_DEVICETREE_optee += "${@' '.join(dt for dt in '${STM32MP_DEVICETREE}'.split() if 'stm32mp13' in dt)}"
OPTEE_DEVICETREE_optee += "${@' '.join(dt for dt in '${STM32MP_DEVICETREE}'.split() if 'stm32mp15' in dt)}"
OPTEE_DEVICETREE_optee += "${@' '.join(dt for dt in '${STM32MP_DEVICETREE}'.split() if 'stm32mp21' in dt)}"
OPTEE_DEVICETREE_optee += "${@' '.join(dt for dt in '${STM32MP_DEVICETREE}'.split() if 'stm32mp23' in dt)}"
OPTEE_DEVICETREE_optee += "${@' '.join(dt for dt in '${STM32MP_DEVICETREE}'.split() if 'stm32mp25' in dt)}"
OPTEE_DEVICETREE_optee += "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp13' in dt)}"
OPTEE_DEVICETREE_optee += "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp15' in dt)}"
OPTEE_DEVICETREE_optee += "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp21' in dt)}"
OPTEE_DEVICETREE_optee += "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp23' in dt)}"
OPTEE_DEVICETREE_optee += "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp25' in dt)}"

OPTEE_DEVICETREE_opteemin += "${@' '.join(dt for dt in '${STM32MP_DEVICETREE}'.split() if 'stm32mp13' in dt)}"
OPTEE_DEVICETREE_opteemin += "${@' '.join(dt for dt in '${STM32MP_DEVICETREE}'.split() if 'stm32mp15' in dt)}"
OPTEE_DEVICETREE_opteemin += "${@' '.join(dt for dt in '${STM32MP_DEVICETREE}'.split() if 'stm32mp21' in dt)}"
OPTEE_DEVICETREE_opteemin += "${@' '.join(dt for dt in '${STM32MP_DEVICETREE}'.split() if 'stm32mp23' in dt)}"
OPTEE_DEVICETREE_opteemin += "${@' '.join(dt for dt in '${STM32MP_DEVICETREE}'.split() if 'stm32mp25' in dt)}"
OPTEE_DEVICETREE_opteemin += "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp13' in dt)}"
OPTEE_DEVICETREE_opteemin += "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp15' in dt)}"
OPTEE_DEVICETREE_opteemin += "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp21' in dt)}"
OPTEE_DEVICETREE_opteemin += "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp23' in dt)}"
OPTEE_DEVICETREE_opteemin += "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp25' in dt)}"

OPTEE_CONFIG_OPTS_EXTDT ?= "${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', 'CFG_EXT_DTS=${STAGING_EXTDT_DIR}/${EXTDT_DIR_OPTEE}', '', d)}"
OPTEE_CONFIG_OPTS_EXTDT_programmer ?= "${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', 'CFG_EXT_DTS=${STAGING_EXTDT_DIR}/${EXTDT_DIR_OPTEE_SERIAL}', '', d)}"

OPTEE_CONFIG[optee]               ?= "${OPTEE_DEVICETREE_optee},${OPTEE_CONFIG_OPTS_EXTDT} CFG_STM32MP_PROFILE=secure_and_system_services"
OPTEE_CONFIG[opteemin]            ?= "${OPTEE_DEVICETREE_opteemin},${OPTEE_CONFIG_OPTS_EXTDT} CFG_STM32MP_PROFILE=system_services"
OPTEE_CONFIG[optee-programmer]    ?= "${OPTEE_DEVICETREE_optee},${OPTEE_CONFIG_OPTS_EXTDT_programmer} CFG_STM32MP_PROFILE=secure_and_system_services"
OPTEE_CONFIG[opteemin-programmer] ?= "${OPTEE_DEVICETREE_opteemin},${OPTEE_CONFIG_OPTS_EXTDT_programmer} CFG_STM32MP_PROFILE=system_services"
