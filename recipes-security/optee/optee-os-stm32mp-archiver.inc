#
# Archiver Configuration
#
SRC_URI_append = " file://README.HOW_TO.txt "

inherit archiver
ARCHIVER_MODE[src] = "original"
COPYLEFT_LICENSE_INCLUDE_append = " BSD-3* "

inherit archiver_stm32mp_clean

archiver_create_makefile_for_sdk() {
    mkdir -p ${ARCHIVER_OUTDIR}

    #remove default variable
    echo "LDFLAGS=" > ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "CFLAGS=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "CPPFLAGS=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "LOCAL_PATH=\$(PWD)" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    #set default CONFIGURATION with configured OPTEE_CONF
    if [ -n "${OPTEE_CONF}" ]; then
        echo "PLATFORM ?= ${MACHINE}" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
        echo "CFG_EMBED_DTB_SOURCE_FILE ?= ${OPTEE_CONF}" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    fi

    echo -n "EXTRA_OEMAKE=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "${EXTRA_OEMAKE}" | sed "s|LIBGCC_LOCATE_CFLAGS=[^ ]* |LIBGCC_LOCATE_CFLAGS=\$(OECORE_NATIVE_SYSROOTK) |;s|comp-cflagscore='[^']*'|comp-cflagscore='\$(KCFLAGS)'|" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "all:" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	if test -n \"\$(CFG_EMBED_DTB_SOURCE_FILE)\" ; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		for dt in \$(CFG_EMBED_DTB_SOURCE_FILE) ; do \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			\$(MAKE) \$(EXTRA_OEMAKE) -C \$(LOCAL_PATH) PREFIX=\$(SDKTARGETSYSROOT) O=\$(LOCAL_PATH)/../build/\$\$dt CFG_EMBED_DTB_SOURCE_FILE=\$\$dt.dts ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Copy binary files with explicit name
    echo "			cp ../build/\$\$dt/core/${OPTEE_HEADER}.${OPTEE_SUFFIX} ../build/${OPTEE_HEADER}-\$\$dt-${OPTEE_BOOTCHAIN}.${OPTEE_SUFFIX} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			cp ../build/\$\$dt/core/${OPTEE_PAGER}.${OPTEE_SUFFIX} ../build/${OPTEE_PAGER}-\$\$dt-${OPTEE_BOOTCHAIN}.${OPTEE_SUFFIX} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			cp ../build/\$\$dt/core/${OPTEE_PAGEABLE}.${OPTEE_SUFFIX} ../build/${OPTEE_PAGEABLE}-\$\$dt-${OPTEE_BOOTCHAIN}.${OPTEE_SUFFIX} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		done ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	else \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		\$(MAKE) \$(EXTRA_OEMAKE) -C \$(LOCAL_PATH) PREFIX=\$(SDKTARGETSYSROOT) O=\$(LOCAL_PATH)/../build/ ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Copy binary files with explicit name
    echo "		cp ../build/core/${OPTEE_HEADER}.${OPTEE_SUFFIX} ../build/${OPTEE_HEADER}-${OPTEE_BOOTCHAIN}.${OPTEE_SUFFIX} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		cp ../build/core/${OPTEE_PAGER}.${OPTEE_SUFFIX} ../build/${OPTEE_PAGER}-${OPTEE_BOOTCHAIN}.${OPTEE_SUFFIX} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		cp ../build/core/${OPTEE_PAGEABLE}.${OPTEE_SUFFIX} ../build/${OPTEE_PAGEABLE}-${OPTEE_BOOTCHAIN}.${OPTEE_SUFFIX} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	fi" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
}
do_ar_original[prefuncs] += "archiver_create_makefile_for_sdk"
