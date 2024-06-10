#@DESCRIPTION: Common Machine configuration for STM32 systems

require conf/machine/include/st-machine-extlinux-config-stm32mp.inc
require conf/machine/include/st-machine-features-stm32mp.inc
require conf/machine/include/st-machine-flashlayout-stm32mp.inc
require conf/machine/include/st-machine-flashlayout-deleteall-stm32mp.inc
require conf/machine/include/st-machine-flashlayout-extensible-stm32mp.inc
require conf/machine/include/st-machine-image-partition-stm32mp.inc
require conf/machine/include/st-machine-storage-device-stm32mp.inc

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
# Take care of nativesdk override use case
STM32MP_SOC_NAME:append:class-nativesdk = " stm32mp15 "
STM32MP_SOC_NAME:append:class-nativesdk = " stm32mp13 "

STM32MP_ENCRYPT_SOC_NAME = ""
STM32MP_ENCRYPT_SOC_NAME:append:stm32mp13common = " stm32mp13 "

# =========================================================================
# boot scheme
# =========================================================================
# List of supported boot schemes
BOOTSCHEME_LABELS ??= "optee fastboot"

# =========================================================================
# boot device
# =========================================================================
# List of supported boot devices
BOOTDEVICE_LABELS ??= "emmc nand-4-256-1024 nor-sdcard sdcard"

# =========================================================================
# Machine settings
# =========================================================================
# Default machine feature
MACHINE_FEATURES = "usbhost usbgadget alsa screen ext2 pci"
MACHINE_FEATURES:append = " ${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', 'optee', '', d)} "
MACHINE_FEATURES:append = " tpm2 "

# Enable installation of INITRD image on bootfs side
MACHINE_FEATURES:append = " initrd "
# Remove InitRD package install from DISTRO_EXTRA_RRECOMMENDS to add it explicitly
# in our bootfs image (this avoid dependency loops as rootfs depends on bootfs...)
DISTRO_EXTRA_RRECOMMENDS:remove = " ${@bb.utils.contains('COMBINED_FEATURES', 'initrd', '${INITRD_PACKAGE}', '', d)} "

# Configure autoresize for any of the 'ext4' storage devices (through InitRD image)
MACHINE_FEATURES:append = " ${@bb.utils.contains_any('BOOTDEVICE_LABELS',  ['emmc', 'nor-sdcard', 'sdcard'], 'autoresize', '', d)} "

# Use FIP image for boot loaders
MACHINE_FEATURES:append = " fip"

# Enable firmware secure update feature
MACHINE_FEATURES:append = " fw-update"

# Default serial consoles (TTYs) to enable using getty
# Before kernel 4.18, serial console are ttyS3 but after is ttySTM0
SERIAL_CONSOLE = "115200 ttySTM0"
SERIAL_CONSOLES = "${@d.getVar('SERIAL_CONSOLE').replace(' ', ';')}"

# Don't include kernels in standard images
RDEPENDS:${KERNEL_PACKAGE_NAME}-base = ""

# Ship all kernel modules by default
MACHINE_EXTRA_RRECOMMENDS = " kernel-modules ${@bb.utils.contains('MACHINE_FEATURES', 'efi', 'kernel-imageefi', 'kernel-imagebootfs', d)}"

# Default device tree list per supported storages
STM32MP_DT_FILES_EMMC    ??= ""
STM32MP_DT_FILES_NAND    ??= ""
STM32MP_DT_FILES_NOR     ??= ""
STM32MP_DT_FILES_SDCARD  ??= ""
STM32MP_DT_FILES_SPINAND ??= ""

STM32MP_DT_FILES_ALL += "${STM32MP_DT_FILES_EMMC}"
STM32MP_DT_FILES_ALL += "${STM32MP_DT_FILES_NAND}"
STM32MP_DT_FILES_ALL += "${STM32MP_DT_FILES_NOR}"
STM32MP_DT_FILES_ALL += "${STM32MP_DT_FILES_SDCARD}"
STM32MP_DT_FILES_ALL += "${STM32MP_DT_FILES_SPINAND}"

# Set default supported device tree list (without duplicate)
STM32MP_DEVICETREE:append = " ${@' '.join('%s' % dt_file for dt_file in list(dict.fromkeys((d.getVar('STM32MP_DT_FILES_ALL') or '').split())))} "

# Default for programmer:
STM32MP_DEVICETREE_PROGRAMMER_ENABLE ??= "0"
STM32MP_DEVICETREE_PROGRAMMER_SUFFIX ??= ""

EXTERNAL_DEVICETREE_PROGRAMMER ??= ""

STM32MP_DEVICETREE_PROGRAMMER:append = " ${@bb.utils.contains('EXTERNAL_DT_ENABLED', '1', '${EXTERNAL_DEVICETREE_PROGRAMMER}', '', d)}"

STM32MP_PROGRAMMER_SCHEME ??= "${@bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', 'programmer', '<DEVICE>', d)}"

# =========================================================================
# Machine specific packages
# =========================================================================
# Enable Software watchdog when sysvinit
MACHINE_EXTRA_RRECOMMENDS:append = " ${@bb.utils.contains('DISTRO_FEATURES','sysvinit',' watchdog ','',d)} "

# =========================================================================
# Flashlayout configuration
# =========================================================================
#ENABLE_FLASHLAYOUT_CONFIG_FWUP ?= "${@bb.utils.contains('MACHINE_FEATURES', 'fw-update', '1', '0', d)}"
ENABLE_FLASHLAYOUT_CONFIG_FWUP ?= "1"
# Force ENABLE_FLASHLAYOUT_CONFIG_FWUP because only fw-update mapping for flashlayout is supported
ENABLE_FLASHLAYOUT_CONFIG_FWUP:stm32mp1common = "1"

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

# Partitions configuration
IMAGE_CLASSES += "st-partitions-image"

# UBI Configuration
IMAGE_CLASSES += "image_types-stubi"


# Define UBI labels to build
#   Naming rules for UBI partitions:
#      nand_<PageSize>_<BlockSize>
#      nor_<BlockSize>
#   Like that a same UBI partition can be used for severals NAND/NOR providers
MULTIUBI_BUILD += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-4-256-1024', 'nand_4_256_1024', '', d)}"
MULTIUBI_BUILD += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-2-128-128', 'nand_2_128_128', '', d)}"
MULTIUBI_BUILD += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-4-256-512', 'nand_4_256_512', '', d)}"

# UBI Args for NAND by default on MB1262 (Micron MT29F8G16ABACAH4) 1GB
# LEB = BLOCK_SIZE - (2 * page size): 256*1024 - (2*4096)
MKUBIFS_ARGS_nand_4_256_1024 = "--min-io-size 4096 --leb-size 253952 --max-leb-cnt 4096"
UBINIZE_ARGS_nand_4_256_1024 = "--min-io-size 4096 --peb-size 256KiB"
# Set extra size required for UBIFS volume size (KiB)
EXTRA_UBIFS_SIZE_nand_4_256_1024 = "2304"
# UBI Args for SPI NAND by default on MB1262 (Micron MT29F2G01ABA)
# LEB = BLOCK_SIZE - (2 * page size): 128*1024 - (2*2048)
MKUBIFS_ARGS_nand_2_128_128 = "--min-io-size 2048 --leb-size 126976 --max-leb-cnt 2048"
UBINIZE_ARGS_nand_2_128_128 = "--min-io-size 2048 --peb-size 128KiB"
# Set extra size required for UBIFS volume size (KiB)
EXTRA_UBIFS_SIZE_nand_2_128_128 = "1408"
# UBI Args for SPI NAND by default on MB1730 (TOSHIBA TC58CVG2S0HRAIJ) 512MB
# LEB = BLOCK_SIZE - (2 * page size): 256*1024 - (2*4096)
MKUBIFS_ARGS_nand_4_256_512 = "--min-io-size 4096 --leb-size 253952 --max-leb-cnt 4096"
UBINIZE_ARGS_nand_4_256_512 = "--min-io-size 4096 --peb-size 256KiB"
# Set extra size required for UBIFS volume size (KiB)
EXTRA_UBIFS_SIZE_nand_4_256_512 = "1408"

# For efi configuration
IMAGE_CLASSES += "image_types-stfat"
EFI_PROVIDER = ""

# Default FSTYPES requested
WKS_IMAGE_FSTYPES ?= ""
IMAGE_FSTYPES ?= "${WKS_IMAGE_FSTYPES} tar.xz"
# Append ext4 FSTYPES to default ones for emmc and sdcard volumes
IMAGE_FSTYPES += "${@bb.utils.contains_any('BOOTDEVICE_LABELS', 'emmc nor-sdcard sdcard', 'ext4', '', d)}"
# Append ubi FSTYPES to default ones for nand volumes
IMAGE_FSTYPES += "${@bb.utils.contains_any('BOOTDEVICE_LABELS', 'nand-4-256-1024 nand-2-128-128 nand-4-256-512', 'multiubi', '', d)}"

# Define specific EXT4 command line:
#   - Create minimal inode number (as it is done by default in image_types.bbclass)
#   - Add label name (maximum length of the volume label is 16 bytes)
#     So use IMAGE_NAME_SUFFIX name by removing the '.' and truncing to 16 caracters
#   - Deactivate metadata_csum and dir_index (hashed b-trees): update not supported
#     by U-Boot
EXTRA_IMAGECMD:ext4 = "-i 4096 -L ${@d.getVar('IMAGE_NAME_SUFFIX').replace('.', '', 1)[:16]} -O ^metadata_csum,^dir_index"

# Allow debug on the platform with gdb and openocd tools
EXTRA_IMAGEDEPENDS:append = " \
    gdb-cross-${TARGET_ARCH} \
    openocd-stm32mp-native \
    stm32wrapper4dbg-native \
    sdcard-raw-tools-native \
    "

# Make sure to provide all expected tools in SDK
ST_TOOLS_FOR_SDK = " \
    nativesdk-gcc-arm-none-eabi \
    nativesdk-binutils \
    nativesdk-openocd-stm32mp \
    nativesdk-stm32wrapper4dbg \
    nativesdk-sdcard-raw-tools \
    nativesdk-ncurses-libncursesw \
    "

# Add tools to build native application with SDK
# Currently mandatory for kernel and u-boot
ST_TOOLS_FOR_SDK:append = " \
    nativesdk-cpp-symlinks \
    nativesdk-gcc-symlinks \
    "

# For support of perl ansicolor
ST_TOOLS_FOR_SDK_PERL = " \
    nativesdk-perl-module-term-ansicolor \
    nativesdk-perl-module-encode \
    nativesdk-perl-module-encode-encoding \
    nativesdk-perl-module-encode-mime-header \
"

# For some scripts in kernel source code
ST_TOOLS_FOR_SDK_PERL:append = " \
    nativesdk-perl-module-file-spec-functions \
    nativesdk-perl-module-integer \
    nativesdk-perl-module-pod-text \
"

ST_TOOLS_FOR_SDK:append = " ${ST_TOOLS_FOR_SDK_PERL} "

# For support of python module for u-boot
ST_TOOLS_FOR_SDK:append = " \
    nativesdk-python3-dev \
    nativesdk-python3-setuptools \
    nativesdk-swig \
    nativesdk-dtc \
    nativesdk-flex \
    "

# For support of python module for optee-os
ST_TOOLS_FOR_SDK:append = " \
    nativesdk-python3-pyelftools \
    nativesdk-python3-pycryptodomex \
    nativesdk-python3-pillow \
    nativesdk-python3-cffi \
    nativesdk-python3-pycparser \
    nativesdk-python3-cryptography \
"

# For support of string convertion (iconv) in SDK
ST_TOOLS_FOR_SDK:append = " \
    nativesdk-glibc-gconv-utf-16 \
    nativesdk-glibc-gconv-utf-32 \
    "
# For support wayland-scanner in SDK
ST_TOOLS_FOR_SDK:append = " \
    nativesdk-wayland \
    "

# For Kernel yaml dependency
ST_TOOLS_FOR_SDK:remove = " \
    nativesdk-libyaml \
    nativesdk-libyaml-dev \
    "

# For CMSIS-SVD file parser with GDB
ST_TOOLS_FOR_SDK:append = " \
    nativesdk-svd-tools \
    "

# For Trusted Application development using OP-TEE TA SDK
ST_TOOLS_FOR_SDK:append = " \
    nativesdk-optee-sdk \
    "

# Fip tool
ST_TOOLS_FOR_SDK:append = " \
    nativesdk-${FIPTOOL_WRAPPER} \
    "

# Make sure to append mkimage to SDK for kernel uImage build
ST_DEPENDENCIES_BUILD_FOR_SDK = " \
    ${@bb.utils.contains('KERNEL_IMAGETYPE', 'uImage', 'nativesdk-u-boot-mkimage', '', d)} \
    ${@bb.utils.contains('KERNEL_ALT_IMAGETYPE', 'uImage', 'nativesdk-u-boot-mkimage', '', d)} \
  "
# Make sure to append openssl to SDK for kernel-module and scripts build
ST_DEPENDENCIES_BUILD_FOR_SDK:append = " nativesdk-openssl-dev "

# Make sure to append bison to SDK for u-boot build
ST_DEPENDENCIES_BUILD_FOR_SDK:append = " \
    ${@bb.utils.contains('EXTRA_IMAGEDEPENDS', 'u-boot-stm32mp', 'nativesdk-bison', '', d)} \
  "

# for populate_sdk, we will have all the tools
TOOLCHAIN_HOST_TASK:append = " ${ST_TOOLS_FOR_SDK} "
TOOLCHAIN_HOST_TASK:append = " ${ST_DEPENDENCIES_BUILD_FOR_SDK} "
# for populate_sdk_ext, the tools are not desired as mandatory tools (aka basic
# tools for devtool)
TOOLCHAIN_HOST_TASK:remove:task-populate-sdk-ext = " ${ST_TOOLS_FOR_SDK} "
TOOLCHAIN_HOST_TASK:remove:task-populate-sdk-ext = " ${ST_DEPENDENCIES_BUILD_FOR_SDK} "
# buildtools is used only by populate_sdk_ext
# populate_sdk_ext = buildtools + uninative + layer
# buildtools: sdk part of esdk (like sdk generated by populate_sdk)
# uninative: basic tools for devtool
TOOLCHAIN_HOST_TASK:remove:pn-buildtools-tarball = " ${ST_TOOLS_FOR_SDK_PERL} "

TOOLCHAIN_TARGET_TASK += " bash-dev libgomp-dev"
TOOLCHAIN_TARGET_TASK:remove:pn-buildtools-tarball = " bash-dev libgomp-dev"

# =========================================================================
# Signing configuration
# =========================================================================
ENCRYPT_ENABLE ?= "0"
ENCRYPT_SUFFIX ?= "_Encrypted"

SIGN_ENABLE ?= "${@bb.utils.contains('ENCRYPT_ENABLE', '1', '1', '0', d)}"
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
KERNEL_EFI_NAME = "${@bb.utils.contains('MACHINE_FEATURES', 'efi', 'zImage', 'uImage', d)}"
KERNEL_EFI_ALT_NAME = "${@bb.utils.contains('MACHINE_FEATURES', 'efi', 'uImage', 'zImage', d)}"

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
STM32MP_KERNEL_DEVICETREE += "${@' '.join('%s.dtb' % d for d in '${STM32MP_DEVICETREE}'.split())}"
STM32MP_KERNEL_DEVICETREE += "${@' '.join('%s.dtb' % d for d in '${CUBE_M4_EXAMPLES_DT}'.split())}"
STM32MP_KERNEL_DEVICETREE += "${@' '.join('%s.dtb' % d for d in '${LINUX_A7_EXAMPLES_DT}'.split())}"

# Set LOADADDR
# Set this address to 0xC2000040, which is 0xC2000000 + 0x40.
# 0xC2000000 is the memory address where U-Boot will copy from flash the file uImage and 0x40 is uImage header size (64Bytes).
# With this value, U-Boot will be able to execute in place the zImage contained in uImage.
ST_KERNEL_LOADADDR ?= "0xC2000040"

# Define the devicetree for Linux A7 examples
LINUX_A7_EXAMPLES_DT ?= ""

ST_CMD_LINE_DEBUG_TRACE ?= "loglevel=1 quiet"

# =========================================================================
# u-boot
# =========================================================================
# Define default U-Boot config
UBOOT_CONFIG ?= ""
# STM32mp15
UBOOT_CONFIG:append:stm32mp15common = " ${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', 'default_stm32mp15', '', d)} "
UBOOT_CONFIG:append:stm32mp15common = " ${@bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', 'programmer_stm32mp15', '', d)} "
UBOOT_CONFIG:append:stm32mp15common = " ${@bb.utils.contains('BOOTSCHEME_LABELS', 'fastboot', bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', 'fastboot-sdcard_stm32mp15', '', d), '', d)} "
UBOOT_CONFIG:append:stm32mp15common = " ${@bb.utils.contains('BOOTSCHEME_LABELS', 'fastboot', bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'fastboot-emmc_stm32mp15', '', d), '', d)} "
# STM32mp13

# Define U-boot splashscreen file naming
UBOOT_SPLASH_PORTRAIT_IMAGE = "splash_portrait"
UBOOT_SPLASH_LANDSCAPE_IMAGE = "splash_landscape"

# Enable MTDPART check for UBOOT_CONFIG
UBOOT_MTDPART_CHECK_ENABLE = ""

# Set U-Boot MTD partition configurations
UBOOT_MTDPART_NAND_BOOT    ?= "${@','.join(['%sk(%s)' % (align_size(d, 'NAND', l.split(',')[2], l.split(',')[4]), l.split(',')[1]) for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'nand-4-256-1024').split()])}"
UBOOT_MTDPART_NOR_BOOT     ?= "${@','.join(['%sk(%s)' % (align_size(d, 'NOR', l.split(',')[2], l.split(',')[4]), l.split(',')[1]) for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'nor').split()])}"
UBOOT_MTDPART_SPINAND_BOOT ?= "${@','.join(['%sk(%s)' % (align_size(d, 'SPINAND', l.split(',')[2], l.split(',')[4]), l.split(',')[1]) for l in d.getVarFlag('PARTITIONS_BOOTLOADER_CONFIG', 'nand-2-128-128').split()])}"

UBOOT_MTDPART_CHECK ?= "\
    ${@bb.utils.contains('BOOTDEVICE_LABELS',  'nand-4-256-1024', 'CONFIG_MTDPARTS_NAND0_BOOT=${UBOOT_MTDPART_NAND_BOOT}', '', d)} \
    ${@bb.utils.contains('BOOTDEVICE_LABELS', 'nor-sdcard', 'CONFIG_MTDPARTS_NOR0_BOOT=${UBOOT_MTDPART_NOR_BOOT}', '', d)} \
    ${@bb.utils.contains_any('BOOTDEVICE_LABELS', [ 'nand-2-128-128', 'nand-4-256-512'], 'CONFIG_MTDPARTS_SPINAND0_BOOT=${UBOOT_MTDPART_SPINAND_BOOT}', '', d)} \
    "

PREFERRED_PROVIDER_u-boot-fw-utils:stm32mp1common = "libubootenv"

MACHINE_EXTRA_RRECOMMENDS:append:stm32mp1common = " \
    u-boot-fw-config-stm32mp \
"

ST_UBOOT_DEBUG_TRACE ?= "${@bb.utils.contains('ST_DEBUG_TRACE', '1', '1', '0', d)}"
# =========================================================================
# trusted-firmware-a
# =========================================================================
# Add optionnaly trusted-firmware-a
EXTRA_IMAGEDEPENDS += "virtual/trusted-firmware-a"

# Configure trusted-firmware-a build
# Manage specific config settings
TF_A_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'optee-emmc', '', d)}"
TF_A_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-4-256-1024', 'optee-nand', '', d)}"
TF_A_CONFIG += "${@bb.utils.contains_any('BOOTDEVICE_LABELS', 'nor nor-sdcard', 'optee-nor', '', d)}"
TF_A_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', 'optee-sdcard', '', d)}"
TF_A_CONFIG += "${@bb.utils.contains_any('BOOTDEVICE_LABELS', 'nand-2-128-128 nand-4-256-512', 'optee-spinand', '', d)}"
TF_A_CONFIG += "${@bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', 'optee-programmer', '', d)}"
TF_A_CONFIG += "uart"
TF_A_CONFIG += "usb"

# Append SSP config to TF_A_CONFIG
TF_A_SSP_ENABLE ?= "0"
TF_A_CONFIG += "${@bb.utils.contains('TF_A_SSP_ENABLE', '1', 'uart-ssp usb-ssp', '', d)}"

# Default configuration for signing trusted-firmware-a binary
TF_A_SIGN_SUFFIX ?= "${@bb.utils.contains('SIGN_ENABLE', '1', '${SIGN_SUFFIX}', '', d)}"
TF_A_ENCRYPT_SUFFIX ?= "${@bb.utils.contains('ENCRYPT_ENABLE', '1', '${ENCRYPT_SUFFIX}', '', d)}"

# Configure TF-A to build the metadata binary
TF_A_ENABLE_METADATA ?= "${@bb.utils.contains('MACHINE_FEATURES', 'fw-update', '1', '0', d)}"
TF_A_METADATA_BINARY ?= "metadata.bin"

# Configure the default MTD_START_OFFSET
TF_A_MTD_START_OFFSET_NAND    ?= "${@bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '0', '0x00100000', '0x00200000', d)}"
TF_A_MTD_START_OFFSET_NOR     ?= "${@bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '0', '0x00080000', '0x00100000', d)}"
TF_A_MTD_START_OFFSET_SPINAND ?= "${@bb.utils.contains('ENABLE_FLASHLAYOUT_CONFIG_FWUP', '0', '0x00100000', '0x00200000', d)}"

ST_TF_A_DEBUG_TRACE ?= "${@bb.utils.contains('ST_DEBUG_TRACE', '1', '1', '0', d)}"
# =========================================================================
# optee
# =========================================================================
# Map OPTEE configuration to device tree list
OPTEE_CONF = "${STM32MP_DEVICETREE}"
OPTEE_CONF += "${@bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', '${STM32MP_DEVICETREE_PROGRAMMER}', '', d)}"

ST_OPTEE_DEBUG_TRACE ?= "${@bb.utils.contains('ST_DEBUG_TRACE', '1', '1', '0', d)}"
# =========================================================================
# fip
# =========================================================================

# Configure fip build
FIP_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'optee-emmc', '', d)}"
FIP_CONFIG += "${@bb.utils.contains_any('BOOTDEVICE_LABELS', 'nand-2-128-128 nand-4-256-512', 'optee-spinand', '', d)}"
FIP_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nand-4-256-1024', 'optee-nand', '', d)}"
FIP_CONFIG += "${@bb.utils.contains_any('BOOTDEVICE_LABELS', 'nor nor-sdcard', 'optee-nor', '', d)}"
FIP_CONFIG += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', 'optee-sdcard', '', d)}"
FIP_CONFIG += "${@bb.utils.contains('STM32MP_DEVICETREE_PROGRAMMER_ENABLE', '1', 'optee-programmer', '', d)}"

FIP_CONFIG += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'fastboot', bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'fastboot-emmc', '', d), '', d)}"
FIP_CONFIG += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'fastboot', bb.utils.contains('BOOTDEVICE_LABELS', 'sdcard', 'fastboot-sdcard', '', d), '', d)}"

# Define fiptool wrapper name
FIPTOOL_WRAPPER ?= "fiptool-stm32mp"

# Manage specific config to sign FIP
FIP_SIGN_SUFFIX ?= "${@bb.utils.contains('SIGN_ENABLE', '1', '${SIGN_SUFFIX}', '', d)}"
FIP_ENCRYPT_SUFFIX ?= "${@bb.utils.contains('ENCRYPT_ENABLE', '1', '${ENCRYPT_SUFFIX}', '', d)}"

# -----------------------------------------------
# Define config for each FIP_CONFIG
#   FIP_CONFIG[config] ?= "<bl32_conf>,<dt_conf>,<uboot_conf>,<device_conf>"
#
FIP_CONFIG[optee-emmc]    ?= "optee,${STM32MP_DT_FILES_EMMC},default"
FIP_CONFIG[optee-nand]    ?= "optee,${STM32MP_DT_FILES_NAND},default"
FIP_CONFIG[optee-nor]     ?= "optee,${STM32MP_DT_FILES_NOR},default"
FIP_CONFIG[optee-sdcard]  ?= "optee,${STM32MP_DT_FILES_SDCARD},default"
FIP_CONFIG[optee-spinand] ?= "optee,${STM32MP_DT_FILES_SPINAND},default"

FIP_CONFIG[optee-programmer] ?= "optee,${STM32MP_DEVICETREE_PROGRAMMER},programmer,usb"

FIP_CONFIG[fastboot-emmc]   ?= "optee,${STM32MP_DT_FILES_EMMC},fastboot,optee-emmc"
FIP_CONFIG[fastboot-sdcard] ?= "optee,${STM32MP_DT_FILES_SDCARD},fastboot,optee-sdcard"

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
# M4 copro
# =========================================================================
# Define the devicetree for M4 examples
CUBE_M4_EXAMPLES_DT ?= ""

# Define the name of default copro firmware executed @boot time
# This name is cherry picked from list defined in m4projects-stm32mp1.bb
DEFAULT_COPRO_FIRMWARE = "OpenAMP_TTY_echo"

# Define M4 example installation dir
M4_INSTALLDIR = "${STM32MP_USERFS_MOUNTPOINT}"
M4_PACKAGE_4USERFS = "${@bb.utils.contains('ST_USERFS','1','1','0',d)}"

# =========================================================================
# GCNANO userland configuration
# =========================================================================
# Configure use of vendorfs if enable to install gcnano libs
GCNANO_USERLAND_OUTPUT_LIBDIR = "${@bb.utils.contains('ST_VENDORFS', '1', '${STM32MP_VENDORFS_MOUNTPOINT}/lib', '${libdir}', d)}"

# =========================================================================
# =========================================================================
