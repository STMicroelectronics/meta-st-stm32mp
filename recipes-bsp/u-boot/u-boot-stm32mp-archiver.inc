#
# Archiver Configuration
#
SRC_URI_append = " file://README.HOW_TO.txt "

inherit archiver
ARCHIVER_MODE[src] = "original"

inherit archiver_stm32mp_clean

archiver_create_makefile_for_sdk() {
    # Init internal var for uboot_configs: should be 'defconfig,type,binary'
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
                            uboot_configs="${uboot_configs} ${config},${type},${binary}"
                        fi
                    done
                    unset k
                fi
            done
            unset j
        done
        unset i
    else
        uboot_configs="${UBOOT_MACHINE},,${UBOOT_BINARY}"
    fi

    mkdir -p ${ARCHIVER_OUTDIR}

    cat << EOF > ${ARCHIVER_OUTDIR}/Makefile.sdk
# Set default path
SRC_PATH ?= \$(PWD)
BLD_PATH ?= \$(SRC_PATH)/../build
DEPLOYDIR ?= \$(SRC_PATH)/../deploy

# Remove default variables
LDFLAGS =
CFLAGS =
CPPFLAGS =

UBOOT_LOCALVERSION = ${UBOOT_LOCALVERSION}

# Configure default U-Boot configs
UBOOT_CONFIGS ?= ${uboot_configs}
DEVICE_TREE ?= ${UBOOT_DEVICETREE}

# Configure default fip feature
ENABLE_FIP ?= "${@bb.utils.contains('MACHINE_FEATURES','fip','1','',d)}"

help:
	@echo
	@echo "Configured U-Boot config(s):"
	@for config in \$(UBOOT_CONFIGS); do \\
		defconfig=\$\$(echo \$\$config | cut -d',' -f1) ; \\
		type=\$\$(echo \$\$config | cut -d',' -f2) ; \\
		binary=\$\$(echo \$\$config | cut -d',' -f3) ; \\
		echo "  \$\$defconfig config (\$\$type type) for \$\$binary binary" ; \\
		for devicetree in \$(DEVICE_TREE); do \\
			echo "    with device tree: \$\$devicetree" ; \\
		done ; \\
	done
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
	@echo "  clean : clean build directories from generated files"

version:
	@if test ! -e .scmversion ; then echo \$(UBOOT_LOCALVERSION) > \$(SRC_PATH)/.scmversion; fi

all: uboot \$(if \$(ENABLE_FIP),fip)

uboot: version
	@for config in \$(UBOOT_CONFIGS); do \\
		uboot_config=\$\$(echo \$\$config | cut -d',' -f1) ; \\
		uboot_type=\$\$(echo \$\$config | cut -d',' -f2) ; \\
		uboot_binary=\$\$(echo \$\$config | cut -d',' -f3) ; \\
		uboot_suffix=\$\$(echo \$\$uboot_binary | cut -d'.' -f2) ; \\
		# Configure destination folder \\
		if [ "\$\$uboot_suffix" = "img" ]; then \\
			subfolder=/\$\$uboot_type ; \\
		else \\
			subfolder= ; \\
		fi ; \\
		mkdir -p \$(DEPLOYDIR)\$\$subfolder ; \\
		mkdir -p \$(DEPLOYDIR)\$\$subfolder/debug ; \\
		# Make sure about configuration set \\
		if test -z "\$\$uboot_config" -o -z "\$\$uboot_type" -o -z "\$\$uboot_binary"; then \\
			echo ; \\
			echo "[ERROR] UBOOT_CONFIGS wrongly configured. It should be space separated list of element <defconfig>,<type>,<binary>" ; \\
			echo ; \\
			exit 1 ; \\
		fi ; \\
		# Dynamic update for defconfig file \\
		if [ "\$\$uboot_suffix" = "stm32" ]; then \\
			if ! grep -q 'CONFIG_STM32MP15x_STM32IMAGE=y' "\$(SRC_PATH)/configs/\$\$uboot_config"; then \\
				echo "CONFIG_STM32MP15x_STM32IMAGE=y" >> "\$(SRC_PATH)/configs/\$\$uboot_config" ; \\
			fi ; \\
		fi ; \\
		# Init folder and defconfig selected \\
		if [ ! -d \$(BLD_PATH)/\$\$uboot_type ]; then \\
			mkdir -p \$(BLD_PATH)/\$\$uboot_type ; \\
			echo \$(UBOOT_LOCALVERSION) > \$(BLD_PATH)/\$\$uboot_type/.scmversion ; \\
			\$(MAKE) -C \$(SRC_PATH) O=\$(BLD_PATH)/\$\$uboot_type \$\$uboot_config ; \\
		fi ; \\
		# Build binaries \\
		if [ -z "\$(DEVICE_TREE)" ]; then \\
			\$(MAKE) -C \$(SRC_PATH) O=\$(BLD_PATH)/\$\$uboot_type ${UBOOT_MAKE_TARGET} ; \\
			# Copy binary files with explicit name \\
			cp -f \$(BLD_PATH)/\$\$uboot_type/\$\$uboot_binary \$(DEPLOYDIR)\$\$subfolder/u-boot-\$\$uboot_type.\$\$uboot_suffix ; \\
			if [ -f \$(BLD_PATH)/\$\$uboot_type/${SPL_BINARY_STM32} ]; then \\
				cp -f \$(BLD_PATH)/\$\$uboot_type/${SPL_BINARY_STM32} \$(DEPLOYDIR)\$\$subfolder/${SPL_BINARYNAME}-\$\$uboot_type ; \\
			fi ; \\
			if [ -f \$(BLD_PATH)/\$\$uboot_type/${UBOOT_ELF} ]; then \\
				cp -f \$(BLD_PATH)/\$\$uboot_type/${UBOOT_ELF} \$(DEPLOYDIR)\$\$subfolder/debug/u-boot-\$\$uboot_type.${UBOOT_ELF_SUFFIX} ; \\
			fi ; \\
			if [ -f \$(BLD_PATH)/\$\$uboot_type/${SPL_ELF} ]; then \\
				cp -f \$(BLD_PATH)/\$\$uboot_type/${SPL_ELF} \$(DEPLOYDIR)\$\$subfolder/debug/${SPL_ELF_NAME}-\$\$uboot_type ; \\
			fi ; \\
			# Install 'u-boot-nodtb.bin' binary in case '*.dtb' binary installation configured \\
			if [ "\$\$uboot_suffix" = "dtb" ]; then \\
				cp -f \$(BLD_PATH)/\$\$uboot_type/u-boot-nodtb.bin \$(DEPLOYDIR)\$\$subfolder/u-boot-nodtb.bin ; \\
			fi ; \\
		else \\
			for devicetree in \$(DEVICE_TREE); do \\
				\$(MAKE) -C \$(SRC_PATH) O=\$(BLD_PATH)/\$\$uboot_type ${UBOOT_MAKE_TARGET} DEVICE_TREE=\$\$devicetree DEVICE_TREE_EXT=\$\$devicetree.dtb; \\
				# Copy binary files with explicit name \\
				cp -f \$(BLD_PATH)/\$\$uboot_type/\$\$uboot_binary \$(DEPLOYDIR)\$\$subfolder/u-boot-\$\$devicetree-\$\$uboot_type.\$\$uboot_suffix ; \\
				if [ -f \$(BLD_PATH)/\$\$uboot_type/${SPL_BINARY_STM32} ]; then \\
					cp -f \$(BLD_PATH)/\$\$uboot_type/${SPL_BINARY_STM32} \$(DEPLOYDIR)\$\$subfolder/${SPL_BINARYNAME}-\$\$devicetree-\$\$uboot_type ; \\
				fi ; \\
				if [ -f \$(BLD_PATH)/\$\$uboot_type/${UBOOT_ELF} ]; then \\
					cp -f \$(BLD_PATH)/\$\$uboot_type/${UBOOT_ELF} \$(DEPLOYDIR)\$\$subfolder/debug/u-boot-\$\$devicetree-\$\$uboot_type.${UBOOT_ELF_SUFFIX} ; \\
				fi ; \\
				if [ -f \$(BLD_PATH)/\$\$uboot_type/${SPL_ELF} ]; then \\
					cp -f \$(BLD_PATH)/\$\$uboot_type/${SPL_ELF} \$(DEPLOYDIR)\$\$subfolder/debug/${SPL_ELF_NAME}-\$\$devicetree-\$\$uboot_type ; \\
				fi ; \\
				# Install ''u-boot-nodtb.bin' binary in case '*.dtb' binary installation configured \\
				if [ "\$\$uboot_suffix" = "dtb" ]; then \\
					# Init soc suffix \\
					soc_suffix="" ; \\
					if [ -n "${STM32MP_SOC_NAME}" ]; then \\
						for soc in ${STM32MP_SOC_NAME}; do \\
							if [ "\$\$(echo \$\$devicetree | grep -c \$\$soc)" -eq 1 ]; then \\
								soc_suffix="-\$\$soc" ; \\
							fi ; \\
						done ; \\
					fi ; \\
					cp -f \$(BLD_PATH)/\$\$uboot_type/u-boot-nodtb.bin \$(DEPLOYDIR)\$\$subfolder/u-boot-nodtb\$\$soc_suffix.bin ; \\
				fi ; \\
			done ; \\
		fi ; \\
	done

fip: uboot
	FIP_DEPLOYDIR_UBOOT=\$(DEPLOYDIR) FIP_DEVICETREE="\$(DEVICE_TREE)" fiptool-stm32mp

clean:
	@for config in \$(UBOOT_CONFIGS); do \\
		uboot_type=\$\$(echo \$\$config | cut -d',' -f2) ; \\
		echo "Removing \$(BLD_PATH)/\$\$uboot_type ..." ; \\
		rm -rf \$(BLD_PATH)/\$\$uboot_type ; \\
	done
	@echo "Removing \$(DEPLOYDIR) ..."
	@rm -rf \$(DEPLOYDIR)
	@echo
EOF
}
do_ar_original[prefuncs] += "archiver_create_makefile_for_sdk"
