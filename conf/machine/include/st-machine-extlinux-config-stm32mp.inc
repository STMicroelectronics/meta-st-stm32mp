#@DESCRIPTION: STM32MP machine extlinux file configuration

# Define extlinux console for stm32mp machine
UBOOT_EXTLINUX_CONSOLE = "console=${@d.getVar('SERIAL_CONSOLE').split()[1]},${@d.getVar('SERIAL_CONSOLE').split()[0]}"
# Define FIT option for extlinux file generation
UBOOT_EXTLINUX_FIT = "${@bb.utils.contains('MACHINE_FEATURES', 'fit', '1', '0', d)}"
# Define default FDTDIR for all configs
UBOOT_EXTLINUX_FDTDIR = "/"
# Define default boot config for all config
UBOOT_EXTLINUX_DEFAULT_LABEL ?= "OpenSTLinux"
# Define default INITRD for all configs
INITRD_IMAGE ??= ""
UBOOT_EXTLINUX_INITRD ?= "${@bb.utils.contains('COMBINED_FEATURES', 'initrd', '/${INITRD_IMAGE}', '', d)}"
# Define default SPLASH SCREEN
UBOOT_EXTLINUX_SPLASH ?= "${UBOOT_SPLASH_LANDSCAPE_IMAGE}"
UBOOT_EXTLINUX_SPLASH_stm32mp15 ?= "${UBOOT_SPLASH_PORTRAIT_IMAGE}"

UBOOT_EXTLINUX_KERNEL_ARGS ?= "rootwait rw"
UBOOT_EXTLINUX_KERNEL_ARGS_append += " ${@bb.utils.contains('ST_DEBUG_TRACE', '1', '', '${ST_CMD_LINE_DEBUG_TRACE}', d)} "

# -----------------------------------------------------------------------------
# Configurations
# -----------------------------------------------------------------------------
# Define additional targets to use
UBOOT_EXTLINUX_TARGETS += "${@bb.utils.contains_any('BOOTDEVICE_LABELS', [ 'sdcard', 'nor-sdcard' ], 'target-sdcard', '', d)}"
UBOOT_EXTLINUX_TARGETS += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'nor', 'target-nor', '', d)}"
UBOOT_EXTLINUX_TARGETS += "${@bb.utils.contains('BOOTDEVICE_LABELS', 'emmc', 'target-emmc', '', d)}"
UBOOT_EXTLINUX_TARGETS += "${@bb.utils.contains_any('BOOTDEVICE_LABELS', [ 'nand-4-256', 'nor-nand-4-256' ], 'target-nand', '', d)}"
# Define labels for each target
# (mandatory to allow specific var override if needed)
UBOOT_EXTLINUX_LABELS_target-sdcard += "rootfs"
UBOOT_EXTLINUX_LABELS_target-nor += "rootfs"
UBOOT_EXTLINUX_LABELS_target-emmc += "rootfs"
UBOOT_EXTLINUX_LABELS_target-nand += "rootfs"
# Define specific bootprefix for each target
UBOOT_EXTLINUX_BOOTPREFIXES_target-sdcard = "${DEVICE_SDCARD}_"
UBOOT_EXTLINUX_BOOTPREFIXES_target-nor = "${DEVICE_NOR}_"
UBOOT_EXTLINUX_BOOTPREFIXES_target-emmc = "${DEVICE_EMMC}_"
UBOOT_EXTLINUX_BOOTPREFIXES_target-nand = "${DEVICE_NAND}_"
# Define ROOT overrides for each target
UBOOT_EXTLINUX_ROOT_target-nand = "ubi.mtd=UBI rootfstype=ubifs root=ubi0:rootfs"
UBOOT_EXTLINUX_ROOT_target-sdcard = "root=PARTUUID=${DEVICE_PARTUUID_ROOTFS_SDCARD}"
UBOOT_EXTLINUX_ROOT_target-nor = "root=/dev/mtdblock6"
UBOOT_EXTLINUX_ROOT_target-emmc = "root=PARTUUID=${DEVICE_PARTUUID_ROOTFS_EMMC}"
# Define INITRD overrides for nand target
UBOOT_EXTLINUX_INITRD_target-nand = ""

# -----------------------------------------------------------------------------
# Configure default labels
# -----------------------------------------------------------------------------
# Define MENU_DESCRIPTION for rootfs label to default one to force selection
UBOOT_EXTLINUX_MENU_DESCRIPTION_rootfs = "OpenSTLinux"

# -----------------------------------------------------------------------------
# Append specific examples configs
# -----------------------------------------------------------------------------
# Enable UBOOT_EXTLINUX_TARGETS_EXTRA_CONFIG var that allows to configure
# additional target:
#   UBOOT_EXTLINUX_TARGETS_EXTRA_CONFIG = "extra-target"
# Then define new label(s) to append to the label list from current target
#   UBOOT_EXTLINUX_TARGETS_EXTRA_CONFIG[extra-target] = "newlabel1 newlabel2"
# This will create new 'extlinux.conf' file with name 'extra-target_extlinux.conf'
# that will use UBOOT_EXTLINUX_LABELS label list defined for current target plus
# the 'newlabel1' and 'newlabel2' labels.
# -----------------------------------------------------------------------------
# Define additional extra targets to consider
UBOOT_EXTLINUX_TARGETS_EXTRA_CONFIG_target-sdcard += "${STM32MP_DT_FILES_DK}"
UBOOT_EXTLINUX_TARGETS_EXTRA_CONFIG_target-sdcard += "${STM32MP_DT_FILES_ED}"
UBOOT_EXTLINUX_TARGETS_EXTRA_CONFIG_target-sdcard += "${STM32MP_DT_FILES_EV}"
UBOOT_EXTLINUX_TARGETS_EXTRA_CONFIG_target-emmc += "${STM32MP_DT_FILES_ED}"
UBOOT_EXTLINUX_TARGETS_EXTRA_CONFIG_target-emmc += "${STM32MP_DT_FILES_EV}"
UBOOT_EXTLINUX_TARGETS_EXTRA_CONFIG_target-nand += "${STM32MP_DT_FILES_ED}"
UBOOT_EXTLINUX_TARGETS_EXTRA_CONFIG_target-nand += "${STM32MP_DT_FILES_EV}"
UBOOT_EXTLINUX_TARGETS_EXTRA_CONFIG_target-nor += "${STM32MP_DT_FILES_ED}"
UBOOT_EXTLINUX_TARGETS_EXTRA_CONFIG_target-nor += "${STM32MP_DT_FILES_EV}"
# Define extra label configuration
UBOOT_EXTLINUX_TARGETS_EXTRA_CONFIG[stm32mp157c-dk2] += "stm32mp157c-dk2-a7-examples"
UBOOT_EXTLINUX_TARGETS_EXTRA_CONFIG[stm32mp157f-dk2] += "stm32mp157f-dk2-a7-examples"
UBOOT_EXTLINUX_TARGETS_EXTRA_CONFIG[stm32mp157c-ev1] += "stm32mp157c-ev1-a7-examples"
UBOOT_EXTLINUX_TARGETS_EXTRA_CONFIG[stm32mp157f-ev1] += "stm32mp157f-ev1-a7-examples"

UBOOT_EXTLINUX_TARGETS_EXTRA_CONFIG[stm32mp157c-dk2] += "stm32mp157c-dk2-m4-examples"
UBOOT_EXTLINUX_TARGETS_EXTRA_CONFIG[stm32mp157c-ev1] += "stm32mp157c-ev1-m4-examples"
UBOOT_EXTLINUX_TARGETS_EXTRA_CONFIG[stm32mp157f-dk2] += "stm32mp157f-dk2-m4-examples"
UBOOT_EXTLINUX_TARGETS_EXTRA_CONFIG[stm32mp157f-ev1] += "stm32mp157f-ev1-m4-examples"

# -----------------------------------------------------------------------------
# Configure A7 examples labels
# -----------------------------------------------------------------------------
# Define FDT overrides for new A7 labels
UBOOT_EXTLINUX_FDT_stm32mp157c-dk2-a7-examples = "/stm32mp157c-dk2-a7-examples.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-a7-examples = "/stm32mp157c-ev1-a7-examples.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157f-dk2-a7-examples = "/stm32mp157f-dk2-a7-examples.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157f-ev1-a7-examples = "/stm32mp157f-ev1-a7-examples.dtb"

# -----------------------------------------------------------------------------
# Configure M4 examples labels
# -----------------------------------------------------------------------------
# Define FDT overrides for new M4 labels
UBOOT_EXTLINUX_FDT_stm32mp157c-dk2-m4-examples = "/stm32mp157c-dk2-m4-examples.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-m4-examples = "/stm32mp157c-ev1-m4-examples.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157f-dk2-m4-examples = "/stm32mp157f-dk2-m4-examples.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157f-ev1-m4-examples = "/stm32mp157f-ev1-m4-examples.dtb"
