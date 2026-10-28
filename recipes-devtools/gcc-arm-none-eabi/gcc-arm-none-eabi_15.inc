SUMMARY = "Baremetal GCC for ARM"
LICENSE = "GPL-3.0-with-GCC-exception & GPL-3.0-only"

LIC_FILES_CHKSUM = "file://share/info/gcc.info;md5=adeab8f3686e5c025c13c1854ee8f23a"

SRC_URI = "https://developer.arm.com/-/media/Files/downloads/gnu/15.2.rel1/binrel/arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi.tar.xz;name=gcc-arm-x86"
SRC_URI[gcc-arm-x86.sha256sum] = "597893282ac8c6ab1a4073977f2362990184599643b4c5ee34870a8215783a16"

SRC_URI:aarch64 = "https://developer.arm.com/-/media/Files/downloads/gnu/15.2.rel1/binrel/arm-gnu-toolchain-15.2.rel1-aarch64-arm-none-eabi.tar.xz;name=gcc-arm-aarch64"
SRC_URI[gcc-arm-aarch64.sha256sum] = "d061559d814b205ed30c5b7c577c03317ec447ca51cd5a159d26b12a5bbeb20c"

PV = "15.2"

S = "${UNPACKDIR}/arm-gnu-toolchain-15.2.rel1-x86_64-arm-none-eabi"
S:aarch64 = "${UNPACKDIR}/arm-gnu-toolchain-15.2.rel1-aarch64-arm-none-eabi"
LIC_FILES_CHKSUM:aarch64 = "file://share/info/gcc.info;md5=adeab8f3686e5c025c13c1854ee8f23a"

do_install() {
    install -d ${D}/${datadir}/gcc-arm-none-eabi
    cp -r ${S}/. ${D}/${datadir}/gcc-arm-none-eabi
}

INSANE_SKIP:${PN} = "already-stripped file-rdeps"
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_SYSROOT_STRIP = "1"
# Avoid QA Issue: contains reference to TMPDIR [buildpaths]
INSANE_SKIP:${PN} += "buildpaths"
