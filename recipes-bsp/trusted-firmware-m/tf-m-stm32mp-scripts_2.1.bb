require tf-m-stm32mp-common_${PV}.inc

SUMMARY = "Trusted Firmware scripts for Cortex-M"
LICENSE = "BSD-3-Clause & Apache-2.0"

PACKAGE_ARCH = "${MACHINE_ARCH}"

SRC_URI:append = " file://create_st_m33fw_binary.sh;subdir=scripts"
SRC_URI:append = " file://st_m33td_firmware_signature.sh;subdir=scripts"

BBCLASSEXTEND = "native nativesdk"

DEPENDS:class-native = ""

S = "${UNPACKDIR}"

do_compile[noexec] = "1"

do_install() {
    # Install assemble and signing firmware script
    install -d ${D}${bindir}
    install -m 755 ${UNPACKDIR}/scripts/create_st_m33fw_binary.sh ${D}${bindir}
    install -m 755 ${UNPACKDIR}/scripts/st_m33td_firmware_signature.sh ${D}${bindir}
    # Update version
    sed 's/^SIGN_VERSION=.*$/SIGN_VERSION='"${TF_M_VERSION}"'/' -i ${D}${bindir}/st_m33td_firmware_signature.sh
    # Install default MCUBOOT keys
    install -d ${D}${datadir}/tf-m/keys
    install -m 0644 ${S}/bl2/ext/mcuboot/root-EC-P256.pem ${D}${datadir}/tf-m/keys/root-ec-p256.pem
    install -m 0644 ${S}/bl2/ext/mcuboot/root-EC-P256_1.pem ${D}${datadir}/tf-m/keys/root-ec-p256_1.pem
    # Install all python scripts needed for assemble and sign
    install -d ${D}${datadir}/tf-m/scripts
    install -m 0755 ${S}/bl2/ext/mcuboot/scripts/assemble.py ${D}${datadir}/tf-m/scripts
    install -m 0755 ${S}/bl2/ext/mcuboot/scripts/macro_parser.py ${D}${datadir}/tf-m/scripts
    install -d ${D}${datadir}/tf-m/scripts/wrapper
    install -m 0755 ${S}/bl2/ext/mcuboot/scripts/wrapper/wrapper.py ${D}${datadir}/tf-m/scripts/wrapper
    # Install imgtool suite
    install -m 0755 ${UNPACKDIR}/${TF_M_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_MCUBOOT}/scripts/imgtool.py ${D}${datadir}/tf-m/scripts/wrapper
    install -d ${D}${datadir}/tf-m/scripts/wrapper/imgtool/keys
    for f in $(find ${UNPACKDIR}/${TF_M_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_MCUBOOT}/scripts/imgtool/ -maxdepth 1 -type f ); do
        install -m 0755 $f ${D}${datadir}/tf-m/scripts/wrapper/imgtool/
    done
    for f in $(find ${UNPACKDIR}/${TF_M_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_MCUBOOT}/scripts/imgtool/keys -maxdepth 1 -type f ); do
        install -m 0644 $f ${D}${datadir}/tf-m/scripts/wrapper/imgtool/keys
    done
}

FILES:${PN} = "${datadir}/tf-m"
RDEPENDS:${PN} += "bash"
RDEPENDS:${PN} += "openssl"
RDEPENDS:${PN} += "python3-cbor2"
RDEPENDS:${PN} += "python3-click"
RDEPENDS:${PN} += "python3-cryptography"
RDEPENDS:${PN} += "python3-intelhex"
RDEPENDS:${PN} += "python3-pyasn1"
RDEPENDS:${PN} += "python3-pyyaml"

FILES:${PN}:append:class-nativesdk = " ${bindir} ${datadir}"
