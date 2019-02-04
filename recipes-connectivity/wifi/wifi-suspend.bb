# Copyright (C) 2018, STMicroelectronics - All Rights Reserved

DESCRIPTION = "Systemd service to suspend/resume correctly the wifi"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

SRC_URI = " \
    file://wifi_brcmfmac_driver.sh \
    file://wifi-brcmfmac-sleep.service \
    "

inherit systemd
SYSTEMD_PACKAGES += "${PN}"
SYSTEMD_SERVICE_${PN} = "wifi-brcmfmac-sleep.service"

do_install() {
    install -d ${D}${bindir}
    install -m 0755  ${WORKDIR}/wifi_brcmfmac_driver.sh ${D}${bindir}/

    if ${@bb.utils.contains('DISTRO_FEATURES','systemd','true','false',d)}; then
        install -d ${D}${systemd_unitdir}/system/

        install -m 0644 ${WORKDIR}/wifi-brcmfmac-sleep.service ${D}${systemd_unitdir}/system/
    fi
}

ALLOW_EMPTY_${PN} = "1"

