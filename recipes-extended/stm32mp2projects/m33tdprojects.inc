COMPATIBLE_MACHINE = "(stm32mp2common)"

PACKAGE_ARCH = "${MACHINE_ARCH}"

B = "${S}"

DEPENDS += "gcc-arm-none-eabi-native cmake-native ninja-native"
DEPENDS += " ${@oe.utils.ifelse(d.getVar('MULTILIBS'), 'lib64-virtual-optee-os','virtual-optee-os')} "
DEPENDS += "virtual/trusted-firmware-m"

DEPENDS += "python3-cbor2-native \
            python3-click-native \
            python3-cryptography-native \
            python3-pyasn1-native \
            python3-imgtool-native \
            python3-jinja2-native \
            python3-pyyaml-native \
            python3-pyelftools-native \
            python3-pycryptodomex-native \
"

# python3-cryptography needs the legacy provider, so set OPENSSL_MODULES to the
# right path until this is relocated automatically.
export OPENSSL_MODULES = "${STAGING_LIBDIR_NATIVE}/ossl-modules"

STAGING_TFM_BUILDDIR ?= "${TMPDIR}/work-shared/${MACHINE}/tfm-build-artifacts"

M33TD_FIWMWARE_TYPE_SIGNED          ?= "_CM33_NonSecure_tfm_s_ns_signed"
M33TD_FIWMWARE_DEPLOYED_TYPE_SIGNED ?= "_s_ns_signed"
M33TD_FIWMWARE_TYPE          ?= "_CM33_NonSecure_tfm_s_ns"
M33TD_FIWMWARE_TYPE_ELF      ?= "_CM33_NonSecure"
M33TD_FIWMWARE_TYPE_ELF_STRIPPED ?= "_CM33_NonSecure_stripped"
M33TD_FIWMWARE_DEPLOYED_TYPE ?= "_s_ns"
M33TD_FIWMWARE_DEPLOYED_PREFIX ?= "tfm-starterapp"


M33TD_FIWMWARE_SUFFIX ?= "bin"


inherit deploy python3native
# -----------------------------------------------
# Handle M33TD firmware config to set internal var:
#   UBOOT_DEVICETREE
#
M33TD_FIRMWARE_CONFIG ??= ""
M33TD_FIRMWARE_PROJECT ??= ""
python () {
    if bb.utils.contains('MACHINE_FEATURES', 'm33td', True, False, d):
        if (d.getVar('M33TD_PROJECT') or "").split():
            raise bb.parse.SkipRecipe("You cannot use M33TD_PROJECT as it is internal for var expansion.")

        m33td_firmware_config = (d.getVar('M33TD_FIRMWARE_CONFIG') or "").split()

        if len(m33td_firmware_config) > 0:
            m33td_firmware_projectflags = d.getVarFlags('M33TD_FIRMWARE_PROJECT') or ""
            # The "doc" varflag is special, we don't want to see it here
            m33td_firmware_projectflags.pop('doc', None)

            if len(m33td_firmware_projectflags) > 0:
                for config in m33td_firmware_config:
                    found = False
                    for f, v in m33td_firmware_projectflags.items():
                        if config == f:
                            found = True
                            # Make sure to get var flag properly expanded
                            v = d.getVarFlag('M33TD_FIRMWARE_PROJECT', config)
                            if not v.strip():
                                bb.note('[M33TD_FIRMWARE_PROJECT] No project configured for %s config' % config)
                            d.appendVar('M33TD_PROJECT', config+':'+ v + ',')
                            break

                    if not found:
                        raise bb.parse.SkipRecipe("The selected M33TD_FIRMWARE_CONFIG key %s has no match in M33TD_FIRMWARE_PROJECT keys: %s." % (config, m33td_firmware_projectflags.keys()))
}

do_compile[depends] += "virtual/trusted-firmware-m:do_shared_workdir"
do_compile() {
    unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS
    if [ -n "${M33TD_FIRMWARE_CONFIG}" ]; then
        unset i
        for config in ${TF_M_CONFIG}; do
            i=$(expr $i + 1)
            # Initialize plateform list, devicetree list, boot device, dt bl2 suffix and dt s suffix
            pf_config=$(echo ${TF_M_PLATFORM} | cut -d',' -f${i})
            dt_config=$(echo ${TF_M_DEVICETREE} | cut -d',' -f${i})
            boot_dev=$(echo ${TF_M_BOOTDEV} | cut -d',' -f${i})
            for platform in ${pf_config}; do
                pf_base="$(basename "${platform}")"
                for dt in ${dt_config}; do
                    # Filter for devicetree that matches platform
                    echo ${dt} | sed 's/-/_/g' | grep -q "${pf_base}" || continue

                    unset j
                    for m33_fw_config in ${M33TD_FIRMWARE_CONFIG}; do
                        j=$(expr $j + 1)
                        m33td_project=$(echo ${M33TD_PROJECT} | cut -d',' -f${j} | cut -d':' -f2)
                        tmp_m33td_config=$(echo ${M33TD_PROJECT} | cut -d',' -f${j} | cut -d':' -f1)
                        if [ "$tmp_m33td_config" = "$pf_base" ]; then
                            break;
                        fi
                    done

                    tfm_build_path="${STAGING_TFM_BUILDDIR}/${config}-${dt}"
                    cd "${B}/${m33td_project}"

                    rm -rf ${B}/${m33td_project}/build-${config}-${dt}
                    export PATH=${STAGING_DATADIR_NATIVE}/gcc-arm-none-eabi/bin:${STAGING_BINDIR_NATIVE}:$PATH
                    # Hack for python usage
                    sed -i "s|python.exe|python3|g" CMakeLists.txt
                    # generate Makefile
                    echo "${STAGING_BINDIR_NATIVE}/cmake -G"Unix Makefiles" -B ${B}/${m33td_project}/build-${config}-${dt} -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DTFM_BUILD_DIR=$tfm_build_path/spe"
                    ${STAGING_BINDIR_NATIVE}/cmake -G"Unix Makefiles" -B ${B}/${m33td_project}/build-${config}-${dt} -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DTFM_BUILD_DIR=$tfm_build_path/spe
                    # build
                    echo "${STAGING_BINDIR_NATIVE}/make ${PARALLEL_MAKE} all"
                    cd ${B}/${m33td_project}/build-${config}-${dt}; ${STAGING_BINDIR_NATIVE}/make ${PARALLEL_MAKE} all
                done
            done
        done
    fi
}

do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}/arm-trusted-firmware-m-cube"
do_deploy() {
    install -d ${DEPLOYDIR}
    if [ -n "${M33TD_FIRMWARE_CONFIG}" ]; then
        unset i
        for config in ${TF_M_CONFIG}; do
            i=$(expr $i + 1)
            # Initialize plateform list, devicetree list, boot device, dt bl2 suffix and dt s suffix
            pf_config=$(echo ${TF_M_PLATFORM} | cut -d',' -f${i})
            dt_config=$(echo ${TF_M_DEVICETREE} | cut -d',' -f${i})
            boot_dev=$(echo ${TF_M_BOOTDEV} | cut -d',' -f${i})
            suffix_boota35=$(echo ${TF_M_BOOT_SUFFIX_A35} | cut -d',' -f${i})
            [ -z "${suffix_boota35}" ] || suffix_boota35="-${suffix_boota35}"
            suffix_bootm33=$(echo ${TF_M_BOOT_SUFFIX_M33} | cut -d',' -f${i})
            [ -z "${suffix_bootm33}" ] || suffix_bootm33="-${suffix_bootm33}"

            for platform in ${pf_config}; do
                pf_base="$(basename "${platform}")"
                for dt in ${dt_config}; do
                    # Filter for devicetree that matches platform
                    echo ${dt} | sed 's/-/_/g' | grep -q "${pf_base}" || continue

                    unset j
                    for m33_fw_config in ${M33TD_FIRMWARE_CONFIG}; do
                        j=$(expr $j + 1)
                        m33td_project=$(echo ${M33TD_PROJECT} | cut -d',' -f${j} | cut -d':' -f2)
                        tmp_m33td_config=$(echo ${M33TD_PROJECT} | cut -d',' -f${j} | cut -d':' -f1)
                        if [ "$tmp_m33td_config" = "$pf_base" ]; then
                            break;
                        fi
                    done
                    m33td_project_name=$(basename ${m33td_project})
                    output_path=${B}/${m33td_project}/build-${config}-${dt}

                    if [ -e "${output_path}/${m33td_project_name}${M33TD_FIWMWARE_TYPE}.${M33TD_FIWMWARE_SUFFIX}" ]; then
                        install -m 0644 ${output_path}/${m33td_project_name}${M33TD_FIWMWARE_TYPE}.${M33TD_FIWMWARE_SUFFIX} ${DEPLOYDIR}/${M33TD_FIWMWARE_DEPLOYED_PREFIX}-${dt}${suffix_bootm33}${suffix_boota35}${M33TD_FIWMWARE_DEPLOYED_TYPE}.${M33TD_FIWMWARE_SUFFIX}
                    fi
                    if [ -e "${output_path}/${m33td_project_name}${M33TD_FIWMWARE_TYPE_SIGNED}.${M33TD_FIWMWARE_SUFFIX}" ]; then
                        install -m 0644 ${output_path}/${m33td_project_name}${M33TD_FIWMWARE_TYPE_SIGNED}.${M33TD_FIWMWARE_SUFFIX} ${DEPLOYDIR}/${M33TD_FIWMWARE_DEPLOYED_PREFIX}-${dt}${suffix_bootm33}${suffix_boota35}${M33TD_FIWMWARE_DEPLOYED_TYPE_SIGNED}.${M33TD_FIWMWARE_SUFFIX}
                    fi
                    if [ -e "${output_path}/${m33td_project_name}${M33TD_FIWMWARE_TYPE_ELF}.elf" ]; then
                        install -d ${DEPLOYDIR}/debug
                        install -m 0644 ${output_path}/${m33td_project_name}${M33TD_FIWMWARE_TYPE_ELF}.elf ${DEPLOYDIR}/debug/${M33TD_FIWMWARE_DEPLOYED_PREFIX}-${dt}${suffix_bootm33}${suffix_boota35}${M33TD_FIWMWARE_TYPE_ELF}.elf
                    fi
                    if [ -e "${output_path}/${m33td_project_name}${M33TD_FIWMWARE_TYPE_ELF_STRIPPED}.elf" ]; then
                        install -d ${DEPLOYDIR}/debug
                        install -m 0644 ${output_path}/${m33td_project_name}${M33TD_FIWMWARE_TYPE_ELF_STRIPPED}.elf ${DEPLOYDIR}/debug/${M33TD_FIWMWARE_DEPLOYED_PREFIX}-${dt}${suffix_bootm33}${suffix_boota35}${M33TD_FIWMWARE_TYPE_ELF_STRIPPED}.elf
                    fi
                done
            done
        done
    fi
}
addtask deploy before do_build after do_compile

