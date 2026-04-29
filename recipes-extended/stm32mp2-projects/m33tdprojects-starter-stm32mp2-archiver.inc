#
# Archiver Configuration
#
SRC_URI:append = " file://README.HOW_TO.txt "

M33TD_STARTER_ARCHIVER ?= "${@bb.utils.contains('MACHINE_FEATURES', 'm33td', 'archiver', '', d)}"
inherit_defer ${M33TD_STARTER_ARCHIVER}
ARCHIVER_MODE[src] = "original"
COPYLEFT_LICENSE_INCLUDE:append = " BSD-3* "

inherit archiver_stm32mp_clean

do_archiver_create_makefile_for_sdk() {
    mkdir -p ${ARCHIVER_OUTDIR}
    cat << EOF > ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
# Set default path
SRC_PATH ?= \$(PWD)
BLD_PATH ?= \$(SRC_PATH)/../build
DEPLOYDIR ?= \$(SRC_PATH)/../deploy
M33FWTOOLDIR ?= \$(SRC_PATH)/..

# Set default M33FW config
M33FW_CONFIG ?= ${M33FW_CONFIG}
M33FW_BOARDS ?= ${M33FW_BOARDS}
M33FW_BUILDCONF ?= ${M33FW_BUILDCONF}

# Default M33FW overall settings
M33FW_DEVICETREE ?=
M33FW_BOOT_SUFFIX_M33 ?=
M33FW_BOOT_SUFFIX_A35 ?=
M33FW_EXTRA_OPTFLAGS ?=
M33FW_PROJECT_BUILDCONF ?=
M33FW_TYPE ?= ${M33TD_FW_TYPE}

# Set default M33PROJECT config
M33FW_PROJECT_PATH ?=
M33FW_PROJECT_NAME ?=

# Init default config settings
EOF
    unset l
    for bldconf in ${M33FW_BUILDCONF}; do
        l=$(expr $l + 1)
        cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
M33FW_EXTRA_OPTFLAGS_$bldconf ?= $(echo ${M33FW_EXTRA_OPTFLAGS} | cut -d',' -f${l})
EOF
    done
    unset i
    for config in ${M33FW_CONFIG}; do
        i=$(expr $i + 1)
        cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
M33FW_DEVICETREE_INTERNAL_$config ?= $(echo ${M33FW_DEVICETREE_INTERNAL} | cut -d',' -f${i})
M33FW_DEVICETREE_EXTERNAL_$config ?= $(echo ${M33FW_DEVICETREE_EXTERNAL} | cut -d',' -f${i})
M33FW_DEVICETREE_$config = \$(M33FW_DEVICETREE_INTERNAL_${config}) \$(if \$(EXTDT_DIR),\$(M33FW_DEVICETREE_EXTERNAL_${config}))
M33FW_BOOT_SUFFIX_M33_$config ?= $(echo ${M33FW_BOOT_SUFFIX_M33} | cut -d',' -f${i})
M33FW_BOOT_SUFFIX_A35_$config ?= $(echo ${M33FW_BOOT_SUFFIX_A35} | cut -d',' -f${i})
EOF
    done
    unset k
    for board in ${M33FW_BOARDS}; do
        k=$(expr $k + 1)
        cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}
M33FW_PROJECT_NAME_$board ?= $(echo ${M33FW_PROJECT_NAME} | cut -d',' -f${k} | cut -d':' -f1)
M33FW_PROJECT_PATH_$board ?= $(echo ${M33FW_PROJECT_PATH} | cut -d',' -f${k} | cut -d':' -f1)
M33FW_PROJECT_BUILDCONF_$board ?= $(echo ${M33FW_PROJECT_BUILDCONF} | cut -d',' -f${k} | tr ':' ' ')
EOF
    done
    cat << EOF >> ${ARCHIVER_OUTDIR}/Makefile.sdk.${MACHINE}

# Append toolchain path for SDK
PATH += :\$(OECORE_NATIVE_SYSROOT)/usr/share/gcc-arm-none-eabi/bin
# Unset CMAKE_TOOLCHAIN_FILE for SDK
CMAKE_TOOLCHAIN_FILE =

# Check for device tree list (dt-check)
define dt-check
	mkdir -p \$(BLD_PATH) ; \\
	rm -f \$(BLD_PATH)/dt-check ; \\
	\$(foreach config, \$(M33FW_CONFIG), \\
		if [ -n "\$(strip \$(M33FW_DEVICETREE))" ] || [ -n "\$(strip \$(M33FW_DEVICETREE_\$(config)))" ]; then \\
			echo "\$(if \$(M33FW_DEVICETREE),\$(M33FW_DEVICETREE),\$(M33FW_DEVICETREE_\$(config)))" > \$(BLD_PATH)/dt-check ; \\
		fi ; \\
	)
endef

# Display M33FW config details (m33td-configs <CONFIG>)
define m33td-configs
	echo "  \$(1)" ; \\
	echo "    with device tree     : \$(if \$(M33FW_DEVICETREE),\$(M33FW_DEVICETREE),\$(M33FW_DEVICETREE_\$(1)))" ; \\
	echo "    bootdevice m33 suffix: \$(if \$(M33FW_BOOT_SUFFIX_M33),\$(M33FW_BOOT_SUFFIX_M33),\$(M33FW_BOOT_SUFFIX_M33_\$(1)))" ; \\
	echo "    bootdevice a35 suffix: \$(if \$(M33FW_BOOT_SUFFIX_A35),\$(M33FW_BOOT_SUFFIX_A35),\$(M33FW_BOOT_SUFFIX_A35_\$(1)))" ;
endef

# Display M33FW project config details (m33td-projects <M33FW_PROJECT_NAME> <BOARD>)
define m33td-projects
	\$(if \$(M33FW_PROJECT_NAME),\\
		echo "  M33FW_PROJECT_NAME=\$(M33FW_PROJECT_NAME)" ; \\
		echo "  M33FW_PROJECT_PATH=\$(M33FW_PROJECT_PATH)" ; \\
		,\\
		echo "  Configured project:" ; \\
		echo "    Name: \$(1)" ; \\
		echo "    Path: \$(M33FW_PROJECT_PATH_\$(2))" ;\\
	) \\
	\$(if \$(M33FW_EXTRA_OPTFLAGS),\\
		echo "    with extra options flags: \$(M33FW_EXTRA_OPTFLAGS)" ;,\\
		\$(if \$(findstring ${M33FW_PROJECT_DEFAULT_BUILDCONF},\$(if \$(M33FW_PROJECT_BUILDCONF),\$(M33FW_PROJECT_BUILDCONF),\$(M33FW_PROJECT_BUILDCONF_\$(2)))), \\
			echo "      ${M33FW_PROJECT_DEFAULT_BUILDCONF}" ; , \\
			\$(foreach bldconf, \$(M33FW_BUILDCONF), \\
				\$(if \$(findstring \$(bldconf),\$(if \$(M33FW_PROJECT_BUILDCONF),\$(M33FW_PROJECT_BUILDCONF),\$(M33FW_PROJECT_BUILDCONF_\$(2)))), \\
					echo "      \$(bldconf) build with extra options flags: \$(M33FW_EXTRA_OPTFLAGS_\$(bldconf))" ; \\
				) \\
			) \\
		) \\
	)
endef

# Configure M33PROJECT make rules (m33td-rules-by-dt <CONFIG> <DT> <M33FW_PROJECT_NAME> <BLD_CONFIG> <BOARD>)
#   m33td-<CONFIG>-<DT>-<M33FW_PROJECT_NAME>-<BLD_CONFIG>:
define m33td-starter-rules-by-dt
m33td-\$(1)-\$(2)-\$(3)-\$(4)-\$(5):
	@if [ -z "\$(TF_M_BUILD_PATH)" ]; then \\
		echo "[M33PROJECT CONFIG ERROR] Empty path: TF_M_BUILD_PATH" ; \\
		exit 1 ; \\
	fi;
	@if ! [ -d "\$(TF_M_BUILD_PATH)/\$(1)-\$(2)/spe" ]; then \\
		echo "[M33PROJECT TF-M ERROR] Not able to found '\$(1)-\$(2)/spe' folder in TF_M_BUILD_PATH (\$(TF_M_BUILD_PATH))" ; \\
		exit 1 ; \\
	fi;
	@mkdir -p "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)-\$(4)"
	cd "\$(SRC_PATH)/\$(if \$(M33FW_PROJECT_PATH),\$(M33FW_PROJECT_PATH),\$(M33FW_PROJECT_PATH_\$(5)))"; \\
	cmake -G"Unix Makefiles" \\
		-DTFM_BUILD_DIR="\$(TF_M_BUILD_PATH)/\$(1)-\$(2)/spe" \\
		-S "\$(SRC_PATH)/\$(if \$(M33FW_PROJECT_PATH),\$(M33FW_PROJECT_PATH),\$(M33FW_PROJECT_PATH_\$(5)))" \\
		-B "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)-\$(4)" \\
		\$(if \$(M33FW_EXTRA_OPTFLAGS),\$(M33FW_EXTRA_OPTFLAGS),\$(M33FW_EXTRA_OPTFLAGS_\$(4))) || { echo "[M33PROJECT CMAKE ERROR] M33FW_CONFIG=\$(1), M33FW_PROJECT_NAME=\$(3), M33FW_PROJECT_PATH=\$(3), M33FW_PROJECT_BUILDCONF=\$(4), devicetree=\$(2)"; exit 1; } ; \\
	cd "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)-\$(4)"; \\
	make all || { echo "[M33PROJECT MAKE ERROR] M33FW_CONFIG=\$(1), M33FW_PROJECT_NAME=\$(3), M33FW_PROJECT_BUILDCONF=\$(4), devicetree=\$(2)"; exit 1; }
endef

# Configure M33PROJECT deploy rules (deploy-rules-by-dt <CONFIG> <DT> <M33FW_PROJECT_NAME> <BLD_CONFIG> <BOARD>)
#   deploy-<CONFIG>-<DT>-<M33FW_PROJECT_NAME>-<BLD_CONFIG>:
define deploy-rules-by-dt
deploy-\$(1)-\$(2)-\$(3)-\$(4): m33td-\$(1)-\$(2)-\$(3)-\$(4)-\$(5)
	@mkdir -p \$(DEPLOYDIR)
	@echo "\$(if \$(M33FW_EXTRA_OPTFLAGS),,\$(if \$(findstring \$(4),${M33FW_PROJECT_DEFAULT_BUILDCONF}),,_\$(4)))-\$(2)\$(if \$(M33FW_BOOT_SUFFIX_M33),-\$(M33FW_BOOT_SUFFIX_M33),-\$(M33FW_BOOT_SUFFIX_M33_\$(1)))\$(if \$(M33FW_BOOT_SUFFIX_A35),-\$(M33FW_BOOT_SUFFIX_A35),-\$(M33FW_BOOT_SUFFIX_A35_\$(1)))" > \$(DEPLOYDIR)/m33td_suffix
	if [ -e "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)-\$(4)/\$(3)\$(M33FW_TYPE).${M33TD_FW_SUFFIX}" ]; then \\
		cp -f "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)-\$(4)/\$(3)\$(M33FW_TYPE).${M33TD_FW_SUFFIX}" \\
			"\$(DEPLOYDIR)/\$(3)\`cat \$(DEPLOYDIR)/m33td_suffix\`${M33FW_NS_TYPE}.${M33TD_FW_SUFFIX}" ; \\
	fi;
	if [ -e "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)-\$(4)/\$(3)\$(M33FW_TYPE).elf" ]; then \\
		mkdir -p \$(DEPLOYDIR)/debug; \\
		cp -f "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)-\$(4)/\$(3)\$(M33FW_TYPE).elf" \\
			"\$(DEPLOYDIR)/debug/\$(3)\`cat \$(DEPLOYDIR)/m33td_suffix\`${M33FW_NS_TYPE}.elf" ; \\
	fi;
	if [ -e "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)-\$(4)/\$(3)\$(M33FW_TYPE)_stripped.elf" ]; then \\
		mkdir -p \$(DEPLOYDIR)/debug; \\
		cp -f "\$(BLD_PATH)/\$(1)-\$(2)-\$(3)-\$(4)/\$(3)\$(M33FW_TYPE)_stripped.elf" \\
			"\$(DEPLOYDIR)/debug/\$(3)\`cat \$(DEPLOYDIR)/m33td_suffix\`${M33FW_NS_TYPE}_stripped.elf" ; \\
	fi;
	@rm \$(DEPLOYDIR)/m33td_suffix
endef

# Configure overall deploy rules list
m33td-deploy :=\$(foreach config, \$(M33FW_CONFIG), \\
					\$(foreach dt, \$(if \$(M33FW_DEVICETREE), \$(M33FW_DEVICETREE),\$(M33FW_DEVICETREE_\$(config))), \\
						\$(foreach board, \$(M33FW_BOARDS), \\
							\$(if \$(findstring \$(board),\$(dt)), \\
								\$(if \$(M33FW_EXTRA_OPTFLAGS), deploy-\$(config)-\$(dt)-\$(if \$(M33FW_PROJECT_NAME),\$(M33FW_PROJECT_NAME),\$(M33FW_PROJECT_NAME_\$(board)))-default, \\
									\$(foreach bldconf, \$(M33FW_BUILDCONF) ${M33FW_PROJECT_DEFAULT_BUILDCONF}, \\
										\$(if \$(findstring \$(bldconf),\$(if \$(M33FW_PROJECT_BUILDCONF),\$(M33FW_PROJECT_BUILDCONF),\$(M33FW_PROJECT_BUILDCONF_\$(board)))), \\
											deploy-\$(config)-\$(dt)-\$(if \$(M33FW_PROJECT_NAME),\$(M33FW_PROJECT_NAME),\$(M33FW_PROJECT_NAME_\$(board)))-\$(bldconf) \\
										) \\
									) \\
								) \\
							) \\
						) \\
					) \\
				)

# Set dependencies list for building all
DEPS = \$(m33td-deploy)
DEPS += m33fw

help:
	@echo ""
	@echo "M33PROJECT configuration:"
	@echo ""
	@\$(foreach board, \$(M33FW_BOARDS), \\
		\$(call m33td-projects,\$(if \$(M33FW_PROJECT_NAME),\$(M33FW_PROJECT_NAME),\$(M33FW_PROJECT_NAME_\$(board))),\$(board)) \\
	)
	@echo "Note that each M33PROJECT configuration settings can be updated through overall or specific config var:"
	@echo "  M33FW_EXTRA_OPTFLAGS"
	@echo "  M33FW_PROJECT_BUILDCONF"
	@echo ""
	@echo "  M33FW_CONFIG = \$(M33FW_CONFIG)"
	@echo "Config details:"
	@\$(foreach config, \$(M33FW_CONFIG), \\
		\$(call m33td-configs,\$(config)) \\
	)
	@echo "Note that each M33FW configuration settings can be updated through overall or specific config var:"
	@echo "  M33FW_DEVICETREE"
	@echo "  M33FW_BOOT_SUFFIX_M33"
	@echo "  M33FW_BOOT_SUFFIX_A35"
	@echo ""
	@echo "M33PROJECT firmware folder configuration:"
	@echo "  SRC_PATH  = \$(SRC_PATH)"
	@echo "  BLD_PATH  = \$(BLD_PATH)"
	@echo "  DEPLOYDIR = \$(DEPLOYDIR)"
	@echo "  EXTDT_DIR = \$(EXTDT_DIR)"
	@echo "  TF_M_BUILD_PATH = \$(TF_M_BUILD_PATH)"
	@echo ""
	@echo "M33FW configuration:"
	@echo "  Do not forget to set M33FW deploydir folders (such as M33FW_DEPLOYDIR_ROOT) to provide path to needed binaries"
	@echo ""
	@echo "Available targets:"
	@echo "  all   : default target to build M33PROJECT Starter App with CubeMp2 for defined config(s)"
	@echo "  m33fw : assemble and sign M33 firmwares for defined config(s)"
	@echo "  m33td : build M33PROJECT Starter App with CubeMp2 for defined config(s)"
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

m33fw: \$(m33td-deploy)
	@\$(call dt-check)
	@if [ -e "\$(BLD_PATH)/dt-check" ]; then \\
		for m33fwconfig in \$(M33FW_CONFIG) ; do \\
			M33FW_DEPLOYDIR_CUBE=\$(DEPLOYDIR) \\
			M33FW_CONFIG="\$\$m33fwconfig" \\
			\$(if \$(M33FW_DEVICETREE),M33FW_DEVICETREE="\$(M33FW_DEVICETREE)") \\
			\$(if \$(M33FW_BOOT_SUFFIX_M33),M33FW_BOOT_SUFFIX_M33="\$(M33FW_BOOT_SUFFIX_M33)") \\
			\$(if \$(M33FW_BOOT_SUFFIX_A35),M33FW_BOOT_SUFFIX_A35="\$(M33FW_BOOT_SUFFIX_A35)") \\
			\$(if \$(M33FW_PROJECT_NAME),M33FW_DEFAULT_FIRMWARE="\$(M33FW_PROJECT_NAME)") \\
			\$(M33FWTOOLDIR)/m33fwtool-stm32mp.${MACHINE} || { echo "[M33FW BUILD ERROR] M33FW_CONFIG=\$\$m33fwconfig \$(if \$(M33FW_DEVICETREE),M33FW_DEVICETREE=\$(M33FW_DEVICETREE))"; exit 1; } ; \\
		done ; \\
		echo "***** M33FW binary available under: \$(if \$(M33FW_DEPLOYDIR_M33FW),\$(M33FW_DEPLOYDIR_M33FW),\$(M33FW_DEPLOYDIR_ROOT)/) *****" ; \\
	else \\
		echo "[INFO] No device tree file configured: skip \$(@) target"; \\
	fi

m33td: \$(m33td-deploy)
	@\$(call dt-check)
	@if [ -e "\$(BLD_PATH)/dt-check" ]; then \\
		rm "\$(BLD_PATH)/dt-check" ; \\
		echo "***** M33PROJECT binary available under: \$(DEPLOYDIR)/ *****" ; \\
	else \\
		echo "[INFO] No device tree file configured: skip \$(@) target"; \\
	fi

# generate M33PROJECT rules by dt
\$(foreach config, \$(M33FW_CONFIG), \\
	\$(foreach dt, \$(if \$(M33FW_DEVICETREE),\$(M33FW_DEVICETREE),\$(M33FW_DEVICETREE_\$(config))), \\
		\$(foreach board, \$(M33FW_BOARDS), \\
			\$(if \$(findstring \$(board),\$(dt)), \\
				\$(if \$(M33FW_EXTRA_OPTFLAGS), \$(eval \$(call m33td-starter-rules-by-dt,\$(config),\$(dt),\$(if \$(M33FW_PROJECT_NAME),\$(M33FW_PROJECT_NAME),\$(M33FW_PROJECT_NAME_\$(board))),default,\$(board))), \\
					\$(foreach bldconf, \$(M33FW_BUILDCONF) ${M33FW_PROJECT_DEFAULT_BUILDCONF}, \\
						\$(if \$(findstring \$(bldconf),\$(if \$(M33FW_PROJECT_BUILDCONF),\$(M33FW_PROJECT_BUILDCONF),\$(M33FW_PROJECT_BUILDCONF_\$(board)))), \\
							\$(eval \$(call m33td-starter-rules-by-dt,\$(config),\$(dt),\$(if \$(M33FW_PROJECT_NAME),\$(M33FW_PROJECT_NAME),\$(M33FW_PROJECT_NAME_\$(board))),\$(bldconf),\$(board))) \\
						) \\
					) \\
				) \\
			) \\
		) \\
	) \\
)

# generate deploy rules by dt
\$(foreach config, \$(M33FW_CONFIG), \\
	\$(foreach dt, \$(if \$(M33FW_DEVICETREE),\$(M33FW_DEVICETREE),\$(M33FW_DEVICETREE_\$(config))), \\
		\$(foreach board, \$(M33FW_BOARDS), \\
			\$(if \$(findstring \$(board),\$(dt)), \\
				\$(if \$(M33FW_EXTRA_OPTFLAGS), \$(eval \$(call deploy-rules-by-dt,\$(config),\$(dt),\$(if \$(M33FW_PROJECT_NAME),\$(M33FW_PROJECT_NAME),\$(M33FW_PROJECT_NAME_\$(board))),default,\$(board))), \\
					\$(foreach bldconf, \$(M33FW_BUILDCONF) ${M33FW_PROJECT_DEFAULT_BUILDCONF}, \\
						\$(if \$(findstring \$(bldconf),\$(if \$(M33FW_PROJECT_BUILDCONF),\$(M33FW_PROJECT_BUILDCONF),\$(M33FW_PROJECT_BUILDCONF_\$(board)))), \\
							\$(eval \$(call deploy-rules-by-dt,\$(config),\$(dt),\$(if \$(M33FW_PROJECT_NAME),\$(M33FW_PROJECT_NAME),\$(M33FW_PROJECT_NAME_\$(board))),\$(bldconf),\$(board))) \\
						) \\
					) \\
				) \\
			) \\
		) \\
	) \\
)
EOF
}
addtask do_archiver_create_makefile_for_sdk before do_deploy_archives
