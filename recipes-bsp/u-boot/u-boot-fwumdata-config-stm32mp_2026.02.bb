SUMMARY = "U-Boot bootloader fwumdata configuration"
DESCRIPTION = "Generate and install the fwumdata configuration file"
SECTION = "bootloaders"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS += "u-boot-tools-stm32mp"

SRC_URI = "\
    file://fwumdata.config.in \
"

S = "${WORKDIR}"

MDATA_SIZE = "${@bb.utils.contains('MACHINE_FEATURES','fw-update-ab','0x118','0x78',d)}"

do_install () {
    install -d ${D}${sysconfdir}

    sed "s|@MDATA_SIZE@|${MDATA_SIZE}|g" ${S}/fwumdata.config.in \
        > ${D}${sysconfdir}/fwumdata.config
}

FILES:${PN} += "${sysconfdir}/"
RDEPENDS:${PN} += "u-boot-tools-stm32mp"
