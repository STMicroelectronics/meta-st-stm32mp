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
    file://asound-stm32mp15yx-ev.conf   \
    file://asound-stm32mp15yx-dk.conf   \
    \
    file://asound-stm32mp15yx-ev.state  \
    file://asound-stm32mp15yx-dk.state  \
    "

S = "${UNPACKDIR}"

COMPATIBLE_MACHINE = "(stm32mpcommon)"
RDEPENDS:${PN} = "alsa-state alsa-state-script"

do_install() {
    install -d ${D}${sysconfdir}
    install -m 0644 ${UNPACKDIR}/*.conf ${D}${sysconfdir}/
    install -d ${D}/${localstatedir}/lib/alsa
    install -m 0644 ${UNPACKDIR}/*.state ${D}${localstatedir}/lib/alsa

    # create link to support all packages configuration
    for p in a b c d e f;
    do
        for n in 1 3 7;
        do
            cd ${D}${sysconfdir}/
            ln -sf asound-stm32mp15yx-ev.conf asound-stm32mp15$n$p-ev.conf
            ln -sf asound-stm32mp15yx-dk.conf asound-stm32mp15$n$p-dk.conf
            cd ${D}${localstatedir}/lib/alsa
            ln -sf asound-stm32mp15yx-ev.state asound-stm32mp15$n$p-ev.state
            ln -sf asound-stm32mp15yx-dk.state asound-stm32mp15$n$p-dk.state
        done
    done
}

FILES:${PN} = "${localstatedir}/lib/alsa/*.state ${sysconfdir}/*.conf "
