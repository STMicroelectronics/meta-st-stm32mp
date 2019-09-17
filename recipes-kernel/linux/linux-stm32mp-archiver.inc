#
# Archiver Configuration
#
SRC_URI_append = " file://README.HOW_TO.txt "

inherit archiver
ARCHIVER_MODE[src] = "original"

inherit archiver_stm32mp_clean
