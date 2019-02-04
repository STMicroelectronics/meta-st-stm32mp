FILESEXTRAPATHS_prepend_stm32mpcommon := "${THISDIR}/${PN}:"

SRC_URI_append_stm32mpcommon = " \
    file://0001-Enable-hardware-watchdog-inside-systemd.patch \
    "
