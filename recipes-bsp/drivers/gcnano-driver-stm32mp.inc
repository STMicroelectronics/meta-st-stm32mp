COMPATIBLE_MACHINE = "(stm32mpcommon)"

# To share stm32mp1 patches between layers
FILESEXTRAPATHS_prepend := "${THISDIR}/gcnano-driver-stm32mp:"

inherit module

# todo manage android build (sync)
EXTRA_OEMAKE  = "KERNEL_DIR=${STAGING_KERNEL_BUILDDIR}"
EXTRA_OEMAKE += "SOC_PLATFORM=st-st"
# Use DEBUG=1 for more traces and easier debug
EXTRA_OEMAKE += "DEBUG=0"
EXTRA_OEMAKE += "-C ${STAGING_KERNEL_BUILDDIR} M=${S}"
EXTRA_OEMAKE += "AQROOT=${S}"
