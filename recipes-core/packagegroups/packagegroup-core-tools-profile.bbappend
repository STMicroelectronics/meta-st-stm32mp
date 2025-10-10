FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

PACKAGE_ARCH:stm32mpcommon = "${MACHINE_ARCH}"

# Toporaty disable LTTNG
LTTNGTOOLS:stm32mpcommon = ""
