FILESEXTRAPATHS:prepend := "${THISDIR}/tf-a-tools:${THISDIR}/tf-a-stm32mp:"

SRC_URI:append = " \
    file://0001-tools-allow-to-use-a-root-key-password-from-command-.patch \
    file://0002-TOOLS-enable-native-target.patch \
    \
    file://create_st_fip_binary.sh \
    "

DEPENDS += "dtc-native openssl openssl-native"

TFA_PLATFORM ?= "stm32mp2"

COMPATIBLE_HOST:class-target = "null"

TOOL_HOSTCC:class-native = "${BUILD_CC}"
TOOL_CFLASG:class-native = "${BUILD_CFLAGS}"
TOOL_LDFLASG:class-native = "${BUILD_LDFLAGS}"
TOOL_HOSTCC:class-nativesdk = "${CC}"
TOOL_CFLASG:class-native = "${BUILD_CFLAGS}"
TOOL_LDFLASG:class-native = "${BUILD_LDFLAGS}"

EXTRA_OEMAKE += "V=1"
EXTRA_OEMAKE += "HOSTCC='${TOOL_HOSTCC}' TOOLS_CFLAGS='${TOOL_CFLASG}' TOOLS_LDFLAGS='${TOOL_LDFLASG}'"
EXTRA_OEMAKE += "OPENSSL_DIR='${STAGING_EXECPREFIXDIR}'"
EXTRA_OEMAKE += "PLAT=${TFA_PLATFORM}"
EXTRA_OEMAKE += "certtool enctool fiptool"

do_configure[noexec] = "1"

PARALLEL_MAKE = "-j 1"

do_compile:prepend:class-nativesdk () {
    sed -i 's/-pedantic//' ${S}/tools/fiptool/Makefile

    mkdir -p ${STAGING_EXECPREFIXDIR}/apps/
    echo "#!/bin/sh"  > ${STAGING_EXECPREFIXDIR}/apps/openssl
    echo "echo 'OpenSSL 3.5.5 27 Jan 2026 (Library: OpenSSL 3.5.5 27 Jan 2026)'" >> ${STAGING_EXECPREFIXDIR}/apps/openssl
    chmod +x ${STAGING_EXECPREFIXDIR}/apps/openssl
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 \
        ${B}/build/${TFA_PLATFORM}/release/tools/fiptool/fiptool \
        ${B}/build/${TFA_PLATFORM}/release/tools/cert_create/cert_create \
        ${B}/build/${TFA_PLATFORM}/release/tools/encrypt_fw/encrypt_fw \
        ${D}${bindir}/

    install -m 0755 ${UNPACKDIR}/create_st_fip_binary.sh ${D}${bindir}
}

BBCLASSEXTEND += "native nativesdk"
INSANE_SKIP:${PN} += "buildpaths rpaths"
