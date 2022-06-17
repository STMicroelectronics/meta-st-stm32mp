#
# Archiver Configuration
#
SRC_URI:append = " file://README.HOW_TO.txt "

inherit archiver
ARCHIVER_MODE[src] = "original"
COPYLEFT_LICENSE_INCLUDE:append = " BSD-3* "

inherit archiver_stm32mp_clean

archiver_create_makefile_for_sdk() {
    mkdir -p ${ARCHIVER_OUTDIR}

    cat << EOF > ${ARCHIVER_OUTDIR}/Makefile.sdk
# Set default path
SRC_PATH ?= \$(PWD)
BLD_PATH ?= \$(SRC_PATH)/../build
DEPLOYDIR ?= \$(SRC_PATH)/../deploy

# Set default optee-os config
CFG_EMBED_DTB_SOURCE_FILE ?= ${OPTEE_CONF}

# Remove default variables
LDFLAGS =
CFLAGS =
CPPFLAGS =
# Define default make options
EXTRA_OEMAKE = $(echo "${EXTRA_OEMAKE}" | sed "s|LIBGCC_LOCATE_CFLAGS=[^ ]* |LIBGCC_LOCATE_CFLAGS=\$(KCFLAGS) |")

# Set dependencies list for building all
DEPS = optee
DEPS += fip

help:
	@echo
	@echo "OPTEE-OS configuration:"
	@echo "  CFG_EMBED_DTB_SOURCE_FILE = \$(CFG_EMBED_DTB_SOURCE_FILE)"
	@echo
	@echo "OPTEE-OS folder configuration:"
	@echo "  SRC_PATH  = \$(SRC_PATH)"
	@echo "  BLD_PATH  = \$(BLD_PATH)"
	@echo "  DEPLOYDIR = \$(DEPLOYDIR)"
	@echo
	@echo "FIP configuration:"
	@echo "  Do not forget to set FIP deploydir folders (such as FIP_DEPLOYDIR_ROOT) to provide path to needed binaries"
	@echo
	@echo "Available targets:"
	@echo "  all   : default target to build all binaries for defined config(s)"
	@echo "  fip   : build FIP binaries for defined config(s)"
	@echo "  optee : build OPTEE-OS binaries for defined config(s)"
	@echo "  clean : clean build directories from generated files"
	@echo

all: \$(DEPS)

optee:
	@mkdir -p \$(DEPLOYDIR)
	@mkdir -p \$(DEPLOYDIR)/debug
	@if [ -n "\$(CFG_EMBED_DTB_SOURCE_FILE)" ]; then \\
		for dt in \$(CFG_EMBED_DTB_SOURCE_FILE); do \\
			# Configure SOC switch \\
			soc_extra="" ; \\
			for soc in ${STM32MP_SOC_NAME} ; do \\
				if [ "\$\$(echo \$\$dt | grep -c \$\$soc)" -eq 1 ]; then \\
					soc_extra="\$\$(echo CFG_\$\$soc | tr a-z A-Z)=y" ; \\
					break ; \\
				fi ; \\
			done ; \\
			\$(MAKE) \$(EXTRA_OEMAKE) -C \$(SRC_PATH) PREFIX=\$(SDKTARGETSYSROOT) O=\$(BLD_PATH)/\$\$dt CFG_EMBED_DTB_SOURCE_FILE=\$\$dt.dts \$\$soc_extra ; \\
			# Copy binary files with explicit name \\
			cp \$(BLD_PATH)/\$\$dt/core/${OPTEE_HEADER}.${OPTEE_SUFFIX} \$(DEPLOYDIR)/${OPTEE_HEADER}-\$\$dt.${OPTEE_SUFFIX} ; \\
			cp \$(BLD_PATH)/\$\$dt/core/${OPTEE_PAGER}.${OPTEE_SUFFIX} \$(DEPLOYDIR)/${OPTEE_PAGER}-\$\$dt.${OPTEE_SUFFIX} ; \\
			cp \$(BLD_PATH)/\$\$dt/core/${OPTEE_PAGEABLE}.${OPTEE_SUFFIX} \$(DEPLOYDIR)/${OPTEE_PAGEABLE}-\$\$dt.${OPTEE_SUFFIX} ; \\
			cp \$(BLD_PATH)/\$\$dt/core/${OPTEE_ELF}.${OPTEE_ELF_SUFFIX} \$(DEPLOYDIR)/debug/${OPTEE_ELF}-\$\$dt.${OPTEE_ELF_SUFFIX} ; \\
		done ; \\
	else \\
		\$(MAKE) \$(EXTRA_OEMAKE) -C \$(SRC_PATH) PREFIX=\$(SDKTARGETSYSROOT) O=\$(BLD_PATH)/ ; \\
		# Copy binary files with explicit name \\
		cp \$(BLD_PATH)/core/${OPTEE_HEADER}.${OPTEE_SUFFIX} \$(DEPLOYDIR)/ ; \\
		cp \$(BLD_PATH)/core/${OPTEE_PAGER}.${OPTEE_SUFFIX} \$(DEPLOYDIR)/ ; \\
		cp \$(BLD_PATH)/core/${OPTEE_PAGEABLE}.${OPTEE_SUFFIX} \$(DEPLOYDIR)/ ; \\
		cp \$(BLD_PATH)/core/${OPTEE_ELF}.${OPTEE_ELF_SUFFIX} \$(DEPLOYDIR)/debug/ ; \\
	fi

fip: optee
	FIP_DEPLOYDIR_OPTEE=\$(DEPLOYDIR) FIP_DEVICETREE="\$(CFG_EMBED_DTB_SOURCE_FILE)" FIP_CONFIG="optee" FIP_BL32_CONF="optee" fiptool-stm32mp

clean:
	@echo "Removing \$(BLD_PATH) ..."
	@rm -rf \$(BLD_PATH)
	@echo "Removing \$(DEPLOYDIR) ..."
	@rm -rf \$(DEPLOYDIR)
	@echo
EOF
}
do_ar_original[prefuncs] += "archiver_create_makefile_for_sdk"
