SUMMARY = "Firwmare update metadata"
DESCRIPTION = "Raw firmware to manage information about firmware update"
SECTION = "bootloaders"
LICENSE = "BSD-3-Clause"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/files/common-licenses/BSD-3-Clause;md5=550794465ba0ec5312d6919e203a55f9"

PV = "1.0"

PACKAGE_ARCH = "${MACHINE_ARCH}"
COMPATIBLE_MACHINE = "(stm32mpcommon)"

DEPENDS = "u-boot-tools-stm32mp-native"

inherit deploy

do_fetch[noexec] = "1"
do_unpack[noexec] = "1"
do_patch[noexec] = "1"
do_configure[noexec] = "1"
do_install[noexec] = "1"


# Set metadata generation
ENABLE_METADATA_GENERATION ??= "${@bb.utils.contains('MACHINE_FEATURES', 'fw-update', '1', '0', d)}"
METADATA_PREFIX_NAME ??= "metadata"
METADATA_SUFFIX ??= "bin"

# Manage only fip A/B
METADATA_BINARY_DEFAULT ??= "${METADATA_PREFIX_NAME}.${METADATA_SUFFIX}"

# manage FIP A/B and linux gpt image for emmc
METADATA_AB_BINARY_EMMC ??= "${METADATA_PREFIX_NAME}-ab-emmc.${METADATA_SUFFIX}"

# manage FIP A/B and linux gpt image for sdcard
METADATA_AB_BINARY_SDCARD ??= "${METADATA_PREFIX_NAME}-ab-sdcard.${METADATA_SUFFIX}"

# manage FIP A/B and linux gpt image for nor (only nor-sdcard or nor-emmc)
METADATA_AB_BINARY_NOR ??= "${METADATA_PREFIX_NAME}-ab-nor.${METADATA_SUFFIX}"
METADATA_AB_BINARY_NOR_SDCARD ??= "${METADATA_PREFIX_NAME}-ab-nor-sdcard.${METADATA_SUFFIX}"
METADATA_AB_BINARY_NOR_EMMC ??= "${METADATA_PREFIX_NAME}-ab-nor-emmc.${METADATA_SUFFIX}"

# manage FIP A/B and linux gpt image for nand (only nand-sdcard or nand-emmc)
METADATA_AB_BINARY_NAND ??= "${METADATA_PREFIX_NAME}-ab-nand.${METADATA_SUFFIX}"
METADATA_AB_BINARY_NAND_SDCARD ??= "${METADATA_PREFIX_NAME}-ab-nand-sdcard.${METADATA_SUFFIX}"
METADATA_AB_BINARY_NAND_EMMC ??= "${METADATA_PREFIX_NAME}-ab-nand-emmc.${METADATA_SUFFIX}"

# manage FIP A/B and linux gpt image for spinand (only spinand-sdcard or spinand-emmc)
METADATA_AB_BINARY_SPINAND ??= "${METADATA_PREFIX_NAME}-ab-spinand.${METADATA_SUFFIX}"
METADATA_AB_BINARY_SPINAND_SDCARD ??= "${METADATA_PREFIX_NAME}-ab-spinand-sdcard.${METADATA_SUFFIX}"
METADATA_AB_BINARY_SPINAND_EMMC ??= "${METADATA_PREFIX_NAME}-ab-spinand-emmc.${METADATA_SUFFIX}"

METADATA_TOOL_NAME ??= "mkfwumdata"
METADATA_TOOL ??= "${STAGING_BINDIR_NATIVE}/${METADATA_TOOL_NAME}"
METADATA_TOOL_VERSION ??= "2"

# Metadata firmware argurment for fip support only
METADATA_TOOL_ARGS_DEFAULT ??= "-g -i 1 -b 2 -v ${METADATA_TOOL_VERSION} ${DEVICE_TYPEUUID_FWU_MDATA},${DEVICE_TYPEUUID_FIP},${DEVICE_PARTUUID_FIP_A},${DEVICE_PARTUUID_FIP_B}"

# EMMC configuration for 3 images: fip, bootfs, rootfs
METADATA_TOOL_AB_ARGS_STORAGE_EMMC ??= "-g -i 3 -b 2 -v ${METADATA_TOOL_VERSION} \
    ${DEVICE_TYPEUUID_FWU_MDATA},${DEVICE_TYPEUUID_FIP},${DEVICE_PARTUUID_FIP_A},${DEVICE_PARTUUID_FIP_B}\
    ${DEVICE_TYPEUUID_FWU_MDATA},${DEVICE_METADATA_TYPEUUID_BOOTFS},${DEVICE_PARTUUID_BOOTFS_A:EMMC},${DEVICE_PARTUUID_BOOTFS_B:EMMC}\
    ${DEVICE_TYPEUUID_FWU_MDATA},${DEVICE_METADATA_TYPEUUID_ROOTFS},${DEVICE_PARTUUID_ROOTFS_A:EMMC},${DEVICE_PARTUUID_ROOTFS_B:EMMC}"

METADATA_TOOL_AB_ARGS_EMMC ??= "${METADATA_TOOL_AB_ARGS_STORAGE_EMMC}"
METADATA_TOOL_AB_ARGS_NOR_EMMC ??= "${METADATA_TOOL_AB_ARGS_STORAGE_EMMC}"
METADATA_TOOL_AB_ARGS_NAND_EMMC ??= "${METADATA_TOOL_AB_ARGS_STORAGE_EMMC}"
METADATA_TOOL_AB_ARGS_SPINAND_EMMC ??= "${METADATA_TOOL_AB_ARGS_STORAGE_EMMC}"

# SDCARD configuration for 3 images: fip, bootfs, rootfs
METADATA_TOOL_AB_ARGS_STORAGE_SDCARD ??= "-g -i 3 -b 2 -v ${METADATA_TOOL_VERSION} \
    ${DEVICE_TYPEUUID_FWU_MDATA},${DEVICE_TYPEUUID_FIP},${DEVICE_PARTUUID_FIP_A},${DEVICE_PARTUUID_FIP_B}\
    ${DEVICE_TYPEUUID_FWU_MDATA},${DEVICE_METADATA_TYPEUUID_BOOTFS},${DEVICE_PARTUUID_BOOTFS_A:SDCARD},${DEVICE_PARTUUID_BOOTFS_B:SDCARD}\
    ${DEVICE_TYPEUUID_FWU_MDATA},${DEVICE_METADATA_TYPEUUID_ROOTFS},${DEVICE_PARTUUID_ROOTFS_A:SDCARD},${DEVICE_PARTUUID_ROOTFS_B:SDCARD}"

METADATA_TOOL_AB_ARGS_SDCARD ??= "${METADATA_TOOL_AB_ARGS_STORAGE_SDCARD}"
METADATA_TOOL_AB_ARGS_NOR_SDCARD ??= "${METADATA_TOOL_AB_ARGS_STORAGE_SDCARD}"
METADATA_TOOL_AB_ARGS_NAND_SDCARD ??= "${METADATA_TOOL_AB_ARGS_STORAGE_SDCARD}"
METADATA_TOOL_AB_ARGS_SPINAND_SDCARD ??= "${METADATA_TOOL_AB_ARGS_STORAGE_SDCARD}"

# filesystem on nor, nand or spinand are not supported by metadata, by default use fip a/b configuration
METADATA_TOOL_AB_ARGS_NOR ??= "${METADATA_TOOL_ARGS_DEFAULT}"
METADATA_TOOL_AB_ARGS_NAND ??= "${METADATA_TOOL_ARGS_DEFAULT}"
METADATA_TOOL_AB_ARGS_SPINAND ??= "${METADATA_TOOL_ARGS_DEFAULT}"

do_compile() {
    if [ "${ENABLE_METADATA_GENERATION}" = "1" ]; then

        ${METADATA_TOOL_NAME} ${METADATA_TOOL_ARGS_DEFAULT} ${METADATA_BINARY_DEFAULT}

        if ${@bb.utils.contains('MACHINE_FEATURES','fw-update-ab','true','false',d)}; then

            ${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_EMMC} ${METADATA_AB_BINARY_EMMC}
            ${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_NOR_EMMC} ${METADATA_AB_BINARY_NOR_EMMC}
            ${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_NAND_EMMC} ${METADATA_AB_BINARY_NAND_EMMC}
            ${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_SPINAND_EMMC} ${METADATA_AB_BINARY_SPINAND_EMMC}

            ${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_SDCARD} ${METADATA_AB_BINARY_SDCARD}
            ${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_NOR_SDCARD} ${METADATA_AB_BINARY_NOR_SDCARD}
            ${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_NAND_SDCARD} ${METADATA_AB_BINARY_NAND_SDCARD}
            ${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_SPINAND_SDCARD} ${METADATA_AB_BINARY_SPINAND_SDCARD}

            ${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_NOR} ${METADATA_AB_BINARY_NOR}
            ${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_NAND} ${METADATA_AB_BINARY_NAND}
            ${METADATA_TOOL_NAME} ${METADATA_TOOL_AB_ARGS_SPINAND} ${METADATA_AB_BINARY_SPINAND}

        fi
    fi
}

do_deploy[sstate-outputdirs] = "${DEPLOY_DIR_IMAGE}/metadata"
do_deploy() {
    if [ "${ENABLE_METADATA_GENERATION}" = "1" ]; then
        install -d "${DEPLOYDIR}"
        if [ -f "${B}/${METADATA_BINARY_DEFAULT}" ]; then
            install -m 644 "${B}/${METADATA_BINARY_DEFAULT}" "${DEPLOYDIR}"
        fi
        if ${@bb.utils.contains('MACHINE_FEATURES','fw-update-ab','true','false',d)}; then
            if ${@bb.utils.contains('PARTITIONS_BOOTLOADER_CONFIG','emmc','true','false',d)}; then
                # AB: emmc
                if [ -f "${B}/${METADATA_AB_BINARY_EMMC}" ]; then
                    install -m 644 "${B}/${METADATA_AB_BINARY_EMMC}" "${DEPLOYDIR}"
                fi
                if ${@bb.utils.contains('BOOTDEVICE_LABELS','nor-emmc','true','false',d)}; then
                    # AB: nor-emmc
                    if [ -f "${B}/${METADATA_AB_BINARY_NOR_EMMC}" ]; then
                        install -m 644 "${B}/${METADATA_AB_BINARY_NOR_EMMC}" "${DEPLOYDIR}"
                    fi
                fi
                if [ "${STM32MP_MACHINE_HAVE_NAND_EMMC}" = "1" ]; then
                    # AB; nand-emmc
                    if [ -f "${B}/${METADATA_AB_BINARY_NAND_EMMC}" ]; then
                        install -m 644 "${B}/${METADATA_AB_BINARY_NAND_EMMC}" "${DEPLOYDIR}"
                    fi
                    # AB; spinand-emmc
                    if [ -f "${B}/${METADATA_AB_BINARY_SPINAND_EMMC}" ]; then
                        install -m 644 "${B}/${METADATA_AB_BINARY_SPINAND_EMMC}" "${DEPLOYDIR}"
                    fi
                fi
            fi
            if ${@bb.utils.contains('PARTITIONS_BOOTLOADER_CONFIG','sdcard','true','false',d)}; then
                # AB; sdcard
                if [ -f "${B}/${METADATA_AB_BINARY_SDCARD}" ]; then
                    install -m 644 "${B}/${METADATA_AB_BINARY_SDCARD}" "${DEPLOYDIR}"
                fi
                if ${@bb.utils.contains('BOOTDEVICE_LABELS','nor-sdcard','true','false',d)}; then
                    # AB; nor-sdcard
                    if [ -f "${B}/${METADATA_AB_BINARY_NOR_SDCARD}" ]; then
                        install -m 644 "${B}/${METADATA_AB_BINARY_NOR_SDCARD}" "${DEPLOYDIR}"
                    fi
                fi
                if [ "${STM32MP_MACHINE_HAVE_NAND_SDCARD}" = "1" ]; then
                    # AB; nand-sdcard
                    if [ -f "${B}/${METADATA_AB_BINARY_NAND_SDCARD}" ]; then
                        install -m 644 "${B}/${METADATA_AB_BINARY_NAND_SDCARD}" "${DEPLOYDIR}"
                    fi
                    # AB; spinand-sdcard
                    if [ -f "${B}/${METADATA_AB_BINARY_SPINAND_SDCARD}" ]; then
                        install -m 644 "${B}/${METADATA_AB_BINARY_SPINAND_SDCARD}" "${DEPLOYDIR}"
                    fi
                fi
            fi
            if ${@bb.utils.contains('PARTITIONS_BOOTLOADER_CONFIG','nor','true','false',d)}; then
                # AB; nor
                if [ -f "${B}/${METADATA_AB_BINARY_NOR}" ]; then
                    install -m 644 "${B}/${METADATA_AB_BINARY_NOR}" "${DEPLOYDIR}"
                fi
            fi
            if [ "${STM32MP_MACHINE_HAVE_NAND_ONLY}" = "1" ]; then
                # AB; nand
                if [ -f "${B}/${METADATA_AB_BINARY_NAND}" ]; then
                    install -m 644 "${B}/${METADATA_AB_BINARY_NAND}" "${DEPLOYDIR}"
                fi
                # AB; spinand
                if [ -f "${B}/${METADATA_AB_BINARY_SPINAND}" ]; then
                    install -m 644 "${B}/${METADATA_AB_BINARY_SPINAND}" "${DEPLOYDIR}"
                fi
            fi
        fi
    fi
}

addtask deploy before do_build after do_compile

ALLOW_EMPTY:${PN} = "1"
