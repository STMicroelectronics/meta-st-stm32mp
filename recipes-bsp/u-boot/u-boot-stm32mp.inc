FILESEXTRAPATHS:prepend := "${THISDIR}/u-boot-stm32mp:"

require recipes-bsp/u-boot/u-boot.inc

inherit external-dt
inherit fip-utils-stm32mp

# Include U-BOOT config definitions
require u-boot-stm32mp-config.inc

B = "${WORKDIR}/build"
# Configure build dir for externalsrc class usage through devtool
EXTERNALSRC_BUILD:pn-${PN} = "${WORKDIR}/build"

SRC_URI:append = " ${@bb.utils.contains_any('BOOTSCHEME_LABELS', 'fastboot fastboot-opteemin', 'file://fragment-04-fastboot_mmc0.fb_cfg;subdir=fragments/features', '', d)}"
SRC_URI:append = " ${@bb.utils.contains_any('BOOTSCHEME_LABELS', 'fastboot fastboot-opteemin', 'file://fragment-05-fastboot_mmc1.fb_cfg;subdir=fragments/features', '', d)}"

# Configure for debug elf
ELF_DEBUG_ENABLE ?= "1"
UBOOT_ELF = "${@'u-boot' if d.getVar('ELF_DEBUG_ENABLE') == '1' else ''}"

# -----------------------------------------------
# Enable use of work-shared folder
STAGING_UBOOT_DIR = "${TMPDIR}/work-shared/${MACHINE}/uboot-source"
DEBUG_PREFIX_MAP += " -fmacro-prefix-map=${STAGING_UBOOT_DIR}=${TARGET_DBGSRC_DIR} "
DEBUG_PREFIX_MAP += " -fdebug-prefix-map=${STAGING_UBOOT_DIR}=${TARGET_DBGSRC_DIR} "

# Make sure to move ${S} to STAGING_UBOOT_DIR. We can't just
# create the symlink in advance as the git fetcher can't cope with
# the symlink.
do_unpack[cleandirs] += " ${S} ${STAGING_UBOOT_DIR}"
do_clean[cleandirs] += " ${S} ${STAGING_UBOOT_DIR}"
base_do_unpack:append () {
    # Specific part to update devtool-source class
    if bb.data.inherits_class('devtool-source', d):
        # We don't want to move the source to STAGING_UBOOT_DIR here
        if d.getVar('STAGING_UBOOT_DIR', d):
            d.setVar('STAGING_UBOOT_DIR', '${S}')

    # Copy/Paste from kernel class with adaptation to UBOOT var
    s = d.getVar("S")
    if s[-1] == '/':
        # drop trailing slash, so that os.symlink(ubootsrc, s) doesn't use s as directory name and fail
        s=s[:-1]
    ubootsrc = d.getVar("STAGING_UBOOT_DIR")
    if s != ubootsrc:
        bb.utils.mkdirhier(ubootsrc)
        bb.utils.remove(ubootsrc, recurse=True)
        if d.getVar("EXTERNALSRC"):
            # With EXTERNALSRC S will not be wiped so we can symlink to it
            os.symlink(s, ubootsrc)
        else:
            import shutil
            shutil.move(s, ubootsrc)
            os.symlink(ubootsrc, s)
}

# -----------------------------------------------------------------------------
# Prepend a way to add specific fragments for fastboot
# returns all the elements from the src uri that are .fb_cfg files
def find_fb_cfgs(d):
    sources=src_patches(d, True)
    sources_list=[]
    for s in sources:
        if s.endswith('.fb_cfg'):
            sources_list.append(s)

    return sources_list

python get_fastboot_fragments() {
    fbconfigflags = d.getVarFlags('UBOOT_CONFIG')
    # The "doc" varflag is special, we don't want to see it here
    fbconfigflags.pop('doc', None)
    fbconfig = (d.getVar('UBOOT_CONFIG') or "").split()

    UBOOT_FB_CFG = ''
    if len(fbconfig) > 0:
        for config in fbconfig:
            for f, v in fbconfigflags.items():
                if config == f and "fastboot" in f:
                    items = v.split(',')
                    if items[0]:
                        device_type = items[0].upper().split('-')[2]
                        device_type = device_type.split('_')[0]
                        device_frag = d.getVar('DEVICE:%s' % device_type)
                        if device_frag:
                            d.appendVar('UBOOT_FB_CFG', device_frag)
                        else:
                            bb.fatal('Wrong UBOOT_CONFIG. %s is not a correct device' % i)
            d.appendVar('UBOOT_FB_CFG', ',')
}

do_generate_defconfig() {
    if [ -n "${UBOOT_CONFIG}" ]; then
        unset i j
        for config in ${UBOOT_MACHINE}; do
            i=$(expr $i + 1);
            for type in ${UBOOT_CONFIG}; do
                j=$(expr $j + 1);
                if [ $j -eq $i ]; then
                    # Check if a fastboot configuration needs to be done
                    fb_cfg="$(echo ${UBOOT_FB_CFG} | cut -d',' -f $i )"
                    if [ -n "${fb_cfg}" ]; then
                        tmp_cfg="$(echo ${@" ".join(find_fb_cfgs(d))})"
                        for f in ${tmp_cfg}; do
                            fcf=$(echo $f | sed 's,.*/fragment-[0-9]*-\(.*\)\.fb_cfg,\1,')
                            if echo ${fcf} | grep ${fb_cfg}; then
                                def_cfg="$(echo ${config} | sed 's,-fastboot.*_,_,')"
                                cat ${S}/configs/${def_cfg} ${f} > ${S}/configs/${config}
                            fi
                        done
                    fi
                fi
            done
            unset j
        done
        unset i
    fi
}
addtask generate_defconfig before do_configure after do_patch
do_generate_defconfig[prefuncs] += "get_fastboot_fragments"
do_generate_defconfig[vardeps] += "SRC_URI"

# -----------------------------------------------------------------------------
# Append compile to handle specific device tree compilation
#
do_compile:append() {
    if [ -n "${UBOOT_CONFIG}" ]; then
        unset i j k
        for config in ${UBOOT_MACHINE}; do
            i=$(expr $i + 1);
            # Initialize devicetree list
            uboot_devicetree=$(echo ${UBOOT_DEVICETREE} | cut -d',' -f${i})
            uboot_extra_oemake=$(echo ${UBOOT_MAKE_OPT} | cut -d',' -f${i})
            for type in ${UBOOT_CONFIG}; do
                j=$(expr $j + 1);
                if [ $j -eq $i ]; then
                    # Get short suffix for current type
                    type_suffix=$(echo ${type} | cut -d'_' -f1)
                    for devicetree in ${uboot_devicetree}; do
                        # Cleanup previous build artifact
                        [ -f "${B}/${config}/dts/dt.dtb" ] && rm "${B}/${config}/dts/dt.dtb"
                        # Build target
                        oe_runmake -C ${S} O=${B}/${config} $uboot_extra_oemake DEVICE_TREE=${devicetree} DEVICE_TREE_EXT=${devicetree}.dtb
                        # Install specific binary
                        for binary in ${UBOOT_BINARIES}; do
                            k=$(expr $k + 1);
                            if [ $k -eq $i ]; then
                                binarysuffix=$(echo ${binary} | cut -d'.' -f2)
                                install -m 644 ${B}/${config}/${binary} ${B}/${config}/u-boot-${devicetree}-${type_suffix}.${binarysuffix}
                                if [ "${binarysuffix}" = "dtb" ]; then
                                    install -m 644 ${B}/${config}/u-boot-nodtb.bin ${B}/${config}/u-boot-nodtb-${type_suffix}.bin
                                fi
                            fi
                        done
                        unset k
                    done
                fi
            done
            unset j
        done
    fi
}

# -----------------------------------------------------------------------------
# Append deploy to handle specific device tree binary deployement
#
export_binaries() {
    local dest="${1}"
    local fit_uboot_sign_deploy="${2}"

    if [ -n "${UBOOT_CONFIG}" ]; then
        unset i j k
        for config in ${UBOOT_MACHINE}; do
            i=$(expr $i + 1)
            # Initialize devicetree list
            uboot_devicetree=$(echo ${UBOOT_DEVICETREE} | cut -d',' -f${i})
            for type in ${UBOOT_CONFIG}; do
                j=$(expr $j + 1)
                if [ $j -eq $i ]; then
                    for binary in ${UBOOT_BINARIES}; do
                        k=$(expr $k + 1)
                        if [ $k -eq $i ]; then
                            binarysuffix=$(echo ${binary} | cut -d'.' -f2)
                            # Install destination folder
                            install -d ${dest}
                            [ -n "${ELF_DEBUG_ENABLE}" ] && install -d ${dest}/debug
                            # Get short suffix for current type
                            type_suffix=$(echo ${type} | cut -d'_' -f1)
                            install -m 0644  ${B}/${config}/.config ${dest}/configuration-${config}
                            for devicetree in ${uboot_devicetree}; do
                                # Install u-boot binary
                                install -m 644 ${B}/${config}/u-boot-${devicetree}-${type_suffix}.${binarysuffix} ${dest}
                                # Init soc suffix
                                soc_suffix=""
                                if [ -n "${STM32MP_SOC_NAME}" ]; then
                                    for soc in ${STM32MP_SOC_NAME}; do
                                        [ "$(echo ${devicetree} | grep -c ${soc})" -eq 1 ] && soc_suffix="-${soc}"
                                    done
                                fi
                                # Install 'u-boot-nodtb.bin' binary in case '*.dtb' binary installation configured
                                if [ "${binarysuffix}" = "dtb" ]; then
                                    install -m 644 ${B}/${config}/u-boot-nodtb-${type_suffix}.bin ${dest}/u-boot-nodtb${soc_suffix}-${type_suffix}.bin
                                fi
                                if [ -n "${UBOOT_ELF}" ]; then
                                    install -m 644 ${B}/${config}/${UBOOT_ELF} ${dest}/debug/u-boot${soc_suffix}-${type_suffix}.${UBOOT_ELF_SUFFIX}
                                fi

                                if [ "${UBOOT_SIGN_ENABLE}" = "1" ] && [ "${fit_uboot_sign_deploy}" = "1" ]; then
                                    # Prepare original binary for signature
                                    cp -f ${B}/${config}/u-boot-${devicetree}-${type_suffix}.${binarysuffix} ${dest}/u-boot-${devicetree}-${type_suffix}-without-signature.${binarysuffix}
                                    # Sign dummy image images in order to add the image signing keys to our dtb
                                    ${UBOOT_MKIMAGE_SIGN} \
                                            ${@'-D "${UBOOT_MKIMAGE_DTCOPTS}"' if len('${UBOOT_MKIMAGE_DTCOPTS}') else ''} \
                                            -f auto \
                                            -k "${UBOOT_SIGN_KEYDIR}" \
                                            -o "${FIT_HASH_ALG},${FIT_SIGN_ALG}" \
                                            -g "${UBOOT_SIGN_IMG_KEYNAME}" \
                                            -K "${B}/${config}/u-boot-${devicetree}-${type_suffix}.${binarysuffix}" \
                                            -d /dev/null \
                                            ${UBOOT_MKIMAGE_SIGN_ARGS} \
                                            -r ${B}/${config}/unused.itb
                                    # Sign dummy image configurations in order to add the configuration signing keys to our dtb
                                    ${UBOOT_MKIMAGE_SIGN} \
                                            ${@'-D "${UBOOT_MKIMAGE_DTCOPTS}"' if len('${UBOOT_MKIMAGE_DTCOPTS}') else ''} \
                                            -f auto-conf \
                                            -k "${UBOOT_SIGN_KEYDIR}" \
                                            -o "${FIT_HASH_ALG},${FIT_SIGN_ALG}" \
                                            -g "${UBOOT_SIGN_IMG_KEYNAME}" \
                                            -K "${B}/${config}/u-boot-${devicetree}-${type_suffix}.${binarysuffix}" \
                                            -d /dev/null \
                                            ${UBOOT_MKIMAGE_SIGN_ARGS} \
                                            -r ${B}/${config}/unused.itb

                                    # Install signed binary
                                    install -m 644 ${B}/${config}/u-boot-${devicetree}-${type_suffix}.${binarysuffix} ${dest}/u-boot-${devicetree}-${type_suffix}.${binarysuffix}
                                    install -m 644 ${B}/${config}/u-boot-${devicetree}-${type_suffix}.${binarysuffix} ${dest}/u-boot-${devicetree}-${type_suffix}-with-signature.${binarysuffix}
                                fi
                            done
                        fi
                    done
                    unset k
                fi
            done
            unset j
        done
    fi
}

do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}${FIP_DIR_UBOOT}"
do_deploy() {
    export_binaries ${DEPLOYDIR} 1
}
addtask deploy before do_build after do_install

u_boot_sysroot_populate() {
    export_binaries ${SYSROOT_DESTDIR}/${FIP_DIR_UBOOT} 0
}
SYSROOT_PREPROCESS_FUNCS =+ "u_boot_sysroot_populate"
SYSROOT_DIRS:append = " ${FIP_DIR_UBOOT}"

# ---------------------------------------------------------------------
# Avoid QA Issue: No GNU_HASH in the elf binary
INSANE_SKIP:${PN} += "ldflags"
# ---------------------------------------------------------------------
# Avoid QA Issue: ELF binary has relocations in .text
# (uboot no need -fPIC option : remove check)
INSANE_SKIP:${PN} += "textrel"

