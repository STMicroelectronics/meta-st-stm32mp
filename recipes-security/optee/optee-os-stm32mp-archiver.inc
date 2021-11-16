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

    cat << EOF > ${ARCHIVER_OUTDIR}/Makefile.sdk
# Set default path
SRC_PATH ?= \$(PWD)
BLD_PATH ?= \$(SRC_PATH)/../build
DEPLOYDIR ?= \$(SRC_PATH)/../deploy

# Set default optee-os config
CFG_EMBED_DTB_SOURCE_FILE ?= ${OPTEE_CONF}

# Configure default optee-os features
ENABLE_FIP ?= ${@bb.utils.contains('MACHINE_FEATURES','fip','1','',d)}

# Remove default variables
LDFLAGS =
CFLAGS =
CPPFLAGS =
# Define default make options
EXTRA_OEMAKE = $(echo "${EXTRA_OEMAKE}" | sed "s|LIBGCC_LOCATE_CFLAGS=[^ ]* |LIBGCC_LOCATE_CFLAGS=\$(KCFLAGS) |")
ifeq (\$(ENABLE_FIP), 0)
EXTRA_OEMAKE += CFG_STM32MP15x_STM32IMAGE=y
endif

# Configure default optee-os features
ifeq (\$(ENABLE_FIP), 1)
OPTEE_SUFFIX ?= bin
else
OPTEE_SUFFIX ?= stm32
endif

# Set dependencies list for building all
DEPS = optee
ifeq (\$(ENABLE_FIP), 1)
DEPS += fip
endif

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
	@echo "  ENABLE_FIP = \$(ENABLE_FIP) ('1' to generate fip binary)"
ifeq (\$(ENABLE_FIP),1)
	@echo "  Do not forget to set FIP deploydir folders (such as FIP_DEPLOYDIR_ROOT) to provide path to needed binaries"
endif
	@echo
	@echo "Available targets:"
	@echo "  all   : build OPTEE-OS binaries for defined config(s)"
	@echo "  clean : clean build directories from generated files"
	@echo

all: \$(DEPS)

optee:
	@mkdir -p \$(DEPLOYDIR)
	@mkdir -p \$(DEPLOYDIR)/debug
	@if test -n "\$(CFG_EMBED_DTB_SOURCE_FILE)" ; then \\
		for dt in \$(CFG_EMBED_DTB_SOURCE_FILE) ; do \\
			\$(MAKE) \$(EXTRA_OEMAKE) -C \$(SRC_PATH) PREFIX=\$(SDKTARGETSYSROOT) O=\$(BLD_PATH)/\$\$dt CFG_EMBED_DTB_SOURCE_FILE=\$\$dt.dts ; \\
			# Copy binary files with explicit name \\
			cp \$(BLD_PATH)/\$\$dt/core/${OPTEE_HEADER}.\$(OPTEE_SUFFIX) \$(DEPLOYDIR)/${OPTEE_HEADER}-\$\$dt.\$(OPTEE_SUFFIX) ; \\
			cp \$(BLD_PATH)/\$\$dt/core/${OPTEE_PAGER}.\$(OPTEE_SUFFIX) \$(DEPLOYDIR)/${OPTEE_PAGER}-\$\$dt.\$(OPTEE_SUFFIX) ; \\
			cp \$(BLD_PATH)/\$\$dt/core/${OPTEE_PAGEABLE}.\$(OPTEE_SUFFIX) \$(DEPLOYDIR)/${OPTEE_PAGEABLE}-\$\$dt.\$(OPTEE_SUFFIX) ; \\
			cp \$(BLD_PATH)/\$\$dt/core/${OPTEE_ELF}.${OPTEE_ELF_SUFFIX} \$(DEPLOYDIR)/debug/${OPTEE_ELF}-\$\$dt.${OPTEE_ELF_SUFFIX} ; \\
		done ; \\
	else \\
		\$(MAKE) \$(EXTRA_OEMAKE) -C \$(SRC_PATH) PREFIX=\$(SDKTARGETSYSROOT) O=\$(BLD_PATH)/ ; \\
		# Copy binary files with explicit name \\
		cp \$(BLD_PATH)/core/${OPTEE_HEADER}.\$(OPTEE_SUFFIX) \$(DEPLOYDIR)/ ; \\
		cp \$(BLD_PATH)/core/${OPTEE_PAGER}.\$(OPTEE_SUFFIX) \$(DEPLOYDIR)/ ; \\
		cp \$(BLD_PATH)/core/${OPTEE_PAGEABLE}.\$(OPTEE_SUFFIX) \$(DEPLOYDIR)/ ; \\
		cp \$(BLD_PATH)/core/${OPTEE_ELF}.${OPTEE_ELF_SUFFIX} \$(DEPLOYDIR)/debug/ ; \\
	fi

fip: optee
	FIP_DEPLOYDIR_OPTEE=\$(DEPLOYDIR) FIP_DEVICETREE="\$(CFG_EMBED_DTB_SOURCE_FILE)" fiptool-stm32mp

clean:
	@echo "Removing \$(BLD_PATH) ..."
	@rm -rf \$(BLD_PATH)
	@echo "Removing \$(DEPLOYDIR) ..."
	@rm -rf \$(DEPLOYDIR)
	@echo
EOF
}
do_ar_original[prefuncs] += "archiver_create_makefile_for_sdk"
