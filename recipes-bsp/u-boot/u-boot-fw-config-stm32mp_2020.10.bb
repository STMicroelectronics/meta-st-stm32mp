SUMMARY = "U-Boot bootloader fw_printenv/setenv utilities"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = " \
    file://fw_env.config.mmc \
    file://fw_env.config.nand \
    file://fw_env.config.nor \
"

DEPENDS += "u-boot-fw-utils"

do_install () {
    install -d ${D}${sysconfdir}
    install -m 0644 ${WORKDIR}/fw_env.config.* ${D}${sysconfdir}/
    if ${@bb.utils.contains('MACHINE_FEATURES','fip','true','false',d)}; then
        sed -i 's/ssbl/fip/g' ${D}${sysconfdir}/fw_env.config.mmc
        sed -i 's/0x280000/0x480000/g' ${D}${sysconfdir}/fw_env.config.nor
        sed -i 's/0x2C0000/0x4C0000/g' ${D}${sysconfdir}/fw_env.config.nor
    fi
}

FILES_${PN} += "${sysconfdir}/"
RDEPENDS_${PN} += "u-boot-fw-utils"
