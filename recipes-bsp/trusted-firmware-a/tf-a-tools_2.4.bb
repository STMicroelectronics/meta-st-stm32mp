SUMMARY = "Cert_create & Fiptool for fip generation for Trusted Firmware-A"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://license.rst;md5=1dd070c98a281d18d9eefd938729b031"

SRC_URI = "git://github.com/ARM-software/arm-trusted-firmware.git;protocol=https;nobranch=1"
#SRCREV corresponds to v2.4
SRCREV = "e2c509a39c6cc4dda8734e6509cdbe6e3603cdfc"

# Mandatory fix to allow feeding password through command line
SRC_URI += "file://0099-tools-allow-to-use-a-root-key-password-from-command-.patch"

DEPENDS_class-nativesdk = "nativesdk-openssl"

S = "${WORKDIR}/git"

do_compile() {
    oe_runmake certtool fiptool
}

do_install() {
    install -d ${D}${bindir}
    # cert_create
    install -m 0755 ${B}/tools/cert_create/cert_create ${D}${bindir}/cert_create
    # fiptool
    install -m 0755 ${B}/tools/fiptool/fiptool ${D}${bindir}/fiptool
}

FILES_${PN}_class-nativesdk = "${bindir}/cert_create ${bindir}/fiptool"

RDEPENDS_${PN}_class-nativesdk += "nativesdk-libcrypto"

BBCLASSEXTEND += "native nativesdk"
