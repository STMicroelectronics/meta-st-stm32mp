FILESEXTRAPATHS_prepend_stm32mpcommon := "${THISDIR}/${PN}:"
SRC_URI_append_stm32mpcommon = " \
        file://0001-conf-add-card-configs-for-stm32mp15x-boards.patch \
        "
