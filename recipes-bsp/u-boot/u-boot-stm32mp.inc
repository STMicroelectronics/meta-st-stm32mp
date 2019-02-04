require recipes-bsp/u-boot/u-boot.inc

FILESEXTRAPATHS_prepend := "${THISDIR}/u-boot-stm32mp:"

# Configure build dir for externalsrc class usage through devtool
EXTERNALSRC_BUILD_pn-${PN} = "${WORKDIR}/build"

# Define dedicated var to configure SPL binary name to override U-Boot default
# configuration. By this way we allow to mix configuration with and without SPL
# binary generation without trouble with binary existence.
SPL_BINARY_STM32 = "spl/u-boot-spl.stm32"
SPL_BINARYNAME = "${@os.path.basename(d.getVar("SPL_BINARY_STM32"))}"
SPL_BINARYROOT = "${@d.getVar('SPL_BINARY_STM32').split('.')[0]}"

# Configure for debug elf
ELF_DEBUG_ENABLE ?= ""
UBOOT_ELF = "${@'u-boot' if d.getVar('ELF_DEBUG_ENABLE') == '1' else ''}"
SPL_ELF = "${@'${SPL_BINARYROOT}' if d.getVar('ELF_DEBUG_ENABLE') == '1' else ''}"
SPL_ELF_NAME  = "${@os.path.basename(d.getVar("SPL_ELF"))}.elf"

# Init UBOOT_DEVICETREE list if not configured
UBOOT_DEVICETREE ?= ""

# -----------------------------------------------
# Enable use of work-shared folder
STAGING_UBOOT_DIR = "${TMPDIR}/work-shared/${MACHINE}/uboot-source"
# Make sure to move ${S} to STAGING_UBOOT_DIR. We can't just
# create the symlink in advance as the git fetcher can't cope with
# the symlink.
do_unpack[cleandirs] += " ${S} ${STAGING_UBOOT_DIR}"
do_clean[cleandirs] += " ${S} ${STAGING_UBOOT_DIR}"
base_do_unpack_append () {
    # Specific part to update devtool-source class
    if bb.data.inherits_class('devtool-source', d):
        # We don't want to move the source to STAGING_UBOOT_DIR here
        if d.getVar('STAGING_UBOOT_DIR', d):
            d.setVar('STAGING_UBOOT_DIR', '${S}')

    # Copy/Paste from kernel class with adaptation to UBOOT var
    s = d.getVar("S")
    if s[-1] == '/':
        # drop trailing slash, so that os.symlink(ubootsrc, s) doesn't use s as directory name and fail
        s=s[:-1]
    ubootsrc = d.getVar("STAGING_UBOOT_DIR")
    if s != ubootsrc:
        bb.utils.mkdirhier(ubootsrc)
        bb.utils.remove(ubootsrc, recurse=True)
        if d.getVar("EXTERNALSRC"):
            # With EXTERNALSRC S will not be wiped so we can symlink to it
            os.symlink(s, ubootsrc)
        else:
            import shutil
            shutil.move(s, ubootsrc)
            os.symlink(ubootsrc, s)
}

# -----------------------------------------------------------------------------
# Append compile to handle specific device tree compilation
#
do_compile_append() {
    if [ -n "${UBOOT_DEVICETREE}" ]; then
        for devicetree in ${UBOOT_DEVICETREE}; do
            if [ -n "${UBOOT_CONFIG}" ]; then
                unset i j k
                for config in ${UBOOT_MACHINE}; do
                    i=$(expr $i + 1);
                    for type in ${UBOOT_CONFIG}; do
                        j=$(expr $j + 1);
                        if [ $j -eq $i ]; then
                            if [ -f ${B}/${config}/dts/dt.dtb ];
                            then
                                rm ${B}/${config}/dts/dt.dtb
                            fi
                            oe_runmake -C ${S} O=${B}/${config} DEVICE_TREE=${devicetree}
                            for binary in ${UBOOT_BINARIES}; do
                                binarysuffix=$(echo ${binary} | cut -d'.' -f2)
                                k=$(expr $k + 1);
                                if [ $k -eq $i ]; then
                                    install -m 644 ${B}/${config}/${binary} ${B}/${config}/u-boot-${devicetree}-${type}.${binarysuffix}
                                    if [ -n "${UBOOT_ELF}" ]; then
                                        install -m 644 ${B}/${config}/${UBOOT_ELF} ${B}/${config}/u-boot-${devicetree}-${type}.${UBOOT_ELF_SUFFIX}
                                    fi
                                    # As soon as SPL binary exists, copy it with specific binary_type name
                                    # This allow to mix u-boot configuration, with and without SPL
                                    if [ -f ${B}/${config}/${SPL_BINARY_STM32} ]; then
                                        install -m 644 ${B}/${config}/${SPL_BINARY_STM32} ${B}/${config}/${SPL_BINARYNAME}-${devicetree}-${type}
                                    fi
                                    if [ -n "${SPL_ELF}" ] && [ -f ${B}/${config}/${SPL_ELF} ]; then
                                        install -m 644 ${B}/${config}/${SPL_ELF} ${B}/${config}/${SPL_ELF_NAME}-${devicetree}-${type}
                                    fi
                                fi
                            done
                            unset k
                        fi
                    done
                    unset j
                done
                unset i
            else
                bbfatal "Wrong u-boot-stm32mp configuration: please make sure to use UBOOT_CONFIG through BOOTSCHEME_LABELS config"
            fi
        done
    fi
}

# -----------------------------------------------------------------------------
# Append deploy to handle specific device tree binary deployement
#
do_deploy_append() {
    if [ -n "${UBOOT_DEVICETREE}" ]; then
        # Clean deploydir from any available binary first
        # This allows to only install the devicetree binary ones
        rm -rf ${DEPLOYDIR}

        # Install destination folder
        install -d ${DEPLOYDIR}

        for devicetree in ${UBOOT_DEVICETREE}; do
            if [ -n "${UBOOT_CONFIG}" ]; then
                unset i j k
                for config in ${UBOOT_MACHINE}; do
                    i=$(expr $i + 1);
                    for type in ${UBOOT_CONFIG}; do
                        j=$(expr $j + 1);
                        if [ $j -eq $i ]; then
                            for binary in ${UBOOT_BINARIES}; do
                                binarysuffix=$(echo ${binary} | cut -d'.' -f2)
                                k=$(expr $k + 1);
                                if [ $k -eq $i ]; then
                                    install -m 644 ${B}/${config}/u-boot-${devicetree}-${type}.${binarysuffix} ${DEPLOYDIR}
                                    if [ -n "${UBOOT_ELF}" ]; then
                                        install -m 644 ${B}/${config}/u-boot-${devicetree}-${type}.${UBOOT_ELF_SUFFIX} ${DEPLOYDIR}
                                    fi
                                    # As soon as SPL binary exists, install it
                                    # This allow to mix u-boot configuration, with and without SPL
                                    if [ -f ${B}/${config}/${SPL_BINARYNAME}-${devicetree}-${type} ]; then
                                        install -m 644 ${B}/${config}/${SPL_BINARYNAME}-${devicetree}-${type} ${DEPLOYDIR}
                                    fi
                                    if [ -n "${SPL_ELF}" ] && [ -f ${B}/${config}/${SPL_ELF_NAME}-${devicetree}-${type} ]; then
                                        install -m 644 ${B}/${config}/${SPL_ELF_NAME}-${devicetree}-${type} ${DEPLOYDIR}
                                    fi
                                fi
                            done
                            unset k
                        fi
                    done
                    unset j
                done
                unset i
            else
                bbfatal "Wrong u-boot-stm32mp configuration: please make sure to use UBOOT_CONFIG through BOOTSCHEME_LABELS config"
            fi
        done
    fi
}

# -----------------------------------------------------------------------------
# ARCHIVER
#
inherit archiver
ARCHIVER_MODE[src] = "${@'original' if d.getVar('ST_ARCHIVER_ENABLE') == '1' else ''}"
SRC_URI  += " file://README.HOW_TO.txt "

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

    # Remove default variable
    echo "LDFLAGS=" > ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "CFLAGS=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "CPPFLAGS=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "LOCAL_PATH=\$(PWD)" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo -n "EXTRA_OEMAKE=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "${EXTRA_OEMAKE}" | sed "s|HOSTCC=${BUILD_CC}||" | sed "s|STAGING_INCDIR=${STAGING_INCDIR_NATIVE}||" | sed "s|STAGING_LIBDIR=${STAGING_LIBDIR_NATIVE}||" | sed "s|${BUILD_CFLAGS} ||" | sed "s|${BUILD_LDFLAGS}||" |sed "s|CC=\([^ ]*\) --sysroot=[^ ]* |CC=\"\1 \$(KCFLAGS)\" |" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "UBOOT_LOCALVERSION=${UBOOT_LOCALVERSION}" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    # Configure default U-Boot configs
    echo "UBOOT_CONFIGS ?= ${uboot_configs}" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "DEVICE_TREE ?= ${UBOOT_DEVICETREE}" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "help:" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"Configured U-Boot config(s):\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@for config in \$(UBOOT_CONFIGS); do \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		defconfig=\$\$(echo \$\$config | cut -d',' -f1) ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		type=\$\$(echo \$\$config | cut -d',' -f2) ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		binary=\$\$(echo \$\$config | cut -d',' -f3) ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		echo \"  \$\$defconfig config (\$\$type type) for \$\$binary binary\" ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		for devicetree in \$(DEVICE_TREE); do \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			echo \"    with device tree: \$\$devicetree\" ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		done ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	done" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"Available targets:\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"  all   : build U-Boot binaries for defined config(s)\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"  clean : clean build directories from generated files\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "version:" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@if test ! -e .scmversion ; then echo \$(UBOOT_LOCALVERSION) > \$(LOCAL_PATH)/.scmversion; fi" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "all: version" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	for config in \$(UBOOT_CONFIGS); do \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		uboot_config=\$\$(echo \$\$config | cut -d',' -f1) ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		uboot_type=-\$\$(echo \$\$config | cut -d',' -f2) ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		uboot_binary=\$\$(echo \$\$config | cut -d',' -f3) ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		uboot_suffix=\$\$(echo \$\$uboot_binary | cut -d'.' -f2) ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Make sure about configuration set
    echo "		if test -z \"\$\$uboot_config\" -o -z \"\$\$uboot_type\" -o -z \"\$\$uboot_binary\"; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			echo ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			echo \"[ERROR] UBOOT_CONFIGS wrongly configured. It should be space separated list of element <defconfig>,<type>,<binary>\" ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			echo ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			exit 1 ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Init folder and defconfig selected
    echo "		if [ ! -d \$(LOCAL_PATH)/../build\$\$uboot_type ]; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			mkdir -p \$(LOCAL_PATH)/../build\$\$uboot_type ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			echo \$(UBOOT_LOCALVERSION) > \$(LOCAL_PATH)/../build\$\$uboot_type/.scmversion ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			\$(MAKE) \$(EXTRA_OEMAKE) -C \$(LOCAL_PATH) O=\$(LOCAL_PATH)/../build\$\$uboot_type \$\$uboot_config ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Build binaries
    echo "		if [ -z \"\$(DEVICE_TREE)\" ]; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			\$(MAKE) \$(EXTRA_OEMAKE) -C \$(LOCAL_PATH) O=\$(LOCAL_PATH)/../build\$\$uboot_type ${UBOOT_MAKE_TARGET} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Copy binary files with explicit name
    echo "			cp -f \$(LOCAL_PATH)/../build\$\$uboot_type/\$\$uboot_binary \$(LOCAL_PATH)/../build\$\$uboot_type/u-boot\$\$uboot_type.\$\$uboot_suffix ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			cp -f \$(LOCAL_PATH)/../build\$\$uboot_type/${UBOOT_ELF} \$(LOCAL_PATH)/../build\$\$uboot_type/u-boot\$\$uboot_type.${UBOOT_ELF_SUFFIX} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			if [ -f \$(LOCAL_PATH)/../build\$\$uboot_type/${SPL_BINARY_STM32} ]; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				cp -f \$(LOCAL_PATH)/../build\$\$uboot_type/${SPL_BINARY_STM32} \$(LOCAL_PATH)/../build\$\$uboot_type/${SPL_BINARYNAME}\$\$uboot_type ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				cp -f \$(LOCAL_PATH)/../build\$\$uboot_type/${SPL_ELF} \$(LOCAL_PATH)/../build\$\$uboot_type/${SPL_ELF_NAME}\$\$uboot_type ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		else \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			for devicetree in \$(DEVICE_TREE); do \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				\$(MAKE) \$(EXTRA_OEMAKE) -C \$(LOCAL_PATH) O=\$(LOCAL_PATH)/../build\$\$uboot_type ${UBOOT_MAKE_TARGET} DEVICE_TREE=\$\$devicetree ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Copy binary files with explicit name
    echo "				cp -f \$(LOCAL_PATH)/../build\$\$uboot_type/\$\$uboot_binary \$(LOCAL_PATH)/../build\$\$uboot_type/u-boot-\$\$devicetree\$\$uboot_type.\$\$uboot_suffix ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				cp -f \$(LOCAL_PATH)/../build\$\$uboot_type/${UBOOT_ELF} \$(LOCAL_PATH)/../build\$\$uboot_type/u-boot-\$\$devicetree\$\$uboot_type.${UBOOT_ELF_SUFFIX} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				if [ -f \$(LOCAL_PATH)/../build\$\$uboot_type/${SPL_BINARY_STM32} ]; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "					cp -f \$(LOCAL_PATH)/../build\$\$uboot_type/${SPL_BINARY_STM32} \$(LOCAL_PATH)/../build\$\$uboot_type/${SPL_BINARYNAME}-\$\$devicetree\$\$uboot_type ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "					cp -f \$(LOCAL_PATH)/../build\$\$uboot_type/${SPL_ELF} \$(LOCAL_PATH)/../build\$\$uboot_type/${SPL_ELF_NAME}-\$\$devicetree\$\$uboot_type ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			done ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	done" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "clean:" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@for config in \$(UBOOT_CONFIGS); do \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		uboot_type=-\$\$(echo \$\$config | cut -d',' -f2) ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		echo \"Removing \$(LOCAL_PATH)/../build\$\$uboot_type ...\" ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		rm -rf \$(LOCAL_PATH)/../build\$\$uboot_type ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	done" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
}
do_ar_original[prefuncs] += "archiver_create_makefile_for_sdk"

# ---------------------------------------------------------------------
# Avoid QA Issue: No GNU_HASH in the elf binary
INSANE_SKIP_${PN} += "ldflags"
# ---------------------------------------------------------------------
# Avoid QA Issue: ELF binary has relocations in .text
# (uboot no need -fPIC option : remove check)
INSANE_SKIP_${PN} += "textrel"
