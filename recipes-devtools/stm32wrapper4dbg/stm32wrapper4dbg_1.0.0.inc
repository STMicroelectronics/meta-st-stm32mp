SUMMARY = "Wrapper for FSBL to debug TF-A U-Boot and bare metal on STM32MP1"
LICENSE = "GPLv2+ | BSD-3-Clause"
LIC_FILES_CHKSUM = "file://COPYING;md5=7c996e24cb10a869efb08b521b20242f"

SRC_URI = "git://github.com/STMicroelectronics/stm32wrapper4dbg;protocol=https;branch=master"
SRCREV = "4b3887fa31754825b63608b7c88ce4cab472f6fd"

S = "${WORKDIR}/git"

SECTION = "devel"

do_install() {
    install -D -m 0755 stm32wrapper4dbg -t ${D}${bindir}
}
