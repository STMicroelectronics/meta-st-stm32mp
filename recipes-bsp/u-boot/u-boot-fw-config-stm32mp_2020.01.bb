SUMMARY = "U-Boot bootloader fw_printenv/setenv utilities"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
    file://fw_env.config.mmc \
    file://fw_env.config.nand \
    file://fw_env.config.nor \
"
DEPENDS += "u-boot-fw-utils"

# Specific function to manage any trial for source code extraction through
# devtool which can not be supported as we're sharing original source from
# virtual/bootloader provider via STAGING_UBOOT_DIR shared folder
python () {
    if bb.data.inherits_class('devtool-source', d):
        bb.fatal('The %s recipe does not actually check out own source and thus cannot be supported by devtool.' % d.getVar("BPN"))
}

do_install () {
	install -d ${D}${sysconfdir}
	install -m 0644 ${WORKDIR}/fw_env.config.* ${D}${sysconfdir}/
}


FILES_${PN} += "${sysconfdir}/"
RDEPENDS_${PN} += "u-boot-fw-utils"
