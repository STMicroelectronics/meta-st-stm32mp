FILESEXTRAPATHS:prepend := "${THISDIR}/u-boot-stm32mp:"

require recipes-bsp/u-boot/u-boot.inc

inherit external-dt
inherit fip-utils-stm32mp

# Include U-BOOT config definitions
require u-boot-stm32mp-config.inc

B = "${WORKDIR}/build"
# Configure build dir for externalsrc class usage through devtool
EXTERNALSRC_BUILD:pn-${PN} = "${WORKDIR}/build"

SRC_URI:append = " ${@bb.utils.contains('BOOTSCHEME_LABELS', 'fastboot', 'file://fragment-04-fastboot_mmc0.fb_cfg;subdir=fragments/features', '', d)}"
SRC_URI:append = " ${@bb.utils.contains('BOOTSCHEME_LABELS', 'fastboot', 'file://fragment-05-fastboot_mmc1.fb_cfg;subdir=fragments/features', '', d)}"

EXTRA_OEMAKE += "${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', 'EXT_DTS=${STAGING_EXTDT_DIR}/${EXTDT_DIR_UBOOT}', '', d)}"

# Configure for debug elf
ELF_DEBUG_ENABLE ?= "1"
UBOOT_ELF = "${@'u-boot' if d.getVar('ELF_DEBUG_ENABLE') == '1' else ''}"

# Init default MTDPART configurations
UBOOT_MTDPART_CHECK_ENABLE ??= ""
UBOOT_MTDPART_CHECK ??= ""

# -----------------------------------------------
# Enable use of work-shared folder
STAGING_UBOOT_DIR = "${TMPDIR}/work-shared/${MACHINE}/uboot-source"
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
# Append configure to handle specific MTDPART check if required
#
do_configure:append() {
    if [ -n "${UBOOT_MTDPART_CHECK_ENABLE}" ]; then
        if [ -n "${UBOOT_CONFIG}" ]; then
            for config in ${UBOOT_MACHINE}; do
                for mtdpart_check_config in ${UBOOT_MTDPART_CHECK_ENABLE}; do
                    if [ "${mtdpart_check_config}" = "${config}" ]; then
                        if [ -f "${B}/${config}/.config" ]; then
                            for mtdpart_chk in $(echo "${UBOOT_MTDPART_CHECK}"); do
                                mtdpart_conf=$(echo ${mtdpart_chk} | cut -d'=' -f1)
                                mtdpart_set=$(echo ${mtdpart_chk} | cut -d'=' -f2)
                                if grep -q "${mtdpart_conf}=" "${B}/${config}/.config"; then
                                    bbnote "Found ${mtdpart_conf} definition in '.config' file from '${config}/' build folder"
                                    # Get the current MTDPART configuration settings (and remove the double quotes)
                                    mtdpart_cur=$(grep "${mtdpart_conf}=" "${B}/${config}/.config" | cut -d'=' -f2 | sed 's/"//g')
                                    # Init for loop on configured partition list
                                    i=0
                                    mtdpart_count=$(echo "${mtdpart_cur}" | awk -F',' '{print NF-1}')
                                    while [ ${i} -lt "${mtdpart_count}" ]; do
                                        i=$(expr $i + 1)
                                        mtdpart_cur_size=$(echo ${mtdpart_cur} | cut -d',' -f${i} | sed 's/\([0-9]*[mk]\).*/\1/')
                                        mtdpart_set_size=$(echo ${mtdpart_set} | cut -d',' -f${i} | sed 's/\([0-9]*\).*/\1/')
                                        # Make sure to use KiB format for partition size
                                        if [ -z "$(echo ${mtdpart_cur_size} | grep m)" ]; then
                                            cur_size_kb=$(echo ${mtdpart_cur_size} | sed 's/k//')
                                        else
                                            cur_size_kb=$(expr $(echo ${mtdpart_cur_size} | sed 's/m//') \* 1024)
                                        fi
                                        # Compare partition size
                                        if [ "${mtdpart_set_size}" != "${cur_size_kb}" ]; then
                                            bbfatal "The default settings for ${mtdpart_conf} (${mtdpart_cur}) is different from the one configured (${mtdpart_set}) : ${mtdpart_set_size}k versus ${mtdpart_cur_size}"
                                        fi
                                    done
                                    bbnote "Found ${mtdpart_set} settings for ${mtdpart_conf} in '.config' file from '${config}/' build folder : configuration is ok"
                                else
                                    bbfatal "${mtdpart_conf} definition not found in '.config' file from '${config}/' build folder : may need update for CONFIG name..."
                                fi
                            done
                        fi
                    fi
                done
            done
        fi
    fi
}

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
            for type in ${UBOOT_CONFIG}; do
                j=$(expr $j + 1);
                if [ $j -eq $i ]; then
                    # Get short suffix for current type
                    type_suffix=$(echo ${type} | cut -d'_' -f1)
                    for devicetree in ${uboot_devicetree}; do
                        # Cleanup previous build artifact
                        [ -f "${B}/${config}/dts/dt.dtb" ] && rm "${B}/${config}/dts/dt.dtb"
                        # Build target
                        oe_runmake -C ${S} O=${B}/${config} DEVICE_TREE=${devicetree} DEVICE_TREE_EXT=${devicetree}.dtb
                        # Install specific binary
                        for binary in ${UBOOT_BINARIES}; do
                            k=$(expr $k + 1);
                            if [ $k -eq $i ]; then
                                binarysuffix=$(echo ${binary} | cut -d'.' -f2)
                                install -m 644 ${B}/${config}/${binary} ${B}/${config}/u-boot-${devicetree}.${binarysuffix}
                                if [ "x${UBOOT_SIGN_ENABLE}" = "x1" ] ; then
                                        ubootdevicetree="${B}/${config}/u-boot-${devicetree}.${binarysuffix}"
                                        namewithoutsignature="${B}/${config}/u-boot-${devicetree}-without-signature.${binarysuffix}"
                                        cp $ubootdevicetree $namewithoutsignature
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

    if [ -n "${UBOOT_CONFIG}" ]; then
        # Clean deploydir from any available binary first
        # This allows to only install the devicetree binary ones

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
                            for devicetree in ${uboot_devicetree}; do
                                # Install u-boot binary
                                install -m 644 ${B}/${config}/u-boot-${devicetree}.${binarysuffix} ${dest}
                                # Init soc suffix
                                soc_suffix=""
                                if [ -n "${STM32MP_SOC_NAME}" ]; then
                                    for soc in ${STM32MP_SOC_NAME}; do
                                        [ "$(echo ${devicetree} | grep -c ${soc})" -eq 1 ] && soc_suffix="-${soc}"
                                    done
                                fi
                                # Install 'u-boot-nodtb.bin' binary in case '*.dtb' binary installation configured
                                if [ "${binarysuffix}" = "dtb" ]; then
                                    install -m 644 ${B}/${config}/u-boot-nodtb.bin ${dest}/u-boot-nodtb${soc_suffix}-${type_suffix}.bin
                                fi
                                if [ -n "${UBOOT_ELF}" ]; then
                                    install -m 644 ${B}/${config}/${UBOOT_ELF} ${dest}/debug/u-boot${soc_suffix}-${type_suffix}.${UBOOT_ELF_SUFFIX}
                                fi

                                if [ "${UBOOT_SIGN_ENABLE}" = "1" -a -n "${UBOOT_DTB_BINARY}" ] ; then
                                    ${UBOOT_MKIMAGE_SIGN} \
                                            ${@'-D "${UBOOT_MKIMAGE_DTCOPTS}"' if len('${UBOOT_MKIMAGE_DTCOPTS}') else ''} \
                                            -F -k "${UBOOT_SIGN_KEYDIR}" \
                                            -K "${B}/${config}/u-boot-${devicetree}.${binarysuffix}" \
                                            -r ${B}/fitImage-linux \
                                            ${UBOOT_MKIMAGE_SIGN_ARGS}

                                    cp ${B}/${config}/u-boot-${devicetree}.${binarysuffix} ${B}/${config}/u-boot-${devicetree}-with-signature.${binarysuffix}
                                    install -m 644 ${B}/${config}/u-boot-${devicetree}-without-signature.${binarysuffix} ${dest}/
                                    install -m 644 ${B}/${config}/u-boot-${devicetree}-with-signature.${binarysuffix} ${dest}/
                                    install -m 644 ${B}/${config}/u-boot-${devicetree}.${binarysuffix} ${dest}/
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

do_install:append() {
    export_binaries ${D}${FIP_DIR_UBOOT}
}

do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}${FIP_DIR_UBOOT}"
do_deploy() {
    export_binaries ${DEPLOYDIR}
}
addtask deploy before do_build after do_install

PACKAGES =+ "${PN}-fip"

FILES:${PN}-fip = "${FIP_DIR_UBOOT}"
SYSROOT_DIRS:append = " ${FIP_DIR_UBOOT}"

# ---------------------------------------------------------------------
# Avoid QA Issue: No GNU_HASH in the elf binary
INSANE_SKIP:${PN} += "ldflags"
# ---------------------------------------------------------------------
# Avoid QA Issue: ELF binary has relocations in .text
# (uboot no need -fPIC option : remove check)
INSANE_SKIP:${PN} += "textrel"
# ---------------------------------------------------------------------
# Avoid QA Issue: contains reference to TMPDIR [buildpaths]
INSANE_SKIP:${PN} += "buildpaths"

