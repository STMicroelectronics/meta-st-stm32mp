FILESEXTRAPATHS:prepend := "${THISDIR}/optee-os:"

PACKAGE_ARCH = "${MACHINE_ARCH}"

PROVIDES += "virtual-optee-os virtual-optee-os-ta-pkcs11"
PROVIDES += "virtual-optee-os-ta-pkcs11 virtual-optee-os-ta-avb"
PROVIDES += "virtual-optee-os-ta-remoteproc virtual-optee-os-ta-stm32mp-nvmem"
PROVIDES += "virtual-optee-os-ta-trusted-keys"
RPROVIDES:${PN} += "virtual-optee-os virtual-systemd-bootconf"

CVE_PRODUCT = "linaro:op-tee op-tee:op-tee_os_os mbed_tls"

inherit external-dt
inherit fip-utils-stm32mp

# Include TF-A config definitions
require optee-os-stm32mp-config.inc

B = "${WORKDIR}/build"
# Configure build dir for externalsrc class usage through devtool
EXTERNALSRC_BUILD:pn-${PN} = "${WORKDIR}/build"

DEPENDS += "dtc-native"
DEPENDS += "python3-pycryptodomex-native"
DEPENDS += "python3-pyelftools-native"
DEPENDS += "libgcc python3-cryptography-native"
DEPENDS += "python3-pillow-native"
DEPENDS:append:virtclass-multilib-lib64 = " gcc-arm-none-eabi-native "

inherit deploy python3native

OPTEEMACHINE ?= "${MACHINE}"
OPTEEOUTPUTMACHINE ?= "${MACHINE}"

# Default log level
ST_OPTEE_DEBUG_LOG_LEVEL ??= "2"

# default core debug
ST_OPTEE_CORE_DEBUG ??= "y"
ST_OPTEE_CORE_DEBUG:stm32mp15common ?= "n"

# Set default OPTEE-OS config
OPTEE_CONFIG ??= ""

EXTRA_OEMAKE = "PLATFORM=${OPTEEMACHINE}"
EXTRA_OEMAKE += "CROSS_COMPILE_core=${HOST_PREFIX}"
EXTRA_OEMAKE += "${@bb.utils.contains('TUNE_FEATURES', 'aarch64', 'ARCH=arm CFG_ARM64_core=y CROSS_COMPILE_ta_arm64=${HOST_PREFIX}', 'ARCH=arm CFG_ARM32_core=y CROSS_COMPILE_ta_arm32=${HOST_PREFIX}', d)}"
EXTRA_OEMAKE += "NOWERROR=1"
EXTRA_OEMAKE += "LDFLAGS="
# debug and trace
EXTRA_OEMAKE += "${@bb.utils.contains('ST_OPTEE_DEBUG_TRACE', '1', 'CFG_TEE_CORE_LOG_LEVEL=${ST_OPTEE_DEBUG_LOG_LEVEL} CFG_TEE_CORE_DEBUG=${ST_OPTEE_CORE_DEBUG}', '', d)}"

EXTRA_OEMAKE:append:virtclass-multilib-lib64 = " CROSS_COMPILE_ta_arm32=${STAGING_DATADIR_NATIVE}/gcc-arm-none-eabi/bin/arm-none-eabi- "
EXTRA_OEMAKE:append:virtclass-multilib-lib64 = " supported-ta-targets='ta_arm64 ta_arm32' "

DEBUG_PREFIX_MAP:virtclass-multilib-lib64 ?= " \
 -fmacro-prefix-map=${S}=${TARGET_DBGSRC_DIR} \
 -fdebug-prefix-map=${S}=${TARGET_DBGSRC_DIR} \
 -fmacro-prefix-map=${B}=${TARGET_DBGSRC_DIR} \
 -fdebug-prefix-map=${B}=${TARGET_DBGSRC_DIR} \
 -fdebug-prefix-map=${STAGING_DIR_HOST}= \
 -fmacro-prefix-map=${STAGING_DIR_HOST}= \
 -fdebug-prefix-map=${STAGING_DIR_NATIVE}= \
"

# --------------------------------------------
# Export TA config
ST_OPTEE_EXPORT_TA_REF_BOARD ??= ""
ST_OPTEE_EXPORT_TA_REF_BOARD:stm32mp1common ??= "stm32mp135f-dk.dts"
ST_OPTEE_EXPORT_TA_REF_BOARD:stm32mp2common ??= "stm32mp257f-dk.dts"
ST_OPTEE_EXPORT_TA_OEMAKE_EXTRA ??= ""
ST_OPTEE_EXPORT_TA_OUTDIR ?= "${B}/export-ta"

OPTEE_EXPORT_TA_DIR_ARM32 = "${ST_OPTEE_EXPORT_TA_OUTDIR}/export-ta_arm32"
OPTEE_EXPORT_TA_DIR_ARM64 = "${ST_OPTEE_EXPORT_TA_OUTDIR}/export-ta_arm64"
OPTEE_EXPORT_TA_INSTALL_ARM32 = "optee/export-user_ta_arm32"
OPTEE_EXPORT_TA_INSTALL_ARM32:stm32mp1common = "optee/export-user_ta"
OPTEE_EXPORT_TA_INSTALL_ARM64 = "optee/export-user_ta_arm64"
# avb TA
OPTEE_EXPORT_TA_AVB_DIR:stm32mp1common ?= "${ST_OPTEE_EXPORT_TA_OUTDIR}/ta/avb/"
OPTEE_EXPORT_TA_AVB_DIR:stm32mp2common ?= "${ST_OPTEE_EXPORT_TA_OUTDIR}/export-ta_arm64/ta/"
OPTEE_EXPORT_TA_AVB ?= "023f8f1a-292a-432b-8fc4-de8471358067.ta"
# pkcs11 TA
OPTEE_EXPORT_TA_PKCS11_DIR:stm32mp1common ?= "${ST_OPTEE_EXPORT_TA_OUTDIR}/ta/pkcs11/"
OPTEE_EXPORT_TA_PKCS11_DIR:stm32mp2common ?= "${ST_OPTEE_EXPORT_TA_OUTDIR}/export-ta_arm64/ta/"
OPTEE_EXPORT_TA_PKCS11 ?= "fd02c9da-306c-48c7-a49c-bbd827ae86ee.ta"
# remoteproc TA
OPTEE_EXPORT_TA_REMOTEPROC_DIR:stm32mp1common ?= "${ST_OPTEE_EXPORT_TA_OUTDIR}/ta/remoteproc/"
OPTEE_EXPORT_TA_REMOTEPROC_DIR:stm32mp2common ?= "${ST_OPTEE_EXPORT_TA_OUTDIR}/export-ta_arm64/ta/"
OPTEE_EXPORT_TA_REMOTEPROC ?= "80a4c275-0a47-4905-8285-1486a9771a08.ta"
# stm32mp_nvmem TA
OPTEE_EXPORT_TA_NVMEM_DIR:stm32mp1common = "${ST_OPTEE_EXPORT_TA_OUTDIR}/ta/stm32mp_nvmem/"
OPTEE_EXPORT_TA_NVMEM_DIR:stm32mp2common = "${ST_OPTEE_EXPORT_TA_OUTDIR}/export-ta_arm64/ta/"
OPTEE_EXPORT_TA_NVMEM ?= "1a8342cc-81a5-4512-99fe-9e2b3e37d626.ta"
# trusted_keys TA
OPTEE_EXPORT_TA_TRUSTED_KEYS_DIR:stm32mp1common ?= "${ST_OPTEE_EXPORT_TA_OUTDIR}/ta/trusted_keys/"
OPTEE_EXPORT_TA_TRUSTED_KEYS_DIR:stm32mp2common ?= "${ST_OPTEE_EXPORT_TA_OUTDIR}/export-ta_arm64/ta/"
OPTEE_EXPORT_TA_TRUSTED_KEYS ?= "f04a0fe7-1f5d-4b9b-abf7-619b85b4ce8c.ta"

do_configure:prepend(){
    chmod 755 ${S}/scripts/bin_to_c.py
}

do_compile() {
    export CFLAGS="${CFLAGS} --sysroot=${STAGING_DIR_HOST}"
    export OPENSSL_MODULES=${STAGING_LIBDIR_NATIVE}/ossl-modules/

    unset i
    for config in ${OPTEE_CONFIG}; do
        i=$(expr $i + 1)
        # Initialize devicetree list and tf-a basename
        dt_config=$(echo ${OPTEE_DEVICETREE} | cut -d',' -f${i})
        extra_opt=$(echo ${OPTEE_EXTRA_OPTFLAGS} | cut -d',' -f${i})
        for dt in ${dt_config}; do
            oe_runmake -C ${S} O=${B}/${config}-${dt} CFG_EMBED_DTB_SOURCE_FILE=${dt}.dts ${extra_opt}
        done
    done
    # generate export-ta
    # Need to use the profile = secure_and_system_services
    if [ -n "${OPTEE_CONFIG}" ]; then
        oe_runmake -C ${S} O=${ST_OPTEE_EXPORT_TA_OUTDIR} CFG_EMBED_DTB_SOURCE_FILE=${ST_OPTEE_EXPORT_TA_REF_BOARD} CFG_STM32MP_PROFILE=secure_and_system_services ${ST_OPTEE_EXPORT_TA_OEMAKE_EXTRA}
    fi
}

do_install() {
    # install TA devkit
    if [ -d "${OPTEE_EXPORT_TA_DIR_ARM32}" ]; then
        install -d ${D}${includedir}/${OPTEE_EXPORT_TA_INSTALL_ARM32}
        for f in ${OPTEE_EXPORT_TA_DIR_ARM32}/* ; do
            cp -aRf "$f" ${D}${includedir}/${OPTEE_EXPORT_TA_INSTALL_ARM32}/
        done
    fi
    if [ -d "${OPTEE_EXPORT_TA_DIR_ARM64}" ]; then
        install -d ${D}${includedir}/${OPTEE_EXPORT_TA_INSTALL_ARM64}
        for f in ${OPTEE_EXPORT_TA_DIR_ARM64}/* ; do
            cp -aRf "$f" ${D}${includedir}/${OPTEE_EXPORT_TA_INSTALL_ARM64}/
        done
    fi
    # copy TA on target filesystem
    install -d ${D}${base_libdir}/optee_armtz/
    # avb TA
    if [ -f "${OPTEE_EXPORT_TA_AVB_DIR}${OPTEE_EXPORT_TA_AVB}" ]; then
        install -m 0644 ${OPTEE_EXPORT_TA_AVB_DIR}${OPTEE_EXPORT_TA_AVB} ${D}${base_libdir}/optee_armtz/
    fi
    # pkcs11 TA
    if [ -f "${OPTEE_EXPORT_TA_PKCS11_DIR}${OPTEE_EXPORT_TA_PKCS11}" ]; then
        install -m 0644 ${OPTEE_EXPORT_TA_PKCS11_DIR}${OPTEE_EXPORT_TA_PKCS11} ${D}${base_libdir}/optee_armtz/
    fi
    # remoteproc TA
    if [ -f "${OPTEE_EXPORT_TA_REMOTEPROC_DIR}${OPTEE_EXPORT_TA_REMOTEPROC}" ]; then
        install -m 0644 ${OPTEE_EXPORT_TA_REMOTEPROC_DIR}${OPTEE_EXPORT_TA_REMOTEPROC} ${D}${base_libdir}/optee_armtz/
    fi
    # stm32mp_nvmem TA
    if [ -f "${OPTEE_EXPORT_TA_NVMEM_DIR}${OPTEE_EXPORT_TA_NVMEM}" ]; then
        install -m 0644 ${OPTEE_EXPORT_TA_NVMEM_DIR}${OPTEE_EXPORT_TA_NVMEM} ${D}${base_libdir}/optee_armtz/
    fi
    # trusted_keys TA
    if [ -f "${OPTEE_EXPORT_TA_TRUSTED_KEYS_DIR}${OPTEE_EXPORT_TA_TRUSTED_KEYS}" ]; then
        install -m 0644 ${OPTEE_EXPORT_TA_TRUSTED_KEYS_DIR}${OPTEE_EXPORT_TA_TRUSTED_KEYS} ${D}${base_libdir}/optee_armtz/
    fi
    # remove empty folder
    if [ $(ls -1 ${D}${base_libdir}/optee_armtz/*.ta | wc -l) -eq 0 ]; then
        rm -rf ${D}${base_libdir}
    fi

    if [ -n "${MULTILIBS}" ]; then
        if [ "${MULTILIBS}" = "multilib:lib64" ]; then
            # hack to install on sdk
            install -d ${D}${SDKPATH}/sysroots/${PACKAGE_EXTRA_ARCHS:tune-aarch32}${TARGET_VENDOR_MULTILIB_ORIGINAL}-linux-gnueabi/${includedir}
            cp -fR ${D}${includedir}/* ${D}${SDKPATH}/sysroots/${PACKAGE_EXTRA_ARCHS:tune-aarch32}${TARGET_VENDOR_MULTILIB_ORIGINAL}-linux-gnueabi/${includedir}
        fi
    fi
}

# Configure optee binaries
OPTEE_HEADER    = "tee-header_v2"
OPTEE_PAGEABLE  = "tee-pageable_v2"
OPTEE_PAGER     = "tee-pager_v2"
OPTEE_SUFFIX    = "bin"
# Output the ELF generated
ELF_DEBUG_ENABLE ?= ""
OPTEE_ELF = "tee"
OPTEE_ELF_SUFFIX = "elf"

export_binaries() {
    local dest="${1}"
    install -d ${dest}

    unset i
    for config in ${OPTEE_CONFIG}; do
        i=$(expr $i + 1)
        # Initialize devicetree list and tf-a basename
        dt_config=$(echo ${OPTEE_DEVICETREE} | cut -d',' -f${i})
        extra_opt=$(echo ${OPTEE_EXTRA_OPTFLAGS} | cut -d',' -f${i})
        for dt in ${dt_config}; do
            install -m 644 ${B}/${config}-${dt}/core/${OPTEE_HEADER}.${OPTEE_SUFFIX} ${dest}/${OPTEE_HEADER}-${dt}-${config}.${OPTEE_SUFFIX}
            install -m 644 ${B}/${config}-${dt}/core/${OPTEE_PAGER}.${OPTEE_SUFFIX} ${dest}/${OPTEE_PAGER}-${dt}-${config}.${OPTEE_SUFFIX}
            install -m 644 ${B}/${config}-${dt}/core/${OPTEE_PAGEABLE}.${OPTEE_SUFFIX} ${dest}/${OPTEE_PAGEABLE}-${dt}-${config}.${OPTEE_SUFFIX}
            if [ -n "${ELF_DEBUG_ENABLE}" ]; then
                install -d ${dest}/debug
                install -m 644 ${B}/${config}-${dt}/core/${OPTEE_ELF}.${OPTEE_ELF_SUFFIX} ${dest}/debug/${OPTEE_ELF}-${dt}-${config}.${OPTEE_ELF_SUFFIX}
            fi
        done
    done
}

optee_sysroot_populate() {
    export_binaries ${SYSROOT_DESTDIR}${FIP_DIR_OPTEE}
}
SYSROOT_PREPROCESS_FUNCS =+ "optee_sysroot_populate"
SYSROOT_DIRS:append = " ${FIP_DIR_OPTEE}"

do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}${FIP_DIR_OPTEE}"
do_deploy() {
    export_binaries ${DEPLOYDIR}
}
addtask deploy before do_build after do_compile

PACKAGES =+ "${PN}-ta-avb ${PN}-ta-pkcs11 ${PN}-ta-remoteproc ${PN}-ta-stm32mp-nvmem ${PN}-ta-trusted-keys"
PACKAGES =+ "${PN}-dev-sdk"

FILES:${PN}-ta-avb = "${base_libdir}/optee_armtz/023f8f1a-292a-432b-8fc4-de8471358067.ta"
RPROVIDES:${PN}-ta-avb += "virtual-optee-os-ta-avb"
ALLOW_EMPTY:${PN-TA-avb} = "1"

FILES:${PN}-ta-pkcs11 = "${base_libdir}/optee_armtz/fd02c9da-306c-48c7-a49c-bbd827ae86ee.ta"
RPROVIDES:${PN}-ta-pkcs11 += "virtual-optee-os-ta-pkcs11"
ALLOW_EMPTY:${PN}-ta-pkcs11 = "1"

FILES:${PN}-ta-remoteproc = "${base_libdir}/optee_armtz/80a4c275-0a47-4905-8285-1486a9771a08.ta"
RPROVIDES:${PN}-ta-remoteproc += "virtual-optee-os-ta-remoteproc"
ALLOW_EMPTY:${PN}-ta-remoteproc = "1"

FILES:${PN}-ta-stm32mp-nvmem = "${base_libdir}/optee_armtz/1a8342cc-81a5-4512-99fe-9e2b3e37d626.ta"
RPROVIDES:${PN}-ta-stm32mp-nvmem += "virtual-optee-os-ta-stm32mp-nvmem"
ALLOW_EMPTY:${PN}-ta-stm32mp-nvmem = "1"

FILES:${PN}-ta-trusted-keys = "${base_libdir}/optee_armtz/f04a0fe7-1f5d-4b9b-abf7-619b85b4ce8c.ta"
RPROVIDES:${PN}-ta-trusted-keys += "virtual-optee-os-ta-trusted-keys"
ALLOW_EMPTY:${PN}-ta-trusted-keys = "1"

FILES:${PN} = "${nonarch_base_libdir}/firmware/"
FILES:${PN}-dev = "/usr/include/optee"
INSANE_SKIP:${PN}-dev = "staticdev"

FILES:${PN}-dev-sdk = "${SDKPATH}"
INSANE_SKIP:${PN}-dev-sdk = "staticdev buildpaths"
ALLOW_EMPTY:${PN}-dev-sdk = "1"

INHIBIT_PACKAGE_STRIP = "1"
# ---------------------------------------------------------------------
# Avoid QA Issue: contains reference to TMPDIR [buildpaths]
INSANE_SKIP:${PN} += "buildpaths"
INSANE_SKIP:${PN}-src += "buildpaths"
INSANE_SKIP:${PN}-dev += "buildpaths"
