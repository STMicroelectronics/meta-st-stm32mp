SUMMARY = "U-Boot bootloader fwumdata configuration"
DESCRIPTION = "Generate and install the fwumdata configuration file"
SECTION = "bootloaders"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

DEPENDS += "u-boot-tools-stm32mp"

SRC_URI = "\
    file://fwumdata.config.in \
    file://fwumdata.nor.config.in \
"

S = "${UNPACKDIR}"

# MDATA_SIZE correspond to size of metadata file:
# without firmware update A/B: metadata.bin = 120 b = 0x78
# with firmware update A/B:    metadata-ab-sdcard.bin = 280 b = 0x118
MDATA_SIZE = "${@bb.utils.contains('MACHINE_FEATURES','fw-update-ab','0x118','0x78',d)}"

do_install () {
    install -d ${D}${sysconfdir}

    sed "s|@MDATA_SIZE@|${MDATA_SIZE}|g" ${S}/fwumdata.config.in \
        > ${D}${sysconfdir}/fwumdata.config
    sed "s|@MDATA_SIZE@|${MDATA_SIZE}|g" ${S}/fwumdata.nor.config.in \
        > ${D}${sysconfdir}/fwumdata.nor.config
}

FILES:${PN} += "${sysconfdir}/"
RDEPENDS:${PN} += "u-boot-tools-stm32mp"
