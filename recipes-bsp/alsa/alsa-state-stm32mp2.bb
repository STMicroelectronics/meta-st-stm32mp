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
    file://asound-stm32mp25yx-dk.conf \
    file://asound-stm32mp25yx-dk.state \
    file://asound-stm32mp25yx-ev1.conf \
    file://asound-stm32mp25yx-ev1.state \
    "

S = "${WORKDIR}"

COMPATIBLE_MACHINE = "(stm32mp2common)"
RDEPENDS:${PN} = "alsa-state alsa-state-script"

do_install() {
    install -d ${D}${sysconfdir}
    install -m 0644 ${WORKDIR}/*.conf ${D}${sysconfdir}/
    install -d ${D}/${localstatedir}/lib/alsa
    install -m 0644 ${WORKDIR}/*.state ${D}${localstatedir}/lib/alsa

    # create link to support all packages configuration
    for p in a b c d e f; # a b c d e f
    do
        for n in 7; # 1 3 7
        do
            cd ${D}${sysconfdir}/
            ln -sf asound-stm32mp25yx-dk.conf asound-stm32mp25$n$p-dk.conf
            ln -sf asound-stm32mp25yx-dk.conf asound-stm32mp25$n$p-ev1.conf
            cd ${D}${localstatedir}/lib/alsa
            ln -sf asound-stm32mp25yx-dk.state asound-stm32mp25$n$p-dk.state
            ln -sf asound-stm32mp25yx-dk.state asound-stm32mp25$n$p-ev1.state
        done
    done
}

FILES:${PN} = "${localstatedir}/lib/alsa/*.state ${sysconfdir}/*.conf "
