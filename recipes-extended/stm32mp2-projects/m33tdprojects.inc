FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

inherit features_check
REQUIRED_MACHINE_FEATURES = "m33td"

COMPATIBLE_MACHINE = "(stm32mp2common)"

PACKAGE_ARCH = "${MACHINE_ARCH}"

B = "${S}"

DEPENDS += "gcc-arm-none-eabi-native cmake-native ninja-native"

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
DEPENDS += "virtual/trusted-firmware-m"

inherit deploy python3native
inherit m33fw-utils-stm32mp

# python3-cryptography needs the legacy provider, so set OPENSSL_MODULES to the
# right path until this is relocated automatically.
export OPENSSL_MODULES = "${STAGING_LIBDIR_NATIVE}/ossl-modules"

M33TD_ELF_SUFFIX ?= "elf"
M33TD_FW_SUFFIX ?= "bin"
M33TD_FW_TYPE ?= "_CM33_NonSecure"

M33TD_CMAKE_TOOLCHAIN ?= ""

do_compile() {
    unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS

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
            # Init TF-M build path
            tfm_build_path="${RECIPE_SYSROOT}${datadir}/tf-m/${config}-${dt}"
            [ -e "${tfm_build_path}" ] || continue

            # Initialize project list to build and build config list
            m33fw_project_name=""
            m33fw_project_path=""
            m33fw_bldconf_list=""
            if [ "$(echo "${M33FW_BOARDS}" | wc -w)" -eq 1 ]; then
                m33fw_project_name=$(echo ${M33FW_PROJECT_NAME} | cut -d',' -f1)
                m33fw_project_path=$(echo ${M33FW_PROJECT_PATH} | cut -d',' -f1)
                m33fw_bldconf_list=$(echo ${M33FW_PROJECT_BUILDCONF} | cut -d',' -f1)
            else
                unset k
                for board in ${M33FW_BOARDS}; do
                    k=$(expr $k + 1)
                    # Filter for devicetree that matches board configure
                    echo ${dt} | grep -q "${board}" || continue

                    m33fw_project_name=$(echo ${M33FW_PROJECT_NAME} | cut -d',' -f${k})
                    m33fw_project_path=$(echo ${M33FW_PROJECT_PATH} | cut -d',' -f${k})
                    m33fw_bldconf_list=$(echo ${M33FW_PROJECT_BUILDCONF} | cut -d',' -f${k})
                    break
                done
            fi
            # Init project name, path and build config for first loop
            prj_name=$(echo ${m33fw_project_name} | cut -d':' -f1)
            prj_path=$(echo ${m33fw_project_path} | cut -d':' -f1)
            bld_conf=$(echo ${m33fw_bldconf_list} | cut -d':' -f1)
            l=1
            while [ -n "${prj_path}" ]; do
                bbnote "config    : ${config}"
                bbnote "board     : ${board}"
                bbnote "devicetree: ${dt}"
                bbnote "project name: ${prj_name}"
                bbnote "project path: ${prj_path}"
                bbnote "project conf: ${bld_conf}"

                extra_opt=""
                unset n
                for buildconf in ${M33FW_BUILDCONF}; do
                    n=$(expr $n + 1)
                    # Filter for buildconf that matches project build configure
                    [ "${bld_conf}" = "${buildconf}" ] || continue
                    # Initialize extra option to build
                    extra_opt=$(echo ${M33FW_EXTRA_OPTFLAGS} | cut -d',' -f${n})
                    break
                done

                # Init cmake source dir
                m33td_srcdir="${S}/${prj_path}"
                if [ -e "${STAGING_EXTDT_DIR}/${prj_path}" ]; then
                    bbnote "M33TD: use external m33td project"
                    m33td_srcdir="${STAGING_EXTDT_DIR}/${prj_path}"
                fi
                # Init build dir
                m33td_outputdir="${B}/build-${prj_name}-${config}-${dt}-${bld_conf}"
                rm -rf "${m33td_outputdir}"

                export PATH=${STAGING_DATADIR_NATIVE}/gcc-arm-none-eabi/bin:${STAGING_BINDIR_NATIVE}:$PATH
                # Generate Makefile
                bbnote "${STAGING_BINDIR_NATIVE}/cmake -G"Unix Makefiles" -S "${m33td_srcdir}" -B "${m33td_outputdir}" ${M33TD_CMAKE_TOOLCHAIN} -DTFM_BUILD_DIR=${tfm_build_path}/spe ${extra_opt}"
                ${STAGING_BINDIR_NATIVE}/cmake -G"Unix Makefiles" -S "${m33td_srcdir}" -B "${m33td_outputdir}" ${M33TD_CMAKE_TOOLCHAIN} -DTFM_BUILD_DIR=${tfm_build_path}/spe ${extra_opt}
                # Build
                bbnote "${STAGING_BINDIR_NATIVE}/make ${PARALLEL_MAKE} all"
                cd "${m33td_outputdir}"
                ${STAGING_BINDIR_NATIVE}/make ${PARALLEL_MAKE} all

                # Apply namming
                if [ "${bld_conf}" = "${M33FW_PROJECT_DEFAULT_BUILDCONF}" ]; then
                    m33td_deploy_basename="${prj_name}-${dt}${suffix_bootm33}${suffix_boota35}"
                else
                    m33td_deploy_basename="${prj_name}_${bld_conf}-${dt}${suffix_bootm33}${suffix_boota35}"
                fi
                if [ -e "${B}/${m33td_deploy_basename}${M33TD_FW_TYPE}.${M33TD_FW_SUFFIX}" ]; then
                    bbfatal "Wrong config: ${m33td_deploy_basename}${M33TD_FW_TYPE}.${M33TD_FW_SUFFIX} already exists"
                else
                    cp ${m33td_outputdir}/${prj_name}${M33TD_FW_TYPE}.${M33TD_FW_SUFFIX} \
                        ${B}/${m33td_deploy_basename}${M33FW_NS_TYPE}.${M33TD_FW_SUFFIX}
                fi
                if [ -e "${B}/${m33td_deploy_basename}${M33TD_FW_TYPE}.elf" ]; then
                    bbfatal "Wrong config: ${m33td_deploy_basename}${M33TD_FW_TYPE}.elf already exists"
                elif [ -e "${m33td_outputdir}/${prj_name}${M33TD_FW_TYPE}.elf" ]; then
                    cp ${m33td_outputdir}/${prj_name}${M33TD_FW_TYPE}.elf \
                        ${B}/${m33td_deploy_basename}${M33FW_NS_TYPE}.elf
                else
                    bbnote "No ${prj_name}${M33TD_FW_TYPE}.elf file to deploy"
                fi
                if [ -e "${B}/${m33td_deploy_basename}${M33TD_FW_TYPE}_stripped.elf" ]; then
                    bbfatal "Wrong config: ${m33td_deploy_basename}${M33TD_FW_TYPE}_stripped.elf already exists"
                elif [ -e "${m33td_outputdir}/${prj_name}${M33TD_FW_TYPE}_stripped.elf" ]; then
                    cp ${m33td_outputdir}/${prj_name}${M33TD_FW_TYPE}_stripped.elf \
                        ${B}/${m33td_deploy_basename}${M33FW_NS_TYPE}_stripped.elf
                else
                    bbnote "No ${m33td_deploy_basename}${M33FW_NS_TYPE}_stripped.elf file to deploy"
                fi

                # Update for next loop
                l=$(expr $l + 1)
                prj_name=$(echo ${m33fw_project_name} | cut -d':' -f${l})
                prj_path=$(echo ${m33fw_project_path} | cut -d':' -f${l})
                bld_conf=$(echo ${m33fw_bldconf_list} | cut -d':' -f${l})
            done
        done
    done
}

do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}${M33FW_DIR_CUBE}"
do_deploy() {
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
            m33td_fw="$(find "${B}" -name "*-${dt}${suffix_bootm33}${suffix_boota35}${M33FW_NS_TYPE}.${M33TD_FW_SUFFIX}" -maxdepth 1)"
            [ -z "${m33td_fw}" ] || install -d ${DEPLOYDIR}
            for fw in ${m33td_fw}; do
                install -m 0644 ${fw} ${DEPLOYDIR}
            done
            m33td_elf="$(find "${B}" -name "*-${dt}${suffix_bootm33}${suffix_boota35}${M33FW_NS_TYPE}.elf" -maxdepth 1)"
            [ -z "${m33td_elf}" ] || install -d ${DEPLOYDIR}/debug
            for elf in ${m33td_elf}; do
                install -m 0644 ${elf} ${DEPLOYDIR}/debug
            done
            m33td_elf_stripped="$(find "${B}" -name "*-${dt}${suffix_bootm33}${suffix_boota35}${M33FW_NS_TYPE}_stripped.elf" -maxdepth 1)"
            [ -z "${m33td_elf_stripped}" ] || install -d ${DEPLOYDIR}/debug
            for elf in ${m33td_elf_stripped}; do
                install -m 0644 ${elf} ${DEPLOYDIR}/debug
            done
        done
    done
}
addtask deploy before do_build after do_compile

m33td_sysroot_populate() {
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
            m33td_fw="$(find "${B}" -name "*-${dt}${suffix_bootm33}${suffix_boota35}${M33FW_NS_TYPE}.${M33TD_FW_SUFFIX}" -maxdepth 1)"
            [ -z "${m33td_fw}" ] || install -d ${SYSROOT_DESTDIR}${M33FW_SYSROOT_NS}
            for fw in ${m33td_fw}; do
                install -m 0644 ${fw} ${SYSROOT_DESTDIR}${M33FW_SYSROOT_NS}
            done
        done
    done
}
SYSROOT_PREPROCESS_FUNCS =+ "m33td_sysroot_populate"
SYSROOT_DIRS += "${M33FW_SYSROOT_NS}"
