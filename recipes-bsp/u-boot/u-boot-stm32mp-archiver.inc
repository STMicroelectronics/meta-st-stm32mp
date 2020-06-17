#
# Archiver Configuration
#
SRC_URI_append = " file://README.HOW_TO.txt "

inherit archiver
ARCHIVER_MODE[src] = "original"

inherit archiver_stm32mp_clean

archiver_create_makefile_for_sdk() {
    # Init internal var for uboot_configs: should be 'defconfig,type,binary'
    if [ -n "${UBOOT_CONFIG}" ]; then
        unset i j k
        for config in ${UBOOT_MACHINE}; do
            i=$(expr $i + 1);
            for type in ${UBOOT_CONFIG}; do
                j=$(expr $j + 1);
                if [ $j -eq $i ]; then
                    for binary in ${UBOOT_BINARIES}; do
                        k=$(expr $k + 1);
                        if [ $k -eq $i ]; then
                            uboot_configs="${uboot_configs} ${config},${type},${binary}"
                        fi
                    done
                    unset k
                fi
            done
            unset j
        done
        unset i
    else
        uboot_configs="${UBOOT_MACHINE},,${UBOOT_BINARY}"
    fi

    mkdir -p ${ARCHIVER_OUTDIR}

    # Remove default variable
    echo "LDFLAGS=" > ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "CFLAGS=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "CPPFLAGS=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "LOCAL_PATH=\$(PWD)" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "UBOOT_LOCALVERSION=${UBOOT_LOCALVERSION}" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    # Configure default U-Boot configs
    echo "UBOOT_CONFIGS ?= ${uboot_configs}" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "DEVICE_TREE ?= ${UBOOT_DEVICETREE}" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "help:" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"Configured U-Boot config(s):\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@for config in \$(UBOOT_CONFIGS); do \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		defconfig=\$\$(echo \$\$config | cut -d',' -f1) ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		type=\$\$(echo \$\$config | cut -d',' -f2) ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		binary=\$\$(echo \$\$config | cut -d',' -f3) ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		echo \"  \$\$defconfig config (\$\$type type) for \$\$binary binary\" ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		for devicetree in \$(DEVICE_TREE); do \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			echo \"    with device tree: \$\$devicetree\" ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		done ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	done" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"Available targets:\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"  all   : build U-Boot binaries for defined config(s)\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"  clean : clean build directories from generated files\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "version:" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@if test ! -e .scmversion ; then echo \$(UBOOT_LOCALVERSION) > \$(LOCAL_PATH)/.scmversion; fi" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "all: version" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	for config in \$(UBOOT_CONFIGS); do \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		uboot_config=\$\$(echo \$\$config | cut -d',' -f1) ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		uboot_type=-\$\$(echo \$\$config | cut -d',' -f2) ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		uboot_binary=\$\$(echo \$\$config | cut -d',' -f3) ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		uboot_suffix=\$\$(echo \$\$uboot_binary | cut -d'.' -f2) ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Make sure about configuration set
    echo "		if test -z \"\$\$uboot_config\" -o -z \"\$\$uboot_type\" -o -z \"\$\$uboot_binary\"; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			echo ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			echo \"[ERROR] UBOOT_CONFIGS wrongly configured. It should be space separated list of element <defconfig>,<type>,<binary>\" ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			echo ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			exit 1 ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Init folder and defconfig selected
    echo "		if [ ! -d \$(LOCAL_PATH)/../build\$\$uboot_type ]; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			mkdir -p \$(LOCAL_PATH)/../build\$\$uboot_type ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			echo \$(UBOOT_LOCALVERSION) > \$(LOCAL_PATH)/../build\$\$uboot_type/.scmversion ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			\$(MAKE) -C \$(LOCAL_PATH) O=\$(LOCAL_PATH)/../build\$\$uboot_type \$\$uboot_config ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Build binaries
    echo "		if [ -z \"\$(DEVICE_TREE)\" ]; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			\$(MAKE) -C \$(LOCAL_PATH) O=\$(LOCAL_PATH)/../build\$\$uboot_type ${UBOOT_MAKE_TARGET} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Copy binary files with explicit name
    echo "			cp -f \$(LOCAL_PATH)/../build\$\$uboot_type/\$\$uboot_binary \$(LOCAL_PATH)/../build\$\$uboot_type/u-boot\$\$uboot_type.\$\$uboot_suffix ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			cp -f \$(LOCAL_PATH)/../build\$\$uboot_type/${UBOOT_ELF} \$(LOCAL_PATH)/../build\$\$uboot_type/u-boot\$\$uboot_type.${UBOOT_ELF_SUFFIX} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			if [ -f \$(LOCAL_PATH)/../build\$\$uboot_type/${SPL_BINARY_STM32} ]; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				cp -f \$(LOCAL_PATH)/../build\$\$uboot_type/${SPL_BINARY_STM32} \$(LOCAL_PATH)/../build\$\$uboot_type/${SPL_BINARYNAME}\$\$uboot_type ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				cp -f \$(LOCAL_PATH)/../build\$\$uboot_type/${SPL_ELF} \$(LOCAL_PATH)/../build\$\$uboot_type/${SPL_ELF_NAME}\$\$uboot_type ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		else \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			for devicetree in \$(DEVICE_TREE); do \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				\$(MAKE) -C \$(LOCAL_PATH) O=\$(LOCAL_PATH)/../build\$\$uboot_type ${UBOOT_MAKE_TARGET} DEVICE_TREE=\$\$devicetree DEVICE_TREE_EXT=\$\$devicetree.dtb; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Copy binary files with explicit name
    echo "				cp -f \$(LOCAL_PATH)/../build\$\$uboot_type/\$\$uboot_binary \$(LOCAL_PATH)/../build\$\$uboot_type/u-boot-\$\$devicetree\$\$uboot_type.\$\$uboot_suffix ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				cp -f \$(LOCAL_PATH)/../build\$\$uboot_type/${UBOOT_ELF} \$(LOCAL_PATH)/../build\$\$uboot_type/u-boot-\$\$devicetree\$\$uboot_type.${UBOOT_ELF_SUFFIX} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				if [ -f \$(LOCAL_PATH)/../build\$\$uboot_type/${SPL_BINARY_STM32} ]; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "					cp -f \$(LOCAL_PATH)/../build\$\$uboot_type/${SPL_BINARY_STM32} \$(LOCAL_PATH)/../build\$\$uboot_type/${SPL_BINARYNAME}-\$\$devicetree\$\$uboot_type ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "					cp -f \$(LOCAL_PATH)/../build\$\$uboot_type/${SPL_ELF} \$(LOCAL_PATH)/../build\$\$uboot_type/${SPL_ELF_NAME}-\$\$devicetree\$\$uboot_type ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			done ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	done" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "clean:" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@for config in \$(UBOOT_CONFIGS); do \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		uboot_type=-\$\$(echo \$\$config | cut -d',' -f2) ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		echo \"Removing \$(LOCAL_PATH)/../build\$\$uboot_type ...\" ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		rm -rf \$(LOCAL_PATH)/../build\$\$uboot_type ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	done" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
}
do_ar_original[prefuncs] += "archiver_create_makefile_for_sdk"
