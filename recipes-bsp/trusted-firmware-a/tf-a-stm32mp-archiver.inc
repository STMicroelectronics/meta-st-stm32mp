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
FIPTOOLDIR ?= \$(SRC_PATH)/..

BL31_DEPLOYDIR     ?= \$(DEPLOYDIR)/bl31
BL32_DEPLOYDIR     ?= \$(DEPLOYDIR)/bl32
FWCONFIG_DEPLOYDIR ?= \$(DEPLOYDIR)/fwconfig
FWDDR_DEPLOYDIR    ?= \$(DEPLOYDIR)/ddr

# Configure default TF-A features
TF_A_ENABLE_BL31 ?= ${FIP_BL31_ENABLE}
TF_A_ENABLE_DEBUG_WRAPPER ?= ${TF_A_ENABLE_DEBUG_WRAPPER}
TF_A_ENABLE_FWDDR ?= ${TF_A_FWDDR}

# Set default TF-A config
TF_A_CONFIG ?= ${TF_A_CONFIG}

# Set default FIP config
FIP_CONFIG ?= ${@' '.join(d for d in '${FIP_CONFIG}'.split() if not 'fastboot-' in d)}

# Default TF-A overall settings to null
TF_A_BINARY ?=
TF_A_DEVICETREE ?=
TF_A_MAKE_TARGET ?=
TF_A_EXTRA_OPTFLAGS ?=

# Default TF-A metadata template
TF_A_METADATA_JSON ?= \$(SRC_PATH)/${TF_A_METADATA_JSON}

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
EXTRA_OEMAKE ?= $(echo "${EXTRA_OEMAKE}" |sed 's,TFA_EXTERNAL_DT='"${STAGING_EXTDT_DIR}/${EXTDT_DIR_TF_A}"',TFA_EXTERNAL_DT=\$(EXTDT_DIR)/'"${EXTDT_DIR_TF_A}"',')

# Display TF-A config details
define tf-configs
	echo "  \$(1)" ; \\
	echo "    with device tree : \$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_\$(1)))" ; \\
	echo "    extra optionflags: \$(if \$(TF_A_EXTRA_OPTFLAGS),\$(TF_A_EXTRA_OPTFLAGS),\$(TF_A_EXTRA_OPTFLAGS_\$(1)))" ; \\
	echo "    binary basename  : \$(if \$(TF_A_BINARY),\$(TF_A_BINARY),\$(TF_A_BINARY_\$(1)) (TF_A_BINARY_\$(1)))" ; \\
	echo "    tf-a build target: \$(if \$(TF_A_MAKE_TARGET),\$(TF_A_MAKE_TARGET),\$(TF_A_MAKE_TARGET_\$(1)))" ;
endef

# Configure TF-A make rules ( tf-<CONFIG>-<SOC>: <DEPS> )
define tf-rules
tf-\$(1)-\$(2): \$3
	@\$(foreach dt, \$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_\$(1))), \\
		\$(if \$(findstring \$(2),\$(dt)), \\
			\$(MAKE) \$(EXTRA_OEMAKE) -C \$(SRC_PATH) \\
				BUILD_PLAT=\$(BLD_PATH)/\$(1)-\$(2)-\$(dt) \\
				DTB_FILE_NAME=\$(dt).dtb \\
				\$(if \$(TF_A_EXTRA_OPTFLAGS),\$(TF_A_EXTRA_OPTFLAGS),\$(TF_A_EXTRA_OPTFLAGS_\$(1))) \\
				dtbs \\
				\$(shell echo \$(2) | tr a-z A-Z)=1 || exit 1; \\
			touch \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/\$(dt)-ddr_property.txt ; \\
			if [ "\$(TF_A_ENABLE_FWDDR)" = "1" ] ; then \\
				if [ -f "\$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/fdts/\$(dt)-bl2.dtb" ]; then \\
					fdtget -l "\$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/fdts/\$(dt)-bl2.dtb" /soc | grep ddr | head -n 1 > \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/\$(dt)-ddr_dtb_node.txt ; \\
					(fdtget "\$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/fdts/\$(dt)-bl2.dtb" /soc/\`cat \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/\$(dt)-ddr_dtb_node.txt\` st,mem-name) > \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/\$(dt)-ddr_property.txt ; \\
					sed -i -e 's|^\(.*DDR[0-9]\).*| STM32MP_\1_TYPE=1 |' \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/\$(dt)-ddr_property.txt ; \\
					sed -e 's|.*_\(.*DDR[0-9]\)_.*|\1|' \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/\$(dt)-ddr_property.txt | tr A-Z a-z > \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/\$(dt)-ddr_target.txt ; \\
					if [ -f "\$(FWDDR_DIR)/${FWDDR_SRC_SUBDIR}/\`cat \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/\$(dt)-ddr_target.txt\`_pmu_train.bin" ]; then \\
						cp -f \$(FWDDR_DIR)/${FWDDR_SRC_SUBDIR}/\`cat \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/\$(dt)-ddr_target.txt\`_pmu_train.bin \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/${FWDDR_NAME}-\$(dt).${FWDDR_SUFFIX} ; \\
					fi ; \\
				fi ; \\
			fi ; \\
			\$(MAKE) \$(EXTRA_OEMAKE) -C \$(SRC_PATH) \\
				BUILD_PLAT=\$(BLD_PATH)/\$(1)-\$(2)-\$(dt) \\
				DTB_FILE_NAME=\$(dt).dtb \\
				\`cat \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/\$(dt)-ddr_property.txt\` \\
				\$(if \$(TF_A_EXTRA_OPTFLAGS),\$(TF_A_EXTRA_OPTFLAGS),\$(TF_A_EXTRA_OPTFLAGS_\$(1))) \\
				\$(if \$(TF_A_MAKE_TARGET),\$(TF_A_MAKE_TARGET),\$(TF_A_MAKE_TARGET_\$(1))) \\
				\$(shell echo \$(2) | tr a-z A-Z)=1 || exit 1; \\
		,) \\
	)
endef

# Configure TF-A deploy rules ( deploy-<CONFIG>-<SOC>: tf-<CONFIG>-<SOC> [\$(3)=<BINARY>] )
define deploy-rules
deploy-\$(1)-\$(2): tf-\$(1)-\$(2)
	@mkdir -p \$(DEPLOYDIR)
	@mkdir -p \$(DEPLOYDIR)/debug
	@\$(foreach dt, \$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_\$(1))), \\
		if [ -f "\$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/${FWDDR_NAME}-\$(dt).${FWDDR_SUFFIX}" ]; then \\
			mkdir -p \$(FWDDR_DEPLOYDIR) ; \\
			cp -f \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/${FWDDR_NAME}-\$(dt).${FWDDR_SUFFIX} \$(FWDDR_DEPLOYDIR)/ ; \\
		fi ; \\
	)
	@\$(foreach dt, \$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_\$(1))), \\
		if [ -f \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/\$(3)-\$(dt).${TF_A_SUFFIX} ]; then \\
			cp -f \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/\$(3)-\$(dt).${TF_A_SUFFIX} \$(DEPLOYDIR)/\$(3)-\$(dt)-\$(1).${TF_A_SUFFIX} ; \\
			if [ "\$(TF_A_ENABLE_DEBUG_WRAPPER)" = "1" ] ; then \\
				stm32wrapper4dbg -s \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/\$(3)-\$(dt).${TF_A_SUFFIX} -d \$(DEPLOYDIR)/debug/debug-\$(3)-\$(dt)-\$(1).${TF_A_SUFFIX} ; \\
			fi ; \\
		fi ; \\
	)
	@\$(foreach dt, \$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_\$(1))), \\
		if [ -f \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/${BL31_BASENAME}.${BL31_SUFFIX} ] ; then \\
			mkdir -p \$(BL31_DEPLOYDIR) ; \\
			cp -f \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/${BL31_BASENAME}.${BL31_SUFFIX} \$(BL31_DEPLOYDIR)/\$(3)-${BL31_BASENAME}-\$(2)-\$(1).${BL31_SUFFIX} ; \\
			if [ -f \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/fdts/\$(dt)-${BL31_BASENAME}.${DT_SUFFIX} ]; then \\
				cp -f \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/fdts/\$(dt)-${BL31_BASENAME}.${DT_SUFFIX} \$(BL31_DEPLOYDIR)/\$(dt)-${BL31_BASENAME}-\$(1).${DT_SUFFIX}; \\
			fi ; \\
			if [ -f \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/${BL31_ELF} ]; then \\
				mkdir -p \$(BL31_DEPLOYDIR)/debug ; \\
				cp -f \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/${BL31_ELF} \$(BL31_DEPLOYDIR)/debug/\$(3)-${BL31_BASENAME}-\$(dt)-\$(1).${TF_A_ELF_SUFFIX} ; \\
			fi ; \\
		fi ; \\
	)
	@\$(foreach dt, \$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_\$(1))), \\
		if [ -f \$(BLD_PATH)/\$(1)-\$(2)/${BL32_BASENAME}.${BL32_SUFFIX} ] ; then \\
			mkdir -p \$(BL32_DEPLOYDIR) ; \\
			cp -f \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/${BL32_BASENAME}.${BL32_SUFFIX} \$(BL32_DEPLOYDIR)/\$(3)-${BL32_BASENAME}-\$(2)-\$(1).${BL32_SUFFIX} ; \\
			if [ -f \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/fdts/\$(dt)-${BL32_BASENAME}.${DT_SUFFIX} ]; then \\
				cp -f \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/fdts/\$(dt)-${BL32_BASENAME}.${DT_SUFFIX} \$(BL32_DEPLOYDIR)/\$(dt)-${BL32_BASENAME}-\$(1).${DT_SUFFIX} ; \\
			fi ; \\
			if [ -f \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/${BL32_ELF} ]; then \\
				mkdir -p \$(BL32_DEPLOYDIR)/debug ; \\
				cp -f \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/${BL32_ELF} \$(BL32_DEPLOYDIR)/debug/\$(3)-${BL32_BASENAME}-\$(2)-\$(1).${TF_A_ELF_SUFFIX} ; \\
			fi ; \\
		fi ; \\
	)
	@\$(foreach dt, \$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_\$(1))), \\
		if [ -f \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/fdts/\$(dt)-${FWCONFIG_NAME}.${DT_SUFFIX} ]; then \\
			mkdir -p \$(FWCONFIG_DEPLOYDIR) ; \\
			cp -f \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/fdts/\$(dt)-${FWCONFIG_NAME}.${DT_SUFFIX} \$(FWCONFIG_DEPLOYDIR)/\$(dt)-${FWCONFIG_NAME}-\$(1).${DT_SUFFIX} ; \\
		fi ; \\
	)
	@\$(foreach dt, \$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_\$(1))), \\
		if [ -f \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/${BL2_ELF} ] ; then \\
			cp -f \$(BLD_PATH)/\$(1)-\$(2)-\$(dt)/${BL2_ELF} \$(DEPLOYDIR)/debug/\$(3)-${BL2_BASENAME}-\$(2)-\$(1).${TF_A_ELF_SUFFIX} ; \\
		fi ; \\
	)
endef

# Configure overall deploy rules list
deploy-targets := \$(foreach config, \$(TF_A_CONFIG), \$(foreach soc, ${STM32MP_SOC_NAME}, deploy-\$(config)-\$(soc)))

# Set dependencies list for building all
DEPS = \$(deploy-targets)
DEPS += fip
DEPS += metadata

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
	@echo "  TF_A_ENABLE_BL31 = \$(TF_A_ENABLE_BL31) ('1' to generate bl31 binary)"
	@echo "  TF_A_ENABLE_DEBUG_WRAPPER = \$(TF_A_ENABLE_DEBUG_WRAPPER) ('1' to generate tf-a for debugging)"
	@echo "  TF_A_ENABLE_FWDDR = \$(TF_A_ENABLE_FWDDR) ('1' to generate tf-a ddr firmware for fip ddr)"
	@echo
	@echo "TF-A folder configuration:"
	@echo "  SRC_PATH  = \$(SRC_PATH)"
	@echo "  BLD_PATH  = \$(BLD_PATH)"
	@echo "  DEPLOYDIR = \$(DEPLOYDIR)"
	@echo "  BL31_DEPLOYDIR     = \$(DEPLOYDIR)/bl31"
	@echo "  BL32_DEPLOYDIR     = \$(DEPLOYDIR)/bl32"
	@echo "  FWCONFIG_DEPLOYDIR = \$(DEPLOYDIR)/fwconfig"
	@echo "  FWDDR_DEPLOYDIR    = \$(DEPLOYDIR)/ddr"
	@echo
	@echo "FIP configuration:"
	@echo "  Do not forget to set FIP deploydir folders (such as FIP_DEPLOYDIR_ROOT) to provide path to needed binaries"
	@echo
	@echo "METADATA configuration:"
	@echo "  TF_A_METADATA_TOOL_ARGS = \$(TF_A_METADATA_TOOL_ARGS)"
	@echo
	@echo "Available targets:"
	@echo "  all      : build TF-A binaries for defined config(s)"
	@echo "  fip      : build FIP binaries"
	@echo "  metadata : build the TF-A metadata binary"
	@echo "  stm32    : build TF-A stm32 binaries"
	@echo "  clean    : clean build directories from generated files"
	@echo

all: \$(DEPS)

host_tools:
	@\$(MAKE) --no-print-directory -C \$(SRC_PATH)/tools/stm32image

# Set TF-A make rules
\$(foreach config, \$(TF_A_CONFIG), \\
	\$(foreach soc, ${STM32MP_SOC_NAME}, \\
		\$(eval \$(call tf-rules,\$(config),\$(soc),host_tools))))

# Set TF-A deploy rules
\$(foreach config, \$(TF_A_CONFIG), \\
	\$(foreach soc, ${STM32MP_SOC_NAME}, \\
		\$(eval \$(call deploy-rules,\$(config),\$(soc),\$(if \$(TF_A_BINARY),\$(TF_A_BINARY),\$(TF_A_BINARY_\$(config)))))))

fip: \$(deploy-targets)
	for config in \$(TF_A_CONFIG) ; do \\
		for fipconfig in \$(FIP_CONFIG) ; do \\
			if [ "\$\$config" = "\$\$fipconfig" ]; then \\
				FIP_DEPLOYDIR_TFA=\$(BL32_DEPLOYDIR) FIP_DEPLOYDIR_BL31=\$(BL31_DEPLOYDIR) FIP_DEPLOYDIR_FWCONF=\$(FWCONFIG_DEPLOYDIR) FIP_DEPLOYDIR_FWDDR=\$(FWDDR_DEPLOYDIR) FIP_CONFIG="\$\$fipconfig" FIP_BL31_ENABLE="\$(TF_A_ENABLE_BL31)" \$(if \$(TF_A_DEVICETREE),FIP_DEVICETREE="\$(TF_A_DEVICETREE)") \$(FIPTOOLDIR)/fiptool-stm32mp || exit 1; \\
			fi ; \\
		done ; \\
	done

stm32: \$(deploy-targets)

metadata:
	@mkdir -p \$(DEPLOYDIR)
	${TF_A_METADATA_TOOL_NAME} ${TF_A_METADATA_TOOL_ARGS} \$(DEPLOYDIR)/${TF_A_METADATA_BINARY}

clean:
	@for config in \$(TF_A_CONFIG) ; do \\
		for soc in ${STM32MP_SOC_NAME}; do \\
			echo "Removing \$(BLD_PATH)/\$\$config-\$\$soc ..." ; \\
			rm -rf \$(BLD_PATH)/\$\$config-\$\$soc* ; \\
		done ; \\
	done
	@echo "Removing \$(DEPLOYDIR) ..."
	@rm -rf \$(DEPLOYDIR)
	@echo
EOF
}
do_ar_original[prefuncs] += "archiver_create_makefile_for_sdk"
