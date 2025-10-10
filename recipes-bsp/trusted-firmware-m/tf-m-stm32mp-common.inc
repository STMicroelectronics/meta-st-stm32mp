FILESEXTRAPATHS:prepend := "${THISDIR}/tf-m-stm32mp:"

PACKAGE_ARCH = "${MACHINE_ARCH}"

CVE_PRODUCT = "arm:trusted_firmware-m mbed_tls arm:cmsis-rtos"

inherit python3native deploy external-dt

# Include TF-A config definitions
require tf-m-stm32mp-config.inc

# Include for STAGING_TF_M_TESTS_DIR
require tf-m-tests-stm32mp-share.inc
do_compile[depends] += "tf-m-tests-stm32mp:do_configure"

# Baremetal and we bring a compiler below
INHIBIT_DEFAULT_DEPS = "1"

DEPENDS += "cmake-native \
            ninja-native \
            gcc-arm-none-eabi-native \
            python3-cbor2-native \
            python3-click-native \
            python3-cryptography-native \
            python3-pyasn1-native \
            python3-imgtool-native \
            python3-jinja2-native \
            python3-pyyaml-native \
            python3-pyelftools-native \
            python3-pycryptodomex-native \
"

B = "${WORKDIR}/build"

# Init default external source path
TF_M_PATH_MBEDCRYPTO ?= ""
TF_M_PATH_MCUBOOT         ?= ""
TF_M_PATH_QCBOR           ?= ""
TF_M_PATH_CMSIS           ?= ""
TF_M_PATH_DDR_PHY_BIN_SRC ?= ""
TF_M_PATH_SCP_FW          ?= ""

# Add path to tf-m source
EXTRA_OECMAKE += "-DCONFIG_TFM_SOURCE_PATH=${S}"

# Configure TFM external sources
TFM_EXTERNAL_SOURCES ??= "1"
TFM_EXTERNAL_SOURCES_ROOTDIR ?= ""

# Configure device tree subpath
TFM_DTS_SUB_PATH = "platform/ext/target/stm/common/devicetree/dts/arm/stm"

PACKAGECONFIG ??= ""
# Whether to integrate the test suite
PACKAGECONFIG[test-secure] = "-DTEST_S=ON,-DTEST_S=OFF"
PACKAGECONFIG[test-nonsecure] = "-DTEST_NS=ON,-DTEST_NS=OFF"
PACKAGECONFIG[test-psa] = "-DTEST_PSA_API=ON,-DTEST_PSA_API=OFF"

DEPENDS:append = " ${@bb.utils.contains('TF_M_ENABLE_DEBUG_WRAPPER', '1', 'stm32wrapper4dbg-native', '', d)}"
TF_M_ENABLE_DEBUG_WRAPPER ??= "${@bb.utils.contains('MACHINE_FEATURES', 'm33td', '1', '0', d)}"

# Define default TF-M namings
BL2_NAME   ?= "bl2"
BL2_SUFFIX ?= "stm32"
BL2_TYPE   ?= "-${BL2_NAME}"

DTS_SUFFIX  ?= "dts"
DTS_TYPE_NS ?= "-ns"
DTS_TYPE_S  ?= "-s"

FWDDR_NAME   ?= "ddr_phy_signed"
FWDDR_SUFFIX ?= "bin"

TF_M_NAME   ?= "tfm"
TF_M_TYPE   ?= "${@bb.utils.contains('MACHINE_FEATURES', 'm33td', '_s_ns_signed', '_s', d)}"
TF_M_SUFFIX ?= "bin"

TF_M_ELF_TYPE   ?= "_s"
TF_M_ELF_SUFFIX ?= "elf"

# Build for debug (set TFM_DEBUG to 1 to activate)
TFM_DEBUG ?= "${@bb.utils.contains('MACHINE_FEATURES', 'm33td', '0', '1', d)}"
# Handle TFM_DEBUG parameter
EXTRA_OECMAKE += "${@bb.utils.contains('TFM_DEBUG', '1', '-DCMAKE_BUILD_TYPE=Debug', '-DCMAKE_BUILD_TYPE=Relwithdebinfo', d)}"

# Verbose builds
EXTRA_OECMAKE += "-DCMAKE_VERBOSE_MAKEFILE:BOOL=ON"

# Don't let FetchContent download more sources during do_configure
TFM_EXTERNAL_SOURCES_OECMAKE += "-DFETCHCONTENT_FULLY_DISCONNECTED=ON "
# Path to extra components
TFM_EXTERNAL_SOURCES_OECMAKE += "-DMBEDCRYPTO_PATH=${UNPACKDIR}/${TFM_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_MBEDCRYPTO} "
TFM_EXTERNAL_SOURCES_OECMAKE += "-DMCUBOOT_PATH=${UNPACKDIR}/${TFM_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_MCUBOOT} "
TFM_EXTERNAL_SOURCES_OECMAKE += "-DQCBOR_PATH=${UNPACKDIR}/${TFM_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_QCBOR} "
TFM_EXTERNAL_SOURCES_OECMAKE += "-DCMSIS_PATH=${UNPACKDIR}/${TFM_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_CMSIS} "
TFM_EXTERNAL_SOURCES_OECMAKE += "-DDDR_PHY_BIN_SRC_PATH=${UNPACKDIR}/${TFM_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_DDR_PHY_BIN_SRC} "
TFM_EXTERNAL_SOURCES_OECMAKE += "-DSCP_FW_PATH=${UNPACKDIR}/${TFM_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_SCP_FW} "
EXTRA_OECMAKE += "${@bb.utils.contains('TFM_EXTERNAL_SOURCES', '1', '${TFM_EXTERNAL_SOURCES_OECMAKE}', '', d)}"

# TFM Profile: medium
TFM_PROFILE ??= "profile_medium"
EXTRA_OECMAKE += " ${@'-DTFM_PROFILE=${TFM_PROFILE}' if ((d.getVar('TFM_PROFILE') or '') != '') else ''}"

# Let the Makefile handle setting up the CFLAGS and LDFLAGS as it is a standalone application
CFLAGS[unexport] = "1"
LDFLAGS[unexport] = "1"
AS[unexport] = "1"
LD[unexport] = "1"

# Enable parallel build for cmake
export CMAKE_BUILD_PARALLEL_LEVEL = "${@oe.utils.parallel_make(d, False)}"

# do not calculate the hash of task
BB_HASH_IGNORE_MISMATCH = "1"

# python3-cryptography needs the legacy provider, so set OPENSSL_MODULES to the
# right path until this is relocated automatically.
export OPENSSL_MODULES = "${STAGING_LIBDIR_NATIVE}/ossl-modules"

do_clean[cleandirs] += " ${STAGING_TFM_BUILDDIR}"

do_patch:append() {
    if d.getVar('TFM_EXTERNAL_SOURCES') == "1":
        bb.build.exec_func('apply_local_patches', d)
}
# Apply specific patches shipped within TF-M source code.
# Make sure to apply them in submodule to avoid failure with 'devtool modify'
apply_local_patches() {
    if [ "${TFM_EXTERNAL_SOURCES}" = "1" ]; then
        if [ ! -e "${UNPACKDIR}/${TFM_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_EXTERNAL}/.patched" ]; then
            cd ${UNPACKDIR}/${TFM_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_QCBOR}
            for fp in ${S}/lib/ext/qcbor/*.patch; do
                git am ${fp}
            done
            cd ${UNPACKDIR}/${TFM_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_CMSIS}
            for fp in ${S}/lib/ext/cmsis/*.patch; do
                git am ${fp}
            done
            cd ${UNPACKDIR}/${TFM_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_MBEDCRYPTO}
            for fp in ${S}/lib/ext/mbedcrypto/*.patch; do
                git am ${fp}
            done
            touch ${UNPACKDIR}/${TFM_EXTERNAL_SOURCES_ROOTDIR}/${TF_M_PATH_EXTERNAL}/.patched
            cd -
        fi
    fi
}
do_configure[prefuncs] += "apply_local_patches"
do_configure() {
    :
}

# Invoke install here as there's no point in splitting compile from install: the
# first thing the build does is 'install' inside the build tree thus causing a
# rebuild. It also overrides the install prefix to be in the build tree, so you
# can't use the usual install prefix variables.
do_compile() {
    export PATH=$PATH:${STAGING_DATADIR_NATIVE}/gcc-arm-none-eabi/bin

    # Init tf_m_platform_optflags_* var for all supported platform
    ${@' '.join(['tf_m_platform_optflags_%s="%s";' % (name, (d.getVar('TF_M_PLATFORM_OPTFLAGS_%s' % name) or "")) for name in list(dict.fromkeys((d.getVar('TF_M_PLATFORM_BASENAME_LIST') or "").split()))])}

    unset i
    for config in ${TF_M_CONFIG}; do
        i=$(expr $i + 1)
        # Initialize plateform list, devicetree list, boot device, dt bl2 suffix and dt s suffix
        pf_config=$(echo ${TF_M_PLATFORM} | cut -d',' -f${i})
        dt_config=$(echo ${TF_M_DEVICETREE} | cut -d',' -f${i})
        extra_opt=$(echo ${TF_M_EXTRA_OPTFLAGS} | cut -d',' -f${i})
        boot_dev=$(echo ${TF_M_BOOTDEV} | cut -d',' -f${i})
        dt_bl2_suffix=$(echo ${TF_M_DT_BL2_SUFFIX} | cut -d',' -f${i})
        [ "${EXTDT_USE_SUFFIX}" = "1" ] || dt_bl2_suffix=""
        dt_tfm_suffix=$(echo ${TF_M_DT_TFM_SUFFIX} | cut -d',' -f${i})
        [ "${EXTDT_USE_SUFFIX}" = "1" ] || dt_tfm_suffix=""
        for platform in ${pf_config}; do
            pf_base="$(basename "${platform}")"
            pf_opts="$(eval echo \"\${tf_m_platform_optflags_${pf_base}}\")"
            extra_oecmake_platform="${pf_opts} -DTFM_PLATFORM=${platform}"
            for dt in ${dt_config}; do
                # Filter for devicetree that matches platform
                echo ${dt} | sed 's/-/_/g' | grep -q "${pf_base}" || continue
                # Configuration for build path (one per build)
                build_path="${B}/${config}-${pf_base}-${dt}"
                # Configuration for device tree path
                dt_name_s="${dt}${dt_tfm_suffix}${DTS_TYPE_S}.${DTS_SUFFIX}"
                dt_name_ns="${dt}${dt_tfm_suffix}${DTS_TYPE_NS}.${DTS_SUFFIX}"
                if [ -e "${S}/${TFM_DTS_SUB_PATH}/${dt_name_s}" ]; then
                    dt_dir="${S}/${TFM_DTS_SUB_PATH}"
                    dt_subdir_tfm=""
                    dt_subdir_bl2=""
                elif [ -e "${STAGING_EXTDT_DIR}/${EXTDT_DIR_TF_M}/${dt_name_s}" ]; then
                    dt_dir="${STAGING_EXTDT_DIR}"
                    dt_subdir_tfm="${EXTDT_DIR_TF_M}/"
                    dt_subdir_bl2="${EXTDT_DIR_MCU}/"
                else
                    bbnote "TF-M: devicetree not found (${dt_name_s}) NO TF-M binaries generated"
                    continue
                fi
                extra_oecmake_ext_dir="-DDTS_EXT_DIR=${dt_dir}"
                # Configuration for device tree s
                extra_oecmake_board_s="-DDTS_BOARD_S=${dt_subdir_tfm}${dt_name_s}"
                # Configuration for device tree ns
                if [ -e "${STAGING_EXTDT_DIR}/${dt_subdir_tfm}${dt_name_ns}" ]; then
                    extra_oecmake_board_ns="-DDTS_BOARD_NS=${dt_subdir_tfm}${dt_name_ns}"
                else
                    bbnote "TF-M: devicetree ns not found (${dt_name_ns}) NO -DDTS_BOARD_NS option for binaries generation"
                    extra_oecmake_board_ns=""
                fi
                # Configuration for boot device and bl2
                if [ -n "${boot_dev}" ]; then
                    extra_oecmake_bootdev="-DSTM32_BOOT_DEV=${boot_dev}"
                    dt_name_bl2="${dt}${dt_bl2_suffix}${BL2_TYPE}.${DTS_SUFFIX}"
                    if [ -e "${dt_dir}/${dt_subdir_bl2}${dt_name_bl2}" ]; then
                        extra_oecmake_bl2="-DDTS_BOARD_BL2=${dt_subdir_bl2}${dt_name_bl2}"
                    else
                        bbnote "TF-M: BL2 devicetree not found (${dt_subdir_bl2}${dt_name_bl2}) NO TF-M binaries generated"
                        continue
                    fi
                else
                    extra_oecmake_bootdev=""
                    extra_oecmake_bl2=""
                fi

                # Display cmake command
                echo "BL2 & TFM_S: cmake -GNinja \
                    -DTFM_TOOLCHAIN_FILE=${S}/toolchain_GNUARM.cmake \
                    ${EXTRA_OECMAKE} \
                    ${extra_opt} \
                    -S ${STAGING_TF_M_TESTS_DIR}/tests_reg/spe -B ${build_path}/spe \
                    ${extra_oecmake_platform} \
                    ${extra_oecmake_board_s} \
                    ${extra_oecmake_board_ns} \
                    ${extra_oecmake_bootdev} \
                    ${extra_oecmake_bl2} \
                    ${extra_oecmake_ext_dir} \
                    ${PACKAGECONFIG_CONFARGS}"
                cmake -GNinja \
                    -DTFM_TOOLCHAIN_FILE=${S}/toolchain_GNUARM.cmake \
                    ${EXTRA_OECMAKE} \
                    ${extra_opt} \
                    -S ${STAGING_TF_M_TESTS_DIR}/tests_reg/spe -B ${build_path}/spe \
                    ${extra_oecmake_platform} \
                    ${extra_oecmake_board_s} \
                    ${extra_oecmake_board_ns} \
                    ${extra_oecmake_bootdev} \
                    ${extra_oecmake_bl2} \
                    ${extra_oecmake_ext_dir} \
                    ${PACKAGECONFIG_CONFARGS}
                echo "BL2 & TFM_S: cmake --build ${build_path}/spe -- install"
                cmake --build ${build_path}/spe -- install
            done
        done
    done
}
do_compile[progress] = "outof:^\[(\d+)/(\d+)\]\s+"

do_shared_workdir[cleandirs] += " ${STAGING_TFM_BUILDDIR}"
do_shared_workdir () {
    for config in ${TF_M_CONFIG}; do
        i=$(expr $i + 1)
        # provide build artifactory only for m33td configuration
        [ "${config}" = "m33copro" ] && continue
        # Initialize plateform list, devicetree list, dt bl2 suffix and dt s suffix
        pf_config=$(echo ${TF_M_PLATFORM} | cut -d',' -f${i})
        dt_config=$(echo ${TF_M_DEVICETREE} | cut -d',' -f${i})
        for platform in ${pf_config}; do
            pf_base="$(basename "${platform}")"
            for dt in ${dt_config}; do
                # Filter for devicetree that matches platform
                echo ${dt} | sed 's/-/_/g' | grep -q "${pf_base}" || continue
                # Configuration for build path (one per build)
                build_path="${B}/${config}-${pf_base}-${dt}"
                if [ -d "${build_path}" ]; then
                    output_path="${STAGING_TFM_BUILDDIR}/${config}-${dt}"
                    install -d ${output_path}
                    cp -ar ${build_path}/* ${output_path}/
                else
                    bbnote "Folder ${config}-${pf_base}-${dt} doesn't exist in ${B}"
                fi
            done
        done
    done
}
addtask shared_workdir after do_compile before do_install
addtask shared_workdir_setscene

do_shared_workdir_setscene () {
    exit 1
}

do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}/arm-trusted-firmware-m"
do_deploy() {
    install -d ${DEPLOYDIR}
    install -d ${DEPLOYDIR}/key
    unset i
    for config in ${TF_M_CONFIG}; do
        i=$(expr $i + 1)
        # Initialize plateform list, devicetree list, dt bl2 suffix and dt s suffix
        pf_config=$(echo ${TF_M_PLATFORM} | cut -d',' -f${i})
        dt_config=$(echo ${TF_M_DEVICETREE} | cut -d',' -f${i})
        suffix_boota35=$(echo ${TF_M_BOOT_SUFFIX_A35} | cut -d',' -f${i})
        [ -z "${suffix_boota35}" ] || suffix_boota35="-${suffix_boota35}"
        suffix_bootm33=$(echo ${TF_M_BOOT_SUFFIX_M33} | cut -d',' -f${i})
        [ -z "${suffix_bootm33}" ] || suffix_bootm33="-${suffix_bootm33}"
        for platform in ${pf_config}; do
            pf_base="$(basename "${platform}")"
            for dt in ${dt_config}; do
                # Filter for devicetree that matches platform
                echo ${dt} | sed 's/-/_/g' | grep -q "${pf_base}" || continue
                # Configuration for build path (one per build)
                output_path="${B}/${config}-${pf_base}-${dt}"
                if [ -e "${output_path}/spe/bin/${FWDDR_NAME}.${FWDDR_SUFFIX}" ]; then
                    install -m 0644 ${output_path}/spe/bin/${FWDDR_NAME}.${FWDDR_SUFFIX} ${DEPLOYDIR}/${FWDDR_NAME}-${dt}${suffix_bootm33}.${FWDDR_SUFFIX}
                fi
                if [ -e "${output_path}/spe/bin/${BL2_NAME}.${BL2_SUFFIX}" ]; then
                    install -m 0644 ${output_path}/spe/bin/${BL2_NAME}.${BL2_SUFFIX} ${DEPLOYDIR}/${BL2_NAME}-${dt}${suffix_bootm33}.${BL2_SUFFIX}
                    if [ "${TF_M_ENABLE_DEBUG_WRAPPER}" = "1" ]; then
                        install -d ${DEPLOYDIR}/debug
                        stm32wrapper4dbg -s "${output_path}/spe/bin/${BL2_NAME}.${BL2_SUFFIX}" -d "${DEPLOYDIR}/debug//debug-${BL2_NAME}-${dt}${suffix_bootm33}.${BL2_SUFFIX}"
                    fi
                fi
                if [ -e "${output_path}/spe/bin/${TF_M_NAME}${TF_M_ELF_TYPE}.${TF_M_ELF_SUFFIX}" ]; then
                    install -d ${DEPLOYDIR}/debug
                    install -m 0644 ${output_path}/spe/bin/${TF_M_NAME}${TF_M_ELF_TYPE}.${TF_M_ELF_SUFFIX} ${DEPLOYDIR}/debug/${TF_M_NAME}-${dt}${suffix_bootm33}${suffix_boota35}${TF_M_ELF_TYPE}.${TF_M_ELF_SUFFIX}
                fi
                if [ -e "${output_path}/spe/bin/${TF_M_NAME}${TF_M_TYPE}.${TF_M_SUFFIX}" ]; then
                    install -m 0644 ${output_path}/spe/bin/${TF_M_NAME}${TF_M_TYPE}.${TF_M_SUFFIX} ${DEPLOYDIR}/${TF_M_NAME}-${dt}${suffix_bootm33}${suffix_boota35}${TF_M_TYPE}.${TF_M_SUFFIX}
                fi
                if [ -e "${output_path}/spe/api_ns/image_signing/keys/image_s_signing_public_key.pem" ]; then
                    install -m 0644 ${output_path}/spe/api_ns/image_signing/keys/image_s_signing_public_key.pem ${DEPLOYDIR}/key/image_s_signing_public_key-${dt}${suffix_bootm33}.pem
                fi
                if [ -e "${output_path}/spe/api_ns/image_signing/keys/image_s_signing_private_key.pem" ]; then
                    install -m 0644 ${output_path}/spe/api_ns/image_signing/keys/image_s_signing_private_key.pem ${DEPLOYDIR}/key/image_s_signing_private_key-${dt}${suffix_bootm33}.pem
                fi
                if [ -e "${output_path}/spe/build-spe/bl2/ext/mcuboot/image_ns_signing_public_key.pem" ]; then
                    install -m 0644 ${output_path}/spe/build-spe/bl2/ext/mcuboot/image_ns_signing_public_key.pem ${DEPLOYDIR}/key/image_ns_signing_public_key-${dt}${suffix_bootm33}.pem
                fi
            done
        done
    done
}
addtask deploy before do_build after do_compile

tf_m_sysroot_populate() {
    unset i
    for config in ${TF_M_CONFIG}; do
        i=$(expr $i + 1)
        # Initialize plateform list, devicetree list, dt bl2 suffix and dt s suffix
        pf_config=$(echo ${TF_M_PLATFORM} | cut -d',' -f${i})
        dt_config=$(echo ${TF_M_DEVICETREE} | cut -d',' -f${i})
        for platform in ${pf_config}; do
            pf_base="$(basename "${platform}")"
            for dt in ${dt_config}; do
                # Filter for devicetree that matches platform
                echo ${dt} | sed 's/-/_/g' | grep -q "${pf_base}" || continue
                # Configuration for build path (one per build)
                output_path="${B}/${config}-${pf_base}-${dt}"
                if [ -e "${output_path}/spe/bin/${TF_M_NAME}${TF_M_ELF_TYPE}.${TF_M_ELF_SUFFIX}" ]; then
                    install -d ${SYSROOT_DESTDIR}/firmware/${config}
                    install -m 0644 ${output_path}/spe/bin/${TF_M_NAME}${TF_M_ELF_TYPE}.${TF_M_ELF_SUFFIX} ${SYSROOT_DESTDIR}/firmware/${config}/${TF_M_NAME}-${dt}${TF_M_ELF_TYPE}.${TF_M_ELF_SUFFIX}
                fi
            done
        done
    done
}
SYSROOT_PREPROCESS_FUNCS =+ "tf_m_sysroot_populate"
SYSROOT_DIRS += "/firmware"

# Build paths are currently embedded
INSANE_SKIP:${PN} += "buildpaths"
INSANE_SKIP:${PN} += "arch"
