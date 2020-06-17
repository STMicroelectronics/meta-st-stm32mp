COMPATIBLE_MACHINE = "(stm32mpcommon)"

PACKAGE_ARCH = "${MACHINE_ARCH}"

FILESEXTRAPATHS_prepend := "${THISDIR}/tf-a-stm32mp:"

inherit deploy

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

BL2_NAME ?= "bl2/bl2"
BL2_ELF = "${BL2_NAME}.${TF_A_ELF_SUFFIX}"
BL2_BASENAME = "${@os.path.basename(d.getVar("BL2_NAME"))}"

BL32_NAME ?= "bl32/bl32"
BL32_ELF = "${BL32_NAME}.${TF_A_ELF_SUFFIX}"
BL32_BASENAME = "${@os.path.basename(d.getVar("BL32_NAME"))}"

# Set default TF-A config
TF_A_CONFIG ?= ""

#Enable the wrapper for debug
TF_A_ENABLE_DEBUG_WRAPPER ??= "1"

# -----------------------------------------------
# Enable use of work-shared folder
TFA_SHARED_SOURCES ??= "1"
STAGING_TFA_DIR = "${TMPDIR}/work-shared/${MACHINE}/tfa-source"
# Make sure to move ${S} to STAGING_TFA_DIR. We can't just
# create the symlink in advance as the git fetcher can't cope with
# the symlink.
do_unpack[cleandirs] += " ${S} ${STAGING_TFA_DIR}"
do_clean[cleandirs] += " ${S} ${STAGING_TFA_DIR}"
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

# Manage to export any specific setting for defined configs
python tfaconfig_env () {
    if d.getVar('TF_A_CONFIG'):
        try:
            f = open( ("%s/tfaconfig_env" % d.getVar('T')), 'w')
            for config in d.getVar('TF_A_CONFIG').split():
                f.write( "export TF_A_CONFIG_%s=\"%s\"\n" % (config, d.getVar(('TF_A_CONFIG_' + config))) )
            f.close()
        except:
            pass
}
do_compile[prefuncs] += "tfaconfig_env"

do_compile() {
    . ${T}/tfaconfig_env

    unset LDFLAGS
    unset CFLAGS
    unset CPPFLAGS

    for config in ${TF_A_CONFIG}; do
        # Get any specific EXTRA_OEMAKE for current config
        eval local add_extraoemake=\"\$TF_A_CONFIG_${config}\"
        if [ -n "${TF_A_DEVICETREE}" ]; then
            for dt in ${TF_A_DEVICETREE}; do
                oe_runmake -C ${S} DTB_FILE_NAME=${dt}.dtb BUILD_PLAT=${B}/${config} ${add_extraoemake}
                cp ${B}/${config}/${TF_A_BASENAME}-${dt}.${TF_A_SUFFIX} ${B}/${config}/${TF_A_BASENAME}-${dt}-${config}.${TF_A_SUFFIX}
                if [ "${TF_A_ENABLE_DEBUG_WRAPPER}" = "1" ]; then
                    stm32wrapper4dbg -s ${B}/${config}/${TF_A_BASENAME}-${dt}.${TF_A_SUFFIX} -d ${B}/${config}/debug-${TF_A_BASENAME}-${dt}-${config}.${TF_A_SUFFIX}
                fi
            done
        else
            oe_runmake -C ${S} BUILD_PLAT=${B}/${config} ${add_extraoemake}
            tf_a_binary_basename=$(find ${B}/${config} -name "${TF_A_BASENAME}-*.${TF_A_SUFFIX}" -exec basename {} \; | sed 's|\.'"${TF_A_SUFFIX}"'||g')
            if [ "${TF_A_ENABLE_DEBUG_WRAPPER}" = "1" ]; then
                stm32wrapper4dbg -s ${B}/${config}/${tf_a_binary_basename}.${TF_A_SUFFIX} -d ${B}/${config}/debug-${tf_a_binary_basename}.${TF_A_SUFFIX}
            fi
        fi
    done
}
do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}/arm-trusted-firmware"
do_deploy() {
    install -d ${DEPLOYDIR}

    for config in ${TF_A_CONFIG}; do
        if [ -n "${TF_A_DEVICETREE}" ]; then
            for dt in ${TF_A_DEVICETREE}; do
                install -m 644 ${B}/${config}/${TF_A_BASENAME}-${dt}-${config}.${TF_A_SUFFIX} ${DEPLOYDIR}
                if [ "${TF_A_ENABLE_DEBUG_WRAPPER}" = "1" ]; then
                    install -d ${DEPLOYDIR}/debug
                    install -m 644 ${B}/${config}/debug-${TF_A_BASENAME}-${dt}-${config}.${TF_A_SUFFIX} ${DEPLOYDIR}/debug
                fi
            done
        else
            # Get tf-a binary basename to copy
            tf_a_binary_basename=$(find ${B}/${config} -name "${TF_A_BASENAME}-*.${TF_A_SUFFIX}" -exec basename {} \; | sed 's|\.'"${TF_A_SUFFIX}"'||g')
            for f in ${tf_a_binary_basename}; do
                install -m 644 ${B}/${config}/${f}.${TF_A_SUFFIX} ${DEPLOYDIR}/${f}-${config}.${TF_A_SUFFIX}
                if [ "${TF_A_ENABLE_DEBUG_WRAPPER}" = "1" ]; then
                    install -d ${DEPLOYDIR}/debug
                    install -m 644 ${B}/${config}/debug-${f}.${TF_A_SUFFIX} ${DEPLOYDIR}/debug/debug-${f}-${config}.${TF_A_SUFFIX}
                fi
            done
        fi
    done

    if [ -n "${ELF_DEBUG_ENABLE}" ]; then
        for config in ${TF_A_CONFIG}; do
            if [ -f ${B}/${config}/${BL1_ELF} ]; then
                install -m 644 ${B}/${config}/${BL1_ELF} ${DEPLOYDIR}/${TF_A_BASENAME}-${BL1_BASENAME}-${config}.${TF_A_ELF_SUFFIX}
            fi
            if [ -f ${B}/${config}/${BL2_ELF} ]; then
                install -m 644 ${B}/${config}/${BL2_ELF} ${DEPLOYDIR}/${TF_A_BASENAME}-${BL2_BASENAME}-${config}.${TF_A_ELF_SUFFIX}
            fi
            if [ -f ${B}/${config}/${BL32_ELF} ]; then
                install -m 644 ${B}/${config}/${BL32_ELF} ${DEPLOYDIR}/${TF_A_BASENAME}-${BL32_BASENAME}-${config}.${TF_A_ELF_SUFFIX}
            fi
        done
    fi
}
addtask deploy before do_build after do_compile
