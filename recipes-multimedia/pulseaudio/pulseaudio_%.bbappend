FILESEXTRAPATHS:prepend:stm32mpcommon := "${THISDIR}/${PN}:"

# Pulse audio configuration files
SRC_URI:append:stm32mpcommon = " \
        file://default.pa \
        file://system.pa \
        "

# Pulse audio configuration files installation
do_install:append:stm32mpcommon() {
    if [ -e "${WORKDIR}/default.pa" ]; then
        install -m 0644 ${WORKDIR}/default.pa ${D}/${sysconfdir}/pulse/default.pa
    fi

    if [ -e "${WORKDIR}/system.pa" ]; then
        install -m 0644 ${WORKDIR}/system.pa ${D}/${sysconfdir}/pulse/system.pa
    fi

    if [ -f ${D}${datadir}/pulseaudio/alsa-mixer/paths/analog-output-speaker.conf ];
    then
        sed -i "s|^priority = .*$|priority = 88|g" ${D}${datadir}/pulseaudio/alsa-mixer/paths/analog-output-speaker.conf
    fi
}
