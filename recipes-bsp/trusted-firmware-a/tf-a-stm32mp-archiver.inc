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

BL32_DEPLOYDIR     ?= \$(DEPLOYDIR)/bl32
FWCONFIG_DEPLOYDIR ?= \$(DEPLOYDIR)/fwconfig

# Configure default TF-A features
TF_A_ENABLE_DEBUG_WRAPPER ?= ${TF_A_ENABLE_DEBUG_WRAPPER}

# Configure FIP
ENABLE_FIP ?= ${@bb.utils.contains('MACHINE_FEATURES', 'fip', '1', '', d)}

# Set default TF-A config
ifeq (\$(ENABLE_FIP), 1)
TF_A_CONFIG ?= ${TF_A_CONFIG}
else
TF_A_CONFIG ?= ${BOOTSCHEME_LABELS} ${TF_A_CONFIG_4LEGACY}
endif

# Default TF-A overall settings to null
TF_A_BINARY ?=
TF_A_DEVICETREE ?=
TF_A_MAKE_TARGET ?=
TF_A_EXTRA_OPTFLAGS ?=

ifeq (\$(ENABLE_FIP), 1)

EOF
    unset i
    for config in ${TF_A_CONFIG}; do
        i=$(expr $i + 1)
        cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk
# Init default config settings
TF_A_DEVICETREE_$config ?= $(echo ${TF_A_DEVICETREE} | cut -d',' -f${i})
TF_A_EXTRA_OPTFLAGS_$config ?= $(echo ${TF_A_EXTRA_OPTFLAGS} | cut -d',' -f${i})
TF_A_BINARY_$config ?= $(echo ${TF_A_BINARIES} | cut -d',' -f${i})
TF_A_MAKE_TARGET_$config ?= $(echo ${TF_A_MAKE_TARGET} | cut -d',' -f${i})

EOF
    done
    cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk
else

EOF
    for bootscheme in ${BOOTSCHEME_LABELS}; do
        unset i
        for config in ${TF_A_CONFIG}; do
            i=$(expr $i + 1)
            if [ "$bootscheme" = "$config" ]; then
                cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk
# Init default config settings
TF_A_DEVICETREE_$config ?= $(echo ${TF_A_DEVICETREE} | cut -d',' -f${i})
TF_A_EXTRA_OPTFLAGS_$config ?= $(echo ${TF_A_EXTRA_OPTFLAGS} | cut -d',' -f${i}) ${TF_A_EXTRACONF_LEGACY}
TF_A_BINARY_$config ?= $(echo ${TF_A_BINARIES} | cut -d',' -f${i})
TF_A_MAKE_TARGET_$config ?=

EOF
            fi
        done
    done

    cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk
TF_A_DEVICETREE_$(echo ${TF_A_CONFIG_4LEGACY} | xargs) ?=
TF_A_EXTRA_OPTFLAGS_$(echo ${TF_A_CONFIG_4LEGACY} | xargs) ?=
TF_A_BINARY_$(echo ${TF_A_CONFIG_4LEGACY} | xargs) ?=
TF_A_MAKE_TARGET_$(echo ${TF_A_CONFIG_4LEGACY} | xargs) ?=

ifeq (\$(TF_A_DEVICETREE_$(echo ${TF_A_CONFIG_4LEGACY} | xargs)),)
\$(error "Please, define the variable TF_A_DEVICETREE_$(echo ${TF_A_CONFIG_4LEGACY} | xargs)")
endif

ifeq (\$(TF_A_EXTRA_OPTFLAGS_$(echo ${TF_A_CONFIG_4LEGACY} | xargs)),)
\$(error "Please, define the variable TF_A_EXTRA_OPTFLAGS_$(echo ${TF_A_CONFIG_4LEGACY} | xargs)")
endif

ifeq (\$(TF_A_BINARY_$(echo ${TF_A_CONFIG_4LEGACY} | xargs)),)
\$(error "Please, define the variable TF_A_BINARY_$(echo ${TF_A_CONFIG_4LEGACY} | xargs)")
endif

endif

# Reset default variables
LDFLAGS =
CFLAGS =
CPPFLAGS =
CC =
CPP =
AS =
AR =
LD =
NM =
# Define default make options
EXTRA_OEMAKE ?= ${EXTRA_OEMAKE}

# Configure overall devicetree list for FIP
TF_A_DEVICETREE_ALL = $(echo "${TF_A_DEVICETREE}" | sed 's/,/ /g' | xargs -n1 | sort -u | xargs)

# Display TF-A config details
define tf-configs
	echo "  \$(1)" ; \\
	echo "    with device tree : \$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_\$(1)))" ; \\
	echo "    extra optionflags: \$(if \$(TF_A_EXTRA_OPTFLAGS),\$(TF_A_EXTRA_OPTFLAGS),\$(TF_A_EXTRA_OPTFLAGS_\$(1)))" ; \\
	echo "    binary basename  : \$(if \$(TF_A_BINARY),\$(TF_A_BINARY),\$(TF_A_BINARY_\$(1)) (TF_A_BINARY_\$(1)))" ; \\
	echo "    tf-a build target: \$(if \$(TF_A_MAKE_TARGET),\$(TF_A_MAKE_TARGET),\$(TF_A_MAKE_TARGET_\$(1)))" ;
endef

# Configure TF-A make rules
define tf-rules
tf-\$(1): \$2
	@mkdir -p \$(BLD_PATH)/\$(1)
	@\$(foreach dt, \$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_\$(1))), \\
		\$(MAKE) \$(EXTRA_OEMAKE) -C \$(SRC_PATH) \\
			BUILD_PLAT=\$(BLD_PATH)/\$(1) \\
			DTB_FILE_NAME=\$(dt).dtb \\
			\$(if \$(TF_A_EXTRA_OPTFLAGS),\$(TF_A_EXTRA_OPTFLAGS),\$(TF_A_EXTRA_OPTFLAGS_\$(1))) \\
			\$(if \$(TF_A_MAKE_TARGET),\$(TF_A_MAKE_TARGET),\$(TF_A_MAKE_TARGET_\$(1))) \\
			\$(foreach soc,${STM32MP_SOC_NAME},\$(if \$(findstring \$(soc),\$(dt)),\$(shell echo \$(soc) | tr a-z A-Z)=1,)) ; \\
	)
endef

# Configure TF-A deploy rules
define deploy-rules
deploy-\$(1): tf-\$(1)
	@mkdir -p \$(DEPLOYDIR)
	@mkdir -p \$(DEPLOYDIR)/debug
	@\$(foreach dt, \$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_\$(1))), \\
		if [ -f \$(BLD_PATH)/\$(1)/\$(2)-\$(dt).${TF_A_SUFFIX} ]; then \\
			cp -f \$(BLD_PATH)/\$(1)/\$(2)-\$(dt).${TF_A_SUFFIX} \$(DEPLOYDIR)/\$(2)-\$(dt)-\$(1).${TF_A_SUFFIX} ; \\
			if [ "\$(TF_A_ENABLE_DEBUG_WRAPPER)" = "1" ] ; then \\
				stm32wrapper4dbg -s \$(BLD_PATH)/\$(1)/\$(2)-\$(dt).${TF_A_SUFFIX} -d \$(DEPLOYDIR)/debug/debug-\$(2)-\$(dt)-\$(1).${TF_A_SUFFIX} ; \\
			fi ; \\
		fi ; \\
		if [ -f \$(BLD_PATH)/\$(1)/${BL32_BASENAME}.${BL32_SUFFIX} ] ; then \\
			mkdir -p \$(BL32_DEPLOYDIR) ; \\
			cp -f \$(BLD_PATH)/\$(1)/${BL32_BASENAME}.${BL32_SUFFIX} \$(BL32_DEPLOYDIR)/\$(2)-${BL32_BASENAME}\$(strip \$(foreach soc,${STM32MP_SOC_NAME},\$(if \$(findstring \$(soc),\$(dt)),-\$(soc),))).${BL32_SUFFIX} ; \\
			if [ -f \$(BLD_PATH)/\$(1)/fdts/\$(dt)-${BL32_BASENAME}.${DT_SUFFIX} ]; then \\
				cp -f \$(BLD_PATH)/\$(1)/fdts/\$(dt)-${BL32_BASENAME}.${DT_SUFFIX} \$(BL32_DEPLOYDIR)/ ; \\
			fi ; \\
			if [ -f \$(BLD_PATH)/\$(1)/${BL32_ELF} ]; then \\
				mkdir -p \$(BL32_DEPLOYDIR)/debug ; \\
				mv -f \$(BLD_PATH)/\$(1)/${BL32_ELF} \$(BL32_DEPLOYDIR)/debug/\$(2)-${BL32_BASENAME}\$(strip \$(foreach soc,${STM32MP_SOC_NAME},\$(if \$(findstring \$(soc),\$(dt)),-\$(soc),))).${TF_A_ELF_SUFFIX} ; \\
			fi ; \\
		fi ; \\
		if [ -f \$(BLD_PATH)/\$(1)/fdts/\$(dt)-${FWCONFIG_NAME}.${DT_SUFFIX} ]; then \\
			mkdir -p \$(FWCONFIG_DEPLOYDIR) ; \\
			cp -f \$(BLD_PATH)/\$(1)/fdts/\$(dt)-${FWCONFIG_NAME}.${DT_SUFFIX} \$(FWCONFIG_DEPLOYDIR)/\$(dt)-${FWCONFIG_NAME}-\$(1).${DT_SUFFIX} ; \\
		fi ; \\
		if [ -f \$(BLD_PATH)/\$(1)/${BL2_ELF} ] ; then \\
			cp -f \$(BLD_PATH)/\$(1)/${BL2_ELF} \$(DEPLOYDIR)/debug/\$(2)-${BL2_BASENAME}-\$(1).${TF_A_ELF_SUFFIX} ; \\
		fi ; \\
		if [ -f \$(BLD_PATH)/\$(1)/${BL32_ELF} ] ; then \\
			cp -f \$(BLD_PATH)/\$(1)/${BL32_ELF} \$(DEPLOYDIR)/debug/\$(2)-${BL32_BASENAME}-\$(1).${TF_A_ELF_SUFFIX} ; \\
		fi ; \\
	)
endef

# Set dependencies list for building all
DEPS = \$(addprefix deploy-,\$(TF_A_CONFIG))
ifeq (\$(ENABLE_FIP), 1)
DEPS += fip
endif

help:
	@echo
	@echo "TF-A configuration:"
	@echo "  TF_A_CONFIG = \$(TF_A_CONFIG)"
	@echo "Config details:"
	@\$(foreach config, \$(TF_A_CONFIG), \$(call tf-configs,\$(config)))
	@echo
	@echo "Note that each TF-A configuration settings can be updated through overall or specific config var:"
	@echo "  TF_A_DEVICETREE"
	@echo "  TF_A_EXTRA_OPTFLAGS"
	@echo "  TF_A_BINARY"
	@echo "  TF_A_MAKE_TARGET"
	@echo
	@echo "TF-A features configuration:"
	@echo "  TF_A_ENABLE_DEBUG_WRAPPER = \$(TF_A_ENABLE_DEBUG_WRAPPER) ('1' to generate tf-a for debugging)"
	@echo
	@echo "TF-A folder configuration:"
	@echo "  SRC_PATH  = \$(SRC_PATH)"
	@echo "  BLD_PATH  = \$(BLD_PATH)"
	@echo "  DEPLOYDIR = \$(DEPLOYDIR)"
	@echo "  BL32_DEPLOYDIR     = \$(DEPLOYDIR)/bl32"
	@echo "  FWCONFIG_DEPLOYDIR = \$(DEPLOYDIR)/fwconfig"
	@echo
	@echo "FIP configuration:"
	@echo "  ENABLE_FIP = \$(ENABLE_FIP) ('1' to generate fip binary)"
ifeq (\$(ENABLE_FIP),1)
	@echo "  Do not forget to set FIP deploydir folders (such as FIP_DEPLOYDIR_ROOT) to provide path to needed binaries"
endif
	@echo
	@echo "Available targets:"
	@echo "  all   : build TF-A binaries for defined config(s)"
	@echo "  fip   : build FIP binaries"
	@echo "  stm32 : build TF-A stm32 binaries"
	@echo "  clean : clean build directories from generated files"
	@echo

all: \$(DEPS)

host_tools:
	@\$(MAKE) --no-print-directory -C \$(SRC_PATH)/tools/stm32image

# Set TF-A make rules
\$(foreach config, \$(TF_A_CONFIG), \$(eval \$(call tf-rules,\$(config),host_tools)))

# Set TF-A deploy rules
\$(foreach config, \$(TF_A_CONFIG), \$(eval \$(call deploy-rules,\$(config),\$(if \$(TF_A_BINARY),\$(TF_A_BINARY),\$(TF_A_BINARY_\$(config))))))

fip: \$(addprefix deploy-,\$(TF_A_CONFIG))
	FIP_DEPLOYDIR_TFA=\$(BL32_DEPLOYDIR) FIP_DEPLOYDIR_FWCONF=\$(FWCONFIG_DEPLOYDIR) FIP_DEVICETREE="\$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_ALL))" fiptool-stm32mp

stm32: \$(addprefix deploy-,\$(TF_A_CONFIG))

clean:
	@for config in \$(TF_A_CONFIG) ; do \\
		echo "Removing \$(BLD_PATH)/\$\$config ..." ; \\
		rm -rf \$(BLD_PATH)/\$\$config ; \\
	done
	@echo "Removing \$(DEPLOYDIR) ..."
	@rm -rf \$(DEPLOYDIR)
	@echo
EOF
}
do_ar_original[prefuncs] += "archiver_create_makefile_for_sdk"
