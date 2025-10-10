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

    cat << EOF > ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
# Set default path
SRC_PATH ?= \$(PWD)
BLD_PATH ?= \$(SRC_PATH)/../build
DEPLOYDIR ?= \$(SRC_PATH)/../deploy
FIPTOOLDIR ?= \$(SRC_PATH)/..

# Set default optee-os config
OPTEE_CONFIG ?= ${OPTEE_CONFIG}

# Set default FIP config
FIP_CONFIG ?= ${@' '.join(d for d in '${FIP_CONFIG}'.split() if not 'fastboot-' in d)}

# Default Optee-os overall settings to null
CFG_EMBED_DTB_SOURCE_FILE ?=
OPTEE_EXTRA_OPTFLAGS ?=

EXTDT_DIR_OPTEE ?= ${EXTDT_DIR_OPTEE}
EXTDT_DIR_OPTEE_SERIAL ?= ${EXTDT_DIR_OPTEE_SERIAL}

EOF
    unset i
    for config in ${OPTEE_CONFIG}; do
        i=$(expr $i + 1)
        extra_optflags=$(echo ${OPTEE_EXTRA_OPTFLAGS} | cut -d',' -f${i})
        if $(echo $config | grep -q "programmer") ; then
            extra_optflags_modified=$(echo ${extra_optflags} | sed 's|CFG_EXT_DTS='"${STAGING_EXTDT_DIR}/${EXTDT_DIR_OPTEE_SERIAL}"'|CFG_EXT_DTS=\$(EXTDT_DIR)/\$(EXTDT_DIR_OPTEE_SERIAL)|' )
        else
            extra_optflags_modified=$(echo ${extra_optflags} | sed 's|CFG_EXT_DTS='"${STAGING_EXTDT_DIR}/${EXTDT_DIR_OPTEE}"'|CFG_EXT_DTS=\$(EXTDT_DIR)/\$(EXTDT_DIR_OPTEE)|' )
        fi
        cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
OPTEE_DEVICETREE_INTERNAL_$config ?= $(echo ${OPTEE_DEVICETREE_INTERNAL} | cut -d',' -f${i} | tr ' ' '\n' | sort -u | tr '\n' ' ')
OPTEE_DEVICETREE_EXTERNAL_$config ?= $(echo ${OPTEE_DEVICETREE_EXTERNAL} | cut -d',' -f${i} | tr ' ' '\n' | sort -u | tr '\n' ' ')
OPTEE_DEVICETREE_$config = \$(OPTEE_DEVICETREE_INTERNAL_${config}) \$(if \$(EXTDT_DIR),\$(OPTEE_DEVICETREE_EXTERNAL_${config}))
OPTEE_EXTRA_OPTFLAGS_$config ?= ${extra_optflags_modified}
EOF
    done

cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}

# Remove default variables
LDFLAGS =
CFLAGS = \$(LIBGCC_LOCATE_CFLAGS)
CPPFLAGS =

# Define default make options
EXTRA_OEMAKE = $(echo "${EXTRA_OEMAKE}" | sed "s|LIBGCC_LOCATE_CFLAGS=[^ ]* |LIBGCC_LOCATE_CFLAGS=\$(KCFLAGS) |" | \
                sed 's,CFG_EXT_DTS='"${STAGING_EXTDT_DIR}/${EXTDT_DIR_OPTEE}"',CFG_EXT_DTS=\$(EXTDT_DIR)/\$(EXTDT_DIR_OPTEE),')

# support of TA_arm64 and TA_arm32
SDK_CROSS_COMPILE_TA_ARM32_TOOLCHAIN ?= \$(OECORE_NATIVE_SYSROOT)/usr/share/gcc-arm-none-eabi/bin/arm-none-eabi-
ifeq (\$(TA32_64),1)
EXTRA_OEMAKE += CROSS_COMPILE_ta_arm32=\${SDK_TA_ARM32_TOOLCHAIN} supported-ta-targets='ta_arm64 ta_arm32'
endif

# Display Optee-os config details (optee-configs <CONFIG>)
define optee-configs
	echo "  \$(1)" ; \\
	echo "    with device tree : \$(if \$(CFG_EMBED_DTB_SOURCE_FILE),\$(CFG_EMBED_DTB_SOURCE_FILE),\$(OPTEE_DEVICETREE_\$(1)))" ; \\
	echo "    extra optionflags: \$(if \$(OPTEE_EXTRA_OPTFLAGS),\$(OPTEE_EXTRA_OPTFLAGS),\$(OPTEE_EXTRA_OPTFLAGS_\$(1)))" ;
endef


# Configure Optee-os check configuration rules (check-config <CONFIG>)
#   chk-conf-<CONFIG>
define check-config
chk-conf-\$(1):
	@\$(foreach dt, \$(if \$(CFG_EMBED_DTB_SOURCE_FILE), \$(CFG_EMBED_DTB_SOURCE_FILE),\$(OPTEE_DEVICETREE_\$(1))), \\
		\$(foreach soc, ${STM32MP_SOC_NAME}, \\
			\$(if \$(findstring \$(soc),\$(dt)), \\
				mkdir -p \$(BLD_PATH)/\$(1)-\$(dt); \\
				touch \$(BLD_PATH)/\$(1)-\$(dt)/\$(dt)-chk-conf.txt ; \\
			,) \\
		) \\
		if ! [ -f \$(BLD_PATH)/\$(1)-\$(dt)/\$(dt)-chk-conf.txt ]; then \\
			echo "[CONFIG ERROR] Wrong device tree file name: \$(dt)"; \\
			echo "[CONFIG ERROR] It should contain the SOC pattern name (any of ${STM32MP_SOC_NAME})"; \\
			exit 1; \\
		fi; \\
		rm -f \$(BLD_PATH)/\$(1)-\$(dt)/\$(dt)-chk-conf.txt ; \\
	)
endef

# Configure Optee-os make rules (optee-rules-by-dt <CONFIG> <SOC> <DT>)
#   tee-<CONFIG>-<SOC>-<DT>: <DEPS>
define optee-rules-by-dt
tee-\$(1)-\$(2)-\$(3): chk-conf-\$(1)
	@mkdir -p \$(BLD_PATH)/\$(1)-\$(3); \\
	\$(MAKE) \$(EXTRA_OEMAKE) \\
		\$(OPTEE_EXTRA_OPTFLAGS_\$(1)) \\
		-C \$(SRC_PATH) PREFIX=\$(SDKTARGETSYSROOT) \\
		O=\$(BLD_PATH)/\$(1)-\$(3) \\
		CFG_EMBED_DTB_SOURCE_FILE=\$(3).dts || exit 1;
endef

# Configure Optee-os deploy rules (deploy-rules-by-dt <CONFIG> <SOC> <DT>)
#   deploy-<CONFIG>-<SOC>-<DT>: <DEPS>
define deploy-rules-by-dt
deploy-\$(1)-\$(2)-\$(3): tee-\$(1)-\$(2)-\$(3)
	@mkdir -p \$(DEPLOYDIR)
	@mkdir -p \$(DEPLOYDIR)/debug
	\$(if \$(findstring \$(2),\$(3)), \\
		cp -f \$(BLD_PATH)/\$(1)-\$(3)/core/tee-header_v2.bin \$(DEPLOYDIR)/tee-header_v2-\$(3)-\$(1).bin ; \\
		cp -f \$(BLD_PATH)/\$(1)-\$(3)/core/tee-pager_v2.bin \$(DEPLOYDIR)/tee-pager_v2-\$(3)-\$(1).bin ; \\
		cp -f \$(BLD_PATH)/\$(1)-\$(3)/core/tee-pageable_v2.bin \$(DEPLOYDIR)/tee-pageable_v2-\$(3)-\$(1).bin ; \\
		cp -f \$(BLD_PATH)/\$(1)-\$(3)/core/tee.elf \$(DEPLOYDIR)/debug/tee-\$(3)-\$(1).elf ; \\
	,)
endef

# Configure overall check config rules lits
optee-check-config := \$(foreach config, \$(OPTEE_CONFIG), chk-conf-\$(config))

# Configure overall deploy rules list
optee-deploy := \$(foreach config, \$(OPTEE_CONFIG), \\
					\$(foreach soc, ${STM32MP_SOC_NAME}, \\
						\$(foreach dt, \$(if \$(CFG_EMBED_DTB_SOURCE_FILE), \$(CFG_EMBED_DTB_SOURCE_FILE),\$(OPTEE_DEVICETREE_\$(config))), \\
							\$(if \$(findstring \$(soc),\$(dt)), deploy-\$(config)-\$(soc)-\$(dt), chk-conf-\$(config)) \\
						) \\
					) \\
				)

# Set dependencies list for building all
DEPS = \$(optee-deploy)
DEPS += fip

help: \$(optee-check-config)
	@echo ""
	@echo "OPTEE-OS configuration:"
	@echo "  OPTEE_CONFIG = \$(OPTEE_CONFIG)"
	@echo "Config details:"
	@\$(foreach config, \$(OPTEE_CONFIG), \$(call optee-configs,\$(config)))
	@echo ""
	@echo "Note that each Optee-os configuration settings can be updated through overall or specific config var:"
	@echo "  CFG_EMBED_DTB_SOURCE_FILE"
	@echo "  OPTEE_EXTRA_OPTFLAGS"
	@echo ""
	@echo "OPTEE-OS folder configuration:"
	@echo "  SRC_PATH  = \$(SRC_PATH)"
	@echo "  BLD_PATH  = \$(BLD_PATH)"
	@echo "  DEPLOYDIR = \$(DEPLOYDIR)"
	@echo "  EXTDT_DIR = \$(EXTDT_DIR)"
	@echo "Support of TA_arm64 and TA_arm32"
	@echo "  TA32_64   = \$(TA32_64)
	@echo ""
	@echo "FIP configuration:"
	@echo "  Do not forget to set FIP deploydir folders (such as FIP_DEPLOYDIR_ROOT) to provide path to needed binaries"
	@echo ""
	@echo "Available targets:"
	@echo "  all   : default target to build all binaries for defined config(s)"
	@echo "  fip   : build FIP binaries for defined config(s)"
	@echo "  optee : build OPTEE-OS binaries for defined config(s)"
	@echo "  clean : clean build directories from generated files"
	@echo ""

all: \$(DEPS)

# generate check config rules
\$(foreach config, \$(OPTEE_CONFIG), \$(eval \$(call check-config,\$(config))))

# generate optee rules by dt
\$(foreach config, \$(OPTEE_CONFIG), \\
	\$(foreach soc,  ${STM32MP_SOC_NAME} , \\
		\$(foreach dt, \$(if \$(CFG_EMBED_DTB_SOURCE_FILE), \$(CFG_EMBED_DTB_SOURCE_FILE),\$(OPTEE_DEVICETREE_\$(config))), \\
			\$(if \$(findstring \$(soc),\$(dt)), \\
				\$(eval \$(call optee-rules-by-dt,\$(config),\$(soc),\$(dt))) \\
			,)\\
		)\\
	)\\
)

# generate deploy rules by dt
\$(foreach config, \$(OPTEE_CONFIG), \\
	\$(foreach soc, ${STM32MP_SOC_NAME} , \\
		\$(foreach dt, \$(if \$(CFG_EMBED_DTB_SOURCE_FILE), \$(CFG_EMBED_DTB_SOURCE_FILE),\$(OPTEE_DEVICETREE_\$(config))), \\
			\$(if \$(findstring \$(soc),\$(dt)), \\
				\$(eval \$(call deploy-rules-by-dt,\$(config),\$(soc),\$(dt))) \\
			,)\\
		)\\
	)\\
)

fip: \$(optee-deploy)
	for fipconfig in \$(FIP_CONFIG) ; do \\
		FIP_DEPLOYDIR_OPTEE=\$(DEPLOYDIR) FIP_CONFIG="\$\$fipconfig" \$(if \$(CFG_EMBED_DTB_SOURCE_FILE),FIP_DEVICETREE="\$(CFG_EMBED_DTB_SOURCE_FILE)") \$(FIPTOOLDIR)/fiptool-stm32mp.${MACHINE} || exit 1;\\
	done

optee: \$(optee-deploy)

clean:
	@echo "Removing \$(BLD_PATH) ..."
	@rm -rf \$(BLD_PATH)
	@echo "Removing \$(DEPLOYDIR) ..."
	@rm -rf \$(DEPLOYDIR)
	@echo ""
EOF
}
do_ar_original[prefuncs] += "archiver_create_makefile_for_sdk"
