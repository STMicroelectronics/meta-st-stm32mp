require tf-m-stm32mp-common_${PV}.inc
require tf-m-stm32mp.inc

SUMMARY = "Trusted Firmware for Cortex-M"
DESCRIPTION = "Trusted Firmware-M"
HOMEPAGE = "https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git"
LICENSE = "BSD-3-Clause & Apache-2.0"

PROVIDES = "virtual/trusted-firmware-m"
PROVIDES:append:virtclass-multilib-lib64 = " virtual/trusted-firmware-m"

COMPATIBLE_MACHINE = "(stm32mp2common)"

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'tf-m-stm32mp-archiver.inc','')}
