SUMMARY = "CoPro development kit built"

LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9"

SRC_URI = " file://st_copro_firmware_signature.sh"

BBCLASSEXTEND = " native nativesdk"

do_configure[noexec] = "1"
do_compile[noexec] = "1"

do_install() {
    mkdir -p ${D}${bindir}
    install -m 755 ${WORKDIR}/st_copro_firmware_signature.sh ${D}${bindir}
}
RDEPENDS:${PN} += "bash"
