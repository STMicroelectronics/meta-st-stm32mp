SUMMARY = "Cert_create & Fiptool for fip generation for Trusted Firmware-A"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://license.rst;md5=1dd070c98a281d18d9eefd938729b031"

SRC_URI = "git://github.com/ARM-software/arm-trusted-firmware.git;protocol=https;branch=master \
           file://0001-tools-allow-to-use-a-root-key-password-from-command-.patch \
           "

SRCREV = "35f4c7295bafeb32c8bcbdfb6a3f2e74a57e732b"

DEPENDS = "openssl"

COMPATIBLE_HOST:class-target = "null"

S = "${WORKDIR}/git"

EXTRA_OEMAKE += "V=1 HOSTCC='${BUILD_CC}' OPENSSL_DIR='${STAGING_EXECPREFIXDIR}'"
EXTRA_OEMAKE += "certtool fiptool"

do_configure[noexec] = "1"

do_compile:prepend () {
    # This is still needed to have the native fiptool executing properly by
    # setting the RPATH
    sed -e '/^LDLIBS/ s,$, \$\{BUILD_LDFLAGS},' \
        -e '/^INCLUDE_PATHS/ s,$, \$\{BUILD_CFLAGS},' \
        -i ${S}/tools/fiptool/Makefile
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 \
        ${B}/tools/fiptool/fiptool \
        ${B}/tools/cert_create/cert_create \
        ${D}${bindir}
}

BBCLASSEXTEND += "native nativesdk"
