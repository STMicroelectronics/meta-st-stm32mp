# Copyright (C) 2018, STMicroelectronics - All Rights Reserved
# Released under the MIT license (see COPYING.MIT for the terms)

SUMMARY = "Alsa scenario files to enable alsa state restoration"
HOMEPAGE = "http://www.alsa-project.org/"
DESCRIPTION = "Alsa Scenario Files - an init script and state files to restore \
sound state at system boot and save it at system shut down."
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

PV = "1.0"

# Machine generic
SRC_URI = " \
    file://asound-stm32mp157c-ev.conf   \
    file://asound-stm32mp157c-dk.conf   \
    \
    file://asound-stm32mp157c-ev.state  \
    file://asound-stm32mp157c-dk.state  \
    \
    file://system-generator-alsa-states \
    file://system-generator-alsa-conf   \
    "

S = "${WORKDIR}"

COMPATIBLE_MACHINE = "(stm32mpcommon)"
RDEPENDS_${PN} = "alsa-state"

do_install() {
    install -d ${D}${sysconfdir}
    install -m 0644 ${WORKDIR}/*.conf ${D}${sysconfdir}/
    install -d ${D}/${localstatedir}/lib/alsa
    install -m 0644 ${WORKDIR}/*.state ${D}${localstatedir}/lib/alsa

    # Enable systemd automatic selection
    if ${@bb.utils.contains('DISTRO_FEATURES','systemd','true','false',d)}; then
        install -d ${D}${systemd_unitdir}/system-generators/
        if [ -f ${WORKDIR}/system-generator-alsa-states ]; then
            install -m 0755 ${WORKDIR}/system-generator-alsa-states ${D}${systemd_unitdir}/system-generators/
        fi
        if [ -f ${WORKDIR}/system-generator-alsa-conf ]; then
            install -m 0755 ${WORKDIR}/system-generator-alsa-conf ${D}${systemd_unitdir}/system-generators/
        fi
    fi
}

FILES_${PN} = "${localstatedir}/lib/alsa/*.state ${systemd_unitdir}/system-generators ${sysconfdir}/*.conf "
