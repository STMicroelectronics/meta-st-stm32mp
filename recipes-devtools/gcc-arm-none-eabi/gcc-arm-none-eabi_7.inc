SUMMARY = "Baremetal GCC for ARM"
LICENSE = "GPL-3.0-with-GCC-exception & GPLv3"

LIC_FILES_CHKSUM = "file://share/doc/gcc-arm-none-eabi/license.txt;md5=f77466c63f5787f4bd669c402aabe061"


SRC_URI = "https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/7-2017q4/gcc-arm-none-eabi-7-2017-q4-major-linux.tar.bz2;name=gcc-arm-none"

SRC_URI[gcc-arm-none.md5sum] = "d3b00ae09e847747ef11316a8b04989a"
SRC_URI[gcc-arm-none.sha256sum] = "96a029e2ae130a1210eaa69e309ea40463028eab18ba19c1086e4c2dafe69a6a"

PV = "7"

S = "${WORKDIR}/gcc-arm-none-eabi-7-2017-q4-major"

do_install() {
    install -d ${D}/${datadir}/gcc-arm-none-eabi
    cp -r ${S}/. ${D}/${datadir}/gcc-arm-none-eabi
}

INSANE_SKIP_${PN} = "already-stripped file-rdeps"
