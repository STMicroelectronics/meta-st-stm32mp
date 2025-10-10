FILESEXTRAPATHS:prepend := "${THISDIR}/tf-a-stm32mp:"

PROVIDES += "virtual/trusted-firmware-a"

PACKAGE_ARCH = "${MACHINE_ARCH}"

inherit deploy
inherit sign-stm32mp
inherit fip-utils-stm32mp
inherit external-dt

# Include TF-A config definitions
require tf-a-stm32mp-config.inc

# ------------------------------------
# Set MBEDTLS support
TFA_MBEDTLS_DIR ?= "mbedtls"
# MBEDTLS v3.6.2
SRC_URI_MBEDTLS = "git://github.com/ARMmbed/mbedtls.git;protocol=https;destsuffix=git/${TFA_MBEDTLS_DIR};nobranch=1;name=mbedtls"
SRCREV_mbedtls = "107ea89daaefb9867ea9121002fbbdf926780e98"
LIC_FILES_CHKSUM_MBEDTLS = "file://mbedtls/LICENSE;md5=379d5819937a6c2f1ef1630d341e026d"
LICENSE_MBEDTLS = "Apache-2.0"
# Add MBEDTLS to our sources
SRC_URI:append = " ${@bb.utils.contains('SIGN_ENABLE', '1', '${SRC_URI_MBEDTLS}', '', d)}"
# Update license variables
LICENSE:append = "${@bb.utils.contains('SIGN_ENABLE', '1', ' & ${LICENSE_MBEDTLS}', '', d)}"
LIC_FILES_CHKSUM:append = "${@bb.utils.contains('SIGN_ENABLE', '1', ' ${LIC_FILES_CHKSUM_MBEDTLS}', '', d)}"
# Add mbed TLS to version
SRCREV_FORMAT:append = "${@bb.utils.contains('SIGN_ENABLE', '1', '_mbedtls', '', d)}"
# ------------------------------------

B = "${WORKDIR}/build"
# Configure build dir for externalsrc class usage through devtool
EXTERNALSRC_BUILD:pn-${PN} = "${WORKDIR}/build"

DEPENDS += "dtc-native openssl-native"
DEPENDS += "u-boot-tools-stm32mp-native"

DEPENDS:append = " ${@bb.utils.contains('TF_A_ENABLE_DEBUG_WRAPPER', '1', 'stm32wrapper4dbg-native', '', d)}"
DEPENDS:append = " ${@bb.utils.contains('TF_A_FWDDR', '1', 'stm32mp-ddr-phy', '', d)}"

# Default log level
ST_TF_A_DEBUG ??= "1"
ST_TF_A_DEBUG_TRACE ??= "0"
ST_TF_A_LOG_LEVEL_RELEASE ??= "20"
ST_TF_A_LOG_LEVEL_DEBUG ??= "40"

# Configure make settings
EXTRA_OEMAKE += 'PLAT=${TFA_PLATFORM}'
EXTRA_OEMAKE += 'ARCH=${TFA_ARM_ARCH}'
EXTRA_OEMAKE += 'ARM_ARCH_MAJOR=${TFA_ARM_MAJOR}'
EXTRA_OEMAKE += 'CROSS_COMPILE=${TARGET_PREFIX}'
# Debug support
EXTRA_OEMAKE += "${@bb.utils.contains('ST_TF_A_DEBUG_TRACE', '1', 'DEBUG=${ST_TF_A_DEBUG}', '', d)}"
EXTRA_OEMAKE += "${@bb.utils.contains('ST_TF_A_DEBUG_TRACE', '1', 'LOG_LEVEL=${ST_TF_A_LOG_LEVEL_DEBUG}', 'LOG_LEVEL=${ST_TF_A_LOG_LEVEL_RELEASE}', d)}"

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

# Output the firwmare ddr
TF_A_FWDDR ?= "0"
TF_A_FWDDR:stm32mp2common = "1"

FWDDR_NAME ?= "ddr_pmu"
FWDDR_SUFFIX ?= "bin"

FWDDR_SRC ?= "${STAGING_DATADIR}"
FWDDR_SRC_SUBDIR ?= ""
FWDDR_SRC_SUBDIR:stm32mp2common = "stm32mp2"
FWDDR_DIR ?= "${FWDDR_SRC}/${FWDDR_SRC_SUBDIR}"

# Set default TF-A config
TF_A_CONFIG ?= ""

# Enable the wrapper for debug
TF_A_ENABLE_DEBUG_WRAPPER ??= "${@bb.utils.contains('MACHINE_FEATURES', 'm33td', '0', '1', d)}"

# Set default configuration to allow signing
TF_A_SIGN_SUFFIX ??= "${@bb.utils.contains('SIGN_ENABLE', '1', '${SIGN_SUFFIX}', '', d)}"
TF_A_SIGN_OF ?= "0x00000001"

TF_A_ENCRYPT_SUFFIX ??= "${@bb.utils.contains('ENCRYPT_ENABLE', '1', '${ENCRYPT_SUFFIX}', '', d)}"
TF_A_ENCRYPT_DC ?= "0x0E5F2025"
TF_A_ENCRYPT_DC:stm32mp1common ?= "0x0E5F2025"
TF_A_ENCRYPT_DC:stm32mp2common ?= "0x25205f0e"

TF_A_ENCRYPT_IMGVER ?= "0"
TF_A_ENCRYPT_OF ?= "0x80000003"

# Set metadata generation
TF_A_ENABLE_METADATA ??= "${@bb.utils.contains('MACHINE_FEATURES', 'fw-update', '1', '0', d)}"
TF_A_METADATA_NAME ?= "metadata"
TF_A_METADATA_SUFFIX ?= "bin"
TF_A_METADATA_BINARY ??= "${TF_A_METADATA_NAME}.${TF_A_METADATA_SUFFIX}"

TF_A_METADATA_TOOL_NAME ?= "mkfwumdata"
TF_A_METADATA_TOOL ?= "${STAGING_BINDIR_NATIVE}/${TF_A_METADATA_TOOL_NAME}"
TF_A_METADATA_TOOL_VERSION ?= "2"
TF_A_METADATA_TOOL_ARGS ?= "-g -i 1 -b 2 -v ${TF_A_METADATA_TOOL_VERSION} ${DEVICE_PARTUUID_FWU_MDATA},${DEVICE_TYPEUUID_FIP},${DEVICE_PARTUUID_FIP_A},${DEVICE_PARTUUID_FIP_B}"

# Configure specific build flags
EXTRA_OEMAKE += "${@bb.utils.contains('SIGN_ENABLE', '1', 'TRUSTED_BOARD_BOOT=1', '', d)}"
EXTRA_OEMAKE += "${@bb.utils.contains('SIGN_ENABLE', '1', 'MBEDTLS_DIR=${S}/${TFA_MBEDTLS_DIR}', '', d)}"
EXTRA_OEMAKE:append:stm32mp2common = " ${@bb.utils.contains('SIGN_ENABLE', '1', 'BRANCH_PROTECTION=0', '', d)} "

EXTRA_OEMAKE += "${@bb.utils.contains('ENCRYPT_ENABLE', '1', 'DECRYPTION_SUPPORT=aes_gcm ENCRYPT_BL32=1', '', d)}"
EXTRA_OEMAKE += "${@bb.utils.contains('ENCRYPT_ENABLE', '1', bb.utils.contains('FIP_BL31_ENABLE', '1', 'ENCRYPT_BL31=1', '', d), '', d)} "

# Addons parameters for SIGN_TOOL
SIGN_TOOL_EXTRA ?= ""
# stm32mp13 = header-version 2
SIGN_TOOL_EXTRA_stm32mp13 = " ${@bb.utils.contains('ENCRYPT_ENABLE', '1', '-of ${TF_A_ENCRYPT_OF}', '-of ${TF_A_SIGN_OF}', d)}"
# stm32mp15 = header-version 1
SIGN_TOOL_EXTRA_stm32mp15 = " ${@bb.utils.contains('ENCRYPT_ENABLE', '1', '-of ${TF_A_ENCRYPT_OF}', '', d)}"
SIGN_TOOL_EXTRA_stm32mp21 = " --header-version 2.3 ${@bb.utils.contains('ENCRYPT_ENABLE', '1', '-of ${TF_A_ENCRYPT_OF}', '-of ${TF_A_SIGN_OF}', d)}"
SIGN_TOOL_EXTRA_stm32mp23 = " --header-version 2.2 ${@bb.utils.contains('ENCRYPT_ENABLE', '1', '-of ${TF_A_ENCRYPT_OF}', '-of ${TF_A_SIGN_OF}', d)}"
SIGN_TOOL_EXTRA_stm32mp25 = " --header-version 2.2 ${@bb.utils.contains('ENCRYPT_ENABLE', '1', '-of ${TF_A_ENCRYPT_OF}', '-of ${TF_A_SIGN_OF}', d)}"

# -----------------------------------------------
# Enable use of work-shared folder
TFA_SHARED_SOURCES ??= "1"
STAGING_TFA_DIR = "${TMPDIR}/work-shared/${MACHINE}/tfa-source"
DEBUG_PREFIX_MAP += " ${@bb.utils.contains('TFA_SHARED_SOURCES', '1', '-fmacro-prefix-map=${STAGING_TFA_DIR}=${TARGET_DBGSRC_DIR}', '', d)} "
DEBUG_PREFIX_MAP += " ${@bb.utils.contains('TFA_SHARED_SOURCES', '1', '-fdebug-prefix-map=${STAGING_TFA_DIR}=${TARGET_DBGSRC_DIR}', '', d)} "

# Make sure to move ${S} to STAGING_TFA_DIR. We can't just
# create the symlink in advance as the git fetcher can't cope with
# the symlink.
do_unpack[cleandirs] += "${S}"
do_unpack[cleandirs] += "${@bb.utils.contains('TFA_SHARED_SOURCES', '1', '${STAGING_TFA_DIR}', '', d)}"
do_clean[cleandirs] += "${S}"
do_clean[cleandirs] += "${@bb.utils.contains('TFA_SHARED_SOURCES', '1', '${STAGING_TFA_DIR}', '', d)}"
base_do_unpack:append () {
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
        dt_suffix=$(echo ${TF_A_DT_SUFFIX} | cut -d',' -f${i})
        [ "${EXTDT_USE_SUFFIX}" = "1" ] || dt_suffix=""
        extra_opt=$(echo ${TF_A_EXTRA_OPTFLAGS} | cut -d',' -f${i})
        tfa_basename=$(echo ${TF_A_BINARIES} | cut -d',' -f${i})
        tf_a_make_target=$(echo ${TF_A_MAKE_TARGET} | cut -d',' -f${i})
        for dt in ${dt_config}; do
            # Init specific soc settings
            soc_extra_opt=""
            soc_suffix=""
            soc_name=""
            if [ -n "${STM32MP_SOC_NAME}" ]; then
                for soc in ${STM32MP_SOC_NAME}; do
                    if [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ]; then
                        soc_extra_opt="$(echo ${soc} | awk '{print toupper($0)}')=1"
                        soc_suffix="-${soc}"

                        SIGN_TOOL_EXTRA_soc="${SIGN_TOOL_EXTRA}"
                        case ${soc} in
                        stm32mp13)
                            SIGN_TOOL_EXTRA_soc="${SIGN_TOOL_EXTRA_stm32mp13}"
                            ;;
                        stm32mp15)
                            SIGN_TOOL_EXTRA_soc="${SIGN_TOOL_EXTRA_stm32mp15}"
                            if echo ${config} | grep -q 'optee-'; then
                                soc_extra_opt="${soc_extra_opt} STM32MP1_OPTEE_IN_SYSRAM=1"
                            fi
                            ;;
                        stm32mp21)
                            SIGN_TOOL_EXTRA_soc="${SIGN_TOOL_EXTRA_stm32mp21}"
                            ;;
                        stm32mp23)
                            SIGN_TOOL_EXTRA_soc="${SIGN_TOOL_EXTRA_stm32mp23}"
                            ;;
                        stm32mp25)
                            SIGN_TOOL_EXTRA_soc="${SIGN_TOOL_EXTRA_stm32mp25}"
                            ;;
                        esac
                    fi
                done
            fi
            build_dir="${B}/${config}${soc_suffix}-${dt}${dt_suffix}"
            mkdir -p "${build_dir}"
            if [ "${TF_A_ENABLE_METADATA}" = "1" ]; then
                rm -rf "${build_dir}/${TF_A_METADATA_NAME}.${TF_A_METADATA_SUFFIX}"
                ${TF_A_METADATA_TOOL} ${TF_A_METADATA_TOOL_ARGS} "${B}/${TF_A_METADATA_NAME}.${TF_A_METADATA_SUFFIX}"
            fi

            # generate dt to check the content
            oe_runmake -C "${S}" BUILD_PLAT="${build_dir}" DTB_FILE_NAME="${dt}${dt_suffix}.dtb" ${extra_opt} ${soc_extra_opt} dtbs

            # check which pmic1l is present on dtb
            if [ -f "${build_dir}/fdts/${dt}${dt_suffix}-bl2.dtb" ]; then
                pcmi1_present=$(${STAGING_BINDIR_NATIVE}/fdtdump ${build_dir}/fdts/${dt}${dt_suffix}-bl2.dtb 2>/dev/null | grep  -c "st,stpmic1l" || ${HOSTTOOLS_DIR}/true)
                if [ "${pcmi1_present}" -gt 0 ]; then
                    # st pmic1l is present, need to force to compilation with specific pcmi1l optionn
                    soc_extra_opt="${soc_extra_opt} STM32MP_STPMIC1L=1"
                fi
            fi

            # Init specific ddr settings
            ddr_extra_opt=""
            if [ "${TF_A_FWDDR}" = "1" ]; then
                # Detect ddr type if it's present
                if [ -f "${build_dir}/fdts/${dt}${dt_suffix}-bl2.dtb" ]; then
                    ddr_dtb_node=$(${STAGING_BINDIR_NATIVE}/fdtget -l ${build_dir}/fdts/${dt}${dt_suffix}-bl2.dtb /soc | grep ddr | head -n 1)
                    ddr_propertie=$(${STAGING_BINDIR_NATIVE}/fdtget ${build_dir}/fdts/${dt}${dt_suffix}-bl2.dtb /soc/${ddr_dtb_node} st,mem-name || echo "none")
                    ddr_target=""
                    # potentials value of ddr_propertie:
                    # DDR3 16bits
                    # DDR4 32bits
                    # DDR4 8Gbits
                    # LPDDR4 32bits
                    case ${ddr_propertie} in
                        DDR3*)
                            ddr_extra_opt=" STM32MP_DDR3_TYPE=1 "
                            ddr_target="ddr3"
                            ;;
                        DDR4*)
                            ddr_extra_opt=" STM32MP_DDR4_TYPE=1 "
                            ddr_target="ddr4"
                            ;;
                        LPDDR4*)
                            ddr_extra_opt=" STM32MP_LPDDR4_TYPE=1 "
                            ddr_target="lpddr4"
                            ;;
                        *)
                            bbfatal "Missing st,mem-name information for ${dt}${dt_suffix}"
                            ;;
                    esac
                    bbnote "${dt}${dt_suffix}: ${tf_a_make_target} -> ${ddr_extra_opt}"
                    # Copy TF-A ddr binary with explicit devicetree filename
                    if [ -s "${FWDDR_DIR}/${ddr_target}_pmu_train.bin" ]; then
                        install -m 644 "${FWDDR_DIR}/${ddr_target}_pmu_train.bin" "${build_dir}/${FWDDR_NAME}-${dt}-${config}.${FWDDR_SUFFIX}"
                    else
                        bbfatal "Missing ddr firmware file ${ddr_target}_pmu_train.bin for ${dt}${dt_suffix}"
                    fi
                fi
            fi

            encrypt_extra_opt=""
            if [ "${ENCRYPT_ENABLE}" = "1" ]; then
                    encrypt_key="${ENCRYPT_FIP_KEY_PATH_LIST}"
                    if [ -n "${STM32MP_ENCRYPT_SOC_NAME}" ]; then
                        unset k
                        for soc in ${STM32MP_ENCRYPT_SOC_NAME}; do
                            k=$(expr $k + 1)
                            [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] && encrypt_key=$(echo ${ENCRYPT_FIP_KEY_PATH_LIST} | cut -d',' -f${k})
                        done
                    fi
                    if [ "$(file "${encrypt_key}" | sed 's#.*: \(.*\)$#\1#')" = "ASCII text" ]; then
                        # The encryption key is already available in hexadecimal format, so just extract it from file
                        encrypt_key="$(cat ${encrypt_key})"
                    else
                        encrypt_key="$(hexdump -e '/1 "%02x"' ${encrypt_key})"
                    fi
                    encrypt_extra_opt="ENC_KEY=${encrypt_key}"
            fi

            oe_runmake -C "${S}" BUILD_PLAT="${build_dir}" DTB_FILE_NAME="${dt}${dt_suffix}.dtb" ${extra_opt} ${soc_extra_opt} ${ddr_extra_opt} ${encrypt_extra_opt} ${tf_a_make_target}
            if [ -f "${build_dir}/bl2.bin" ]; then
                    cp "${build_dir}/bl2.bin" "${build_dir}/${tfa_basename}-${dt}-${config}.bin"
            fi
            # Copy TF-A binary with explicit devicetree filename
            if [ -f "${build_dir}/${tfa_basename}-${dt}${dt_suffix}.${TF_A_SUFFIX}" ]; then
                cp "${build_dir}/${tfa_basename}-${dt}${dt_suffix}.${TF_A_SUFFIX}" "${build_dir}/${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}"
                if [ "${TF_A_ENABLE_DEBUG_WRAPPER}" = "1" ]; then
                    stm32wrapper4dbg -s "${build_dir}/${tfa_basename}-${dt}${dt_suffix}.${TF_A_SUFFIX}" -d "${build_dir}/debug-${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}"
                fi

                if [ "${SIGN_ENABLE}" = "1" ]; then
                    # Init sign key for signing tools
                    sign_key="${SIGN_KEY_PATH_LIST}"
                    if [ -n "${STM32MP_SOC_NAME}" ]; then
                        unset k
                        for soc in ${STM32MP_SOC_NAME}; do
                            k=$(expr $k + 1)
                            [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] && sign_key=$(echo ${SIGN_KEY_PATH_LIST} | cut -d',' -f${k})
                        done
                    fi
                    # Init default encryption options for signing tool
                    tf_a_encrypt_opts=""
                    if [ "${ENCRYPT_ENABLE}" = "1" ]; then
                        # Init encrypt key for signing tools
                        encrypt_key="${ENCRYPT_FSBL_KEY_PATH_LIST}"
                        if [ -n "${STM32MP_ENCRYPT_SOC_NAME}" ]; then
                            unset k
                            for soc in ${STM32MP_ENCRYPT_SOC_NAME}; do
                                k=$(expr $k + 1)
                                [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] && encrypt_key=$(echo ${ENCRYPT_FSBL_KEY_PATH_LIST} | cut -d',' -f${k})
                            done
                        fi
                        # Set encryption options for signing tools
                        tf_a_encrypt_opts="\
                            --enc-key ${encrypt_key} \
                            --enc-dc ${TF_A_ENCRYPT_DC} \
                            --image-version ${TF_A_ENCRYPT_IMGVER} \
                            "
                    fi
                    # Sign tf-a binary
                    bbnote "${SIGN_TOOL} \
                        -bin "${build_dir}/${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}" \
                        -o "${build_dir}/${tfa_basename}-${dt}-${config}${TF_A_ENCRYPT_SUFFIX}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" \
                        --password ${SIGN_KEY_PASS} \
                        --public-key $(ls -1 $(dirname ${sign_key})/publicKey*.pem | tr '\n' '\t') \
                        --private-key ${sign_key} \
                        --type fsbl \
                        --silent \
                        ${SIGN_TOOL_EXTRA_soc} \
                        ${tf_a_encrypt_opts} "

                    ${SIGN_TOOL} \
                        -bin "${build_dir}/${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}" \
                        -o "${build_dir}/${tfa_basename}-${dt}-${config}${TF_A_ENCRYPT_SUFFIX}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" \
                        --password ${SIGN_KEY_PASS} \
                        --public-key $(ls -1 $(dirname ${sign_key})/publicKey*.pem | tr '\n' '\t') \
                        --private-key ${sign_key} \
                        --type fsbl \
                        --silent \
                        ${SIGN_TOOL_EXTRA_soc} \
                        ${tf_a_encrypt_opts}
                    if [ "${TF_A_ENABLE_DEBUG_WRAPPER}" = "1" ]; then
                         bbnote "${SIGN_TOOL} \
                            -bin "${build_dir}/debug-${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}" \
                            -o "${build_dir}/debug-${tfa_basename}-${dt}-${config}${TF_A_ENCRYPT_SUFFIX}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" \
                            --password ${SIGN_KEY_PASS} \
                            --public-key $(ls -1 $(dirname ${sign_key})/publicKey*.pem | tr '\n' '\t') \
                            --private-key "${sign_key}" \
                            --type fsbl \
                            --silent \
                            ${SIGN_TOOL_EXTRA_soc} \
                            ${tf_a_encrypt_opts}"

                        ${SIGN_TOOL} \
                            -bin "${build_dir}/debug-${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}" \
                            -o "${build_dir}/debug-${tfa_basename}-${dt}-${config}${TF_A_ENCRYPT_SUFFIX}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" \
                            --password ${SIGN_KEY_PASS} \
                            --public-key $(ls -1 $(dirname ${sign_key})/publicKey*.pem | tr '\n' '\t') \
                            --private-key "${sign_key}" \
                            --type fsbl \
                            --silent \
                            ${SIGN_TOOL_EXTRA_soc} \
                            ${tf_a_encrypt_opts}
                    fi
                fi
            fi
        done
    done

    if [ "${TF_A_ENABLE_METADATA}" = "1" ]; then
        rm -rf "${B}/${TF_A_METADATA_NAME}.${TF_A_METADATA_SUFFIX}"
        ${TF_A_METADATA_TOOL} ${TF_A_METADATA_TOOL_ARGS} "${B}/${TF_A_METADATA_NAME}.${TF_A_METADATA_SUFFIX}"
    fi
}

export_binaries() {
    local dest="${1}"
    local fip_deploydir_tfa_base="${dest}"
    local fip_deploydir_bl31="${dest}${FIP_DIR_BL31}"
    local fip_deploydir_tfa="${dest}${FIP_DIR_TFA}"
    local fip_deploydir_fwconf="${dest}${FIP_DIR_FWCONF}"
    local fip_deploydir_fwddr="${dest}${FIP_DIR_FWDDR}"

    install -d ${fip_deploydir_tfa_base}

    unset i
    for config in ${TF_A_CONFIG}; do
        i=$(expr $i + 1)
        # Initialize devicetree list and tf-a basename
        dt_config=$(echo ${TF_A_DEVICETREE} | cut -d',' -f${i})
        dt_suffix=$(echo ${TF_A_DT_SUFFIX} | cut -d',' -f${i})
        [ "${EXTDT_USE_SUFFIX}" = "1" ] || dt_suffix=""
        tfa_basename=$(echo ${TF_A_BINARIES} | cut -d',' -f${i})
        tfa_file_type=$(echo ${TF_A_FILES} | cut -d',' -f${i})
        for dt in ${dt_config}; do
            # Init soc suffix
            soc_suffix=""
            if [ -n "${STM32MP_SOC_NAME}" ]; then
                for soc in ${STM32MP_SOC_NAME}; do
                    [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] && soc_suffix="-${soc}"
                done
            fi
            build_dir="${B}/${config}${soc_suffix}-${dt}${dt_suffix}"
            for file_type in ${tfa_file_type}; do
                case "${file_type}" in
                    bl2)
                        # Install TF-A binary
                        if [ -f "${build_dir}/${tfa_basename}-${dt}-${config}.${TF_A_SUFFIX}" ]; then
                            install -m 644 "${build_dir}/${tfa_basename}-${dt}-${config}${TF_A_ENCRYPT_SUFFIX}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" "${fip_deploydir_tfa_base}/"
                            if [ "${TF_A_ENABLE_DEBUG_WRAPPER}" = "1" ]; then
                                install -d "${fip_deploydir_tfa_base}/debug"
                                install -m 644 "${build_dir}/debug-${tfa_basename}-${dt}-${config}${TF_A_ENCRYPT_SUFFIX}${TF_A_SIGN_SUFFIX}.${TF_A_SUFFIX}" "${fip_deploydir_tfa_base}/debug/"
                            fi
                        fi
                        if [ -f "${build_dir}/${tfa_basename}-${dt}-${config}.bin" ]; then
                            install -d "${fip_deploydir_tfa_base}/bl2"
                            install -m 644 "${build_dir}/${tfa_basename}-${dt}-${config}.bin" "${fip_deploydir_tfa_base}/bl2/"
                        fi
                        if [ -n "${ELF_DEBUG_ENABLE}" ]; then
                            install -d "${fip_deploydir_tfa_base}/debug"
                            if [ -f "${build_dir}/${BL2_ELF}" ]; then
                                install -m 644 "${build_dir}/${BL2_ELF}" "${fip_deploydir_tfa_base}/debug/${tfa_basename}-${BL2_BASENAME_DEPLOY}${soc_suffix}-${config}.${TF_A_ELF_SUFFIX}"
                            fi
                        fi
                        if [ "${TF_A_FWDDR}" = "1" ]; then
                            install -d "${fip_deploydir_fwddr}"
                            # Install DDR firmware binary
                            if [ -f "${build_dir}/${FWDDR_NAME}-${dt}-${config}.${FWDDR_SUFFIX}" ]; then
                                install -m 644 "${build_dir}/${FWDDR_NAME}-${dt}-${config}.${FWDDR_SUFFIX}" "${fip_deploydir_fwddr}/"
                            fi
                        fi
                        ;;
                    bl31)
                        # Install BL31 files
                        install -d "${fip_deploydir_bl31}"
                        # Install BL31 binary
                        if [ -f "${build_dir}/${BL31_BASENAME}.${BL31_SUFFIX}" ]; then
                            install -m 644 "${build_dir}/${BL31_BASENAME}.${BL31_SUFFIX}" "${fip_deploydir_bl31}/${tfa_basename}-${BL31_BASENAME_DEPLOY}-${dt}${dt_suffix}-${config}.${BL31_SUFFIX}"
                        fi
                        # Install BL31 devicetree
                        if [ -f "${build_dir}/fdts/${dt}${dt_suffix}-${BL31_BASENAME}.${DT_SUFFIX}" ]; then
                            install -m 644 "${build_dir}/fdts/${dt}${dt_suffix}-${BL31_BASENAME}.${DT_SUFFIX}" "${fip_deploydir_bl31}/${dt}${dt_suffix}-${BL31_BASENAME}-${config}.${DT_SUFFIX}"
                        fi
                        if [ -n "${ELF_DEBUG_ENABLE}" ]; then
                            install -d "${fip_deploydir_bl31}/debug"
                            if [ -f "${build_dir}/${BL31_ELF}" ]; then
                                install -m 644 "${build_dir}/${BL31_ELF}" "${fip_deploydir_bl31}/debug/${tfa_basename}-${BL31_BASENAME_DEPLOY}-${dt}${dt_suffix}-${config}.${TF_A_ELF_SUFFIX}"
                            fi
                        fi
                        ;;
                    bl32)
                        # Install BL32 files
                        install -d "${fip_deploydir_tfa}"
                        # Install BL32 binary
                        if [ -f "${build_dir}/${BL32_BASENAME}.${BL32_SUFFIX}" ]; then
                            install -m 644 "${build_dir}/${BL32_BASENAME}.${BL32_SUFFIX}" "${fip_deploydir_tfa}/${tfa_basename}-${BL32_BASENAME_DEPLOY}${soc_suffix}-${config}.${BL32_SUFFIX}"
                        fi
                        # Install BL32 devicetree
                        if [ -f "${build_dir}/fdts/${dt}${dt_suffix}-${BL32_BASENAME}.${DT_SUFFIX}" ]; then
                            install -m 644 "${build_dir}/fdts/${dt}${dt_suffix}-${BL32_BASENAME}.${DT_SUFFIX}" "${fip_deploydir_tfa}/${dt}${dt_suffix}-${BL32_BASENAME}-${config}.${DT_SUFFIX}"
                        fi
                        if [ -n "${ELF_DEBUG_ENABLE}" ]; then
                            install -d "${fip_deploydir_tfa}/debug"
                            if [ -f "${build_dir}/${BL32_ELF}" ]; then
                                install -m 644 "${build_dir}/${BL32_ELF}" "${fip_deploydir_tfa}/debug/${tfa_basename}-${BL32_BASENAME_DEPLOY}${soc_suffix}-${config}.${TF_A_ELF_SUFFIX}"
                            fi
                        fi
                        ;;
                    fwconfig)
                        # Install fwconfig
                        install -d "${fip_deploydir_fwconf}"
                        if [ -f "${build_dir}/fdts/${dt}${dt_suffix}-${FWCONFIG_NAME}.${DT_SUFFIX}" ]; then
                            install -m 644 "${build_dir}/fdts/${dt}${dt_suffix}-${FWCONFIG_NAME}.${DT_SUFFIX}" "${fip_deploydir_fwconf}/${dt}${dt_suffix}-${FWCONFIG_NAME}-${config}.${DT_SUFFIX}"
                        fi
                        ;;
                esac
            done # for file_type in ${tfa_file_type}
        done # for dt in ${dt_config}
        if [ -n "${ELF_DEBUG_ENABLE}" ]; then
            install -d "${fip_deploydir_tfa_base}/debug"
            if [ -f "${build_dir}/${BL1_ELF}" ]; then
                install -m 644 "${build_dir}/${BL1_ELF}" "${fip_deploydir_tfa_base}/debug/${tfa_basename}-${BL1_BASENAME_DEPLOY}-${config}.${TF_A_ELF_SUFFIX}"
            fi
        fi
    done # for config in ${TF_A_CONFIG}

    if [ "${TF_A_ENABLE_METADATA}" = "1" ]; then
        install -d "${fip_deploydir_tfa_base}"
        if [ -f "${B}/${TF_A_METADATA_NAME}.${TF_A_METADATA_SUFFIX}" ]; then
            install -m 644 "${B}/${TF_A_METADATA_NAME}.${TF_A_METADATA_SUFFIX}" "${fip_deploydir_tfa_base}/${TF_A_METADATA_BINARY}"
        fi
    fi
}

tf_a_sysroot_populate() {
  export_binaries ${SYSROOT_DESTDIR}${FIP_DIR_TFA_BASE}
}
SYSROOT_PREPROCESS_FUNCS =+ "tf_a_sysroot_populate"
SYSROOT_DIRS:append = " ${FIP_DIR_TFA_BASE}"

do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}${FIP_DIR_TFA_BASE}"
do_deploy() {
    export_binaries ${DEPLOYDIR}
}

addtask deploy before do_build after do_install
