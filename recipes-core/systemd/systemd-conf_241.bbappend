
do_install_append_stm32mpcommon() {
    # enable watchdog on systemd configuration
    if ${@bb.utils.contains('MACHINE_FEATURES','watchdog','true','false',d)}; then
        sed -e 's|^[#]*RuntimeWatchdogSec.*|RuntimeWatchdogSec=30|g' -i ${D}${sysconfdir}/systemd/system.conf
        sed -e 's|^[#]*ShutdownWatchdogSec.*|ShutdownWatchdogSec=5min|g' -i ${D}${sysconfdir}/systemd/system.conf
    fi
}
