do_install_append_stm32mpcommon() {
    # enable watchdog on systemd configuration
    if ${@bb.utils.contains('MACHINE_FEATURES','watchdog','true','false',d)}; then
        install -d ${D}${systemd_unitdir}/system.conf.d/
        echo "[Manager]" > ${D}${systemd_unitdir}/system.conf.d/01-watchdog.conf
        echo "RuntimeWatchdogSec=32" >> ${D}${systemd_unitdir}/system.conf.d/01-watchdog.conf
        echo "ShutdownWatchdogSec=2min" >> ${D}${systemd_unitdir}/system.conf.d/01-watchdog.conf
    fi
}
