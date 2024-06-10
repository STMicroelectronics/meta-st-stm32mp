FILESEXTRAPATHS:prepend := "${THISDIR}/optee-os:"

PACKAGE_ARCH = "${MACHINE_ARCH}"

PROVIDES += "virtual/optee-os virtual/optee-os-ta-pkcs11"
PROVIDES += "virtual/optee-os-ta-pkcs11 virtual/optee-os-ta-avb"
PROVIDES += "virtual/optee-os-ta-remoteproc virtual/optee-os-ta-stm32mp-nvmem"
PROVIDES += "virtual/optee-os-ta-trusted-keys"
RPROVIDES:${PN} += "virtual/optee-os virtual/systemd-bootconf"

CVE_PRODUCT = "op-tee:op-tee_os"

inherit external-dt
inherit fip-utils-stm32mp

B = "${WORKDIR}/build"
# Configure build dir for externalsrc class usage through devtool
EXTERNALSRC_BUILD:pn-${PN} = "${WORKDIR}/build"

DEPENDS += "dtc-native"
DEPENDS += "python3-pycryptodomex-native"
DEPENDS += "python3-pyelftools-native"
DEPENDS += "libgcc python3-cryptography-native"
DEPENDS += "python3-pillow-native"

inherit deploy python3native

OPTEEMACHINE ?= "${MACHINE}"
OPTEEOUTPUTMACHINE ?= "${MACHINE}"

# Default log level
ST_OPTEE_DEBUG_LOG_LEVEL ??= "2"

# default core debug
ST_OPTEE_CORE_DEBUG ??= "y"
ST_OPTEE_CORE_DEBUG:stm32mp15common ?= "n"

EXTRA_OEMAKE = "PLATFORM=${OPTEEMACHINE}"
EXTRA_OEMAKE += "CROSS_COMPILE_core=${HOST_PREFIX}"
EXTRA_OEMAKE += "CROSS_COMPILE_ta_arm64=${HOST_PREFIX}"
EXTRA_OEMAKE += "${@bb.utils.contains('TUNE_FEATURES', 'aarch64', 'ARCH=arm CFG_ARM64_core=y', 'ARCH=arm CFG_ARM32_core=y CROSS_COMPILE_ta_arm32=${HOST_PREFIX}', d)}"
EXTRA_OEMAKE += "NOWERROR=1"
EXTRA_OEMAKE += "LDFLAGS="
# Set external-dt support
EXTRA_OEMAKE += "${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', 'CFG_EXT_DTS=${STAGING_EXTDT_DIR}/${EXTDT_DIR_OPTEE}', '', d)}"

# debug and trace
EXTRA_OEMAKE += "${@bb.utils.contains('ST_OPTEE_DEBUG_TRACE', '1', 'CFG_TEE_CORE_LOG_LEVEL=${ST_OPTEE_DEBUG_LOG_LEVEL} CFG_TEE_CORE_DEBUG=${ST_OPTEE_CORE_DEBUG}', '', d)}"

# SCP firmware
EXTRA_OEMAKE_SCP_FIRMWARE ??= ""
EXTRA_OEMAKE_SCP_FIRMWARE:stm32mp15common ??= ""
EXTRA_OEMAKE_SCP_FIRMWARE:stm32mp13common ??= "${@bb.utils.contains('ENABLE_SCMI_SCPFW', '1', 'CFG_SCMI_SCPFW=y', 'CFG_SCMI_SCPFW=n', d)}"
EXTRA_OEMAKE_SCP_FIRMWARE:stm32mp25common ??= "CFG_SCMI_SCPFW=y"
STAGING_SCPFW_DIR = "${S}/core/lib/scmi-server/SCP-firmware"
EXTRA_OEMAKE += " ${EXTRA_OEMAKE_SCP_FIRMWARE}"

OPTEE_ARCH:armv7a = "arm32"
OPTEE_ARCH:armv7ve = "arm32"
OPTEE_ARCH:aarch64 = "arm64"

ST_OPTEE_PROFILE ??= "secure_and_system_services"
ST_OPTEE_PROFILE_internal = "${@';'.join([x+":"+ (d.getVar('ST_OPTEE_PROFILE:'+x) or d.getVar('ST_OPTEE_PROFILE')) for x in d.getVar('STM32MP_SOC_NAME').split()])}"

do_configure:prepend(){
    chmod 755 ${S}/scripts/bin_to_c.py
}

do_compile() {
    export CFLAGS="${CFLAGS} --sysroot=${STAGING_DIR_HOST}"
    export OPENSSL_MODULES=${STAGING_LIBDIR_NATIVE}/ossl-modules/
    if [ -n "${OPTEE_CONF}" ]; then
        for conf in ${OPTEE_CONF}; do
            # Configure SOC switch
            soc_extra=""
            i=0
            for soc in ${STM32MP_SOC_NAME}; do
                i=$(expr $i + 1)
                soc_profile_conf=$(echo "${ST_OPTEE_PROFILE_internal}" | cut -d';' -f${i} | cut -d':' -f 2)
                if [ "$(echo ${conf} | grep -c ${soc})" -eq 1 ]; then
                    soc_extra="$(echo CFG_${soc} | awk '{print toupper($0)}')=y CFG_STM32MP_PROFILE=${soc_profile_conf}"
                    if [ "${soc}" = "stm32mp15" ]; then
                        if [ "${soc_profile_conf}" = "secure_and_system_services" ]; then
                            soc_extra="${soc_extra} CFG_STM32MP1_OPTEE_IN_SYSRAM=y"
                        fi
                    fi
                    break
                fi
            done
            oe_runmake -C ${S} O=${B}/${conf} CFG_EMBED_DTB_SOURCE_FILE=${conf}.dts ${soc_extra}
        done
    else
        oe_runmake -C ${S} O=${B}/out
    fi
}
ST_OPTEE_EXPORT_TA_REF_BOARD = "stm32mp157f-dk2.dts"
ST_OPTEE_EXPORT_TA_OEMAKE_EXTRA = "CFG_WITH_TUI=y"
# --------------------------------------------
# specific to stm32mp1
ST_OPTEE_EXPORT_TA_REF_BOARD:stm32mp1common ??= "stm32mp157f-dk2.dts"
do_compile:append:stm32mp1common() {
    # generate export-ta
    # Need to use the profile = secure_and_system_services and stm32mp157f-dk2 devicetree
    if [ -n "${OPTEE_CONF}" ]; then
        for soc in ${STM32MP_SOC_NAME}; do
            i=$(expr $i + 1)
            if [ "$(echo ${ST_OPTEE_EXPORT_TA_REF_BOARD} | grep -c ${soc})" -eq 1 ]; then
                soc_extra="$(echo CFG_${soc} | awk '{print toupper($0)}')=y"
                break
            fi
        done
        oe_runmake -C ${S} O=${B}/export-ta CFG_EMBED_DTB_SOURCE_FILE=${ST_OPTEE_EXPORT_TA_REF_BOARD} CFG_STM32MP_PROFILE=secure_and_system_services ${soc_extra} ${ST_OPTEE_EXPORT_TA_OEMAKE_EXTRA}
    fi
}

do_install:stm32mp1common() {
    #install TA devkit
    install -d ${D}${includedir}/optee/export-user_ta/
    install -d ${D}${base_libdir}/optee_armtz/

    if [ -n "${OPTEE_CONF}" ]; then
        for conf in ${OPTEE_CONF}; do
            for f in  ${B}/export-ta/export-ta_${OPTEE_ARCH}/* ; do
                cp -aRf  $f ${D}${includedir}/optee/export-user_ta/
            done
            # copy TA on target filesystem
            if [ -d ${B}/export-ta/export-ta_${OPTEE_ARCH}/ta ]; then
                # avb TA
                if [ -f ${B}/export-ta/export-ta_${OPTEE_ARCH}/ta/023f8f1a-292a-432b-8fc4-de8471358067.ta ]; then
                    install -m 0644 ${B}/export-ta/export-ta_${OPTEE_ARCH}/ta/023f8f1a-292a-432b-8fc4-de8471358067.ta ${D}${base_libdir}/optee_armtz/
                fi
                # pkcs11 TA
                if [ -f ${B}/export-ta/export-ta_${OPTEE_ARCH}/ta/fd02c9da-306c-48c7-a49c-bbd827ae86ee.ta ]; then
                    install -m 0644 ${B}/export-ta/export-ta_${OPTEE_ARCH}/ta/fd02c9da-306c-48c7-a49c-bbd827ae86ee.ta ${D}${base_libdir}/optee_armtz/
                fi
                # remoteproc TA
                if [ -f ${B}/export-ta/export-ta_${OPTEE_ARCH}/ta/80a4c275-0a47-4905-8285-1486a9771a08.ta ]; then
                    install -m 0644 ${B}/export-ta/export-ta_${OPTEE_ARCH}/ta/80a4c275-0a47-4905-8285-1486a9771a08.ta ${D}${base_libdir}/optee_armtz/
                fi
                # stm32mp_nvmem TA
                if [ -f ${B}/export-ta/export-ta_${OPTEE_ARCH}/ta/1a8342cc-81a5-4512-99fe-9e2b3e37d626.ta ]; then
                    install -m 0644 ${B}/export-ta/export-ta_${OPTEE_ARCH}/ta/1a8342cc-81a5-4512-99fe-9e2b3e37d626.ta ${D}${base_libdir}/optee_armtz/
                fi
                # trusted_keys TA
                if [ -f ${B}/export-ta/export-ta_${OPTEE_ARCH}/ta/f04a0fe7-1f5d-4b9b-abf7-619b85b4ce8c.ta ]; then
                    install -m 0644 ${B}/export-ta/export-ta_${OPTEE_ARCH}/ta/f04a0fe7-1f5d-4b9b-abf7-619b85b4ce8c.ta ${D}${base_libdir}/optee_armtz/
                fi
            fi
            break;
        done
        if [ $(ls -1 ${D}${base_libdir}/optee_armtz/*.ta | wc -l) -eq 0 ]; then
            rm -rf ${D}${base_libdir}
        fi
    fi
}
# --------------------------------------------
# specific to stm32mp2
ST_OPTEE_EXPORT_TA_REF_BOARD:stm32mp2common ??= "stm32mp257f-dk.dts"
do_compile:append:stm32mp2common() {
    # generate export-ta
    # Need to use the profile = secure_and_system_services and stm32mp257f-dk devicetree
    if [ -n "${OPTEE_CONF}" ]; then
        for soc in ${STM32MP_SOC_NAME}; do
            i=$(expr $i + 1)
            if [ "$(echo ${ST_OPTEE_EXPORT_TA_REF_BOARD} | grep -c ${soc})" -eq 1 ]; then
                soc_extra="$(echo CFG_${soc} | awk '{print toupper($0)}')=y"
                break
            fi
        done
        oe_runmake -C ${S} O=${B}/export-ta CFG_EMBED_DTB_SOURCE_FILE=${ST_OPTEE_EXPORT_TA_REF_BOARD} CFG_STM32MP_PROFILE=secure_and_system_services ${soc_extra} ${ST_OPTEE_EXPORT_TA_OEMAKE_EXTRA}
    fi
}

do_install:stm32mp2common() {
    #install TA devkit
    install -d ${D}${includedir}/optee/export-user_ta_arm32/
    install -d ${D}${includedir}/optee/export-user_ta_arm64/
    install -d ${D}${base_libdir}/optee_armtz/

    if [ -n "${OPTEE_CONF}" ]; then
        for conf in ${OPTEE_CONF}; do
            if [ -d  ${B}/export-ta/export-ta_arm32 ]; then
                for f in  ${B}/export-ta/export-ta_arm32/* ; do
                    cp -aRf  $f ${D}${includedir}/optee/export-user_ta_arm32/
                done
            fi
            if [ -d  ${B}/export-ta/export-ta_arm64 ]; then
                for f in  ${B}/export-ta/export-ta_arm64/* ; do
                    cp -aRf $f ${D}${includedir}/optee/export-user_ta_arm64/
                done
                # copy TA on target filesystem
                # avb TA
                if [ -f ${B}/export-ta/export-ta_arm64/ta/023f8f1a-292a-432b-8fc4-de8471358067.ta ]; then
                    install -m 0644 ${B}/export-ta/export-ta_arm64/ta/023f8f1a-292a-432b-8fc4-de8471358067.ta ${D}${base_libdir}/optee_armtz/
                fi
                # pkcs11 TA
                if [ -f ${B}/export-ta/export-ta_arm64/ta/fd02c9da-306c-48c7-a49c-bbd827ae86ee.ta ]; then
                    install -m 0644 ${B}/export-ta/export-ta_arm64/ta/fd02c9da-306c-48c7-a49c-bbd827ae86ee.ta ${D}${base_libdir}/optee_armtz/
                fi
                # remoteproc TA
                if [ -f ${B}/export-ta/export-ta_arm64/ta/80a4c275-0a47-4905-8285-1486a9771a08.ta ]; then
                    install -m 0644 ${B}/export-ta/export-ta_arm64/ta/80a4c275-0a47-4905-8285-1486a9771a08.ta ${D}${base_libdir}/optee_armtz/
                fi
                # stm32mp_nvmem TA
                if [ -f ${B}/export-ta/export-ta_arm64/ta/1a8342cc-81a5-4512-99fe-9e2b3e37d626.ta ]; then
                    install -m 0644 ${B}/export-ta/export-ta_arm64/ta/1a8342cc-81a5-4512-99fe-9e2b3e37d626.ta ${D}${base_libdir}/optee_armtz/
                fi
                # trusted_keys TA
                if [ -f ${B}/export-ta/export-ta_arm64/ta/f04a0fe7-1f5d-4b9b-abf7-619b85b4ce8c.ta ]; then
                    install -m 0644 ${B}/export-ta/export-ta_arm64/ta/f04a0fe7-1f5d-4b9b-abf7-619b85b4ce8c.ta ${D}${base_libdir}/optee_armtz/
                fi
            fi
            break;
        done
        if [ $(ls -1 ${D}${base_libdir}/optee_armtz/*.ta | wc -l) -eq 0 ]; then
            rm -rf ${D}${base_libdir}
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
    if [ -n "${OPTEE_CONF}" ]; then
        for conf in ${OPTEE_CONF}; do
            install -m 644 ${B}/${conf}/core/${OPTEE_HEADER}.${OPTEE_SUFFIX} ${dest}/${OPTEE_HEADER}-${conf}.${OPTEE_SUFFIX}
            install -m 644 ${B}/${conf}/core/${OPTEE_PAGER}.${OPTEE_SUFFIX} ${dest}/${OPTEE_PAGER}-${conf}.${OPTEE_SUFFIX}
            install -m 644 ${B}/${conf}/core/${OPTEE_PAGEABLE}.${OPTEE_SUFFIX} ${dest}/${OPTEE_PAGEABLE}-${conf}.${OPTEE_SUFFIX}
            if [ -n "${ELF_DEBUG_ENABLE}" ]; then
                install -d ${dest}/debug
                install -m 644 ${B}/${conf}/core/${OPTEE_ELF}.${OPTEE_ELF_SUFFIX} ${dest}/debug/${OPTEE_ELF}-${conf}.${OPTEE_ELF_SUFFIX}
            fi
        done
    else
        install -m 644 ${B}/core/${OPTEE_HEADER}.${OPTEE_SUFFIX} ${dest}/${OPTEE_HEADER}.${OPTEE_SUFFIX}
        install -m 644 ${B}/core/${OPTEE_PAGER}.${OPTEE_SUFFIX} ${dest}/${OPTEE_PAGER}.${OPTEE_SUFFIX}
        install -m 644 ${B}/core/${OPTEE_PAGEABLE}.${OPTEE_SUFFIX} ${dest}/${OPTEE_PAGEABLE}.${OPTEE_SUFFIX}
        if [ -n "${ELF_DEBUG_ENABLE}" ]; then
            install -d ${dest}/debug
            install -m 644 ${B}/core/${OPTEE_ELF}.${OPTEE_ELF_SUFFIX} ${dest}/debug/${OPTEE_ELF}.${OPTEE_ELF_SUFFIX}
        fi
    fi
}

do_install:append() {
    export_binaries ${D}${FIP_DIR_OPTEE}
}

do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}${FIP_DIR_OPTEE}"
do_deploy() {
    export_binaries ${DEPLOYDIR}
}
addtask deploy before do_build after do_compile

PACKAGES =+ "${PN}-ta-avb ${PN}-ta-pkcs11 ${PN}-ta-remoteproc ${PN}-ta-stm32mp-nvmem ${PN}-ta-trusted-keys ${PN}-fip"

FILES:${PN}-ta-avb = "${base_libdir}/optee_armtz/023f8f1a-292a-432b-8fc4-de8471358067.ta"
RPROVIDES:${PN}-ta-avb += "virtual/optee-os-ta-avb"
ALLOW_EMPTY:${PN-TA-avb} = "1"

FILES:${PN}-ta-pkcs11 = "${base_libdir}/optee_armtz/fd02c9da-306c-48c7-a49c-bbd827ae86ee.ta"
RPROVIDES:${PN}-ta-pkcs11 += "virtual/optee-os-ta-pkcs11"
ALLOW_EMPTY:${PN}-ta-pkcs11 = "1"

FILES:${PN}-ta-remoteproc = "${base_libdir}/optee_armtz/80a4c275-0a47-4905-8285-1486a9771a08.ta"
RPROVIDES:${PN}-ta-remoteproc += "virtual/optee-os-ta-remoteproc"
ALLOW_EMPTY:${PN}-ta-remoteproc = "1"

FILES:${PN}-ta-stm32mp-nvmem = "${base_libdir}/optee_armtz/1a8342cc-81a5-4512-99fe-9e2b3e37d626.ta"
RPROVIDES:${PN}-ta-stm32mp-nvmem += "virtual/optee-os-ta-stm32mp-nvmem"
ALLOW_EMPTY:${PN}-ta-stm32mp-nvmem = "1"

FILES:${PN}-ta-trusted-keys = "${base_libdir}/optee_armtz/f04a0fe7-1f5d-4b9b-abf7-619b85b4ce8c.ta"
RPROVIDES:${PN}-ta-trusted-keys += "virtual/optee-os-ta-trusted-keys"
ALLOW_EMPTY:${PN}-ta-trusted-keys = "1"

FILES:${PN} = "${nonarch_base_libdir}/firmware/"
FILES:${PN}-dev = "/usr/include/optee"
FILES:${PN}-fip = "${FIP_DIR_OPTEE}"
SYSROOT_DIRS:append = " ${FIP_DIR_OPTEE}"

INSANE_SKIP:${PN}-dev = "staticdev"

INHIBIT_PACKAGE_STRIP = "1"
# ---------------------------------------------------------------------
# Avoid QA Issue: contains reference to TMPDIR [buildpaths]
INSANE_SKIP:${PN} += "buildpaths"
