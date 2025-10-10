COMPATIBLE_MACHINE = "(stm32mp2common)"

PACKAGE_ARCH = "${MACHINE_ARCH}"

B = "${S}"

DEPENDS += "gcc-arm-none-eabi-native cmake-native"

# Temporary
SRC_URI += "file://fw_cortex_m0.sh"

# Create specific userfs package
M0_PACKAGE_4USERFS ?= "1"
PACKAGES += "${@bb.utils.contains('M0_PACKAGE_4USERFS', '1', '${PN}-userfs', '', d)}"

# Define default board reference for M0
M0_BOARDS ?= ""

# list of boards/devitree on which the service must be started
M33_BOARDS_START_AT_STARTUP ?= ""

BUILD_CONFIG ?= "Debug"

# Init default installation path
M0_FOLDER ?= "Cube-M0-examples"
M0_INSTALLDIR ?= "${prefix}local"
M0_OUTPUT_4USERFS = "${M33_INSTALLDIR}/${M0_FOLDER}"


M0_ELF_NOT_STRIPPED ??= "0"

do_compile() {
    unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS
    for BIN_MACHINE in ${M0_BOARDS}; do
        for project in ${PROJECTS_LIST_M0} ; do
            [ "$(echo ${project} | cut -d'/' -f1)" = "${BIN_MACHINE}" ] || continue
            [ -d "${B}/Projects/${project}" ] || continue
            [ -e "${B}/Projects/${project}/CMakeLists.txt" ] || continue

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
    # Install M0 firmwares, scripts and README in userfs:
    # <userfs>/Cube-M0-examples/${project} (ie STM32MP257F-EV1/Applications/OpenAMP/OpenAMP_TTY_echo)
    #                        |-- fw_cortex_m0.sh
    #                        |-- README
    #                        |--lib
    #                             |--firmware
    #                                  |-- ELF file for impacted board (ie OpenAMP_TTY_echo_CM0_NonSecure.elf)
    for BIN_MACHINE in ${M0_BOARDS}; do
        for project in ${PROJECTS_LIST_M0} ; do
            [ "$(echo ${project} | cut -d'/' -f1)" = "${BIN_MACHINE}" ] || continue

            if [ -d "${B}/Projects/${project}/build" ]; then
                BIN_NAME=$(basename ${project})
                PROJ_DIR=${project}/build
                INSTALL_PROJ=${project}
            elif [ -d "${B}/Projects/${project}" ]; then
                BIN_NAME=$(basename $(dirname ${project}))
                PROJ_DIR=${project}
                INSTALL_PROJ=$(echo ${project} | sed 's,/bin,,')
            else
                bbfatal "[install] Issue with ${project}"
            fi
            BIN_NAME_NS="${BIN_NAME}_NonSecure"

            # Install M0 firmwares
            install -d ${D}${M0_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/
            install -m 0755 ${B}/Projects/${PROJ_DIR}/${BIN_NAME_NS}_stripped.elf ${D}${M0_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/
            install -m 0755 ${B}/Projects/${PROJ_DIR}/${BIN_NAME_NS}.bin ${D}${M0_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/
            if [ "${M0_ELF_NOT_STRIPPED}" = "1" ]; then
                install -m 0755 ${B}/Projects/${PROJ_DIR}/${BIN_NAME_NS}.elf ${D}${M0_OUTPUT_4USERFS}/${INSTALL_PROJ}/lib/firmware/
            fi
            # Install sh and README files if any for each example
            install -m 0755 ${UNPACKDIR}/fw_cortex_m0.sh ${D}${M0_OUTPUT_4USERFS}/${INSTALL_PROJ}/
        done
    done

}

# -----------------------------------------------------------

INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_SYSROOT_STRIP = "1"

FILES:${PN} += "${nonarch_base_libdir}/firmware ${sysconfdir}/init.d ${systemd_unitdir}/system ${systemd_unitdir}/system-shutdown"
# Configure package split
FILES:${PN} += "${@bb.utils.contains('M0_PACKAGE_4USERFS', '1', '', '${M0_OUTPUT_4USERFS}', d)}"
FILES:${PN}-userfs = "${@bb.utils.contains('M0_PACKAGE_4USERFS', '1', '${M0_OUTPUT_4USERFS}', '', d)}"
RDEPENDS:${PN}-userfs += "busybox"

# Avoid QA issue because binaries are for an Arm architecture but the platform is an AArch64
INSANE_SKIP = "arch"
INSANE_SKIP:${PN} = "already-stripped buildpaths"
INSANE_SKIP:${PN}-userfs = "buildpaths"
INSANE_SKIP:${PN}-dbg = "buildpaths"
