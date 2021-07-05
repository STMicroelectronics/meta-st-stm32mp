COMPATIBLE_MACHINE = "(stm32mpcommon)"

inherit kernel
inherit kernel-fitimage

DEPENDS += "openssl-native util-linux-native libyaml-native"

B = "${WORKDIR}/build"
# Configure build dir for externalsrc class usage through devtool
EXTERNALSRC_BUILD_pn-${PN} = "${WORKDIR}/build"

# To share config fragments between layers
FILESEXTRAPATHS_prepend := "${THISDIR}:"

do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}/kernel"

#---------------------------------------------------------------
# Module kernel signature
KERNEL_SIGN_ENABLE ?= "0"
EXTRA_OEMAKE += "${@oe.utils.ifelse(d.getVar('KERNEL_SIGN_ENABLE') == '1', 'INSTALL_MOD_STRIP=1','')}"

# Deploy kernel config file to deploy folder
KERNEL_CONFIG_DEPLOY ??= "0"

# YoctoProject bugzilla : 140044
#   There's a race between do_symlink_kernsrc and do_populate_lic, since the latter is ordered "after do_patch";
#
# Below a STMicroelectonics' hacks waiting for official Yocto patch:
#       commit already exists in gathesgarth :
#a2b50b74d609ce079ab36b82d4c7c3539fb56485 kernel.bbclass: ensure symlink_kernsrc task gets run even with externalsrc
python () {
    bb.build.deltask('do_symlink_kernsrc', d)
    bb.build.addtask('do_symlink_kernsrc', 'do_patch do_configure', 'do_unpack', d)
}


# ---------------------------------------------------------------------
# Defconfig
#
#If the defconfig is contained on the kernel tree (arch/${ARCH}/configs)
#you must use the following line
do_configure_append() {
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
            yes '' | oe_runmake -C ${S} O=${B} CC="${KERNEL_CC}" LD="${KERNEL_LD}" oldconfig
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
do_compile_kernelmodules_append() {
    if (grep -q -i -e '^CONFIG_MODULES=y$' ${B}/.config); then
        # 5.10+ kernels have module.lds that we need to copy for external module builds
        if [ -e "${B}/scripts/module.lds" ]; then
            install -Dm 0644 ${B}/scripts/module.lds ${STAGING_KERNEL_BUILDDIR}/scripts/module.lds
        fi
    fi
}
# ---------------------------------------------------------------------
do_install_append() {
    # Install KERNEL_IMAGETYPE for kernel-imagebootfs package
    install -m 0644 ${KERNEL_OUTPUT_DIR}/${KERNEL_IMAGETYPE} ${D}/${KERNEL_IMAGEDEST}

    if ${@bb.utils.contains('MACHINE_FEATURES','gpu','true','false',d)}; then
        # when ACCEPT_EULA are filled
        echo "blacklist etnaviv" > ${D}/${sysconfdir}/modprobe.d/blacklist.conf
    fi
    #snd dependencies
    install -d ${D}/${sysconfdir}/modprobe.d/
    echo "softdep snd-soc-cs42l51 pre: snd-soc-cs42l51-i2c" > ${D}/${sysconfdir}/modprobe.d/stm32mp1-snd.conf
}
# ---------------------------------------------------------------------
do_deploy_append() {
    if [ ${MODULE_TARBALL_DEPLOY} = "1" ] && (grep -q -i -e '^CONFIG_MODULES=y$' .config); then
        mkdir -p ${D}${root_prefix}/lib
        tar -cvzf $deployDir/modules-stripped-${MODULE_TARBALL_NAME}.tgz -C ${WORKDIR}/package/${root_prefix} lib
        ln -sf modules-stripped-${MODULE_TARBALL_NAME}.tgz $deployDir/modules-stripped-${MODULE_TARBALL_LINK_NAME}.tgz
    fi
    if [ ${KERNEL_CONFIG_DEPLOY} = "1" ] && [ -f "${D}/boot/config-${KERNEL_VERSION}" ]; then
        install -m 0644 ${D}/boot/config-${KERNEL_VERSION} $deployDir
    fi
}
do_deploy[depends] += " virtual/kernel:do_package "
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
FILES_${KERNEL_PACKAGE_NAME}-modules += "${sysconfdir}/modprobe.d"
