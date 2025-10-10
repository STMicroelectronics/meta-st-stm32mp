# The format for u-boot configuration is: <conf-name>_<soc-name>
#   <conf-name>: used as U-Boot 'configuration name' in final U-Boot binaries
#   <soc-name> : used to filtered devicetree list to use

# -----------------------------------------------
# Handle U-Boot config to set internal var:
#   UBOOT_DEVICETREE
#
python () {
    ubootdevicetreeflags = d.getVarFlags('UBOOT_DEVICETREE')
    ubootoptflags = d.getVarFlags('UBOOT_MAKE_OPT')
    # The "doc" varflag is special, we don't want to see it here
    ubootdevicetreeflags.pop('doc', None)
    ubootoptflags.pop('doc', None)
    ubootconfig = (d.getVar('UBOOT_CONFIG') or "").split()

    if (d.getVar('UBOOT_DEVICETREE') or "").split():
        raise bb.parse.SkipRecipe("You cannot use UBOOT_DEVICETREE as it is internal for var expansion.")
    if (d.getVar('UBOOT_DEVICETREE_INTERNAL') or "").split():
        raise bb.parse.SkipRecipe("You cannot use UBOOT_DEVICETREE_INTERNAL as it is internal for var expansion.")
    if (d.getVar('UBOOT_DEVICETREE_EXTERNAL') or "").split():
        raise bb.parse.SkipRecipe("You cannot use UBOOT_DEVICETREE_EXTERNAL as it is internal for var expansion.")

    if (d.getVar('EXTERNAL_DT_ENABLED') or "0") == "1":
        localdata = bb.data.createCopy(d)
        localdata.setVar('EXTERNAL_DT_ENABLED', '0')

    if len(ubootconfig) > 0 and len(ubootdevicetreeflags) > 0:
        for config in ubootconfig:
            found = False
            for f, v in ubootdevicetreeflags.items():
                if config == f:
                    found = True
                    # Make sure to get var flag properly expanded
                    v = d.getVarFlag('UBOOT_DEVICETREE', config)
                    if not v.strip():
                        bb.note('[UBOOT_DEVICETREE] No devicetree configured for %s config' % config)
                    d.appendVar('UBOOT_DEVICETREE', v + ',')

                    if (d.getVar('EXTERNAL_DT_ENABLED') or "0") == "1":
                        internal_devicetree = localdata.getVarFlag('UBOOT_DEVICETREE', config)
                        external_devicetree = ' '.join([dt for dt in v.split() if dt not in internal_devicetree.split()])
                    else:
                        internal_devicetree = v
                        external_devicetree = ''
                    d.appendVar('UBOOT_DEVICETREE_INTERNAL', internal_devicetree + ',')
                    d.appendVar('UBOOT_DEVICETREE_EXTERNAL', external_devicetree + ',')

                    break

            if not found:
                raise bb.parse.SkipRecipe("The selected UBOOT_CONFIG key %s has no match in UBOOT_DEVICETREE keys: %s." % (config, ubootdevicetreeflags.keys()))

    if len(ubootconfig) > 0 and len(ubootoptflags) > 0:
        for config in ubootconfig:
            for f, v in ubootoptflags.items():
                if config == f:
                    # Make sure to get var flag properly expanded
                    v = d.getVarFlag('UBOOT_MAKE_OPT', config)
                    if not v.strip():
                        bb.note('[UBOOT_MAKE_OPT] No make opt configured for %s config' % config)
                    d.appendVar('UBOOT_MAKE_OPT', v + ',')
                    break

}

# -----------------------------------------------
# Define U-Boot defconfig and binary to use for each UBOOT_CONFIG
#   UBOOT_CONFIG[config] ?= "<defconfig>,<fs-type images>,<binary to deploy>"
#
UBOOT_CONFIG[default_stm32mp15] ?= "stm32mp15_defconfig,,u-boot.dtb"
UBOOT_CONFIG[default_stm32mp13] ?= "stm32mp13_defconfig,,u-boot.dtb"
UBOOT_CONFIG[default_stm32mp21] ?= "stm32mp21_defconfig,,u-boot.dtb"
UBOOT_CONFIG[default_stm32mp23] ?= "stm32mp23_defconfig,,u-boot.dtb"
UBOOT_CONFIG[default_stm32mp25] ?= "stm32mp25_defconfig,,u-boot.dtb"
UBOOT_CONFIG[programmer_stm32mp15] ?= "stm32mp15_defconfig,,u-boot.dtb"
UBOOT_CONFIG[programmer_stm32mp13] ?= "stm32mp13_defconfig,,u-boot.dtb"
UBOOT_CONFIG[programmer_stm32mp21] ?= "stm32mp21_defconfig,,u-boot.dtb"
UBOOT_CONFIG[programmer_stm32mp23] ?= "stm32mp23_defconfig,,u-boot.dtb"
UBOOT_CONFIG[programmer_stm32mp25] ?= "stm32mp25_defconfig,,u-boot.dtb"
UBOOT_CONFIG[fastboot-sdcard_stm32mp15] ?= "stm32mp15-fastboot-sdcard_defconfig,,u-boot.dtb"
UBOOT_CONFIG[fastboot-sdcard_stm32mp13] ?= "stm32mp13-fastboot-sdcard_defconfig,,u-boot.dtb"
UBOOT_CONFIG[fastboot-sdcard_stm32mp21] ?= "stm32mp21-fastboot-sdcard_defconfig,,u-boot.dtb"
UBOOT_CONFIG[fastboot-sdcard_stm32mp23] ?= "stm32mp23-fastboot-sdcard_defconfig,,u-boot.dtb"
UBOOT_CONFIG[fastboot-sdcard_stm32mp25] ?= "stm32mp25-fastboot-sdcard_defconfig,,u-boot.dtb"
UBOOT_CONFIG[fastboot-emmc_stm32mp13] ?= "stm32mp13-fastboot-emmc_defconfig,,u-boot.dtb"
UBOOT_CONFIG[fastboot-emmc_stm32mp15] ?= "stm32mp15-fastboot-emmc_defconfig,,u-boot.dtb"
UBOOT_CONFIG[fastboot-emmc_stm32mp21] ?= "stm32mp21-fastboot-emmc_defconfig,,u-boot.dtb"
UBOOT_CONFIG[fastboot-emmc_stm32mp23] ?= "stm32mp23-fastboot-emmc_defconfig,,u-boot.dtb"
UBOOT_CONFIG[fastboot-emmc_stm32mp25] ?= "stm32mp25-fastboot-emmc_defconfig,,u-boot.dtb"

# -----------------------------------------------
# Define U-Boot device tree to select for each U-Boot config
#
UBOOT_DEVICETREE[default_stm32mp13] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE}'.split() if 'stm32mp13' in dt)}"
UBOOT_DEVICETREE[default_stm32mp15] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE}'.split() if 'stm32mp15' in dt)}"
UBOOT_DEVICETREE[default_stm32mp21] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE}'.split() if 'stm32mp21' in dt)}"
UBOOT_DEVICETREE[default_stm32mp23] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE}'.split() if 'stm32mp23' in dt)}"
UBOOT_DEVICETREE[default_stm32mp25] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE}'.split() if 'stm32mp25' in dt)}"
UBOOT_DEVICETREE[programmer_stm32mp13] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp13' in dt)}"
UBOOT_DEVICETREE[programmer_stm32mp15] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp15' in dt)}"
UBOOT_DEVICETREE[programmer_stm32mp21] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp21' in dt)}"
UBOOT_DEVICETREE[programmer_stm32mp23] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp23' in dt)}"
UBOOT_DEVICETREE[programmer_stm32mp25] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp25' in dt)}"
UBOOT_DEVICETREE[fastboot-sdcard_stm32mp13] ?= "${@' '.join(dt for dt in '${STM32MP_DT_FILES_SDCARD}'.split() if 'stm32mp13' in dt)}"
UBOOT_DEVICETREE[fastboot-sdcard_stm32mp15] ?= "${@' '.join(dt for dt in '${STM32MP_DT_FILES_SDCARD}'.split() if 'stm32mp15' in dt)}"
UBOOT_DEVICETREE[fastboot-sdcard_stm32mp21] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp21' in dt)}"
UBOOT_DEVICETREE[fastboot-sdcard_stm32mp23] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp23' in dt)}"
UBOOT_DEVICETREE[fastboot-sdcard_stm32mp25] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp25' in dt)}"
UBOOT_DEVICETREE[fastboot-emmc_stm32mp13] ?= "${@' '.join(dt for dt in '${STM32MP_DT_FILES_EMMC}'.split() if 'stm32mp13' in dt)}"
UBOOT_DEVICETREE[fastboot-emmc_stm32mp15] ?= "${@' '.join(dt for dt in '${STM32MP_DT_FILES_EMMC}'.split() if 'stm32mp15' in dt)}"
UBOOT_DEVICETREE[fastboot-emmc_stm32mp21] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp21' in dt)}"
UBOOT_DEVICETREE[fastboot-emmc_stm32mp23] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp23' in dt)}"
UBOOT_DEVICETREE[fastboot-emmc_stm32mp25] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp25' in dt)}"

# -----------------------------------------------
# Define U-Boot device tree to select for each U-Boot config
#
UBOOT_EXTERNALDT_opt = "${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', 'EXT_DTS=${STAGING_EXTDT_DIR}/${EXTDT_DIR_UBOOT}', '', d)}"
UBOOT_EXTERNALDT_SERIAL_opt = "${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', 'EXT_DTS=${STAGING_EXTDT_DIR}/${EXTDT_DIR_UBOOT_SERIAL}', '', d)}"

UBOOT_MAKE_OPT[default_stm32mp13] ?= "${UBOOT_EXTERNALDT_opt}"
UBOOT_MAKE_OPT[default_stm32mp15] ?= "${UBOOT_EXTERNALDT_opt}"
UBOOT_MAKE_OPT[default_stm32mp21] ?= "${UBOOT_EXTERNALDT_opt}"
UBOOT_MAKE_OPT[default_stm32mp23] ?= "${UBOOT_EXTERNALDT_opt}"
UBOOT_MAKE_OPT[default_stm32mp25] ?= "${UBOOT_EXTERNALDT_opt}"
UBOOT_MAKE_OPT[programmer_stm32mp13] ?= "${UBOOT_EXTERNALDT_SERIAL_opt}"
UBOOT_MAKE_OPT[programmer_stm32mp15] ?= "${UBOOT_EXTERNALDT_SERIAL_opt}"
UBOOT_MAKE_OPT[programmer_stm32mp21] ?= "${UBOOT_EXTERNALDT_SERIAL_opt}"
UBOOT_MAKE_OPT[programmer_stm32mp23] ?= "${UBOOT_EXTERNALDT_SERIAL_opt}"
UBOOT_MAKE_OPT[programmer_stm32mp25] ?= "${UBOOT_EXTERNALDT_SERIAL_opt}"
UBOOT_MAKE_OPT[fastboot-sdcard_stm32mp13] ?= "${UBOOT_EXTERNALDT_SERIAL_opt}"
UBOOT_MAKE_OPT[fastboot-sdcard_stm32mp15] ?= "${UBOOT_EXTERNALDT_SERIAL_opt}"
UBOOT_MAKE_OPT[fastboot-sdcard_stm32mp21] ?= "${UBOOT_EXTERNALDT_SERIAL_opt}"
UBOOT_MAKE_OPT[fastboot-sdcard_stm32mp23] ?= "${UBOOT_EXTERNALDT_SERIAL_opt}"
UBOOT_MAKE_OPT[fastboot-sdcard_stm32mp25] ?= "${UBOOT_EXTERNALDT_SERIAL_opt}"
UBOOT_MAKE_OPT[fastboot-emmc_stm32mp13] ?= "${UBOOT_EXTERNALDT_SERIAL_opt}"
UBOOT_MAKE_OPT[fastboot-emmc_stm32mp15] ?= "${UBOOT_EXTERNALDT_SERIAL_opt}"
UBOOT_MAKE_OPT[fastboot-emmc_stm32mp21] ?= "${UBOOT_EXTERNALDT_SERIAL_opt}"
UBOOT_MAKE_OPT[fastboot-emmc_stm32mp23] ?= "${UBOOT_EXTERNALDT_SERIAL_opt}"
UBOOT_MAKE_OPT[fastboot-emmc_stm32mp25] ?= "${UBOOT_EXTERNALDT_SERIAL_opt}"
