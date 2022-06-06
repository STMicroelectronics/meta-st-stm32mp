FILESEXTRAPATHS:prepend:stm32mpcommon := "${THISDIR}/${PN}:"
SRC_URI:append:stm32mpcommon = " \
        file://0001-conf-add-card-configs-for-stm32mp15x-boards.patch \
        file://0002-conf-add-card-config-for-stm32mp13x_evd-board.patch \
        "
