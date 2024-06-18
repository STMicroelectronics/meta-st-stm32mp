COMPATIBLE_MACHINE = "(stm32mpcommon)"

inherit module

# todo manage android build (sync)
EXTRA_OEMAKE  = "KERNEL_DIR=${STAGING_KERNEL_BUILDDIR}"
EXTRA_OEMAKE:append:stm32mp1common = " SOC_PLATFORM=st-mp1"
EXTRA_OEMAKE:append:stm32mp2common = " SOC_PLATFORM=st-mp2"
# Use DEBUG=1 for more traces and easier debug
EXTRA_OEMAKE:append = " DEBUG=0"
EXTRA_OEMAKE:append = " -C ${STAGING_KERNEL_BUILDDIR} M=${S}"
EXTRA_OEMAKE:append = " AQROOT=${S}"

do_create_runtime_spdx[depends] += "virtual/kernel:do_create_runtime_spdx"
