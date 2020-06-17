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
    cat << EOF > ${ARCHIVER_OUTDIR}/Makefile.sdk
#remove default variable
LDFLAGS=
CFLAGS=
CPPFLAGS=
CC=
CPP=
AS=
AR=
LD=
NM=

LOCAL_PATH=\$(PWD)

EXTRA_OEMAKE=${EXTRA_OEMAKE}
EXTRA_OEMAKE_SERIAL=$(subst STM32MP_SDMMC=1 STM32MP_EMMC=1 STM32MP_SPI_NOR=1 STM32MP_RAW_NAND=1 STM32MP_SPI_NAND=1,,${EXTRA_OEMAKE}) STM32MP_UART_PROGRAMMER=1 STM32MP_USB_PROGRAMMER=1

# Set default config
ELF_DEBUG_ENABLE ?= ${ELF_DEBUG_ENABLE}
TF_A_CONFIG ?= ${TF_A_CONFIG} serialboot

# Set specific OEMAKE config
TF_A_CONFIG_OEMAKE = ${tf_a_config_oemake} serialboot,"AARCH32_SP=sp_min"
# Set default TFA_DEVICETREE
TFA_DEVICETREE ?= ${TF_A_DEVICETREE}

#Set default TF_A_ENABLE_DEBUG_WRAPPER
TF_A_ENABLE_DEBUG_WRAPPER ?= ${TF_A_ENABLE_DEBUG_WRAPPER}

help:
	@echo
	@echo "Available targets:"
	@echo "  all   : build TF-A binaries for defined config(s)"
	@echo "  clean : clean build directories from generated files"
	@echo
	@echo "TF-A configuration:"
	@echo "  TF_A_CONFIG = \$(TF_A_CONFIG)"
	@echo "  TFA_DEVICETREE = \$(TFA_DEVICETREE)"
	@echo "  ELF_DEBUG_ENABLE = '\$(ELF_DEBUG_ENABLE)' ('1' to export elf files)"
	@echo "  TF_A_ENABLE_DEBUG_WRAPPER = '\$(TF_A_ENABLE_DEBUG_WRAPPER)' ('1' to generate tf-a for debugging)"
	@echo

all: tf

host_tools:
	@\$(MAKE) --no-print-directory -C \$(LOCAL_PATH)/tools/stm32image

tf: host_tools
	for config in \$(TF_A_CONFIG) ; do \\
		# Init any extraoemake \\
		add_extraoemake= ; \\
		for fullconfig in \$(TF_A_CONFIG_OEMAKE) ; do \\
			extraconfig=\$\$(echo \$\$fullconfig | cut -d',' -f1) ; \\
			if [ "\$\$extraconfig" = "\$\$config" ]; then \\
				add_extraoemake=\$\$(echo \$\$fullconfig | cut -d',' -f2) ; \\
			fi ; \\
		done ; \\
		mkdir -p \$(LOCAL_PATH)/../build/\$\$config ; \\
		if test -n "\$(TFA_DEVICETREE)" ; then \\
			for dt in \$(TFA_DEVICETREE) ; do \\
				if [ "\$(TF_A_CONFIG)" != "serialboot" ]; then \\
					\$(MAKE) \$(EXTRA_OEMAKE) -C \$(LOCAL_PATH) DTB_FILE_NAME=\$\$dt.dtb BUILD_PLAT=\$(LOCAL_PATH)/../build/\$\$config \$\$add_extraoemake ; \\
				else \\
					\$(MAKE) \$(EXTRA_OEMAKE_SERIAL) -C \$(LOCAL_PATH) DTB_FILE_NAME=\$\$dt.dtb BUILD_PLAT=\$(LOCAL_PATH)/../build/\$\$config \$\$add_extraoemake ; \\
				fi \\
				# Copy binary file with explicit name \\
				cp -f \$(LOCAL_PATH)/../build/\$\$config/${TF_A_BASENAME}-\$\$dt.${TF_A_SUFFIX} \$(LOCAL_PATH)/../build/\$\$config/${TF_A_BASENAME}-\$\$dt-\$\$config.${TF_A_SUFFIX} ; \\
				if [ "\$(TF_A_ENABLE_DEBUG_WRAPPER)" = "1" ]; then \\
					# Generate wrapper for debugging \\
					stm32wrapper4dbg -s \$(LOCAL_PATH)/../build/\$\$config/${TF_A_BASENAME}-\$\$dt.${TF_A_SUFFIX} -d \$(LOCAL_PATH)/../build/\$\$config/debug-${TF_A_BASENAME}-\$\$dt-\$\$config.${TF_A_SUFFIX} ; \\
				fi \\
			done ; \\
		else \\
			\$(MAKE) \$(EXTRA_OEMAKE) -C \$(LOCAL_PATH) BUILD_PLAT=\$(LOCAL_PATH)/../build/\$\$config \$\$add_extraoemake; \\
			tf_version=\$\$(find \$(LOCAL_PATH)/../build/\$\$config -name ${TF_A_BASENAME}*.${TF_A_SUFFIX} -exec basename {} \; | sed "s/\.${TF_A_SUFFIX}//") ; \\
			# Copy binary file with explicit name \\
			cp -f \$(LOCAL_PATH)/../build/\$\$config/\$\$tf_version.${TF_A_SUFFIX} \$(LOCAL_PATH)/../build/\$\$config/\$\$tf_version-\$\$config.${TF_A_SUFFIX} ; \\
			if [ "\$(TF_A_ENABLE_DEBUG_WRAPPER)" = "1" ]; then \\
				# Generate wrapper for debugging \\
				stm32wrapper4dbg -s \$(LOCAL_PATH)/../build/\$\$config/\$\$tf_version.${TF_A_SUFFIX} \$(LOCAL_PATH)/../build/\$\$config/debug-\$\$tf_version-\$\$config.${TF_A_SUFFIX} ; \\
			fi \\
		fi ; \\
		# Copy elf files with explicit name \\
		if [ "\$(ELF_DEBUG_ENABLE)" = "1" ] ; then \\
			if [ -f \$(LOCAL_PATH)/../build/\$\$config/${BL2_ELF} ] ; then \\
				cp -f \$(LOCAL_PATH)/../build/\$\$config/${BL2_ELF} \$(LOCAL_PATH)/../build/\$\$config/${TF_A_BASENAME}-${BL2_BASENAME}-\$\$config.${TF_A_ELF_SUFFIX} ; \\
			fi ; \\
			if [ -f \$(LOCAL_PATH)/../build/\$\$config/${BL32_ELF} ] ; then \\
				cp -f \$(LOCAL_PATH)/../build/\$\$config/${BL32_ELF} \$(LOCAL_PATH)/../build/\$\$config/${TF_A_BASENAME}-${BL32_BASENAME}-\$\$config.${TF_A_ELF_SUFFIX} ; \\
			fi ; \\
		fi ; \\
	done

clean:
	@for config in \$(TF_A_CONFIG) ; do \\
		rm -rf \$(LOCAL_PATH)/../build/\$\$config ; \\
	done
EOF
}
do_ar_original[prefuncs] += "tfaconfig_env archiver_create_makefile_for_sdk"
