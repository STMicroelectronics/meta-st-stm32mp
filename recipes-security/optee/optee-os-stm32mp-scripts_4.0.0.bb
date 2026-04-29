require optee-os-stm32mp-common_${PV}.inc

SUMMARY = "OPTEE TA scripts for stm32mp"
LICENSE = "BSD-2-Clause & BSD-3-Clause"

SRC_URI:append = " file://st_copro_firmware_signature.sh;subdir=scripts"

BBCLASSEXTEND = "native nativesdk"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
    install -d ${D}${bindir}
    install -m 755 ${UNPACKDIR}/scripts/st_copro_firmware_signature.sh ${D}${bindir}

    # Install optee scripts
    install -d ${D}${datadir}/optee/scripts
    install -m 755 ${B}/scripts/sign_rproc_fw.py ${D}${datadir}/optee/scripts
    # Install default optee key
    install -d ${D}${datadir}/optee/keys
    install -m 644 ${B}/keys/default.pem ${D}${datadir}/optee/keys
}

# Manage script dependencies
RDEPENDS:${PN} += "bash"
# Needed for optee script
RDEPENDS:${PN} += "python3-pyelftools python3-pycryptodomex python3-cryptography"
RDEPENDS:${PN} += "openssl"
