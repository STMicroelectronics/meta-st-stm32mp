SUMMARY = "Wrapper for FSBL to debug TF-A U-Boot and bare metal on STM32MP"
SECTION = "devel"
LICENSE = "GPL-2.0-or-later | BSD-3-Clause"
LIC_FILES_CHKSUM = "file://COPYING;md5=7c6588c98f2a3299681e1bc746c60490"

SRC_URI = "git://github.com/STMicroelectronics/stm32wrapper4dbg;protocol=https;branch=main"
SRCREV = "239390db4412c3e919ee5c4b9e2dedf7391b7eaf"

BBCLASSEXTEND += "native nativesdk"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/stm32wrapper4dbg -t ${D}${bindir}
}
