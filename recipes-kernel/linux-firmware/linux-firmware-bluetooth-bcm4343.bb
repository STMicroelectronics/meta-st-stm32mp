# Copyright (C) 2018, STMicroelectronics - All Rights Reserved

SUMMARY = "Bluetooth firmware for BCM4343"
HOMEPAGE = "https://github.com/murata-wireless/cyw-bt-patch"
LICENSE = "Firmware-cypress-bcm4343"
LIC_FILES_CHKSUM = "file://LICENCE.cypress;md5=cbc5f665d04f741f1e006d2096236ba7"

NO_GENERIC_LICENSE[Firmware-cypress-bcm4343] = "LICENCE.cypress"

inherit allarch

SRC_URI = "git://github.com/murata-wireless/cyw-bt-patch;protocol=https;branch=master"
SRCREV = "9d24c254dae92af99ddfd661a4ea30af69190038"

PV = "3.1"

S = "${WORKDIR}/git"

PACKAGES =+ "${PN}-cypress-license"

do_install() {
    install -d ${D}${nonarch_base_libdir}/firmware/brcm/

    # 43430
    install -m 644 ${S}/BCM43430A1_001.002.009.0159.0528.1DX.hcd ${D}${nonarch_base_libdir}/firmware/brcm/BCM43430A1.hcd
    install -m 644 ${S}/LICENCE.cypress ${D}${nonarch_base_libdir}/firmware/LICENCE.cypress_bcm4343
    # 43439
    install -m 644 ${S}/CYW4343A2_001.003.016.0031.0000.1YN.hcd ${D}${nonarch_base_libdir}/firmware/brcm/BCM4343A2.hcd
    cd ${D}${nonarch_base_libdir}/firmware/brcm/
    ln -sf BCM4343A2.hcd BCM.st,stm32mp257f-dk.hcd
    ln -sf BCM4343A2.hcd BCM.st,stm32mp257f-dk-ca35tdcid-ostl.hcd
    ln -sf BCM4343A2.hcd BCM.st,stm32mp257f-dk-ca35tdcid-ostl-m33-examples.hcd
}

LICENSE:${PN} = "Firmware-cypress-bcm4343"
LICENSE:${PN}-cypress-license = "Firmware-cypress-bcm4343"

FILES:${PN}-cypress-license = "${nonarch_base_libdir}/firmware/LICENCE.cypress_bcm4343"
FILES:${PN} = "${nonarch_base_libdir}/firmware/"

RDEPENDS:${PN} += "${PN}-cypress-license"

RRECOMMENDS:${PN}:append:stm32mpcommon = " ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', 'bluetooth-suspend', '', d)} "

# Firmware files are generally not ran on the CPU, so they can be
# allarch despite being architecture specific
INSANE_SKIP = "arch"
