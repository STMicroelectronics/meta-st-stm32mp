require recipes-bsp/u-boot/u-boot.inc

FILESEXTRAPATHS_prepend := "${THISDIR}/u-boot-stm32mp:"

# Configure build dir for externalsrc class usage through devtool
EXTERNALSRC_BUILD_pn-${PN} = "${WORKDIR}/build"
B = "${WORKDIR}/build"

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
                            oe_runmake -C ${S} O=${B}/${config} DEVICE_TREE=${devicetree} DEVICE_TREE_EXT=${devicetree}.dtb
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
do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}/bootloader"
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

# ---------------------------------------------------------------------
# Avoid QA Issue: No GNU_HASH in the elf binary
INSANE_SKIP_${PN} += "ldflags"
# ---------------------------------------------------------------------
# Avoid QA Issue: ELF binary has relocations in .text
# (uboot no need -fPIC option : remove check)
INSANE_SKIP_${PN} += "textrel"
