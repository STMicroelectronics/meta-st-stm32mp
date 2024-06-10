SUMMARY = "Firmware for DDR PHY on STM32MP"
LICENSE = "Proprietary"
LIC_FILES_CHKSUM = "file://${S}/LICENSE.md;md5=bb8009a40d2aca1844e6eb550bf8a6bc"

SRC_URI = "git://github.com/STMicroelectronics/stm32-ddr-phy-binary.git;protocol=https;branch=main"
SRCREV = "77447cf214eadf128e487fcb10a4a78cd4ab6d56"

PV = "A2022.11"

S = "${WORKDIR}/git"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(stm32mpcommon)"

STM32MP_DDR_DIR ?= ""
STM32MP_DDR_DIR:stm32mp2common = "stm32mp2"

do_compile() {
	:
}

do_install() {
    [ -z "${STM32MP_DDR_DIR}" ] && return
    if [ -d "${S}/${STM32MP_DDR_DIR}" ]; then
        install -d ${D}${datadir}/${STM32MP_DDR_DIR}
        install -m 0644 ${S}/${STM32MP_DDR_DIR}/* ${D}${datadir}/${STM32MP_DDR_DIR}
    fi
}

FILES:${PN} += "${datadir}"

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'stm32mp-ddr-phy-archiver.inc','')}
