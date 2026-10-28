SUMMARY = "Service to set cprufreq boost"
HOMEPAGE = "www.st.com"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9"

SRC_URI = " \
    file://st-cpufreq-boost.service \
    file://st-cpufreq-boost.sh \
"
S = "${UNPACKDIR}"

inherit systemd update-rc.d

SYSTEMD_PACKAGES = "${@bb.utils.contains('DISTRO_FEATURES','systemd','${PN}','',d)}"
SYSTEMD_SERVICE:${PN} = "st-cpufreq-boost.service"

INITSCRIPT_NAME = "st-cpufreq-boost.sh"
INITSCRIPT_PARAMS = "start 22 5 3 ."

do_configure[noexec] = "1"

do_install() {
    if ${@bb.utils.contains('DISTRO_FEATURES','systemd','true','false',d)}; then
        install -d ${D}${systemd_unitdir}/system
        install -m 0644 ${S}/st-cpufreq-boost.service ${D}${systemd_unitdir}/system/
    else
        install -d ${D}${sysconfdir}/init.d
        install -m 0755 ${UNPACKDIR}/st-cpufreq-boost.sh ${D}${sysconfdir}/init.d/st-cpufreq-boost.sh
    fi
}

FILES:${PN} += "${systemd_unitdir}/system"

