inherit sign-stm32mp
inherit python3native

# Configure default folder path for binaries to package
M33FW_DIR_M33FW ?= "/m33-firmware"
M33FW_DIR_CUBE  ?= "/m33-projects"
M33FW_DIR_FWDDR ?= "/ddr"
M33FW_DIR_TFM   ?= "/arm-trusted-firmware-m"

M33FW_SYSROOT_M33 ?= "${M33FW_DIR_M33FW}"
M33FW_SYSROOT_NS  ?= "${M33FW_DIR_M33FW}/fw${M33FW_NS_TYPE}"
M33FW_SYSROOT_S   ?= "${M33FW_DIR_M33FW}/fw${M33FW_S_TYPE}"
M33FW_SYSROOT_DDR ?= "${M33FW_DIR_M33FW}/ddr"

# Define default M33FW namings
M33FW_LAYOUT_SUFFIX ?= "signing_layout"

M33FW_DDR_PREFIX ?= "ddr_phy"
M33FW_DDR_SUFFIX ?= "bin"

M33FW_S_PREFIX ?= "tfm"
M33FW_S_TYPE   ?= "_s"
M33FW_NS_TYPE  ?= "_ns"

M33FW_PREFIX ?= "tfm-starterapp"
M33FW_TYPE   ?= "${M33FW_S_TYPE}${M33FW_NS_TYPE}"
M33FW_SUFFIX ?= "bin"

# Set default configuration to allow M33 firmware signing
#M33FW_ENCRYPT_SUFFIX ??= "${@bb.utils.contains('ENCRYPT_ENABLE', '1', '${ENCRYPT_SUFFIX}', '', d)}"
M33FW_SIGN_SUFFIX ??= "${SIGN_SUFFIX}"

# Set default name for buid configuration
M33FW_BUILDCONF ??= ""
M33FW_PROJECT_DEFAULT_BUILDCONF ??= "no_build_conf"

# Set STM32MP m33 firmware tool wrapper
M33FWTOOL          ?= "st_m33td_firmware_signature.sh"
M33FW_TOOL_WRAPPER ?= "m33fwtool-stm32mp.${MACHINE}"
M33FW_WRAPPER      ?= "create_st_m33fw_binary.sh"

# -----------------------------------------------
# Handle M33FW config and set internal vars
#   M33FW_DEFAULT_FIRMWARE
#   M33FW_DEVICETREE
#   M33FW_DEVICETREE_INTERNAL
#   M33FW_DEVICETREE_EXTERNAL
#   M33FW_BOOT_SUFFIX_M33
#   M33FW_BOOT_SUFFIX_A35
# From M33FW board, set
#   M33FW_PROJECT_PATH
#   M33FW_PROJECT_NAME
#   M33FW_PROJECT_BUILDCONF
#   M33FW_DEFAULT_FIRMWARE
#   M33FW_TFM_PLATFORM
#   M33FW_TFM_OPTFLAGS
# From M33FW buildconf, set
#   M33FW_EXTRA_OPTFLAGS

python () {
    if 'm33td' not in d.getVar('MACHINE_FEATURES').split() and 'm33copro' not in d.getVar('MACHINE_FEATURES').split():
        return

    # Configure M33FW_CONFIG
    m33fwconfigflags = d.getVarFlags('M33FW_CONFIG') or ""
    # The "doc" varflag is special, we don't want to see it here
    m33fwconfigflags.pop('doc', None)
    m33fwconfig = (d.getVar('M33FW_CONFIG') or "").split()

    if not m33fwconfig:
        raise bb.parse.SkipRecipe("M33FW_CONFIG must be set in the %s machine configuration." % d.getVar("MACHINE"))
    if (d.getVar('M33FW_DEVICETREE') or "").split():
        raise bb.parse.SkipRecipe("You cannot use M33FW_DEVICETREE as it is internal to M33FW_CONFIG var expansion.")
    if (d.getVar('M33FW_DEVICETREE_EXTERNAL') or "").split():
        raise bb.parse.SkipRecipe("You cannot use M33FW_DEVICETREE_EXTERNAL as it is internal for var expansion.")
    if (d.getVar('M33FW_DEVICETREE_INTERNAL') or "").split():
        raise bb.parse.SkipRecipe("You cannot use M33FW_DEVICETREE_INTERNAL as it is internal for var expansion.")
    if (d.getVar('M33FW_BOOT_SUFFIX_M33') or "").split():
        raise bb.parse.SkipRecipe("You cannot use M33FW_BOOT_SUFFIX_M33 as it is internal to M33FW_CONFIG var expansion.")
    if (d.getVar('M33FW_BOOT_SUFFIX_A35') or "").split():
        raise bb.parse.SkipRecipe("You cannot use M33FW_BOOT_SUFFIX_A35 as it is internal to M33FW_CONFIG var expansion.")

    if (d.getVar('EXTERNAL_DT_ENABLED') or "0") == "1":
        localdata = bb.data.createCopy(d)
        localdata.setVar('EXTERNAL_DT_ENABLED', '0')

    if len(m33fwconfig) > 0:
        for config in m33fwconfig:
            found = False
            for f, v in m33fwconfigflags.items():
                if config == f:
                    found = True
                    # Make sure to get var flag properly expanded
                    v = d.getVarFlag('M33FW_CONFIG', config)
                    if not v.strip():
                        bb.fatal('[M33FW_CONFIG] Empty configuration for %s config' % config)
                    items = v.split(',')
                    if items[0] and len(items) > 3:
                        raise bb.parse.SkipRecipe('Only <DEVICETREE>,<BOOT_SUFFIX_M33>,<BOOT_SUFFIX_A35> can be specified! (%s: %s, %s)' % (config,items[0],len(items)))
                    # Set internal vars
                    bb.debug(1, "Appending '%s' to M33FW_DEVICETREE" % items[0])
                    d.appendVar('M33FW_DEVICETREE', items[0] + ',')

                    if (d.getVar('EXTERNAL_DT_ENABLED') or "0") == "1":
                        internal_devicetree = localdata.getVarFlag('M33FW_CONFIG', config).split(',')[0]
                        external_devicetree = ' '.join([dt for dt in items[0].split() if dt not in internal_devicetree.split()])
                    else:
                        internal_devicetree = items[0]
                        external_devicetree = ''
                    bb.debug(1, "Appending '%s' to M33FW_DEVICETREE_INTERNAL" % internal_devicetree)
                    d.appendVar('M33FW_DEVICETREE_INTERNAL', internal_devicetree + ',')
                    bb.debug(1, "Appending '%s' to M33FW_DEVICETREE_EXTERNAL" % external_devicetree)
                    d.appendVar('M33FW_DEVICETREE_EXTERNAL', external_devicetree + ',')

                    if len(items) > 1 and items[1]:
                        bb.debug(1, "Appending '%s' to M33FW_BOOT_SUFFIX_M33." % items[1])
                        d.appendVar('M33FW_BOOT_SUFFIX_M33', items[1].strip() + ',')
                    else:
                        d.appendVar('M33FW_BOOT_SUFFIX_M33', '' + ',')
                    if len(items) > 2 and items[2]:
                        bb.debug(1, "Appending '%s' to M33FW_BOOT_SUFFIX_A35." % items[2])
                        d.appendVar('M33FW_BOOT_SUFFIX_A35', items[2].strip() + ',')
                    else:
                        d.appendVar('M33FW_BOOT_SUFFIX_A35', '' + ',')

                    break
            if not found:
                raise bb.parse.SkipRecipe('[M33FW_CONFIG] The selected M33FW_CONFIG key %s has no match in %s' % (config, m33fwconfigflags.keys()))


    # Configure M33FW_FIRMWARE
    m33fwboards = (d.getVar('M33FW_BOARDS') or "").split()

    if not m33fwboards:
        raise bb.parse.SkipRecipe("M33FW_BOARDS must be set in the %s machine configuration." % d.getVar("MACHINE"))
    if (d.getVar('M33FW_PROJECT_PATH') or "").split():
        raise bb.parse.SkipRecipe("You cannot use M33FW_PROJECT_PATH as it is internal to M33FW_BOARDS var expansion.")
    if (d.getVar('M33FW_PROJECT_NAME') or "").split():
        raise bb.parse.SkipRecipe("You cannot use M33FW_PROJECT_NAME as it is internal to M33FW_BOARDS var expansion.")
    if (d.getVar('M33FW_PROJECT_BUILDCONF') or "").split():
        raise bb.parse.SkipRecipe("You cannot use M33FW_PROJECT_BUILDCONF as it is internal to M33FW_BOARDS var expansion.")
    if (d.getVar('M33FW_TFM_PLATFORM') or "").split():
        raise bb.parse.SkipRecipe("You cannot use M33FW_TFM_PLATFORM as it is internal to M33FW_BOARDS var expansion.")
    if (d.getVar('M33FW_TFM_OPTFLAGS') or "").split():
        raise bb.parse.SkipRecipe("You cannot use M33FW_TFM_OPTFLAGS as it is internal to M33FW_BOARDS var expansion.")

    # Configure M33FW_DEFAULT_FIRMWARE
    if (d.getVar('M33FW_DEFAULT_FIRMWARE') or "").split():
        raise bb.parse.SkipRecipe("You cannot use M33FW_DEFAULT_FIRMWARE as it is internal to M33FW_BOARDS var expansion.")

    if len(m33fwboards) > 0:
        m33fwboardsflags = d.getVarFlags('M33FW_FIRMWARE') or ""
        # The "doc" varflag is special, we don't want to see it here
        m33fwboardsflags.pop('doc', None)

        m33fwdefaultflags = d.getVarFlags('M33FW_DEFAULT_FIRMWARE') or ""
        # The "doc" varflag is special, we don't want to see it here
        m33fwdefaultflags.pop('doc', None)

        m33fwtfmflags = d.getVarFlags('M33FW_TFM_PLATFORM') or ""
        # The "doc" varflag is special, we don't want to see it here
        m33fwtfmflags.pop('doc', None)

        m33fwtfmoptflags = d.getVarFlags('M33FW_TFM_OPTFLAGS') or ""
        # The "doc" varflag is special, we don't want to see it here
        m33fwtfmoptflags.pop('doc', None)

        for config in m33fwboards:
            found = False
            for f, v in m33fwboardsflags.items():
                if config == f:
                    found = True
                    # Make sure to get var flag properly expanded
                    v = d.getVarFlag('M33FW_FIRMWARE', config)
                    if not v.strip():
                        bb.fatal('[M33FW_FIRMWARE] Empty configuration for %s config' % config)
                    for prjconf in v.split():
                        items = prjconf.split(',')
                        if items[0] and len(items) > 3:
                            raise bb.parse.SkipRecipe('Only <PROJECT_PATH>,<PROJECT_NAME>,<BUILDCONF_TYPE> can be specified! (%s: %s, %s)' % (config,items[0],len(items)))
                        if items[0] == '':
                            bb.fatal('[M33FW_FIRMWARE] Missing project path setting for %s config' % config)
                        if items[1] == '':
                            bb.warn('[M33FW_FIRMWARE] Missing project name setting for %s config' % config)
                        # Set internal vars
                        bb.debug(1, "Appending '%s' to M33FW_PROJECT_PATH" % items[0])
                        d.appendVar('M33FW_PROJECT_PATH', items[0].strip() + ':')
                        bb.debug(1, "Appending '%s' to M33FW_PROJECT_NAME" % items[1])
                        d.appendVar('M33FW_PROJECT_NAME', items[1].strip() + ':')
                        if len(items) > 2 and items[2]:
                            bb.debug(1, "Appending '%s' to M33FW_PROJECT_BUILDCONF" % items[2])
                            d.appendVar('M33FW_PROJECT_BUILDCONF', items[2].strip() + ':')
                        else:
                            d.appendVar('M33FW_PROJECT_BUILDCONF', d.getVar('M33FW_PROJECT_DEFAULT_BUILDCONF') + ':')
                    # Set config separator
                    bb.debug(1, "Appending ',' to M33FW_PROJECT_PATH")
                    d.appendVar('M33FW_PROJECT_PATH', ',')
                    bb.debug(1, "Appending ',' to M33FW_PROJECT_NAME")
                    d.appendVar('M33FW_PROJECT_NAME', ',')
                    bb.debug(1, "Appending ',' to M33FW_PROJECT_BUILDCONF")
                    d.appendVar('M33FW_PROJECT_BUILDCONF', ',')
                    break
            if not found:
                raise bb.parse.SkipRecipe('[M33FW_BOARDS] The selected M33FW_FIRMWARE key %s has no match in %s' % (config, m33fwboardsflags.keys()))

            found = False
            for f, v in m33fwdefaultflags.items():
                if config == f:
                    found = True
                    # Make sure to get var flag properly expanded
                    v = d.getVarFlag('M33FW_DEFAULT_FIRMWARE', config)
                    if not v.strip() and 'm33td' in d.getVar('MACHINE_FEATURES').split():
                        bb.warn('[M33FW_DEFAULT_FIRMWARE] Empty configuration for %s config' % config)
                    items = v.split(',')
                    if items[0] and len(items) > 1:
                        raise bb.parse.SkipRecipe('Only single M33FW_DEFAULT_FIRMWARE project can be specified! (%s: %s, %s)' % (config,items[0],len(items)))
                    # Set internal vars
                    bb.debug(1, "Appending '%s' to M33FW_DEFAULT_FIRMWARE" % items[0])
                    d.appendVar('M33FW_DEFAULT_FIRMWARE', items[0] + ',')
                    break
            if not found:
                raise bb.parse.SkipRecipe('[M33FW_BOARDS] The selected M33FW_DEFAULT_FIRMWARE key %s has no match in %s' % (config, m33fwdefaultflags.keys()))

            found = False
            for f, v in m33fwtfmflags.items():
                if config == f:
                    found = True
                    # Make sure to get var flag properly expanded
                    v = d.getVarFlag('M33FW_TFM_PLATFORM', config)
                    if not v.strip() and 'm33td' in d.getVar('MACHINE_FEATURES').split():
                        bb.fatal('[M33FW_TFM_PLATFORM] Empty configuration for %s config' % config)
                    items = v.split(',')
                    if items[0] and len(items) > 1:
                        raise bb.parse.SkipRecipe('Only single M33FW_TFM_PLATFORM project can be specified! (%s: %s, %s)' % (config,items[0],len(items)))
                    # Set internal vars
                    bb.debug(1, "Appending '%s' to M33FW_TFM_PLATFORM" % items[0])
                    d.appendVar('M33FW_TFM_PLATFORM', items[0].strip() + ',')
                    break
            if not found:
                raise bb.parse.SkipRecipe('[M33FW_BOARDS] The selected M33FW_TFM_PLATFORM key %s has no match in %s' % (config, m33fwtfmflags.keys()))

            found = False
            for f, v in m33fwtfmoptflags.items():
                if config == f:
                    found = True
                    # Make sure to get var flag properly expanded
                    v = d.getVarFlag('M33FW_TFM_OPTFLAGS', config)
                    # Set internal vars
                    if not v.strip():
                        bb.debug(1, 'Appending empty options to M33FW_TFM_OPTFLAGS for %s config' % config)
                        d.appendVar('M33FW_TFM_OPTFLAGS', '' + ',')
                    else:
                        items = v.split(',')
                        if items[0] and len(items) > 1:
                            raise bb.parse.SkipRecipe('Only single M33FW_TFM_OPTFLAGS project can be specified! (%s: %s, %s)' % (config,items[0],len(items)))
                        bb.debug(1, "Appending '%s' to M33FW_TFM_OPTFLAGS" % items[0])
                        d.appendVar('M33FW_TFM_OPTFLAGS', items[0] + ',')
                    break
            if not found:
                raise bb.parse.SkipRecipe('[M33FW_BOARDS] The selected M33FW_TFM_OPTFLAGS key %s has no match in %s' % (config, m33fwtfmoptflags.keys()))

    # Configure M33FW_BUILDCONF
    m33fwbldconf = (d.getVar('M33FW_BUILDCONF') or "").split()

    if (d.getVar('M33FW_EXTRA_OPTFLAGS') or "").split():
        raise bb.parse.SkipRecipe("You cannot use M33FW_EXTRA_OPTFLAGS as it is internal to M33FW_BOARDS var expansion.")

    if len(m33fwbldconf) > 0:
        m33fwbldconfflags = d.getVarFlags('M33FW_BUILDCONF') or ""
        # The "doc" varflag is special, we don't want to see it here
        m33fwbldconfflags.pop('doc', None)

        for config in m33fwbldconf:
            found = False
            for f, v in m33fwbldconfflags.items():
                if config == f:
                    found = True
                    # Make sure to get var flag properly expanded
                    v = d.getVarFlag('M33FW_BUILDCONF', config)
                    if not v.strip():
                        bb.warn('[M33FW_BUILDCONF] Empty configuration for %s config' % config)
                    items = v.split(',')
                    if items[0] and len(items) > 1:
                        raise bb.parse.SkipRecipe('Only <EXTRA_OPTFLAGS> can be specified! (%s: %s, %s)' % (config,items[0],len(items)))
                    # Set internal vars
                    if items[0]:
                        bb.debug(1, "Appending '%s' to M33FW_EXTRA_OPTFLAGS" % items[0])
                        d.appendVar('M33FW_EXTRA_OPTFLAGS', items[0] + ',')
                    else:
                        d.appendVar('M33FW_EXTRA_OPTFLAGS', '' + ',')
                    break
            if not found:
                raise bb.parse.SkipRecipe('[M33FW_BUILDCONF] The selected M33FW_BUILDCONF key %s has no match in %s' % (config, m33fwbldconfflags.keys()))
    else:
        bb.debug(1, "Init M33FW_EXTRA_OPTFLAGS to empty value")
        d.setVar('M33FW_EXTRA_OPTFLAGS', '')
}

archiver_create_m33fwtool_wrapper_for_sdk() {
    # Create the M33FW_TOOL_WRAPPER script to use on sdk side
    mkdir -p ${ARCHIVER_OUTDIR}
    cat << EOF > ${ARCHIVER_OUTDIR}/${M33FW_TOOL_WRAPPER}
#!/bin/bash -
function bbfatal() { echo "\$*" ; exit 1 ; }

# Set default M33FW config
M33FW_CONFIG="\${M33FW_CONFIG:-${M33FW_CONFIG}}"

M33FW_DEVICETREE="\${M33FW_DEVICETREE:-}"
M33FW_BOOT_SUFFIX_M33="\${M33FW_BOOT_SUFFIX_M33:-}"
M33FW_BOOT_SUFFIX_A35="\${M33FW_BOOT_SUFFIX_A35:-}"
# Set default supported configuration for devicetree and bl32 configuration
declare -A M33FW_DEVICETREE_ARRAY
declare -A M33FW_BOOT_SUFFIX_M33_ARRAY
declare -A M33FW_BOOT_SUFFIX_A35_ARRAY
EOF
    unset i
    for config in ${M33FW_CONFIG}; do
        i=$(expr $i + 1)
        cat << EOF >> ${ARCHIVER_OUTDIR}/${M33FW_TOOL_WRAPPER}
M33FW_DEVICETREE_ARRAY[${config}]="$(echo ${M33FW_DEVICETREE} | cut -d',' -f${i})"
M33FW_BOOT_SUFFIX_M33_ARRAY[${config}]="$(echo ${M33FW_BOOT_SUFFIX_M33} | cut -d',' -f${i})"
M33FW_BOOT_SUFFIX_A35_ARRAY[${config}]="$(echo ${M33FW_BOOT_SUFFIX_A35} | cut -d',' -f${i})"
EOF
    done
    cat << EOF >> ${ARCHIVER_OUTDIR}/${M33FW_TOOL_WRAPPER}
# Set default M33 firmware board config
M33FW_BOARDS="\${M33FW_BOARDS:-${M33FW_BOARDS}}"

M33FW_DEFAULT_FIRMWARE="\${M33FW_DEFAULT_FIRMWARE:-}"
# Set default supported configuration for default firmware configuration
declare -A M33FW_DEFAULT_FIRMWARE_ARRAY
EOF
    unset i
    for board in ${M33FW_BOARDS}; do
        i=$(expr $i + 1)
        cat << EOF >> ${ARCHIVER_OUTDIR}/${M33FW_TOOL_WRAPPER}
M33FW_DEFAULT_FIRMWARE_ARRAY[${board}]="$(echo ${M33FW_DEFAULT_FIRMWARE} | cut -d',' -f${i})"
EOF
    done
    cat << EOF >> ${ARCHIVER_OUTDIR}/${M33FW_TOOL_WRAPPER}

# Make sure about M33FW_CONFIG value
if [ -z "\$M33FW_CONFIG" ]; then
    bbfatal "Wrong configuration 'M33FW_CONFIG' is empty."
else
    # Check that configuration match any of the supported ones
    for config in \$M33FW_CONFIG; do
        CONFIG_FOUND=NO
        for m33fw_config in ${M33FW_CONFIG}; do
            [ "\${config}" = "\${m33fw_config}" ] && { CONFIG_FOUND="YES" ; break; }
        done
        [ "\${CONFIG_FOUND}" = "NO" ] && bbfatal "Wrong 'M33FW_CONFIG' configuration : \${config} is not one of the supported one (${M33FW_CONFIG})"
    done
fi
# Manage M33FW_DEVICETREE default init
if [ -z "\$M33FW_DEVICETREE" ]; then
    # Assigned default supported value
    for config in \$M33FW_CONFIG; do
        M33FW_DEVICETREE+="\${M33FW_DEVICETREE_ARRAY[\${config}]},"
    done
fi
# Manage M33FW_BOOT_SUFFIX_M33 default init
if [ -z "\$M33FW_BOOT_SUFFIX_M33" ]; then
    # Assigned default supported value
    for config in \$M33FW_CONFIG; do
        M33FW_BOOT_SUFFIX_M33+="\${M33FW_BOOT_SUFFIX_M33_ARRAY[\${config}]},"
    done
fi
# Manage M33FW_BOOT_SUFFIX_A35 default init
if [ -z "\$M33FW_BOOT_SUFFIX_A35" ]; then
    # Assigned default supported value
    for config in \$M33FW_CONFIG; do
        M33FW_BOOT_SUFFIX_A35+="\${M33FW_BOOT_SUFFIX_A35_ARRAY[\${config}]},"
    done
fi
# Manage M33FW_DEFAULT_FIRMWARE default init
if [ -z "\$M33FW_DEFAULT_FIRMWARE" ]; then
    # Assigned default supported value
    for board in \$M33FW_BOARDS; do
        M33FW_DEFAULT_FIRMWARE+="\${M33FW_DEFAULT_FIRMWARE_ARRAY[\${board}]},"
    done
fi

# Configure default folder path for binaries to package
M33FW_DEPLOYDIR_ROOT="\${M33FW_DEPLOYDIR_ROOT:-}"
if [ -z "\${M33FW_DEPLOYDIR_ROOT}" ] ; then
    echo "--------------------------------------------------------"
    echo "M33FW: STOP generation of m33fw"
    echo "because all binaries mandatory to generate M33FW must be provided."
    echo "Please verify that M33FW_DEPLOYDIR_ROOT variable is correctly populated and contains the binaries requested by m33fw generation."
    echo "--------------------------------------------------------"
    exit 1
fi

M33FW_DEPLOYDIR_M33FW="\${M33FW_DEPLOYDIR_M33FW:-\$M33FW_DEPLOYDIR_ROOT${M33FW_DIR_M33FW}}"
M33FW_DEPLOYDIR_TFM="\${M33FW_DEPLOYDIR_TFM:-\$M33FW_DEPLOYDIR_ROOT${M33FW_DIR_TFM}}"
M33FW_DEPLOYDIR_FWDDR="\${M33FW_DEPLOYDIR_FWDDR:-\$M33FW_DEPLOYDIR_ROOT${M33FW_DIR_TFM}${M33FW_DIR_FWDDR}}"
M33FW_DEPLOYDIR_CUBE="\${M33FW_DEPLOYDIR_CUBE:-\$M33FW_DEPLOYDIR_ROOT${M33FW_DIR_CUBE}}"
M33FW_WRAPPER="create_st_m33fw_binary.sh"

M33FW_TYPE="\${M33FW_TYPE:-${M33FW_TYPE}}"
M33FW_SIGN_SUFFIX="\${M33FW_SIGN_SUFFIX:-${M33FW_SIGN_SUFFIX}}"
M33FW_SUFFIX="\${M33FW_SUFFIX:-${M33FW_SUFFIX}}"

echo ""
echo "${M33FW_TOOL_WRAPPER} config:"
for config in \$M33FW_CONFIG; do
    i=\$(expr \$i + 1)
    dt_config=\$(echo \$M33FW_DEVICETREE | cut -d',' -f\$i)
    suffix_bootm33=\$(echo \$M33FW_BOOT_SUFFIX_M33 | cut -d',' -f\$i)
    suffix_boota35=\$(echo \$M33FW_BOOT_SUFFIX_A35 | cut -d',' -f\$i)
    echo "  \${config}:" ; \\
    echo "    devicetree config: \${dt_config}"
    echo "    bootm33 suffix   : \${suffix_bootm33}"
    echo "    boota35 suffix   : \${suffix_boota35}"
done
echo ""
echo "Output folders:"
echo "  M33FW_DEPLOYDIR_ROOT : \$M33FW_DEPLOYDIR_ROOT"
echo "  M33FW_DEPLOYDIR_M33FW: \$M33FW_DEPLOYDIR_M33FW"
echo "  M33FW_DEPLOYDIR_CUBE : \$M33FW_DEPLOYDIR_CUBE"
echo "  M33FW_DEPLOYDIR_TFM  : \$M33FW_DEPLOYDIR_TFM"
echo "  M33FW_DEPLOYDIR_FWDDR: \$M33FW_DEPLOYDIR_FWDDR"
echo ""
echo "Default namming"
echo "  M33FW_TYPE       : \$M33FW_TYPE"
echo "  M33FW_SIGN_SUFFIX: \$M33FW_SIGN_SUFFIX"
echo "  M33FW_SUFFIX     : \$M33FW_SUFFIX"
echo ""
unset i
for config in \$M33FW_CONFIG; do
    i=\$(expr \$i + 1)
    dt_config=\$(echo \$M33FW_DEVICETREE | cut -d',' -f\$i)
    suffix_bootm33=\$(echo \$M33FW_BOOT_SUFFIX_M33 | cut -d',' -f\$i)
    [ -z "\${suffix_bootm33}" ] || suffix_bootm33="-\${suffix_bootm33}"
    suffix_boota35=\$(echo \$M33FW_BOOT_SUFFIX_A35 | cut -d',' -f\$i)
    [ -z "\${suffix_boota35}" ] || suffix_boota35="-\${suffix_boota35}"
    for dt in \${dt_config}; do
        # Init soc suffix
        soc_suffix=""
        if [ -n "${STM32MP_SOC_NAME}" ]; then
            for soc in ${STM32MP_SOC_NAME}; do
                [ "\$(echo \${dt} | grep -c \${soc})" -eq 1 ] && soc_suffix="\${soc}"
            done
        fi

        echo "\$M33FW_WRAPPER \\
                --ddrfw \\
                --devicetree \${dt} \\
                --bootsuffix-m33 \${suffix_bootm33} \\
                --output \$M33FW_DEPLOYDIR_M33FW"
        \$M33FW_WRAPPER \\
                --ddrfw \\
                --devicetree \${dt} \\
                --bootsuffix-m33 \${suffix_bootm33} \\
                --output \$M33FW_DEPLOYDIR_M33FW

        echo "\$M33FW_WRAPPER \\
                --m33fw \\
                --devicetree \${dt} \\
                --bootsuffix-m33 \${suffix_bootm33} \\
                --bootsuffix-a35 \${suffix_boota35} \\
                --output \$M33FW_DEPLOYDIR_M33FW"
        \$M33FW_WRAPPER \\
                --m33fw \\
                --devicetree \${dt} \\
                --bootsuffix-m33 \${suffix_bootm33} \\
                --bootsuffix-a35 \${suffix_boota35} \\
                --output \$M33FW_DEPLOYDIR_M33FW

        # Add default firmware symlink
        m33fw_ns_prefix=""
        if [ "\$(echo \${M33FW_DEFAULT_FIRMWARE//,/ } | wc -w)" -eq 1 ]; then
            m33fw_ns_prefix="\$M33FW_DEFAULT_FIRMWARE"
        else
            # Init default firmware from list
            unset l
            for board in \$M33FW_BOARDS; do
                l=\$(expr \$l + 1)
                echo \${dt} | grep -q \${board} || continue
                # Get default firmware
                m33fw_ns_prefix=\$(echo \$M33FW_DEFAULT_FIRMWARE | cut -d',' -f\$l)
            done
        fi
        if [ -e "\$M33FW_DEPLOYDIR_M33FW/${M33FW_S_PREFIX}-\$m33fw_ns_prefix-\${dt}\${suffix_bootm33}\${suffix_boota35}\$M33FW_TYPE\$M33FW_SIGN_SUFFIX.\$M33FW_SUFFIX" ]; then
            cd \$M33FW_DEPLOYDIR_M33FW
            ln -sf ${M33FW_S_PREFIX}-\$m33fw_ns_prefix-\${dt}\${suffix_bootm33}\${suffix_boota35}\$M33FW_TYPE\$M33FW_SIGN_SUFFIX.\$M33FW_SUFFIX \\
                        ${M33FW_PREFIX}-\${dt}\${suffix_bootm33}\${suffix_boota35}\$M33FW_TYPE\$M33FW_SIGN_SUFFIX.\$M33FW_SUFFIX
        fi
    done
done
EOF

    chmod 755 ${ARCHIVER_OUTDIR}/${M33FW_TOOL_WRAPPER}
}
do_ar_original[prefuncs] += "archiver_create_m33fwtool_wrapper_for_sdk"
