#@DESCRIPTION: Common Machine configuration for STM32 systems

require conf/machine/include/st-machine-nand-storage.inc
require conf/machine/include/st-machine-extlinux-config-stm32mp.inc
require conf/machine/include/st-machine-features-stm32mp.inc
require conf/machine/include/st-machine-flashlayout-stm32mp.inc
require conf/machine/include/st-machine-flashlayout-deleteall-stm32mp.inc
require conf/machine/include/st-machine-flashlayout-extensible-stm32mp.inc
require conf/machine/include/st-machine-image-partition-stm32mp.inc
require conf/machine/include/st-machine-storage-device-stm32mp.inc
require conf/machine/include/st-machine-sdk-stm32mp.inc

# Define specific common layer name
MACHINEOVERRIDES =. "stm32mpcommon:"

# Define specific common machine name
MACHINEOVERRIDES =. "stcommon:"

# =========================================================================
# SOC
# =========================================================================
STM32MP_SOC_NAME = ""
STM32MP_SOC_NAME:append:stm32mp15common = " stm32mp15 "
STM32MP_SOC_NAME:append:stm32mp13common = " stm32mp13 "
STM32MP_SOC_NAME:append:stm32mp21common = " stm32mp21 "
STM32MP_SOC_NAME:append:stm32mp23common = " stm32mp23 "
STM32MP_SOC_NAME:append:stm32mp25common = " stm32mp25 "
# Take care of nativesdk override use case
STM32MP_SOC_NAME:append:class-nativesdk = " stm32mp15 "
STM32MP_SOC_NAME:append:class-nativesdk = " stm32mp13 "
STM32MP_SOC_NAME:append:class-nativesdk = " stm32mp21 "
STM32MP_SOC_NAME:append:class-nativesdk = " stm32mp23 "
STM32MP_SOC_NAME:append:class-nativesdk = " stm32mp25 "

STM32MP_ENCRYPT_SOC_NAME = ""
STM32MP_ENCRYPT_SOC_NAME:append:stm32mp13common = " stm32mp13 "
STM32MP_ENCRYPT_SOC_NAME:append:stm32mp21common = " stm32mp21 "
STM32MP_ENCRYPT_SOC_NAME:append:stm32mp23common = " stm32mp23 "
STM32MP_ENCRYPT_SOC_NAME:append:stm32mp25common = " stm32mp25 "

STM32MP_ENCRYPT_COPRO_SOC_NAME = ""
STM32MP_ENCRYPT_COPRO_SOC_NAME:append:stm32mp21common = " stm32mp21 "
STM32MP_ENCRYPT_COPRO_SOC_NAME:append:stm32mp23common = " stm32mp23 "
STM32MP_ENCRYPT_COPRO_SOC_NAME:append:stm32mp25common = " stm32mp25 "


# =========================================================================
# boot scheme
# =========================================================================
# List of supported boot schemes
BOOTSCHEME_LABELS ??= "optee opteemin fastboot fastboot-opteemin"

# =========================================================================
# boot device
# =========================================================================
# List of supported boot devices
BOOTDEVICE_LABELS ??= "emmc nor-emmc nand-4-256-1024 nor-sdcard sdcard nand-4-256-1024-sdcard"

# =========================================================================
# Machine settings
# =========================================================================
# Default machine feature
MACHINE_FEATURES = "usbhost usbgadget alsa screen ext2 pci"
MACHINE_FEATURES:append = " ${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', 'optee', '', d)} "
MACHINE_FEATURES:append = " tpm2 "

# Enable installation of INITRD image on bootfs side
MACHINE_FEATURES:append = " initrd "

# Configure autoresize for any of the 'ext4' storage devices (through InitRD image)
MACHINE_FEATURES:append = " ${@bb.utils.contains_any('BOOTDEVICE_LABELS', \
                ['emmc', 'nor-sdcard', \
                'nand-4-256-1024-sdcard', 'nand-4-256-512-sdcard', 'nand-2-128-256-sdcard', 'nand-custom-sdcard',\
                'sdcard'], \
                'autoresize', '', d)} "

# Use FIP image for boot loaders
MACHINE_FEATURES:append = " fip"

# Enable firmware secure update feature
MACHINE_FEATURES:append = " fw-update"

# Optional A/B firmware update feature.
# Uncomment to enable support for bootfs-a/b and rootfs-a/b partitions
# required for atomic system updates.
# Requires "fw-update".
#MACHINE_FEATURES:append = " fw-update-ab"

python () {
    features = (d.getVar("MACHINE_FEATURES") or "").split()

    if "fw-update-ab" in features and "fw-update" not in features:
        bb.fatal("MACHINE_FEATURES 'fw-update-ab' requires 'fw-update'")
}

# Default serial consoles (TTYs) to enable using getty
# Before kernel 4.18, serial console are ttyS3 but after is ttySTM0
SERIAL_CONSOLE = "115200 ttySTM0"
SERIAL_CONSOLES = "${@d.getVar('SERIAL_CONSOLE').replace(' ', ';')}"

# Default external-dt init
EXTERNAL_DT_ENABLED ?= "1"

# Configure use of device tree suffix
EXTDT_USE_SUFFIX ?= "0"
EXTDT_SUFFIX_EMMC    ?= ""
EXTDT_SUFFIX_NAND    ?= ""
EXTDT_SUFFIX_NOR     ?= ""
EXTDT_SUFFIX_SDCARD  ?= ""
EXTDT_SUFFIX_SPINAND ?= ""

# Configure external-dt to consider
EXTDT_ROOTDIR:stm32mp1common = "stm32mp1/"
EXTDT_ROOTDIR:stm32mp2common = "${@bb.utils.contains('MACHINE_FEATURES', 'm33td', 'stm32mp2/m33-td/', 'stm32mp2/a35-td/', d)}"

EXTERNAL_DEVICETREE_EMMC ??= ""
EXTERNAL_DEVICETREE_NAND ??= ""
EXTERNAL_DEVICETREE_NOR_all   ??= ""
EXTERNAL_DEVICETREE_NOR_mb    ??= ""
EXTERNAL_DEVICETREE_NOR_mb_sb ??= ""
EXTERNAL_DEVICETREE_SDCARD ??= ""
EXTERNAL_DEVICETREE_SPINAND ??= ""

EXTERNAL_DT_ALL += "${EXTERNAL_DEVICETREE_EMMC}"
EXTERNAL_DT_ALL += "${EXTERNAL_DEVICETREE_NAND}"
EXTERNAL_DT_ALL += "${EXTERNAL_DEVICETREE_NOR_all}"
EXTERNAL_DT_ALL += "${EXTERNAL_DEVICETREE_NOR_mb}"
EXTERNAL_DT_ALL += "${EXTERNAL_DEVICETREE_NOR_mb_sb}"
EXTERNAL_DT_ALL += "${EXTERNAL_DEVICETREE_SDCARD}"
EXTERNAL_DT_ALL += "${EXTERNAL_DEVICETREE_SPINAND}"
# Feed overall external device tree list (without duplicate)
EXTERNAL_DEVICETREE:append = " ${@' '.join('%s' % dt_file for dt_file in list(dict.fromkeys((d.getVar('EXTERNAL_DT_ALL') or '').split())))} "

EXTERNAL_DT_SUFFIX_ALL += "${@' '.join(dt + d.getVar('EXTDT_SUFFIX_EMMC') for dt in '${EXTERNAL_DEVICETREE_EMMC}'.split())}"
EXTERNAL_DT_SUFFIX_ALL += "${@' '.join(dt + d.getVar('EXTDT_SUFFIX_NAND') for dt in '${EXTERNAL_DEVICETREE_NAND}'.split())}"
EXTERNAL_DT_SUFFIX_ALL += "${@' '.join(dt + d.getVar('EXTDT_SUFFIX_NOR') for dt in '${EXTERNAL_DEVICETREE_NOR_all}'.split())}"
EXTERNAL_DT_SUFFIX_ALL += "${@' '.join(dt + d.getVar('EXTDT_SUFFIX_NOR') for dt in '${EXTERNAL_DEVICETREE_NOR_mb}'.split())}"
EXTERNAL_DT_SUFFIX_ALL += "${@' '.join(dt + d.getVar('EXTDT_SUFFIX_NOR') for dt in '${EXTERNAL_DEVICETREE_NOR_mb_sb}'.split())}"
EXTERNAL_DT_SUFFIX_ALL += "${@' '.join(dt + d.getVar('EXTDT_SUFFIX_SDCARD') for dt in '${EXTERNAL_DEVICETREE_SDCARD}'.split())}"
EXTERNAL_DT_SUFFIX_ALL += "${@' '.join(dt + d.getVar('EXTDT_SUFFIX_SPINAND') for dt in '${EXTERNAL_DEVICETREE_SPINAND}'.split())}"
# Feed overall external device tree list with suffix (without duplicate)
EXTERNAL_DEVICETREE_SUFFIX:append = " ${@' '.join('%s' % dt_file for dt_file in list(dict.fromkeys((d.getVar('EXTERNAL_DT_SUFFIX_ALL') or '').split())))} "

# Default device tree list per storage types
STM32MP_DT_FILES_EMMC ??= ""
STM32MP_DT_FILES_NAND ??= ""
STM32MP_DT_FILES_NOR_all   ??= ""
STM32MP_DT_FILES_NOR_mb    ??= ""
STM32MP_DT_FILES_NOR_mb_sb ??= ""
STM32MP_DT_FILES_SDCARD ??= ""
STM32MP_DT_FILES_SPINAND ??= ""

STM32MP_DT_FILES_EMMC:append = " ${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', '${EXTERNAL_DEVICETREE_EMMC}', '', d)}"
STM32MP_DT_FILES_NAND:append = " ${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', '${EXTERNAL_DEVICETREE_NAND}', '', d)}"
STM32MP_DT_FILES_NOR_all:append   = " ${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', '${EXTERNAL_DEVICETREE_NOR_all}', '', d)}"
STM32MP_DT_FILES_NOR_mb:append    = " ${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', '${EXTERNAL_DEVICETREE_NOR_mb}', '', d)}"
STM32MP_DT_FILES_NOR_mb_sb:append = " ${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', '${EXTERNAL_DEVICETREE_NOR_mb_sb}', '', d)}"
STM32MP_DT_FILES_SDCARD:append = " ${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', '${EXTERNAL_DEVICETREE_SDCARD}', '', d)}"
STM32MP_DT_FILES_SPINAND:append = " ${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', '${EXTERNAL_DEVICETREE_SPINAND}', '', d)}"

# NOR configured with all partitions (main boot and filesytems)
STM32MP_DT_FILES_ALL_NOR += "${STM32MP_DT_FILES_NOR_all}"
# NOR configured with only main boot partitions
STM32MP_DT_FILES_ALL_NOR += "${STM32MP_DT_FILES_NOR_mb}"
# NOR configured with only main and secondary boot partitions
STM32MP_DT_FILES_ALL_NOR += "${STM32MP_DT_FILES_NOR_mb_sb}"
# Concatenate all device tree per storage for all partition configurations
STM32MP_DT_FILES_NOR ??= ""
STM32MP_DT_FILES_NOR:append =" ${@' '.join(dt_file for dt_file in list(dict.fromkeys((d.getVar('STM32MP_DT_FILES_ALL_NOR') or '').split())))} "

STM32MP_DT_FILES_ALL += "${STM32MP_DT_FILES_EMMC}"
STM32MP_DT_FILES_ALL += "${STM32MP_DT_FILES_NAND}"
STM32MP_DT_FILES_ALL += "${STM32MP_DT_FILES_NOR}"
STM32MP_DT_FILES_ALL += "${STM32MP_DT_FILES_SDCARD}"
STM32MP_DT_FILES_ALL += "${STM32MP_DT_FILES_SPINAND}"

# Set default supported device tree list without suffix (without duplicate)
STM32MP_DEVICETREE_DEFAULT = "${@' '.join('%s' % dt_file for dt_file in list(dict.fromkeys((d.getVar('STM32MP_DT_FILES_ALL') or '').split())))}"
# Set default supported device tree list with suffix expansion (without duplicate)
STM32MP_DEVICETREE_SUFFIX = "${@' '.join('%s' % dt_file for dt_file in list(dict.fromkeys((d.getVar('STM32MP_DT_FILES_ALL') or '').split())))}"
STM32MP_DEVICETREE_SUFFIX:remove = "${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', '${EXTERNAL_DEVICETREE}', '', d)}"
STM32MP_DEVICETREE_SUFFIX:append = " ${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', '${EXTERNAL_DEVICETREE_SUFFIX}', '', d)}"

STM32MP_DEVICETREE:append = " ${@bb.utils.contains('EXTDT_USE_SUFFIX', '1', '${STM32MP_DEVICETREE_SUFFIX}', '${STM32MP_DEVICETREE_DEFAULT}', d)}"

# Default for programmer:
STM32MP_DEVICETREE_PROGRAMMER_ENABLE ??= "1"
# Programmer external-dt init
EXTERNAL_DEVICETREE_PROGRAMMER ??= ""
# Programmer device tree list for all supported storage
STM32MP_DT_FILES_PROGRAMMER ??= "${STM32MP_DT_FILES_ALL}"
STM32MP_DT_FILES_PROGRAMMER:append = " ${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', '${EXTERNAL_DEVICETREE_PROGRAMMER}', '', d)}"
STM32MP_DEVICETREE_PROGRAMMER:append = " ${@' '.join('%s' % dt_file for dt_file in list(dict.fromkeys((d.getVar('STM32MP_DT_FILES_PROGRAMMER') or '').split())))} "
# Programmer device tree file suffix (for flashlayout file creation)
STM32MP_DEVICETREE_PROGRAMMER_SUFFIX ??= ""
# Programmer specific binary namming (for flashlayout file creation)
STM32MP_PROGRAMMER_SCHEME ??= "${@bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', 'programmer', '<DEVICE>', d)}"
# Set the serial devie to use to program storage: uart or usb
STMP32MP_PROGRAMMER_SERIAL ??= "usb"

# =========================================================================
# Machine specific packages
# =========================================================================
# Enable Software watchdog when sysvinit
MACHINE_EXTRA_RRECOMMENDS:append = " ${@bb.utils.contains('DISTRO_FEATURES','sysvinit',' watchdog ','',d)} "

# for devicetree OVERLAY
ST_DEVICE_OVERLAY_ADDONS ??= ""

# =========================================================================
# Flashlayout configuration
# =========================================================================
ENABLE_FLASHLAYOUT_CONFIG_FWUP ?= "${@bb.utils.contains('MACHINE_FEATURES', 'fw-update', '1', '0', d)}"

# =========================================================================
# Image
# =========================================================================
# For sparse, align on 4096b
IMAGE_ROOTFS_ALIGNMENT ?= "4"

# Enable licence summary and configure License content generation
ENABLE_IMAGE_LICENSE_SUMMARY ?= "1"
# Enable cve summary
ENABLE_IMAGE_CVE_SUMMARY ?= "1"

ST_MAIN_COMPONENTS:stm32mp1common = "tf-a-stm32mp, optee-os-stm32mp, u-boot-stm32mp, linux-stm32mp"
ST_MAIN_COMPONENTS:stm32mp2common = "tf-a-stm32mp, tf-m-stm32mp, optee-os-stm32mp, u-boot-stm32mp, linux-stm32mp"

# Default FSTYPES requested
WKS_IMAGE_FSTYPES ?= ""
IMAGE_FSTYPES ?= "${WKS_IMAGE_FSTYPES} tar.xz"

# Partitions configuration
ENABLE_PARTITIONS_IMAGE ??= "1"
IMAGE_CLASSES += "st-partitions-image"
IMAGE_CLASSES += "${@bb.utils.contains('ENABLE_PARTITIONS_IMAGE', '1', 'image_types-stsplitpartitions', '', d)}"
IMAGE_FSTYPES += "${@bb.utils.contains('ENABLE_PARTITIONS_IMAGE', '1', 'stsplitpartitions','', d)}"

# Append ext4 FSTYPES to default ones for emmc and sdcard volumes
IMAGE_FSTYPES += "${@bb.utils.contains_any('BOOTDEVICE_LABELS', 'emmc nor-sdcard nor-nor-sdcard nor-emmc nor-nor-emmc sdcard', 'ext4', '', d)}"
IMAGE_FSTYPES += "${@bb.utils.contains('STM32MP_MACHINE_HAVE_NAND_SDCARD', '1', 'ext4', '', d)}"

# Append ubi FSTYPES to default ones for nand volumes
IMAGE_FSTYPES += "${@bb.utils.contains('STM32MP_MACHINE_HAVE_NAND', '1', 'multiubi','', d)}"

IMAGE_FSTYPES += "${@bb.utils.contains('STM32MP_MACHINE_HAVE_NAND_SDCARD', '1', 'ubootenvmultiubi', '', d)}"

# Define specific EXT4 command line:
#   - Create minimal inode number (as it is done by default in image_types.bbclass)
#   - Add label name (maximum length of the volume label is 16 bytes)
#     So use IMAGE_NAME_SUFFIX name by removing the '.' and truncing to 16 caracters
#   - Deactivate metadata_csum and dir_index (hashed b-trees): update not supported
#     by U-Boot
EXTRA_IMAGECMD:ext4 = "-i 4096 -O ^metadata_csum,^dir_index \
        ${@'-L %s' % d.getVar('IMAGE_NAME_SUFFIX').replace('.', '', 1)[:16] if bb.utils.contains('ENABLE_PARTITIONS_IMAGE', '1', False, True, d) else ''} "

# Allow debug on the platform with gdb and openocd tools
EXTRA_IMAGEDEPENDS:append = " \
    gdb-cross-${TARGET_ARCH} \
    openocd-stm32mp-native \
    stm32wrapper4dbg-native \
    sdcard-raw-tools-native \
    "
MACHINE_EXTRA_RRECOMMENDS:append = " \
    gdb-stm32mp-addons \
"

# =========================================================================
# Signing configuration
# =========================================================================
ENCRYPT_ENABLE ?= "0"
ENCRYPT_COPRO_ENABLE ?= "${@bb.utils.contains('ENCRYPT_ENABLE', '1', '1', '0', d)}"
# encryption of coprocessor firmware is not supported on stm32mp1
ENCRYPT_COPRO_ENABLE:stm32mp1common = "0"
ENCRYPT_SUFFIX ?= "_Encrypted"

SIGN_ENABLE ?= "${@bb.utils.contains('ENCRYPT_ENABLE', '1', '1', '0', d)}"
SIGN_COPRO_ENABLE ?= "${@bb.utils.contains('SIGN_ENABLE', '1', '1', '0', d)}"
# signature of coprocessor firmware with hardware verification is not supported on stm32mp1
SIGN_COPRO_ENABLE:stm32mp1common = "0"
SIGN_SUFFIX ?= "_Signed"

# =========================================================================
# Debug trace
# =========================================================================
# acitvate/desactive the debug and trace on boot stage: tf-a, optee, u-boot and kernel
ST_DEBUG_TRACE ?= "1"

# =========================================================================
# Kernel
# =========================================================================
# Kernel image type
KERNEL_DEFAULT_NAME = "uImage"
KERNEL_DEFAULT_NAME:aarch64 = "Image.gz"

# arm32 bits
KERNEL_EFI_NAME = "${@bb.utils.contains('MACHINE_FEATURES', 'efi', 'zImage', 'uImage', d)}"
KERNEL_EFI_ALT_NAME = "${@bb.utils.contains('MACHINE_FEATURES', 'efi', 'uImage', 'zImage', d)}"
# arm64 bits
KERNEL_EFI_NAME:aarch64 = "${@bb.utils.contains('MACHINE_FEATURES', 'efi', 'Image', 'Image.gz', d)}"
KERNEL_EFI_ALT_NAME:aarch64 = "${@bb.utils.contains('MACHINE_FEATURES', 'efi', 'Image.gz', 'Image', d)}"

KERNEL_IMAGETYPE     = "${@bb.utils.contains('MACHINE_FEATURES', 'fit', 'fitImage', '${KERNEL_EFI_NAME}', d)}"
KERNEL_ALT_IMAGETYPE = " vmlinux "
KERNEL_ALT_IMAGETYPE =+ " ${@bb.utils.contains('MACHINE_FEATURES', 'fit', '${KERNEL_DEFAULT_NAME}', '${KERNEL_EFI_ALT_NAME}', d)} "

KERNEL_CLASSES = " ${@bb.utils.contains('MACHINE_FEATURES', 'fit', 'kernel-fitimage', 'kernel-uimage', d)} "

# Maxsize authorized for uncompressed kernel binary
# Define to null to skip kernel image size check
KERNEL_IMAGE_MAXSIZE ?= ""

# Enable kernel config file deploy
KERNEL_CONFIG_DEPLOY ?= "1"

# For fit usage
UBOOT_ENTRYPOINT = "0xC4000000"

# List of device tree to install
KERNEL_DEVICETREE ?= "${STM32MP_KERNEL_DEVICETREE}"
KERNEL_SUB_PATH = "st/"

STM32MP_KERNEL_DEVICETREE += "${@' '.join('${KERNEL_SUB_PATH}%s.dtb' % d for d in '${STM32MP_DEVICETREE}'.split())}"
STM32MP_KERNEL_DEVICETREE += "${@' '.join('${KERNEL_SUB_PATH}%s.dtb' % d for d in '${CUBE_M4_EXAMPLES_DT}'.split())}"
STM32MP_KERNEL_DEVICETREE += "${@' '.join('${KERNEL_SUB_PATH}%s.dtb' % d for d in '${LINUX_A7_EXAMPLES_DT}'.split())}"
STM32MP_KERNEL_DEVICETREE += "${@' '.join('${KERNEL_SUB_PATH}%s.dtb' % d for d in '${CUBE_M33_EXAMPLES_DT}'.split())}"
STM32MP_KERNEL_DEVICETREE += "${@' '.join('${KERNEL_SUB_PATH}%s.dtb' % d for d in '${LINUX_A35_EXAMPLES_DT}'.split())}"

# Set LOADADDR
# Set this address to 0xC2000040, which is 0xC2000000 + 0x40.
# 0xC2000000 is the memory address where U-Boot will copy from flash the file uImage and 0x40 is uImage header size (64Bytes).
# With this value, U-Boot will be able to execute in place the zImage contained in uImage.
ST_KERNEL_LOADADDR ?= "0xC2000040"
ST_KERNEL_LOADADDR:stm32mp1common ?= "0xC2000040"
ST_KERNEL_LOADADDR:stm32mp2common ?= "0x88008000"

# Define the devicetree for Linux A7 examples
LINUX_A7_EXAMPLES_DT ?= ""
# Define the devicetree for Linux A35 examples
LINUX_A35_EXAMPLES_DT ?= ""

ST_CMD_LINE_DEBUG_TRACE ?= "loglevel=1 quiet"

# package to install on boot directory
ST_KERNEL_DEFAULT_IMAGE ?= "${@bb.utils.contains('MACHINE_FEATURES', 'efi', 'kernel-image-efi', 'kernel-image-uimage', d)}"
ST_KERNEL_DEFAULT_IMAGE:aarch64 ?= "${@bb.utils.contains('MACHINE_FEATURES', 'efi', 'kernel-image-efi', 'kernel-image-image.gz', d)}"

ST_KERNEL_IMAGE_PACKAGE ??= "${@bb.utils.contains('MACHINE_FEATURES', 'fit', 'kernel-image-fitimage', '${ST_KERNEL_DEFAULT_IMAGE}', d)}"


# Variable to manage EXTLINUX
ST_EXTLINUX_PACKAGE ??= "${@bb.utils.contains('MACHINE_FEATURES', 'efi', 'stm32mp-efi', 'stm32mp-extlinux', d)}"
ST_EXTLINUX_SPLASH_PACKAGE ??= "${@bb.utils.contains('MACHINE_FEATURES', 'splashscreen', 'u-boot-stm32mp-splash', '', d)}"

# populate boot directory
MACHINE_ESSENTIAL_EXTRA_RDEPENDS += "${ST_KERNEL_IMAGE_PACKAGE}"
MACHINE_ESSENTIAL_EXTRA_RDEPENDS += "kernel-modules"
MACHINE_ESSENTIAL_EXTRA_RDEPENDS += "kernel-devicetree"
MACHINE_ESSENTIAL_EXTRA_RDEPENDS += "${ST_EXTLINUX_PACKAGE}"
MACHINE_ESSENTIAL_EXTRA_RDEPENDS += "${ST_EXTLINUX_SPLASH_PACKAGE}"
MACHINE_ESSENTIAL_EXTRA_RDEPENDS += "${ST_DEVICE_OVERLAY_ADDONS}"

# =========================================================================
# u-boot
# =========================================================================
# Define default U-Boot config
UBOOT_CONFIG ?= ""
# STM32mp13
UBOOT_CONFIG:append:stm32mp13common = " ${@bb.utils.contains_any('BOOTSCHEME_LABELS', 'optee opteemin', 'default_stm32mp13', '', d)} "
UBOOT_CONFIG:append:stm32mp13common = " ${@bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', 'programmer_stm32mp13', '', d)} "
UBOOT_CONFIG:append:stm32mp13common = " ${@bb.utils.contains_any('BOOTSCHEME_LABELS', 'fastboot fastboot-opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', 'fastboot-sdcard_stm32mp13', '', d), '', d)} "
UBOOT_CONFIG:append:stm32mp13common = " ${@bb.utils.contains_any('BOOTSCHEME_LABELS', 'fastboot fastboot-opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'fastboot-emmc_stm32mp13', '', d), '', d)} "

# STM32mp15
UBOOT_CONFIG:append:stm32mp15common = " ${@bb.utils.contains_any('BOOTSCHEME_LABELS', 'optee opteemin', 'default_stm32mp15', '', d)} "
UBOOT_CONFIG:append:stm32mp15common = " ${@bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', 'programmer_stm32mp15', '', d)} "
UBOOT_CONFIG:append:stm32mp15common = " ${@bb.utils.contains_any('BOOTSCHEME_LABELS', 'fastboot fastboot-opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', 'fastboot-sdcard_stm32mp15', '', d), '', d)} "
UBOOT_CONFIG:append:stm32mp15common = " ${@bb.utils.contains_any('BOOTSCHEME_LABELS', 'fastboot fastboot-opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'fastboot-emmc_stm32mp15', '', d), '', d)} "

# STM32mp21
UBOOT_CONFIG:append:stm32mp21common = " ${@bb.utils.contains_any('BOOTSCHEME_LABELS', 'optee opteemin', 'default_stm32mp21', '', d)} "
UBOOT_CONFIG:append:stm32mp21common = " ${@bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', 'programmer_stm32mp21', '', d)} "
UBOOT_CONFIG:append:stm32mp21common = " ${@bb.utils.contains_any('BOOTSCHEME_LABELS', 'fastboot fastboot-opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', 'fastboot-sdcard_stm32mp21', '', d), '', d)} "
UBOOT_CONFIG:append:stm32mp21common = " ${@bb.utils.contains_any('BOOTSCHEME_LABELS', 'fastboot fastboot-opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'fastboot-emmc_stm32mp21', '', d), '', d)} "
# STM32mp23
UBOOT_CONFIG:append:stm32mp23common = " ${@bb.utils.contains_any('BOOTSCHEME_LABELS', 'optee opteemin', 'default_stm32mp23', '', d)} "
UBOOT_CONFIG:append:stm32mp23common = " ${@bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', 'programmer_stm32mp23', '', d)} "
UBOOT_CONFIG:append:stm32mp23common = " ${@bb.utils.contains_any('BOOTSCHEME_LABELS', 'fastboot fastboot-opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', 'fastboot-sdcard_stm32mp23', '', d), '', d)} "
UBOOT_CONFIG:append:stm32mp23common = " ${@bb.utils.contains_any('BOOTSCHEME_LABELS', 'fastboot fastboot-opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'fastboot-emmc_stm32mp23', '', d), '', d)} "
# STM32mp25
UBOOT_CONFIG:append:stm32mp25common = " ${@bb.utils.contains_any('BOOTSCHEME_LABELS', 'optee opteemin', 'default_stm32mp25', '', d)} "
UBOOT_CONFIG:append:stm32mp25common = " ${@bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', 'programmer_stm32mp25', '', d)} "
UBOOT_CONFIG:append:stm32mp25common = " ${@bb.utils.contains_any('BOOTSCHEME_LABELS', 'fastboot fastboot-opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', 'fastboot-sdcard_stm32mp25', '', d), '', d)} "
UBOOT_CONFIG:append:stm32mp25common = " ${@bb.utils.contains_any('BOOTSCHEME_LABELS', 'fastboot fastboot-opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'fastboot-emmc_stm32mp25', '', d), '', d)} "

# Define U-boot splashscreen file naming
UBOOT_SPLASH_PORTRAIT_IMAGE = "splash_portrait"
UBOOT_SPLASH_LANDSCAPE_IMAGE = "splash_landscape"

PREFERRED_PROVIDER_u-boot-fw-utils ??= "libubootenv"

MACHINE_ESSENTIAL_EXTRA_RDEPENDS:append = " \
    u-boot-fw-config-stm32mp \
"

ST_UBOOT_DEBUG_TRACE ?= "${@bb.utils.contains('ST_DEBUG_TRACE', '1', '1', '0', d)}"

# =========================================================================
# trusted-firmware-a
# =========================================================================
# Add optionnaly trusted-firmware-a
EXTRA_IMAGEDEPENDS += "virtual/trusted-firmware-a"

# Configure the TF-A tool platform to use to generate the TF-A tools
TF_A_TOOLS_PLATFORM ?= "stm32mp2"

# Configure trusted-firmware-a build
# Manage specific config settings
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'optee-emmc', '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains_any('BOOTDEVICE_LABELS', 'nand-4-256-1024 nand-4-256-1024-sdcard', 'optee-nand', '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains_any('BOOTDEVICE_LABELS', 'nand-2-128-256 nand-2-128-256-sdcard nand-4-256-512 nand-4-256-512-sdcard', 'optee-spinand', '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains_any('BOOTDEVICE_LABELS', 'nand-custom nand-custom-sdcard', bb.utils.contains('CUSTOM_NAND_TYPE', 'fmc', 'optee-nand', 'optee-spinand', d), '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains('BOOTDEVICE_LABELS', 'nor', 'optee-nor', '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains('BOOTDEVICE_LABELS', 'nor-emmc', bb.utils.contains('MACHINE_FEATURES', 'm33td', 'optee-emmc', 'optee-nor', d), '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains('BOOTDEVICE_LABELS', 'nor-nor-emmc', 'optee-nor', '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains('BOOTDEVICE_LABELS', 'nor-sdcard', bb.utils.contains('MACHINE_FEATURES', 'm33td', 'optee-sdcard', 'optee-nor', d), '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains('BOOTDEVICE_LABELS', 'nor-nor-sdcard', 'optee-nor', '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', 'optee-sdcard', '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', 'optee-programmer-usb', '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', 'optee-programmer-uart', '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'opteemin-emmc', '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains_any('BOOTDEVICE_LABELS', 'nand-4-256-1024 nand-4-256-1024-sdcard', 'opteemin-nand', '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains_any('BOOTDEVICE_LABELS', 'nand-2-128-256 nand-2-128-256-sdcard nand-4-256-512 nand-4-256-512-sdcard', 'opteemin-spinand', '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains_any('BOOTDEVICE_LABELS', 'nand-custom nand-custom-sdcard', bb.utils.contains('CUSTOM_NAND_TYPE', 'fmc', 'opteemin-nand', 'opteemin-spinand', d), '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'nor', 'opteemin-nor', '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'nor-emmc', bb.utils.contains('MACHINE_FEATURES', 'm33td', 'opteemin-emmc', 'opteemin-nor', d), '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'nor-sdcard', bb.utils.contains('MACHINE_FEATURES', 'm33td', 'opteemin-sdcard', 'opteemin-nor', d), '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', 'opteemin-sdcard', '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', 'opteemin-programmer-usb', '', d), '', d)}"
TF_A_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', 'opteemin-programmer-uart', '', d), '', d)}"
# Set default supported trusted-firmware-a config list (without duplicate)
TF_A_CONFIG:append = " ${@' '.join('%s' % tfa_conf for tfa_conf in list(dict.fromkeys((d.getVar('TF_A_CONFIG_ALL') or '').split())))} "

# Append SSP config to TF_A_CONFIG
TF_A_SSP_ENABLE ?= "0"
TF_A_CONFIG += "${@bb.utils.contains('TF_A_SSP_ENABLE', '1', 'uart-ssp usb-ssp', '', d)}"

# Default configuration for signing trusted-firmware-a binary
TF_A_SIGN_SUFFIX ?= "${@bb.utils.contains('SIGN_ENABLE', '1', '${SIGN_SUFFIX}', '', d)}"
TF_A_ENCRYPT_SUFFIX ?= "${@bb.utils.contains('ENCRYPT_ENABLE', '1', '${ENCRYPT_SUFFIX}', '', d)}"

# Configure TF-A to build the metadata binary
TF_A_ENABLE_METADATA ?= "${@bb.utils.contains('MACHINE_FEATURES', 'fw-update', '1', '0', d)}"
TF_A_METADATA_BINARY ?= "metadata.bin"
TF_A_METADATA_AB_BINARY:EMMC ?= "metadata-ab-emmc.bin"
TF_A_METADATA_AB_BINARY:SDCARD ?= "metadata-ab-sdcard.bin"

# Configure the default MTD_START_OFFSET
TF_A_MTD_START_OFFSET_NAND    ?= "${@bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '0', '0x00100000', '0x00200000', d)}"
TF_A_MTD_START_OFFSET_NOR     ?= "${@bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '0', '0x00080000', '0x00100000', d)}"
TF_A_MTD_START_OFFSET_SPINAND ?= "${@bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '0', '0x00100000', '0x00200000', d)}"

ST_TF_A_DEBUG_TRACE ?= "${@bb.utils.contains('ST_DEBUG_TRACE', '1', '1', '0', d)}"

# =========================================================================
# trusted-firmware-m
# =========================================================================
# Default configuration for signing trusted-firmware-a binary
TF_M_SIGN_SUFFIX ?= "${@bb.utils.contains('SIGN_ENABLE', '1', '${SIGN_SUFFIX}', '', d)}"
TF_M_ENCRYPT_SUFFIX ?= "${@bb.utils.contains('ENCRYPT_ENABLE', '1', '${ENCRYPT_SUFFIX}', '', d)}"

# =========================================================================
# optee
# =========================================================================
## Map OPTEE configuration to device tree list
OPTEE_CONFIG += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', 'optee', '', d)}"
OPTEE_CONFIG += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', 'opteemin', '', d)}"

OPTEE_CONFIG += " ${@bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', bb.utils.contains('BOOTSCHEME_LABELS', 'optee', 'optee-programmer', '', d), '', d)} "
OPTEE_CONFIG += " ${@bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', 'opteemin-programmer', '', d), '', d)} "

ST_OPTEE_DEBUG_TRACE ?= "${@bb.utils.contains('ST_DEBUG_TRACE', '1', '1', '0', d)}"

# =========================================================================
# M33Project
# =========================================================================
# Add optionnaly the m33 firmware
EXTRA_IMAGEDEPENDS += "${@bb.utils.contains('MACHINE_FEATURES', 'm33td', 'm33fw-stm32mp2', '', d)}"

# Trustzone enable or not on M33 co-processor
# if Trustzone is enabled, that mean we need to have TF-M as dependency
# By default, it's enabled
M33PROJECT_TF_M_TRUSTZONE ??= "${@bb.utils.contains('MACHINE_FEATURES', 'm33td', '0', '1', d)}"

# Configure m33fw build
M33FW_CONFIG += "${@bb.utils.contains('MACHINE_FEATURES', 'm33copro', 'm33copro', '', d)}"
M33FW_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', bb.utils.contains('MACHINE_FEATURES', 'm33td', 'm33td_emmc', '', d), '', d)}"
M33FW_CONFIG += "${@bb.utils.contains_any('BOOTDEVICE_LABELS', 'nor-nor-emmc nor-nor-sdcard', bb.utils.contains('MACHINE_FEATURES', 'm33td', 'm33td_nor', '', d), '', d)}"
M33FW_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nor-emmc', bb.utils.contains('MACHINE_FEATURES', 'm33td', 'm33td_nor-emmc', '', d), '', d)}"
M33FW_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nor-sdcard', bb.utils.contains('MACHINE_FEATURES', 'm33td', 'm33td_nor-sdcard', '', d), '', d)}"
M33FW_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', bb.utils.contains('MACHINE_FEATURES', 'm33td', 'm33td_sdcard', '', d), '', d)}"

# -----------------------------------------------
# Define config for each M33FW_CONFIG
#   M33FW_CONFIG[config] ?= "<dt_conf>,<boot_suffix_m33>,<boot_suffix_a35>"
#
M33FW_CONFIG[m33copro]         ?= "${STM32MP_DEVICETREE}"
M33FW_CONFIG[m33td_emmc]       ?= "${STM32MP_DT_FILES_EMMC},emmc,emmc"
M33FW_CONFIG[m33td_nor]        ?= "${STM32MP_DT_FILES_NOR_mb_sb},nor,nor"
M33FW_CONFIG[m33td_nor-emmc]   ?= "${@' '.join(dt for dt in (d.getVar('STM32MP_DT_FILES_NOR_mb') or '').split() if dt in (d.getVar('STM32MP_DT_FILES_EMMC') or '').split())},nor,emmc"
M33FW_CONFIG[m33td_nor-sdcard] ?= "${@' '.join(dt for dt in (d.getVar('STM32MP_DT_FILES_NOR_mb') or '').split() if dt in (d.getVar('STM32MP_DT_FILES_SDCARD') or '').split())},nor,sdcard"
M33FW_CONFIG[m33td_sdcard]     ?= "${STM32MP_DT_FILES_SDCARD},sdcard,sdcard"

# Default configuration for signing trusted-firmware-a binary
M33FW_SIGN_SUFFIX ?= "${SIGN_SUFFIX}"

# =========================================================================
# fip
# =========================================================================
# Add fip binaries generation
EXTRA_IMAGEDEPENDS += "fip-stm32mp"

# Configure fip build
FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'optee-emmc', '', d), '', d)}"
FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains_any('BOOTDEVICE_LABELS', 'nand-2-128-256 nand-2-128-256-sdcard nand-4-256-512 nand-4-256-512-sdcard', 'optee-spinand', '', d), '', d)}"
FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains_any('BOOTDEVICE_LABELS', 'nand-4-256-1024 nand-4-256-1024-sdcard', 'optee-nand', '', d), '', d)}"
FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains_any('BOOTDEVICE_LABELS', 'nand-custom nand-custom-sdcard', bb.utils.contains('CUSTOM_NAND_TYPE', 'fmc', 'optee-nand', 'optee-spinand', d), '', d), '', d)}"
FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains('BOOTDEVICE_LABELS', 'nor', 'optee-nor', '', d), '', d)}"
FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains('BOOTDEVICE_LABELS', 'nor-emmc', bb.utils.contains('MACHINE_FEATURES', 'm33td', 'optee-emmc', 'optee-nor', d), '', d), '', d)}"
FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains('BOOTDEVICE_LABELS', 'nor-sdcard', bb.utils.contains('MACHINE_FEATURES', 'm33td', 'optee-sdcard', 'optee-nor', d), '', d), '', d)}"
FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', 'optee-sdcard', '', d), '', d)}"
FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', 'optee-programmer-${STMP32MP_PROGRAMMER_SERIAL}', '', d), '', d)}"

FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains('BOOTSCHEME_LABELS', 'fastboot', bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'fastboot-emmc', '', d), '', d), '', d)}"
FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains('BOOTSCHEME_LABELS', 'fastboot', bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', 'fastboot-sdcard', '', d), '', d), '', d)}"

FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', bb.utils.contains('MACHINE_FEATURES', 'm33td', bb.utils.contains_any('BOOTDEVICE_LABELS', 'nor-nor-emmc nor-nor-sdcard', 'optee-nor', '', d), '', d), '', d)}"

FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'opteemin-emmc', '', d), '', d)}"
FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains_any('BOOTDEVICE_LABELS', 'nand-2-128-256 nand-2-128-256-sdcard nand-4-256-512 nand-4-256-512-sdcard', 'opteemin-spinand', '', d), '', d)}"
FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains_any('BOOTDEVICE_LABELS', 'nand-4-256-1024 nand-4-256-1024-sdcard', 'opteemin-nand', '', d), '', d)}"
FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains_any('BOOTDEVICE_LABELS', 'nand-custom nand-custom-sdcard', bb.utils.contains('CUSTOM_NAND_TYPE', 'fmc', 'opteemin-nand', 'opteemin-spinand', d), '', d), '', d)}"
FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'nor', 'opteemin-nor', '', d), '', d)}"
FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'nor-emmc', bb.utils.contains('MACHINE_FEATURES', 'm33td', 'opteemin-emmc', 'opteemin-nor', d), '', d), '', d)}"
FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'nor-sdcard', bb.utils.contains('MACHINE_FEATURES', 'm33td', 'opteemin-sdcard', 'opteemin-nor', d), '', d), '', d)}"
FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', 'opteemin-sdcard', '', d), '', d)}"
FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', 'opteemin-programmer-${STMP32MP_PROGRAMMER_SERIAL}', '', d), '', d)}"

FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains('BOOTSCHEME_LABELS', 'fastboot-opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'fastboot-opteemin-emmc', '', d), '', d), '', d)}"
FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains('BOOTSCHEME_LABELS', 'fastboot-opteemin', bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', 'fastboot-opteemin-sdcard', '', d), '', d), '', d)}"

FIP_CONFIG_ALL += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'opteemin', bb.utils.contains('MACHINE_FEATURES', 'm33td', bb.utils.contains_any('BOOTDEVICE_LABELS', 'nor-nor-emmc nor-nor-sdcard', 'opteemin-nor', '', d), '', d), '', d)}"

# Set default supported fip config list (without duplicate)
FIP_CONFIG:append = " ${@' '.join('%s' % fip_conf for fip_conf in list(dict.fromkeys((d.getVar('FIP_CONFIG_ALL') or '').split())))} "

# Define fiptool wrapper name
FIPTOOL_WRAPPER ?= "fiptool-stm32mp.${MACHINE}"

# Manage specific config to sign FIP
FIP_SIGN_SUFFIX ?= "${@bb.utils.contains('SIGN_ENABLE', '1', '${SIGN_SUFFIX}', '', d)}"
FIP_ENCRYPT_SUFFIX ?= "${@bb.utils.contains('ENCRYPT_ENABLE', '1', '${ENCRYPT_SUFFIX}', '', d)}"

# Configure use of BL31
FIP_BL31_ENABLE = "0"
FIP_BL31_ENABLE:stm32mp2common = "1"

# -----------------------------------------------
# Define config for each FIP_CONFIG
#   FIP_CONFIG[config] ?= "<bl32_conf>,<dt_conf>,<dt_suffix,<search_conf>,<device_conf>"
#
FIP_CONFIG[optee-emmc]    ?= "optee,${STM32MP_DT_FILES_EMMC},${EXTDT_SUFFIX_EMMC},default:optee"
FIP_CONFIG[optee-nand]    ?= "optee,${STM32MP_DT_FILES_NAND},${EXTDT_SUFFIX_NAND},default:optee"
FIP_CONFIG[optee-nor]     ?= "optee,${STM32MP_DT_FILES_NOR},${EXTDT_SUFFIX_NOR},default:optee"
FIP_CONFIG[optee-sdcard]  ?= "optee,${STM32MP_DT_FILES_SDCARD},${EXTDT_SUFFIX_SDCARD},default:optee"
FIP_CONFIG[optee-spinand] ?= "optee,${STM32MP_DT_FILES_SPINAND},${EXTDT_SUFFIX_SPINAND},default:optee"
FIP_CONFIG[optee-programmer-uart] ?= "optee,${STM32MP_DEVICETREE_PROGRAMMER},,programmer:optee-programmer-uart:optee-programmer,uart"
FIP_CONFIG[optee-programmer-usb]  ?= "optee,${STM32MP_DEVICETREE_PROGRAMMER},,programmer:optee-programmer-usb:optee-programmer,usb"

FIP_CONFIG[fastboot-emmc]   ?= "optee,${@bb.utils.contains('MACHINE_FEATURES', 'm33td', '${STM32MP_DEVICETREE_PROGRAMMER}', '${STM32MP_DT_FILES_EMMC}', d)},,fastboot-emmc:optee-programmer,optee-programmer-usb"
FIP_CONFIG[fastboot-sdcard] ?= "optee,${@bb.utils.contains('MACHINE_FEATURES', 'm33td', '${STM32MP_DEVICETREE_PROGRAMMER}', '${STM32MP_DT_FILES_SDCARD}', d)},,fastboot-sdcard:optee-programmer,optee-programmer-usb"

FIP_CONFIG[opteemin-emmc]    ?= "optee,${STM32MP_DT_FILES_EMMC},${EXTDT_SUFFIX_EMMC},default:opteemin"
FIP_CONFIG[opteemin-nand]    ?= "optee,${STM32MP_DT_FILES_NAND},${EXTDT_SUFFIX_NAND},default:opteemin"
FIP_CONFIG[opteemin-nor]     ?= "optee,${STM32MP_DT_FILES_NOR},${EXTDT_SUFFIX_NOR},default:opteemin"
FIP_CONFIG[opteemin-sdcard]  ?= "optee,${STM32MP_DT_FILES_SDCARD},${EXTDT_SUFFIX_SDCARD},default:opteemin"
FIP_CONFIG[opteemin-spinand] ?= "optee,${STM32MP_DT_FILES_SPINAND},${EXTDT_SUFFIX_SPINAND},default:opteemin"
FIP_CONFIG[opteemin-programmer-uart] ?= "optee,${STM32MP_DEVICETREE_PROGRAMMER},,programmer:opteemin-programmer-uart:opteemin-programmer,uart"
FIP_CONFIG[opteemin-programmer-usb] ?= "optee,${STM32MP_DEVICETREE_PROGRAMMER},,programmer:opteemin-programmer-usb:opteemin-programmer,usb"

FIP_CONFIG[fastboot-opteemin-emmc]   ?= "optee,${STM32MP_DT_FILES_EMMC},,fastboot-emmc:opteemin-programmer,opteemin-emmc"
FIP_CONFIG[fastboot-opteemin-sdcard] ?= "optee,${STM32MP_DT_FILES_SDCARD},,fastboot-sdcard:opteemin-programmer,opteemin-sdcard"

# =========================================================================
# RPMSG
# =========================================================================
# install tools on file system
MACHINE_ESSENTIAL_EXTRA_RDEPENDS += " rpmsg-tools"
MACHINE_ESSENTIAL_EXTRA_RDEPENDS += " ${@bb.utils.contains('MACHINE_FEATURES', 'm33td', 'm33rpfwu', '', d)} "

# =========================================================================
# Xserver
# =========================================================================
#XSERVER ?= " \
#    xserver-xorg \
#    xserver-xorg-module-libint10 \
#    xf86-input-evdev \
#    xf86-video-modesetting \
#"

# =========================================================================
# Enable deploy of bootloader elf files
# =========================================================================
ELF_DEBUG_ENABLE = "1"

# =========================================================================
# M0 copro
# =========================================================================
# Define the devicetree for M0 examples
CUBE_M0_EXAMPLES_DT ?= ""

# =========================================================================
# M4 copro
# =========================================================================
# Define the devicetree for M4 examples
CUBE_M4_EXAMPLES_DT ?= ""

# Define the name of default copro firmware executed @boot time
# This name is cherry picked from list defined in m4projects-stm32mp1.bb
DEFAULT_COPRO_FIRMWARE = "OpenAMP_TTY_echo"

# =========================================================================
# M33 copro
# =========================================================================
# Define the devicetree for M33 examples
CUBE_M33_EXAMPLES_DT ?= ""

# =========================================================================
# Vendorfs libraries location configuration
# =========================================================================
# Configure use of vendorfs if enable to install libraries in vendorfs
ST_SPECIFIC_OUTPUT_LIBDIR = "${@bb.utils.contains('ST_VENDORFS', '1', '${STM32MP_VENDORFS_MOUNTPOINT}/lib', '${libdir}', d)}"

# =========================================================================
# WESTON HDMI configuration
# =========================================================================
# Configure the size screen use by weston with HDMI screen
# 720p: 1280x720
# 1080p: 1920x1080
WESTON_HDMI_MODE:stm32mp1common ?= "1280x720"
WESTON_HDMI_MODE:stm32mp2common ?= "1920x1080"

# =========================================================================
# EFI configuration
# =========================================================================
# For efi configuration
EFI_PROVIDER = ""
