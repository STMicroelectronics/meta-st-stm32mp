COMPATIBLE_MACHINE = "(stm32mpcommon)"

PACKAGE_ARCH = "${MACHINE_ARCH}"

FILESEXTRAPATHS_prepend := "${THISDIR}/tf-a-stm32mp:"

inherit deploy

B = "${WORKDIR}/build"
# Configure build dir for externalsrc class usage through devtool
EXTERNALSRC_BUILD_pn-${PN} = "${WORKDIR}/build"

DEPENDS += "dtc-native"

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

# Extra make settings
EXTRA_OEMAKE = 'CROSS_COMPILE=${TARGET_PREFIX}'
EXTRA_OEMAKE += 'PLAT=stm32mp1'
EXTRA_OEMAKE += 'ARCH=aarch32'
EXTRA_OEMAKE += 'ARM_ARCH_MAJOR=7'

# Debug support
EXTRA_OEMAKE += 'DEBUG=1'
EXTRA_OEMAKE += "LOG_LEVEL=40"

# Set default TF-A config
TF_A_CONFIG ?= ""

# -----------------------------------------------
# Enable use of work-shared folder
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
            done
        else
            oe_runmake -C ${S} BUILD_PLAT=${B}/${config} ${add_extraoemake}
        fi
    done
}

do_deploy() {
    install -d ${DEPLOYDIR}

    for config in ${TF_A_CONFIG}; do
        if [ -n "${TF_A_DEVICETREE}" ]; then
            for dt in ${TF_A_DEVICETREE}; do
                install -m 644 ${B}/${config}/${TF_A_BASENAME}-${dt}-${config}.${TF_A_SUFFIX} ${DEPLOYDIR}
            done
        else
            # Get tf-a binary basename to copy
            tf_a_binary_basename=$(find ${B}/${config} -name "${TF_A_BASENAME}-*.${TF_A_SUFFIX}" -exec basename {} \; | sed 's|\.'"${TF_A_SUFFIX}"'||g')
            install -m 644 ${B}/${config}/${tf_a_binary_basename}.${TF_A_SUFFIX} ${DEPLOYDIR}/${tf_a_binary_basename}-${config}.${TF_A_SUFFIX}
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


# -----------------------------------------------
# Archiver
inherit archiver
ARCHIVER_MODE[src] = "${@'original' if d.getVar('ST_ARCHIVER_ENABLE') == '1' else ''}"

inherit archiver_stm32mp_clean
SRC_URI =+ " file://README.HOW_TO.txt "

archiver_create_makefile_for_sdk() {
    . ${T}/tfaconfig_env

    # Init internal var for tfa_config_oemake: should be 'config,extraoemake'
    for config in ${TF_A_CONFIG}; do
        tf_a_config_oemake="${tf_a_config_oemake} ${config},$(eval echo \$TF_A_CONFIG_${config})"
    done

    mkdir -p ${ARCHIVER_OUTDIR}
    #remove default variable
    echo "LDFLAGS=" > ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "CFLAGS=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "CPPFLAGS=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "CC=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "CPP=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "AS=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "AR=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "LD=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "NM=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "LOCAL_PATH=\$(PWD)" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "EXTRA_OEMAKE=${EXTRA_OEMAKE}" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    # Set default config
    echo "ELF_DEBUG_ENABLE ?= ${ELF_DEBUG_ENABLE}" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "TF_A_CONFIG ?= ${TF_A_CONFIG}" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Set specific OEMAKE config
    echo "TF_A_CONFIG_OEMAKE = ${tf_a_config_oemake}" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    # Set default TFA_DEVICETREE with configured TF_A_DEVICETREE
    if [ -n "${TF_A_DEVICETREE}" ]; then
        echo "TFA_DEVICETREE ?= ${TF_A_DEVICETREE}" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    else
        echo "TFA_DEVICETREE ?=" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    fi
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "help:" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"Available targets:\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"  all   : build TF-A binaries for defined config(s)\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"  clean : clean build directories from generated files\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"TF-A configuration:\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"  TF_A_CONFIG = \$(TF_A_CONFIG)\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"  TFA_DEVICETREE = \$(TFA_DEVICETREE)\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo \"  ELF_DEBUG_ENABLE = '\$(ELF_DEBUG_ENABLE)' ('1' to export elf files)\"" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@echo" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "all: tf" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "host_tools:" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@\$(MAKE) --no-print-directory -C \$(LOCAL_PATH)/tools/stm32image" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "tf: host_tools" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	for config in \$(TF_A_CONFIG) ; do \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Init any extraoemake
    echo "		add_extraoemake= ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		for fullconfig in \$(TF_A_CONFIG_OEMAKE) ; do \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			extraconfig=\$\$(echo \$\$fullconfig | cut -d',' -f1) ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			if [ \"\$\$extraconfig\" = \"\$\$config\" ]; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				add_extraoemake=\$\$(echo \$\$fullconfig | cut -d',' -f2) ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		done ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		mkdir -p \$(LOCAL_PATH)/../build/\$\$config ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		if test -n \"\$(TFA_DEVICETREE)\" ; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			for dt in \$(TFA_DEVICETREE) ; do \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				\$(MAKE) \$(EXTRA_OEMAKE) -C \$(LOCAL_PATH) DTB_FILE_NAME=\$\$dt.dtb BUILD_PLAT=\$(LOCAL_PATH)/../build/\$\$config \$\$add_extraoemake ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Copy binary file with explicit name
    echo "				cp -f \$(LOCAL_PATH)/../build/\$\$config/${TF_A_BASENAME}-\$\$dt.${TF_A_SUFFIX} \$(LOCAL_PATH)/../build/\$\$config/${TF_A_BASENAME}-\$\$dt-\$\$config.${TF_A_SUFFIX} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			done ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		else \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			\$(MAKE) \$(EXTRA_OEMAKE) -C \$(LOCAL_PATH) BUILD_PLAT=\$(LOCAL_PATH)/../build/\$\$config \$\$add_extraoemake; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Copy binary file with explicit name
    echo "			tf_version=\$\$(find \$(LOCAL_PATH)/../build/\$\$config -name ${TF_A_BASENAME}*.${TF_A_SUFFIX} -exec basename {} \; | sed \"s/\.${TF_A_SUFFIX}//\") ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			cp -f \$(LOCAL_PATH)/../build/\$\$config/\$\$tf_version.${TF_A_SUFFIX} \$(LOCAL_PATH)/../build/\$\$config/\$\$tf_version-\$\$config.${TF_A_SUFFIX} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    # Copy elf files with explicit name
    echo "		if [ \"\$(ELF_DEBUG_ENABLE)\" = \"1\" ] ; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			if [ -f \$(LOCAL_PATH)/../build/\$\$config/${BL1_ELF} ] ; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				cp -f \$(LOCAL_PATH)/../build/\$\$config/${BL1_ELF} \$(LOCAL_PATH)/../build/\$\$config/${TF_A_BASENAME}-${BL1_BASENAME}-\$\$config.${TF_A_ELF_SUFFIX} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			if [ -f \$(LOCAL_PATH)/../build/\$\$config/${BL2_ELF} ] ; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				cp -f \$(LOCAL_PATH)/../build/\$\$config/${BL2_ELF} \$(LOCAL_PATH)/../build/\$\$config/${TF_A_BASENAME}-${BL2_BASENAME}-\$\$config.${TF_A_ELF_SUFFIX} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			if [ -f \$(LOCAL_PATH)/../build/\$\$config/${BL32_ELF} ] ; then \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "				cp -f \$(LOCAL_PATH)/../build/\$\$config/${BL32_ELF} \$(LOCAL_PATH)/../build/\$\$config/${TF_A_BASENAME}-${BL32_BASENAME}-\$\$config.${TF_A_ELF_SUFFIX} ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "			fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		fi ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	done" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "" >> ${ARCHIVER_OUTDIR}/Makefile.sdk

    echo "clean:" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	@for config in \$(TF_A_CONFIG) ; do \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "		rm -rf \$(LOCAL_PATH)/../build/\$\$config ; \\" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
    echo "	done" >> ${ARCHIVER_OUTDIR}/Makefile.sdk
}
do_ar_original[prefuncs] += "tfaconfig_env archiver_create_makefile_for_sdk"
