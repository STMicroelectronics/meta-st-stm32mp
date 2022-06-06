SUMMARY = "Wrapper for FSBL to debug TF-A U-Boot and bare metal on STM32MP1"
SECTION = "devel"
LICENSE = "GPL-2.0-or-later | BSD-3-Clause"
LIC_FILES_CHKSUM = "file://COPYING;md5=7c996e24cb10a869efb08b521b20242f"

SRC_URI = "git://gerrit.st.com:29418/mpu/oe/st/stm32wrapper4dbg;protocol=ssh;branch=stm32mp13-dev"
SRCREV = "b371b99df4f9af5397686290ebe597b47ab1d72a"

S = "${WORKDIR}/git"

BBCLASSEXTEND += "native nativesdk"

do_install() {
    install -d ${D}${bindir}
    install -m 0755 ${S}/stm32wrapper4dbg -t ${D}${bindir}
}
