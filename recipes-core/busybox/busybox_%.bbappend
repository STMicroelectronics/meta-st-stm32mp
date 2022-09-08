FILESEXTRAPATHS:prepend:stm32mpcommon := "${THISDIR}/${PN}:"

SRC_URI:append:stm32mpcommon = " \
       file://busybox-stm32mp.cfg \
       file://0001-miscutils-watchdog-Add-gettimeleft.patch \
       file://ifplugd.conf \
       file://ifplugd.action \
       file://ifplugd.sh \
       "

#inherit update-rc.d
DEPENDS:append:stm32mpcommon = " ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', '', 'update-rc.d-native', d)}"

do_install:append:stm32mpcommon () {
    if [ "${@bb.utils.contains('DISTRO_FEATURES', 'systemd', '1', '0', d)}" = "0" ]; then
        if grep -q "CONFIG_IFPLUGD=y" ${B}/.config; then
            install -d ${D}${sysconfdir}/ifplugd
            install -m 755 ${WORKDIR}/ifplugd.sh ${D}${sysconfdir}/init.d/ifplugd.sh
            update-rc.d -r ${D} ifplugd.sh start 99 2 3 4 5 .
            install -m 755 ${WORKDIR}/ifplugd.conf ${D}${sysconfdir}/ifplugd/ifplugd.conf
            install -m 755 ${WORKDIR}/ifplugd.action ${D}${sysconfdir}/ifplugd/ifplugd.action
        fi
    fi
}
