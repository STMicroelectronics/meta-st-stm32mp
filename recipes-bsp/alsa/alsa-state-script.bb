# Copyright (C) 2018, STMicroelectronics - All Rights Reserved
# Released under the MIT license (see COPYING.MIT for the terms)

SUMMARY = "Alsa script file for alsa state restoration"
HOMEPAGE = "http://www.alsa-project.org/"
DESCRIPTION = "Alsa Script Files - somes scripts files to restore \
sound state at system boot and save it at system shut down."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

PV = "1.0"

SRC_URI = " \
    file://system-generator-alsa-states \
    file://system-generator-alsa-conf   \
    \
    file://alsa-state-stm32mp.service \
    "

S = "${WORKDIR}"

COMPATIBLE_MACHINE = "(stm32mpcommon)"
RDEPENDS:${PN} = "alsa-state"

do_install() {
    # Enable systemd automatic selection
    if ${@bb.utils.contains('DISTRO_FEATURES','systemd','true','false',d)}; then
        install -d ${D}${bindir}
        if [ -f ${WORKDIR}/system-generator-alsa-states ]; then
            install -m 0755 ${WORKDIR}/system-generator-alsa-states ${D}${bindir}
        fi
        if [ -f ${WORKDIR}/system-generator-alsa-conf ]; then
            install -m 0755 ${WORKDIR}/system-generator-alsa-conf ${D}${bindir}
        fi

        install -d ${D}${systemd_unitdir}/system
        install -m 644 ${WORKDIR}/alsa-state-stm32mp.service ${D}/${systemd_unitdir}/system

    fi
}

FILES:${PN} = "${systemd_unitdir}/system ${bindir}"

# -----------------------------------------------------------
# specific for service
inherit systemd
SYSTEMD_PACKAGES += " ${PN} "
SYSTEMD_SERVICE:${PN} = "alsa-state-stm32mp.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"
