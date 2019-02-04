FILESEXTRAPATHS_prepend_stm32mpcommon := "${THISDIR}/${PN}:"

SRC_URI_append_stm32mpcommon = " \
       file://${BUSYBOX_CONFIG_FRAGMENT} \
       file://0001-miscutils-watchdog-Add-gettimeleft.patch \
       file://ifplugd.conf \
       file://ifplugd.action \
       file://ifplugd.sh \
       "

BUSYBOX_CONFIG_FRAGMENT_stm32mpcommon = "busybox-stm32mp.cfg"

#inherit update-rc.d
DEPENDS_append_stm32mpcommon = " ${@bb.utils.contains('DISTRO_FEATURES', 'systemd', '', 'update-rc.d-native', d)}"

do_configure_append_stm32mpcommon () {
    # merge specific configuration to newly generated .config
    merge_config.sh -m -r -O ${B} ${B}/.config ${WORKDIR}/${BUSYBOX_CONFIG_FRAGMENT} 1>&2
    # make sure to generate proper config file for busybox
    cml1_do_configure
}

do_install_append_stm32mpcommon () {
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
