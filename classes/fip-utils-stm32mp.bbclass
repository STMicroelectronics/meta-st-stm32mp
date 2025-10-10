inherit sign-stm32mp

DEPENDS += "tf-a-tools-native util-linux-native"

BBCLASSEXTEND:append = " nativesdk"

# Define default TF-A FIP namings
FIP_BASENAME ?= "fip"
FIP_SUFFIX   ?= "bin"

# Set default TF-A FIP config
FIP_CONFIG ?= ""

# Default FIP config:
#   There are two options implemented to select two different firmware and each
#   FIP_CONFIG should configure one: 'tfa' or 'optee'
FIP_CONFIG_FW_TFA = "tfa"
FIP_CONFIG_FW_TEE = "optee"

# Init BL31 config
FIP_BL31_ENABLE ?= ""

# Set CERTTOOL binary name to use
CERTTOOL ?= "cert_create"
# Set ENCTOOL binary name to use
ENCTOOL ?= "encrypt_fw"
# Set FIPTOOL binary name to use
FIPTOOL ?= "fiptool"
# Set STM32MP fiptool wrapper
FIPTOOL_WRAPPER ?= "fiptool-stm32mp.${MACHINE}"

# Configure default folder path for binaries to package
FIP_DIR_FIP    ?= "/fip"
FIP_DIR_TFA_BASE ?= "/arm-trusted-firmware"
FIP_DIR_BL31   ?= "/bl31"
FIP_DIR_TFA    ?= "/bl32"
FIP_DIR_FWCONF ?= "/fwconfig"
FIP_DIR_FWDDR  ?= "/ddr"
FIP_DIR_OPTEE  ?= "/optee"
FIP_DIR_UBOOT  ?= "/u-boot"

# Set default configuration to allow FIP signing
FIP_ENCRYPT_SUFFIX ??= "${@bb.utils.contains('ENCRYPT_ENABLE', '1', '${ENCRYPT_SUFFIX}', '', d)}"
FIP_ENCRYPT_NONCE ??= "1234567890abcdef12345678"
FIP_SIGN_SUFFIX ??= "${@bb.utils.contains('SIGN_ENABLE', '1', '${SIGN_SUFFIX}', '', d)}"

FIP_WRAPPER ??= "${RECIPE_SYSROOT_NATIVE}/${bindir}/create_st_fip_binary.sh"
# -----------------------------------------------
# Handle FIP config and set internal vars
#   FIP_BL32_CONF
#   FIP_DEVICETREE
#   FIP_DEVICETREE_SUFFIX
#   FIP_SEARCH_CONF
#   FIP_DEVICE_CONF
python () {
    import re

    # Make sure that deploy class is configured
    if not bb.data.inherits_class('deploy', d):
         bb.fatal("The st-fip-utils class needs the deploy class to be configured on recipe side.")

    # Manage FIP config settings
    fipconfigflags = d.getVarFlags('FIP_CONFIG')
    if fipconfigflags is not None:
        # The "doc" varflag is special, we don't want to see it here
        fipconfigflags.pop('doc', None)
    fipconfig = (d.getVar('FIP_CONFIG') or "").split()
    if not fipconfig:
        raise bb.parse.SkipRecipe("FIP_CONFIG must be set in the %s machine configuration." % d.getVar("MACHINE"))
    if (d.getVar('FIP_BL32_CONF') or "").split():
        raise bb.parse.SkipRecipe("You cannot use FIP_BL32_CONF as it is internal to FIP_CONFIG var expansion.")
    if (d.getVar('FIP_DEVICETREE') or "").split():
        raise bb.parse.SkipRecipe("You cannot use FIP_DEVICETREE as it is internal to FIP_CONFIG var expansion.")
    if (d.getVar('FIP_DEVICETREE_SUFFIX') or "").split():
        raise bb.parse.SkipRecipe("You cannot use FIP_DEVICETREE_SUFFIX as it is internal to FIP_CONFIG var expansion.")
    if (d.getVar('FIP_SEARCH_CONF') or "").split():
        raise bb.parse.SkipRecipe("You cannot use FIP_SEARCH_CONF as it is internal to FIP_CONFIG var expansion.")
    if (d.getVar('FIP_DEVICE_CONF') or "").split():
        raise bb.parse.SkipRecipe("You cannot use FIP_DEVICE_CONF as it is internal to FIP_CONFIG var expansion.")
    if len(fipconfig) > 0:
        # Init internal fip firmware config
        fip_config_fw_tfa = d.getVar('FIP_CONFIG_FW_TFA') or ""
        fip_config_fw_tee = d.getVar('FIP_CONFIG_FW_TEE') or ""
        for config in fipconfig:
            for f, v in fipconfigflags.items():
                if config == f:
                    # Make sure to get var flag properly expanded
                    v = d.getVarFlag('FIP_CONFIG', config)
                    if not v.strip():
                        bb.fatal('[FIP_CONFIG] Missing configuration for %s config' % config)
                    items = v.split(',')
                    if items[0] and len(items) > 5:
                        raise bb.parse.SkipRecipe('Only <BL32_CONF>, <DT_CONFIG>, <DT_SUFFIX>, <SEARCH_CONF> and <DEVICE_CONF> can be specified! (items={})'.format(items))
                    # Set internal vars
                    if items[0] == fip_config_fw_tfa or items[0] == fip_config_fw_tee:
                        bb.debug(1, "Appending '%s' to FIP_BL32_CONF" % items[0])
                        d.appendVar('FIP_BL32_CONF', items[0].strip() + ',')
                    else:
                        bb.fatal('[FIP_CONFIG] Wrong configuration for %s config: %s should be one of %s or %s' % (config,items[0],fip_config_fw_tfa,fip_config_fw_tee))
                    if items[2]:
                        bb.debug(1, "Appending '%s' to FIP_DEVICETREE_SUFFIX." % items[2])
                        d.appendVar('FIP_DEVICETREE_SUFFIX', items[2].strip() + ',')
                    else:
                        d.appendVar('FIP_DEVICETREE_SUFFIX', '' + ',')
                    if items[3]:
                        bb.debug(1, "Appending '%s' to FIP_SEARCH_CONF" % items[0])
                        d.appendVar('FIP_SEARCH_CONF', items[3].strip() + ',')
                    else:
                        bb.fatal('[FIP_CONFIG] Wrong configuration for <UBOOT_CONF>. It must be specified')
                    if len(items) == 5:
                        bb.debug(1, "Appending '%s' to FIP_DEVICE_CONF" % items[4])
                        d.appendVar('FIP_DEVICE_CONF', items[4].strip() + ',')
                    else:
                        d.appendVar('FIP_DEVICE_CONF', ',')
                    bb.debug(1, "Appending '%s' to FIP_DEVICETREE" % items[1])
                    d.appendVar('FIP_DEVICETREE', items[1] + ',')
                    break
}

archiver_create_fiptool_wrapper_for_sdk() {
    # Create the FIPTOOL_WRAPPER script to use on sdk side
    mkdir -p ${ARCHIVER_OUTDIR}
    cat << EOF > ${ARCHIVER_OUTDIR}/${FIPTOOL_WRAPPER}
#!/bin/bash -
function bbfatal() { echo "\$*" ; exit 1 ; }

# Set default TF-A FIP config
FIP_CONFIG="\${FIP_CONFIG:-${@' '.join(d for d in '${FIP_CONFIG}'.split() if not 'fastboot-' in d)}}"

FIP_BL31_ENABLE="\${FIP_BL31_ENABLE:-${FIP_BL31_ENABLE}}"
FIP_BL32_CONF=""
FIP_DEVICETREE="\${FIP_DEVICETREE:-}"
FIP_DEVICETREE_SUFFIX=
FIP_SEARCH_CONF=""
FIP_DEVICE_CONF=""
# Set default supported configuration for devicetree and bl32 configuration
declare -A FIP_BL32_CONF_ARRAY
declare -A FIP_DEVICETREE_ARRAY
declare -A FIP_DEVICETREE_SUFFIX_ARRAY
declare -A FIP_SEARCH_CONF_ARRAY
declare -A FIP_DEVICE_CONF_ARRAY
EOF
    for config in ${FIP_CONFIG}; do
        i=$(expr $i + 1)
        cat << EOF >> ${ARCHIVER_OUTDIR}/${FIPTOOL_WRAPPER}
FIP_BL32_CONF_ARRAY[${config}]="$(echo ${FIP_BL32_CONF} | cut -d',' -f${i})"
FIP_DEVICETREE_ARRAY[${config}]="$(echo ${FIP_DEVICETREE} | cut -d',' -f${i})"
FIP_DEVICETREE_SUFFIX_ARRAY[${config}]="$(echo ${FIP_DEVICETREE_SUFFIX} | cut -d',' -f${i})"
FIP_SEARCH_CONF_ARRAY[${config}]="$(echo ${FIP_SEARCH_CONF} | cut -d',' -f${i})"
FIP_DEVICE_CONF_ARRAY[${config}]="$(echo ${FIP_DEVICE_CONF} | cut -d',' -f${i})"
EOF
    done
    unset i
    cat << EOF >> ${ARCHIVER_OUTDIR}/${FIPTOOL_WRAPPER}

# Make sure about FIP_CONFIG value
if [ -z "\$FIP_CONFIG" ]; then
    bbfatal "Wrong configuration 'FIP_CONFIG' is empty."
else
    # Check that configuration match any of the supported ones
    for config in \$FIP_CONFIG; do
        CONFIG_FOUND=NO
        for fip_config in ${FIP_CONFIG}; do
            [ "\${config}" = "\${fip_config}" ] && { CONFIG_FOUND="YES" ; break; }
        done
        [ "\${CONFIG_FOUND}" = "NO" ] && bbfatal "Wrong 'FIP_CONFIG' configuration : \${config} is not one of the supported one (${FIP_CONFIG})"
    done
fi
# Manage FIP_BL32_CONF default init
if [ -z "\$FIP_BL32_CONF" ]; then
    # Assigned default supported value
    for config in \$FIP_CONFIG; do
        FIP_BL32_CONF+="\${FIP_BL32_CONF_ARRAY[\${config}]},"
    done
fi
# Manage FIP_DEVICETREE default init
if [ -z "\$FIP_DEVICETREE" ]; then
    # Assigned default supported value
    for config in \$FIP_CONFIG; do
        FIP_DEVICETREE+="\${FIP_DEVICETREE_ARRAY[\${config}]},"
    done
fi
# Manage FIP_DEVICETREE_SUFFIX default init
if [ -z "\$FIP_DEVICETREE_SUFFIX" ]; then
    # Assigned default supported value
    for config in \$FIP_CONFIG; do
        FIP_DEVICETREE_SUFFIX+="\${FIP_DEVICETREE_SUFFIX_ARRAY[\${config}]},"
    done
fi
# Manage FIP_SEARCH_CONF default init
if [ -z "\$FIP_SEARCH_CONF" ]; then
    # Assigned default supported value
    for config in \$FIP_CONFIG; do
        FIP_SEARCH_CONF+="\${FIP_SEARCH_CONF_ARRAY[\${config}]},"
    done
fi
# Manage FIP_DEVICE_CONF default init
if [ -z "\$FIP_DEVICE_CONF" ]; then
    # Assigned default supported value
    for config in \$FIP_CONFIG; do
        FIP_DEVICE_CONF+="\${FIP_DEVICE_CONF_ARRAY[\${config}]},"
    done
fi

# Configure default folder path for binaries to package
FIP_DEPLOYDIR_ROOT="\${FIP_DEPLOYDIR_ROOT:-}"
if [ -z "\${FIP_DEPLOYDIR_ROOT}" ] ; then
    echo "--------------------------------------------------------"
    echo "FIP: STOP generation of fip"
    echo "because all binaries mandatory to generate FIP are present and must be provided."
    echo "Please verify that FIP_DEPLOYDIR_ROOT variable is correctly populated and contains the binaries requested by fip generation."
    echo "--------------------------------------------------------"
    exit 1
fi

FIP_DEPLOYDIR_FIP="\${FIP_DEPLOYDIR_FIP:-\$FIP_DEPLOYDIR_ROOT${FIP_DIR_FIP}}"
FIP_DEPLOYDIR_TFA="\${FIP_DEPLOYDIR_TFA:-\$FIP_DEPLOYDIR_ROOT${FIP_DIR_TFA_BASE}${FIP_DIR_TFA}}"
FIP_DEPLOYDIR_BL31="\${FIP_DEPLOYDIR_BL31:-\$FIP_DEPLOYDIR_ROOT${FIP_DIR_TFA_BASE}${FIP_DIR_BL31}}"
FIP_DEPLOYDIR_FWDDR="\${FIP_DEPLOYDIR_FWDDR:-\$FIP_DEPLOYDIR_ROOT${FIP_DIR_TFA_BASE}${FIP_DIR_FWDDR}}"
FIP_DEPLOYDIR_FWCONF="\${FIP_DEPLOYDIR_FWCONF:-\$FIP_DEPLOYDIR_ROOT${FIP_DIR_TFA_BASE}${FIP_DIR_FWCONF}}"
FIP_DEPLOYDIR_OPTEE="\${FIP_DEPLOYDIR_OPTEE:-\$FIP_DEPLOYDIR_ROOT${FIP_DIR_OPTEE}}"
FIP_DEPLOYDIR_UBOOT="\${FIP_DEPLOYDIR_UBOOT:-\$FIP_DEPLOYDIR_ROOT${FIP_DIR_UBOOT}}"
FIP_WRAPPER="create_st_fip_binary.sh"

echo ""
echo "${FIPTOOL_WRAPPER} config:"
for config in \$FIP_CONFIG; do
    i=\$(expr \$i + 1)
    bl32_conf=\$(echo \$FIP_BL32_CONF | cut -d',' -f\$i)
    dt_config=\$(echo \$FIP_DEVICETREE | cut -d',' -f\$i)
    dt_suffix=\$(echo \$FIP_DEVICETREE_SUFFIX | cut -d',' -f\$i)
    search_conf=\$(echo \$FIP_SEARCH_CONF | cut -d',' -f\$i)
    device_conf=\$(echo \$FIP_DEVICE_CONF | cut -d',' -f\$i)
    echo "  \${config}:" ; \\
    echo "    bl32 config value: \${bl32_conf}"
    echo "    devicetree config: \${dt_config}"
    echo "    devicetree suffix: \${dt_suffix}"
    echo "    search config    : \${search_conf}"
    echo "    device config    : \${device_conf}"
done
echo ""
echo "Switch configuration:"
echo "  FIP_BL31_ENABLE : \$FIP_BL31_ENABLE"
echo ""
echo "Output folders:"
echo "  FIP_DEPLOYDIR_ROOT  : \$FIP_DEPLOYDIR_ROOT"
echo "  FIP_DEPLOYDIR_FIP   : \$FIP_DEPLOYDIR_FIP"
echo "  FIP_DEPLOYDIR_TFA   : \$FIP_DEPLOYDIR_TFA"
echo "  FIP_DEPLOYDIR_BL31  : \$FIP_DEPLOYDIR_BL31"
echo "  FIP_DEPLOYDIR_FWCONF: \$FIP_DEPLOYDIR_FWCONF"
echo "  FIP_DEPLOYDIR_FWDDR : \$FIP_DEPLOYDIR_FWDDR"
echo "  FIP_DEPLOYDIR_OPTEE : \$FIP_DEPLOYDIR_OPTEE"
echo "  FIP_DEPLOYDIR_UBOOT : \$FIP_DEPLOYDIR_UBOOT"
echo ""
unset i
for config in \$FIP_CONFIG; do
    i=\$(expr \$i + 1)
    bl32_conf=\$(echo \$FIP_BL32_CONF | cut -d',' -f\$i)
    dt_config=\$(echo \$FIP_DEVICETREE | cut -d',' -f\$i)
    dt_suffix=\$(echo \$FIP_DEVICETREE_SUFFIX | cut -d',' -f\$i)
    search_conf=\$(echo \$FIP_SEARCH_CONF | cut -d',' -f\$i)
    device_conf=\$(echo \$FIP_DEVICE_CONF | cut -d',' -f\$i)
    for dt in \${dt_config}; do
        # Init soc suffix
        soc_suffix=""
        if [ -n "${STM32MP_SOC_NAME}" ]; then
            for soc in ${STM32MP_SOC_NAME}; do
                [ "\$(echo \${dt} | grep -c \${soc})" -eq 1 ] && soc_suffix="\${soc}"
            done
        fi
        # Init FIP bl31 settings
        FIP_PARAM_BLxx=""
        if [ "\$FIP_BL31_ENABLE" = "1" ]; then
            FIP_PARAM_BLxx="--use-bl31"
        fi
        # Init FIP bl32 settings
        if [ "\${bl32_conf}" = "${FIP_CONFIG_FW_TFA}" ]; then
            FIP_PARAM_BLxx="--use-bl32"
        elif [ -n "\${bl32_conf}" ] && [ "\${bl32_conf}" != "${FIP_CONFIG_FW_TEE}" ]; then
            bbfatal "Wrong configuration '\${bl32_conf}' found in FIP_CONFIG for \${config} config."
        fi

        # Configure storage search
        STORAGE_SEARCH=""
        [ -z "\${device_conf}" ] || STORAGE_SEARCH="--search-storage \${device_conf}"

        # Configure devicetree suffix search
        DT_SUFFIX_SEARCH=""
        [ -z "\${dt_suffix}" ] || DT_SUFFIX_SEARCH="--search-devicetree-suffix \${dt_suffix}"

        FIP_PARAM_ddr=""
        if [ -d "\$FIP_DEPLOYDIR_FWDDR" ]; then
            FIP_PARAM_ddr="--use-ddr"
            \$FIP_WRAPPER \\
                \$FIP_PARAM_BLxx \\
                \$STORAGE_SEARCH \\
                --use-ddr --generate-only-ddr \\
                --search-configuration \${config} \\
                --search-devicetree \${dt} \\
                \$DT_SUFFIX_SEARCH \\
                --search-soc-name \${soc_suffix} \\
                --output \$FIP_DEPLOYDIR_FIP
        fi

        SECOND_CONFSEARCH=""
        [ -z "\${search_conf}" ] || SECOND_CONFSEARCH="--search-secondary-config \${search_conf}"

        echo "\$FIP_WRAPPER \\
                \$FIP_PARAM_BLxx \\
                \$FIP_PARAM_ddr \\
                \$STORAGE_SEARCH \\
                \$SECOND_CONFSEARCH \\
                --search-configuration \${config} \\
                --search-devicetree \${dt} \\
                \$DT_SUFFIX_SEARCH \\
                --search-soc-name \${soc_suffix} \\
                --output \$FIP_DEPLOYDIR_FIP"
        \$FIP_WRAPPER \\
                \$FIP_PARAM_BLxx \\
                \$FIP_PARAM_ddr \\
                \$STORAGE_SEARCH \\
                \$SECOND_CONFSEARCH \\
                --search-configuration \${config} \\
                --search-devicetree \${dt} \\
                \$DT_SUFFIX_SEARCH \\
                --search-soc-name \${soc_suffix} \\
                --output \$FIP_DEPLOYDIR_FIP
    done
done
EOF

    chmod 755 ${ARCHIVER_OUTDIR}/${FIPTOOL_WRAPPER}
}
do_ar_original[prefuncs] += "archiver_create_fiptool_wrapper_for_sdk"
