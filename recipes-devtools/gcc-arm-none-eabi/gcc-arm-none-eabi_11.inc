SUMMARY = "Baremetal GCC for ARM"
LICENSE = "GPL-3.0-with-GCC-exception & GPL-3.0-only"

LIC_FILES_CHKSUM = "file://share/info/gcc.info;md5=32f1a898d3ddd89ccb368ff8f483c98a"
LIC_FILES_CHKSUM:aarch64 = "file://share/info/gcc.info;md5=dfc35c9a2dc2669a7eb33db64ef328cc"

SRC_URI = "https://developer.arm.com/-/media/Files/downloads/gnu/11.2-2022.02/binrel/gcc-arm-11.2-2022.02-x86_64-arm-none-eabi.tar.xz;name=gcc-arm-x86"
SRC_URI[gcc-arm-x86.sha256sum] = "8c5acd5ae567c0100245b0556941c237369f210bceb196edfe5a2e7532c60326"

SRC_URI:aarch64 = "https://developer.arm.com/-/media/Files/downloads/gnu/11.2-2022.02/binrel/gcc-arm-11.2-2022.02-aarch64-arm-none-eabi.tar.xz;name=gcc-arm-aarch64"
SRC_URI[gcc-arm-aarch64.sha256sum] = "ef1d82e5894e3908cb7ed49c5485b5b95deefa32872f79c2b5f6f5447cabf55f"

PV = "11"

S = "${WORKDIR}/gcc-arm-11.2-2022.02-x86_64-arm-none-eabi"
S:aarch64 = "${WORKDIR}/gcc-arm-11.2-2022.02-aarch64-arm-none-eabi"

do_install() {
    install -d ${D}/${datadir}/gcc-arm-none-eabi
    cp -r ${S}/. ${D}/${datadir}/gcc-arm-none-eabi
}

INSANE_SKIP:${PN} = "already-stripped file-rdeps"
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_SYSROOT_STRIP = "1"
