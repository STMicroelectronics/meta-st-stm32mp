FILESEXTRAPATHS_prepend_stm32mpcommon := "${THISDIR}/${PN}:"

SRC_URI_append_stm32mpcommon = " file://0001-do-not-use-GBM-modifiers.patch "
SRC_URI_append_stm32mpcommon = " file://0001-desktop-shell-always-paint-background-color-first.patch "
SRC_URI_append_stm32mpcommon = " file://0002-desktop-shell-allow-to-center-background-image.patch "
SRC_URI_append_stm32mpcommon = " file://0003-Allow-to-get-hdmi-output-with-several-outputs.patch "
SRC_URI_append_stm32mpcommon = " file://0004-Force-to-close-all-output.patch "
SRC_URI_append_stm32mpcommon = " file://0005-clients-close-unused-keymap-fd.patch "
SRC_URI_append_stm32mpcommon = " file://0006-backend-drm-fix-race-during-system-suspend.patch "
