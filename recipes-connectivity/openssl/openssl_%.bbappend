FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

do_install:append:class-nativesdk() {
    echo "export OPENSSL_MODULES=\"\$OECORE_NATIVE_SYSROOT/usr/lib/ossl-modules/\"" >> ${D}${SDKPATHNATIVE}/environment-setup.d/openssl.sh
}
