SUMMARY = "FIP generation"
LICENSE = "BSD-3-Clause"

PACKAGE_ARCH = "${MACHINE_ARCH}"

inherit sign-stm32mp
inherit fip-utils-stm32mp

COMPATIBLE_MACHINE = "(stm32mpcommon)"
PACKAGE_ARCH = "${MACHINE_ARCH}"

DEPENDS += "tf-a-tools-native util-linux-native"
DEPENDS += "virtual/trusted-firmware-a"
DEPENDS += "virtual-optee-os"
DEPENDS += "virtual/bootloader"

inherit deploy

PV = "6.0"

# Deploy the fip binary for current target
do_deploy() {
    install -d ${DEPLOYDIR}/${FIP_DIR_FIP}

    unset i
    for config in ${FIP_CONFIG}; do
        i=$(expr $i + 1)
        bl32_conf=$(echo ${FIP_BL32_CONF} | cut -d',' -f${i})
        dt_config=$(echo ${FIP_DEVICETREE} | cut -d',' -f${i})
        dt_suffix=$(echo ${FIP_DEVICETREE_SUFFIX} | cut -d',' -f${i})
        [ "${EXTDT_USE_SUFFIX}" = "1" ] || dt_suffix=""
        search_conf=$(echo ${FIP_SEARCH_CONF} | cut -d',' -f${i})
        device_conf=$(echo ${FIP_DEVICE_CONF} | cut -d',' -f${i})
        for dt in ${dt_config}; do
            # Init soc suffix
            soc_suffix=""
            if [ -n "${STM32MP_SOC_NAME}" ]; then
                for soc in ${STM32MP_SOC_NAME}; do
                    [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] && soc_suffix="${soc}"
                done
            fi
            encrypt_key=""
            if [ "${ENCRYPT_ENABLE}" = "1" ]; then
                encrypt_key="${ENCRYPT_FIP_KEY_PATH_LIST}"
                if [ -n "${STM32MP_ENCRYPT_SOC_NAME}" ]; then
                    unset k
                    for soc in ${STM32MP_ENCRYPT_SOC_NAME}; do
                        k=$(expr $k + 1)
                        [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] && encrypt_key=$(echo ${ENCRYPT_FIP_KEY_PATH_LIST} | cut -d',' -f${k})
                    done
                fi
            fi
            # Init FIP bl31 settings
            FIP_PARAM_BLxx=""
            # Init FIP extra conf settings
            if [ "${bl32_conf}" = "${FIP_CONFIG_FW_TFA}" ]; then
                FIP_PARAM_BLxx="--use-bl32"
            elif [ "${bl32_conf}" = "${FIP_CONFIG_FW_TEE}" ]; then
                if [ "${FIP_BL31_ENABLE}" = "1" ]; then
                    FIP_PARAM_BLxx="--use-bl31"
                    if [ "${ENCRYPT_ENABLE}" = "1" ]; then
                         FIP_PARAM_BLxx="--use-bl31 --encrypt $encrypt_key"
                    fi
                else
                    if [ "${ENCRYPT_ENABLE}" = "1" ]; then
                        FIP_PARAM_BLxx="--encrypt $encrypt_key"
                    fi
                fi
            else
                bbfatal "Wrong configuration '${bl32_conf}' found in FIP_CONFIG for ${config} config."
            fi
            FIP_PARAM_SIGN=""
            if [ "${SIGN_ENABLE}" = "1" ]; then
               sign_key="${SIGN_KEY_PATH_LIST}"
                if [ $(echo ${SIGN_KEY_PASS} | wc -w) -gt 1 ]; then
                    sign_single_key_pass=$(echo ${SIGN_KEY_PASS} | cut -d' ' -f1)
                else
                    sign_single_key_pass="${SIGN_KEY_PASS}"
                fi
                if [ -n "${STM32MP_SOC_NAME}" ]; then
                    unset k
                    for soc in ${STM32MP_SOC_NAME}; do
                        k=$(expr $k + 1)
                        [ "$(echo ${dt} | grep -c ${soc})" -eq 1 ] && sign_key=$(echo ${SIGN_KEY_PATH_LIST} | cut -d',' -f${k})
                    done
                fi
                FIP_PARAM_SIGN="--sign --signature-key $sign_key --signature-key-pass $sign_single_key_pass"
            fi

            # Configure storage search
            STORAGE_SEARCH=""
            [ -z "${device_conf}" ] || STORAGE_SEARCH="--search-storage ${device_conf}"

            # Configure devicetree suffix search
            DT_SUFFIX_SEARCH=""
            [ -z "${dt_suffix}" ] || DT_SUFFIX_SEARCH="--search-devicetree-suffix ${dt_suffix}"

            FIP_PARAM_ddr=""
            if [ -d "${RECIPE_SYSROOT}/${FIP_DIR_TFA_BASE}/${FIP_DIR_FWDDR}" ]; then
                FIP_PARAM_ddr="--use-ddr"
                echo "********************************************"
                bbnote "[fip-utils-stm32mp] FIP DDR command details:\
                \nFIP_DEPLOYDIR_ROOT=${RECIPE_SYSROOT} \
                \n${FIP_WRAPPER} \
                    \n${FIP_PARAM_BLxx} \
                    \n${FIP_PARAM_SIGN} \
                    \n${STORAGE_SEARCH} \
                    \n--use-ddr --generate-only-ddr \
                    \n--search-configuration ${config}\
                    \n--search-devicetree ${dt} \
                    \n${DT_SUFFIX_SEARCH} \
                    \n--search-soc-name ${soc_suffix} \
                    \n--output ${DEPLOYDIR}/${FIP_DIR_FIP}"
                echo "********************************************"
                FIP_DEPLOYDIR_ROOT="${RECIPE_SYSROOT}" \
                ${FIP_WRAPPER} \
                    ${FIP_PARAM_BLxx} \
                    ${FIP_PARAM_SIGN} \
                    ${STORAGE_SEARCH} \
                    --use-ddr --generate-only-ddr \
                    --search-configuration ${config}\
                    --search-devicetree ${dt} \
                    ${DT_SUFFIX_SEARCH} \
                    --search-soc-name ${soc_suffix} \
                    --output ${DEPLOYDIR}/${FIP_DIR_FIP}
            fi
            # Configure secondary config search
            SECOND_CONFSEARCH=""
            [ -z "${search_conf}" ] || SECOND_CONFSEARCH="--search-secondary-config ${search_conf}"
            echo "****************************************"
            bbnote "[fip-utils-stm32mp] FIP command details:\
            \nFIP_DEPLOYDIR_ROOT=${RECIPE_SYSROOT} \
            \n${FIP_WRAPPER} \
                    \n${FIP_PARAM_BLxx} \
                    \n${FIP_PARAM_SIGN} \
                    \n${FIP_PARAM_ddr} \
                    \n${STORAGE_SEARCH} \
                    \n${SECOND_CONFSEARCH} \
                    \n--search-configuration ${config} \
                    \n--search-devicetree ${dt} \
                    \n${DT_SUFFIX_SEARCH} \
                    \n--search-soc-name ${soc_suffix} \
                    \n--output ${DEPLOYDIR}/${FIP_DIR_FIP}"
            echo "****************************************"
            FIP_DEPLOYDIR_ROOT="${RECIPE_SYSROOT}" \
            ${FIP_WRAPPER} \
                    ${FIP_PARAM_BLxx} \
                    ${FIP_PARAM_SIGN} \
                    ${FIP_PARAM_ddr} \
                    ${STORAGE_SEARCH} \
                    ${SECOND_CONFSEARCH} \
                    --search-configuration ${config} \
                    --search-devicetree ${dt} \
                    ${DT_SUFFIX_SEARCH} \
                    --search-soc-name ${soc_suffix} \
                    --output ${DEPLOYDIR}/${FIP_DIR_FIP}
        done
    done
}
addtask deploy before do_build after do_compile
