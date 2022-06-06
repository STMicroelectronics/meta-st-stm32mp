SRC_URI:append = " file://README.HOW_TO.txt "

inherit archiver
ARCHIVER_MODE[src] = "original"

# Override generated tarball for original with gcnano-driver-stm32mp source tarball
do_set_archiver_tarball() {
    rm -f ${ARCHIVER_OUTDIR}/${PF}*.tar.*
    cp ${WORKDIR}/git/${GCNANO_DRIVER_TARBALL} ${ARCHIVER_OUTDIR}/${PF}.tar.xz
}
do_ar_original[postfuncs] =+ "do_set_archiver_tarball"

inherit archiver_stm32mp_clean
