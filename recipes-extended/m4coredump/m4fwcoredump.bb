DESCRIPTION = "Script to manage coredump of cortexM4"
HOMEPAGE = "www.st.com"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

DEPENDS = "udev"

SRC_URI = " \
    file://85-m4-dump.rules \
    file://stm32mp-m4fwdump.sh \
    \
    file://stm32mp-coredump-sysfs.sh \
    file://st-m4coredump.service \
    "

S = "${WORKDIR}"

inherit systemd

do_install() {
    install -D -p -m0644 ${WORKDIR}/85-m4-dump.rules \
        ${D}${sysconfdir}/udev/rules.d/85-m4-dump.rules

    install -d ${D}${sbindir}/
    install -m0755 ${WORKDIR}/stm32mp-m4fwdump.sh ${D}${sbindir}/

    sed -i -e "s:#BINDIR#:${sbindir}:g" \
              ${D}${sysconfdir}/udev/rules.d/85-m4-dump.rules

    if ${@bb.utils.contains('DISTRO_FEATURES','systemd','true','false',d)}; then
        install -d ${D}${systemd_unitdir}/system
        install -d ${D}${base_sbindir}
        install -m 644 ${WORKDIR}/st-m4coredump.service ${D}${systemd_unitdir}/system/
        install -m 755 ${WORKDIR}/stm32mp-coredump-sysfs.sh ${D}${base_sbindir}/
    fi
}
FILES_${PN} += "${systemd_unitdir}/system"
