FILESEXTRAPATHS:prepend := "${THISDIR}/tf-m-stm32mp:"

PACKAGE_ARCH = "${MACHINE_ARCH}"

CVE_PRODUCT = "arm:trusted_firmware-m mbed_tls arm:cmsis-rtos"

inherit python3native deploy external-dt
inherit m33fw-utils-stm32mp

# Include TF-M config definitions
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
DEPENDS:append = " ${@bb.utils.contains('TF_M_ENABLE_DEBUG_WRAPPER', '1', 'stm32wrapper4dbg-native', '', d)}"

B = "${WORKDIR}/build"

PACKAGECONFIG ??= ""
# Whether to integrate the test suite
PACKAGECONFIG[test-secure] = "-DTEST_S=ON,-DTEST_S=OFF"
PACKAGECONFIG[test-nonsecure] = "-DTEST_NS=ON,-DTEST_NS=OFF"
PACKAGECONFIG[test-psa] = "-DTEST_PSA_API=ON,-DTEST_PSA_API=OFF"

# Enable the wrapper for debug
TF_M_ENABLE_DEBUG_WRAPPER ??= "1"

# Output the ELF generated
ELF_DEBUG_ENABLE ?= ""
TF_M_ELF_SUFFIX  ?= "elf"

# Define default TF-M namings
BL2_NAME   ?= "bl2"
BL2_SUFFIX ?= "stm32"
BL2_TYPE   ?= "-${BL2_NAME}"

DTS_SUB_PATH ?= "platform/ext/target/stm/common/devicetree/dts/arm/stm"
DTS_SUFFIX  ?= "dts"
DTS_TYPE_NS ?= "-ns"
DTS_TYPE_S  ?= "-s"

FWDDR_LAYOUT_BASENAME ?= "ddrfw_"
FWDDR_LAYOUT_PATH     ?= "api_ns/bin/layout_files"
FWDDR_NAME   ?= "ddr_phy"
FWDDR_SUFFIX ?= "bin"

TF_M_LAYOUT_NAME   ?= "signing_layout"
TF_M_LAYOUT_SUFFIX ?= "o"
TF_M_NAME    ?= "tfm"
TF_M_SUFFIX  ?= "bin"
TF_M_TYPE_NS ?= "_ns"
TF_M_TYPE_S  ?= "_s"

# Add path to tf-m source
EXTRA_OECMAKE += "-DCONFIG_TFM_SOURCE_PATH=${S}"

# Build for debug (set TF_M_DEBUG to 1 to activate)
TF_M_DEBUG ?= "${@bb.utils.contains('MACHINE_FEATURES', 'm33td', '0', '1', d)}"
# Handle TF_M_DEBUG parameter
EXTRA_OECMAKE += "${@bb.utils.contains('TF_M_DEBUG', '1', '-DCMAKE_BUILD_TYPE=Debug', '-DCMAKE_BUILD_TYPE=Relwithdebinfo', d)}"

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

do_configure() {
    :
}
# Invoke install here as there's no point in splitting compile from install: the
# first thing the build does is 'install' inside the build tree thus causing a
# rebuild. It also overrides the install prefix to be in the build tree, so you
# can't use the usual install prefix variables.
do_compile() {
    export PATH=$PATH:${STAGING_DATADIR_NATIVE}/gcc-arm-none-eabi/bin

    unset i
    for config in ${M33FW_CONFIG}; do
        i=$(expr $i + 1)
        # Initialize devicetree list, boot device, dt bl2 suffix and dt s suffix
        dt_config=$(echo ${M33FW_DEVICETREE} | cut -d',' -f${i})
        extra_opt=$(echo ${TF_M_EXTRA_OPTFLAGS} | cut -d',' -f${i})
        boot_dev=$(echo ${TF_M_BOOTDEV} | cut -d',' -f${i})
        dt_bl2_suffix=$(echo ${TF_M_DT_BL2_SUFFIX} | cut -d',' -f${i})
        [ "${EXTDT_USE_SUFFIX}" = "1" ] || dt_bl2_suffix=""
        dt_tfm_suffix=$(echo ${TF_M_DT_TFM_SUFFIX} | cut -d',' -f${i})
        [ "${EXTDT_USE_SUFFIX}" = "1" ] || dt_tfm_suffix=""

        for dt in ${dt_config}; do
            platform=""
            pltf_opt=""
            if [ "$(echo "${M33FW_BOARDS}" | wc -w)" -eq 1 ]; then
                platform=$(echo ${M33FW_TFM_PLATFORM} | cut -d',' -f1)
                pltf_opt=$(echo ${M33FW_TFM_OPTFLAGS} | cut -d',' -f1)
            else
                unset k
                for board in ${M33FW_BOARDS}; do
                    k=$(expr $k + 1)
                    # Filter for devicetree that matches board configure
                    echo ${dt} | grep -q "${board}" || continue
                    # Initialize project list to build
                    platform=$(echo ${M33FW_TFM_PLATFORM} | cut -d',' -f${k})
                    pltf_opt=$(echo ${M33FW_TFM_OPTFLAGS} | cut -d',' -f${k})
                    break
                done
            fi
            if [ -z "${platform}" ]; then
                bbnote "TF-M: empty platform path... Skip build!"
                continue
            elif [ -e "${STAGING_EXTDT_DIR}/${platform}" ]; then
                bbnote "TF-M: use external platform profile"
                platform="${STAGING_EXTDT_DIR}/${platform}"
            fi
            # Configuration for platform
            extra_oecmake_platform="-DTFM_PLATFORM=${platform} ${pltf_opt}"
            # Configuration for build path (one per build)
            build_path="${B}/${config}-${dt}"
            # Configuration for device tree path
            dt_name_s="${dt}${dt_tfm_suffix}${DTS_TYPE_S}.${DTS_SUFFIX}"
            dt_name_ns="${dt}${dt_tfm_suffix}${DTS_TYPE_NS}.${DTS_SUFFIX}"
            if [ -e "${S}/${DTS_SUB_PATH}/${dt_name_s}" ]; then
                dt_dir="${S}/${DTS_SUB_PATH}"
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
}
do_compile[progress] = "outof:^\[(\d+)/(\d+)\]\s+"

tf_m_sysroot_populate() {
    unset i
    for config in ${M33FW_CONFIG}; do
        i=$(expr $i + 1)
        # Initialize devicetree list, boot suffix a35 and boot suffix m33
        dt_config=$(echo ${M33FW_DEVICETREE} | cut -d',' -f${i})
        suffix_boota35=$(echo ${M33FW_BOOT_SUFFIX_A35} | cut -d',' -f${i})
        [ -z "${suffix_boota35}" ] || suffix_boota35="-${suffix_boota35}"
        suffix_bootm33=$(echo ${M33FW_BOOT_SUFFIX_M33} | cut -d',' -f${i})
        [ -z "${suffix_bootm33}" ] || suffix_bootm33="-${suffix_bootm33}"
        for dt in ${dt_config}; do
            # Configuration for build path (one per build)
            output_path="${B}/${config}-${dt}"
            if [ -e "${output_path}/spe/bin/${TF_M_NAME}${TF_M_TYPE_S}.${TF_M_ELF_SUFFIX}" ]; then
                install -d ${SYSROOT_DESTDIR}${M33FW_SYSROOT_S}
                install -m 0644 ${output_path}/spe/bin/${TF_M_NAME}${TF_M_TYPE_S}.${TF_M_ELF_SUFFIX} ${SYSROOT_DESTDIR}${M33FW_SYSROOT_S}/${TF_M_NAME}-${dt}${suffix_bootm33}${suffix_boota35}${TF_M_TYPE_S}.${TF_M_ELF_SUFFIX}
            fi
            if [ -e "${output_path}/spe/bin/${TF_M_NAME}${TF_M_TYPE_S}.${TF_M_SUFFIX}" ]; then
                install -d ${SYSROOT_DESTDIR}${M33FW_SYSROOT_S}
                install -m 0644 ${output_path}/spe/bin/${TF_M_NAME}${TF_M_TYPE_S}.${TF_M_SUFFIX} ${SYSROOT_DESTDIR}${M33FW_SYSROOT_S}/${TF_M_NAME}-${dt}${suffix_bootm33}${suffix_boota35}${TF_M_TYPE_S}.${TF_M_SUFFIX}
            fi
            if [ -e "${output_path}/spe/api_ns/image_signing/layout_files/${TF_M_LAYOUT_NAME}${TF_M_TYPE_S}${TF_M_TYPE_NS}.${TF_M_LAYOUT_SUFFIX}" ]; then
                # Remove local path in file
                sed -i "s|${TMPDIR}||g" "${output_path}/spe/api_ns/image_signing/layout_files/${TF_M_LAYOUT_NAME}${TF_M_TYPE_S}${TF_M_TYPE_NS}.${TF_M_LAYOUT_SUFFIX}"
                install -d ${SYSROOT_DESTDIR}${M33FW_SYSROOT_S}
                install -m 0644 ${output_path}/spe/api_ns/image_signing/layout_files/${TF_M_LAYOUT_NAME}${TF_M_TYPE_S}${TF_M_TYPE_NS}.${TF_M_LAYOUT_SUFFIX} ${SYSROOT_DESTDIR}${M33FW_SYSROOT_S}/${TF_M_NAME}-${dt}${suffix_bootm33}${suffix_boota35}${TF_M_TYPE_S}.${TF_M_LAYOUT_NAME}
            fi
            if [ -e "${output_path}/spe/bin/${FWDDR_NAME}.${FWDDR_SUFFIX}" ]; then
                install -d ${SYSROOT_DESTDIR}${M33FW_SYSROOT_DDR}
                install -m 0644 ${output_path}/spe/bin/${FWDDR_NAME}.${FWDDR_SUFFIX} ${SYSROOT_DESTDIR}${M33FW_SYSROOT_DDR}/${FWDDR_NAME}-${dt}${suffix_bootm33}.${FWDDR_SUFFIX}
            fi
            if [ -e "${output_path}/spe/api_ns/bin/layout_files/${FWDDR_LAYOUT_BASENAME}${TF_M_LAYOUT_NAME}.${TF_M_LAYOUT_SUFFIX}" ]; then
                # Remove local path in file
                sed -i "s|${TMPDIR}||g" "${output_path}/spe/api_ns/bin/layout_files/${FWDDR_LAYOUT_BASENAME}${TF_M_LAYOUT_NAME}.${TF_M_LAYOUT_SUFFIX}"
                install -d ${SYSROOT_DESTDIR}${M33FW_SYSROOT_DDR}
                install -m 0644 ${output_path}/spe/api_ns/bin/layout_files/${FWDDR_LAYOUT_BASENAME}${TF_M_LAYOUT_NAME}.${TF_M_LAYOUT_SUFFIX} ${SYSROOT_DESTDIR}${M33FW_SYSROOT_DDR}/${FWDDR_NAME}-${dt}${suffix_bootm33}.${TF_M_LAYOUT_NAME}
            fi

            if ${@bb.utils.contains('MACHINE_FEATURES', 'm33td', 'true', 'false', d)}; then
                if [ -d "${output_path}/spe/api_ns" ]; then
                    # Copy tf-m build content on shared directory
                    output_path_config="${SYSROOT_DESTDIR}${datadir}/tf-m/${config}-${dt}"
                    install -d ${output_path_config}/spe/api_ns/interface
                    cp -ar ${output_path}/spe/api_ns/interface/* ${output_path_config}/spe/api_ns/interface
                    install -d ${output_path_config}/spe/api_ns/common
                    cp -ar ${output_path}/spe/api_ns/common/* ${output_path_config}/spe/api_ns/common
                    install -d ${output_path_config}/spe/api_ns/image_signing/
                    cp -ar ${output_path}/spe/api_ns/image_signing/* ${output_path_config}/spe/api_ns/image_signing/
                    install -d ${output_path_config}/spe/api_ns/bin

                    install -m 0644  ${output_path}/spe/api_ns/bin/bl2.stm32 ${output_path_config}/spe/api_ns/bin/
                    install -m 0644  ${output_path}/spe/api_ns/bin/ddr_phy_signed.bin ${output_path_config}/spe/api_ns/bin/
                    install -m 0644  ${output_path}/spe/api_ns/bin/tfm_s.bin ${output_path_config}/spe/api_ns/bin/
                else
                    bbnote "Folder '${config}-${dt}/spe/api_ns' doesn't exist in ${B}"
                fi
            fi
        done
    done
}
SYSROOT_PREPROCESS_FUNCS =+ "tf_m_sysroot_populate"
SYSROOT_DIRS += "${M33FW_DIR_TFM}"

do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}${M33FW_DIR_TFM}"
do_deploy() {
    install -d ${DEPLOYDIR}
    unset i
    for config in ${M33FW_CONFIG}; do
        i=$(expr $i + 1)
        # Initialize devicetree list, boot suffix a35 and boot suffix m33
        dt_config=$(echo ${M33FW_DEVICETREE} | cut -d',' -f${i})
        suffix_boota35=$(echo ${M33FW_BOOT_SUFFIX_A35} | cut -d',' -f${i})
        [ -z "${suffix_boota35}" ] || suffix_boota35="-${suffix_boota35}"
        suffix_bootm33=$(echo ${M33FW_BOOT_SUFFIX_M33} | cut -d',' -f${i})
        [ -z "${suffix_bootm33}" ] || suffix_bootm33="-${suffix_bootm33}"
        for dt in ${dt_config}; do
            # Configuration for build path (one per build)
            output_path="${B}/${config}-${dt}"

            # BL2 firmware
            if [ -e "${output_path}/spe/bin/${BL2_NAME}.${BL2_SUFFIX}" ]; then
                install -d ${DEPLOYDIR}/bl2
                install -m 0644 ${output_path}/spe/bin/${BL2_NAME}.${BL2_SUFFIX} ${DEPLOYDIR}/bl2/${BL2_NAME}-${dt}${suffix_bootm33}.${BL2_SUFFIX}
                # Add debug wrapper
                if [ "${TF_M_ENABLE_DEBUG_WRAPPER}" = "1" ]; then
                    install -d ${DEPLOYDIR}/bl2/debug
                    stm32wrapper4dbg -s "${DEPLOYDIR}/bl2/${BL2_NAME}-${dt}${suffix_bootm33}.${BL2_SUFFIX}" -d "${DEPLOYDIR}/bl2/debug/debug-${BL2_NAME}-${dt}${suffix_bootm33}.${BL2_SUFFIX}"
                fi
                # Add device tree for debug
                if [ -e "${output_path}/spe/build-spe/generated/devicetree/platform_bl2/out.${DTS_SUFFIX}" ]; then
                    install -d ${DEPLOYDIR}/bl2/debug
                    install -m 0644 ${output_path}/spe/build-spe/generated/devicetree/platform_bl2/out.${DTS_SUFFIX} ${DEPLOYDIR}/bl2/debug/${dt}${suffix_bootm33}-${BL2_NAME}.${DTS_SUFFIX}
                fi
                if [ -n "${ELF_DEBUG_ENABLE}" ]; then
                    if [ -e "${output_path}/spe/bin/${BL2_NAME}.${TF_M_ELF_SUFFIX}" ]; then
                        install -d ${DEPLOYDIR}/bl2/debug
                        install -m 0644 ${output_path}/spe/bin/${BL2_NAME}.${TF_M_ELF_SUFFIX} ${DEPLOYDIR}/bl2/debug/${BL2_NAME}-${dt}${suffix_bootm33}.${TF_M_ELF_SUFFIX}
                    fi
                fi
            fi
            # DDR firwmare
            if [ -e "${output_path}/spe/bin/${FWDDR_NAME}.${FWDDR_SUFFIX}" ]; then
                install -d ${DEPLOYDIR}${M33FW_DIR_FWDDR}
                install -m 0644 ${output_path}/spe/bin/${FWDDR_NAME}.${FWDDR_SUFFIX} ${DEPLOYDIR}${M33FW_DIR_FWDDR}/${FWDDR_NAME}-${dt}${suffix_bootm33}.${FWDDR_SUFFIX}
                if [ -e "${output_path}/spe/api_ns/bin/layout_files/${FWDDR_LAYOUT_BASENAME}${TF_M_LAYOUT_NAME}.${TF_M_LAYOUT_SUFFIX}" ]; then
                    install -m 0644 ${output_path}/spe/api_ns/bin/layout_files/${FWDDR_LAYOUT_BASENAME}${TF_M_LAYOUT_NAME}.${TF_M_LAYOUT_SUFFIX} ${DEPLOYDIR}${M33FW_DIR_FWDDR}/${FWDDR_NAME}-${dt}${suffix_bootm33}.${TF_M_LAYOUT_NAME}
                fi
            fi
            # TFM firmware
            if [ -e "${output_path}/spe/bin/${TF_M_NAME}${TF_M_TYPE_S}.${TF_M_SUFFIX}" ]; then
                install -m 0644 ${output_path}/spe/bin/${TF_M_NAME}${TF_M_TYPE_S}.${TF_M_SUFFIX} ${DEPLOYDIR}/${TF_M_NAME}-${dt}${suffix_bootm33}${suffix_boota35}${TF_M_TYPE_S}.${TF_M_SUFFIX}
            fi
            if [ -e "${output_path}/spe/api_ns/image_signing/layout_files/${TF_M_LAYOUT_NAME}${TF_M_TYPE_S}${TF_M_TYPE_NS}.${TF_M_LAYOUT_SUFFIX}" ]; then
                install -m 0644 ${output_path}/spe/api_ns/image_signing/layout_files/${TF_M_LAYOUT_NAME}${TF_M_TYPE_S}${TF_M_TYPE_NS}.${TF_M_LAYOUT_SUFFIX} ${DEPLOYDIR}/${TF_M_NAME}-${dt}${suffix_bootm33}${suffix_boota35}${TF_M_TYPE_S}.${TF_M_LAYOUT_NAME}
            fi
            # Add device tree for debug
            if [ -e "${output_path}/spe/build-spe/generated/devicetree/platform_s/out.${DTS_SUFFIX}" ]; then
                install -d ${DEPLOYDIR}/debug
                install -m 0644 ${output_path}/spe/build-spe/generated/devicetree/platform_s/out.${DTS_SUFFIX} ${DEPLOYDIR}/debug/${dt}${suffix_bootm33}${suffix_boota35}-${TF_M_NAME}${TF_M_TYPE_S}.${DTS_SUFFIX}
            fi
            if [ -e "${output_path}/spe/bin/${TF_M_NAME}${TF_M_TYPE_S}.hex" ]; then
                install -d ${DEPLOYDIR}/debug
                install -m 0644 ${output_path}/spe/bin/${TF_M_NAME}${TF_M_TYPE_S}.hex ${DEPLOYDIR}/debug/${TF_M_NAME}-${dt}${suffix_bootm33}${suffix_boota35}${TF_M_TYPE_S}.hex
            fi
            if [ -e "${output_path}/spe/bin/${TF_M_NAME}${TF_M_TYPE_S}.map" ]; then
                install -d ${DEPLOYDIR}/debug
                install -m 0644 ${output_path}/spe/bin/${TF_M_NAME}${TF_M_TYPE_S}.map ${DEPLOYDIR}/debug/${TF_M_NAME}-${dt}${suffix_bootm33}${suffix_boota35}${TF_M_TYPE_S}.map
            fi
            if [ -n "${ELF_DEBUG_ENABLE}" ]; then
                if [ -e "${output_path}/spe/bin/${TF_M_NAME}${TF_M_TYPE_S}.${TF_M_ELF_SUFFIX}" ]; then
                    install -d ${DEPLOYDIR}/debug
                    install -m 0644 ${output_path}/spe/bin/${TF_M_NAME}${TF_M_TYPE_S}.${TF_M_ELF_SUFFIX} ${DEPLOYDIR}/debug/${TF_M_NAME}-${dt}${suffix_bootm33}${suffix_boota35}${TF_M_TYPE_S}.${TF_M_ELF_SUFFIX}
                fi
            fi
        done
    done
}
addtask deploy before do_build after do_compile

do_deploy[postfuncs] += "sign_bl2_fw"
# Sign bl2 firmware
sign_bl2_fw() {
    # Init signing suffix
    if [ "${ENCRYPT_ENABLE}" = "1" ]; then
        signing_suffix="${ENCRYPT_SUFFIX}${SIGN_SUFFIX}"
    else
        signing_suffix="${SIGN_SUFFIX}"
    fi

    for bl2 in $(find ${DEPLOYDIR}/ -name "${BL2_NAME}-*.${BL2_SUFFIX}"); do
        for soc in ${STM32MP_SOC_NAME}; do
            basename "${bl2}" | grep -q "^${BL2_NAME}-${soc}" || continue
            # Get bl2 bin radix name
            bl2_name="$(echo "${bl2}" | sed 's#\.'"${BL2_SUFFIX}"'$##')"
            # Sign bl2
            sign_stm32mp_fw \
                    "${bl2}" \
                    "cm33" \
                    "${soc}" \
                    "${bl2_name}${signing_suffix}.${BL2_SUFFIX}"
        done
    done
    # Sign for TF_M_ENABLE_DEBUG_WRAPPER
    for bl2_wrapper in $(find ${DEPLOYDIR}/ -name "debug-${BL2_NAME}-*.${BL2_SUFFIX}"); do
        for soc in ${STM32MP_SOC_NAME}; do
            basename "${bl2_wrapper}" | grep -q "^debug-${BL2_NAME}-${soc}" || continue
            # Get bl2 bin radix name
            bl2_wrapper_name="$(echo "${bl2_wrapper}" | sed 's#\.'"${BL2_SUFFIX}"'$##')"
            # Sign bl2
            sign_stm32mp_fw \
                    "${bl2_wrapper}" \
                    "cm33" \
                    "${soc}" \
                    "${bl2_wrapper_name}${signing_suffix}.${BL2_SUFFIX}"
        done
    done
}

# Build paths are currently embedded
INSANE_SKIP:${PN} += "buildpaths"
INSANE_SKIP:${PN} += "arch"
