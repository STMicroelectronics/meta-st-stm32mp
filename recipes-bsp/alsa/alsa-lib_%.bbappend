FILESEXTRAPATHS:prepend:stm32mpcommon := "${THISDIR}/${PN}:"

PACKAGE_ARCH:stm32mpcommon = "${MACHINE_ARCH}"

SRC_URI:append:stm32mpcommon = " \
        file://0001-conf-add-card-configs-for-stm32mp15x-boards.patch \
        file://0002-conf-add-card-config-for-stm32mp13x_evd-board.patch \
        file://0003-conf-add-card-config-for-stm32mp25-ev1-eval-board.patch \
        file://0004-conf-add-card-config-for-stm32mp25-dk-board.patch \
        "
