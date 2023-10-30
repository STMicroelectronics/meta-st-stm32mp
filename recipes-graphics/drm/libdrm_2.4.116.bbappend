# We don't want etnaviv drm package
PACKAGECONFIG:stm32mpcommon = "install-test-programs tests"

FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}/:"
