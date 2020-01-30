COMPATIBLE_MACHINE = "(stm32mpcommon)"

inherit kernel

DEPENDS += "openssl-native util-linux-native"

B = "${WORKDIR}/build"
# Configure build dir for externalsrc class usage through devtool
EXTERNALSRC_BUILD_pn-${PN} = "${WORKDIR}/build"

# To share config fragments between layers
FILESEXTRAPATHS_prepend := "${THISDIR}:"

# -------------------------------------------------------------
# Do not deploy kernel module with specfic tarball
MODULE_TARBALL_DEPLOY = "0"

# ---------------------------------------------------------------------
# Defconfig
#
#If the defconfig is contained on the kernel tree (arch/${ARCH}/configs)
#you must use the following line
do_configure_prepend() {
    unset CFLAGS CPPFLAGS CXXFLAGS LDFLAGS MACHINE
    if [ ! -z ${KERNEL_DEFCONFIG} ];
    then
        bbnote "Kernel customized: configuration of linux STM by using DEFCONFIG: ${KERNEL_DEFCONFIG}"
        oe_runmake ${PARALLEL_MAKE} -C ${S} O=${B} CC="${KERNEL_CC}" LD="${KERNEL_LD}" ${KERNEL_DEFCONFIG}
    else
        if [ ! -z ${KERNEL_EXTERNAL_DEFCONFIG} ];
        then
            bbnote "Kernel customized: configuration of linux STM by using external DEFCONFIG"
            install -m 0644 ${WORKDIR}/${KERNEL_EXTERNAL_DEFCONFIG} ${B}/.config
            oe_runmake -C ${S} O=${B} CC="${KERNEL_CC}" LD="${KERNEL_LD}" oldconfig
        else
            bbwarn "You must specify KERNEL_DEFCONFIG or KERNEL_EXTERNAL_DEFCONFIG"
            die "NO DEFCONFIG SPECIFIED"
        fi
    fi

    if [ ! -z "${KERNEL_CONFIG_FRAGMENTS}" ]
    then
        for f in ${KERNEL_CONFIG_FRAGMENTS}
        do
            bbnote "file = $f"
            # Check if the config fragment was copied into the WORKDIR from
            # the OE meta data
            if [ ! -e "$f" ]
            then
                echo "Could not find kernel config fragment $f"
                exit 1
            fi
        done

        bbnote "${S}/scripts/kconfig/merge_config.sh -m -r -O ${B} ${B}/.config ${KERNEL_CONFIG_FRAGMENTS} 1>&2"
        # Now that all the fragments are located merge them.
        (${S}/scripts/kconfig/merge_config.sh -m -r -O ${B} ${B}/.config ${KERNEL_CONFIG_FRAGMENTS} 1>&2 )
    fi

    yes '' | oe_runmake -C ${S} O=${B} CC="${KERNEL_CC}" LD="${KERNEL_LD}" oldconfig
    #oe_runmake -C ${S} O=${B} savedefconfig && cp ${B}/defconfig ${WORKDIR}/defconfig.saved
}

# ---------------------------------------------------------------------
do_install_append() {
    # Install KERNEL_IMAGETYPE for kernel-imagebootfs package
    install -m 0644 ${KERNEL_OUTPUT_DIR}/${KERNEL_IMAGETYPE} ${D}/${KERNEL_IMAGEDEST}
}

# ---------------------------------------------------------------------
# Support checking the kernel load address parameter: expecting proper value for ST kernel.
#
python do_loadaddrcheck() {
    if not d.getVar('ST_KERNEL_LOADADDR'):
        bb.fatal('Missing ST_KERNEL_LOADADDR value for ST kernel build: please define it in your machine.')
}

PACKAGES =+ "${KERNEL_PACKAGE_NAME}-headers ${KERNEL_PACKAGE_NAME}-imagebootfs"
FILES_${KERNEL_PACKAGE_NAME}-headers = "${exec_prefix}/src/linux*"
FILES_${KERNEL_PACKAGE_NAME}-image  += "boot/ ${KERNEL_IMAGEDEST}"
FILES_${KERNEL_PACKAGE_NAME}-imagebootfs = "boot/*.dtb boot/${KERNEL_IMAGETYPE}"
