SUMMARY = "A New System Troubleshooting Tool Built for the Way You Work"
DESCRIPTION = "Sysdig is open source, system-level exploration: capture \
system state and activity from a running Linux instance, then save, \
filter and analyze."
HOMEPAGE = "http://www.sysdig.org/"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=f8fee3d59797546cffab04f3b88b2d44"

SRC_URI = "git://github.com/draios/sysdig.git;protocol=https"
SRCREV = "aa82b2fb329ea97a8ade31590954ddaa675e1728"

PV = "0.24.2+git${SRCPV}"

COMPATIBLE_MACHINE = "(stm32mpcommon)"

S = "${WORKDIR}/git"

# Inherit of cmake for configure step
inherit cmake pkgconfig

DEPENDS = "luajit zlib ncurses"
DEPENDS += "jsoncpp openssl curl jq"
DEPENDS += "tbb"
DEPENDS += "libb64"
DEPENDS += "elfutils"

OECMAKE_GENERATOR = "Unix Makefiles"

EXTRA_OECMAKE = ' -DBUILD_DRIVER="ON" \
                  -DBUILD_BPF="OFF" \
                  -DENABLE_DKMS="OFF" \
                  -DDIR_ETC="/etc" \
                '

EXTRA_OECMAKE += ' -DUSE_BUNDLED_LUAJIT="OFF" \
                   -DUSE_BUNDLED_ZLIB="OFF" \
                   -DUSE_BUNDLED_NCURSES="OFF" \
                   -DUSE_BUNDLED_JSONCPP="OFF" \
                   -DUSE_BUNDLED_OPENSSL="OFF" \
                   -DUSE_BUNDLED_CURL="OFF" \
                   -DUSE_BUNDLED_B64="OFF" \
                   -DUSE_BUNDLED_JQ="OFF" \
                   -DUSE_BUNDLED_TBB="OFF" \
                 '

# Inherit of module class for driver building
inherit module

DEPENDS += "virtual/kernel"

export KERNELDIR = "${STAGING_KERNEL_BUILDDIR}"

MAKE_TARGETS = "all"

# Compile prepend for cmake build first
do_compile_prepend() {
	cmake_runcmake_build --target ${OECMAKE_TARGET_COMPILE}
}

do_install() {
	install -d ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/
	install -m 0755 ${B}/driver/sysdig-probe.ko ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/
}

FILES_${PN} += " ${base_libdir}/modules/${KERNEL_VERSION}/extra "

KERNEL_MODULES_META_PACKAGE = ""

RDEPENDS_${PN} = "bash"
