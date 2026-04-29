#
# Archiver Configuration
#
SRC_URI:append = " file://README.HOW_TO.txt "

TF_M_ARCHIVER ?= "${@bb.utils.contains('MACHINE_FEATURES', 'm33td', 'archiver', '', d)}"
inherit_defer ${TF_M_ARCHIVER}
ARCHIVER_MODE[src] = "configured"
do_ar_configured[network] = "${@bb.utils.contains('TF_M_EXTERNAL_SOURCES', '1', '1', '0', d)}"
ARCHIVER_MODE[diff] = "1"
COPYLEFT_LICENSE_INCLUDE:append = " BSD-3* "

inherit archiver_stm32mp_clean

do_archiver_git_uri:append() {
    if [ -e "${ARCHIVER_OUTDIR}/${ARCHIVER_README}.${MACHINE}" ]; then
        sed -i \
            -e "s|##TF_M_PATH_QCBOR##|${TF_M_PATH_QCBOR}|g" \
            -e "s|##TF_M_PATH_MCUBOOT##|${TF_M_PATH_MCUBOOT}|g" \
            -e "s|##TF_M_PATH_CMSIS##|${TF_M_PATH_CMSIS}|g" \
            -e "s|##TF_M_PATH_STM32MP2_CMSIS_CORE##|${TF_M_PATH_STM32MP2_CMSIS_CORE}|g" \
            -e "s|##TF_M_PATH_MBEDCRYPTO##|${TF_M_PATH_MBEDCRYPTO}|g" \
            -e "s|##TF_M_PATH_DDR_PHY_BIN_SRC##|${TF_M_PATH_DDR_PHY_BIN_SRC}|g" \
            -e "s|##TF_M_PATH_PSA_ADAC##|${TF_M_PATH_PSA_ADAC}|g" \
            -e "s|##TF_M_PATH_SCP_FW##|${TF_M_PATH_SCP_FW}|g" \
            -e "s|##TF_M_PATH_STM32MP2_HAL_DRIVER##|${TF_M_PATH_STM32MP2_HAL_DRIVER}|g" \
            -e "s|##TF_M_PATH_STM32MP2_CMSIS_DEVICE##|${TF_M_PATH_STM32MP2_CMSIS_DEVICE}|g" \
            -e "s|##TF_M_PATH_STM32MP2_LOW_POWER##|${TF_M_PATH_STM32MP2_LOW_POWER}|g" \
            -e "s|##ARCHIVER_REVISION_QCBOR##|${ARCHIVER_REVISION_QCBOR}|g" \
            -e "s|##ARCHIVER_REVISION_MCUBOOT##|${ARCHIVER_REVISION_MCUBOOT}|g" \
            -e "s|##ARCHIVER_REVISION_CMSIS##|${ARCHIVER_REVISION_CMSIS}|g" \
            -e "s|##ARCHIVER_REVISION_STM32MP2_CMSIS_CORE##|${ARCHIVER_REVISION_STM32MP2_CMSIS_CORE}|g" \
            -e "s|##ARCHIVER_REVISION_MBEDCRYPTO##|${ARCHIVER_REVISION_MBEDCRYPTO}|g" \
            -e "s|##ARCHIVER_REVISION_DDR_PHY_BIN_SRC##|${ARCHIVER_REVISION_DDR_PHY_BIN_SRC}|g" \
            -e "s|##ARCHIVER_REVISION_PSA_ADAC##|${ARCHIVER_REVISION_PSA_ADAC}|g" \
            -e "s|##ARCHIVER_REVISION_SCP_FW##|${ARCHIVER_REVISION_SCP_FW}|g" \
            -e "s|##ARCHIVER_REVISION_STM32MP2_DRIVER##|${ARCHIVER_REVISION_STM32MP2_DRIVER}|g" \
            -e "s|##ARCHIVER_URL_STM32MP2_HAL_DRIVER##|${ARCHIVER_URL_STM32MP2_HAL_DRIVER}|g" \
            -e "s|##ARCHIVER_REVISION_STM32MP2_CMSIS_DEVICE##|${ARCHIVER_REVISION_STM32MP2_CMSIS_DEVICE}|g" \
            -e "s|##ARCHIVER_URL_STM32MP2_CMSIS_DEVICE##|${ARCHIVER_URL_STM32MP2_CMSIS_DEVICE}|g" \
            -e "s|##ARCHIVER_REVISION_MP2_LOW_POWER##|${ARCHIVER_REVISION_MP2_LOW_POWER}|g" \
            -e "s|##ARCHIVER_URL_MP2_LOW_POWER##|${ARCHIVER_URL_MP2_LOW_POWER}|g" \
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
M33FWTOOLDIR ?= \$(SRC_PATH)/..

BL2_DEPLOYDIR ?= \$(DEPLOYDIR)/bl2
DDR_DEPLOYDIR ?= \$(DEPLOYDIR)${M33FW_DIR_FWDDR}
M33FW_DEPLOYDIR ?= \$(DEPLOYDIR)${M33FW_DIR_M33FW}

TFM_TESTS_DIR ?=

EXTDT_DIR_TF_M ?= ${EXTDT_DIR_TF_M}
EXTDT_DIR_MCU ?= ${EXTDT_DIR_MCU}

# Set default TF-M config
TF_M_CONFIG ?= ${M33FW_CONFIG}
M33FW_BOARDS ?= ${M33FW_BOARDS}

# Set default M33PROJECT config
M33FW_PROJECT_NAME ?=

# Default TF-M overall settings to null
M33FW_DEVICETREE ?=
M33FW_BOOT_SUFFIX_M33 ?=
M33FW_BOOT_SUFFIX_A35 ?=
TF_M_PLATFORM ?=
TF_M_PLATFORM_OPTFLAGS ?=
TF_M_EXTRA_OPTFLAGS ?=
TF_M_BOOTDEV ?=
TF_M_DT_BL2_SUFFIX ?=
TF_M_DT_TFM_SUFFIX ?=

# Init default config settings
EOF
    unset i
    for config in ${M33FW_CONFIG}; do
        i=$(expr $i + 1)
        cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
M33FW_DEVICETREE_INTERNAL_$config ?= $(echo ${M33FW_DEVICETREE_INTERNAL} | cut -d',' -f${i})
M33FW_DEVICETREE_EXTERNAL_$config ?= $(echo ${M33FW_DEVICETREE_EXTERNAL} | cut -d',' -f${i})
M33FW_DEVICETREE_$config = \$(M33FW_DEVICETREE_INTERNAL_${config}) \$(if \$(EXTDT_DIR),\$(M33FW_DEVICETREE_EXTERNAL_${config}))
M33FW_BOOT_SUFFIX_M33_$config ?= $(echo ${M33FW_BOOT_SUFFIX_M33} | cut -d',' -f${i})
M33FW_BOOT_SUFFIX_A35_$config ?= $(echo ${M33FW_BOOT_SUFFIX_A35} | cut -d',' -f${i})
TF_M_EXTRA_OPTFLAGS_$config ?= $(echo ${TF_M_EXTRA_OPTFLAGS} | cut -d',' -f${i})
TF_M_BOOTDEV_$config ?= $(echo ${TF_M_BOOTDEV} | cut -d',' -f${i})
TF_M_DT_BL2_SUFFIX_$config ?= $(echo ${TF_M_DT_BL2_SUFFIX} | cut -d',' -f${i})
TF_M_DT_TFM_SUFFIX_$config ?= $(echo ${TF_M_DT_TFM_SUFFIX} | cut -d',' -f${i})
EOF
    done
    unset k
    for board in ${M33FW_BOARDS}; do
        k=$(expr $k + 1)
        cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
TF_M_PLATFORM_$board ?= $(echo ${M33FW_TFM_PLATFORM} | cut -d',' -f${k})
TF_M_PLATFORM_OPTFLAGS_$board ?= $(echo ${M33FW_TFM_OPTFLAGS} | cut -d',' -f${k})
EOF
    done
    cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}

# Init external source use
TF_M_EXTERNAL_SOURCES ?= ${TF_M_EXTERNAL_SOURCES}
# Path to extra components
TF_M_PATH_QCBOR ?= \$(SRC_PATH)/${TF_M_PATH_QCBOR}
TF_M_PATH_MCUBOOT ?= \$(SRC_PATH)/${TF_M_PATH_MCUBOOT}
TF_M_PATH_CMSIS ?= \$(SRC_PATH)/${TF_M_PATH_CMSIS}
TF_M_PATH_STM32MP2_CMSIS_CORE ?= \$(SRC_PATH)/${TF_M_PATH_STM32MP2_CMSIS_CORE}
TF_M_PATH_MBEDCRYPTO ?= \$(SRC_PATH)/${TF_M_PATH_MBEDCRYPTO}
TF_M_PATH_DDR_PHY_BIN_SRC ?= \$(SRC_PATH)/${TF_M_PATH_DDR_PHY_BIN_SRC}
TF_M_PATH_PSA_ADAC ?= \$(SRC_PATH)/${TF_M_PATH_PSA_ADAC}
TF_M_PATH_SCP_FW ?= \$(SRC_PATH)/${TF_M_PATH_SCP_FW}
TF_M_PATH_STM32MP2_HAL_DRIVER ?= \$(SRC_PATH)/${TF_M_PATH_STM32MP2_HAL_DRIVER}
TF_M_PATH_STM32MP2_CMSIS_DEVICE ?= \$(SRC_PATH)/${TF_M_PATH_STM32MP2_CMSIS_DEVICE}
TF_M_PATH_STM32MP2_LOW_POWER ?= \$(SRC_PATH)/${TF_M_PATH_STM32MP2_LOW_POWER}

# Define default make options
EXTRA_OECMAKE ?= $(echo "${EXTRA_OECMAKE}" | sed -e 's#-DCONFIG_TFM_SOURCE_PATH=[^[:space:]]*##' \
                                                    -e 's#-DFETCHCONTENT_FULLY_DISCONNECTED=[^[:space:]]*##' \
                                                    -e 's#-DQCBOR_PATH=[^[:space:]]*##' \
                                                    -e 's#-DMCUBOOT_PATH=[^[:space:]]*##' \
                                                    -e 's#-DCMSIS_PATH=[^[:space:]]*##' \
                                                    -e 's#-DCMSIS_CORE_PATH=[^[:space:]]*##' \
                                                    -e 's#-DMBEDCRYPTO_PATH=[^[:space:]]*##' \
                                                    -e 's#-DDDR_PHY_BIN_SRC_PATH=[^[:space:]]*##' \
                                                    -e 's#-DPLATFORM_PSA_ADAC_SOURCE_PATH=[^[:space:]]*##' \
                                                    -e 's#-DSCP_FW_PATH=[^[:space:]]*##' \
                                                    -e 's#-DSTM32MP2XX_HAL_DRIVER_PATH=[^[:space:]]*##' \
                                                    -e 's#-DCMSIS_DEVICE_MP2_PATH=[^[:space:]]*##' \
                                                    -e 's#-DSTM32MP2_LP_PATH=[^[:space:]]*##' \
                )
# Append source configuration
EXTRA_OECMAKE +=  -DTFM_VERSION='${TF_M_VERSION}'
EXTRA_OECMAKE +=  -DCONFIG_TFM_SOURCE_PATH=\$(SRC_PATH)
EXTRA_OECMAKE += \$(if \$(findstring 1,\$(TF_M_EXTERNAL_SOURCES)), -DFETCHCONTENT_FULLY_DISCONNECTED=ON)
EXTRA_OECMAKE += \$(if \$(findstring 1,\$(TF_M_EXTERNAL_SOURCES)), -DQCBOR_PATH=\$(TF_M_PATH_QCBOR))
EXTRA_OECMAKE += \$(if \$(findstring 1,\$(TF_M_EXTERNAL_SOURCES)), -DMCUBOOT_PATH=\$(TF_M_PATH_MCUBOOT))
EXTRA_OECMAKE += \$(if \$(findstring 1,\$(TF_M_EXTERNAL_SOURCES)), -DCMSIS_PATH=\$(TF_M_PATH_CMSIS))
EXTRA_OECMAKE += \$(if \$(findstring 1,\$(TF_M_EXTERNAL_SOURCES)), -DCMSIS_CORE_PATH=\$(TF_M_PATH_STM32MP2_CMSIS_CORE))
EXTRA_OECMAKE += \$(if \$(findstring 1,\$(TF_M_EXTERNAL_SOURCES)), -DMBEDCRYPTO_PATH=\$(TF_M_PATH_MBEDCRYPTO))
EXTRA_OECMAKE += \$(if \$(findstring 1,\$(TF_M_EXTERNAL_SOURCES)), -DDDR_PHY_BIN_SRC_PATH=\$(TF_M_PATH_DDR_PHY_BIN_SRC))
EXTRA_OECMAKE += \$(if \$(findstring 1,\$(TF_M_EXTERNAL_SOURCES)), -DPLATFORM_PSA_ADAC_SOURCE_PATH=\$(TF_M_PATH_PSA_ADAC))
EXTRA_OECMAKE += \$(if \$(findstring 1,\$(TF_M_EXTERNAL_SOURCES)), -DSCP_FW_PATH=\$(TF_M_PATH_SCP_FW))
EXTRA_OECMAKE += \$(if \$(findstring 1,\$(TF_M_EXTERNAL_SOURCES)), -DSTM32MP2XX_HAL_DRIVER_PATH=\$(TF_M_PATH_STM32MP2_HAL_DRIVER))
EXTRA_OECMAKE += \$(if \$(findstring 1,\$(TF_M_EXTERNAL_SOURCES)), -DCMSIS_DEVICE_MP2_PATH=\$(TF_M_PATH_STM32MP2_CMSIS_DEVICE))
EXTRA_OECMAKE += \$(if \$(findstring 1,\$(TF_M_EXTERNAL_SOURCES)), -DSTM32MP2_LP_PATH=\$(TF_M_PATH_STM32MP2_LOW_POWER))

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

# Check for device tree list (dt-check)
define dt-check
	mkdir -p \$(BLD_PATH) ; \\
	rm -f \$(BLD_PATH)/dt-check ; \\
	\$(foreach config, \$(TF_M_CONFIG), \\
		if [ -n "\$(strip \$(M33FW_DEVICETREE))" ] || [ -n "\$(strip \$(M33FW_DEVICETREE_\$(config)))" ]; then \\
			echo "\$(if \$(M33FW_DEVICETREE),\$(M33FW_DEVICETREE),\$(M33FW_DEVICETREE_\$(config)))" > \$(BLD_PATH)/dt-check ; \\
		fi ; \\
	)
endef

# Display TF-M config details (tfm-configs <CONFIG>)
define tfm-configs
	echo "  \$(1)" ; \\
	echo "    with device tree     : \$(if \$(M33FW_DEVICETREE),\$(M33FW_DEVICETREE),\$(M33FW_DEVICETREE_\$(1)))" ; \\
	echo "    bootdevice m33 suffix: \$(if \$(M33FW_BOOT_SUFFIX_M33),\$(M33FW_BOOT_SUFFIX_M33),\$(M33FW_BOOT_SUFFIX_M33_\$(1)))" ; \\
	echo "    bootdevice a35 suffix: \$(if \$(M33FW_BOOT_SUFFIX_A35),\$(M33FW_BOOT_SUFFIX_A35),\$(M33FW_BOOT_SUFFIX_A35_\$(1)))" ;
	echo "    extra optionflags    : \$(if \$(TF_M_EXTRA_OPTFLAGS),\$(TF_M_EXTRA_OPTFLAGS),\$(TF_M_EXTRA_OPTFLAGS_\$(1)))" ; \\
	echo "    bootdevice target    : \$(if \$(TF_M_BOOTDEV),\$(TF_M_BOOTDEV),\$(TF_M_BOOTDEV_\$(1)))" ; \\
	echo "    bl2 devicetree suffix: \$(if \$(TF_M_DT_BL2_SUFFIX),\$(TF_M_DT_BL2_SUFFIX),\$(TF_M_DT_BL2_SUFFIX_\$(1)))" ; \\
	echo "    tfm devicetree suffix: \$(if \$(TF_M_DT_TFM_SUFFIX),\$(TF_M_DT_TFM_SUFFIX),\$(TF_M_DT_TFM_SUFFIX_\$(1)))" ;
endef

# Display TF-M plateform config details (tfm-plt-configs <BOARD>)
define tfm-plt-configs
	echo "  \$(1)" ; \\
	echo "    plateform config     : \$(if \$(TF_M_PLATFORM),\$(TF_M_PLATFORM),\$(TF_M_PLATFORM_\$(1)))" ; \\
	echo "    plateform option flag: \$(if \$(TF_M_PLATFORM_OPTFLAGS),\$(TF_M_PLATFORM_OPTFLAGS),\$(TF_M_PLATFORM_OPTFLAGS_\$(1)))" ;
endef

# Configure TF-M check configuration rules (check-config <CONFIG>)
#   chk-conf-<CONFIG>
define check-config
chk-conf-\$(1):
	@\$(foreach dt, \$(if \$(M33FW_DEVICETREE), \$(M33FW_DEVICETREE),\$(M33FW_DEVICETREE_\$(1))), \\
		\$(foreach board, \$(M33FW_BOARDS), \\
			\$(if \$(findstring \$(board),\$(dt)), \\
				mkdir -p "\$(BLD_PATH)/\$(1)-\$(dt)"; \\
				touch "\$(BLD_PATH)/\$(1)-\$(dt)/\$(dt)-chk-conf.txt" ; \\
			) \\
		) \\
		if ! [ -f "\$(BLD_PATH)/\$(1)-\$(dt)/\$(dt)-chk-conf.txt" ]; then \\
			echo "[CONFIG ERROR] Wrong device tree file name: \$(dt)"; \\
			echo "[CONFIG ERROR] It should contain the board pattern name (any of \$(M33FW_BOARDS))"; \\
			exit 1; \\
		fi; \\
		rm -rf "\$(BLD_PATH)/\$(1)-\$(dt)/\$(dt)-chk-conf.txt" ; \\
	)
endef

# Configure TF-M make rules (tfm-rules-by-dt <CONFIG> <BOARD> <DT>)
#   tfm-<CONFIG>-<BOARD>-<DT>: <DEPS>
define tfm-rules-by-dt
tfm-\$(1)-\$(2)-\$(3): chk-conf-\$(1)
	@mkdir -p "\$(BLD_PATH)/\$(1)-\$(3)"; \\
	if [ -e "\$(SRC_PATH)/${DTS_SUB_PATH}/\$(3)\$(if \$(TF_M_DT_TFM_SUFFIX),\$(TF_M_DT_TFM_SUFFIX),\$(TF_M_DT_TFM_SUFFIX_\$(1)))-s.${DTS_SUFFIX}" ]; then \\
		echo "\$(SRC_PATH)/${DTS_SUB_PATH}" > "\$(BLD_PATH)/\$(1)-\$(3)/dt_dirpath"; \\
		touch "\$(BLD_PATH)/\$(1)-\$(3)/dt_tfm_subdirpath"; \\
		touch "\$(BLD_PATH)/\$(1)-\$(3)/dt_bl2_subdirpath"; \\
	elif [ -e "\$(EXTDT_DIR)/\$(EXTDT_DIR_TF_M)/\$(3)\$(if \$(TF_M_DT_TFM_SUFFIX),\$(TF_M_DT_TFM_SUFFIX),\$(TF_M_DT_TFM_SUFFIX_\$(1)))-s.${DTS_SUFFIX}" ]; then \\
		echo "\$(EXTDT_DIR)" > "\$(BLD_PATH)/\$(1)-\$(3)/dt_dirpath"; \\
		echo "\$(EXTDT_DIR_TF_M)/" > "\$(BLD_PATH)/\$(1)-\$(3)/dt_tfm_subdirpath"; \\
		echo "\$(EXTDT_DIR_MCU)/" > "\$(BLD_PATH)/\$(1)-\$(3)/dt_bl2_subdirpath"; \\
	fi;
	if ! [ -e "\$(BLD_PATH)/\$(1)-\$(3)/dt_dirpath" ]; then \\
		echo "[TF-M CONFIG ERROR] Not able to find \$(3)\$(if \$(TF_M_DT_TFM_SUFFIX),\$(TF_M_DT_TFM_SUFFIX),\$(TF_M_DT_TFM_SUFFIX_\$(1)))-s.dts devicetree file" ; \\
		exit 1 ; \\
	fi
	echo "\$(if \$(TF_M_PLATFORM),\$(TF_M_PLATFORM),\$(TF_M_PLATFORM_\$(2)))" > "\$(BLD_PATH)/\$(1)-\$(3)/pltf_dirpath"; \\
	if [ -e "\$(EXTDT_DIR)/\$(if \$(TF_M_PLATFORM),\$(TF_M_PLATFORM),\$(TF_M_PLATFORM_\$(2)))" ]; then \\
		echo "\$(EXTDT_DIR)/\$(if \$(TF_M_PLATFORM),\$(TF_M_PLATFORM),\$(TF_M_PLATFORM_\$(2)))" > "\$(BLD_PATH)/\$(1)-\$(3)/pltf_dirpath"; \\
	fi;
	cmake -GNinja \$(EXTRA_OECMAKE) \\
		-DTFM_TOOLCHAIN_FILE="\$(SRC_PATH)/toolchain_GNUARM.cmake" \\
		\$(if \$(TF_M_EXTRA_OPTFLAGS),\$(TF_M_EXTRA_OPTFLAGS),\$(TF_M_EXTRA_OPTFLAGS_\$(1))) \\
		\$(if \$(TF_M_PLATFORM_OPTFLAGS),\$(TF_M_PLATFORM_OPTFLAGS),\$(TF_M_PLATFORM_OPTFLAGS_\$(2))) \\
		-S "\$(TFM_TESTS_DIR)/tests_reg/spe" -B "\$(BLD_PATH)/\$(1)-\$(3)/spe" \\
		-DTFM_PLATFORM="\`cat "\$(BLD_PATH)/\$(1)-\$(3)/pltf_dirpath"\`" \\
		-DDTS_BOARD_NS="\`cat "\$(BLD_PATH)/\$(1)-\$(3)/dt_tfm_subdirpath"\`\$(3)\$(if \$(TF_M_DT_TFM_SUFFIX),\$(TF_M_DT_TFM_SUFFIX),\$(TF_M_DT_TFM_SUFFIX_\$(1)))-ns.${DTS_SUFFIX}" \\
		-DDTS_BOARD_S="\`cat "\$(BLD_PATH)/\$(1)-\$(3)/dt_tfm_subdirpath"\`\$(3)\$(if \$(TF_M_DT_TFM_SUFFIX),\$(TF_M_DT_TFM_SUFFIX),\$(TF_M_DT_TFM_SUFFIX_\$(1)))-s.${DTS_SUFFIX}" \\
		-DSTM32_BOOT_DEV=\$(if \$(TF_M_BOOTDEV),\$(TF_M_BOOTDEV),\$(TF_M_BOOTDEV_\$(1))) \\
		-DDTS_BOARD_BL2="\`cat "\$(BLD_PATH)/\$(1)-\$(3)/dt_bl2_subdirpath"\`\$(3)\$(if \$(TF_M_DT_BL2_SUFFIX),\$(TF_M_DT_BL2_SUFFIX),\$(TF_M_DT_BL2_SUFFIX_\$(1)))-${BL2_NAME}.${DTS_SUFFIX}" \\
		-DDTS_EXT_DIR="\`cat "\$(BLD_PATH)/\$(1)-\$(3)/dt_dirpath"\`" \\
		${PACKAGECONFIG_CONFARGS} || { echo "[TF-M CMAKE ERROR] TF_M_CONFIG=\$(1), devicetree=\$(3)"; exit 1; } ; \\
	cmake --build "\$(BLD_PATH)/\$(1)-\$(3)/spe" -- install || { echo "[TF-M INSTALL ERROR] TF_M_CONFIG=\$(1), devicetree=\$(3)"; exit 1; }
endef

# Configure TF-M deploy rules (deploy-rules-by-dt <CONFIG> <BOARD> <DT>)
#   deploy-<CONFIG>-<BOARD>-<DT>: <DEPS>
define deploy-rules-by-dt
deploy-\$(1)-\$(2)-\$(3): tfm-\$(1)-\$(2)-\$(3)
	@mkdir -p \$(DEPLOYDIR)
	if [ -e "\$(BLD_PATH)/\$(1)-\$(3)/spe/bin/${BL2_NAME}.${BL2_SUFFIX}" ]; then \\
		mkdir -p "\$(BL2_DEPLOYDIR)" ; \\
		cp -f "\$(BLD_PATH)/\$(1)-\$(3)/spe/bin/${BL2_NAME}.${BL2_SUFFIX}" \\
			"\$(BL2_DEPLOYDIR)/${BL2_NAME}-\$(3)\$(if \$(M33FW_BOOT_SUFFIX_M33),-\$(M33FW_BOOT_SUFFIX_M33),-\$(M33FW_BOOT_SUFFIX_M33_\$(1))).${BL2_SUFFIX}" ; \\
	fi;
	if [ -e "\$(BLD_PATH)/\$(1)-\$(3)/spe/bin/${FWDDR_NAME}.${FWDDR_SUFFIX}" ]; then \\
		mkdir -p "\$(DDR_DEPLOYDIR)" ; \\
		cp -f "\$(BLD_PATH)/\$(1)-\$(3)/spe/bin/${FWDDR_NAME}.${FWDDR_SUFFIX}" \\
			"\$(DDR_DEPLOYDIR)/${FWDDR_NAME}-\$(3)\$(if \$(M33FW_BOOT_SUFFIX_M33),-\$(M33FW_BOOT_SUFFIX_M33),-\$(M33FW_BOOT_SUFFIX_M33_\$(1))).${FWDDR_SUFFIX}" ; \\
	fi;
	if [ -e "\$(BLD_PATH)/\$(1)-\$(3)/spe/${FWDDR_LAYOUT_PATH}/${FWDDR_LAYOUT_BASENAME}${TF_M_LAYOUT_NAME}.${TF_M_LAYOUT_SUFFIX}" ]; then \\
		mkdir -p "\$(DDR_DEPLOYDIR)" ; \\
		cp -f "\$(BLD_PATH)/\$(1)-\$(3)/spe/${FWDDR_LAYOUT_PATH}/${FWDDR_LAYOUT_BASENAME}${TF_M_LAYOUT_NAME}.${TF_M_LAYOUT_SUFFIX}" \\
			"\$(DDR_DEPLOYDIR)/${FWDDR_NAME}-\$(3)\$(if \$(M33FW_BOOT_SUFFIX_M33),-\$(M33FW_BOOT_SUFFIX_M33),-\$(M33FW_BOOT_SUFFIX_M33_\$(1))).${TF_M_LAYOUT_NAME}" ; \\
	fi;
	if [ -e "\$(BLD_PATH)/\$(1)-\$(3)/spe/bin/${TF_M_NAME}${TF_M_TYPE_S}.${TF_M_SUFFIX}" ]; then \\
		cp -f "\$(BLD_PATH)/\$(1)-\$(3)/spe/bin/${TF_M_NAME}${TF_M_TYPE_S}.${TF_M_SUFFIX}" \\
			"\$(DEPLOYDIR)/${TF_M_NAME}-\$(3)\$(if \$(M33FW_BOOT_SUFFIX_M33),-\$(M33FW_BOOT_SUFFIX_M33),-\$(M33FW_BOOT_SUFFIX_M33_\$(1)))\$(if \$(M33FW_BOOT_SUFFIX_A35),\-$(M33FW_BOOT_SUFFIX_A35),-\$(M33FW_BOOT_SUFFIX_A35_\$(1)))${TF_M_TYPE_S}.${TF_M_SUFFIX}" ; \\
	fi;
	if [ -e "\$(BLD_PATH)/\$(1)-\$(3)/spe/api_ns/image_signing/layout_files/${TF_M_LAYOUT_NAME}${TF_M_TYPE_S}${TF_M_TYPE_NS}.${TF_M_LAYOUT_SUFFIX}" ]; then \\
		cp -f "\$(BLD_PATH)/\$(1)-\$(3)/spe/api_ns/image_signing/layout_files/${TF_M_LAYOUT_NAME}${TF_M_TYPE_S}${TF_M_TYPE_NS}.${TF_M_LAYOUT_SUFFIX}" \\
			"\$(DEPLOYDIR)/${TF_M_NAME}-\$(3)\$(if \$(M33FW_BOOT_SUFFIX_M33),-\$(M33FW_BOOT_SUFFIX_M33),-\$(M33FW_BOOT_SUFFIX_M33_\$(1)))\$(if \$(M33FW_BOOT_SUFFIX_A35),-\$(M33FW_BOOT_SUFFIX_A35),-\$(M33FW_BOOT_SUFFIX_A35_\$(1)))${TF_M_TYPE_S}.${TF_M_LAYOUT_NAME}" ; \\
	fi;
endef

# Configure overall check config rules lits
tfm-check-config := \$(foreach config, \$(TF_M_CONFIG), chk-conf-\$(config))

# Configure overall deploy rules list
tfm-deploy := \$(foreach config, \$(TF_M_CONFIG), \\
					\$(foreach dt, \$(if \$(M33FW_DEVICETREE), \$(M33FW_DEVICETREE),\$(M33FW_DEVICETREE_\$(config))), \\
						\$(foreach board, \$(M33FW_BOARDS), \\
							\$(if \$(findstring \$(board),\$(dt)), \\
								deploy-\$(config)-\$(board)-\$(dt), chk-conf-\$(config) \\
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
	)
	@echo "Plateform config details:"
	@\$(foreach board, \$(M33FW_BOARDS), \\
		\$(call tfm-plt-configs,\$(board)) \\
	)
	@echo ""
	@echo "Note that each TF-M configuration settings can be updated through overall or specific config var:"
	@echo "  M33FW_DEVICETREE"
	@echo "  M33FW_BOOT_SUFFIX_M33"
	@echo "  M33FW_BOOT_SUFFIX_A35"
	@echo "  TF_M_EXTRA_OPTFLAGS"
	@echo "  TF_M_BOOTDEV"
	@echo "  TF_M_DT_BL2_SUFFIX"
	@echo "  TF_M_DT_TFM_SUFFIX"
	@echo "  TF_M_PLATFORM"
	@echo "  TF_M_PLATFORM_OPTFLAGS"
	@echo ""
	@echo "TF-M folder configuration:"
	@echo "  SRC_PATH  = \$(SRC_PATH)"
	@echo "  BLD_PATH  = \$(BLD_PATH)"
	@echo "  DEPLOYDIR = \$(DEPLOYDIR)"
	@echo "  EXTDT_DIR = \$(EXTDT_DIR)"
	@echo "  BL2_DEPLOYDIR = \$(BL2_DEPLOYDIR)"
	@echo "  DDR_DEPLOYDIR = \$(DDR_DEPLOYDIR)"
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
		echo "  TF_M_PATH_DDR_FW_UTILS_SRC = \$(TF_M_PATH_DDR_FW_UTILS_SRC)"; \\
		echo "  TF_M_PATH_SCP_FW          = \$(TF_M_PATH_SCP_FW)"; \\
	else \\
		echo "  Use TF-M external source download from build process"; \\
	fi
	@echo ""
	@echo "M33FW configuration:"
	@echo "  Do not forget to set M33FW deploydir folders (such as M33FW_DEPLOYDIR_ROOT) to provide path to needed binaries"
	@echo ""
	@echo "Available targets:"
	@echo "  all   : default target to build TF-M binaries for defined config(s)"
	@echo "  m33fw : assemble and sign M33 firmwares for defined config(s)"
	@echo "  tfm   : build TF-M binaries for defined config(s)"
	@echo "  clean : clean build directories from generated files"
	@echo ""

all: \$(DEPS)

clean:
	@\$(call dt-check)
	@if [ -e "\$(BLD_PATH)/dt-check" ]; then \\
		echo "Removing \$(BLD_PATH) ..." ; \\
		rm -rf \$(BLD_PATH) ; \\
		echo "Removing \$(DEPLOYDIR) ..." ; \\
		rm -rf \$(DEPLOYDIR) ; \\
		echo "" ; \\
	else \\
		echo "[INFO] No device tree file configured: skip \$(@) target"; \\
	fi

m33fw: \$(tfm-deploy)
	@\$(call dt-check)
	@if [ -e "\$(BLD_PATH)/dt-check" ]; then \\
		for config in \$(TF_M_CONFIG) ; do \\
			M33FW_DEPLOYDIR_TFM=\$(DEPLOYDIR) \\
			M33FW_DEPLOYDIR_DDR=\$(DDR_DEPLOYDIR) \\
			M33FW_CONFIG="\$\$config" \\
			\$(if \$(M33FW_DEVICETREE),M33FW_DEVICETREE="\$(M33FW_DEVICETREE)") \\
			\$(if \$(M33FW_BOOT_SUFFIX_M33),M33FW_BOOT_SUFFIX_M33="\$(M33FW_BOOT_SUFFIX_M33)") \\
			\$(if \$(M33FW_BOOT_SUFFIX_A35),M33FW_BOOT_SUFFIX_A35="\$(M33FW_BOOT_SUFFIX_A35)") \\
			\$(if \$(M33FWtfm-deploy_PROJECT_NAME),M33FW_DEFAULT_FIRMWARE="\$(M33FW_PROJECT_NAME)") \\
			\$(M33FWTOOLDIR)/m33fwtool-stm32mp.${MACHINE} || { echo "[M33FW BUILD ERROR] M33FW_CONFIG=\$\$config \$(if \$(M33FW_DEVICETREE),M33FW_DEVICETREE=\$(M33FW_DEVICETREE))"; exit 1; } ; \\
		done ; \\
		echo "***** M33FW binary available under: \$(if \$(M33FW_DEPLOYDIR_M33FW),\$(M33FW_DEPLOYDIR_M33FW),\$(M33FW_DEPLOYDIR_ROOT)/) *****" ; \\
	else \\
		echo "[INFO] No device tree file configured: skip \$(@) target"; \\
	fi

tfm: \$(tfm-deploy)
	@\$(call dt-check)
	@if [ -e "\$(BLD_PATH)/dt-check" ]; then \\
		rm "\$(BLD_PATH)/dt-check" ; \\
		echo "***** TF-M binary available under: \$(DEPLOYDIR)/ *****" ; \\
	else \\
		echo "[INFO] No device tree file configured: skip \$(@) target"; \\
	fi

# generate check config rules
\$(foreach config, \$(TF_M_CONFIG), \$(eval \$(call check-config,\$(config))))

# generate TF-M rules by dt
\$(foreach config, \$(TF_M_CONFIG), \\
	\$(foreach dt, \$(if \$(M33FW_DEVICETREE), \$(M33FW_DEVICETREE),\$(M33FW_DEVICETREE_\$(config))), \\
		\$(foreach board, \$(M33FW_BOARDS), \\
			\$(if \$(findstring \$(board),\$(dt)), \\
				\$(eval \$(call tfm-rules-by-dt,\$(config),\$(board),\$(dt))) \\
			) \\
		) \\
	) \\
)

# generate deploy rules by dt
\$(foreach config, \$(TF_M_CONFIG), \\
	\$(foreach dt, \$(if \$(M33FW_DEVICETREE), \$(M33FW_DEVICETREE),\$(M33FW_DEVICETREE_\$(config))), \\
		\$(foreach board, \$(M33FW_BOARDS), \\
			\$(if \$(findstring \$(board),\$(dt)), \\
				\$(eval \$(call deploy-rules-by-dt,\$(config),\$(board),\$(dt))) \\
			) \\
		) \\
	) \\
)
EOF
}
addtask do_archiver_create_makefile_for_sdk before do_deploy_archives after do_prepare_recipe_sysroot
