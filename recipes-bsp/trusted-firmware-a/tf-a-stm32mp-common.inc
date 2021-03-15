PACKAGE_ARCH = "${MACHINE_ARCH}"

FILESEXTRAPATHS_prepend := "${THISDIR}/tf-a-stm32mp:"

inherit deploy
inherit ${@bb.utils.contains('MACHINE_FEATURES', 'fip', 'fip-utils-stm32mp', '', d)}

# Include TF-A config definitions
include tf-a-stm32mp-config.inc

# ------------------------------------
# Set MBEDTLS support
TFA_MBEDTLS_DIR ?= "mbedtls"
# MBEDTLS v2.24.0
SRC_URI_MBEDTLS = "git://github.com/ARMmbed/mbedtls.git;protocol=https;destsuffix=git/${TFA_MBEDTLS_DIR};branch=master;name=mbedtls"
SRCREV_mbedtls = "523f0554b6cdc7ace5d360885c3f5bbcc73ec0e8"
LIC_FILES_CHKSUM_MBEDTLS = "file://mbedtls/LICENSE;md5=3b83ef96387f14655fc854ddc3c6bd57"
LICENSE_MBEDTLS = "Apache-2.0"
# Add MBEDTLS to our sources
SRC_URI_append = " ${@bb.utils.contains('TF_A_SIGN_ENABLE', '1', '${SRC_URI_MBEDTLS}', '', d)}"
# Update license variables
LICENSE_append = "${@bb.utils.contains('TF_A_SIGN_ENABLE', '1', ' & ${LICENSE_MBEDTLS}', '', d)}"
LIC_FILES_CHKSUM_append = "${@bb.utils.contains('TF_A_SIGN_ENABLE', '1', ' ${LIC_FILES_CHKSUM_MBEDTLS}', '', d)}"
# Add mbed TLS to version
SRCREV_FORMAT_append = "${@bb.utils.contains('TF_A_SIGN_ENABLE', '1', '_mbedtls', '', d)}"
# ------------------------------------

B = "${WORKDIR}/build"
# Configure build dir for externalsrc class usage through devtool
EXTERNALSRC_BUILD_pn-${PN} = "${WORKDIR}/build"

DEPENDS += "dtc-native"
DEPENDS_append = " ${@bb.utils.contains('TF_A_ENABLE_DEBUG_WRAPPER', '1', 'stm32wrapper4dbg-native', '', d)}"

# Extra make settings
EXTRA_OEMAKE = 'CROSS_COMPILE=${TARGET_PREFIX}'
# Debug support
EXTRA_OEMAKE += 'DEBUG=1'
EXTRA_OEMAKE += 'LOG_LEVEL=40'

# Define default TF-A namings
TF_A_BASENAME ?= "tf-a"
TF_A_SUFFIX ?= "stm32"

# Output the ELF generated
ELF_DEBUG_ENABLE ?= ""
TF_A_ELF_SUFFIX = "elf"

BL1_NAME ?= "bl1/bl1"
BL1_ELF = "${BL1_NAME}.${TF_A_ELF_SUFFIX}"
BL1_BASENAME = "${@os.path.basename(d.getVar("BL1_NAME"))}"
BL1_BASENAME_DEPLOY ?= "${@os.path.basename(d.getVar("BL1_NAME"))}"

BL2_NAME ?= "bl2/bl2"
BL2_ELF = "${BL2_NAME}.${TF_A_ELF_SUFFIX}"
BL2_BASENAME = "${@os.path.basename(d.getVar("BL2_NAME"))}"
BL2_BASENAME_DEPLOY ?= "${@os.path.basename(d.getVar("BL2_NAME"))}"

BL31_NAME ?= "bl31/bl31"
BL31_ELF = "${BL31_NAME}.${TF_A_ELF_SUFFIX}"
BL31_BASENAME = "${@os.path.basename(d.getVar("BL31_NAME"))}"
BL31_BASENAME_DEPLOY ?= "${@os.path.basename(d.getVar("BL31_NAME"))}"
BL31_SUFFIX ?= "bin"

BL32_NAME ?= "bl32/bl32"
BL32_ELF = "${BL32_NAME}.${TF_A_ELF_SUFFIX}"
BL32_BASENAME = "${@os.path.basename(d.getVar("BL32_NAME"))}"
BL32_BASENAME_DEPLOY ?= "${@os.path.basename(d.getVar("BL32_NAME"))}"
BL32_SUFFIX ?= "bin"

DT_SUFFIX ?= "dtb"
FWCONFIG_NAME ?= "fw-config"

# Set default TF-A config
TF_A_CONFIG ?= ""

# Enable the wrapper for debug
TF_A_ENABLE_DEBUG_WRAPPER ??= "1"

# Set default configuration to allow signing
TF_A_SIGN_ENABLE ??= "0"

# Configure specific build flags
EXTRA_OEMAKE += "${@bb.utils.contains('TF_A_SIGN_ENABLE', '1', 'TRUSTED_BOARD_BOOT=1', '', d)}"
EXTRA_OEMAKE += "${@bb.utils.contains('TF_A_SIGN_ENABLE', '1', 'MBEDTLS_DIR=${TFA_MBEDTLS_DIR}', '', d)}"

# -----------------------------------------------
# Handle TF-A config and set internal vars
#   TF_A_DEVICETREE
#   TF_A_EXTRA_OPTFLAGS
python () {
    import re

    tfaconfigflags = d.getVarFlags('TF_A_CONFIG')
    # The "doc" varflag is special, we don't want to see it here
    tfaconfigflags.pop('doc', None)
    tfaconfig = (d.getVar('TF_A_CONFIG') or "").split()
    tfabasename = d.getVar('TF_A_BASENAME')

    if not tfaconfig:
        raise bb.parse.SkipRecipe("TF_A_CONFIG must be set in the %s machine configuration." % d.getVar("MACHINE"))
    if (d.getVar('TF_A_DEVICETREE') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_A_DEVICETREE as it is internal to TF_A_CONFIG var expansion.")
    if (d.getVar('TF_A_EXTRA_OPTFLAGS') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_A_EXTRA_OPTFLAGS as it is internal to TF_A_CONFIG var expansion.")
    if (d.getVar('TF_A_BINARIES') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_A_BINARIES as it is internal to TF_A_CONFIG var expansion.")
    if (d.getVar('TF_A_MAKE_TARGET') or "").split():
        raise bb.parse.SkipRecipe("You cannot use TF_A_MAKE_TARGET as it is internal to TF_A_CONFIG var expansion.")

    if len(tfaconfig) > 0:
        for config in tfaconfig:
            for f, v in tfaconfigflags.items():
                if config == f:
                    # Make sure to get var flag properly expanded
                    v = d.getVarFlag('TF_A_CONFIG', config)
                    if not v.strip():
                        bb.fatal('[TF_A_CONFIG] Missing configuration for %s config' % config)
                    items = v.split(',')
                    if items[0] and len(items) > 4:
                        raise bb.parse.SkipRecipe('Only <DEVICETREE>,<EXTRA_OPTFLAGS>,<BINARY_BASENAME>,<MAKE_TARGET> can be specified!')
                    # Set internal vars
                    bb.debug(1, "Appending '%s' to TF_A_DEVICETREE" % items[0])
                    d.appendVar('TF_A_DEVICETREE', items[0] + ',')
                    if len(items) > 1 and items[1]:
                        bb.debug(1, "Appending '%s' to TF_A_EXTRA_OPTFLAGS." % items[1])
                        d.appendVar('TF_A_EXTRA_OPTFLAGS', items[1] + ',')
                    else:
                        d.appendVar('TF_A_EXTRA_OPTFLAGS', '' + ',')
                    if len(items) > 2 and items[2]:
                        bb.debug(1, "Appending '%s' to TF_A_BINARIES." % items[2])
                        d.appendVar('TF_A_BINARIES', items[2] + ',')
                    else:
                        bb.debug(1, "Appending '%s' to TF_A_BINARIES." % tfabasename)
                        d.appendVar('TF_A_BINARIES', tfabasename + ',')
                    if len(items) > 3 and items[3]:
                        bb.debug(1, "Appending '%s' to TF_A_MAKE_TARGET." % items[3])
                        d.appendVar('TF_A_MAKE_TARGET', items[3] + ',')
                    else:
                        d.appendVar('TF_A_MAKE_TARGET', 'all' + ',')
                    break
}

# -----------------------------------------------
# Enable use of work-shared folder
TFA_SHARED_SOURCES ??= "1"
STAGING_TFA_DIR = "${TMPDIR}/work-shared/${MACHINE}/tfa-source"
# Make sure to move ${S} to STAGING_TFA_DIR. We can't just
# create the symlink in advance as the git fetcher can't cope with
# the symlink.
do_unpack[cleandirs] += "${S}"
do_unpack[cleandirs] += "${@bb.utils.contains('TFA_SHARED_SOURCES', '1', '${STAGING_TFA_DIR}', '', d)}"
do_clean[cleandirs] += "${S}"
do_clean[cleandirs] += "${@bb.utils.contains('TFA_SHARED_SOURCES', '1', '${STAGING_TFA_DIR}', '', d)}"
base_do_unpack_append () {
    # Specific part to update devtool-source class
    if bb.data.inherits_class('devtool-source', d):
        # We don't want to move the source to STAGING_TFA_DIR here
        if d.getVar('STAGING_TFA_DIR', d):
            d.setVar('STAGING_TFA_DIR', '${S}')

    shared = d.getVar("TFA_SHARED_SOURCES")
    if shared and oe.types.boolean(shared):
        # Copy/Paste from kernel class with adaptation to TFA var
        s = d.getVar("S")
        if s[-1] == '/':
            # drop trailing slash, so that os.symlink(tfasrc, s) doesn't use s as directory name and fail
            s=s[:-1]
        tfasrc = d.getVar("STAGING_TFA_DIR")
        if s != tfasrc:
            bb.utils.mkdirhier(tfasrc)
            bb.utils.remove(tfasrc, recurse=True)
            if d.getVar("EXTERNALSRC"):
                # With EXTERNALSRC S will not be wiped so we can symlink to it
                os.symlink(s, tfasrc)
            else:
                import shutil
                shutil.move(s, tfasrc)
                os.symlink(tfasrc, s)
}

do_compile() {
    unset LDFLAGS
    unset CFLAGS
    unset CPPFLAGS

    unset i
    for config in ${TF_A_CONFIG}; do
        i=$(expr $i + 1)
        # Initialize devicetree list, extra make options and tf-a basename
        dt_config=$(echo ${TF_A_DEVICETREE} | cut -d',' -f${i})
        extra_opt=$(echo ${TF_A_EXTRA_OPTFLAGS} | cut -d',' -f${i})
        tfa_basename=$(echo ${TF_A_BINARIES} | cut -d',' -f${i})
        tf_a_make_target=$(echo ${TF_A_MAKE_TARGET} | cut -d',' -f${i})
        for dt in ${dt_config}; do
            oe_runmake -C ${S} BUILD_PLAT=${B}/${config} DTB_FILE_NAME=${dt}.dtb ${extra_opt} ${tf_a_make_target}
            # Copy TF-A binary with explicit devicetree filename
            if [ -f ${B}/${config}/${tfa_basename}-${dt}.${TF_A_SUFFIX} ]; then
                cp ${B}/${config}/${tfa_basename}-${dt}.${TF_A_SUFFIX} ${B}/${config}/${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}
                if [ "${TF_A_ENABLE_DEBUG_WRAPPER}" = "1" ]; then
                    stm32wrapper4dbg -s ${B}/${config}/${tfa_basename}-${dt}.${TF_A_SUFFIX} -d ${B}/${config}/debug-${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}
                fi
            fi
            # Specific for bl32 target
            if [ "$(echo ${tf_a_make_target} | grep -cw 'bl32')" = "1" ]; then
                # Move 'bl32.elf' file to explicit file name with 'soc_suffix' info (same file for all devicetree build)
                # This avoid unexpected deployment for other config (cf. do_deploy task)
                if [ -f ${B}/${config}/${BL32_ELF} ]; then
                    # Init soc suffix
                    soc_suffix=""
                    if [ -n "${STM32MP_SOC_NAME}" ]; then
                        for soc in ${STM32MP_SOC_NAME}; do
                            [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] && soc_suffix="-${soc}"
                        done
                    fi
                    mv -f ${B}/${config}/${BL32_ELF} ${B}/${config}/${TF_A_BASENAME}-${BL32_BASENAME}${soc_suffix}.${TF_A_ELF_SUFFIX}
                fi
            fi
        done
    done
    unset i
}

do_deploy() {
    install -d ${DEPLOYDIR}
    install -d ${DEPLOYDIR}/arm-trusted-firmware

    unset i
    for config in ${TF_A_CONFIG}; do
        i=$(expr $i + 1)
        # Initialize devicetree list and tf-a basename
        dt_config=$(echo ${TF_A_DEVICETREE} | cut -d',' -f${i})
        tfa_basename=$(echo ${TF_A_BINARIES} | cut -d',' -f${i})
        for dt in ${dt_config}; do
            # Install TF-A binary
            if [ -f ${B}/${config}/${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX} ]; then
                install -m 644 ${B}/${config}/${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX} ${DEPLOYDIR}/arm-trusted-firmware/
                if [ "${TF_A_ENABLE_DEBUG_WRAPPER}" = "1" ]; then
                    install -d ${DEPLOYDIR}/arm-trusted-firmware/debug
                    install -m 644 ${B}/${config}/debug-${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX} ${DEPLOYDIR}/arm-trusted-firmware/debug/
                fi
            fi
            # Init soc suffix
            soc_suffix=""
            if [ -n "${STM32MP_SOC_NAME}" ]; then
                for soc in ${STM32MP_SOC_NAME}; do
                    [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] && soc_suffix="-${soc}"
                done
            fi
            # Install BL31 files
            if [ -f ${B}/${config}/${BL31_BASENAME}.${BL31_SUFFIX} ]; then
                install -d ${DEPLOYDIR}/arm-trusted-firmware/bl31
                # Install BL31 binary
                install -m 644 ${B}/${config}/${BL31_BASENAME}.${BL31_SUFFIX} ${DEPLOYDIR}/arm-trusted-firmware/bl31/${tfa_basename}-${BL31_BASENAME_DEPLOY}${soc_suffix}.${BL31_SUFFIX}
                if [ -n "${ELF_DEBUG_ENABLE}" ]; then
                    if [ -f ${B}/${config}/${tfa_basename}-${BL31_BASENAME}${soc_suffix}.${TF_A_ELF_SUFFIX} ]; then
                        install -d ${DEPLOYDIR}/arm-trusted-firmware/bl32/debug
                        install -m 644 ${B}/${config}/${tfa_basename}-${BL31_BASENAME}${soc_suffix}.${TF_A_ELF_SUFFIX} ${DEPLOYDIR}/arm-trusted-firmware/bl31/debug/${tfa_basename}-${BL31_BASENAME_DEPLOY}${soc_suffix}.${TF_A_ELF_SUFFIX}
                    fi
                fi
            fi
            # Install BL32 files
            if [ -f ${B}/${config}/${BL32_BASENAME}.${BL32_SUFFIX} ]; then
                install -d ${DEPLOYDIR}/arm-trusted-firmware/bl32
                # Install BL32 binary
                install -m 644 ${B}/${config}/${BL32_BASENAME}.${BL32_SUFFIX} ${DEPLOYDIR}/arm-trusted-firmware/bl32/${tfa_basename}-${BL32_BASENAME_DEPLOY}${soc_suffix}.${BL32_SUFFIX}
                # Install BL32 devicetree
                if [ -f ${B}/${config}/fdts/${dt}-${BL32_BASENAME}.${DT_SUFFIX} ]; then
                    install -m 644 ${B}/${config}/fdts/${dt}-${BL32_BASENAME}.${DT_SUFFIX} ${DEPLOYDIR}/arm-trusted-firmware/bl32/${dt}-${BL32_BASENAME}.${DT_SUFFIX}
                fi
                if [ -n "${ELF_DEBUG_ENABLE}" ]; then
                    if [ -f ${B}/${config}/${tfa_basename}-${BL32_BASENAME}${soc_suffix}.${TF_A_ELF_SUFFIX} ]; then
                        install -d ${DEPLOYDIR}/arm-trusted-firmware/bl32/debug
                        install -m 644 ${B}/${config}/${tfa_basename}-${BL32_BASENAME}${soc_suffix}.${TF_A_ELF_SUFFIX} ${DEPLOYDIR}/arm-trusted-firmware/bl32/debug/${tfa_basename}-${BL32_BASENAME_DEPLOY}${soc_suffix}.${TF_A_ELF_SUFFIX}
                    fi
                fi
            fi
            # Install fwconfig
            if [ -f ${B}/${config}/fdts/${dt}-${FWCONFIG_NAME}.${DT_SUFFIX} ]; then
                install -d ${DEPLOYDIR}/arm-trusted-firmware/fwconfig
                install -m 644 ${B}/${config}/fdts/${dt}-${FWCONFIG_NAME}.${DT_SUFFIX} ${DEPLOYDIR}/arm-trusted-firmware/fwconfig/${dt}-${FWCONFIG_NAME}-${config}.${DT_SUFFIX}
            fi
        done
        if [ -n "${ELF_DEBUG_ENABLE}" ]; then
            install -d ${DEPLOYDIR}/arm-trusted-firmware/debug
            if [ -f ${B}/${config}/${BL1_ELF} ]; then
                install -m 644 ${B}/${config}/${BL1_ELF} ${DEPLOYDIR}/arm-trusted-firmware/debug/${tfa_basename}-${BL1_BASENAME_DEPLOY}-${config}.${TF_A_ELF_SUFFIX}
            fi
            if [ -f ${B}/${config}/${BL2_ELF} ]; then
                install -m 644 ${B}/${config}/${BL2_ELF} ${DEPLOYDIR}/arm-trusted-firmware/debug/${tfa_basename}-${BL2_BASENAME_DEPLOY}-${config}.${TF_A_ELF_SUFFIX}
            fi
            if [ -f ${B}/${config}/${BL32_ELF} ]; then
                install -m 644 ${B}/${config}/${BL32_ELF} ${DEPLOYDIR}/arm-trusted-firmware/debug/${tfa_basename}-${BL32_BASENAME_DEPLOY}-${config}.${TF_A_ELF_SUFFIX}
            fi
        fi
    done
    unset i
}
addtask deploy before do_build after do_compile
