SUMMARY = "Baremetal GCC for ARM"
LICENSE = "GPL-3.0-with-GCC-exception & GPL-3.0-only"

LIC_FILES_CHKSUM = "file://share/info/gcc.info;md5=520901b4e94df9b4e99ad47d88719a6b"

SRC_URI = "https://developer.arm.com/-/media/Files/downloads/gnu/11.3.rel1/binrel/arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi.tar.xz;name=gcc-arm-x86"
SRC_URI[gcc-arm-x86.sha256sum] = "d420d87f68615d9163b99bbb62fe69e85132dc0a8cd69fca04e813597fe06121"

SRC_URI:aarch64 = "https://developer.arm.com/-/media/Files/downloads/gnu/11.3.rel1/binrel/arm-gnu-toolchain-11.3.rel1-aarch64-arm-none-eabi.tar.xz;name=gcc-arm-aarch64"
SRC_URI[gcc-arm-aarch64.sha256sum] = "6c713c11d018dcecc16161f822517484a13af151480bbb722badd732412eb55e"

PV = "11.3"

S = "${WORKDIR}/arm-gnu-toolchain-11.3.rel1-x86_64-arm-none-eabi"
S:aarch64 = "${WORKDIR}/arm-gnu-toolchain-11.3.rel1-aarch64-arm-none-eabi"
LIC_FILES_CHKSUM:aarch64 = "file://share/info/gcc.info;md5=f9c63f062ae6cdc4e0d0cc9106b6a469"

do_install() {
    install -d ${D}/${datadir}/gcc-arm-none-eabi
    cp -r ${S}/. ${D}/${datadir}/gcc-arm-none-eabi
}

INSANE_SKIP:${PN} = "already-stripped file-rdeps"
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_SYSROOT_STRIP = "1"
# Avoid QA Issue: contains reference to TMPDIR [buildpaths]
INSANE_SKIP:${PN} += "buildpaths"
