SUMMARY = "Provides Device Tree files for STM32MP boards"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0-only;md5=801f80980d171dd6425610833a22dbe6"

SRC_URI = "git://github.com/STMicroelectronics/dt-stm32mp.git;protocol=https;branch=v7-stm32mp"
SRCREV = "ca7915be39c79379e256a4bc1a24b3dbfb5a558c"

EXT_DT_VERSION = "v7.0"
EXT_DT_RELEASE = "stm32mp-r1"

PV = "${EXT_DT_VERSION}-${EXT_DT_RELEASE}"

COMPATIBLE_MACHINE = "(stm32mpcommon)"

require external-dt-common.inc

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'external-dt-archiver.inc','')}
