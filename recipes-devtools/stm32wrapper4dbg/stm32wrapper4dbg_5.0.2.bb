SUMMARY = "Wrapper for FSBL to debug TF-A U-Boot and bare metal on STM32MP"
SECTION = "devel"
LICENSE = "GPL-2.0-or-later | BSD-3-Clause"
LIC_FILES_CHKSUM = "file://COPYING;md5=7c996e24cb10a869efb08b521b20242f"

SRC_URI = "git://github.com/STMicroelectronics/stm32wrapper4dbg.git;protocol=https;branch=main"
SRCREV = "84b6f2a944f34bf381ab8d3ad097a9c68a691b85"

BBCLASSEXTEND += "native nativesdk"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/stm32wrapper4dbg -t ${D}${bindir}
}
