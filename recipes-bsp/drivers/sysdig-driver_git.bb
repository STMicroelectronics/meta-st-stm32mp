SUMMARY = "A New System Troubleshooting Tool Built for the Way You Work"
DESCRIPTION = "Sysdig is open source, system-level exploration: capture \
system state and activity from a running Linux instance, then save, \
filter and analyze."
HOMEPAGE = "http://www.sysdig.org/"
LICENSE = "GPL-2.0-only"
LIC_FILES_CHKSUM = "file://COPYING;md5=f8fee3d59797546cffab04f3b88b2d44"

SRC_URI = "git://github.com/draios/sysdig.git;branch=dev;protocol=https;name=sysdig \
           git://github.com/falcosecurity/libs;protocol=https;branch=master;name=falco;subdir=git/falcosecurity-libs \
           file://0001-Add-cstdint-for-uintXX_t-types.patch;patchdir=./falcosecurity-libs \
           file://0001-cmake-Pass-PROBE_NAME-via-CFLAGS.patch \
          "
SRCREV_sysdig = "4fb6288275f567f63515df0ff0a6518043ecfa9b"
SRCREV_falco = "caa0e4d0044fdaaebab086592a97f0c7f32aeaa9"
SRCREV_FORMAT = "sysdig"

PV = "0.28.0+git${SRCPV}"

COMPATIBLE_MACHINE = "(stm32mpcommon)"

S = "${WORKDIR}/git"

# Inherit of cmake for configure step
inherit cmake pkgconfig

JIT ?= "jit"
JIT:mipsarchn32 = ""
JIT:mipsarchn64 = ""
JIT:riscv64 = ""
JIT:riscv32 = ""
JIT:powerpc = ""
JIT:powerpc64le = ""
JIT:powerpc64 = ""

DEPENDS += "libb64 lua${JIT} zlib c-ares grpc-native grpc curl ncurses jsoncpp \
            tbb jq openssl elfutils protobuf protobuf-native jq-native valijson"
#DEPENDS += "googletest"

RDEPENDS:${PN} = "bash"

OECMAKE_GENERATOR = "Unix Makefiles"

EXTRA_OECMAKE = ' -DBUILD_DRIVER="ON" \
                  -DBUILD_BPF="OFF" \
                  -DENABLE_DKMS="OFF" \
                  -DDIR_ETC="/etc" \
                  -DUSE_BUNDLED_DEPS=OFF \
                  -DMINIMAL_BUILD=ON \
                  -DCREATE_TEST_TARGETS=OFF \
                  -DDIR_ETC=${sysconfdir} \
                  -DLUA_INCLUDE_DIR=${STAGING_INCDIR}/luajit-2.1 \
                  -DFALCOSECURITY_LIBS_SOURCE_DIR=${S}/falcosecurity-libs \
                  -DVALIJSON_INCLUDE="${STAGING_LIBDIR_NATIVE}" \
                '

# Inherit of module class for driver building
inherit module

DEPENDS += "virtual/kernel"

export KERNELDIR = "${STAGING_KERNEL_BUILDDIR}"

cmake_do_compile[noexec] = "1"
cmake_do_install[noexec] = "1"

do_configure:prepend() {
    bbwarn "This kernel module REQUESTED to have CONFIG_FTRACE, CONFIG_TRACING, CONFIG_TRACEPOINTS activated"
}
do_compile:prepend() {
    cd ${B}/driver/src
}
do_install() {
	install -d ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/
	install -m 0755 ${B}/driver/src/scap.ko ${D}${nonarch_base_libdir}/modules/${KERNEL_VERSION}/extra/
}

FILES:${PN} += " ${base_libdir}/modules/${KERNEL_VERSION}/extra "

KERNEL_MODULES_META_PACKAGE = ""

RDEPENDS:${PN} = "bash"

do_create_runtime_spdx[depends] += "virtual/kernel:do_create_runtime_spdx"
