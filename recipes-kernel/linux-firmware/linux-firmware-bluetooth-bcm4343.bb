# Copyright (C) 2018, STMicroelectronics - All Rights Reserved

DESCRIPTION = "Bluetooth firmware for BCM4343"
HOMEPAGE = "https://github.com/murata-wireless/cyw-bt-patch"
LICENSE = "Firmware-cypress-bcm4343"
LIC_FILES_CHKSUM = "file://LICENCE.cypress;md5=cbc5f665d04f741f1e006d2096236ba7"

SRC_URI = "git://github.com/murata-wireless/cyw-bt-patch;protocol=https"
SRCREV = "f819145223f1f99d3fc81a62c2ea2789d797d7b0"

S = "${WORKDIR}/git"

inherit allarch

do_install() {
    install -d ${D}${nonarch_base_libdir}/firmware/brcm/

    install -m 644 ${S}/CYW43430A1.1DX.hcd ${D}${nonarch_base_libdir}/firmware/brcm/BCM43430A1.hcd
    install -m 644 ${S}/LICENCE.cypress ${D}${nonarch_base_libdir}/firmware/LICENCE.cypress_bcm4343
}

PACKAGES =+ "${PN}-cypress-license"
FILES_${PN} = " ${nonarch_base_libdir}/firmware/ "

NO_GENERIC_LICENSE[Firmware-cypress-bcm4343] = "LICENCE.cypress"
LICENSE_${PN} = "Firmware-cypress-bcm4343"
RDEPENDS_${PN} += "${PN}-cypress-license"

LICENSE_${PN}-cypress-license = "Firmware-cypress-bcm4343"
FILES_${PN}-cypress-license = "${nonarch_base_libdir}/firmware/LICENCE.cypress_bcm4343"

# Firmware files are generally not ran on the CPU, so they can be
# allarch despite being architecture specific
INSANE_SKIP = "arch"
