#
# Archiver Configuration
#
SRC_URI:append = " file://README.HOW_TO.txt "

inherit archiver
ARCHIVER_MODE[src] = "original"

inherit archiver_stm32mp_clean

archiver_create_makefile_for_sdk() {
    mkdir -p ${ARCHIVER_OUTDIR}
    cat << EOF > ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
# Set default path
SRC_PATH ?= \$(PWD)
BLD_PATH ?= \$(SRC_PATH)/../build
DEPLOYDIR ?= \$(SRC_PATH)/../deploy
FIPTOOLDIR ?= \$(SRC_PATH)/..

# Default U-Boot overall settings to null
UBOOT_CONFIG ?=
UBOOT_DEFCONFIG ?=
UBOOT_BINARY ?=
UBOOT_DEVICETREE ?=

EXTDT_DIR_UBOOT ?= ${EXTDT_DIR_UBOOT}
EXTDT_DIR_UBOOT_SERIAL ?= ${EXTDT_DIR_UBOOT_SERIAL}

# Set default FIP config
FIP_CONFIG ?= ${@' '.join(d for d in '${FIP_CONFIG}'.split() if not 'fastboot-' in d)}

EOF
    if [ -n "${UBOOT_CONFIG}" ]; then
        # Initialize type suffix list
        type_suffix_list=""
        unset i j k
        for config in ${UBOOT_MACHINE}; do
            i=$(expr $i + 1);
            # Initialize devicetree list
            devicetree_internal=$(echo ${UBOOT_DEVICETREE_INTERNAL} | cut -d',' -f${i})
            devicetree_external=$(echo ${UBOOT_DEVICETREE_EXTERNAL} | cut -d',' -f${i})
            make_opt=$(echo ${UBOOT_MAKE_OPT} | cut -d',' -f${i})
            for type in ${UBOOT_CONFIG}; do
                j=$(expr $j + 1);
                if [ $j -eq $i ]; then
                    for binary in ${UBOOT_BINARIES}; do
                        k=$(expr $k + 1);
                        if [ $k -eq $i ]; then
                            # Skip any fastboot config
                            echo ${type} | grep '^fastboot-' && continue
                            type_suffix=$(echo ${type} | cut -d'_' -f1)
                            type_suffix_list="${type_suffix_list} ${type_suffix}"
                            if $(echo ${type} | grep -q "fastboot\|programmer") ; then
                                opt=$(echo ${make_opt} | sed 's|EXT_DTS='"${STAGING_EXTDT_DIR}/${EXTDT_DIR_UBOOT_SERIAL}"'|EXT_DTS=\$(EXTDT_DIR)/\$(EXTDT_DIR_UBOOT_SERIAL)|' )
                            else
                                opt=$(echo ${make_opt} | sed 's|EXT_DTS='"${STAGING_EXTDT_DIR}/${EXTDT_DIR_UBOOT}"'|EXT_DTS=\$(EXTDT_DIR)/\$(EXTDT_DIR_UBOOT)|' )
                            fi
                            cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
# Init default config settings
UBOOT_BINARY_${type_suffix}_${config} ?= ${binary}
UBOOT_DEVICETREE_INTERNAL_${type_suffix}_${config} ?= ${devicetree_internal}
UBOOT_DEVICETREE_EXTERNAL_${type_suffix}_${config} ?= ${devicetree_external}
UBOOT_DEVICETREE_${type_suffix}_${config} = \$(UBOOT_DEVICETREE_INTERNAL_${type_suffix}_${config}) \$(if \$(EXTDT_DIR),\$(UBOOT_DEVICETREE_EXTERNAL_${type_suffix}_${config}))
UBOOT_DEFCONFIG_${type_suffix} += ${config}
UBOOT_MAKE_OPTS_${type_suffix}_${config} = \$(if \$(EXTDT_DIR),${opt})

EOF
                        fi
                    done
                    unset k
                fi
            done
            unset j
        done
        unset i
    cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
UBOOT_CONFIGS = $(echo ${type_suffix_list} | tr ' ' '\n' | sort -u | tr '\n' ' ')
EOF
    fi
    cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}

# Remove default variables
LDFLAGS =
CFLAGS =
CPPFLAGS =

UBOOT_LOCALVERSION = ${UBOOT_LOCALVERSION}

# Display U-Boot config details (uboot-configs <config>)
define uboot-configs
	echo "  \$(1)" ; \\
	\$(foreach defconfig, \$(if \$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG_\$(1))), \\
		echo "    defconfig      : \$(defconfig)" ; \\
		echo "    for binary     : \$(if \$(UBOOT_BINARY),\$(UBOOT_BINARY),\$(UBOOT_BINARY_\$(1)_\$(defconfig)))" ; \\
		echo "    with devicetree: \$(if \$(DEVICE_TREE),\$(DEVICE_TREE),\$(UBOOT_DEVICETREE_\$(1)_\$(defconfig)))" ; \\
	)
endef

# Configure U-Boot check configuration rules (check-config <defconfig> <config>)
#   chk-conf-<defconfig>
define check-config
chk-conf-\$(1)::
	@\$(foreach dt, \$(if \$(DEVICE_TREE),\$(DEVICE_TREE),\$(UBOOT_DEVICETREE_\$(2)_\$(1))), \\
		\$(foreach soc, ${STM32MP_SOC_NAME}, \\
			\$(if \$(findstring \$(soc),\$(dt)), \\
				mkdir -p \$(BLD_PATH)/\$(1); \\
				touch \$(BLD_PATH)/\$(1)/\$(dt)-chk-conf.txt ; \\
			,) \\
		) \\
		if ! [ -f \$(BLD_PATH)/\$(1)/\$(dt)-chk-conf.txt ]; then \\
			echo "[CONFIG ERROR] Wrong device tree file name: \$(dt)"; \\
			echo "[CONFIG ERROR] It should contain the SOC pattern name (any of ${STM32MP_SOC_NAME})"; \\
			exit 1; \\
		fi; \\
		rm -f \$(BLD_PATH)/\$(1)/\$(dt)-chk-conf.txt ; \\
	)
endef

# Configure U-Boot configure rules (configure-rules <defconfig> <config>)
#   configure-<defconfig>
define configure-rules
configure-\$(1):: version
	@mkdir -p \$(BLD_PATH)/\$(1)
	@echo \$(UBOOT_LOCALVERSION) > \$(BLD_PATH)/\$(1)/.scmversion
	\$(MAKE) -C \$(SRC_PATH) O=\$(BLD_PATH)/\$(1) \$(1) || exit 1
endef

# Configure U-Boot make rules (uboot-rules <defconfig> <config> <binary>)
#   uboot-<defconfig>
define uboot-rules
uboot-\$(1):: configure-\$(1) chk-conf-\$(1)
	@mkdir -p \$(DEPLOYDIR)
	@\$(foreach dt, \$(if \$(DEVICE_TREE),\$(DEVICE_TREE),\$(UBOOT_DEVICETREE_\$(2)_\$(1))), \\
		\$(MAKE) -C \$(SRC_PATH) \\
			\$(UBOOT_MAKE_OPTS_\$(2)_\$(1)) \\
			O=\$(BLD_PATH)/\$(1) \\
			DEVICE_TREE=\$(dt) \\
			DEVICE_TREE_EXT=\$(dt).dtb || exit 1 ; \\
		cp -f \$(BLD_PATH)/\$(1)/\$(3) \$(BLD_PATH)/\$(1)/u-boot-\$(dt)-\$(2).\$(shell echo \$(3) | cut -d'.' -f2) || exit 1 ; \\
	)
endef

# Configure U-Boot deploy rules (deploy-rules <defconfig> <config> <binary>)
#   deploy-<defconfig>
define deploy-rules
deploy-\$(1):: uboot-\$(1)
	@mkdir -p \$(DEPLOYDIR)
	@mkdir -p \$(DEPLOYDIR)/debug
	@\$(foreach dt, \$(if \$(DEVICE_TREE),\$(DEVICE_TREE),\$(UBOOT_DEVICETREE_\$(2)_\$(1))), \\
		if [ "\$(shell echo \$(3) | cut -d'.' -f2)" = "dtb" ]; then \\
			cp -f \$(BLD_PATH)/\$(1)/u-boot-\$(dt)-\$(2).\$(shell echo \$(3) | cut -d'.' -f2) \$(DEPLOYDIR) || exit 1 ; \\
			cp -f \$(BLD_PATH)/\$(1)/u-boot-nodtb.bin \$(DEPLOYDIR)/u-boot-nodtb\$(strip \$(foreach soc,${STM32MP_SOC_NAME},\$(if \$(findstring \$(soc),\$(dt)),-\$(soc),)))-\$(2).bin || exit 1 ; \\
		fi ; \\
		if [ -f \$(BLD_PATH)/\$(1)/${UBOOT_ELF} ]; then \\
			cp -f \$(BLD_PATH)/\$(1)/${UBOOT_ELF} \$(DEPLOYDIR)/debug/u-boot\$(strip \$(foreach soc,${STM32MP_SOC_NAME},\$(if \$(findstring \$(soc),\$(dt)),-\$(soc),)))-\$(2).${UBOOT_ELF_SUFFIX} ; \\
		fi ; \\
	)
endef

# Configure overall check config rules lits
uboot-check-config := \$(foreach config, \$(if \$(UBOOT_CONFIG),\$(UBOOT_CONFIG),\$(UBOOT_CONFIGS)), \\
							\$(foreach defconfig, \$(if \$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG_\$(config))), chk-conf-\$(defconfig)) \\
						)

# Configure overall deploy rules list
deploy-targets := \$(foreach config, \$(if \$(UBOOT_CONFIG),\$(UBOOT_CONFIG),\$(UBOOT_CONFIGS)), \\
						\$(foreach defconfig, \$(if \$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG_\$(config))), deploy-\$(defconfig)) \\
					)

# Set dependencies list for building all
DEPS = \$(deploy-targets)
DEPS += fip

help: \$(uboot-check-config)
	@echo ""
	@echo "U-Boot configuration:"
	@echo "  UBOOT_CONFIG = \$(if \$(UBOOT_CONFIG),\$(UBOOT_CONFIG),\$(UBOOT_CONFIGS))"
	@echo "Config details:"
	@\$(foreach config, \$(if \$(UBOOT_CONFIG),\$(UBOOT_CONFIG),\$(UBOOT_CONFIGS)), \$(call uboot-configs,\$(config)))
	@echo ""
	@echo "Note that each U-Boot configuration settings can be updated through overall or specific config var:"
	@echo "  UBOOT_DEFCONFIG"
	@echo "  UBOOT_BINARY"
	@echo "  DEVICE_TREE"
	@echo ""
	@echo "U-Boot folder configuration:"
	@echo "  SRC_PATH  = \$(SRC_PATH)"
	@echo "  BLD_PATH  = \$(BLD_PATH)"
	@echo "  DEPLOYDIR = \$(DEPLOYDIR)"
	@echo ""
	@echo "FIP configuration:"
	@echo "  Do not forget to set FIP deploydir folders (such as FIP_DEPLOYDIR_ROOT) to provide path to needed binaries"
	@echo ""
	@echo "Available targets:"
	@echo "  all   : build U-Boot binaries for defined config(s) + fip"
	@echo "  uboot : build U-Boot binaries for defined config(s)"
	@echo "  fip   : build FIP binaries"
	@echo "  clean : clean build directories from generated files"

all: \$(DEPS)

uboot: \$(deploy-targets)

clean:
	@echo "Removing \$(BLD_PATH)/* ..."
	@rm -rf \$(BLD_PATH)/*
	@echo "Removing \$(DEPLOYDIR) ..."
	@rm -rf \$(DEPLOYDIR)
	@echo ""

fip: \$(deploy-targets)
	for fipconfig in \$(FIP_CONFIG) ; do \\
		FIP_DEPLOYDIR_UBOOT=\$(DEPLOYDIR) FIP_CONFIG="\$\$fipconfig" \$(if \$(DEVICE_TREE),FIP_DEVICETREE="\$(DEVICE_TREE)") \$(FIPTOOLDIR)/fiptool-stm32mp.${MACHINE} || exit 1; \\
	done

version:
	@if test ! -e \$(SRC_PATH)/.scmversion ; then echo \$(UBOOT_LOCALVERSION) > \$(SRC_PATH)/.scmversion; fi

# Set U-boot check config rules
\$(foreach config, \$(if \$(UBOOT_CONFIG),\$(UBOOT_CONFIG),\$(UBOOT_CONFIGS)), \\
	\$(foreach defconfig, \$(if \$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG_\$(config))), \\
		\$(eval \$(call check-config,\$(defconfig),\$(config))) \\
	) \\
)
# Set U-Boot configure rules
\$(foreach config, \$(if \$(UBOOT_CONFIG),\$(UBOOT_CONFIG),\$(UBOOT_CONFIGS)), \\
	\$(foreach defconfig, \$(if \$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG_\$(config))), \$(eval \$(call configure-rules,\$(defconfig),\$(config)))))
# Set U-Boot make rules
\$(foreach config, \$(if \$(UBOOT_CONFIG),\$(UBOOT_CONFIG),\$(UBOOT_CONFIGS)), \\
	\$(foreach defconfig, \$(if \$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG_\$(config))), \\
		\$(eval \$(call uboot-rules,\$(defconfig),\$(config),\$(if \$(UBOOT_BINARY),\$(UBOOT_BINARY),\$(UBOOT_BINARY_\$(config)_\$(defconfig)))))))
# Set U-Boot deploy rules
\$(foreach config, \$(if \$(UBOOT_CONFIG),\$(UBOOT_CONFIG),\$(UBOOT_CONFIGS)), \\
	\$(foreach defconfig, \$(if \$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG_\$(config))), \\
		\$(eval \$(call deploy-rules,\$(defconfig),\$(config),\$(if \$(UBOOT_BINARY),\$(UBOOT_BINARY),\$(UBOOT_BINARY_\$(config)_\$(defconfig)))))))

EOF
}
do_ar_original[prefuncs] += "archiver_create_makefile_for_sdk"
