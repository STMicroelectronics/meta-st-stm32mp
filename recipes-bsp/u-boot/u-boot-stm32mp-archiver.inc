#
# Archiver Configuration
#
SRC_URI_append = " file://README.HOW_TO.txt "

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

# Default U-Boot overall settings to null
UBOOT_CONFIG ?=
UBOOT_DEFCONFIG ?=
UBOOT_BINARY ?=
UBOOT_DEVICETREE ?=

EOF
    if [ -n "${UBOOT_CONFIG}" ]; then
        unset i j k
        for config in ${UBOOT_MACHINE}; do
            i=$(expr $i + 1);
            for type in ${UBOOT_CONFIG}; do
                j=$(expr $j + 1);
                if [ $j -eq $i ]; then
                    for binary in ${UBOOT_BINARIES}; do
                        k=$(expr $k + 1);
                        if [ $k -eq $i ]; then
                            type_suffix=$(echo ${type} | cut -d'_' -f1)
                            type_filter=$(echo ${type} | cut -d'_' -f2)
                            [ "${type_suffix}" = "${type_filter}" ] && type_filter=""
                            if [ -z "${type_filter}" ]; then
                                devicetree="${UBOOT_DEVICETREE}"
                            else
                                devicetree=""
                                for dt in ${UBOOT_DEVICETREE}; do
                                    [ -z "$(echo ${dt} | grep ${type_filter})" ] || devicetree="${devicetree} ${dt}"
                                done
                            fi
                            cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk
# Init default config settings
UBOOT_CONFIGS += ${type_suffix}
UBOOT_DEFCONFIG_$type_suffix += ${config}
UBOOT_BINARY_$config ?= ${binary}
UBOOT_DEVICETREE_$config ?= ${devicetree}
EOF
                        fi
                    done
                    unset k
                fi
            done
            unset j
        done
        unset i
    fi
    cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk

# Remove default variables
LDFLAGS =
CFLAGS =
CPPFLAGS =

UBOOT_LOCALVERSION = ${UBOOT_LOCALVERSION}

# Configure default fip feature
ENABLE_FIP ?= ${@bb.utils.contains('MACHINE_FEATURES','fip','1','',d)}

# Display U-Boot config details
define uboot-configs
	echo "  \$(1)" ; \\
	\$(foreach defconfig, \$(if \$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG_\$(1))), \\
		echo "    defconfig      : \$(defconfig)" ; \
		echo "    for binary     : \$(if \$(UBOOT_BINARY),\$(UBOOT_BINARY),\$(UBOOT_BINARY_\$(defconfig)))" ; \
		echo "    with devicetree: \$(if \$(DEVICETREE),\$(DEVICETREE),\$(UBOOT_DEVICETREE_\$(defconfig)))" ; \
	)
endef

# Configure U-Boot configure rules (configure-DEFCONFIG-CONFIG)
define configure-rules
configure-\$(1)-\$(2): version
ifeq (\$(ENABLE_FIP),0)
	@if ! grep -q 'CONFIG_STM32MP15x_STM32IMAGE=y' \$(SRC_PATH)/configs/\$(1); then \\
		echo "CONFIG_STM32MP15x_STM32IMAGE=y" >> \$(SRC_PATH)/configs/\$(1) ; \\
	fi
endif
	@mkdir -p \$(BLD_PATH)/\$(1)
	@echo \$(UBOOT_LOCALVERSION) > \$(BLD_PATH)/\$(1)/.scmversion
	\$(MAKE) -C \$(SRC_PATH) O=\$(BLD_PATH)/\$(1) \$(1)
endef

# Configure U-Boot make rules (uboot-DEFCONFIG-CONFIG)
define uboot-rules
uboot-\$(1)-\$(2): configure-\$(1)-\$(2)
	@mkdir -p \$(DEPLOYDIR)
	@\$(foreach dt, \$(if \$(DEVICETREE),\$(DEVICETREE),\$(UBOOT_DEVICETREE_\$(1))), \\
		\$(MAKE) -C \$(SRC_PATH) ${UBOOT_MAKE_TARGET} \\
			O=\$(BLD_PATH)/\$(1) \\
			DEVICE_TREE=\$(dt) \\
			DEVICE_TREE_EXT=\$(dt).dtb ; \\
		cp -f \$(BLD_PATH)/\$(1)/\$(3) \$(DEPLOYDIR)/u-boot-\$(dt)-\$(2).\$(shell echo \$(3) | cut -d'.' -f2) ; \\
		if [ -f \$(BLD_PATH)/\$(1)/\$(shell echo \$(3) | cut -d'.' -f1).stm32 ]; then \\
			cp -f \$(BLD_PATH)/\$(1)/\$(shell echo \$(3) | cut -d'.' -f1).stm32 \$(DEPLOYDIR)/u-boot-\$(dt)-\$(2).stm32 ; \\
		fi ; \\
		if [ -f \$(BLD_PATH)/\$(1)/${SPL_BINARY_STM32} ]; then \\
			cp -f \$(BLD_PATH)/\$(1)/${SPL_BINARY_STM32} \$(DEPLOYDIR)/${SPL_BINARYNAME}-\$(dt)-\$(2) ; \\
		fi ; \\
	)
endef

# Configure U-Boot deploy rules (deploy-DEFCONFIG-CONFIG)
define deploy-rules
deploy-\$(1)-\$(2): uboot-\$(1)-\$(2)
	@mkdir -p \$(DEPLOYDIR)
	@mkdir -p \$(DEPLOYDIR)/debug
	@\$(foreach dt, \$(if \$(DEVICETREE),\$(DEVICETREE),\$(UBOOT_DEVICETREE_\$(1))), \\
		if [ "\$(shell echo \$(3) | cut -d'.' -f2)" = "dtb" ]; then \\
			cp -f \$(BLD_PATH)/\$(1)/u-boot-nodtb.bin \$(DEPLOYDIR)/u-boot-nodtb\$(strip \$(foreach soc,${STM32MP_SOC_NAME},\$(if \$(findstring \$(soc),\$(dt)),-\$(soc),))).bin ; \\
		fi ; \\
		if [ -f \$(BLD_PATH)/\$(1)/${UBOOT_ELF} ]; then \\
			cp -f \$(BLD_PATH)/\$(1)/${UBOOT_ELF} \$(DEPLOYDIR)/debug/u-boot\$(strip \$(foreach soc,${STM32MP_SOC_NAME},\$(if \$(findstring \$(soc),\$(dt)),-\$(soc),)))-\$(2).${UBOOT_ELF_SUFFIX} ; \\
		fi ; \\
		if [ -f \$(BLD_PATH)/\$(1)/${SPL_ELF} ]; then \\
			cp -f \$(BLD_PATH)/\$(1)/${SPL_ELF} \$(DEPLOYDIR)/debug/${SPL_ELF_NAME}\$(strip \$(foreach soc,${STM32MP_SOC_NAME},\$(if \$(findstring \$(soc),\$(dt)),-\$(soc),)))-\$(2) ; \\
		fi ; \\
	)
endef

# Configure overall deploy rules list
deploy-targets := \$(foreach config, \$(if \$(UBOOT_CONFIG),\$(UBOOT_CONFIG),\$(UBOOT_CONFIGS)), \\
					\$(foreach defconfig, \$(if \$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG),\$(UBOOT_DEFCONFIG_\$(config))), deploy-\$(defconfig)-\$(config)) \\
					)

# Set dependencies list for building all
DEPS = \$(deploy-targets)
ifeq (\$(ENABLE_FIP), 1)
DEPS += fip
endif

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
	@echo "  ENABLE_FIP = \$(ENABLE_FIP) ('1' to generate fip binary)"
ifeq (\$(ENABLE_FIP),1)
	@echo "  Do not forget to set FIP deploydir folders (such as FIP_DEPLOYDIR_ROOT) to provide path to needed binaries"
endif
	@echo
	@echo "Available targets:"
	@echo "  all   : build U-Boot binaries for defined config(s)"
	@echo "  fip   : build FIP binaries"
	@echo "  clean : clean build directories from generated files"

all: \$(DEPS)

clean:
	@for config in \$(UBOOT_CONFIGS); do \\
		uboot_type=\$\$(echo \$\$config | cut -d',' -f2) ; \\
		echo "Removing \$(BLD_PATH)/\$\$uboot_type ..." ; \\
		rm -rf \$(BLD_PATH)/\$\$uboot_type ; \\
	done
	@echo "Removing \$(DEPLOYDIR) ..."
	@rm -rf \$(DEPLOYDIR)
	@echo

fip: \$(deploy-targets)
	FIP_DEPLOYDIR_UBOOT=\$(DEPLOYDIR) FIP_DEVICETREE="\$(DEVICETREE)" fiptool-stm32mp

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
