#
# Archiver Configuration
#
SRC_URI:append = " file://README.HOW_TO.txt "

inherit archiver
ARCHIVER_MODE[src] = "original"

inherit archiver_stm32mp_clean

do_archiver_git_uri:append() {
    if [ -e "${ARCHIVER_OUTDIR}/${ARCHIVER_README}.${MACHINE}" ]; then
        sed -i \
            -e "s|##LOADADDR##|${ST_KERNEL_LOADADDR}|g" \
            "${ARCHIVER_OUTDIR}/${ARCHIVER_README}.${MACHINE}"
    fi
}

