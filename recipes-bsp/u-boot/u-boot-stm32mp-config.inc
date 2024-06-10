# The format for u-boot configuration is: <conf-name>_<soc-name>
#   <conf-name>: used as U-Boot 'configuration name' in final U-Boot binaries
#   <soc-name> : used to filtered devicetree list to use

# -----------------------------------------------
# Handle U-Boot config to set internal var:
#   UBOOT_DEVICETREE
#
python () {
    ubootdevicetreeflags = d.getVarFlags('UBOOT_DEVICETREE')
    # The "doc" varflag is special, we don't want to see it here
    ubootdevicetreeflags.pop('doc', None)
    ubootconfig = (d.getVar('UBOOT_CONFIG') or "").split()

    if (d.getVar('UBOOT_DEVICETREE') or "").split():
        raise bb.parse.SkipRecipe("You cannot use UBOOT_DEVICETREE as it is internal for var expansion.")

    if len(ubootconfig) > 0 and len(ubootdevicetreeflags) > 0:
        for config in ubootconfig:
            found = False
            for f, v in ubootdevicetreeflags.items():
                if config == f:
                    found = True
                    # Make sure to get var flag properly expanded
                    v = d.getVarFlag('UBOOT_DEVICETREE', config)
                    if not v.strip():
                        bb.warn('[UBOOT_DEVICETREE] No devicetree configured for %s config' % config)
                    d.appendVar('UBOOT_DEVICETREE', v + ',')
                    break

            if not found:
                raise bb.parse.SkipRecipe("The selected UBOOT_CONFIG key %s has no match in UBOOT_DEVICETREE keys: %s." % (config, ubootdevicetreeflags.keys()))
}

# -----------------------------------------------
# Define U-Boot defconfig and binary to use for each UBOOT_CONFIG
#   UBOOT_CONFIG[config] ?= "<defconfig>,<fs-type images>,<binary to deploy>"
#
UBOOT_CONFIG[default_stm32mp15] ?= "stm32mp15_defconfig,,u-boot.dtb"
UBOOT_CONFIG[default_stm32mp13] ?= "stm32mp13_defconfig,,u-boot.dtb"
UBOOT_CONFIG[default_stm32mp25] ?= "stm32mp25_defconfig,,u-boot.dtb"
UBOOT_CONFIG[programmer_stm32mp15] ?= "stm32mp15_defconfig,,u-boot.dtb"
UBOOT_CONFIG[programmer_stm32mp13] ?= "stm32mp13_defconfig,,u-boot.dtb"
UBOOT_CONFIG[programmer_stm32mp25] ?= "stm32mp25_defconfig,,u-boot.dtb"
UBOOT_CONFIG[fastboot-sdcard_stm32mp15] ?= "stm32mp15-fastboot-sdcard_defconfig,,u-boot.dtb"
UBOOT_CONFIG[fastboot-sdcard_stm32mp13] ?= "stm32mp13-fastboot-sdcard_defconfig,,u-boot.dtb"
UBOOT_CONFIG[fastboot-sdcard_stm32mp25] ?= "stm32mp25-fastboot-sdcard_defconfig,,u-boot.dtb"
UBOOT_CONFIG[fastboot-emmc_stm32mp15] ?= "stm32mp15-fastboot-emmc_defconfig,,u-boot.dtb"
UBOOT_CONFIG[fastboot-emmc_stm32mp25] ?= "stm32mp25-fastboot-emmc_defconfig,,u-boot.dtb"

# -----------------------------------------------
# Define U-Boot device tree to select for each U-Boot config
#
UBOOT_DEVICETREE[default_stm32mp15] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE}'.split() if 'stm32mp15' in dt)}"
UBOOT_DEVICETREE[default_stm32mp13] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE}'.split() if 'stm32mp13' in dt)}"
UBOOT_DEVICETREE[default_stm32mp25] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE}'.split() if 'stm32mp25' in dt)}"
UBOOT_DEVICETREE[programmer_stm32mp15] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp15' in dt)}"
UBOOT_DEVICETREE[programmer_stm32mp13] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp13' in dt)}"
UBOOT_DEVICETREE[programmer_stm32mp25] ?= "${@' '.join(dt for dt in '${STM32MP_DEVICETREE_PROGRAMMER}'.split() if 'stm32mp25' in dt)}"
UBOOT_DEVICETREE[fastboot-sdcard_stm32mp15] ?= "${@' '.join(dt for dt in '${STM32MP_DT_FILES_SDCARD}'.split() if 'stm32mp15' in dt)}"
UBOOT_DEVICETREE[fastboot-sdcard_stm32mp13] ?= "${@' '.join(dt for dt in '${STM32MP_DT_FILES_SDCARD}'.split() if 'stm32mp13' in dt)}"
UBOOT_DEVICETREE[fastboot-sdcard_stm32mp25] ?= "${@' '.join(dt for dt in '${STM32MP_DT_FILES_SDCARD}'.split() if 'stm32mp25' in dt)}"
UBOOT_DEVICETREE[fastboot-emmc_stm32mp15] ?= "${@' '.join(dt for dt in '${STM32MP_DT_FILES_EMMC}'.split() if 'stm32mp15' in dt)}"
UBOOT_DEVICETREE[fastboot-emmc_stm32mp25] ?= "${@' '.join(dt for dt in '${STM32MP_DT_FILES_EMMC}'.split() if 'stm32mp25' in dt)}"
