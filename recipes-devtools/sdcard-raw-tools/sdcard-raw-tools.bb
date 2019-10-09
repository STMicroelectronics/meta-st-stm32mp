# Copyright (C) 2017, STMicroelectronics - All Rights Reserved
# Released under the MIT license (see COPYING.MIT for the terms)

SUMMARY = "Script for creating raw SDCARD image ready to flash"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://create_sdcard_from_flashlayout.sh"

BBCLASSEXTEND = "native nativesdk"

RDEPENDS_${PN}_append = "bash"

RRECOMMENDS_${PN}_append_class-nativesdk = "nativesdk-gptfdisk"

inherit deploy

SCRIPT_DEPLOYDIR ?= "scripts"

do_install() {
    install -d ${D}/${bindir}
    install -m 0755 ${WORKDIR}/create_sdcard_from_flashlayout.sh ${D}/${bindir}
}

do_deploy() {
    :
}
do_deploy_class-native() {
    install -d ${DEPLOYDIR}/${SCRIPT_DEPLOYDIR}
    install -m 0755 ${WORKDIR}/create_sdcard_from_flashlayout.sh ${DEPLOYDIR}/${SCRIPT_DEPLOYDIR}/
}
addtask deploy before do_build after do_compile
