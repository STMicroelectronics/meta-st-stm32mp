#
# Archiver Configuration
#
SRC_URI:append = " file://README.HOW_TO.txt "

inherit archiver
ARCHIVER_MODE[src] = "original"
COPYLEFT_LICENSE_INCLUDE:append = " BSD-3* "

inherit archiver_stm32mp_clean

# for metadata generation
METADATA_TOOL_NAME ??= "mkfwumdata"

archiver_create_makefile_for_sdk() {
    mkdir -p ${ARCHIVER_OUTDIR}
    cat << EOF > ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
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
TF_A_DEVICETREE_SUFFIX ?=
TF_A_MAKE_TARGET ?=
TF_A_EXTRA_OPTFLAGS ?=

EXTDT_DIR_TF_A ?= ${EXTDT_DIR_TF_A}
EXTDT_DIR_TF_A_SERIAL ?= ${EXTDT_DIR_TF_A_SERIAL}

SIGN_ENABLE ?= 0
ENCRYPT_ENABLE ?= 0

EOF
    unset i
    for config in ${TF_A_ARCHIVER_AVAILABLE_CONFIG}; do
        i=$(expr $i + 1)
        extra_optflags=$(echo "${TF_A_EXTRA_OPTFLAGS_AVAILABLE}" | cut -d',' -f${i})
        if $(echo $config | grep -q "programmer") ; then
            extra_optflag=$(echo ${extra_optflags} | sed 's|TFA_EXTERNAL_DT='"${STAGING_EXTDT_DIR}/${EXTDT_DIR_TF_A_SERIAL}"'|TFA_EXTERNAL_DT=\$(EXTDT_DIR)/\$(EXTDT_DIR_TF_A_SERIAL)|' )
        else
            extra_optflag=$(echo ${extra_optflags} | sed 's|TFA_EXTERNAL_DT='"${STAGING_EXTDT_DIR}/${EXTDT_DIR_TF_A}"'|TFA_EXTERNAL_DT=\$(EXTDT_DIR)/\$(EXTDT_DIR_TF_A)|' )
        fi
        cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
# Init default config settings
TF_A_DEVICETREE_INTERNAL_$config ?= $(echo ${TF_A_DEVICETREE__AVAILABLE_INTERNAL} | cut -d',' -f${i})
TF_A_DEVICETREE_EXTERNAL_$config ?= $(echo ${TF_A_DEVICETREE__AVAILABLE_EXTERNAL} | cut -d',' -f${i})
TF_A_DEVICETREE_$config = \$(TF_A_DEVICETREE_INTERNAL_${config}) \$(if \$(EXTDT_DIR),\$(TF_A_DEVICETREE_EXTERNAL_${config}))
TF_A_DEVICETREE_SUFFIX_$config ?= $(echo ${TF_A_DT_SUFFIX_AVAILABLE} | cut -d',' -f${i})
TF_A_EXTRA_OPTFLAGS_$config ?= ${extra_optflag}
TF_A_BINARY_$config ?= $(echo ${TF_A_BINARIES_AVAILABLE} | cut -d',' -f${i})
TF_A_MAKE_TARGET_$config ?= $(echo ${TF_A_MAKE_TARGET_AVAILABLE} | cut -d',' -f${i})
EOF
    done
    extra_oemake_for_cross_compile=$(echo ${EXTRA_OEMAKE} | sed 's|CROSS_COMPILE='"${STAGING_DATADIR_NATIVE}/${ST_CROSS_COMPILE_BIN}/"'|CROSS_COMPILE=|' )
    extra_oemake_for_cross_compile=$(echo ${extra_oemake_for_cross_compile} | sed 's|DEBUG=[01]*||' )
    extra_oemake_for_cross_compile=$(echo ${extra_oemake_for_cross_compile} | sed 's|LOG_LEVEL=[0-9]*||' )
    extra_sign_oemake=${@bb.utils.contains('MACHINEOVERRIDES', 'stm32mp2common', 'BRANCH_PROTECTION=0', '', d)}
    extra_encrypt_oemake=${@bb.utils.contains('FIP_BL31_ENABLE', '1', 'ENCRYPT_BL31=1', '', d)}
    extra_debug=${@bb.utils.contains('ST_TF_A_DEBUG_TRACE', '1', 'DEBUG=${ST_TF_A_DEBUG}', '', d)}
    extra_log_level=${@bb.utils.contains('ST_TF_A_DEBUG_TRACE', '1', '${ST_TF_A_LOG_LEVEL_DEBUG}', '${ST_TF_A_LOG_LEVEL_RELEASE}', d)}
    cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}

# Force to Add SUFFIX to devicetree
# if you call Makefile.sdk with EXTDT_SUFFIX=1, you request to explecitly add storage suffix to devicetree
# if TF_A_DEVICETREE is not defined on command line of Makefile.sdk, we must force EXTDT_SUFFIX to 1
EXTDT_SUFFIX ?= 0

# Manage specific devicetree file type suffix
ifndef EXTDT_SUFFIX
   ifndef TF_A_DEVICETREE
        EXTDT_SUFFIX_CONFIG = 1
    else
        EXTDT_SUFFIX_CONFIG =
    endif
else ifeq (\$(EXTDT_SUFFIX),1)
    EXTDT_SUFFIX_CONFIG = 1
else
    ifndef TF_A_DEVICETREE
        EXTDT_SUFFIX_CONFIG = 1
    else
        EXTDT_SUFFIX_CONFIG =
    endif
endif

# Append toolchain path for SDK
PATH := \$(OECORE_NATIVE_SYSROOT)/usr/share/${ST_CROSS_COMPILE_BIN}:\$(PATH)

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
EXTRA_OEMAKE ?= $extra_oemake_for_cross_compile

SIGN_OEMAKE =
ifeq (\$(SIGN_ENABLE), 1)
   SIGN_OEMAKE ?= TRUSTED_BOARD_BOOT=1 MBEDTLS_DIR=\$(SRC_PATH)/mbedtls $extra_sign_oemake
endif
ENCRYPT_OEMAKE =
ifeq (\$(ENCRYPT_ENABLE), 1)
   ENCRYPT_OEMAKE ?= DECRYPTION_SUPPORT=aes_gcm ENCRYPT_BL32=1 $extra_encrypt_oemake
endif
ifdef TF_A_DEBUG
   ifeq (\$(TF_A_DEBUG), 1)
     DEBUG_OEMAKE ?= DEBUG=1
   endif
else
     DEBUG_OEMAKE ?= $extra_debug
endif
ifdef TF_A_LOG_LEVEL
     LOG_OEMAKE ?= LOG_LEVEL=\$(TF_A_LOG_LEVEL)
else
     LOG_OEMAKE ?= LOG_LEVEL=$extra_log_level
endif

TF_A_FIRMWARE_UPDATE ?= 1
ifdef TF_A_FIRMWARE_UPDATE
   ifeq (\$(TF_A_FIRMWARE_UPDATE), 1)
     FWU_OEMAKE ?= PSA_FWU_SUPPORT=1
   else
     FWU_OEMAKE ?= PSA_FWU_SUPPORT=0
   endif
else
     FWU_OEMAKE ?=
endif
TF_A_FIRMWARE_UPDATE_AB ?= 0
ifdef TF_A_FIRMWARE_UPDATE_AB
   ifeq (\$(TF_A_FIRMWARE_UPDATE_AB), 1)
     FWU_AB_OEMAKE ?= STM32MP_PSA_FWU_AB_SUPPORT=1 PSA_FWU_SUPPORT=1
   else
     FWU_AB_OEMAKE ?= STM32MP_PSA_FWU_AB_SUPPORT=0
   endif
else
    FWU_AB_OEMAKE ?=
endif

EXTRA_OEMAKE_ADDONS = \$(SIGN_OEMAKE) \$(ENCRYPT_OEMAKE) \$(DEBUG_OEMAKE) \$(LOG_OEMAKE) \$(FWU_OEMAKE) \$(FWU_AB_OEMAKE)

METADATA_NAME ?= metadata_custom.bin

# Display TF-A config details
define tf-configs
	echo "  \$(1)" ; \\
	echo "    with device tree : \$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_\$(1)))" ; \\
	echo "    with suffix      : \$(if \$(EXTDT_SUFFIX_CONFIG),\$(if \$(TF_A_DEVICETREE_SUFFIX),\$(TF_A_DEVICETREE_SUFFIX),\$(TF_A_DEVICETREE_SUFFIX_\$(1))))" ; \\
	echo "    extra optionflags: \$(if \$(TF_A_EXTRA_OPTFLAGS),\$(TF_A_EXTRA_OPTFLAGS),\$(TF_A_EXTRA_OPTFLAGS_\$(1)))" ; \\
	echo "    binary basename  : \$(if \$(TF_A_BINARY),\$(TF_A_BINARY),\$(TF_A_BINARY_\$(1)))" ; \\
	echo "    tf-a build target: \$(if \$(TF_A_MAKE_TARGET),\$(TF_A_MAKE_TARGET),\$(TF_A_MAKE_TARGET_\$(1)))" ;
endef

# Configure TF-A check configuration rules (check-config <CONFIG> <DT_SUFFIX>)
#   chk-conf-<CONFIG>
define check-config
chk-conf-\$(1):
	@\$(foreach dt, \$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_\$(1))), \\
		\$(foreach soc, ${STM32MP_SOC_NAME}, \\
			\$(if \$(findstring \$(soc),\$(dt)), \\
				mkdir -p "\$(BLD_PATH)/\$(1)-\$(dt)\$(2)"; \\
				touch "\$(BLD_PATH)/\$(1)-\$(dt)\$(2)/\$(dt)\$(2)-chk-conf.txt" ; \\
			,) \\
		) \\
		if ! [ -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(2)/\$(dt)\$(2)-chk-conf.txt" ]; then \\
			echo "[CONFIG ERROR] Wrong device tree file name: \$(dt)\$(2)"; \\
			echo "[CONFIG ERROR] It should contain the SOC pattern name (any of ${STM32MP_SOC_NAME})"; \\
			exit 1; \\
		fi; \\
		rm -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(2)/\$(dt)\$(2)-chk-conf.txt" ; \\
	)
endef

# Configure TF-A make rules (tf-rules <CONFIG> <SOC> <DT_SUFFIX> <DEPS>)
#   tf-<CONFIG>-<SOC>-<dt_suffix>: <DEPS>
define tf-rules
tf-\$(1)-\$(2): \$(4)
	@\$(foreach dt, \$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_\$(1))), \\
		\$(if \$(findstring \$(2),\$(dt)), \\
			mkdir -p  "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)"; \\
			touch "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-sysram.txt" ; \\
			if [ "\$(2)" = "stm32mp15" ]; then \\
				(echo \$(1) | grep "optee-" && echo "STM32MP1_OPTEE_IN_SYSRAM=1" > "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-sysram.txt"); \\
			fi; \\
			\$(MAKE) \$(EXTRA_OEMAKE) \$(EXTRA_OEMAKE_ADDONS) -C \$(SRC_PATH) \\
				BUILD_PLAT="\$(BLD_PATH)/\$(1)-\$(dt)\$(3)" \\
				DTB_FILE_NAME="\$(dt)\$(3).dtb" \\
				\$(if \$(TF_A_EXTRA_OPTFLAGS),\$(TF_A_EXTRA_OPTFLAGS),\$(TF_A_EXTRA_OPTFLAGS_\$(1))) \\
				\`cat "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-sysram.txt"\` \\
				dtbs \\
				\$(shell echo \$(2) | tr a-z A-Z)=1 || { echo "[DTB BUILD ERROR] config: \$(1) devicetree: \$(dt)\$(3)"; exit 1; }; \\
			touch "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-pmic1l_property.txt" ; \\
			(fdtdump "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/fdts/\$(dt)\$(3)-bl2.dtb" 2>/dev/null | grep -c "st,stpmic1l" || /bin/true) > "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-pmic1l_node.txt"; \\
			if [ "\`cat "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-pmic1l_node.txt"\`" = "1" ]; then \\
				(echo "STM32MP_STPMIC1L=1" > "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-pmic1l_property.txt"); \\
			fi; \\
			touch "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-ddr_property.txt" ; \\
			if [ "\$(TF_A_ENABLE_FWDDR)" = "1" ]; then \\
				if [ -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/fdts/\$(dt)\$(3)-bl2.dtb" ]; then \\
					fdtget -l "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/fdts/\$(dt)\$(3)-bl2.dtb" /soc | grep ddr | head -n 1 > "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-ddr_dtb_node.txt" ; \\
					(fdtget "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/fdts/\$(dt)\$(3)-bl2.dtb" /soc/\`cat \$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-ddr_dtb_node.txt\` st,mem-name) > "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-ddr_property.txt" ; \\
					sed -i -e 's|^\(.*DDR[0-9]\).*| STM32MP_\1_TYPE=1 |' "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-ddr_property.txt" ; \\
					sed -e 's|.*_\(.*DDR[0-9]\)_.*|\1|' "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-ddr_property.txt" | tr A-Z a-z > "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-ddr_target.txt" ; \\
					if [ -f "\$(FWDDR_DIR)/${FWDDR_SRC_SUBDIR}/\`cat "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-ddr_target.txt"\`_pmu_train.bin" ]; then \\
						cp -f "\$(FWDDR_DIR)/${FWDDR_SRC_SUBDIR}/\`cat "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-ddr_target.txt"\`_pmu_train.bin" "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/${FWDDR_NAME}-\$(dt)\$(3).${FWDDR_SUFFIX}" ; \\
					fi ; \\
				fi ; \\
EOF
	if ${@bb.utils.contains('MACHINE_FEATURES','m33td','true','false',d)}; then
		cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
			else \\
				(echo \$(1) |  grep -c programmer > "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(1)-programmer.txt"); \\
				if test \`cat "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(1)-programmer.txt"\` -eq 1 ; then \\
					if [ -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/fdts/\$(dt)\$(3)-bl2.dtb" ]; then \\
						fdtget -l "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/fdts/\$(dt)\$(3)-bl2.dtb" /soc | grep ddr | head -n 1 > "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-ddr_dtb_node.txt" ; \\
						(fdtget "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/fdts/\$(dt)\$(3)-bl2.dtb" /soc/\`cat \$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-ddr_dtb_node.txt\` st,mem-name) > "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-ddr_property.txt" ; \\
						sed -i -e 's|^\(.*DDR[0-9]\).*| STM32MP_\1_TYPE=1 |' "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-ddr_property.txt" ; \\
						sed -e 's|.*_\(.*DDR[0-9]\)_.*|\1|' "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-ddr_property.txt" | tr A-Z a-z > "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-ddr_target.txt" ; \\
						if [ -f "\$(FWDDR_DIR)/${FWDDR_SRC_SUBDIR}/\`cat "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-ddr_target.txt"\`_pmu_train.bin" ]; then \\
							cp -f "\$(FWDDR_DIR)/${FWDDR_SRC_SUBDIR}/\`cat "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-ddr_target.txt"\`_pmu_train.bin" "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/${FWDDR_NAME}-\$(dt)\$(3).${FWDDR_SUFFIX}" ; \\
						fi ; \\
					fi ; \\
				fi; \\
EOF
    fi
    cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
			fi ; \\
			unset AS LDFLAGS CFLAGS CPPFLAGS CC CPP AR LD NM; \\
			\$(MAKE) \$(EXTRA_OEMAKE) \$(EXTRA_OEMAKE_ADDONS) -C \$(SRC_PATH) \\
				BUILD_PLAT="\$(BLD_PATH)/\$(1)-\$(dt)\$(3)" \\
				DTB_FILE_NAME="\$(dt)\$(3).dtb" \\
				\`cat "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-sysram.txt"\` \\
				\`cat "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-ddr_property.txt"\` \\
				\`cat "\$(BLD_PATH)/\$(1)-\$(dt)\$(3)/\$(dt)\$(3)-pmic1l_property.txt"\` \\
				\$(if \$(TF_A_EXTRA_OPTFLAGS),\$(TF_A_EXTRA_OPTFLAGS),\$(TF_A_EXTRA_OPTFLAGS_\$(1))) \\
				\$(if \$(TF_A_MAKE_TARGET),\$(TF_A_MAKE_TARGET),\$(TF_A_MAKE_TARGET_\$(1))) \\
				\$(shell echo \$(2) | tr a-z A-Z)=1 || { echo "[TF-A BUILD ERROR] config: \$(1) devicetree: \$(dt)\$(3)"; exit 1; }; \\
		,) \\
	)
endef

# Configure TF-A deploy rules (deploy-rules <CONFIG> <SOC> <BINARY> <DT_SUFFIX>)
#   deploy-<CONFIG>-<SOC>: tf-<CONFIG>-<SOC>
define deploy-rules
deploy-\$(1)-\$(2): tf-\$(1)-\$(2)
	@mkdir -p \$(DEPLOYDIR)
	@mkdir -p \$(DEPLOYDIR)/debug
	@\$(foreach dt, \$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_\$(1))), \\
		if [ -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/${FWDDR_NAME}-\$(dt)\$(4).${FWDDR_SUFFIX}" ]; then \\
			mkdir -p "\$(FWDDR_DEPLOYDIR)" ; \\
			cp -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/${FWDDR_NAME}-\$(dt)\$(4).${FWDDR_SUFFIX}" \$(FWDDR_DEPLOYDIR)/ ; \\
		fi ; \\
	)
	@\$(foreach dt, \$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_\$(1))), \\
		if [ -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/\$(3)-\$(dt)\$(4).${TF_A_SUFFIX}" ]; then \\
			cp -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/\$(3)-\$(dt)\$(4).${TF_A_SUFFIX}" "\$(DEPLOYDIR)/\$(3)-\$(dt)-\$(1).${TF_A_SUFFIX}" ; \\
			if [ "\$(TF_A_ENABLE_DEBUG_WRAPPER)" = "1" ] ; then \\
				stm32wrapper4dbg -s "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/\$(3)-\$(dt)\$(4).${TF_A_SUFFIX}" -d "\$(DEPLOYDIR)/debug/debug-\$(3)-\$(dt)-\$(1).${TF_A_SUFFIX}" ; \\
			fi ; \\
		fi ; \\
	)
	@\$(foreach dt, \$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_\$(1))), \\
		if [ -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/${BL31_BASENAME}.${BL31_SUFFIX}" ] ; then \\
			mkdir -p "\$(BL31_DEPLOYDIR)" ; \\
			cp -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/${BL31_BASENAME}.${BL31_SUFFIX}" "\$(BL31_DEPLOYDIR)/\$(3)-${BL31_BASENAME}-\$(dt)\$(4)-\$(1).${BL31_SUFFIX}" ; \\
			if [ -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/fdts/\$(dt)\$(4)-${BL31_BASENAME}.${DT_SUFFIX}" ]; then \\
				cp -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/fdts/\$(dt)\$(4)-${BL31_BASENAME}.${DT_SUFFIX}" "\$(BL31_DEPLOYDIR)/\$(dt)\$(4)-${BL31_BASENAME}-\$(1).${DT_SUFFIX}" ; \\
			fi ; \\
			if [ -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/${BL31_ELF}" ]; then \\
				mkdir -p "\$(BL31_DEPLOYDIR)/debug" ; \\
				cp -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/${BL31_ELF}" "\$(BL31_DEPLOYDIR)/debug/\$(3)-${BL31_BASENAME}-\$(dt)\$(4)-\$(1).${TF_A_ELF_SUFFIX}" ; \\
			fi ; \\
		fi ; \\
	)
	@\$(foreach dt, \$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_\$(1))), \\
		if [ -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/${BL32_BASENAME}.${BL32_SUFFIX}" ] ; then \\
			mkdir -p "\$(BL32_DEPLOYDIR)" ; \\
			cp -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/${BL32_BASENAME}.${BL32_SUFFIX}" "\$(BL32_DEPLOYDIR)/\$(3)-${BL32_BASENAME}-\$(2)-\$(1).${BL32_SUFFIX}" ; \\
			if [ -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/fdts/\$(dt)\$(4)-${BL32_BASENAME}.${DT_SUFFIX}" ]; then \\
				cp -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/fdts/\$(dt)\$(4)-${BL32_BASENAME}.${DT_SUFFIX}" "\$(BL32_DEPLOYDIR)/\$(dt)\$(4)-${BL32_BASENAME}-\$(1).${DT_SUFFIX}" ; \\
			fi ; \\
			if [ -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/${BL32_ELF}" ]; then \\
				mkdir -p "\$(BL32_DEPLOYDIR)/debug" ; \\
				cp -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/${BL32_ELF}" "\$(BL32_DEPLOYDIR)/debug/\$(3)-${BL32_BASENAME}-\$(2)-\$(1).${TF_A_ELF_SUFFIX}" ; \\
			fi ; \\
		fi ; \\
	)
	@\$(foreach dt, \$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_\$(1))), \\
		if [ -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/fdts/\$(dt)\$(4)-${FWCONFIG_NAME}.${DT_SUFFIX}" ]; then \\
			mkdir -p "\$(FWCONFIG_DEPLOYDIR)" ; \\
			cp -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/fdts/\$(dt)\$(4)-${FWCONFIG_NAME}.${DT_SUFFIX}" "\$(FWCONFIG_DEPLOYDIR)/\$(dt)\$(4)-${FWCONFIG_NAME}-\$(1).${DT_SUFFIX}" ; \\
		fi ; \\
	)
	@\$(foreach dt, \$(if \$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE),\$(TF_A_DEVICETREE_\$(1))), \\
		if [ -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/${BL2_ELF}" ] ; then \\
			cp -f "\$(BLD_PATH)/\$(1)-\$(dt)\$(4)/${BL2_ELF}" "\$(DEPLOYDIR)/debug/\$(3)-${BL2_BASENAME}-\$(2)-\$(1).${TF_A_ELF_SUFFIX}" ; \\
		fi ; \\
	)
endef

# Configure overall check config rules lits
tf-check-config := \$(foreach config, \$(TF_A_CONFIG), chk-conf-\$(config))

# Configure overall deploy rules list
deploy-targets := \$(foreach config, \$(TF_A_CONFIG), \$(foreach soc, ${STM32MP_SOC_NAME}, deploy-\$(config)-\$(soc)))

# Set dependencies list for building all
DEPS = \$(deploy-targets)
DEPS += metadata

help: \$(tf-check-config)
	@echo ""
	@echo "TF-A configuration:"
	@echo "  TF_A_CONFIG = \$(TF_A_CONFIG)"
	@echo "Config details:"
	@\$(foreach config, \$(TF_A_CONFIG), \$(call tf-configs,\$(config)))
	@echo ""
	@echo "Note that each TF-A configuration settings can be updated through overall or specific config var:"
	@echo "  TF_A_DEVICETREE"
	@echo "  TF_A_DEVICETREE_SUFFIX"
	@echo "  TF_A_EXTRA_OPTFLAGS"
	@echo "  TF_A_BINARY"
	@echo "  TF_A_MAKE_TARGET"
	@echo ""
	@echo "TF-A devicetree suffix configuration:"
	@echo "  EXTDT_SUFFIX = \$(EXTDT_SUFFIX)"
	@echo ""
	@echo "TF-A features configuration:"
	@echo "  TF_A_ENABLE_BL31 = \$(TF_A_ENABLE_BL31) ('1' to generate bl31 binary)"
	@echo "  TF_A_ENABLE_DEBUG_WRAPPER = \$(TF_A_ENABLE_DEBUG_WRAPPER) ('1' to generate tf-a for debugging)"
	@echo "  TF_A_ENABLE_FWDDR = \$(TF_A_ENABLE_FWDDR) ('1' to generate tf-a ddr firmware for fip ddr)"
	@echo ""
	@echo "TF-A folder configuration:"
	@echo "  SRC_PATH  = \$(SRC_PATH)"
	@echo "  BLD_PATH  = \$(BLD_PATH)"
	@echo "  DEPLOYDIR = \$(DEPLOYDIR)"
	@echo "  BL31_DEPLOYDIR     = \$(DEPLOYDIR)/bl31"
	@echo "  BL32_DEPLOYDIR     = \$(DEPLOYDIR)/bl32"
	@echo "  FWCONFIG_DEPLOYDIR = \$(DEPLOYDIR)/fwconfig"
	@echo "  FWDDR_DEPLOYDIR    = \$(DEPLOYDIR)/ddr"
	@echo ""
	@echo "FIP configuration:"
	@echo "  Do not forget to set FIP deploydir folders (such as FIP_DEPLOYDIR_ROOT) to provide path to needed binaries"
	@echo ""
	@echo "METADATA configuration:"
	@echo "  METADATA_TOOL_ARGS = \$(METADATA_TOOL_ARGS)"
	@echo "  METADATA_NAME      = \$(METADATA_NAME)"
	@echo ""
	@echo "DEBUG options:"
	@echo "  opt= \$(DEBUG_OEMAKE) \$(LOG_OEMAKE)"
	@echo ""
	@echo "Functionality options:"
	@echo "  opt= \$(FWU_OEMAKE) \$(FWU_AB_OEMAKE)"
	@echo ""
	@echo "Available targets:"
	@echo "  all      : build TF-A binaries for defined config(s)"
	@echo "  fip      : build FIP binaries"
	@echo "  metadata : build the TF-A metadata binary"
	@echo "  stm32    : build TF-A stm32 binaries"
	@echo "  clean    : clean build directories from generated files"
	@echo ""

all: \$(DEPS)

clean:
	@for config in \$(TF_A_CONFIG) ; do \\
		for soc in ${STM32MP_SOC_NAME}; do \\
			echo "Removing \$(BLD_PATH)/\$\$config-\$\$soc ..." ; \\
			rm -rf \$(BLD_PATH)/\$\$config-\$\$soc* ; \\
		done ; \\
	done
	@echo "Removing \$(DEPLOYDIR) ..."
	@rm -rf \$(DEPLOYDIR)
	@echo ""

fip: \$(deploy-targets)
	for config in \$(TF_A_CONFIG) ; do \\
		for fipconfig in \$(FIP_CONFIG) ; do \\
			if [ "\$\$config" = "\$\$fipconfig" ]; then \\
				FIP_DEPLOYDIR_TFA=\$(BL32_DEPLOYDIR) \\
				FIP_DEPLOYDIR_BL31=\$(BL31_DEPLOYDIR) \\
				FIP_DEPLOYDIR_FWCONF=\$(FWCONFIG_DEPLOYDIR) \\
				FIP_DEPLOYDIR_FWDDR=\$(FWDDR_DEPLOYDIR) \\
				FIP_CONFIG="\$\$fipconfig" \\
				FIP_BL31_ENABLE="\$(TF_A_ENABLE_BL31)" \\
				\$(if \$(TF_A_DEVICETREE),FIP_DEVICETREE="\$(TF_A_DEVICETREE)") \\
				\$(if \$(EXTDT_SUFFIX_CONFIG),\$(if \$(TF_A_DEVICETREE_SUFFIX),FIP_DEVICETREE_SUFFIX="\$(TF_A_DEVICETREE_SUFFIX)")) \\
				\$(FIPTOOLDIR)/fiptool-stm32mp.${MACHINE} || { echo "[FIP BUILD ERROR] FIP_CONFIG=\$\$fipconfig \$(if \$(TF_A_DEVICETREE),FIP_DEVICETREE=\$(TF_A_DEVICETREE)) \$(if \$(EXTDT_SUFFIX_CONFIG),\$(if \$(TF_A_DEVICETREE_SUFFIX),FIP_DEVICETREE_SUFFIX=\$(TF_A_DEVICETREE_SUFFIX)))"; exit 1; }; \\
			fi ; \\
		done ; \\
	done
	@echo "***** FIP binary available under: \$(if \$(FIP_DEPLOYDIR_FIP),\$(FIP_DEPLOYDIR_FIP),\$(FIP_DEPLOYDIR_ROOT)/) *****"

host_tools:
	@\$(MAKE) --no-print-directory -C \$(SRC_PATH)/tools/stm32image BUILD_PLAT=\$(SRC_PATH)/build

metadata:
	@mkdir -p \$(DEPLOYDIR)/../metadata
	if [ -n "\$(METADATA_TOOL_ARGS)" ]; then \\
		${METADATA_TOOL_NAME} \${METADATA_TOOL_ARGS} \$(DEPLOYDIR)/../metadata/\$(METADATA_NAME); \\
	else \\
		${METADATA_TOOL_NAME} ${METADATA_TOOL_ARGS_DEFAULT} \$(DEPLOYDIR)/../metadata/${METADATA_BINARY_DEFAULT}; \\
		${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_EMMC} \$(DEPLOYDIR)/../metadata/${METADATA_AB_BINARY_EMMC}; \\
		${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_NOR_EMMC} \$(DEPLOYDIR)/../metadata/${METADATA_AB_BINARY_NOR_EMMC}; \\
		${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_NAND_EMMC} \$(DEPLOYDIR)/../metadata/${METADATA_AB_BINARY_NAND_EMMC}; \\
		${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_SPINAND_EMMC} \$(DEPLOYDIR)/../metadata/${METADATA_AB_BINARY_SPINAND_EMMC}; \\
		${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_SDCARD} \$(DEPLOYDIR)/../metadata/${METADATA_AB_BINARY_SDCARD}; \\
		${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_NOR_SDCARD} \$(DEPLOYDIR)/../metadata/${METADATA_AB_BINARY_NOR_SDCARD}; \\
		${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_NAND_SDCARD} \$(DEPLOYDIR)/../metadata/${METADATA_AB_BINARY_NAND_SDCARD}; \\
		${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_SPINAND_SDCARD} \$(DEPLOYDIR)/../metadata/${METADATA_AB_BINARY_SPINAND_SDCARD}; \\
		${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_NOR} \$(DEPLOYDIR)/../metadata/${METADATA_AB_BINARY_NOR}; \\
		${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_NAND} \$(DEPLOYDIR)/../metadata/${METADATA_AB_BINARY_NAND}; \\
		${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_SPINAND} \$(DEPLOYDIR)/../metadata/${METADATA_AB_BINARY_SPINAND}; \\
	fi
	@echo "***** metadata generation done under: \$(DEPLOYDIR)/metadata *****"

stm32: \$(deploy-targets)
	@echo "***** TF-A binary available under: \$(DEPLOYDIR)/ *****"

# Set TF-A check config rules
\$(foreach config, \$(TF_A_CONFIG), \\
	\$(eval \$(call check-config,\$(config),\$(if \$(EXTDT_SUFFIX_CONFIG),\$(if \$(TF_A_DEVICETREE_SUFFIX),\$(TF_A_DEVICETREE_SUFFIX),\$(TF_A_DEVICETREE_SUFFIX_\$(config)))))) \\
)

# Set TF-A make rules
\$(foreach config, \$(TF_A_CONFIG), \\
	\$(foreach soc, ${STM32MP_SOC_NAME}, \\
		\$(eval \$(call tf-rules,\$(config),\$(soc),\$(if \$(EXTDT_SUFFIX_CONFIG),\$(if \$(TF_A_DEVICETREE_SUFFIX),\$(TF_A_DEVICETREE_SUFFIX),\$(TF_A_DEVICETREE_SUFFIX_\$(config)))),chk-conf-\$(config))) \\
	) \\
)

# Set TF-A deploy rules
\$(foreach config, \$(TF_A_CONFIG), \\
	\$(foreach soc, ${STM32MP_SOC_NAME}, \\
		\$(eval \$(call deploy-rules,\$(config),\$(soc),\$(if \$(TF_A_BINARY),\$(TF_A_BINARY),\$(TF_A_BINARY_\$(config))),\$(if \$(EXTDT_SUFFIX_CONFIG),\$(if \$(TF_A_DEVICETREE_SUFFIX),\$(TF_A_DEVICETREE_SUFFIX),\$(TF_A_DEVICETREE_SUFFIX_\$(config)))))) \\
	) \\
)
EOF
}
do_ar_original[prefuncs] += "archiver_create_makefile_for_sdk"
