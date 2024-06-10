#
# Archiver Configuration
#
SRC_URI:append = " file://README.HOW_TO.txt "

inherit archiver
ARCHIVER_MODE[src] = "original"

inherit archiver_stm32mp_clean

archiver_create_makefile_for_sdk() {
    mkdir -p ${ARCHIVER_OUTDIR}
    cat << EOF > ${ARCHIVER_OUTDIR}/Makefile.sdk
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
            devicetree=$(echo ${UBOOT_DEVICETREE} | cut -d',' -f${i})
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
                            cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk
# Init default config settings
UBOOT_BINARY_${config} ?= ${binary}
UBOOT_DEVICETREE_${config} ?= ${devicetree}
UBOOT_DEFCONFIG_${type_suffix} += ${config}
EOF
                        fi
                    done
                    unset k
                fi
            done
            unset j
        done
        unset i
    cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk
UBOOT_CONFIGS = $(echo ${type_suffix_list} | tr ' ' '\n' | sort -u | tr '\n' ' ')
EOF
    fi
    cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk

# Remove default variables
LDFLAGS =
CFLAGS =
CPPFLAGS =

UBOOT_LOCALVERSION = ${UBOOT_LOCALVERSION}

# Define default external-dt options
EXTERNAL_DT_OPTS = ${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', 'EXT_DTS=\$(EXTDT_DIR)/${EXTDT_DIR_UBOOT}', '', d)}

# Display U-Boot config details
define uboot-configs
	echo "  \$(1)" ; \\
	\$(foreach defconfig, \$(if \$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG_\$(1))), \\
		echo "    defconfig      : \$(defconfig)" ; \\
		echo "    for binary     : \$(if \$(UBOOT_BINARY),\$(UBOOT_BINARY),\$(UBOOT_BINARY_\$(defconfig)))" ; \\
		echo "    with devicetree: \$(if \$(DEVICETREE),\$(DEVICETREE),\$(UBOOT_DEVICETREE_\$(defconfig)))" ; \\
	)
endef

# Configure U-Boot configure rules
#   configure-DEFCONFIG <defconfig> <config>
define configure-rules
configure-\$(1):: version
	@mkdir -p \$(BLD_PATH)/\$(1)
	@echo \$(UBOOT_LOCALVERSION) > \$(BLD_PATH)/\$(1)/.scmversion
	\$(MAKE) -C \$(SRC_PATH) O=\$(BLD_PATH)/\$(1) \$(1) || exit 1
endef

# Configure U-Boot make rules
#   uboot-DEFCONFIG <defconfig> <config> <binary>
define uboot-rules
uboot-\$(1):: configure-\$(1)
	@mkdir -p \$(DEPLOYDIR)
	@\$(foreach dt, \$(if \$(DEVICETREE),\$(DEVICETREE),\$(UBOOT_DEVICETREE_\$(1))), \\
		\$(MAKE) -C \$(SRC_PATH) \$(EXTERNAL_DT_OPTS) \\
			O=\$(BLD_PATH)/\$(1) \\
			DEVICE_TREE=\$(dt) \\
			DEVICE_TREE_EXT=\$(dt).dtb || exit 1 ; \\
		cp -f \$(BLD_PATH)/\$(1)/\$(3) \$(DEPLOYDIR)/u-boot-\$(dt).\$(shell echo \$(3) | cut -d'.' -f2) || exit 1 ; \\
		if [ -f \$(BLD_PATH)/\$(1)/\$(shell echo \$(3) | cut -d'.' -f1).stm32 ]; then \\
			cp -f \$(BLD_PATH)/\$(1)/\$(shell echo \$(3) | cut -d'.' -f1).stm32 \$(DEPLOYDIR)/u-boot-\$(dt).stm32 ; \\
		fi ; \\
	)
endef

# Configure U-Boot deploy rules
#   deploy-DEFCONFIG <defconfig> <config> <binary>
define deploy-rules
deploy-\$(1):: uboot-\$(1)
	@mkdir -p \$(DEPLOYDIR)
	@mkdir -p \$(DEPLOYDIR)/debug
	@\$(foreach dt, \$(if \$(DEVICETREE),\$(DEVICETREE),\$(UBOOT_DEVICETREE_\$(1))), \\
		if [ "\$(shell echo \$(3) | cut -d'.' -f2)" = "dtb" ]; then \\
			cp -f \$(BLD_PATH)/\$(1)/u-boot-nodtb.bin \$(DEPLOYDIR)/u-boot-nodtb\$(strip \$(foreach soc,${STM32MP_SOC_NAME},\$(if \$(findstring \$(soc),\$(dt)),-\$(soc),)))-\$(2).bin ; \\
		fi ; \\
		if [ -f \$(BLD_PATH)/\$(1)/${UBOOT_ELF} ]; then \\
			cp -f \$(BLD_PATH)/\$(1)/${UBOOT_ELF} \$(DEPLOYDIR)/debug/u-boot\$(strip \$(foreach soc,${STM32MP_SOC_NAME},\$(if \$(findstring \$(soc),\$(dt)),-\$(soc),)))-\$(2).${UBOOT_ELF_SUFFIX} ; \\
		fi ; \\
	)
endef

# Configure overall deploy rules list
deploy-targets := \$(foreach config, \$(if \$(UBOOT_CONFIG),\$(UBOOT_CONFIG),\$(UBOOT_CONFIGS)), \\
					\$(foreach defconfig, \$(if \$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG_\$(config))), deploy-\$(defconfig)) \\
					)

# Set dependencies list for building all
DEPS = \$(deploy-targets)
DEPS += fip

help:
	@echo
	@echo "U-Boot configuration:"
	@echo "  UBOOT_CONFIG = \$(if \$(UBOOT_CONFIG),\$(UBOOT_CONFIG),\$(UBOOT_CONFIGS))"
	@echo "Config details:"
	@\$(foreach config, \$(if \$(UBOOT_CONFIG),\$(UBOOT_CONFIG),\$(UBOOT_CONFIGS)), \$(call uboot-configs,\$(config)))
	@echo
	@echo "Note that each U-Boot configuration settings can be updated through overall or specific config var:"
	@echo "  UBOOT_DEFCONFIG"
	@echo "  UBOOT_BINARY"
	@echo "  DEVICETREE"
	@echo
	@echo "U-Boot folder configuration:"
	@echo "  SRC_PATH  = \$(SRC_PATH)"
	@echo "  BLD_PATH  = \$(BLD_PATH)"
	@echo "  DEPLOYDIR = \$(DEPLOYDIR)"
	@echo
	@echo "FIP configuration:"
	@echo "  Do not forget to set FIP deploydir folders (such as FIP_DEPLOYDIR_ROOT) to provide path to needed binaries"
	@echo
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
	@echo

fip: \$(deploy-targets)
	for fipconfig in \$(FIP_CONFIG) ; do \\
		FIP_DEPLOYDIR_UBOOT=\$(DEPLOYDIR) FIP_CONFIG="\$\$fipconfig" \$(if \$(DEVICETREE),FIP_DEVICETREE="\$(DEVICETREE)") \$(FIPTOOLDIR)/fiptool-stm32mp || exit 1; \\
	done

version:
	@if test ! -e \$(SRC_PATH)/.scmversion ; then echo \$(UBOOT_LOCALVERSION) > \$(SRC_PATH)/.scmversion; fi

# Set U-Boot configure rules
\$(foreach config, \$(if \$(UBOOT_CONFIG),\$(UBOOT_CONFIG),\$(UBOOT_CONFIGS)), \\
	\$(foreach defconfig, \$(if \$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG_\$(config))), \$(eval \$(call configure-rules,\$(defconfig),\$(config)))))
# Set U-Boot make rules
\$(foreach config, \$(if \$(UBOOT_CONFIG),\$(UBOOT_CONFIG),\$(UBOOT_CONFIGS)), \\
	\$(foreach defconfig, \$(if \$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG_\$(config))), \\
		\$(eval \$(call uboot-rules,\$(defconfig),\$(config),\$(if \$(UBOOT_BINARY),\$(UBOOT_BINARY),\$(UBOOT_BINARY_\$(defconfig)))))))
# Set U-Boot deploy rules
\$(foreach config, \$(if \$(UBOOT_CONFIG),\$(UBOOT_CONFIG),\$(UBOOT_CONFIGS)), \\
	\$(foreach defconfig, \$(if \$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG_\$(config))), \\
		\$(eval \$(call deploy-rules,\$(defconfig),\$(config),\$(if \$(UBOOT_BINARY),\$(UBOOT_BINARY),\$(UBOOT_BINARY_\$(defconfig)))))))

EOF
}
do_ar_original[prefuncs] += "archiver_create_makefile_for_sdk"
