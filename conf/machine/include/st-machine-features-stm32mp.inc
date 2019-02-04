#@DESCRIPTION: Machine features definitions for STM32 systems

include conf/machine/include/gpu_vivante.inc

#------------------
#
# GPU
#
# To support gpu on a machine,
# please add "gpu" to MACHINE_FEATURES in machine conf file

GPU_LIST          = "kernel-module-galcore"
GPU_IMAGE_INSTALL = "${@bb.utils.contains('MACHINE_FEATURES', 'gpu', '${GPU_LIST}', '', d)} "

#------------------
#
# optee
#

OPTEE_LIST = "optee-os-stm32mp"
OPTEE_IMAGE_INSTALL = "${@bb.utils.contains('COMBINED_FEATURES', 'optee', '${OPTEE_LIST}', '', d)} "
PREFERRED_PROVIDER_optee-os = "optee-os-stm32mp"

OPTEE_BINARY = "optee-os-stm32mp"
OPTEE_BINARY_INSTALL = "${@bb.utils.contains('MACHINE_FEATURES', 'optee', '${OPTEE_BINARY}', '', d)} "

#------------------
#
# Alsa
#
ALSA_ADDONS = "alsa-state-stm32mp1"
ALSA_ADDONS_INSTALL = "${@bb.utils.contains('COMBINED_FEATURES', 'alsa', '${ALSA_ADDONS}', '', d)} "

#------------------
#
# bluetooth
#
BLUETOOTH_LIST          = " linux-firmware-bluetooth-bcm4343 "
BLUETOOTH_IMAGE_INSTALL = "${@bb.utils.contains('MACHINE_FEATURES', 'bluetooth', '${BLUETOOTH_LIST}', '', d)} "

#------------------
#
# Image appends
#
EXTRA_IMAGEDEPENDS_append = " \
    ${OPTEE_BINARY_INSTALL} \
    "

MACHINE_EXTRA_RRECOMMENDS_append = " \
    ${GPU_IMAGE_INSTALL}      \
    ${OPTEE_IMAGE_INSTALL}    \
    ${ALSA_ADDONS_INSTALL}    \
    ${BLUETOOTH_IMAGE_INSTALL}\
    "
