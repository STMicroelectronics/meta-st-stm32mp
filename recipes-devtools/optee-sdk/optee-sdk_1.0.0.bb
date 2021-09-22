SUMMARY = "TA development kit built from optee_os, needed to build OP-TEE TAs"

LICENSE = "MIT"

SRC_URI_append_class-nativesdk = " file://environment.d-optee-sdk.sh"

BBCLASSEXTEND = " nativesdk"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install_append_class-nativesdk () {
    mkdir -p ${D}${SDKPATHNATIVE}/environment-setup.d
    install -m 644 ${WORKDIR}/environment.d-optee-sdk.sh ${D}${SDKPATHNATIVE}/environment-setup.d/optee-sdk.sh
}

FILES_${PN}_append_class-nativesdk = " ${SDKPATHNATIVE}/environment-setup.d/optee-sdk.sh"
