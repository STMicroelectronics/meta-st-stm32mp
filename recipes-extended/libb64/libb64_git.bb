SUMMARY = "Library for base64 encoding/decoding data"
DESCRIPTION = "library of ANSI C routines for fast encoding/decoding data into and \
from a base64-encoded format"
HOMEPAGE = "https://github.com/libb64/libb64"
LICENSE = "PD"
LIC_FILES_CHKSUM = "file://LICENSE;md5=ce551aad762074c7ab618a0e07a8dca3"

SRC_URI = "git://github.com/libb64/libb64.git;protocol=https"
SRCREV="04a1ee7590ebb8a81bbd3854ec141c2d4c2bead9"

PV = "1.4.1+git${SRCPV}"

S = "${WORKDIR}/git"

EXTRA_OEMAKE = "all_src all_base64"

do_install() {
	mkdir -p ${D}${bindir}
	install -m 755 ${S}/base64/base64 ${D}${bindir}/
	mkdir -p ${D}${includedir}/b64
	install -m 644 ${S}/include/b64/*.h ${D}${includedir}/b64
	mkdir -p ${D}${libdir}
	install -m 644 ${S}/src/libb64.a ${D}${libdir}/
}
