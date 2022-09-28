SUMMARY = "STM32MP Firmware for G0"
LICENSE = " \
    Apache-2.0 \
    & MIT \
    & BSD-3-Clause \
    & Proprietary \
    "

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

LIC_FILES_CHKSUM = "file://${S}/LICENSE.md;md5=57d9005898cfafdad212456569f8397e"

SRC_URI = "git://github.com/STMicroelectronics/x-cube-ucsi.git;protocol=https;branch=main"

SRCREV = "9644ad72c9a3760c56bd967dc5c490a5a8373446"

PV = "v1.0.0"

S = "${WORKDIR}/git"

STM32MP_G0_FW ?= "stm32g0-ucsi.mp135f-dk.fw"
STM32MP_G0_PROJECT = "${@bb.utils.contains('MACHINE_FEATURES', 'usbg0', 'Projects/STM32MP135F-DK/Applications/USB_PD/UCSI_DRP', '', d)}"

FIRMWARE_INSTALL_DIR = "${nonarch_base_libdir}/firmware"

do_install() {
    install -d ${D}${FIRMWARE_INSTALL_DIR}

    for proj in ${STM32MP_G0_PROJECT}; do
        if [ -s "${S}/${proj}/Binary/${STM32MP_G0_FW}" ]; then
            bbdebug 1 "Install binary firmware for ${proj}"
            install -m 0644 ${S}/${proj}/Binary/${STM32MP_G0_FW} ${D}${FIRMWARE_INSTALL_DIR}/${STM32MP_G0_FW}
        else
            bbwarn "Cannot install ${STM32MP_G0_FW}: file does not exist."
        fi
    done
}

FILES:${PN} += "${FIRMWARE_INSTALL_DIR}"
