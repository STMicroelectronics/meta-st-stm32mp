# Copyright (C) 2017, STMicroelectronics - All Rights Reserved
# Released under the MIT license (see COPYING.MIT for the terms)

SUMMARY = "Script for creating raw SDCARD image ready to flash"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://create_sdcard_from_flashlayout.sh"

S = "${UNPACKDIR}"

BBCLASSEXTEND = "native nativesdk"

RDEPENDS:${PN}:append = " bash "

RRECOMMENDS:${PN}:append:class-nativesdk = " nativesdk-gptfdisk "

inherit deploy

SCRIPT_DEPLOYDIR ?= "scripts"

do_configure() {
    if [ -e ${UNPACKDIR}/create_sdcard_from_flashlayout.sh ]; then
        bbnote "Update DEFAULT_ROOTFS_PARTITION_SIZE to ${STM32MP_ROOTFS_SIZE}"
        sed 's/^DEFAULT_ROOTFS_PARTITION_SIZE=\${ROOTFS_SIZE:.*$/DEFAULT_ROOTFS_PARTITION_SIZE=\${ROOTFS_SIZE:-'"${STM32MP_ROOTFS_SIZE:sdcard}"'}/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        if [ ${STM32MP_ROOTFS_SIZE:sdcard} -gt 1887232 ]; then
            # rootfs > 1.8GB then put sdcard raw size = STM32MP_ROOTFS_SIZE + 1.8GB
            raw_size=$(expr ${STM32MP_ROOTFS_SIZE:sdcard}  + ${STM32MP_USERFS_SIZE:sdcard})
            raw_size=$(expr $raw_size / 1024)
            raw_size=$(expr $raw_size + 2050)
            sed 's/DEFAULT_RAW_SIZE=\${SDCARD_SIZE:.*}$/DEFAULT_RAW_SIZE=\${SDCARD_SIZE:-'"$raw_size"'}/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        fi

        bbnote "Update DEFAULT_SDCARD_FS_PARTUUID to ${DEVICE_PARTUUID_ROOTFS:SDCARD}"
        sed 's/^DEFAULT_SDCARD_FS_PARTUUID=.*$/DEFAULT_SDCARD_FS_PARTUUID='"${DEVICE_PARTUUID_ROOTFS:SDCARD}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh

        # fip
        sed 's/^DEFAULT_FIP_TYPEUUID=.*$/DEFAULT_FIP_TYPEUUID='"${DEVICE_TYPEUUID_FIP}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        sed 's/^DEFAULT_FIP_A_PARTUUID=.*$/DEFAULT_FIP_A_PARTUUID='"${DEVICE_PARTUUID_FIP_A}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        sed 's/^DEFAULT_FIP_B_PARTUUID=.*$/DEFAULT_FIP_B_PARTUUID='"${DEVICE_PARTUUID_FIP_B}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        # FWU Mdata
        sed 's/^DEFAULT_FWU_MDATA_TYPEUUID=.*$/DEFAULT_FWU_MDATA_TYPEUUID='"${DEVICE_TYPEUUID_FWU_MDATA}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        sed 's/^DEFAULT_FWU_MDATA_A_PARTUUID=.*$/DEFAULT_FWU_MDATA_A_PARTUUID='"${DEVICE_PARTUUID_FWU_MDATA_1}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        sed 's/^DEFAULT_FWU_MDATA_B_PARTUUID=.*$/DEFAULT_FWU_MDATA_B_PARTUUID='"${DEVICE_PARTUUID_FWU_MDATA_2}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        # u-boot env
        sed 's/^DEFAULT_UBOOT_ENV_TYPEUUID=.*$/DEFAULT_UBOOT_ENV_TYPEUUID='"${DEVICE_TYPEUUID_UBOOT_ENV}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        # fs type typeuuid
        sed 's/^DEFAULT_BOOTFS_TYPEUUID=.*$/DEFAULT_BOOTFS_TYPEUUID='"${DEVICE_TYPEUUID_BOOTFS}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        sed 's/^DEFAULT_ROOTFS_TYPEUUID=.*$/DEFAULT_ROOTFS_TYPEUUID='"${DEVICE_TYPEUUID_ROOTFS}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh

        # bootfs
        sed 's/^DEFAULT_BOOTFS_SDCARD_PARTUUID=.*$/DEFAULT_BOOTFS_SDCARD_PARTUUID='"${DEVICE_PARTUUID_BOOTFS:mmc0}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        sed 's/^DEFAULT_BOOTFS_SDCARD_A_PARTUUID=.*$/DEFAULT_BOOTFS_SDCARD_A_PARTUUID='"${DEVICE_PARTUUID_BOOTFS_A:mmc0}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        sed 's/^DEFAULT_BOOTFS_SDCARD_B_PARTUUID=.*$/DEFAULT_BOOTFS_SDCARD_B_PARTUUID='"${DEVICE_PARTUUID_BOOTFS_B:mmc0}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        # vendorfs
        sed 's/^DEFAULT_VENDORFS_TYPEUUID=.*$/DEFAULT_VENDORFS_TYPEUUID='"${DEVICE_TYPEUUID_VENDORFS}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        sed 's/^DEFAULT_VENDORFS_SDCARD_PARTUUID=.*$/DEFAULT_VENDORFS_SDCARD_PARTUUID='"${DEVICE_PARTUUID_VENDORFS:mmc0}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        sed 's/^DEFAULT_VENDORFS_SDCARD_A_PARTUUID=.*$/DEFAULT_VENDORFS_SDCARD_A_PARTUUID='"${DEVICE_PARTUUID_VENDORFS_A:mmc0}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        sed 's/^DEFAULT_VENDORFS_SDCARD_B_PARTUUID=.*$/DEFAULT_VENDORFS_SDCARD_B_PARTUUID='"${DEVICE_PARTUUID_VENDORFS_B:mmc0}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        # rootfs
        sed 's/^DEFAULT_ROOTFS_TYPEUUID=.*$/DEFAULT_ROOTFS_TYPEUUID='"${DEVICE_TYPEUUID_ROOTFS}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        sed 's/^DEFAULT_ROOTFS_SDCARD_PARTUUID=.*$/DEFAULT_ROOTFS_SDCARD_PARTUUID='"${DEVICE_PARTUUID_ROOTFS:mmc0}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        sed 's/^DEFAULT_ROOTFS_SDCARD_A_PARTUUID=.*$/DEFAULT_ROOTFS_SDCARD_A_PARTUUID='"${DEVICE_PARTUUID_ROOTFS_A:mmc0}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        sed 's/^DEFAULT_ROOTFS_SDCARD_B_PARTUUID=.*$/DEFAULT_ROOTFS_SDCARD_B_PARTUUID='"${DEVICE_PARTUUID_ROOTFS_B:mmc0}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        # userfs
        sed 's/^DEFAULT_USERFS_TYPEUUID=.*$/DEFAULT_USERFS_TYPEUUID='"${DEVICE_TYPEUUID_USERFS}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        sed 's/^DEFAULT_USERFS_SDCARD_PARTUUID=.*$/DEFAULT_USERFS_SDCARD_PARTUUID='"${DEVICE_PARTUUID_USERFS:mmc0}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        sed 's/^DEFAULT_USERFS_SDCARD_A_PARTUUID=.*$/DEFAULT_USERFS_SDCARD_A_PARTUUID='"${DEVICE_PARTUUID_USERFS_A:mmc0}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh
        sed 's/^DEFAULT_USERFS_SDCARD_B_PARTUUID=.*$/DEFAULT_USERFS_SDCARD_B_PARTUUID='"${DEVICE_PARTUUID_USERFS_B:mmc0}"'/' -i ${UNPACKDIR}/create_sdcard_from_flashlayout.sh

    fi
}

do_install() {
    install -d ${D}/${bindir}
    install -m 0755 ${UNPACKDIR}/create_sdcard_from_flashlayout.sh ${D}/${bindir}
}

do_deploy() {
    :
}
do_deploy:class-native() {
    install -d ${DEPLOYDIR}/${SCRIPT_DEPLOYDIR}
    install -m 0755 ${UNPACKDIR}/create_sdcard_from_flashlayout.sh ${DEPLOYDIR}/${SCRIPT_DEPLOYDIR}/
}
addtask deploy before do_build after do_compile
