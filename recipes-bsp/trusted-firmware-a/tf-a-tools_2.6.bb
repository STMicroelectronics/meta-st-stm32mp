SUMMARY = "Cert_create & Fiptool for fip generation for Trusted Firmware-A"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://license.rst;md5=1dd070c98a281d18d9eefd938729b031"

SRC_URI = "git://github.com/ARM-software/arm-trusted-firmware.git;protocol=https;nobranch=1"
#SRCREV corresponds to v2.6
SRCREV = "a1f02f4f3daae7e21ee58b4c93ec3e46b8f28d15"

# Mandatory fix to allow feeding password through command line
SRC_URI += "file://0099-tools-allow-to-use-a-root-key-password-from-command-.patch"

DEPENDS:class-nativesdk = "nativesdk-openssl"

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

FILES:${PN}:class-nativesdk = "${bindir}/cert_create ${bindir}/fiptool"

RDEPENDS:${PN}:class-nativesdk += "nativesdk-libcrypto"

BBCLASSEXTEND += "native nativesdk"
