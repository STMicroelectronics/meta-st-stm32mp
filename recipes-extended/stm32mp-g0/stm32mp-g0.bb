SUMMARY = "STM32MP Firmware for G0"
LICENSE = " \
    Apache-2.0 \
    & MIT \
    & BSD-3-Clause \
    & Proprietary \
    "

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

LIC_FILES_CHKSUM = "file://${S}/LICENSE.md;md5=f772aa2a812cebaa73f598d19990c9a1"

SRC_URI = "git://github.com/STMicroelectronics/STM32CubeG0.git;protocol=ssh;branch=master"

SRCREV = "03cb8e9ec0cbefad623caebe47359df0bab1d05e"

PV = "1.0-${SRCPV}"

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
