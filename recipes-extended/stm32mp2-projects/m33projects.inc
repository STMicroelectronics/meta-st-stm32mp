FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

inherit features_check
REQUIRED_MACHINE_FEATURES = "m33copro"

COMPATIBLE_MACHINE = "(stm32mp2common)"

PACKAGE_ARCH = "${MACHINE_ARCH}"

B = "${S}"

DEPENDS += "gcc-arm-none-eabi-native cmake-native"
DEPENDS += "${@oe.utils.ifelse(d.getVar('M33PROJECT_TF_M_TRUSTZONE') == '1', 'tf-m-stm32mp','')}"

inherit m33fw-utils-stm32mp

# Default service for systemd
inherit systemd update-rc.d python3native deploy
SRC_URI:append = " file://st-m33firmware-load-default.sh"
SRC_URI:append = " file://st-m33firmware-load.service"
SRC_URI:append = " file://shutdown-stm32mp2-m33.sh"
# Temporary
SRC_URI:append = " file://fw_cortex_m33.sh"

# Init M33 board service install
M33_BOARDS_SERVICE ?= "1"
# list of boards/devitree on which the service must be started
M33_BOARDS_START_AT_STARTUP ?= ""

# Init default installation path
M33_FOLDER ?= "Cube-M33-examples"
M33_INSTALLDIR ?= "${prefix}/local"
M33_OUTPUT = "${M33_INSTALLDIR}/${M33_FOLDER}"

# M33 sign settings
M33_SBOOTADDR  ??= "0x80000000"
M33_NSBOOTADDR ??= "0x80100000"

M33_FW_TYPE ??= "_CM33_NonSecure"

do_compile() {
    unset k
    for board in ${M33FW_BOARDS}; do
        k=$(expr $k + 1)
        # Init project var name and path list
        m33fw_project_name=$(echo ${M33FW_PROJECT_NAME} | cut -d',' -f${k})
        m33fw_project_path=$(echo ${M33FW_PROJECT_PATH} | cut -d',' -f${k})
        # Init loop count
        l=1
        while [ -n "$(echo ${m33fw_project_path} | cut -d':' -f${l})" ]; do
            # Set project name and path
            prj_name=$(echo ${m33fw_project_name} | cut -d':' -f${l})
            prj_path=$(echo ${m33fw_project_path} | cut -d':' -f${l})
            # Update for next loop
            l=$(expr $l + 1)

            bbnote "board: ${board}"
            bbnote "project name: ${prj_name}"
            bbnote "project path: ${prj_path}"

            # Init project source dir
            if [ -e "${STAGING_EXTDT_DIR}/${prj_path}" ]; then
                bbnote "Use external m33 project"
                m33prj_srcdir="${STAGING_EXTDT_DIR}/${prj_path}"
            elif [ -e "${S}/${prj_path}" ]; then
                bbnote "Use internal m33 project"
                m33prj_srcdir="${S}/${prj_path}"
            else
                bbnote "Project ${prj_name} not found for ${prj_path} path: skip action"
                continue
            fi

            [ -e "${m33prj_srcdir}/CMakeLists.txt" ] || { bbnote "Missing CMakeLists.txt file for project path" ; continue ; }
            [ "$(basename ${m33prj_srcdir})" != "bin" ] || { bbnote "Project path is a 'bin' one: nothing to compile" ; continue ; }

            # Init build dir
            builddir="${m33prj_srcdir}/build"
            rm -rf "${builddir}"
            # Generate Makefile
            export PATH=${STAGING_DATADIR_NATIVE}/gcc-arm-none-eabi/bin:${STAGING_BINDIR_NATIVE}:$PATH
            ${STAGING_BINDIR_NATIVE}/cmake -G"Unix Makefiles" -S "${m33prj_srcdir}" -B "${builddir}"
            # Build
            cd ${builddir}
            ${STAGING_BINDIR_NATIVE}/make all
        done
    done
}

do_install() {
    # Install M33 firmwares, scripts and README in userfs:
    # <userfs>/Cube-M33-examples/${project} (ie STM32MP257F-EV1/Applications/OpenAMP/OpenAMP_TTY_echo)
    #                        |-- fw_cortex_m33.sh
    #                        |-- README
    #                        |--lib
    #                             |--firmware
    #                                  |-- ELF file for impacted board (ie OpenAMP_TTY_echo_CM33_NonSecure.elf)
    unset k
    for board in ${M33FW_BOARDS}; do
        k=$(expr $k + 1)
        # Init default m33 firmware project name
        m33fw_default_proj=$(echo ${M33FW_DEFAULT_FIRMWARE} | cut -d',' -f${k})
        # Init project var name and path list
        m33fw_project_name=$(echo ${M33FW_PROJECT_NAME} | cut -d',' -f${k})
        m33fw_project_path=$(echo ${M33FW_PROJECT_PATH} | cut -d',' -f${k})
        # Init path for first loop
        prj_path=$(echo ${m33fw_project_path} | cut -d':' -f1)
        l=1
        while [ -n "$(echo ${m33fw_project_path} | cut -d':' -f${l})" ]; do
            prj_name=$(echo ${m33fw_project_name} | cut -d':' -f${l})
            prj_path=$(echo ${m33fw_project_path} | cut -d':' -f${l})
            # Update for next loop
            l=$(expr $l + 1)

            bbnote "board: ${board}"
            bbnote "project name: ${prj_name}"
            bbnote "project path: ${prj_path}"

            # Remove '^Projects/' and '/bin$' from path (if any)
            INSTALL_PROJ=$(echo ${prj_path} | sed 's,^Projects/,,;s,/bin$,,')
            # Init project source dir
            if [ -e "${STAGING_EXTDT_DIR}/${prj_path}" ] && [ -n "${prj_path}" ]; then
                bbnote "Use external m33 project"
                m33prj_srcdir="${STAGING_EXTDT_DIR}/${prj_path}"
            elif [ -e "${S}/${prj_path}" ] && [ -n "${prj_path}" ]; then
                bbnote "Use internal m33 project"
                m33prj_srcdir="${S}/${prj_path}"
            else
                bbnote "Project ${prj_name} not found for ${prj_path} path: skip action"
                continue
            fi
            # Init output dir
            if [ "$(basename ${prj_path})" != "bin" ] ; then
                builddir="${m33prj_srcdir}/build"
            else
                builddir="${m33prj_srcdir}"
            fi

            # Install M33 firmwares
            install -d ${D}${M33_OUTPUT}/${INSTALL_PROJ}/lib/firmware/
            no_elf_found="true"
            if [ -e "${builddir}/${prj_name}${M33_FW_TYPE}.elf" ]; then
                no_elf_found="false"
                install -m 0755 ${builddir}/${prj_name}${M33_FW_TYPE}.elf ${D}${M33_OUTPUT}/${INSTALL_PROJ}/lib/firmware/
            fi
            if [ -e "${builddir}/${prj_name}${M33_FW_TYPE}_stripped.elf" ]; then
                no_elf_found="false"
                install -m 0755 ${builddir}/${prj_name}${M33_FW_TYPE}_stripped.elf ${D}${M33_OUTPUT}/${INSTALL_PROJ}/lib/firmware/
            fi
            if ${no_elf_found}; then
                bbfatal "[${board}] No ${prj_name}${M33_FW_TYPE} elf file(s) to install from ${builddir}/"
            fi

            # Install sh and README files if any for each example
            if [ -e ${builddir}/fw_cortex_m33.sh ]; then
                install -m 0755 ${builddir}/fw_cortex_m33.sh ${D}${M33_OUTPUT}/${INSTALL_PROJ}
            fi
            if [ -e ${m33prj_srcdir}/Remoteproc/README ]; then
                install -m 0755 ${m33prj_srcdir}/Remoteproc/README ${D}${M33_OUTPUT}/${INSTALL_PROJ}
            fi
            #FIXME temporary: add firmware to permit to load the project at runtime
            install -m 0755 ${UNPACKDIR}/fw_cortex_m33.sh ${D}${M33_OUTPUT}/${INSTALL_PROJ}/

            # Configure default copro firmware to launch
            if [ "${m33fw_default_proj}" = "${prj_name}" ]; then
                for sb in ${M33_BOARDS_START_AT_STARTUP}; do
                    if $(echo ${sb} | grep -q ${board}) ; then
                        echo "${INSTALL_PROJ}" > ${D}${M33_OUTPUT}/${INSTALL_PROJ}/default.${sb}
                    fi
                done
            fi
        done
    done
    if [ "${M33_BOARDS_SERVICE}" -eq 1 ]; then
        # Install sh script to load/execute M33FW_DEFAULT_FIRMWARE @boot up
        if [ -n "${M33FW_DEFAULT_FIRMWARE}" ]; then
            # Install systemd service
            install -d ${D}${sysconfdir}/init.d/
            install -d ${D}${base_sbindir}/
            install -m 0755 ${UNPACKDIR}/st-m33firmware-load-default.sh ${D}${sysconfdir}/init.d/st-m33firmware-load-default.sh
            install -m 0755 ${UNPACKDIR}/st-m33firmware-load-default.sh ${D}${base_sbindir}/st-m33firmware-load-default.sh
            install -m 0755 ${UNPACKDIR}/shutdown-stm32mp2-m33.sh ${D}${base_sbindir}/shutdown-stm32mp2-m33.sh

            sed -i -e "s:##BOARDS_LIST##:${M33_BOARDS_START_AT_STARTUP}:g" \
            ${D}${sysconfdir}/init.d/st-m33firmware-load-default.sh
            sed -i -e "s:##BOARDS_LIST##:${M33_BOARDS_START_AT_STARTUP}:g" \
            ${D}${base_sbindir}/st-m33firmware-load-default.sh

            sed -i -e "s:@userfs_mount_point@:${M33_OUTPUT}:g" \
            ${D}${sysconfdir}/init.d/st-m33firmware-load-default.sh
            sed -i -e "s:@userfs_mount_point@:${M33_OUTPUT}:g" \
            ${D}${base_sbindir}/st-m33firmware-load-default.sh
        fi

        # Install systemd service for all machines configurations
        if ${@bb.utils.contains('DISTRO_FEATURES','systemd','true','false',d)}; then
            install -d ${D}${systemd_unitdir}/system
            install -d ${D}${systemd_unitdir}/system-shutdown
            install -m 644 ${UNPACKDIR}/st-m33firmware-load.service ${D}/${systemd_unitdir}/system
            install -m 755 ${UNPACKDIR}/shutdown-stm32mp2-m33.sh ${D}/${systemd_unitdir}/system-shutdown
        fi
    fi
}
do_install[postfuncs] += "sign_copro"
# Sign copro firmware
sign_copro() {
    for board in ${M33FW_BOARDS}; do
        bbnote "[${board}] Run sign/encrypt for M33 firmwares"
        soc_conf=""
        for soc in ${STM32MP_SOC_NAME}; do
            [ "$(echo ${board} | grep -c ${soc})" -eq 1 ] && { soc_conf="${soc}"; break ; }
        done
        [ -n "${soc_conf}" ] || continue

        # Set sub path for Non Secure firmware search
        if [ "$(echo "${M33FW_BOARDS}" | wc -w)" -eq 1 ]; then
            ns_subpath=""
        else
            ns_subpath="$(echo "${board}" | tr '[:lower:]' '[:upper:]')"
        fi
        bbnote "[${board}] Check for non secure firmware under ${M33_OUTPUT}/${ns_subpath}"
        for ns_elf in $(find "${D}${M33_OUTPUT}/${ns_subpath}" -name "*.elf"); do
            bbnote "[${board}] Non Secure firmware: $(basename "${ns_elf}")"
            # Get NS elf radix name
            ns_name="$(echo "${ns_elf}" | sed 's#\.elf$##')"
            # Get isr_vectors from elf file
            elf_NSBOOTADDR="$(${READELF} -S ${ns_elf} | grep isr_vectors | awk '{print($5)}')"
            (
                export NSBOOTADDR=0x${elf_NSBOOTADDR}
                export SBOOTADDR=${M33_SBOOTADDR}
                sign_copro_fw_m33   "${soc_conf}" \
                                    "${ns_name}" \
                                    "${ns_elf}"
            )

            if [ "${M33PROJECT_TF_M_TRUSTZONE}" -eq 1 ]; then
                if [ "$(echo "${M33FW_CONFIG}" | wc -w)" -gt 1 ]; then
                    bbwarn "[${board}][NonSecure+Secure] Skip sign process: multiple M33FW_CONFIG set"
                    continue
                fi
                # Init devicetree list for single M33FW_CONFIG
                dt_config=$(echo ${M33FW_DEVICETREE} | cut -d',' -f1)
                bbnote "[${board}] Use M33FW devicetree list: ${dt_config}"
                for dt in ${dt_config}; do
                    # Select the right device tree if needed
                    if [ "$(echo "${dt_config}" | wc -w)" -gt 1 ]; then
                        [ -n "$(echo ${dt} | grep ${board})" ] || continue
                    fi
                    # Get S elf
                    [ -e "${RECIPE_SYSROOT}${M33FW_SYSROOT_S}" ] || { bbwarn "[${board}][NonSecure+Secure] Missing secure firmware folder"; continue; }
                    s_elf=$(find "${RECIPE_SYSROOT}${M33FW_SYSROOT_S}" -name "${M33FW_S_PREFIX}-${dt}${M33FW_S_TYPE}.elf")
                    bbnote "[${board}] Secure firmware: $(basename "${s_elf}")"
                    [ -n "${s_elf}" ] || continue
                    (
                        export NSBOOTADDR=0x${elf_NSBOOTADDR}
                        export SBOOTADDR=${M33_SBOOTADDR}
                        sign_copro_fw_m33   "${soc_conf}" \
                                            "${ns_name}-${dt}" \
                                            "${ns_elf}" \
                                            "${s_elf}"
                    )
                done
            fi
        done
    done
}

do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}/m33-projects"
do_deploy() {
    for board in ${M33FW_BOARDS}; do
        bbnote "[${board}] Run sign/encrypt for M33 firmwares"
        soc_conf=""
        for soc in ${STM32MP_SOC_NAME}; do
            [ "$(echo ${board} | grep -c ${soc})" -eq 1 ] && { soc_conf="${soc}"; break ; }
        done
        [ -n "${soc_conf}" ] || continue

        if [ "$(echo "${M33FW_BOARDS}" | wc -w)" -eq 1 ]; then
            board_name=""
        else
            board_name=$(echo "${board}" | tr '[:lower:]' '[:upper:]')
        fi
        for ns_signed_file in $(find "${WORKDIR}/cmd_dump/${M33_OUTPUT}/${board_name}/" -name "*.txt"); do
            # deploy only dump of signed command
            dir=$(dirname ${ns_signed_file} | sed "s|${WORKDIR}/cmd_dump/${M33_OUTPUT}||")
            bbnote "create <$dir> "
            install -d ${DEPLOYDIR}/${dir}
            cp ${ns_signed_file} ${DEPLOYDIR}/${dir}/
        done
    done
}
addtask deploy before do_package after do_install
do_clean[cleandirs] += " ${WORKDIR}/cmd_dump"

# -----------------------------------------------------------
# specific for service: start copro m33 firwmare at boot time
SYSTEMD_PACKAGES += " m33projects-stm32mp2 "
SYSTEMD_SERVICE:${PN} = "st-m33firmware-load.service"
SYSTEMD_AUTO_ENABLE:${PN} = "enable"

INITSCRIPT_NAME = "st-m33firmware-load-default.sh"
INITSCRIPT_PARAMS = "stop 22 5 3 ."
# -----------------------------------------------------------

INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_SYSROOT_STRIP = "1"

FILES:${PN} += "${nonarch_base_libdir}/firmware ${sysconfdir}/init.d ${systemd_unitdir}/system ${systemd_unitdir}/system-shutdown"
# Configure package split
FILES:${PN} += "${M33_OUTPUT}"
RDEPENDS:${PN} += "busybox"

# Avoid QA issue because binaries are for an Arm architecture but the platform is an AArch64
INSANE_SKIP = "arch"
INSANE_SKIP:${PN} = "already-stripped buildpaths"
INSANE_SKIP:${PN}-dbg = "buildpaths"
