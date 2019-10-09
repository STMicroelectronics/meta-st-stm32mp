#
# Archiver Configuration
#
SRC_URI_append = " file://README.HOW_TO.txt "

inherit archiver
ARCHIVER_MODE[src] = "original"
COPYLEFT_LICENSE_INCLUDE_append = " BSD-3* "

inherit archiver_stm32mp_clean

archiver_create_makefile_for_sdk() {
    . ${T}/tfaconfig_env

    # Init internal var for tfa_config_oemake: should be 'config,extraoemake'
    for config in ${TF_A_CONFIG}; do
        tf_a_config_oemake="${tf_a_config_oemake} ${config},\"$(eval echo \$TF_A_CONFIG_${config})\""
    done

    mkdir -p ${ARCHIVER_OUTDIR}
    #remove default variable
    echo "LDFLAGS=" > ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "CFLAGS=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "CPPFLAGS=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "CC=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "CPP=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "AS=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "AR=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "LD=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "NM=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "LOCAL_PATH=\$(PWD)" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "EXTRA_OEMAKE=${EXTRA_OEMAKE}" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    # Set default config
    echo "ELF_DEBUG_ENABLE ?= ${ELF_DEBUG_ENABLE}" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "TF_A_CONFIG ?= ${TF_A_CONFIG}" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Set specific OEMAKE config
    echo "TF_A_CONFIG_OEMAKE = ${tf_a_config_oemake}" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    # Set default TFA_DEVICETREE with configured TF_A_DEVICETREE
    if [ -n "${TF_A_DEVICETREE}" ]; then
        echo "TFA_DEVICETREE ?= ${TF_A_DEVICETREE}" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    else
        echo "TFA_DEVICETREE ?=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    fi
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "help:" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"Available targets:\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"  all   : build TF-A binaries for defined config(s)\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"  clean : clean build directories from generated files\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"TF-A configuration:\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"  TF_A_CONFIG = \$(TF_A_CONFIG)\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"  TFA_DEVICETREE = \$(TFA_DEVICETREE)\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"  ELF_DEBUG_ENABLE = '\$(ELF_DEBUG_ENABLE)' ('1' to export elf files)\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "all: tf" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "host_tools:" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@\$(MAKE) --no-print-directory -C \$(LOCAL_PATH)/tools/stm32image" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "tf: host_tools" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	for config in \$(TF_A_CONFIG) ; do \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Init any extraoemake
    echo "		add_extraoemake= ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		for fullconfig in \$(TF_A_CONFIG_OEMAKE) ; do \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			extraconfig=\$\$(echo \$\$fullconfig | cut -d',' -f1) ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			if [ \"\$\$extraconfig\" = \"\$\$config\" ]; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				add_extraoemake=\$\$(echo \$\$fullconfig | cut -d',' -f2) ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		done ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		mkdir -p \$(LOCAL_PATH)/../build/\$\$config ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		if test -n \"\$(TFA_DEVICETREE)\" ; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			for dt in \$(TFA_DEVICETREE) ; do \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				\$(MAKE) \$(EXTRA_OEMAKE) -C \$(LOCAL_PATH) DTB_FILE_NAME=\$\$dt.dtb BUILD_PLAT=\$(LOCAL_PATH)/../build/\$\$config \$\$add_extraoemake ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Copy binary file with explicit name
    echo "				cp -f \$(LOCAL_PATH)/../build/\$\$config/${TF_A_BASENAME}-\$\$dt.${TF_A_SUFFIX} \$(LOCAL_PATH)/../build/\$\$config/${TF_A_BASENAME}-\$\$dt-\$\$config.${TF_A_SUFFIX} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			done ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		else \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			\$(MAKE) \$(EXTRA_OEMAKE) -C \$(LOCAL_PATH) BUILD_PLAT=\$(LOCAL_PATH)/../build/\$\$config \$\$add_extraoemake; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Copy binary file with explicit name
    echo "			tf_version=\$\$(find \$(LOCAL_PATH)/../build/\$\$config -name ${TF_A_BASENAME}*.${TF_A_SUFFIX} -exec basename {} \; | sed \"s/\.${TF_A_SUFFIX}//\") ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			cp -f \$(LOCAL_PATH)/../build/\$\$config/\$\$tf_version.${TF_A_SUFFIX} \$(LOCAL_PATH)/../build/\$\$config/\$\$tf_version-\$\$config.${TF_A_SUFFIX} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Copy elf files with explicit name
    echo "		if [ \"\$(ELF_DEBUG_ENABLE)\" = \"1\" ] ; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			if [ -f \$(LOCAL_PATH)/../build/\$\$config/${BL1_ELF} ] ; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				cp -f \$(LOCAL_PATH)/../build/\$\$config/${BL1_ELF} \$(LOCAL_PATH)/../build/\$\$config/${TF_A_BASENAME}-${BL1_BASENAME}-\$\$config.${TF_A_ELF_SUFFIX} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			if [ -f \$(LOCAL_PATH)/../build/\$\$config/${BL2_ELF} ] ; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				cp -f \$(LOCAL_PATH)/../build/\$\$config/${BL2_ELF} \$(LOCAL_PATH)/../build/\$\$config/${TF_A_BASENAME}-${BL2_BASENAME}-\$\$config.${TF_A_ELF_SUFFIX} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			if [ -f \$(LOCAL_PATH)/../build/\$\$config/${BL32_ELF} ] ; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				cp -f \$(LOCAL_PATH)/../build/\$\$config/${BL32_ELF} \$(LOCAL_PATH)/../build/\$\$config/${TF_A_BASENAME}-${BL32_BASENAME}-\$\$config.${TF_A_ELF_SUFFIX} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	done" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "clean:" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@for config in \$(TF_A_CONFIG) ; do \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		rm -rf \$(LOCAL_PATH)/../build/\$\$config ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	done" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
}
do_ar_original[prefuncs] += "tfaconfig_env archiver_create_makefile_for_sdk"
