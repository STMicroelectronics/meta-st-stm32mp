COMPATIBLE_MACHINE = "(stm32mpcommon)"

inherit module

# todo manage android build (sync)
EXTRA_OEMAKE  = "KERNEL_DIR=${STAGING_KERNEL_BUILDDIR}"
# Use DEBUG=1 for more traces and easier debug
EXTRA_OEMAKE += "DEBUG=0"
EXTRA_OEMAKE += "-C ${STAGING_KERNEL_BUILDDIR} M=${S}"
EXTRA_OEMAKE += "AQROOT=${S}"

EXTRA_OEMAKE:append:stm32mp1common = " SOC_PLATFORM=st-mp1 "
