require recipes-bsp/u-boot/u-boot.inc

FILESEXTRAPATHS:prepend := "${THISDIR}/u-boot-stm32mp:"

# Configure build dir for externalsrc class usage through devtool
EXTERNALSRC_BUILD:pn-${PN} = "${WORKDIR}/build"
B = "${WORKDIR}/build"

# Define dedicated var to configure SPL binary name to override U-Boot default
# configuration. By this way we allow to mix configuration with and without SPL
# binary generation without trouble with binary existence.
SPL_BINARY_STM32 = "spl/u-boot-spl.stm32"
SPL_BINARYNAME = "${@os.path.basename(d.getVar("SPL_BINARY_STM32"))}"
SPL_BINARYROOT = "${@d.getVar('SPL_BINARY_STM32').split('.')[0]}"

# Configure for debug elf
ELF_DEBUG_ENABLE ?= "1"
UBOOT_ELF = "${@'u-boot' if d.getVar('ELF_DEBUG_ENABLE') == '1' else ''}"
SPL_ELF = "${@'${SPL_BINARYROOT}' if d.getVar('ELF_DEBUG_ENABLE') == '1' else ''}"
SPL_ELF_NAME  = "${@os.path.basename(d.getVar("SPL_ELF"))}.elf"

# Init UBOOT_DEVICETREE list if not configured
UBOOT_DEVICETREE ?= ""

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
# Prepend configure to handle specific configuration (here fip)
#
do_configure:prepend() {
    if [ -n "${UBOOT_CONFIG}" ]; then
        unset i j
        for config in ${UBOOT_MACHINE}; do
            if [ -f "${S}/configs/${config}" ]; then
                # Create copy of original defconfig
                cp -f "${S}/configs/${config}" "${WORKDIR}/"
                i=$(expr $i + 1)
                for binary in ${UBOOT_BINARIES}; do
                    j=$(expr $j + 1)
                    if [ $j -eq $i ]; then
                        binarysuffix=$(echo ${binary} | cut -d'.' -f2)
                        # Make sure to select STM32IMAGE if requested
                        if [ "${binarysuffix}" = "stm32" ]; then
                            if ! grep -q 'CONFIG_STM32MP15x_STM32IMAGE=y' "${S}/configs/${config}"; then
                                echo "CONFIG_STM32MP15x_STM32IMAGE=y" >> "${S}/configs/${config}"
                            fi
                        fi
                        if [ "${UBOOT_SIGN_ENABLE}" = "1" ]; then
                            if ! grep -q 'CONFIG_FIT_SIGNATURE=y' "${S}/configs/${config}"; then
                                echo "CONFIG_FIT_SIGNATURE=y" >> "${S}/configs/${config}"
                            fi
                        fi
                    fi
                done
                unset j
            fi
        done
    fi
}

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
    if [ -n "${UBOOT_DEVICETREE}" ]; then
        if [ -n "${UBOOT_CONFIG}" ]; then
            unset i j k
            for config in ${UBOOT_MACHINE}; do
                i=$(expr $i + 1);
                for type in ${UBOOT_CONFIG}; do
                    j=$(expr $j + 1);
                    if [ $j -eq $i ]; then
                        # Get short suffix for current type
                        type_suffix=$(echo ${type} | cut -d'_' -f1)
                        type_filter=$(echo ${type} | cut -d'_' -f2)
                        [ "${type_suffix}" = "${type_filter}" ] && type_filter=""
                        for devicetree in ${UBOOT_DEVICETREE}; do
                            if [ -n "${type_filter}" ]; then
                                if [ "$(echo ${devicetree} | grep -c ${type_filter})" -eq 1 ]; then
                                    bbnote "The ${type_filter} filter for ${type_suffix} config matches ${devicetree} device tree. Go for build..."
                                else
                                    bbnote "The ${type_filter} filter for ${type_suffix} config doesn't match ${devicetree} device tree. Skip build!"
                                    continue
                                fi
                            fi
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
                                fi
                            done
                            unset k
                            # As soon as SPL binary exists, copy it with specific binary_type name
                            # This allow to mix u-boot configuration, with and without SPL
                            if [ -f "${B}/${config}/${SPL_BINARY_STM32}" ]; then
                                install -m 644 ${B}/${config}/${SPL_BINARY_STM32} ${B}/${config}/${SPL_BINARYNAME}-${devicetree}
                            fi
                        done
                    fi
                done
                unset j
            done
        else
            bbfatal "Wrong u-boot-stm32mp configuration: please make sure to use UBOOT_CONFIG through BOOTSCHEME_LABELS config"
        fi
    fi
}

# -----------------------------------------------------------------------------
# Append deploy to handle specific device tree binary deployement
#
do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}/u-boot"
do_deploy:append() {
    if [ -n "${UBOOT_DEVICETREE}" ]; then
        if [ -n "${UBOOT_CONFIG}" ]; then
            # Clean deploydir from any available binary first
            # This allows to only install the devicetree binary ones
            rm -rf ${DEPLOYDIR}
            install -d ${DEPLOYDIR}

            unset i j k
            for config in ${UBOOT_MACHINE}; do
                i=$(expr $i + 1)
                for type in ${UBOOT_CONFIG}; do
                    j=$(expr $j + 1)
                    if [ $j -eq $i ]; then
                        for binary in ${UBOOT_BINARIES}; do
                            k=$(expr $k + 1)
                            if [ $k -eq $i ]; then
                                binarysuffix=$(echo ${binary} | cut -d'.' -f2)
                                # Manage subfolder in case of u-boot.img
                                if [ "${binarysuffix}" = "img" ]; then
                                    SUBFOLDER="/${type}"
                                else
                                    SUBFOLDER=""
                                fi
                                # Install destination folder
                                install -d ${DEPLOYDIR}${SUBFOLDER}
                                [ -n "${ELF_DEBUG_ENABLE}" ] && install -d ${DEPLOYDIR}${SUBFOLDER}/debug
                                # Get short suffix for current type
                                type_suffix=$(echo ${type} | cut -d'_' -f1)
                                type_filter=$(echo ${type} | cut -d'_' -f2)
                                [ "${type_suffix}" = "${type_filter}" ] && type_filter=""
                                for devicetree in ${UBOOT_DEVICETREE}; do
                                    if [ -n "${type_filter}" ]; then
                                        if [ "$(echo ${devicetree} | grep -c ${type_filter})" -eq 1 ]; then
                                            bbnote "The ${type_filter} filter for ${type_suffix} config matches ${devicetree} device tree. Go for binary deploy..."
                                        else
                                            bbnote "The ${type_filter} filter for ${type_suffix} config doesn't match ${devicetree} device tree. Skip binary deploy!"
                                            continue
                                        fi
                                    fi
                                    # Install u-boot binary
                                    install -m 644 ${B}/${config}/u-boot-${devicetree}.${binarysuffix} ${DEPLOYDIR}${SUBFOLDER}/
                                    # As soon as SPL binary exists, install it
                                    # This allow to mix u-boot configuration, with and without SPL
                                    if [ -f "${B}/${config}/${SPL_BINARYNAME}-${devicetree}" ]; then
                                        install -m 644 ${B}/${config}/${SPL_BINARYNAME}-${devicetree} ${DEPLOYDIR}${SUBFOLDER}/
                                    fi
                                    # Init soc suffix
                                    soc_suffix=""
                                    if [ -n "${STM32MP_SOC_NAME}" ]; then
                                        for soc in ${STM32MP_SOC_NAME}; do
                                            [ "$(echo ${devicetree} | grep -c ${soc})" -eq 1 ] && soc_suffix="-${soc}"
                                        done
                                    fi
                                    # Install 'u-boot-nodtb.bin' binary in case '*.dtb' binary installation configured
                                    if [ "${binarysuffix}" = "dtb" ]; then
                                        install -m 644 ${B}/${config}/u-boot-nodtb.bin ${DEPLOYDIR}${SUBFOLDER}/u-boot-nodtb${soc_suffix}.bin
                                    fi
                                    if [ -n "${UBOOT_ELF}" ]; then
                                        install -m 644 ${B}/${config}/${UBOOT_ELF} ${DEPLOYDIR}${SUBFOLDER}/debug/u-boot${soc_suffix}.${UBOOT_ELF_SUFFIX}
                                    fi
                                    if [ -n "${SPL_ELF}" ] && [ -f "${B}/${config}/${SPL_ELF}" ]; then
                                        install -m 644 ${B}/${config}/${SPL_ELF} ${DEPLOYDIR}${SUBFOLDER}/debug/${SPL_ELF_NAME}${soc_suffix}
                                    fi

                                    if [ "${UBOOT_SIGN_ENABLE}" = "1" -a -n "${UBOOT_DTB_BINARY}" ] ; then
                                        # rm previous installed binaries
                                        if [ -e "${DEPLOYDIR}/${UBOOT_DTB_BINARY}" ]; then
                                            rm ${DEPLOYDIR}/${UBOOT_DTB_BINARY}
                                        fi
                                        if [ -e "${DEPLOYDIR}/${UBOOT_DTB_SYMLINK}" ]; then
                                            rm ${DEPLOYDIR}/${UBOOT_DTB_SYMLINK}
                                        fi
                                        if [ -e "${DEPLOYDIR}/${DEPLOYDIR}/${UBOOT_NODTB_SYMLINK}" ]; then
                                            rm ${DEPLOYDIR}/${UBOOT_NODTB_SYMLINK} ${DEPLOYDIR}/${UBOOT_NODTB_BINARY}
                                        fi
                                        if [ -e "${UBOOT_DTB_BINARY}" ]; then
                                            install u-boot-${MACHINE}-${devicetree}-${PV}-${PR}.dtb ${DEPLOYDIR}/
                                            install u-boot-${MACHINE}-${devicetree}.dtb ${DEPLOYDIR}/
                                        fi

                                        if [ -f "${UBOOT_NODTB_BINARY}" ]; then
                                            install u-boot-nodtb-${soc_suffix}-${PV}-${PR}.bin ${DEPLOYDIR}/
                                        fi


                                    fi

                                done
                            fi
                        done
                        unset k
                    fi
                done
                unset j
            done
        else
            bbfatal "Wrong u-boot-stm32mp configuration: please make sure to use UBOOT_CONFIG through BOOTSCHEME_LABELS config"
        fi
    fi

    if [ "x${UBOOT_SIGN_ENABLE}" = "x1" ] ; then
        #  work only there is only one devicetree
        # get name of u-boot devicetree without signature
        ubootdevicetree=`ls -1 ${DEPLOYDIR}/*.dtb | head -n 1`
        namewithoutsignature=`echo $ubootdevicetree | sed "s/\.dtb/-without-signature.dtb/g"`
        namewithsignature=`echo $ubootdevicetree | sed "s/\.dtb/-with-signature.dtb/g"`
        mv $ubootdevicetree $namewithoutsignature
        #get name of kernel uboot devicetree
        nameonkernel=`ls -1 ${DEPLOY_DIR_IMAGE}/kernel/u-boot-${MACHINE}*.dtb | head -n 1`
        cp $nameonkernel $namewithsignature
        cp $nameonkernel $ubootdevicetree
    fi

}

# ---------------------------------------------------------------------
# Avoid QA Issue: No GNU_HASH in the elf binary
INSANE_SKIP:${PN} += "ldflags"
# ---------------------------------------------------------------------
# Avoid QA Issue: ELF binary has relocations in .text
# (uboot no need -fPIC option : remove check)
INSANE_SKIP:${PN} += "textrel"
