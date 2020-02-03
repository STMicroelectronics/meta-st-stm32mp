# We don't want etnaviv drm package
EXTRA_OECONF_remove_stm32mpcommon += "--enable-etnaviv-experimental-api"

FILESEXTRAPATHS_prepend := "${THISDIR}/${PN}/:"

SRC_URI_append_stm32mpcommon = " \
        file://0001-tests-modetest-automatic-configuration.patch \
        file://0002-tests-util-smtpe-increase-alpha-to-middle-band.patch \
        "
