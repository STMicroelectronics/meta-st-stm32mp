FILESEXTRAPATHS_prepend_stm32mpcommon := "${THISDIR}/${PN}:"

dirs755_append_stm32mpcommon = " ${STM32MP_USERFS_MOUNTPOINT_IMAGE}"
dirs755_append_stm32mpcommon = " ${STM32MP_VENDORFS_MOUNTPOINT_IMAGE}"
