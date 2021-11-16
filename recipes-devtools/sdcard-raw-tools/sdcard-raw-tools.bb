# Copyright (C) 2017, STMicroelectronics - All Rights Reserved
# Released under the MIT license (see COPYING.MIT for the terms)

SUMMARY = "Script for creating raw SDCARD image ready to flash"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://create_sdcard_from_flashlayout.sh"

BBCLASSEXTEND = "native nativesdk"

RDEPENDS_${PN}_append = " bash "

RRECOMMENDS_${PN}_append_class-nativesdk = " nativesdk-gptfdisk "

inherit deploy

SCRIPT_DEPLOYDIR ?= "scripts"

do_configure() {
    if [ -e ${WORKDIR}/create_sdcard_from_flashlayout.sh ]; then
        bbnote "Update DEFAULT_ROOTFS_PARTITION_SIZE to ${STM32MP_ROOTFS_SIZE}"
        sed 's/^DEFAULT_ROOTFS_PARTITION_SIZE=.*$/DEFAULT_ROOTFS_PARTITION_SIZE='"${STM32MP_ROOTFS_SIZE}"'/' -i ${WORKDIR}/create_sdcard_from_flashlayout.sh
        if [ ${STM32MP_ROOTFS_SIZE} -gt 1572864 ]; then
            # rootfs > 1.5GB then put sdcard raw size = STM32MP_ROOTFS_SIZE + 1.5GB
            raw_size=$(expr ${STM32MP_ROOTFS_SIZE} / 1024 )
            raw_size=$(expr $raw_size + 1536)
            sed 's/^DEFAULT_RAW_SIZE=.*$/DEFAULT_RAW_SIZE='"$raw_size"'/' -i ${WORKDIR}/create_sdcard_from_flashlayout.sh
        fi

        bbnote "Update DEFAULT_SDCARD_PARTUUID to ${DEVICE_PARTUUID_ROOTFS_SDCARD}"
        sed 's/^DEFAULT_SDCARD_PARTUUID=.*$/DEFAULT_SDCARD_PARTUUID='"${DEVICE_PARTUUID_ROOTFS_SDCARD}"'/' -i ${WORKDIR}/create_sdcard_from_flashlayout.sh
    fi
}

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
