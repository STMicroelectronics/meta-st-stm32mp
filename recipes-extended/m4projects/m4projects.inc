FILESEXTRAPATHS_prepend := "${THISDIR}/files:"

COMPATIBLE_MACHINE = "(stm32mpcommon)"

PACKAGE_ARCH = "${MACHINE_ARCH}"

B = "${WORKDIR}/build"
# For external source support with devtools
EXTERNALSRC_BUILD_pn-${PN} = "${WORKDIR}/build"

# M4 firwmare build file
SRC_URI += "file://Makefile.stm32"
SRC_URI += "file://parse_project_config.py"

# Default service for systemd
inherit systemd update-rc.d python3native
SRC_URI += "file://st-m4firmware-load-default.sh"
SRC_URI += "file://st-m4firmware-load.service"
SRC_URI += "file://shutdown-stm32mp1-m4.sh"

DEPENDS += " gcc-arm-none-eabi-native "

# Create specific userfs package
M4_PACKAGE_4USERFS ?= "1"
PACKAGES += "${@bb.utils.contains('M4_PACKAGE_4USERFS', '1', '${PN}-userfs', '', d)}"

# Define default board reference for M4
M4_BOARDS ?= "STM32MP157C-EV1 STM32MP157C-DK2"

# Init M4 board service install
M4_BOARDS_SERVICE ?= "1"

# Init default copro example to load/execute
DEFAULT_COPRO_FIRMWARE ?= ""

BUILD_CONFIG ?= "Debug"

# Init default installation path
M4_FOLDER ?= "Cube-M4-examples"
M4_INSTALLDIR ?= "${prefix}local"
M4_OUTPUT_4USERFS = "${M4_INSTALLDIR}/${M4_FOLDER}"

# Use gcc bare toolchain
EXTRA_OEMAKE += "CROSS_COMPILE=${RECIPE_SYSROOT_NATIVE}${datadir}/gcc-arm-none-eabi/bin/arm-none-eabi-"
EXTRA_OEMAKE += "CPU_TYPE=M4"
EXTRA_OEMAKE += "BUILD_CONFIG=${BUILD_CONFIG}"

do_compile() {
    # Compile M4 firmwares listed in bb file. They can be for eval or disco board as specified in bb file
    for BIN_MACHINE in ${M4_BOARDS}; do
        bbnote "Compile M4 project for : ${BIN_MACHINE}"
        for project in ${PROJECTS_LIST} ; do
            bbnote "Parsing M4 project : ${project}"
            if [ "$(echo ${project} | cut -d'/' -f1)" = "${BIN_MACHINE}" ]; then
                bbnote "Selected M4 project : ${project}"

                unset LDFLAGS CFLAGS CPPFLAGS CFLAGS_ASM

                BIN_NAME=$(basename ${project})
                PROJECT_DIR=${B}/${project}
                PROJECT_APP="${S}/Projects/${project}/STM32CubeIDE/CM4"

                bbnote "BIN_NAME     : ${BIN_NAME}"
                bbnote "PROJECT_DIR  : ${PROJECT_DIR}"
                bbnote "PROJECT_APP  : ${PROJECT_APP}"
                bbnote "BUILD_CONFIG : ${BUILD_CONFIG}"
                bbnote "CPU_TYPE     : ${CPU_TYPE}"
                bbnote "SOURCE       : ${S}"

                mkdir -p ${PROJECT_DIR}/out/${BUILD_CONFIG}

                bbnote "Parsing M4 project to get file list and build flags for ${project}"
                ${PYTHON} ${WORKDIR}/parse_project_config.py "${PROJECT_APP}" "${BUILD_CONFIG}" "${PROJECT_DIR}"
                bbnote "Cleaning M4 project : ${project}"
                oe_runmake -f ${WORKDIR}/Makefile.stm32 BIN_NAME=${BIN_NAME} PROJECT_DIR=${PROJECT_DIR} clean
                bbnote "Building M4 project : ${project}"
                oe_runmake -f ${WORKDIR}/Makefile.stm32 BIN_NAME=${BIN_NAME} PROJECT_DIR=${PROJECT_DIR} all
            fi
        done
    done
}

do_install() {
    # Install M4 firmwares, scripts and README in userfs:
    # <userfs>/Cube-M4-examples/${project} (ie STM32MP157C-EV1/Applications/OpenAMP/OpenAMP_TTY_echo)
    #                        |-- fw_cortex_m4.sh
    #                        |-- README
    #                        |--lib
    #                             |--firmware
    #                                  |-- ELF file for impacted board (ie OpenAMP_TTY_echo.elf)
    for BIN_MACHINE in ${M4_BOARDS}; do
        for project in ${PROJECTS_LIST} ; do
            if [ "$(echo ${project} | cut -d'/' -f1)" = "${BIN_MACHINE}" ]; then
                BIN_NAME=$(basename ${project})

                # Install M4 firmwares
                install -d ${D}${M4_OUTPUT_4USERFS}/${project}/lib/firmware/
                install -m 0755 ${B}/${project}/out/${BUILD_CONFIG}/${BIN_NAME}.elf ${D}${M4_OUTPUT_4USERFS}/${project}/lib/firmware/

                # Install sh and README files if any for each example
                if [ -e ${S}/Projects/${project}/Remoteproc/fw_cortex_m4.sh ]; then
                    install -m 0755 ${S}/Projects/${project}/Remoteproc/fw_cortex_m4.sh ${D}${M4_OUTPUT_4USERFS}/${project}
                fi
                if [ -e ${S}/Projects/${project}/Remoteproc/README ]; then
                    install -m 0755 ${S}/Projects/${project}/Remoteproc/README ${D}${M4_OUTPUT_4USERFS}/${project}
                fi
            fi
        done
    done

    if [ "${M4_BOARDS_SERVICE}" -eq 1 ]; then
        # Install sh script to load/execute DEFAULT_COPRO_FIRMWARE @boot up
        if [ -n "${DEFAULT_COPRO_FIRMWARE}" ]; then
            # Install systemd service
            install -d ${D}${sysconfdir}/init.d/
            install -d ${D}${base_sbindir}/
            install -m 0755 ${WORKDIR}/st-m4firmware-load-default.sh ${D}${sysconfdir}/init.d/st-m4firmware-load-default.sh
            install -m 0755 ${WORKDIR}/st-m4firmware-load-default.sh ${D}${base_sbindir}/st-m4firmware-load-default.sh
            install -m 0755 ${WORKDIR}/shutdown-stm32mp1-m4.sh ${D}${base_sbindir}/shutdown-stm32mp1-m4.sh

            sed -i -e "s:@default_fw@:${DEFAULT_COPRO_FIRMWARE}:" \
            ${D}${sysconfdir}/init.d/st-m4firmware-load-default.sh
            sed -i -e "s:@default_fw@:${DEFAULT_COPRO_FIRMWARE}:" \
            ${D}${base_sbindir}/st-m4firmware-load-default.sh

            sed -i -e "s:@userfs_mount_point@:${M4_OUTPUT_4USERFS}:" \
            ${D}${sysconfdir}/init.d/st-m4firmware-load-default.sh
            sed -i -e "s:@userfs_mount_point@:${M4_OUTPUT_4USERFS}:" \
            ${D}${base_sbindir}/st-m4firmware-load-default.sh
        fi

        # Install systemd service for all machines configurations
        if ${@bb.utils.contains('DISTRO_FEATURES','systemd','true','false',d)}; then
            install -d ${D}${systemd_unitdir}/system
            install -d ${D}${systemd_unitdir}/system-shutdown
            install -m 644 ${WORKDIR}/st-m4firmware-load.service ${D}/${systemd_unitdir}/system
            install -m 755 ${WORKDIR}/shutdown-stm32mp1-m4.sh ${D}/${systemd_unitdir}/system-shutdown
        fi
    fi
}

# -----------------------------------------------------------
# specific for service: start copro m4 firwmare at boot time
SYSTEMD_PACKAGES += " m4projects-stm32mp1 "
SYSTEMD_SERVICE_${PN} = "st-m4firmware-load.service"
SYSTEMD_AUTO_ENABLE_${PN} = "disable"

INITSCRIPT_NAME = "st-m4firmware-load-default.sh"
INITSCRIPT_PARAMS = "stop 22 5 3 ."
# -----------------------------------------------------------

INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_SYSROOT_STRIP = "1"

FILES_${PN} += "${nonarch_base_libdir}/firmware ${sysconfdir}/init.d ${systemd_unitdir}/system ${systemd_unitdir}/system-shutdown"
# Configure package split
FILES_${PN} += "${@bb.utils.contains('M4_PACKAGE_4USERFS', '1', '', '${M4_OUTPUT_4USERFS}', d)}"
FILES_${PN}-userfs = "${@bb.utils.contains('M4_PACKAGE_4USERFS', '1', '${M4_OUTPUT_4USERFS}', '', d)}"
RDEPENDS_${PN}-userfs += "busybox"
