SUMMARY = "Baremetal GCC for ARM"
LICENSE = "GPL-3.0-with-GCC-exception & GPL-3.0-only"

LIC_FILES_CHKSUM = "file://share/info/gcc.info;md5=0a3fd55b70b6e692c6af721b0c7a38f1"

SRC_URI = "https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi.tar.xz;name=gcc-arm-x86"
SRC_URI[gcc-arm-x86.sha256sum] = "62a63b981fe391a9cbad7ef51b17e49aeaa3e7b0d029b36ca1e9c3b2a9b78823"

SRC_URI:aarch64 = "https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-aarch64-arm-none-eabi.tar.xz;name=gcc-arm-aarch64"
SRC_URI[gcc-arm-aarch64.sha256sum] = "87330bab085dd8749d4ed0ad633674b9dc48b237b61069e3b481abd364d0a684"

PV = "14.2"

S = "${UNPACKDIR}/arm-gnu-toolchain-14.2.rel1-x86_64-arm-none-eabi"
S:aarch64 = "${UNPACKDIR}/arm-gnu-toolchain-14.2.rel1-aarch64-arm-none-eabi"
LIC_FILES_CHKSUM:aarch64 = "file://share/info/gcc.info;md5=0a3fd55b70b6e692c6af721b0c7a38f1"

do_install() {
    install -d ${D}/${datadir}/gcc-arm-none-eabi
    cp -r ${S}/. ${D}/${datadir}/gcc-arm-none-eabi
}

INSANE_SKIP:${PN} = "already-stripped file-rdeps"
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_SYSROOT_STRIP = "1"
# Avoid QA Issue: contains reference to TMPDIR [buildpaths]
INSANE_SKIP:${PN} += "buildpaths"
