SUMMARY = "Baremetal GCC for ARM"
LICENSE = "GPL-3.0-with-GCC-exception & GPL-3.0-only"

LIC_FILES_CHKSUM = "file://share/info/gcc.info;md5=32f1a898d3ddd89ccb368ff8f483c98a"

SRC_URI = "https://developer.arm.com/-/media/Files/downloads/gnu/11.2-2022.02/binrel/gcc-arm-11.2-2022.02-x86_64-arm-none-eabi.tar.xz;name=gcc-arm-x86"
SRC_URI[gcc-arm-x86.sha256sum] = "8c5acd5ae567c0100245b0556941c237369f210bceb196edfe5a2e7532c60326"

SRC_URI:aarch64 = "https://developer.arm.com/-/media/Files/downloads/gnu/11.2-2022.02/binrel/gcc-arm-11.2-2022.02-x86_64-aarch64-none-linux-gnu.tar.xz;name=gcc-arm-aarch64"
SRC_URI[gcc-arm-aarch64.sha256sum] = "52dbac3eb71dbe0916f60a8c5ab9b7dc9b66b3ce513047baa09fae56234e53f3"

PV = "11"

S = "${WORKDIR}/gcc-arm-11.2-2022.02-x86_64-arm-none-eabi"
S:aarch64 = "${WORKDIR}/gcc-arm-11.2-2022.02-x86_64-aarch64-none-linux-gnu"

do_install() {
    install -d ${D}/${datadir}/gcc-arm-none-eabi
    cp -r ${S}/. ${D}/${datadir}/gcc-arm-none-eabi
}

INSANE_SKIP:${PN} = "already-stripped file-rdeps"
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_SYSROOT_STRIP = "1"
