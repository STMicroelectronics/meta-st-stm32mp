SUMMARY = "M33 firmware generation"
LICENSE = "BSD-3-Clause"

COMPATIBLE_MACHINE = "(stm32mp2common)"

PACKAGE_ARCH = "${MACHINE_ARCH}"

PV = "6.2"

DEPENDS += "${@bb.utils.contains('MACHINE_FEATURES', 'm33td', 'm33tdprojects-starter-stm32mp2', 'm33projects-stm32mp2', d)}"
DEPENDS += "virtual/trusted-firmware-m"

inherit deploy

DEPENDS += "tf-m-stm32mp-scripts-native"
inherit m33fw-utils-stm32mp

M33FW_OPENSSL_MODULES ?= "${STAGING_LIBDIR_NATIVE}/ossl-modules"
M33FW_TFM_DEV_KIT_DIR ?= "${STAGING_DATADIR_NATIVE}/tf-m"

do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}${M33FW_DIR_M33FW}"
do_deploy() {
    install -d ${DEPLOYDIR}
    unset i
    for config in ${M33FW_CONFIG}; do
        i=$(expr $i + 1)
        # Initialize devicetree list, boot a35 suffix and boot m33 suffix
        dt_config=$(echo ${M33FW_DEVICETREE} | cut -d',' -f${i})
        suffix_boota35=$(echo ${M33FW_BOOT_SUFFIX_A35} | cut -d',' -f${i})
        [ -z "${suffix_boota35}" ] || suffix_boota35="-${suffix_boota35}"
        suffix_bootm33=$(echo ${M33FW_BOOT_SUFFIX_M33} | cut -d',' -f${i})
        [ -z "${suffix_bootm33}" ] || suffix_bootm33="-${suffix_bootm33}"
        for dt in ${dt_config}; do
            if [ "${SIGN_ENABLE}" = "1" ]; then
                key_params_found=0
                unset k
                for soc in ${STM32MP_SOC_NAME}; do
                    k=$(expr $k + 1)
                    echo ${dt} | grep -q ${soc} || continue
                    key_params_found=1
                    ddr_priv_sign_key=$(echo ${SIGN_M33DDR_KEY_PATH_LIST} | cut -d',' -f${k})
                    ddr_pass_sign_key=$(echo ${SIGN_M33DDR_KEY_PASS_LIST} | cut -d',' -f${k})
                    m33_priv_sign_key=$(echo ${SIGN_M33FW_KEY_PATH_LIST} | cut -d',' -f${k})
                    m33_pass_sign_key=$(echo ${SIGN_M33FW_KEY_PASS_LIST} | cut -d',' -f${k})
                    break
                done
                [ "${key_params_found}" -eq 1 ] || bbfatal "[${config}] Missing signing parameters for ${dt}"
            else
                ddr_priv_sign_key=""
                ddr_pass_sign_key=""
                m33_priv_sign_key=""
                m33_pass_sign_key=""
            fi
            # Configure signature settings
            [ -z "${ddr_priv_sign_key}" ] || ddr_priv_sign_key="--signature-key ${ddr_priv_sign_key}"
            [ -z "${ddr_pass_sign_key}" ] || ddr_pass_sign_key="--signature-key-pass ${ddr_pass_sign_key}"
            [ -z "${m33_priv_sign_key}" ] || m33_priv_sign_key="--signature-key ${m33_priv_sign_key}"
            [ -z "${m33_pass_sign_key}" ] || m33_pass_sign_key="--signature-key-pass ${m33_pass_sign_key}"

            # Override default env for SIGN_COPRO_TOOL
            [ -z "${M33FW_TFM_DEV_KIT_DIR}" ] || export TFM_DEV_KIT_DIR="${M33FW_TFM_DEV_KIT_DIR}"
            [ -z "${M33FW_OPENSSL_MODULES}" ] || export OPENSSL_MODULES="${M33FW_OPENSSL_MODULES}"

            # Configure M33FW default search folder
            export M33FW_DEPLOYDIR_ROOT="${RECIPE_SYSROOT}${M33FW_SYSROOT_M33}"
            export M33FW_DEPLOYDIR_FWDDR="${RECIPE_SYSROOT}${M33FW_SYSROOT_DDR}"
            export M33FW_DEPLOYDIR_CUBE="${RECIPE_SYSROOT}${M33FW_SYSROOT_NS}"
            export M33FW_DEPLOYDIR_TFM="${RECIPE_SYSROOT}${M33FW_SYSROOT_S}"

            echo "****************************************"
            bbnote "M33 firmware DDR command details:
                        \n${M33FW_WRAPPER} \
                                \n--ddrfw \
                                \n--devicetree ${dt} \
                                \n--bootsuffix-m33 ${suffix_bootm33} \
                                \n${ddr_priv_sign_key} \
                                \n${ddr_pass_sign_key} \
                                \n--output ${DEPLOYDIR}"
            echo "****************************************"
            ${M33FW_WRAPPER} \
                    --ddrfw \
                    --devicetree ${dt} \
                    --bootsuffix-m33 ${suffix_bootm33} \
                    ${ddr_priv_sign_key} \
                    ${ddr_pass_sign_key} \
                    --output ${DEPLOYDIR}

            echo "****************************************"
            bbnote "M33 firmware command details:
                        \n${M33FW_WRAPPER} \
                                \n--m33fw \
                                \n--devicetree ${dt} \
                                \n--bootsuffix-m33 ${suffix_bootm33} \
                                \n--bootsuffix-a35 ${suffix_boota35} \
                                \n${m33_priv_sign_key} \
                                \n${m33_pass_sign_key} \
                                \n--output ${DEPLOYDIR}"
            echo "****************************************"
            ${M33FW_WRAPPER} \
                    --m33fw \
                    --devicetree ${dt} \
                    --bootsuffix-m33 ${suffix_bootm33} \
                    --bootsuffix-a35 ${suffix_boota35} \
                    ${m33_priv_sign_key} \
                    ${m33_pass_sign_key} \
                    --output ${DEPLOYDIR}

            # Add default firmware symlink
            m33fw_ns_prefix=""
            if [ "$(echo "${M33FW_DEFAULT_FIRMWARE}" | sed 's/,/ /g' | wc -w)" -eq 1 ]; then
                m33fw_ns_prefix=$(echo ${M33FW_DEFAULT_FIRMWARE} | cut -d',' -f1)
            else
                unset l
                for board in ${M33FW_BOARDS}; do
                    l=$(expr $l + 1)
                    echo ${dt} | grep -q ${board} || continue
                    # Initialize default non-secure firmware prefix
                    m33fw_ns_prefix=$(echo ${M33FW_DEFAULT_FIRMWARE} | cut -d',' -f${l})
                done
            fi
            if [ -e "${DEPLOYDIR}/${M33FW_S_PREFIX}-${m33fw_ns_prefix}-${dt}${suffix_bootm33}${suffix_boota35}${M33FW_TYPE}${M33FW_SIGN_SUFFIX}.${M33FW_SUFFIX}" ]; then
                cd ${DEPLOYDIR}
                ln -sf ${M33FW_S_PREFIX}-${m33fw_ns_prefix}-${dt}${suffix_bootm33}${suffix_boota35}${M33FW_TYPE}${M33FW_SIGN_SUFFIX}.${M33FW_SUFFIX} ${M33FW_PREFIX}-${dt}${suffix_bootm33}${suffix_boota35}${M33FW_TYPE}${M33FW_SIGN_SUFFIX}.${M33FW_SUFFIX}
            fi
        done
    done
}
addtask deploy before do_build after do_compile
