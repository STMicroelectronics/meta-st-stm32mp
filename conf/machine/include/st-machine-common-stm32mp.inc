#@DESCRIPTION: Common Machine configuration for STM32 systems

require conf/machine/include/st-machine-extlinux-config-stm32mp.inc
require conf/machine/include/st-machine-features-stm32mp.inc
require conf/machine/include/st-machine-flashlayout-stm32mp.inc
require conf/machine/include/st-machine-flashlayout-deleteall-stm32mp.inc

# Define specific common machine name
MACHINEOVERRIDES .= ":stcommon"

# Define specific common layer name
MACHINEOVERRIDES .= ":stm32mpcommon"

# =========================================================================
# boot scheme
# =========================================================================
# List of supported boot schemes
BOOTSCHEME_LABELS ??= "basic trusted optee"

# =========================================================================
# Machine settings
# =========================================================================
# Default machine feature
MACHINE_FEATURES = "usbhost usbgadget alsa screen ext2"
MACHINE_FEATURES_append = " ${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', 'optee', '', d)} "
MACHINE_FEATURES_append = " watchdog "
MACHINE_FEATURES_append = " tpm2 "

# Default serial consoles (TTYs) to enable using getty
# Before kernel 4.18, serial console are ttyS3 but after is ttySTM0
SERIAL_CONSOLES = "115200;ttySTM0"
SERIAL_CONSOLE = "115200 ttySTM0"

# Don't include kernels in standard images
RDEPENDS_${KERNEL_PACKAGE_NAME}-base = ""

# Ship all kernel modules by default
MACHINE_EXTRA_RRECOMMENDS = " kernel-modules"

# Default device tree list supported per board
STM32MP_DT_FILES_DK ??= ""
STM32MP_DT_FILES_ED ??= ""
STM32MP_DT_FILES_EV ??= ""
# Set default supported device tree list
STM32MP_DEVICETREE_append = " ${STM32MP_DT_FILES_DK} "
STM32MP_DEVICETREE_append = " ${STM32MP_DT_FILES_ED} "
STM32MP_DEVICETREE_append = " ${STM32MP_DT_FILES_EV} "

# =========================================================================
# Machine specific packages
# =========================================================================
MACHINE_EXTRA_RRECOMMENDS_append = " ${@bb.utils.contains('MACHINE_FEATURES', 'wifi', 'linux-firmware-bcm43430', '', d)} "
MACHINE_EXTRA_RRECOMMENDS_append = " ${@bb.utils.contains('DISTRO_FEATURES','systemd',' wifi-suspend ','',d)} "
MACHINE_EXTRA_RRECOMMENDS_append = " m4projects-stm32mp1 "
MACHINE_EXTRA_RRECOMMENDS_append = " linux-examples-stm32mp1 "
MACHINE_EXTRA_RRECOMMENDS_append = " m4fwcoredump "
# Enable Software watchdog when sysvinit
# We enable it to be aligned with the activation within u-boot https://gerrit.st.com/#/c/102528
MACHINE_EXTRA_RRECOMMENDS_append = " ${@bb.utils.contains('DISTRO_FEATURES','sysvinit',' watchdog ','',d)} "

# =========================================================================
# Image
# =========================================================================
IMAGE_CLASSES += "image_types-stubi st-partitions-image"

# Define image to use for extra partitions
STM32MP_BOOTFS_IMAGE = "st-image-bootfs"
STM32MP_BOOTFS_LABEL = "boot"
STM32MP_BOOTFS_MOUNTPOINT_IMAGE = "/boot"
STM32MP_USERFS_IMAGE = "st-image-userfs"
STM32MP_USERFS_LABEL = "userfs"
STM32MP_USERFS_MOUNTPOINT_IMAGE = "/usr/local"
STM32MP_VENDORFS_IMAGE = "st-image-vendorfs"
STM32MP_VENDORFS_LABEL = "vendorfs"
STM32MP_VENDORFS_MOUNTPOINT_IMAGE = "/vendor"

# Define extra partition to build
PARTITIONS_IMAGE = "${STM32MP_BOOTFS_IMAGE} ${STM32MP_USERFS_IMAGE} ${STM32MP_VENDORFS_IMAGE}"
PARTITIONS_MOUNTPOINT_IMAGE = "${STM32MP_BOOTFS_MOUNTPOINT_IMAGE} ${STM32MP_USERFS_MOUNTPOINT_IMAGE} ${STM32MP_VENDORFS_MOUNTPOINT_IMAGE}"

# Enable licence summary and configure License content generation
ENABLE_IMAGE_LICENSE_SUMMARY = "1"
IMAGE_SUMMARY_LIST = "${STM32MP_BOOTFS_IMAGE}:${STM32MP_VENDORFS_IMAGE}:#IMAGE#:${STM32MP_USERFS_IMAGE}"

# Provide list of partition to mount
MOUNT_PARTITIONS_LIST  = "${STM32MP_BOOTFS_LABEL},${STM32MP_BOOTFS_MOUNTPOINT_IMAGE}"
MOUNT_PARTITIONS_LIST += "${STM32MP_USERFS_LABEL},${STM32MP_USERFS_MOUNTPOINT_IMAGE}"
MOUNT_PARTITIONS_LIST += "${STM32MP_VENDORFS_LABEL},${STM32MP_VENDORFS_MOUNTPOINT_IMAGE}"

# Define image partition size (supposed to be set as max size in image recipe)
BOOTFS_PARTITION_SIZE = "65536"
# New value proposed for rootfs is 768MB
ROOTFS_PARTITION_SIZE = "768432"
# If we consider the highest constraint is NAND size (so < 1GB)
# Boot binaries 4 MB max (with optee) + bootfs 64 MB max + rootfs 768 MB max + userfs size (4*32MB) < 1024 MB
USERFS_PARTITION_SIZE = "131072"
# New value proposed for vendorfs is 16MB
VENDORFS_PARTITION_SIZE = "16384"

# Define volume list for multivolume UBIFS
STM32MP_UBI_VOLUME += "${STM32MP_BOOTFS_IMAGE}-${DISTRO}-${MACHINE}:${BOOTFS_PARTITION_SIZE}"
STM32MP_UBI_VOLUME += "${IMAGE_LINK_NAME}:${ROOTFS_PARTITION_SIZE}"
STM32MP_UBI_VOLUME += "${STM32MP_VENDORFS_IMAGE}-${DISTRO}-${MACHINE}:${VENDORFS_PARTITION_SIZE}"
STM32MP_UBI_VOLUME += "${STM32MP_USERFS_IMAGE}-${DISTRO}-${MACHINE}:${USERFS_PARTITION_SIZE}"

# Set on machine side the max size for ROOTFS image to apply for default rootfs being built
# On other image partition such settings is directly done in image recipe
IMAGE_ROOTFS_MAXSIZE ?= "${ROOTFS_PARTITION_SIZE}"

# ST Naming rules partitions for UBI format are :
#   nand_<PageSize>_<BlockSize>
#   nor_<BlockSize>
# Like that a same UBI partition can be used for severals NAND/NOR providers

# UBI Args for NAND soldered by default on MB1262
# Micron MT29F8G16ABACAH4
# LEB = BLOCK_SIZE - (2 * page size): 256*1024 - (2*4096)
MKUBIFS_ARGS_nand_4_256 = "--min-io-size 4096 --leb-size 253952 --max-leb-cnt 4096 --space-fixup"
UBINIZE_ARGS_nand_4_256 = "--min-io-size 4096 --peb-size 256KiB"

# Define UBI labels to build
MULTIUBI_BUILD_append = " nand_4_256 "

# Set on machine side the UBI volume label for ROOTFS image to apply to reuse it
# on kernel command line to mount UBI file system
# On other image partition such settings is directly done in image recipe
UBI_VOLNAME ?= "rootfs"

# create minimal inode number (as it is done by default in image_types.bbclass)
# add naming of ext4 FS to be aligned with gpt... scripts
# For label name we are using IMAGE_NAME_SUFFIX we are removing "." and truncing to 16 caracters
# -L new-volume-label
# Set the volume label for the filesystem to new-volume-label.  The maximum length of the volume label is 16 bytes.
EXTRA_IMAGECMD_ext4 = "-i 4096 -L ${@d.getVar('IMAGE_NAME_SUFFIX').replace('.', '', 1)[:16]}"

# Default FSTYPES requested
IMAGE_FSTYPES ?= "tar.xz ext4"

# Allow debug on the platform with gdb and openocd tools
EXTRA_IMAGEDEPENDS_append = " \
    gdb-cross-arm \
    openocd-stm32mp-native \
    sdcard-raw-tools-native \
    "

# Make sure to provide all expected tools in SDK
ST_TOOLS_FOR_SDK = " \
    nativesdk-gcc-arm-none-eabi \
    nativesdk-binutils \
    nativesdk-openocd-stm32mp \
    nativesdk-sdcard-raw-tools \
    nativesdk-ncurses-libncursesw \
    nativesdk-perl-module-term-ansicolor \
    nativesdk-perl-module-encode \
    nativesdk-perl-module-encode-mime-header \
    "
# For support of string convertion (iconv) in SDK
ST_TOOLS_FOR_SDK_append = " \
    nativesdk-glibc-gconv-utf-16 \
    nativesdk-glibc-gconv-utf-32 \
    "
# For support wayland-scanner in SDK
ST_TOOLS_FOR_SDK_append = " \
    nativesdk-wayland \
    "

# for populate_sdk, we will have all the tools
TOOLCHAIN_HOST_TASK_append = " ${ST_TOOLS_FOR_SDK} "

# Make sure to append mkimage to SDK for kernel uImage build
TOOLCHAIN_HOST_TASK_append = " ${@bb.utils.contains('KERNEL_IMAGETYPE', 'uImage', 'nativesdk-u-boot-mkimage', '', d)} "
TOOLCHAIN_HOST_TASK_append = " ${@bb.utils.contains('KERNEL_ALT_IMAGETYPE', 'uImage', 'nativesdk-u-boot-mkimage', '', d)} "
# Make sure to append openssl to SDK for kernel-module and scripts build
TOOLCHAIN_HOST_TASK_append = " nativesdk-openssl-dev "

# Make sure to append bison to SDK for u-boot build
TOOLCHAIN_HOST_TASK_append = " ${@bb.utils.contains('EXTRA_IMAGEDEPENDS', 'u-boot-stm32mp', 'nativesdk-bison', '', d)} "

# for populate_sdk_ext, the tools are not desired as mandatory tools (aka basic
# tools for devtool)
TOOLCHAIN_HOST_TASK_remove_task-populate-sdk-ext = " ${ST_TOOLS_FOR_SDK} "

# buildtools and uninatve are used only by populate_sdk_ext
# populate_sdk_ext = buildtools + uninatve + layer
# buildtools: sdk part of esdk (like sdk generated by populate_sdk)
# uninative: basic tools for devtool
TOOLCHAIN_HOST_TASK_append_pn-buildtools-tarball = " ${ST_TOOLS_FOR_SDK} "
TOOLCHAIN_HOST_TASK_remove_pn-uninative-tarball = " ${ST_TOOLS_FOR_SDK} "

# =========================================================================
# Kernel
# =========================================================================
# Select kernel version
PREFERRED_PROVIDER_virtual/kernel = "linux-stm32mp"

# Kernel image type
KERNEL_IMAGETYPE     =  "uImage"
KERNEL_ALT_IMAGETYPE =  " Image "
KERNEL_ALT_IMAGETYPE =+ " vmlinux "
KERNEL_ALT_IMAGETYPE =+ " zImage "

# Maxsize authorized for uncompressed kernel binary
# Define to null to skip kernel image size check
KERNEL_IMAGE_MAXSIZE ?= ""

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

# =========================================================================
# u-boot
# =========================================================================
EXTRA_IMAGEDEPENDS += "u-boot-stm32mp"

# Define default U-Boot config
UBOOT_CONFIG += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'basic', 'basic', '', d)}"
UBOOT_CONFIG += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'trusted', 'trusted', '', d)}"
UBOOT_CONFIG += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', 'optee', '', d)}"
#Add this config until u-boot is able to flash with basic binary
UBOOT_CONFIG += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'basic', 'trusted', '', d)}"

# Define u-boot defconfig and binary to use for each UBOOT_CONFIG
UBOOT_CONFIG[basic]   = "stm32mp15_basic_defconfig,,u-boot.img"
UBOOT_CONFIG[trusted] = "stm32mp15_trusted_defconfig,,u-boot.stm32"
UBOOT_CONFIG[optee]   = "stm32mp15_optee_defconfig,,u-boot.stm32"

# List of U-Boot device tree to use
UBOOT_DEVICETREE = "${STM32MP_DEVICETREE}"

# Define u-boot splashscreen file naming
UBOOT_SPLASH_IMAGE = "splash"

# =========================================================================
# tf-a
# =========================================================================
# Finally we must compile tf-a in all cases as we need trusted binary to boot
#EXTRA_IMAGEDEPENDS += "${@bb.utils.contains_any('BOOTSCHEME_LABELS', 'optee trusted', 'tf-a-stm32mp', '', d)}"
EXTRA_IMAGEDEPENDS += "tf-a-stm32mp"

# Define default TF-A config
TF_A_CONFIG += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'trusted', 'trusted', '', d)}"
TF_A_CONFIG += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', 'optee', '', d)}"

#Add this config until tf-a is able to flash with basic binary
TF_A_CONFIG += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'basic', 'trusted', '', d)}"
#Add this config until optee is able to flash
TF_A_CONFIG += "${@bb.utils.contains('BOOTSCHEME_LABELS', 'optee', 'trusted', '', d)}"

# Define SECURE_PAYLOAD config to set for each TF_A_CONFIG
TF_A_CONFIG_optee   = "AARCH32_SP=optee"
TF_A_CONFIG_trusted = "AARCH32_SP=sp_min"

# List of TF-A device tree to use
TF_A_DEVICETREE = "${STM32MP_DEVICETREE}"

# =========================================================================
# optee
# =========================================================================
# Map OPTEE configuration to device tree list
OPTEE_CONF = "${STM32MP_DEVICETREE}"

# =========================================================================
# flashlayout
# =========================================================================
# Define bootscheme label to allow specific expansion for partition vars
FLASHLAYOUT_BOOTSCHEME_LABELS += "${BOOTSCHEME_LABELS}"
# Add specific scheme to provide flashlayout that will erase all storage devices
FLASHLAYOUT_BOOTSCHEME_LABELS += "deleteall"

# Default config labels supported
FLASHLAYOUT_CONFIG_LABELS ??= ""

# =========================================================================
# Xserver
# =========================================================================
XSERVER ?= " \
    xserver-xorg \
    xserver-xorg-module-libint10 \
    xf86-input-evdev \
    xf86-video-modesetting \
"

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

# =========================================================================
# GCNANO userland configuration
# =========================================================================
# Variable for using vendor directory instead of usr
GCNANO_USERLAND_VENDOR_DIR = "${STM32MP_VENDORFS_MOUNTPOINT_IMAGE}"
