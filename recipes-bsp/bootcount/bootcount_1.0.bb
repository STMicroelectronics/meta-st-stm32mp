SUMMARY = "Bootcount management and bank validation service"
DESCRIPTION = "Handles A/B bank switching by confirming successful boots. \
               It marks the current bank as 'Accepted' to stop the TF-A \
               bootcounter and prevent automatic rollback to the previous bank."
SECTION = "bootloaders"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit systemd

SRC_URI = "\
    file://bootcount.service \
    file://bootcount.sh \
"

S = "${WORKDIR}"

RDEPENDS:${PN} = "bash u-boot-tools-stm32mp-fwumdata"

SYSTEMD_SERVICE:${PN} = "bootcount.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

do_install() {
    install -d ${D}${sbindir}
    install -d ${D}${systemd_unitdir}/system

    install -m 0644 ${S}/bootcount.service ${D}${systemd_unitdir}/system
    install -m 0755 ${S}/bootcount.sh ${D}${sbindir}
}
