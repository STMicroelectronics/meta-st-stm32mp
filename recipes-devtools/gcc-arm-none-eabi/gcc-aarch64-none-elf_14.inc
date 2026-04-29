SUMMARY = "Baremetal GCC for ARM aarch64"
LICENSE = "GPL-3.0-with-GCC-exception & GPL-3.0-only"

LIC_FILES_CHKSUM = "file://share/info/gcc.info;md5=0a3fd55b70b6e692c6af721b0c7a38f1"

SRC_URI = "https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-elf.tar.xz;name=gcc-arm-x86"
SRC_URI[gcc-arm-x86.sha256sum] = "eb54c4727440d03199a6af9a6d021e77f45410cad39effce4e5a1c10a88b7f04"

SRC_URI:aarch64 = "https://developer.arm.com/-/media/Files/downloads/gnu/14.2.rel1/binrel/arm-gnu-toolchain-14.2.rel1-aarch64-aarch64-none-elf.tar.xz;name=gcc-arm-aarch64"
SRC_URI[gcc-arm-aarch64.sha256sum] = "c4f0daab43f78e0d56ec2bdad76b98a0223ce12ce7fc51a6ce82f9cc6c6dfba0"

PV = "14.2"

S = "${UNPACKDIR}/arm-gnu-toolchain-14.2.rel1-x86_64-aarch64-none-elf"
S:aarch64 = "${UNPACKDIR}/arm-gnu-toolchain-14.2.rel1-aarch64-aarch64-none-elf"
LIC_FILES_CHKSUM:aarch64 = "file://share/info/gcc.info;md5=0a3fd55b70b6e692c6af721b0c7a38f1"

do_install() {
    install -d ${D}/${datadir}/gcc-aarch64-none-elf
    cp -r ${S}/. ${D}/${datadir}/gcc-aarch64-none-elf
}

INSANE_SKIP:${PN} = "already-stripped file-rdeps"
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_SYSROOT_STRIP = "1"
# Avoid QA Issue: contains reference to TMPDIR [buildpaths]
INSANE_SKIP:${PN} += "buildpaths"
