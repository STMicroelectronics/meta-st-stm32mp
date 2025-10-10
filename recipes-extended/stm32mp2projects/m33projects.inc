COMPATIBLE_MACHINE = "(stm32mp2common)"

PACKAGE_ARCH = "${MACHINE_ARCH}"

B = "${S}"

DEPENDS += "gcc-arm-none-eabi-native cmake-native"
DEPENDS += "${@oe.utils.ifelse(d.getVar('MULTILIBS'), 'lib64-virtual-optee-os','virtual-optee-os')} "
DEPENDS += "copro-sdk-native"
DEPENDS += "${@oe.utils.ifelse(d.getVar('M33PROJECT_TF_M_TRUSTZONE') == '1', 'tf-m-stm32mp','')}"
# for optee script
DEPENDS += "python3-pyelftools-native python3-pycryptodomex-native python3-cryptography-native"
DEPENDS += "openssl-native"

inherit sign-stm32mp

# Default service for systemd
inherit systemd update-rc.d python3native
SRC_URI += "file://st-m33firmware-load-default.sh"
SRC_URI += "file://st-m33firmware-load.service"
SRC_URI += "file://shutdown-stm32mp2-m33.sh"
# Temporary
SRC_URI += "file://fw_cortex_m33.sh"

# Create specific userfs package
M33_PACKAGE_4USERFS ?= "1"
PACKAGES += "${@bb.utils.contains('M33_PACKAGE_4USERFS', '1', '${PN}-userfs', '', d)}"

# Define default board reference for M33
M33_BOARDS ?= ""
# Define default board reference for M0
M0_BOARDS ?= ""

# Init M33 board service install
M33_BOARDS_SERVICE ?= "1"
# list of boards/devitree on which the service must be started
M33_BOARDS_START_AT_STARTUP ?= ""

# Init default copro example to load/execute
DEFAULT_COPRO_FIRMWARE ?= ""

BUILD_CONFIG ?= "Debug"

# Init default installation path
M33_FOLDER ?= "Cube-M33-examples"
M33_INSTALLDIR ?= "${prefix}local"
M33_OUTPUT_4USERFS = "${M33_INSTALLDIR}/${M33_FOLDER}"

TA_DEV_KIT_DIR_aarch32_aarch64 = "${WORKDIR}/lib64-recipe-sysroot${includedir}/optee/${@bb.utils.contains('TUNE_FEATURES', 'aarch64', 'export-user_ta_arm64', 'export-user_ta_arm32', d)}"
export TA_DEV_KIT_DIR = "${@oe.utils.ifelse(d.getVar('MULTILIBS'), '${TA_DEV_KIT_DIR_aarch32_aarch64}','${STAGING_INCDIR}/optee/export-user_ta_arm64')}"
export SBOOTADDR = "0x80000000"
export NSBOOTADDR = "0x80100000"

# M33 key settings
M33_OPTEE_KEYNAME ?= "default.pem"

M33_ELF_NOT_STRIPPED ??= "0"

do_compile() {
    unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS
    for BIN_MACHINE in ${M33_BOARDS}; do
        for project in ${PROJECTS_LIST_M33} ; do
            [ "$(echo ${project} | cut -d'/' -f1)" = "${BIN_MACHINE}" ] || continue
            [ -d "${B}/Projects/${project}" ] || continue
            [ -e "${B}/Projects/${project}/CMakeLists.txt" ] || continue
            [ "$(basename ${project})" != "bin" ] || continue

            cd "${B}/Projects/${project}"
            rm -rf ${B}/Projects/${project}/build
            export PATH=${STAGING_DATADIR_NATIVE}/gcc-arm-none-eabi/bin:${STAGING_BINDIR_NATIVE}:$PATH
            # generate Makefile
            ${STAGING_BINDIR_NATIVE}/cmake -G"Unix Makefiles" -B ${B}/Projects/${project}/build -DCMAKE_POLICY_VERSION_MINIMUM=3.5
            # build
            cd ${B}/Projects/${project}/build; ${STAGING_BINDIR_NATIVE}/make all
        done
    done
}

do_install() {
    export OPENSSL_MODULES=${STAGING_LIBDIR_NATIVE}/ossl-modules/
    # Install M33 firmwares, scripts and README in userfs:
    # <userfs>/Cube-M33-examples/${project} (ie STM32MP257F-EV1/Applications/OpenAMP/OpenAMP_TTY_echo)
    #                        |-- fw_cortex_m33.sh
    #                        |-- README
    #                        |--lib
    #                             |--firmware
    #                                  |-- ELF file for impacted board (ie OpenAMP_TTY_echo_CM33_NonSecure.elf)
    for BIN_MACHINE in ${M33_BOARDS}; do
        for project in ${PROJECTS_LIST_M33} ; do
            [ "$(echo ${project} | cut -d'/' -f1)" = "${BIN_MACHINE}" ] || continue

            if [ -d "${B}/Projects/${project}/build" ]; then
                BIN_NAME=$(basename "${project}")
                PROJ_DIR=${project}/build
                INSTALL_PROJ=${project}
            elif [ -d "${B}/Projects/${project}" ]; then
                BIN_NAME=$(basename "$(dirname "${project}")")
                PROJ_DIR=${project}
                INSTALL_PROJ=$(echo ${project} | sed 's,/bin,,')
            else
                bbfatal "[install] Issue with ${project}"
            fi
            BIN_NAME_NS="${BIN_NAME}_CM33_NonSecure"
            board_lower=$(echo $BIN_MACHINE | tr '[:upper:]' '[:lower:]')

            # Install M33 firmwares
            install -d ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/
            if [ "${M33_ELF_NOT_STRIPPED}" = "1" ]; then
                install -m 0755 ${B}/Projects/${PROJ_DIR}/${BIN_NAME_NS}.elf ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/
            fi
            if [ -e ${B}/Projects/${PROJ_DIR}/${BIN_NAME_NS}_stripped.elf ]; then
                BIN_NAME_NS_ELF="${BIN_NAME_NS}_stripped.elf"
            else
                BIN_NAME_NS_ELF="${BIN_NAME_NS}.elf"
            fi
            install -m 0755 ${B}/Projects/${PROJ_DIR}/${BIN_NAME_NS_ELF} ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/

            # Install sh and README files if any for each example
            if [ -e ${B}/Projects/${PROJ_DIR}/fw_cortex_m33.sh ]; then
                install -m 0755 ${B}/Projects/${PROJ_DIR}/fw_cortex_m33.sh ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}
            fi
            if [ -e ${S}/Projects/${project}/Remoteproc/README ]; then
                install -m 0755 ${S}/Projects/${project}/Remoteproc/README ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}
            fi

            # Get isr_vectors from elf file
            elf_NSBOOTADDR=$(${READELF} -S ${B}/Projects/${PROJ_DIR}/${BIN_NAME_NS}.elf | grep isr_vectors | awk '{print($5)}')
            # Generate ${BIN_NAME_NS}_sign.bin
            m33_use_ecc_key=0
            m33_uc1_radix=sign.bin
            k=0
            for soc in ${STM32MP_SOC_NAME}; do
                k=$(expr $k + 1)
                if [ "$(echo ${board_lower} | grep -c ${soc})" -eq 1 ] ; then
                    m33_use_ecc_key=1
                    m33_priv_sign_key=$(echo ${SIGN_COPRO_ECC_PRIV_KEY_PATH_LIST} | cut -d',' -f${k})
                    m33_info_sign_key=$(echo ${SIGN_COPRO_ECC_INFO_KEY_PATH_LIST} | cut -d',' -f${k})
                    m33_pass_sign_key=$(echo ${SIGN_COPRO_ECC_PASS_KEY_PATH_LIST} | cut -d',' -f${k})
                fi
            done
            if [ "${SIGN_ENABLE}" = "1" ] && [ $m33_use_ecc_key ]; then
                    if [ "${ENCRYPT_COPRO_ENABLE}" = "1" ]; then
                        unset l
                        for soc in ${STM32MP_ENCRYPT_SOC_NAME}; do
                            l=$(expr $l + 1)
                            [ "$(echo ${board_lower} | grep -c ${soc})" -eq 1 ] && m33_encrypt_key=$(echo ${ENCRYPT_COPRO_KEY_PATH_LIST} | cut -d',' -f${l})
                        done

                    # Generate ${BIN_NAME_NS}_sign_enc.bin
                    NSBOOTADDR=0x${elf_NSBOOTADDR} ${STAGING_BINDIR_NATIVE}/st_copro_firmware_signature.sh \
                        --input-nsecure ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/${BIN_NAME_NS_ELF} \
                        --sign-ecc --sign-ecc-priv-key $m33_priv_sign_key --sign-ecc-info-key $m33_info_sign_key --sign-ecc-pass $m33_pass_sign_key \
                        --encrypt-key ${m33_encrypt_key} \
                        --output ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/${BIN_NAME_NS}
                else
                    # sign only with ecc key
                    echo "NSBOOTADDR=0x${elf_NSBOOTADDR} DEFAULT_UC1_RADIX=ecc.stm32 ${STAGING_BINDIR_NATIVE}/st_copro_firmware_signature.sh \
                        --input-nsecure ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/${BIN_NAME_NS_ELF} \
                        --sign-ecc --sign-ecc-priv-key $m33_priv_sign_key --sign-ecc-info-key $m33_info_sign_key --sign-ecc-pass $m33_pass_sign_key \
                        --output ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/${BIN_NAME_NS}"
                    NSBOOTADDR=0x${elf_NSBOOTADDR} DEFAULT_UC1_RADIX=ecc.stm32 ${STAGING_BINDIR_NATIVE}/st_copro_firmware_signature.sh \
                        --input-nsecure ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/${BIN_NAME_NS_ELF} \
                        --sign-ecc --sign-ecc-priv-key $m33_priv_sign_key --sign-ecc-info-key $m33_info_sign_key --sign-ecc-pass $m33_pass_sign_key \
                        --output ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/${BIN_NAME_NS}
                fi
            else
                if [ "${ENCRYPT_COPRO_ENABLE}" = "1" ]; then
                    unset l
                    for soc in ${STM32MP_ENCRYPT_SOC_NAME}; do
                        l=$(expr $l + 1)
                        [ "$(echo ${board_lower} | grep -c ${soc})" -eq 1 ] && m33_encrypt_key=$(echo ${ENCRYPT_COPRO_KEY_PATH_LIST} | cut -d',' -f${l})
                    done
                    # Generate ${BIN_NAME_NS}_sign_enc.bin
                    NSBOOTADDR=0x${elf_NSBOOTADDR} ${STAGING_BINDIR_NATIVE}/st_copro_firmware_signature.sh \
                        --input-nsecure ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/${BIN_NAME_NS_ELF} \
                        --signature-key ${TA_DEV_KIT_DIR}/keys/${M33_OPTEE_KEYNAME} \
                        --encrypt-key ${m33_encrypt_key} \
                        --output ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/${BIN_NAME_NS}
                else
                    # sign with default optee key
                    NSBOOTADDR=0x${elf_NSBOOTADDR} ${STAGING_BINDIR_NATIVE}/st_copro_firmware_signature.sh \
                        --input-nsecure ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/${BIN_NAME_NS_ELF} \
                        --signature-key ${TA_DEV_KIT_DIR}/keys/${M33_OPTEE_KEYNAME} \
                        --output ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/${BIN_NAME_NS}
                fi
            fi

            if [ "${M33PROJECT_TF_M_TRUSTZONE}" -eq 1 ]; then
                if [ "$(echo "${TF_M_CONFIG}" | wc -w)" -gt 1 ]; then
                    bbwarn "Skip '*_tfm_sing.bin' creation: multiple TF_M_CONFIG set"
                    continue
                fi
                tfm_unitary_config=$(echo ${TF_M_CONFIG} | sed "s| ||g")
                bbnote "Use TF_M_CONFIG '${tfm_unitary_config}' as subfolder for 'tfm_secure_binary_name' location"
                # Init devicetree list for single TF_M_CONFIG
                dt_config=$(echo ${TF_M_DEVICETREE} | cut -d',' -f1)
                bbnote "Use TF-M devicetree list: ${dt_config}"
                for dt in ${dt_config}; do
                    [ -n "$(echo ${dt} | grep ${board_lower})" ] || continue
                    # Init tfm_secure_binary_name
                    tfm_secure_binary_name="tfm-${dt}_s.elf"
                    if ! [ -e ${RECIPE_SYSROOT}/firmware/${tfm_unitary_config}/${tfm_secure_binary_name} ]; then
                        bbwarn "Skip '*_tfm_sign.bin' creation: '${tfm_secure_binary_name}' not found in sysroot: firmware/${tfm_unitary_config}/"
                        continue
                    fi
                    # Generate ${BIN_NAME_NS}-${dt}_tfm_sign.bin
                    ${STAGING_BINDIR_NATIVE}/st_copro_firmware_signature.sh \
                            --input-nsecure ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/${BIN_NAME_NS_ELF} \
                            --input-secure ${RECIPE_SYSROOT}/firmware/${tfm_unitary_config}/${tfm_secure_binary_name} \
                            --signature-key ${TA_DEV_KIT_DIR}/keys/${M33_OPTEE_KEYNAME} \
                            --output ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/${BIN_NAME_NS}-${dt}
                done
            fi

            #temporary: add firmware to permit to load the project at runtime
            install -m 0755 ${UNPACKDIR}/fw_cortex_m33.sh ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/
            for ex in ${DEFAULT_COPRO_FIRMWARE}; do
                if $(echo "${ex}" | grep -q ${project}) ; then
                    board_project=$(echo ${project} | cut -d'/' -f1 | tr '[:upper:]' '[:lower:]')
                    for sb in ${M33_BOARDS_START_AT_STARTUP}; do
                        if $(echo ${sb} | grep -q ${board_project}) ; then
                            echo "${ex}" > ${D}${M33_OUTPUT_4USERFS}/${INSTALL_PROJ}/default.${sb}
                        fi
                    done
                fi
            done
        done
    done
    if [ "${M33_BOARDS_SERVICE}" -eq 1 ]; then
        # Install sh script to load/execute DEFAULT_COPRO_FIRMWARE @boot up
        if [ -n "${DEFAULT_COPRO_FIRMWARE}" ]; then
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

            sed -i -e "s:@userfs_mount_point@:${M33_OUTPUT_4USERFS}:g" \
            ${D}${sysconfdir}/init.d/st-m33firmware-load-default.sh
            sed -i -e "s:@userfs_mount_point@:${M33_OUTPUT_4USERFS}:g" \
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
FILES:${PN} += "${@bb.utils.contains('M33_PACKAGE_4USERFS', '1', '', '${M33_OUTPUT_4USERFS}', d)}"
FILES:${PN}-userfs = "${@bb.utils.contains('M33_PACKAGE_4USERFS', '1', '${M33_OUTPUT_4USERFS}', '', d)}"
RDEPENDS:${PN}-userfs += "busybox"

# Avoid QA issue because binaries are for an Arm architecture but the platform is an AArch64
INSANE_SKIP = "arch"
INSANE_SKIP:${PN} = "already-stripped buildpaths"
INSANE_SKIP:${PN}-userfs = "buildpaths"
INSANE_SKIP:${PN}-dbg = "buildpaths"
