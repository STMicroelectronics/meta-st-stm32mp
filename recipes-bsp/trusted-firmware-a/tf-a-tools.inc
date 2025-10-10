FILESEXTRAPATHS:prepend := "${THISDIR}/tf-a-tools:${THISDIR}/tf-a-stm32mp:"

SRC_URI:append = " \
    file://0001-FIX-GCC-tools-overwrite.patch \
    file://0001-tools-allow-to-use-a-root-key-password-from-command-.patch \
    \
    file://create_st_fip_binary.sh \
    "

DEPENDS += "dtc-native openssl"

COMPATIBLE_HOST:class-target = "null"

HOSTCC:class-native = "${BUILD_CC}"
HOSTCC:class-nativesdk = "${CC}"

EXTRA_OEMAKE += "HOSTCC='${HOSTCC}' OPENSSL_DIR='${STAGING_EXECPREFIXDIR}'"
# EXTRA_OEMAKE += "certtool enctool fiptool"
# EXTRA_OEMAKE += "PLAT=${TFA_PLATFORM}"

do_configure[noexec] = "1"

do_compile:class-native () {
    # This is still needed to have the native fiptool executing properly by
    # setting the RPATH
    sed -e '/^LDOPTS/ s,$, \$\{BUILD_LDFLAGS},' \
        -e '/^INCLUDE_PATHS/ s,$, \$\{BUILD_CFLAGS},' \
        -i ${S}/tools/fiptool/Makefile
    # This is still needed to have the native cert_create executing properly by
    # setting the RPATH
    sed -e '/^LIB_DIR/ s,$, \$\{BUILD_LDFLAGS},' \
        -e '/^INC_DIR/ s,$, \$\{BUILD_CFLAGS},' \
        -i ${S}/tools/cert_create/Makefile
    # This is still needed to have the native fiptool executing properly by
    # setting the RPATH
    sed -e '/^LIB_DIR/ s,$, \$\{BUILD_LDFLAGS},' \
        -e '/^INC_DIR/ s,$, \$\{BUILD_CFLAGS},' \
        -i ${S}/tools/encrypt_fw/Makefile

    oe_runmake PLAT=${TFA_PLATFORM} certtool enctool
    oe_runmake PLAT_FIPTOOL_HELPER_MK=plat_fiptool/st/stm32mp2/plat_fiptool.mk PLAT_DIR=../../plat/st/stm32mp2 fiptool
}

do_compile:class-nativesdk () {
    oe_runmake PLAT=${TFA_PLATFORM} certtool enctool
    oe_runmake PLAT_FIPTOOL_HELPER_MK=plat_fiptool/st/stm32mp2/plat_fiptool.mk PLAT_DIR=../../plat/st/stm32mp2 fiptool
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 \
        ${B}/tools/fiptool/fiptool \
        ${B}/tools/cert_create/cert_create \
        ${B}/tools/encrypt_fw/encrypt_fw \
        ${D}${bindir}

    install -m 0755 ${UNPACKDIR}/create_st_fip_binary.sh ${D}${bindir}
}

BBCLASSEXTEND += "native nativesdk"
