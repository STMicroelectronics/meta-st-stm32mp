#@DESCRIPTION: Machine features definitions for STM32 systems

include conf/machine/include/gpu_vivante.inc

# =========================================================================
# gpu
# =========================================================================
# To support gpu on a machine,
# please add "gpu" to MACHINE_FEATURES in machine conf file
GPU_LIST ?= "\
    gcnano-driver-stm32mp \
    "
GPU_IMAGE_INSTALL = "${@bb.utils.contains('MACHINE_FEATURES', 'gpu', '${GPU_LIST}', '', d)} "

# =========================================================================
# alsa
# =========================================================================
ALSA_LIST ?= "\
    alsa-state-stm32mp1 \
    "
ALSA_IMAGE_INSTALL = "${@bb.utils.contains('COMBINED_FEATURES', 'alsa', '${ALSA_LIST}', '', d)} "

# =========================================================================
# bluetooth
# =========================================================================
BLUETOOTH_LIST ?= ""
BLUETOOTH_IMAGE_INSTALL = "${@bb.utils.contains('MACHINE_FEATURES', 'bluetooth', '${BLUETOOTH_LIST}', '', d)}"

# =========================================================================
# m4copro
# =========================================================================
M4COPRO_LIST ?= "\
    m4fwcoredump \
    m4projects-stm32mp1 \
    "
M4COPRO_IMAGE_INSTALL = "${@bb.utils.contains('MACHINE_FEATURES', 'm4copro', '${M4COPRO_LIST}', '', d)}"

# =========================================================================
# optee
# =========================================================================
OPTEE_LIST ?= "\
    virtual/optee-os \
    "
OPTEE_IMAGE_INSTALL = "${@bb.utils.contains('COMBINED_FEATURES', 'optee', '${OPTEE_LIST}', '', d)} "

OPTEE_BINARY ?= "\
    virtual/optee-os \
    "
OPTEE_BINARY_INSTALL = "${@bb.utils.contains('MACHINE_FEATURES', 'optee', '${OPTEE_BINARY}', '', d)} "

# =========================================================================
# wifi
# =========================================================================
WIFI_LIST ?= ""
WIFI_IMAGE_INSTALL = "${@bb.utils.contains('MACHINE_FEATURES', 'wifi', '${WIFI_LIST}', '', d)} "

# =========================================================================
# Image appends
# =========================================================================
EXTRA_IMAGEDEPENDS_append = " \
    ${OPTEE_BINARY_INSTALL} \
    "

MACHINE_EXTRA_RRECOMMENDS_append = " \
    ${GPU_IMAGE_INSTALL}        \
    ${ALSA_IMAGE_INSTALL}       \
    ${BLUETOOTH_IMAGE_INSTALL}  \
    ${M4COPRO_IMAGE_INSTALL}    \
    ${OPTEE_IMAGE_INSTALL}      \
    ${WIFI_IMAGE_INSTALL}       \
    "
