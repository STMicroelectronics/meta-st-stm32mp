#
# Archiver Configuration
#
SRC_URI:append = " file://README.HOW_TO.txt "

TF_M_ARCHIVER ?= "${@bb.utils.contains('MACHINE_FEATURES', 'm33td', 'archiver', '', d)}"
inherit_defer ${TF_M_ARCHIVER}
ARCHIVER_MODE[src] = "original"
COPYLEFT_LICENSE_INCLUDE:append = " BSD-3* "

inherit archiver_stm32mp_clean
