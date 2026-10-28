# Copyright (C) 2025 Christophe Priouzeau <christophe.priouzeau@foss.st.com>
# Released under the MIT license (see COPYING.MIT for the terms)

SUMMARY = "ST RPMSG tools to create rpmsg entry"
HOMEPAGE = "www.st.com"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9"

SRC_URI = " \
    file://Makefile \
    file://rpmsg_create_ept.c \
    \
    file://72-rpmsg.rules \
"

S = "${UNPACKDIR}"

do_configure[noexec] = "1"

do_compile() {
    bbnote "EXTRA_OEMAKE=${EXTRA_OEMAKE}"
    oe_runmake clean
    oe_runmake all
}

do_install() {
    install -d ${D}${bindir}
    # add GUID for rpmsg_create_ept
    install -m 2755 ${S}/rpmsg_create_ept ${D}${bindir}

    # instal udev rules
    install -d ${D}${sysconfdir}/udev/rules.d
    install -o root -g rpmsg -m 0644 ${S}/72-rpmsg.rules ${D}${sysconfdir}/udev/rules.d/
}

FILES:${PN} += "${sysconfdir} ${bindir}"

inherit useradd
USERADD_PACKAGES = "${PN}"
GROUPADD_PARAM:${PN} = "--system rpmsg"
