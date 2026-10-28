SUMMARY = "Baremetal GCC for ARM aarch64"
LICENSE = "GPL-3.0-with-GCC-exception & GPL-3.0-only"

LIC_FILES_CHKSUM = "file://share/info/gcc.info;md5=adeab8f3686e5c025c13c1854ee8f23a"

SRC_URI = "https://developer.arm.com/-/media/Files/downloads/gnu/15.2.rel1/binrel/arm-gnu-toolchain-15.2.rel1-x86_64-aarch64-none-elf.tar.xz;name=gcc-arm-x86"
SRC_URI[gcc-arm-x86.sha256sum] = "66f7ce7c1bf662f589a4caf440812375f3cd8000a033ccf0971127a0726d6921"

SRC_URI:aarch64 = "https://developer.arm.com/-/media/Files/downloads/gnu/15.2.rel1/binrel/arm-gnu-toolchain-15.2.rel1-aarch64-aarch64-none-elf.tar.xz;name=gcc-arm-aarch64"
SRC_URI[gcc-arm-aarch64.sha256sum] = "46195685b6aec1077e3f1b7706b43a6aa1fef4d8d3bff3a411b7dad1c5b1196b"

PV = "15.2"

S = "${UNPACKDIR}/arm-gnu-toolchain-15.2.rel1-x86_64-aarch64-none-elf"
S:aarch64 = "${UNPACKDIR}/arm-gnu-toolchain-15.2.rel1-aarch64-aarch64-none-elf"
LIC_FILES_CHKSUM:aarch64 = "file://share/info/gcc.info;md5=adeab8f3686e5c025c13c1854ee8f23a"

do_install() {
    install -d ${D}/${datadir}/gcc-aarch64-none-elf
    cp -r ${S}/. ${D}/${datadir}/gcc-aarch64-none-elf
}

INSANE_SKIP:${PN} = "already-stripped file-rdeps"
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_SYSROOT_STRIP = "1"
# Avoid QA Issue: contains reference to TMPDIR [buildpaths]
INSANE_SKIP:${PN} += "buildpaths"
