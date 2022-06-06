require u-boot-stm32mp-common_${PV}.inc
require u-boot-stm32mp.inc

SUMMARY = "Universal Boot Loader for embedded devices for stm32mp"
LICENSE = "GPL-2.0-or-later"

PROVIDES += "u-boot"
RPROVIDES:${PN} += "u-boot"

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'u-boot-stm32mp-archiver.inc','')}
