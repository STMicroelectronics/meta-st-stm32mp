SUMMARY = "TA development kit built from optee_os, needed to build OP-TEE TAs"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI:append:class-nativesdk = " file://environment.d-optee-sdk.sh"

BBCLASSEXTEND = " nativesdk"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install:append:class-nativesdk () {
    mkdir -p ${D}${SDKPATHNATIVE}/environment-setup.d
    install -m 755 ${UNPACKDIR}/environment.d-optee-sdk.sh ${D}${SDKPATHNATIVE}/environment-setup.d/optee-sdk.sh
}

FILES:${PN}:append:class-nativesdk = " ${SDKPATHNATIVE}/environment-setup.d/optee-sdk.sh"
