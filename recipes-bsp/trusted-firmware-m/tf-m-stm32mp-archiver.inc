#
# Archiver Configuration
#
SRC_URI:append = " file://README.HOW_TO.txt "

TF_M_ARCHIVER ?= "${@bb.utils.contains('MACHINE_FEATURES', 'm33td', 'archiver', '', d)}"
inherit_defer ${TF_M_ARCHIVER}
ARCHIVER_MODE[src] = "configured"
do_ar_configured[network] = "${@bb.utils.contains('TFM_EXTERNAL_SOURCES', '1', '1', '0', d)}"
ARCHIVER_MODE[diff] = "1"
COPYLEFT_LICENSE_INCLUDE:append = " BSD-3* "

inherit archiver_stm32mp_clean

do_archiver_git_uri:append() {
    if [ -e "${ARCHIVER_OUTDIR}/${ARCHIVER_README}.${MACHINE}" ]; then
        sed -i \
            -e "s|##TF_M_PATH_MBEDCRYPTO##|${TF_M_PATH_MBEDCRYPTO}|g" \
            -e "s|##TF_M_PATH_MCUBOOT##|${TF_M_PATH_MCUBOOT}|g" \
            -e "s|##TF_M_PATH_QCBOR##|${TF_M_PATH_QCBOR}|g" \
            -e "s|##TF_M_PATH_CMSIS##|${TF_M_PATH_CMSIS}|g" \
            -e "s|##TF_M_PATH_DDR_PHY_BIN_SRC##|${TF_M_PATH_DDR_PHY_BIN_SRC}|g" \
            -e "s|##TF_M_PATH_SCP_FW##|${TF_M_PATH_SCP_FW}|g" \
            -e "s|##ARCHIVER_REVISION_MBEDCRYPTO##|${ARCHIVER_REVISION_MBEDCRYPTO}|g" \
            -e "s|##ARCHIVER_REVISION_MCUBOOT##|${ARCHIVER_REVISION_MCUBOOT}|g" \
            -e "s|##ARCHIVER_REVISION_QCBOR##|${ARCHIVER_REVISION_QCBOR}|g" \
            -e "s|##ARCHIVER_REVISION_CMSIS##|${ARCHIVER_REVISION_CMSIS}|g" \
            -e "s|##ARCHIVER_REVISION_DDR_PHY_BIN_SRC##|${ARCHIVER_REVISION_DDR_PHY_BIN_SRC}|g" \
            -e "s|##ARCHIVER_REVISION_SCP_FW##|${ARCHIVER_REVISION_SCP_FW}|g" \
            "${ARCHIVER_OUTDIR}/${ARCHIVER_README}.${MACHINE}"
    fi
}

do_archiver_create_makefile_for_sdk() {
    mkdir -p ${ARCHIVER_OUTDIR}
    # Get gcc version for SDK
    gcc_none_eabi_major_version="$(${STAGING_DATADIR_NATIVE}/gcc-arm-none-eabi/bin/arm-none-eabi-gcc -dumpversion | cut -d'.' -f1)"
    gcc_none_eabi_minor_version="$(${STAGING_DATADIR_NATIVE}/gcc-arm-none-eabi/bin/arm-none-eabi-gcc -dumpversion | cut -d'.' -f2)"
    cat << EOF > ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
# Set default path
SRC_PATH ?= \$(PWD)
BLD_PATH ?= \$(SRC_PATH)/../build
DEPLOYDIR ?= \$(SRC_PATH)/../deploy

TFM_TESTS_DIR ?=

EXTDT_DIR_TF_M ?= ${EXTDT_DIR_TF_M}
EXTDT_DIR_MCU ?= ${EXTDT_DIR_MCU}

# Set default TF-M config
TF_M_CONFIG ?= ${TF_M_CONFIG}

# Default TF-M overall settings to null
TF_M_DEVICETREE ?=
TF_M_PLATFORM ?=
TF_M_PLATFORM_OPTFLAGS ?=
TF_M_EXTRA_OPTFLAGS ?=
TF_M_BOOTDEV ?=
TF_M_BOOT_SUFFIX_M33 ?=
TF_M_BOOT_SUFFIX_A35 ?=
TF_M_DT_BL2_SUFFIX ?=
TF_M_DT_TFM_SUFFIX ?=

# Init default config settings
EOF
    ${@' '.join(['tf_m_platform_optflags_%s="%s";' % (name, (d.getVar('TF_M_PLATFORM_OPTFLAGS_%s' % name) or "")) for name in list(dict.fromkeys( (d.getVar('TF_M_PLATFORM_BASENAME_LIST') or "").split() ))])}
    for pltf in ${@' '.join([name for name in list(dict.fromkeys((d.getVar('TF_M_PLATFORM_BASENAME_LIST') or "").split()))])}; do
        cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
TF_M_PLATFORM_OPTFLAGS_$pltf ?= $(eval echo \"\${tf_m_platform_optflags_$pltf}\")
EOF
    done
    unset i
    for config in ${TF_M_CONFIG}; do
        i=$(expr $i + 1)
        cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
TF_M_DEVICETREE_INTERNAL_$config ?= $(echo ${TF_M_DEVICETREE_INTERNAL} | cut -d',' -f${i})
TF_M_DEVICETREE_EXTERNAL_$config ?= $(echo ${TF_M_DEVICETREE_EXTERNAL} | cut -d',' -f${i})
TF_M_DEVICETREE_$config = \$(TF_M_DEVICETREE_INTERNAL_${config}) \$(if \$(EXTDT_DIR),\$(TF_M_DEVICETREE_EXTERNAL_${config}))
TF_M_EXTRA_OPTFLAGS_$config ?= $(echo ${TF_M_EXTRA_OPTFLAGS} | cut -d',' -f${i})
TF_M_PLATFORM_$config ?= $(echo ${TF_M_PLATFORM} | cut -d',' -f${i})
TF_M_BOOTDEV_$config ?= $(echo ${TF_M_BOOTDEV} | cut -d',' -f${i})
TF_M_BOOT_SUFFIX_M33_$config ?= $(echo ${TF_M_BOOT_SUFFIX_M33} | cut -d',' -f${i})
TF_M_BOOT_SUFFIX_A35_$config ?= $(echo ${TF_M_BOOT_SUFFIX_A35} | cut -d',' -f${i})
TF_M_DT_BL2_SUFFIX_$config ?= $(echo ${TF_M_DT_BL2_SUFFIX} | cut -d',' -f${i})
TF_M_DT_TFM_SUFFIX_$config ?= $(echo ${TF_M_DT_TFM_SUFFIX} | cut -d',' -f${i})
EOF
    done
    cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}

# Init external source use
TF_M_EXTERNAL_SOURCES ?= ${TFM_EXTERNAL_SOURCES}
# Path to extra components
TF_M_PATH_MBEDCRYPTO ?= \$(SRC_PATH)/${TF_M_PATH_MBEDCRYPTO}
TF_M_PATH_MCUBOOT ?= \$(SRC_PATH)/${TF_M_PATH_MCUBOOT}
TF_M_PATH_QCBOR ?= \$(SRC_PATH)/${TF_M_PATH_QCBOR}
TF_M_PATH_CMSIS ?= \$(SRC_PATH)/${TF_M_PATH_CMSIS}
TF_M_PATH_DDR_PHY_BIN_SRC ?= \$(SRC_PATH)/${TF_M_PATH_DDR_PHY_BIN_SRC}
TF_M_PATH_SCP_FW ?= \$(SRC_PATH)/${TF_M_PATH_SCP_FW}

# Define default make options
EXTRA_OECMAKE ?= $(echo "${EXTRA_OECMAKE}" | sed 's#-DCONFIG_TFM_SOURCE_PATH=[^[:space:]]*##;s#-DFETCHCONTENT_FULLY_DISCONNECTED=[^[:space:]]*##;s#-DMBEDCRYPTO_PATH=[^[:space:]]*##;s#-DMCUBOOT_PATH=[^[:space:]]*##;s#-DQCBOR_PATH=[^[:space:]]*##;s#-DCMSIS_PATH=[^[:space:]]*##;s#-DDDR_PHY_BIN_SRC_PATH=[^[:space:]]*##;s#-DSCP_FW_PATH=[^[:space:]]*##;')
# Append source configuration
EXTRA_OECMAKE +=  -DTFM_VERSION='${TF_M_VERSION}'
EXTRA_OECMAKE +=  -DCONFIG_TFM_SOURCE_PATH=\$(SRC_PATH)
EXTRA_OECMAKE += \$(if \$(findstring 1,\$(TF_M_EXTERNAL_SOURCES)), -DFETCHCONTENT_FULLY_DISCONNECTED=ON)
EXTRA_OECMAKE += \$(if \$(findstring 1,\$(TF_M_EXTERNAL_SOURCES)), -DMBEDCRYPTO_PATH=\$(TF_M_PATH_MBEDCRYPTO))
EXTRA_OECMAKE += \$(if \$(findstring 1,\$(TF_M_EXTERNAL_SOURCES)), -DMCUBOOT_PATH=\$(TF_M_PATH_MCUBOOT))
EXTRA_OECMAKE += \$(if \$(findstring 1,\$(TF_M_EXTERNAL_SOURCES)), -DQCBOR_PATH=\$(TF_M_PATH_QCBOR))
EXTRA_OECMAKE += \$(if \$(findstring 1,\$(TF_M_EXTERNAL_SOURCES)), -DCMSIS_PATH=\$(TF_M_PATH_CMSIS))
EXTRA_OECMAKE += \$(if \$(findstring 1,\$(TF_M_EXTERNAL_SOURCES)), -DDDR_PHY_BIN_SRC_PATH=\$(TF_M_PATH_DDR_PHY_BIN_SRC))
EXTRA_OECMAKE += \$(if \$(findstring 1,\$(TF_M_EXTERNAL_SOURCES)), -DSCP_FW_PATH=\$(TF_M_PATH_SCP_FW))

# Append toolchain path for SDK
\$(if \$(OECORE_NATIVE_SYSROOT),,\$(error OECORE_NATIVE_SYSROOT is not defined: you should source SDK environement before build))
PATH := \$(OECORE_NATIVE_SYSROOT)/usr/share/gcc-arm-none-eabi/bin:\$(PATH)

# Make sure about gcc version
GCCVERSION = \$(shell \$(OECORE_NATIVE_SYSROOT)/usr/share/gcc-arm-none-eabi/bin/arm-none-eabi-gcc -dumpversion)
GCCVERSION_MAJOR = \$(shell echo \$(GCCVERSION) | cut -d'.' -f1)
GCCVERSION_MINOR = \$(shell echo \$(GCCVERSION) | cut -d'.' -f2)
ifeq (\$(shell if [ \$(GCCVERSION_MAJOR) -ge ${gcc_none_eabi_major_version} -a \$(GCCVERSION_MINOR) -ge ${gcc_none_eabi_minor_version} ]; then echo "y"; else echo "n"; fi;),y)
\$(info You are using arm-none-eabi-gcc \$(GCCVERSION) version.)
else
\$(warning You are using unexpected \$(GCCVERSION) arm-none-eabi-gcc version (>=${gcc_none_eabi_major_version}.${gcc_none_eabi_minor_version} wanted)! Please check your configuration.)
endif

# Unset CMAKE_TOOLCHAIN_FILE for SDK
CMAKE_TOOLCHAIN_FILE =

# Display TF-M config details (tfm-configs <CONFIG>)
define tfm-configs
	echo "  \$(1)" ; \\
	echo "    with device tree     : \$(if \$(TF_M_DEVICETREE),\$(TF_M_DEVICETREE),\$(TF_M_DEVICETREE_\$(1)))" ; \\
	echo "    extra optionflags    : \$(if \$(TF_M_EXTRA_OPTFLAGS),\$(TF_M_EXTRA_OPTFLAGS),\$(TF_M_EXTRA_OPTFLAGS_\$(1)))" ; \\
	echo "    plateform config     : \$(if \$(TF_M_PLATFORM),\$(TF_M_PLATFORM),\$(TF_M_PLATFORM_\$(1)))" ; \\
	echo "    bootdevice target    : \$(if \$(TF_M_BOOTDEV),\$(TF_M_BOOTDEV),\$(TF_M_BOOTDEV_\$(1)))" ; \\
	echo "    bootdevice m33 suffix: \$(if \$(TF_M_BOOT_SUFFIX_M33),\$(TF_M_BOOT_SUFFIX_M33),\$(TF_M_BOOT_SUFFIX_M33_\$(1)))" ; \\
	echo "    bootdevice a35 suffix: \$(if \$(TF_M_BOOT_SUFFIX_A35),\$(TF_M_BOOT_SUFFIX_A35),\$(TF_M_BOOT_SUFFIX_A35_\$(1)))" ;
	echo "    bl2 devicetree suffix: \$(if \$(TF_M_DT_BL2_SUFFIX),\$(TF_M_DT_BL2_SUFFIX),\$(TF_M_DT_BL2_SUFFIX_\$(1)))" ;
	echo "    tfm devicetree suffix: \$(if \$(TF_M_DT_TFM_SUFFIX),\$(TF_M_DT_TFM_SUFFIX),\$(TF_M_DT_TFM_SUFFIX_\$(1)))" ;
endef

# Display TF-M plateform config details (tfm-plt-configs <PLTF>)
define tfm-plt-configs
	echo "      \$(1): \$(if \$(TF_M_PLATFORM_OPTFLAGS),\$(TF_M_PLATFORM_OPTFLAGS),\$(TF_M_PLATFORM_OPTFLAGS_\$(1)))" ;
endef

# Configure TF-M check configuration rules (check-config <CONFIG>)
#   chk-conf-<CONFIG>
define check-config
chk-conf-\$(1):
	@\$(foreach dt, \$(if \$(TF_M_DEVICETREE), \$(TF_M_DEVICETREE),\$(TF_M_DEVICETREE_\$(1))), \\
		\$(foreach plt, \$(if \$(TF_M_PLATFORM),\$(TF_M_PLATFORM),\$(TF_M_PLATFORM_\$(1))), \\
			mkdir -p "\$(BLD_PATH)/\$(1)-\$(dt)"; \\
			if [ "\$(shell echo "\$(dt)" | grep -q "\$\$(echo "\$(plt)" | awk -F'/' '{print \$\$NF}' | tr '_' '-')" && echo yes)" = "yes" ]; then \\
				touch "\$(BLD_PATH)/\$(1)-\$(dt)/\$(dt)-chk-conf.txt" ; \\
			fi; \\
		) \\
		if ! [ -f "\$(BLD_PATH)/\$(1)-\$(dt)/\$(dt)-chk-conf.txt" ]; then \\
			echo "[CONFIG ERROR] Wrong device tree file name: \$(dt)"; \\
			echo "[CONFIG ERROR] It should contain the base plateform pattern name (any of \$(if \$(TF_M_PLATFORM),\$(TF_M_PLATFORM),\$(TF_M_PLATFORM_\$(1))))"; \\
			exit 1; \\
		fi; \\
		rm -rf "\$(BLD_PATH)/\$(1)-\$(dt)" ; \\
	)
endef

# Configure TF-M make rules (tfm-rules-by-dt <CONFIG> <PLATFORM> <DT>)
#   tfm-<CONFIG>-<PLATFORM>-<DT>: <DEPS>
define tfm-rules-by-dt
tfm-\$(1)-\$(2)-\$(3): chk-conf-\$(1)
	@mkdir -p "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)"; \\
	if [ -e "\$(SRC_PATH)/${TFM_DTS_SUB_PATH}/\$(3)-\$(if \$(TF_M_BOOT_SUFFIX_A35),\$(TF_M_BOOT_SUFFIX_A35),\$(TF_M_BOOT_SUFFIX_A35_\$(1)))-s.${DTS_SUFFIX}" ]; then \\
		echo "\$(SRC_PATH)/${TFM_DTS_SUB_PATH}" > "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)/dt_dirpath"; \\
		touch "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)/dt_tfm_subdirpath"; \\
		touch "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)/dt_bl2_subdirpath"; \\
	elif [ -e "\$(EXTDT_DIR)/\$(EXTDT_DIR_TF_M)/\$(3)-\$(if \$(TF_M_BOOT_SUFFIX_A35),\$(TF_M_BOOT_SUFFIX_A35),\$(TF_M_BOOT_SUFFIX_A35_\$(1)))-s.${DTS_SUFFIX}" ]; then \\
		echo "\$(EXTDT_DIR)" > "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)/dt_dirpath"; \\
		echo "\$(EXTDT_DIR_TF_M)/" > "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)/dt_tfm_subdirpath"; \\
		echo "\$(EXTDT_DIR_MCU)/" > "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)/dt_bl2_subdirpath"; \\
	fi;
	if [ -e "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)/dt_dirpath" ]; then \\
		cmake -GNinja \$(EXTRA_OECMAKE) \\
			-DTFM_TOOLCHAIN_FILE="\$(SRC_PATH)/toolchain_GNUARM.cmake" \\
			\$(if \$(TF_M_EXTRA_OPTFLAGS),\$(TF_M_EXTRA_OPTFLAGS),\$(TF_M_EXTRA_OPTFLAGS_\$(1))) \\
			\$(if \$(TF_M_PLATFORM_OPTFLAGS),\$(TF_M_PLATFORM_OPTFLAGS),\$(TF_M_PLATFORM_OPTFLAGS_\$(shell echo \$(2) | awk -F'/' '{print \$\$NF}'))) \\
			-S "\$(TFM_TESTS_DIR)/tests_reg/spe" -B "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)/spe" \\
			-DTFM_PLATFORM=\$(2) \\
			-DDTS_BOARD_NS="\`cat "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)/dt_tfm_subdirpath"\`\$(3)\$(if \$(TF_M_DT_TFM_SUFFIX),\$(TF_M_DT_TFM_SUFFIX),\$(TF_M_DT_TFM_SUFFIX_\$(1)))-ns.${DTS_SUFFIX}" \\
			-DDTS_BOARD_S="\`cat "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)/dt_tfm_subdirpath"\`\$(3)\$(if \$(TF_M_DT_TFM_SUFFIX),\$(TF_M_DT_TFM_SUFFIX),\$(TF_M_DT_TFM_SUFFIX_\$(1)))-s.${DTS_SUFFIX}" \\
			-DSTM32_BOOT_DEV=\$(if \$(TF_M_BOOTDEV),\$(TF_M_BOOTDEV),\$(TF_M_BOOTDEV_\$(1))) \\
			-DDTS_BOARD_BL2="\`cat "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)/dt_bl2_subdirpath"\`\$(3)\$(if \$(TF_M_DT_BL2_SUFFIX),\$(TF_M_DT_BL2_SUFFIX),\$(TF_M_DT_BL2_SUFFIX_\$(1)))-${BL2_NAME}.${DTS_SUFFIX}" \\
			-DDTS_EXT_DIR="\`cat "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)/dt_dirpath"\`" \\
			${PACKAGECONFIG_CONFARGS} || exit 1; \\
		cmake --build "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)/spe" -- install || exit 1; \\
	fi
endef

# Configure TF-M deploy rules (deploy-rules-by-dt <CONFIG> <PLATFORM> <DT>)
#   deploy-<CONFIG>-<PLATFORM>-<DT>: <DEPS>
define deploy-rules-by-dt
deploy-\$(1)-\$(2)-\$(3): tfm-\$(1)-\$(2)-\$(3)
	@mkdir -p \$(DEPLOYDIR)
	if [ -e "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)/spe/bin/${FWDDR_NAME}.${FWDDR_SUFFIX}" ]; then \\
		cp -f "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)/spe/bin/${FWDDR_NAME}.${FWDDR_SUFFIX}" \\
			"\$(DEPLOYDIR)/${FWDDR_NAME}-\$(3)\$(if \$(TF_M_BOOT_SUFFIX_M33),-\$(TF_M_BOOT_SUFFIX_M33),-\$(TF_M_BOOT_SUFFIX_M33_\$(1))).${FWDDR_SUFFIX}" ; \\
	fi;
	if [ -e "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)/spe/bin/${BL2_NAME}.${BL2_SUFFIX}" ]; then \\
		cp -f "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)/spe/bin/${BL2_NAME}.${BL2_SUFFIX}" \\
			"\$(DEPLOYDIR)/${BL2_NAME}-\$(3)\$(if \$(TF_M_BOOT_SUFFIX_M33),-\$(TF_M_BOOT_SUFFIX_M33),-\$(TF_M_BOOT_SUFFIX_M33_\$(1))).${BL2_SUFFIX}" ; \\
	fi;
endef

# Configure overall check config rules lits
tfm-check-config := \$(foreach config, \$(TF_M_CONFIG), chk-conf-\$(config))

# Configure overall deploy rules list
tfm-deploy := \$(foreach config, \$(TF_M_CONFIG), \\
					\$(foreach plt, \$(if \$(TF_M_PLATFORM),\$(TF_M_PLATFORM),\$(TF_M_PLATFORM_\$(config))), \\
						\$(foreach dt, \$(if \$(TF_M_DEVICETREE), \$(TF_M_DEVICETREE),\$(TF_M_DEVICETREE_\$(config))), \\
							\$(if \$(findstring \$(shell echo \$(plt) | awk -F'/' '{print \$\$NF}' | tr '_' '-'),\$(dt)), \\
								deploy-\$(config)-\$(plt)-\$(dt), chk-conf-\$(config) \\
							) \\
						) \\
					) \\
				)

# Set dependencies list for building all
DEPS = \$(tfm-deploy)

help: \$(tfm-check-config)
	@echo ""
	@echo "TF-M configuration:"
	@echo "  TF_M_CONFIG = \$(TF_M_CONFIG)"
	@echo "Config details:"
	@\$(foreach config, \$(TF_M_CONFIG), \\
		\$(call tfm-configs,\$(config)) \\
		echo "    plateform optionflags:"; \\
		\$(foreach plt, \$(if \$(TF_M_PLATFORM),\$(TF_M_PLATFORM),\$(TF_M_PLATFORM_\$(config))), \\
			\$(call tfm-plt-configs,\$(shell echo \$(plt) | awk -F'/' '{print \$\$NF}')) \\
		) \\
	)
	@echo ""
	@echo "Note that each TF-M configuration settings can be updated through overall or specific config var:"
	@echo "  TF_M_DEVICETREE"
	@echo "  TF_M_PLATFORM"
	@echo "  TF_M_PLATFORM_OPTFLAGS"
	@echo "  TF_M_EXTRA_OPTFLAGS"
	@echo "  TF_M_BOOTDEV"
	@echo "  TF_M_BOOT_SUFFIX_M33"
	@echo "  TF_M_BOOT_SUFFIX_A35"
	@echo "  TF_M_DT_BL2_SUFFIX"
	@echo "  TF_M_DT_TFM_SUFFIX"
	@echo ""
	@echo "TF-M folder configuration:"
	@echo "  SRC_PATH  = \$(SRC_PATH)"
	@echo "  BLD_PATH  = \$(BLD_PATH)"
	@echo "  DEPLOYDIR = \$(DEPLOYDIR)"
	@echo "  EXTDT_DIR = \$(EXTDT_DIR)"
	@echo "  TFM_TESTS_DIR = \$(TFM_TESTS_DIR)"
	@echo ""
	@if [ -n  "\$(EXTDT_DIR)" ]; then \\
		echo "TF-M external dt sub configuration:"; \\
		echo "  EXTDT_DIR/EXTDT_DIR_TF_M = \$(EXTDT_DIR)/\$(EXTDT_DIR_TF_M)"; \\
		echo "  EXTDT_DIR/EXTDT_DIR_MCU = \$(EXTDT_DIR)/\$(EXTDT_DIR_MCU)"; \\
	fi
	@echo "TF-M external source configuration:"
	@echo "  TF_M_EXTERNAL_SOURCES = \$(TF_M_EXTERNAL_SOURCES)"
	@if [ "\$(TF_M_EXTERNAL_SOURCES)" -eq "1" ]; then \\
		echo "  TF_M_PATH_MBEDCRYPTO = \$(TF_M_PATH_MBEDCRYPTO)"; \\
		echo "  TF_M_PATH_MCUBOOT         = \$(TF_M_PATH_MCUBOOT)"; \\
		echo "  TF_M_PATH_QCBOR           = \$(TF_M_PATH_QCBOR)"; \\
		echo "  TF_M_PATH_CMSIS           = \$(TF_M_PATH_CMSIS)"; \\
		echo "  TF_M_PATH_DDR_PHY_BIN_SRC = \$(TF_M_PATH_DDR_PHY_BIN_SRC)"; \\
		echo "  TF_M_PATH_SCP_FW          = \$(TF_M_PATH_SCP_FW)"; \\
	else \\
		echo "  Use TF-M external source download from build process"; \\
	fi
	@echo ""
	@echo "Available targets:"
	@echo "  all   : default target to build all binaries for defined config(s)"
	@echo "  tfm   : build TF-M binaries for defined config(s)"
	@echo "  clean : clean build directories from generated files"
	@echo ""

all: \$(DEPS)

# generate check config rules
\$(foreach config, \$(TF_M_CONFIG), \$(eval \$(call check-config,\$(config))))

# generate TF-M rules by dt
\$(foreach config, \$(TF_M_CONFIG), \\
	\$(foreach plt, \$(if \$(TF_M_PLATFORM),\$(TF_M_PLATFORM),\$(TF_M_PLATFORM_\$(config))), \\
		\$(foreach dt, \$(if \$(TF_M_DEVICETREE), \$(TF_M_DEVICETREE),\$(TF_M_DEVICETREE_\$(config))), \\
			\$(if \$(findstring \$(shell echo \$(plt) | awk -F'/' '{print \$\$NF}' | tr '_' '-'),\$(dt)), \\
				\$(eval \$(call tfm-rules-by-dt,\$(config),\$(plt),\$(dt))) \\
			,)\\
		)\\
	)\\
)

# generate deploy rules by dt
\$(foreach config, \$(TF_M_CONFIG), \\
	\$(foreach plt, \$(if \$(TF_M_PLATFORM),\$(TF_M_PLATFORM),\$(TF_M_PLATFORM_\$(config))), \\
		\$(foreach dt, \$(if \$(TF_M_DEVICETREE), \$(TF_M_DEVICETREE),\$(TF_M_DEVICETREE_\$(config))), \\
			\$(if \$(findstring \$(shell echo \$(plt) | awk -F'/' '{print \$\$NF}' | tr '_' '-'),\$(dt)), \\
				\$(eval \$(call deploy-rules-by-dt,\$(config),\$(plt),\$(dt))) \\
			,)\\
		)\\
	)\\
)

tfm: \$(tfm-deploy)

clean:
	@echo "Removing \$(BLD_PATH) ..."
	@rm -rf \$(BLD_PATH)
	@echo "Removing \$(DEPLOYDIR) ..."
	@rm -rf \$(DEPLOYDIR)
	@echo ""
EOF
}
addtask do_archiver_create_makefile_for_sdk before do_deploy_archives after do_prepare_recipe_sysroot
