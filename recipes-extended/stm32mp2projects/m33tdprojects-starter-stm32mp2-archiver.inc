#
# Archiver Configuration
#
SRC_URI:append = " file://README.HOW_TO.txt "

M33TD_STARTER_ARCHIVER ?= "${@bb.utils.contains('MACHINE_FEATURES', 'm33td', 'archiver', '', d)}"
inherit_defer ${M33TD_STARTER_ARCHIVER}
ARCHIVER_MODE[src] = "configured"
COPYLEFT_LICENSE_INCLUDE:append = " BSD-3* "

inherit archiver_stm32mp_clean

do_archiver_create_makefile_for_sdk() {
    mkdir -p ${ARCHIVER_OUTDIR}
    cat << EOF > ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
# Set default path
SRC_PATH ?= \$(PWD)
DEPLOYDIR ?= \$(SRC_PATH)/../deploy

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
M33TD_PROJECT ?=

# Init default config settings
EOF
    unset i
    for config in ${TF_M_CONFIG}; do
        i=$(expr $i + 1)
        cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
TF_M_DEVICETREE_INTERNAL_$config ?= $(echo ${TF_M_DEVICETREE_INTERNAL} | cut -d',' -f${i})
TF_M_DEVICETREE_EXTERNAL_$config ?= $(echo ${TF_M_DEVICETREE_EXTERNAL} | cut -d',' -f${i})
TF_M_DEVICETREE_$config = \$(TF_M_DEVICETREE_INTERNAL_${config}) \$(if \$(EXTDT_DIR),\$(TF_M_DEVICETREE_EXTERNAL_${config}))
TF_M_PLATFORM_$config ?= $(echo ${TF_M_PLATFORM} | cut -d',' -f${i})
TF_M_BOOTDEV_$config ?= $(echo ${TF_M_BOOTDEV} | cut -d',' -f${i})
TF_M_BOOT_SUFFIX_M33_$config ?= $(echo ${TF_M_BOOT_SUFFIX_M33} | cut -d',' -f${i})
TF_M_BOOT_SUFFIX_A35_$config ?= $(echo ${TF_M_BOOT_SUFFIX_A35} | cut -d',' -f${i})
EOF
    done

    unset i
    for config in ${M33TD_FIRMWARE_CONFIG}; do
        i=$(expr $i + 1)
        cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
M33TD_PROJECT_$config ?= $(echo ${M33TD_PROJECT} | cut -d',' -f${i}| cut -d':' -f2)
EOF
    done

    cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}

# Append toolchain path for SDK
PATH += :\$(OECORE_NATIVE_SYSROOT)/usr/share/gcc-arm-none-eabi/bin
# Unset CMAKE_TOOLCHAIN_FILE for SDK
CMAKE_TOOLCHAIN_FILE =

# Display TF-M config details (tfm-configs <CONFIG>)
define tfm-configs
	echo "  \$(1)" ; \\
	echo "    with device tree     : \$(if \$(TF_M_DEVICETREE),\$(TF_M_DEVICETREE),\$(TF_M_DEVICETREE_\$(1)))" ; \\
	echo "    plateform config     : \$(if \$(TF_M_PLATFORM),\$(TF_M_PLATFORM),\$(TF_M_PLATFORM_\$(1)))" ; \\
	echo "    bootdevice target    : \$(if \$(TF_M_BOOTDEV),\$(TF_M_BOOTDEV),\$(TF_M_BOOTDEV_\$(1)))" ;
endef

# Configure TF-M make rules (tfm-rules-by-dt <CONFIG> <PLATFORM> <DT>)
#   tfm-<CONFIG>-<PLATFORM>-<DT>-<M33TD_PROJECT>: <DEPS>
define m33td-starter-rules-by-dt
m33td-\$(1)-\$(3):
	cd "\$(SRC_PATH)/\$(4)"
	@mkdir -p "\$(SRC_PATH)/\$(4)/\$(1)-\$(2)-\$(3)"; \\
	sed -i "s|python.exe|python3|g" "\$(SRC_PATH)/\$(4)/CMakeLists.txt"; \\
	if [ -d "\$(TF_M_BUILD_PATH)/\$(1)-\$(2)-\$(3)/spe" ]; then \\
		cmake -G"Unix Makefiles" \\
			-DTFM_BUILD_DIR="\$(TF_M_BUILD_PATH)/\$(1)-\$(2)-\$(3)/spe" \\
			-S "\$(SRC_PATH)/\$(4)" \\
			-B "\$(SRC_PATH)/\$(4)/\$(1)-\$(2)-\$(3)" || exit 1; \\
		cmake --build "\$(SRC_PATH)/\$(4)/\$(1)-\$(2)-\$(3)" -- all || exit 1; \\
	fi
endef

# Configure TF-M deploy rules (deploy-rules-by-dt <CONFIG> <PLATFORM> <DT>)
#   deploy-<CONFIG>-<PLATFORM>-<DT>-<M33TD_PROJECT>: <DEPS>
define deploy-rules-by-dt
deploy-\$(1)-\$(3): m33td-\$(1)-\$(3)
	@mkdir -p \$(DEPLOYDIR)
	basename \$(4) > "\$(SRC_PATH)/\$(4)/\$(1)-\$(2)-\$(3)/m33td_project_name"; \\
	if [ -e "\$(SRC_PATH)/\$(4)/\$(1)-\$(2)-\$(3)/\`cat \$(SRC_PATH)/\$(4)/\$(1)-\$(2)-\$(3)/m33td_project_name\`${M33TD_FIWMWARE_TYPE}.${M33TD_FIWMWARE_SUFFIX}" ]; then \\
		cp -f "\$(SRC_PATH)/\$(4)/\$(1)-\$(2)-\$(3)/\`cat \$(SRC_PATH)/\$(4)/\$(1)-\$(2)-\$(3)/m33td_project_name\`${M33TD_FIWMWARE_TYPE}.${M33TD_FIWMWARE_SUFFIX}" \\
			"\$(DEPLOYDIR)/${M33TD_FIWMWARE_DEPLOYED_PREFIX}-\$(3)-\$(if \$(TF_M_BOOT_SUFFIX_M33),\$(TF_M_BOOT_SUFFIX_M33),\$(TF_M_BOOT_SUFFIX_M33_\$(1)))-\$(if \$(TF_M_BOOT_SUFFIX_A35),\$(TF_M_BOOT_SUFFIX_A35),\$(TF_M_BOOT_SUFFIX_A35_\$(1)))${M33TD_FIWMWARE_DEPLOYED_TYPE}.${M33TD_FIWMWARE_SUFFIX}" ; \\
	fi;
	if [ -e "\$(SRC_PATH)/\$(4)/\$(1)-\$(2)-\$(3)/\`cat \$(SRC_PATH)/\$(4)/\$(1)-\$(2)-\$(3)/m33td_project_name\`${M33TD_FIWMWARE_TYPE_SIGNED}.${M33TD_FIWMWARE_SUFFIX}" ]; then \\
		cp -f "\$(SRC_PATH)/\$(4)/\$(1)-\$(2)-\$(3)/\`cat \$(SRC_PATH)/\$(4)/\$(1)-\$(2)-\$(3)/m33td_project_name\`${M33TD_FIWMWARE_TYPE_SIGNED}.${M33TD_FIWMWARE_SUFFIX}" \\
			"\$(DEPLOYDIR)/${M33TD_FIWMWARE_DEPLOYED_PREFIX}-\$(3)-\$(if \$(TF_M_BOOT_SUFFIX_M33),\$(TF_M_BOOT_SUFFIX_M33),\$(TF_M_BOOT_SUFFIX_M33_\$(1)))-\$(if \$(TF_M_BOOT_SUFFIX_A35),\$(TF_M_BOOT_SUFFIX_A35),\$(TF_M_BOOT_SUFFIX_A35_\$(1)))${M33TD_FIWMWARE_DEPLOYED_TYPE_SIGNED}.${M33TD_FIWMWARE_SUFFIX}" ; \\
	fi;
endef


# Configure overall deploy rules list
m33td-deploy :=\$(foreach config, \$(TF_M_CONFIG), \\
			\$(foreach plt, \$(if \$(TF_M_PLATFORM),\$(TF_M_PLATFORM),\$(TF_M_PLATFORM_\$(config))), \\
				\$(foreach dt, \$(if \$(TF_M_DEVICETREE), \$(TF_M_DEVICETREE),\$(TF_M_DEVICETREE_\$(config))), \\
					\$(if \$(findstring \$(shell echo \$(plt) | awk -F'/' '{print \$\$NF}' | tr '_' '-'),\$(dt)), \\
						deploy-\$(config)-\$(dt) \\
					,) \\
				) \\
			) \\
		)


# Set dependencies list for building all
DEPS = \$(m33td-deploy)

help: \$(tfm-check-config)
	@echo ""
	@echo "TF-M configuration:"
	@echo "  TF_M_CONFIG = \$(TF_M_CONFIG)"
	@echo "  TF_M_CONFIG = \$(TF_M_CONFIG)"
	@echo "Config details:"
	@\$(foreach config, \$(TF_M_CONFIG), \\
		\$(call tfm-configs,\$(config)) \\
		echo "    plateform optionflags:"; \\
		\$(foreach plt, \$(if \$(TF_M_PLATFORM),\$(TF_M_PLATFORM),\$(TF_M_PLATFORM_\$(config))), \\
			\$(call tfm-plt-configs,\$(shell echo \$(plt) | awk -F'/' '{print \$\$NF}')) \\
		) \\
	)
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
	@echo "M33TD_FIRMWARE_CONFIG folder configuration:"
	@echo "  SRC_PATH  = \$(SRC_PATH)"
	@echo "  DEPLOYDIR = \$(DEPLOYDIR)"
	@echo "  EXTDT_DIR = \$(EXTDT_DIR)"
	@echo "  TF_M_BUILD_PATH = \$(TF_M_BUILD_PATH)"
	@echo ""
	@echo "Available targets:"
	@echo "  all   : default target to build all binaries for defined config(s)"
	@echo "  m33td : build M33TD Starter App with CubeMp2 for defined config(s)"
	@echo "  clean : clean build directories from generated files"
	@echo ""
	@echo "M33TD_FIRMWARE_CONFIG"
	@echo "TF_M_CONFIG"
	@echo "TF_M_PLATFORM"
	@echo "TF_M_DEVICETREE"
	@echo "TF_M_BOOT_SUFFIX_A35"
	@echo "TF_M_BOOT_SUFFIX_M33"


all: \$(DEPS)

# generate M33TD rules by dt
\$(foreach config, \$(TF_M_CONFIG), \\
	\$(foreach plt, \$(if \$(TF_M_PLATFORM),\$(TF_M_PLATFORM),\$(TF_M_PLATFORM_\$(config))), \\
		\$(foreach dt, \$(if \$(TF_M_DEVICETREE), \$(TF_M_DEVICETREE),\$(TF_M_DEVICETREE_\$(config))), \\
			\$(if \$(findstring \$(shell echo \$(plt) | awk -F'/' '{print \$\$NF}' | tr '_' '-'),\$(dt)), \\
				\$(foreach prj, \$(if \$(M33TD_PRJ),\$(M33TD_PRJ),\$(M33TD_PROJECT_\$(shell echo \$(plt) | awk -F'/' '{print \$\$NF}'))), \\
					\$(eval \$(call m33td-starter-rules-by-dt,\$(config),\$(plt),\$(dt),\$(prj))) \\
				) \\
			,)\\
		)\\
	)\\
)


# generate deploy rules by dt
\$(foreach config, \$(TF_M_CONFIG), \\
	\$(foreach plt, \$(if \$(TF_M_PLATFORM),\$(TF_M_PLATFORM),\$(TF_M_PLATFORM_\$(config))), \\
		\$(foreach dt, \$(if \$(TF_M_DEVICETREE), \$(TF_M_DEVICETREE),\$(TF_M_DEVICETREE_\$(config))), \\
			\$(if \$(findstring \$(shell echo \$(plt) | awk -F'/' '{print \$\$NF}' | tr '_' '-'),\$(dt)), \\
				\$(foreach prj, \$(if \$(M33TD_PRJ),\$(M33TD_PRJ),\$(M33TD_PROJECT_\$(shell echo \$(plt) | awk -F'/' '{print \$\$NF}'))), \\
					\$(eval \$(call deploy-rules-by-dt,\$(config),\$(plt),\$(dt),\$(prj))) \\
				) \\
			,)\\
		)\\
	)\\
)

m33td: \$(m33td-deploy)

clean:
	@echo "Removing \$(BLD_PATH) ..."
	@rm -rf \$(BLD_PATH)
	@echo "Removing \$(DEPLOYDIR) ..."
	@rm -rf \$(DEPLOYDIR)
	@echo ""
EOF
}
addtask do_archiver_create_makefile_for_sdk before do_deploy_archives
