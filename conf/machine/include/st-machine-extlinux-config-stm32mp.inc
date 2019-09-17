#@DESCRIPTION: STM32MP machine extlinux file configuration

# Set configuration file to monitor
UBOOT_EXTLINUX_CONFIGURE_FILES_append = " ${STM32MP_BASE}/conf/machine/include/st-machine-extlinux-config-stm32mp.inc:True "

# Define the config flags to use to generate all extlinux targets
UBOOT_EXTLINUX_CONFIG_FLAGS += "${@bb.utils.contains('FLASHLAYOUT_CONFIG_LABELS', 'emmc', '%s' % bb.utils.contains('BOOTSCHEME_LABELS', 'optee', 'emmc emmc-optee', 'emmc', d), '', d)}"
UBOOT_EXTLINUX_CONFIG_FLAGS += "${@bb.utils.contains_any('FLASHLAYOUT_CONFIG_LABELS', [ 'nand-4-256', 'nor-nand-4-256' ], 'nand', '', d)}"
UBOOT_EXTLINUX_CONFIG_FLAGS += "${@bb.utils.contains('FLASHLAYOUT_CONFIG_LABELS', 'nor-emmc', 'nor-emmc', '', d)}"
UBOOT_EXTLINUX_CONFIG_FLAGS += "${@bb.utils.contains('FLASHLAYOUT_CONFIG_LABELS', 'nor-sdcard', 'nor-sdcard', '', d)}"
UBOOT_EXTLINUX_CONFIG_FLAGS += "${@bb.utils.contains('FLASHLAYOUT_CONFIG_LABELS', 'sdcard', '%s' % bb.utils.contains('BOOTSCHEME_LABELS', 'optee', 'sdcard sdcard-optee', 'sdcard', d), '', d)}"

# Provide the list of supported devicetree for each config flag
UBOOT_EXTLINUX_DEVICEFLAG_emmc          ?= "${STM32MP_DT_FILES_ED} ${STM32MP_DT_FILES_EV}"
UBOOT_EXTLINUX_DEVICEFLAG_emmc-optee    ?= "${STM32MP_DT_FILES_ED} ${STM32MP_DT_FILES_EV}"
UBOOT_EXTLINUX_DEVICEFLAG_nand          ?= "${STM32MP_DT_FILES_EV}"
UBOOT_EXTLINUX_DEVICEFLAG_nor-emmc      ?= "${STM32MP_DT_FILES_EV}"
UBOOT_EXTLINUX_DEVICEFLAG_nor-sdcard    ?= "${STM32MP_DT_FILES_EV}"
UBOOT_EXTLINUX_DEVICEFLAG_sdcard        ?= "${STM32MP_DT_FILES_DK} ${STM32MP_DT_FILES_ED} ${STM32MP_DT_FILES_EV}"
UBOOT_EXTLINUX_DEVICEFLAG_sdcard-optee  ?= "${STM32MP_DT_FILES_DK} ${STM32MP_DT_FILES_ED} ${STM32MP_DT_FILES_EV}"

# Set generic extlinux bootdevice variable to ease definition
EXTLINUX_BOOTDEVICE_EMMC      = "mmc1"
EXTLINUX_BOOTDEVICE_SDCARD    = "mmc0"
EXTLINUX_BOOTDEVICE_NORSDCARD = "nor0"
EXTLINUX_BOOTDEVICE_NOREMMC   = "nor0-mmc1"
EXTLINUX_BOOTDEVICE_NAND      = "nand0"

# Set generic extlinux root variable to ease definition
EXTLINUX_ROOT_EMMC         = "${@bb.utils.contains('ST_VENDORFS','1','root=/dev/mmcblk1p4','root=/dev/mmcblk1p3',d)}"
EXTLINUX_ROOT_EMMC_OPTEE   = "${@bb.utils.contains('ST_VENDORFS','1','root=/dev/mmcblk1p7','root=/dev/mmcblk1p6',d)}"
EXTLINUX_ROOT_NAND         = "ubi.mtd=UBI rootfstype=ubifs root=ubi0:rootfs"
EXTLINUX_ROOT_NOREMMC      = "${@bb.utils.contains('ST_VENDORFS','1','root=/dev/mmcblk1p3','root=/dev/mmcblk1p2',d)}"
EXTLINUX_ROOT_NORSDCARD    = "${@bb.utils.contains('ST_VENDORFS','1','root=/dev/mmcblk0p3','root=/dev/mmcblk0p2',d)}"
EXTLINUX_ROOT_SDCARD       = "${@bb.utils.contains('ST_VENDORFS','1','root=/dev/mmcblk0p6','root=/dev/mmcblk0p5',d)}"
EXTLINUX_ROOT_SDCARD_OPTEE = "${@bb.utils.contains('ST_VENDORFS','1','root=/dev/mmcblk0p9','root=/dev/mmcblk0p8',d)}"

# -----------------------------------------------------------------------------
# REMINDER: how 'exlinux.conf' files are built
#
# The 'extlinux.conf' files are generated under ${UBOOT_EXTLINUX_INSTALL_DIR}:
#   ${UBOOT_EXTLINUX_INSTALL_DIR}/${UBOOT_EXTLINUX_BOOTPREFIXES_${UBOOT_EXTLINUX_TARGETS}[0]}extlinux/extlinux.conf
#   ${UBOOT_EXTLINUX_INSTALL_DIR}/${UBOOT_EXTLINUX_BOOTPREFIXES_${UBOOT_EXTLINUX_TARGETS}[1]}extlinux/extlinux.conf
#   ...
#
# File content (${UBOOT_EXTLINUX_BOOTPREFIXES_${UBOOT_EXTLINUX_TARGETS}[0]}extlinux/exlinux.conf):
#   menu title Select the boot mode
#   TIMEOUT ${UBOOT_EXTLINUX_TIMEOUT}
#   DEFAULT ${UBOOT_EXTLINUX_DEFAULT_LABEL_${UBOOT_EXTLINUX_TARGETS}[0]}
#   LABEL ${UBOOT_EXTLINUX_LABELS_${UBOOT_EXTLINUX_TARGETS}[0]}[0]
#       KERNEL ${UBOOT_EXTLINUX_KERNEL}     < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_KERNEL_${IMAGE_UBOOT_EXTLINUX_LABELS}[0]}  >
#       FDT ${UBOOT_EXTLINUX_FDT}           < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_FDT_${IMAGE_UBOOT_EXTLINUX_LABELS}[0]}     >
#       APPEND ${UBOOT_EXTLINUX_ROOT}       < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_ROOT_${IMAGE_UBOOT_EXTLINUX_LABELS}[0]}    >
#   LABEL ${UBOOT_EXTLINUX_LABELS_${UBOOT_EXTLINUX_TARGETS}[0]}[1]
#       KERNEL ${UBOOT_EXTLINUX_KERNEL}     < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_KERNEL_${IMAGE_UBOOT_EXTLINUX_LABELS}[1]}  >
#       FDT ${UBOOT_EXTLINUX_FDT}           < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_FDT_${IMAGE_UBOOT_EXTLINUX_LABELS}[1]}     >
#       APPEND ${UBOOT_EXTLINUX_ROOT}       < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_ROOT_${IMAGE_UBOOT_EXTLINUX_LABELS}[1]}    >
#
# File content (${UBOOT_EXTLINUX_BOOTPREFIXES_${UBOOT_EXTLINUX_TARGETS}[0]}extlinux/exlinux.conf):
#   menu title Select the boot mode
#   TIMEOUT ${UBOOT_EXTLINUX_TIMEOUT}
#   DEFAULT ${UBOOT_EXTLINUX_DEFAULT_LABEL_${UBOOT_EXTLINUX_TARGETS}[1]}
#   LABEL ${UBOOT_EXTLINUX_LABELS_${UBOOT_EXTLINUX_TARGETS}[1]}[0]
#       KERNEL ${UBOOT_EXTLINUX_KERNEL}     < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_KERNEL_${IMAGE_UBOOT_EXTLINUX_LABELS}[0]}  >
#       FDT ${UBOOT_EXTLINUX_FDT}           < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_FDT_${IMAGE_UBOOT_EXTLINUX_LABELS}[0]}     >
#       APPEND ${UBOOT_EXTLINUX_ROOT}       < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_ROOT_${IMAGE_UBOOT_EXTLINUX_LABELS}[0]}    >
#   LABEL ${UBOOT_EXTLINUX_LABELS_${UBOOT_EXTLINUX_TARGETS}[1]}[1]
#       KERNEL ${UBOOT_EXTLINUX_KERNEL}     < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_KERNEL_${IMAGE_UBOOT_EXTLINUX_LABELS}[1]}  >
#       FDT ${UBOOT_EXTLINUX_FDT}           < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_FDT_${IMAGE_UBOOT_EXTLINUX_LABELS}[1]}     >
#       APPEND ${UBOOT_EXTLINUX_ROOT}       < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_ROOT_${IMAGE_UBOOT_EXTLINUX_LABELS}[1]}    >
# -----------------------------------------------------------------------------

# Set extlinux console for stm32mp machine
UBOOT_EXTLINUX_CONSOLE = "console=${@d.getVar('SERIAL_CONSOLE').split()[1]},${@d.getVar('SERIAL_CONSOLE').split()[0]}"

# -----------------------------------------------------------------------------
# DK1 configuration
# -----------------------------------------------------------------------------
# Define available targets to use
UBOOT_EXTLINUX_CONFIGURED_TARGETS += "mp157a-dk1_sdcard"
UBOOT_EXTLINUX_CONFIGURED_TARGETS += "mp157a-dk1_sdcard-optee"
# Define bootprefix for each target
UBOOT_EXTLINUX_BOOTPREFIXES_mp157a-dk1_sdcard       = "${EXTLINUX_BOOTDEVICE_SDCARD}_stm32mp157a-dk1_"
UBOOT_EXTLINUX_BOOTPREFIXES_mp157a-dk1_sdcard-optee = "${EXTLINUX_BOOTDEVICE_SDCARD}_stm32mp157a-dk1-optee_"
# Define labels for each target
UBOOT_EXTLINUX_LABELS_mp157a-dk1_sdcard       = "stm32mp157a-dk1-sdcard"
UBOOT_EXTLINUX_LABELS_mp157a-dk1_sdcard-optee = "stm32mp157a-dk1-sdcard-optee"
# Define default boot config for each target
UBOOT_EXTLINUX_DEFAULT_LABEL_mp157a-dk1_sdcard       ?= "stm32mp157a-dk1-sdcard"
UBOOT_EXTLINUX_DEFAULT_LABEL_mp157a-dk1_sdcard-optee ?= "stm32mp157a-dk1-sdcard-optee"
# Define FDT overrides for all labels
UBOOT_EXTLINUX_FDT_stm32mp157a-dk1-sdcard       = "/stm32mp157a-dk1.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157a-dk1-sdcard-optee = "/stm32mp157a-dk1.dtb"
# Define ROOT overrides for all labels
UBOOT_EXTLINUX_ROOT_stm32mp157a-dk1-sdcard       = "${EXTLINUX_ROOT_SDCARD}"
UBOOT_EXTLINUX_ROOT_stm32mp157a-dk1-sdcard-optee = "${EXTLINUX_ROOT_SDCARD_OPTEE}"

# -----------------------------------------------------------------------------
# DK2 configuration
# -----------------------------------------------------------------------------
# Define available targets to use
UBOOT_EXTLINUX_CONFIGURED_TARGETS += "mp157c-dk2_sdcard"
UBOOT_EXTLINUX_CONFIGURED_TARGETS += "mp157c-dk2_sdcard-optee"
# Define bootprefix for each target
UBOOT_EXTLINUX_BOOTPREFIXES_mp157c-dk2_sdcard       = "${EXTLINUX_BOOTDEVICE_SDCARD}_stm32mp157c-dk2_"
UBOOT_EXTLINUX_BOOTPREFIXES_mp157c-dk2_sdcard-optee = "${EXTLINUX_BOOTDEVICE_SDCARD}_stm32mp157c-dk2-optee_"
# Define labels for each target
UBOOT_EXTLINUX_LABELS_mp157c-dk2_sdcard       = "stm32mp157c-dk2-sdcard"
UBOOT_EXTLINUX_LABELS_mp157c-dk2_sdcard-optee = "stm32mp157c-dk2-sdcard-optee"
# Define default boot config for each target
UBOOT_EXTLINUX_DEFAULT_LABEL_mp157c-dk2_sdcard       ?= "stm32mp157c-dk2-sdcard"
UBOOT_EXTLINUX_DEFAULT_LABEL_mp157c-dk2_sdcard-optee ?= "stm32mp157c-dk2-sdcard-optee"
# Define FDT overrides for all labels
UBOOT_EXTLINUX_FDT_stm32mp157c-dk2-sdcard       = "/stm32mp157c-dk2.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-dk2-sdcard-optee = "/stm32mp157c-dk2.dtb"
# Define ROOT overrides for all labels
UBOOT_EXTLINUX_ROOT_stm32mp157c-dk2-sdcard       = "${EXTLINUX_ROOT_SDCARD}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-dk2-sdcard-optee = "${EXTLINUX_ROOT_SDCARD_OPTEE}"
# -----------------------------------------------------------------------------
# Append A7 examples labels for each target
UBOOT_EXTLINUX_LABELS_mp157c-dk2_sdcard       += "stm32mp157c-dk2-a7-examples-sdcard"
UBOOT_EXTLINUX_LABELS_mp157c-dk2_sdcard-optee += "stm32mp157c-dk2-a7-examples-sdcard-optee"
# Define FDT overrides for A7 labels
UBOOT_EXTLINUX_FDT_stm32mp157c-dk2-a7-examples-sdcard       = "/stm32mp157c-dk2-a7-examples.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-dk2-a7-examples-sdcard-optee = "/stm32mp157c-dk2-a7-examples.dtb"
# Define ROOT overrides for A7 labels
UBOOT_EXTLINUX_ROOT_stm32mp157c-dk2-a7-examples-sdcard       = "${EXTLINUX_ROOT_SDCARD}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-dk2-a7-examples-sdcard-optee = "${EXTLINUX_ROOT_SDCARD_OPTEE}"
# -----------------------------------------------------------------------------
# Append M4 examples labels for each target
UBOOT_EXTLINUX_LABELS_mp157c-dk2_sdcard       += "stm32mp157c-dk2-m4-examples-sdcard"
UBOOT_EXTLINUX_LABELS_mp157c-dk2_sdcard-optee += "stm32mp157c-dk2-m4-examples-sdcard-optee"
# Define FDT overrides for M4 labels
UBOOT_EXTLINUX_FDT_stm32mp157c-dk2-m4-examples-sdcard       = "/stm32mp157c-dk2-m4-examples.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-dk2-m4-examples-sdcard-optee = "/stm32mp157c-dk2-m4-examples.dtb"
# Define ROOT overrides for M4 labels
UBOOT_EXTLINUX_ROOT_stm32mp157c-dk2-m4-examples-sdcard       = "${EXTLINUX_ROOT_SDCARD}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-dk2-m4-examples-sdcard-optee = "${EXTLINUX_ROOT_SDCARD_OPTEE}"

# -----------------------------------------------------------------------------
# ED1 configuration
# -----------------------------------------------------------------------------
# Define available targets to use
UBOOT_EXTLINUX_CONFIGURED_TARGETS += "mp157c-ed1_emmc"
UBOOT_EXTLINUX_CONFIGURED_TARGETS += "mp157c-ed1_emmc-optee"
UBOOT_EXTLINUX_CONFIGURED_TARGETS += "mp157c-ed1_sdcard"
UBOOT_EXTLINUX_CONFIGURED_TARGETS += "mp157c-ed1_sdcard-optee"
# Define bootprefix for each target
UBOOT_EXTLINUX_BOOTPREFIXES_mp157c-ed1_emmc         = "${EXTLINUX_BOOTDEVICE_EMMC}_stm32mp157c-ed1_"
UBOOT_EXTLINUX_BOOTPREFIXES_mp157c-ed1_emmc-optee   = "${EXTLINUX_BOOTDEVICE_EMMC}_stm32mp157c-ed1-optee_"
UBOOT_EXTLINUX_BOOTPREFIXES_mp157c-ed1_sdcard       = "${EXTLINUX_BOOTDEVICE_SDCARD}_stm32mp157c-ed1_"
UBOOT_EXTLINUX_BOOTPREFIXES_mp157c-ed1_sdcard-optee = "${EXTLINUX_BOOTDEVICE_SDCARD}_stm32mp157c-ed1-optee_"
# Define labels for each target
UBOOT_EXTLINUX_LABELS_mp157c-ed1_emmc         = "stm32mp157c-ed1-emmc"
UBOOT_EXTLINUX_LABELS_mp157c-ed1_emmc-optee   = "stm32mp157c-ed1-emmc-optee"
UBOOT_EXTLINUX_LABELS_mp157c-ed1_sdcard       = "stm32mp157c-ed1-sdcard"
UBOOT_EXTLINUX_LABELS_mp157c-ed1_sdcard-optee = "stm32mp157c-ed1-sdcard-optee"
# Define default boot config for each target
UBOOT_EXTLINUX_DEFAULT_LABEL_mp157c-ed1_emmc         ?= "stm32mp157c-ed1-emmc"
UBOOT_EXTLINUX_DEFAULT_LABEL_mp157c-ed1_emmc-optee   ?= "stm32mp157c-ed1-emmc-optee"
UBOOT_EXTLINUX_DEFAULT_LABEL_mp157c-ed1_sdcard       ?= "stm32mp157c-ed1-sdcard"
UBOOT_EXTLINUX_DEFAULT_LABEL_mp157c-ed1_sdcard-optee ?= "stm32mp157c-ed1-sdcard-optee"
# Define FDT overrides for all labels
UBOOT_EXTLINUX_FDT_stm32mp157c-ed1-emmc         = "/stm32mp157c-ed1.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ed1-emmc-optee   = "/stm32mp157c-ed1.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ed1-sdcard       = "/stm32mp157c-ed1.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ed1-sdcard-optee = "/stm32mp157c-ed1.dtb"
# Define ROOT overrides for all labels
UBOOT_EXTLINUX_ROOT_stm32mp157c-ed1-emmc         = "${EXTLINUX_ROOT_EMMC}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ed1-emmc-optee   = "${EXTLINUX_ROOT_EMMC_OPTEE}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ed1-sdcard       = "${EXTLINUX_ROOT_SDCARD}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ed1-sdcard-optee = "${EXTLINUX_ROOT_SDCARD_OPTEE}"

# -----------------------------------------------------------------------------
# EV1 configuration
# -----------------------------------------------------------------------------
# Define available targets to use
UBOOT_EXTLINUX_CONFIGURED_TARGETS += "mp157c-ev1_emmc"
UBOOT_EXTLINUX_CONFIGURED_TARGETS += "mp157c-ev1_emmc-optee"
UBOOT_EXTLINUX_CONFIGURED_TARGETS += "mp157c-ev1_nand"
UBOOT_EXTLINUX_CONFIGURED_TARGETS += "mp157c-ev1_nor-sdcard"
UBOOT_EXTLINUX_CONFIGURED_TARGETS += "mp157c-ev1_nor-emmc"
UBOOT_EXTLINUX_CONFIGURED_TARGETS += "mp157c-ev1_sdcard"
UBOOT_EXTLINUX_CONFIGURED_TARGETS += "mp157c-ev1_sdcard-optee"
# Define bootprefix for each target
UBOOT_EXTLINUX_BOOTPREFIXES_mp157c-ev1_emmc         = "${EXTLINUX_BOOTDEVICE_EMMC}_stm32mp157c-ev1_"
UBOOT_EXTLINUX_BOOTPREFIXES_mp157c-ev1_emmc-optee   = "${EXTLINUX_BOOTDEVICE_EMMC}_stm32mp157c-ev1-optee_"
UBOOT_EXTLINUX_BOOTPREFIXES_mp157c-ev1_nand         = "${EXTLINUX_BOOTDEVICE_NAND}_stm32mp157c-ev1_"
UBOOT_EXTLINUX_BOOTPREFIXES_mp157c-ev1_nor-sdcard   = "${EXTLINUX_BOOTDEVICE_NORSDCARD}_stm32mp157c-ev1_"
UBOOT_EXTLINUX_BOOTPREFIXES_mp157c-ev1_nor-emmc     = "${EXTLINUX_BOOTDEVICE_NOREMMC}_stm32mp157c-ev1_"
UBOOT_EXTLINUX_BOOTPREFIXES_mp157c-ev1_sdcard       = "${EXTLINUX_BOOTDEVICE_SDCARD}_stm32mp157c-ev1_"
UBOOT_EXTLINUX_BOOTPREFIXES_mp157c-ev1_sdcard-optee = "${EXTLINUX_BOOTDEVICE_SDCARD}_stm32mp157c-ev1-optee_"
# Define labels for each target
UBOOT_EXTLINUX_LABELS_mp157c-ev1_emmc         = "stm32mp157c-ev1-emmc"
UBOOT_EXTLINUX_LABELS_mp157c-ev1_emmc-optee   = "stm32mp157c-ev1-emmc-optee"
UBOOT_EXTLINUX_LABELS_mp157c-ev1_nand         = "stm32mp157c-ev1-nand"
UBOOT_EXTLINUX_LABELS_mp157c-ev1_nor-sdcard   = "stm32mp157c-ev1-nor-sdcard"
UBOOT_EXTLINUX_LABELS_mp157c-ev1_nor-emmc     = "stm32mp157c-ev1-nor-emmc"
UBOOT_EXTLINUX_LABELS_mp157c-ev1_sdcard       = "stm32mp157c-ev1-sdcard"
UBOOT_EXTLINUX_LABELS_mp157c-ev1_sdcard-optee = "stm32mp157c-ev1-sdcard-optee"
# Define default boot config for each target
UBOOT_EXTLINUX_DEFAULT_LABEL_mp157c-ev1_emmc         ?= "stm32mp157c-ev1-emmc"
UBOOT_EXTLINUX_DEFAULT_LABEL_mp157c-ev1_emmc-optee   ?= "stm32mp157c-ev1-emmc-optee"
UBOOT_EXTLINUX_DEFAULT_LABEL_mp157c-ev1_nand         ?= "stm32mp157c-ev1-nand"
UBOOT_EXTLINUX_DEFAULT_LABEL_mp157c-ev1_nor-sdcard   ?= "stm32mp157c-ev1-nor-sdcard"
UBOOT_EXTLINUX_DEFAULT_LABEL_mp157c-ev1_nor-emmc     ?= "stm32mp157c-ev1-nor-emmc"
UBOOT_EXTLINUX_DEFAULT_LABEL_mp157c-ev1_sdcard       ?= "stm32mp157c-ev1-sdcard"
UBOOT_EXTLINUX_DEFAULT_LABEL_mp157c-ev1_sdcard-optee ?= "stm32mp157c-ev1-sdcard-optee"
# Define FDT overrides for all labels
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-emmc         = "/stm32mp157c-ev1.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-emmc-optee   = "/stm32mp157c-ev1.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-nand         = "/stm32mp157c-ev1.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-nor-sdcard   = "/stm32mp157c-ev1.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-nor-emmc     = "/stm32mp157c-ev1.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-sdcard       = "/stm32mp157c-ev1.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-sdcard-optee = "/stm32mp157c-ev1.dtb"
# Define ROOT overrides for all labels
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-emmc         = "${EXTLINUX_ROOT_EMMC}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-emmc-optee   = "${EXTLINUX_ROOT_EMMC_OPTEE}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-nand         = "${EXTLINUX_ROOT_NAND}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-nor-sdcard   = "${EXTLINUX_ROOT_NORSDCARD}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-nor-emmc     = "${EXTLINUX_ROOT_NOREMMC}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-sdcard       = "${EXTLINUX_ROOT_SDCARD}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-sdcard-optee = "${EXTLINUX_ROOT_SDCARD_OPTEE}"
# -----------------------------------------------------------------------------
# Append A7 examples labels for each target
UBOOT_EXTLINUX_LABELS_mp157c-ev1_emmc         += "stm32mp157c-ev1-a7-examples-emmc"
UBOOT_EXTLINUX_LABELS_mp157c-ev1_emmc-optee   += "stm32mp157c-ev1-a7-examples-emmc-optee"
UBOOT_EXTLINUX_LABELS_mp157c-ev1_nand         += "stm32mp157c-ev1-a7-examples-nand"
UBOOT_EXTLINUX_LABELS_mp157c-ev1_nor-sdcard   += "stm32mp157c-ev1-a7-examples-nor-sdcard"
UBOOT_EXTLINUX_LABELS_mp157c-ev1_nor-emmc     += "stm32mp157c-ev1-a7-examples-nor-emmc"
UBOOT_EXTLINUX_LABELS_mp157c-ev1_sdcard       += "stm32mp157c-ev1-a7-examples-sdcard"
UBOOT_EXTLINUX_LABELS_mp157c-ev1_sdcard-optee += "stm32mp157c-ev1-a7-examples-sdcard-optee"
# Define FDT overrides for A7 labels
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-a7-examples-emmc         = "/stm32mp157c-ev1-a7-examples.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-a7-examples-emmc-optee   = "/stm32mp157c-ev1-a7-examples.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-a7-examples-nand         = "/stm32mp157c-ev1-a7-examples.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-a7-examples-nor-sdcard   = "/stm32mp157c-ev1-a7-examples.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-a7-examples-nor-emmc     = "/stm32mp157c-ev1-a7-examples.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-a7-examples-sdcard       = "/stm32mp157c-ev1-a7-examples.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-a7-examples-sdcard-optee = "/stm32mp157c-ev1-a7-examples.dtb"
# Define ROOT overrides for A7 labels
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-a7-examples-emmc         = "${EXTLINUX_ROOT_EMMC}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-a7-examples-emmc-optee   = "${EXTLINUX_ROOT_EMMC_OPTEE}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-a7-examples-nand         = "${EXTLINUX_ROOT_NAND}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-a7-examples-nor-sdcard   = "${EXTLINUX_ROOT_NORSDCARD}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-a7-examples-nor-emmc     = "${EXTLINUX_ROOT_NOREMMC}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-a7-examples-sdcard       = "${EXTLINUX_ROOT_SDCARD}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-a7-examples-sdcard-optee = "${EXTLINUX_ROOT_SDCARD_OPTEE}"
# -----------------------------------------------------------------------------
# Append M4 examples labels for each target
UBOOT_EXTLINUX_LABELS_mp157c-ev1_emmc         += "stm32mp157c-ev1-m4-examples-emmc"
UBOOT_EXTLINUX_LABELS_mp157c-ev1_emmc-optee   += "stm32mp157c-ev1-m4-examples-emmc-optee"
UBOOT_EXTLINUX_LABELS_mp157c-ev1_nand         += "stm32mp157c-ev1-m4-examples-nand"
UBOOT_EXTLINUX_LABELS_mp157c-ev1_nor-sdcard   += "stm32mp157c-ev1-m4-examples-nor-sdcard"
UBOOT_EXTLINUX_LABELS_mp157c-ev1_nor-emmc     += "stm32mp157c-ev1-m4-examples-nor-emmc"
UBOOT_EXTLINUX_LABELS_mp157c-ev1_sdcard       += "stm32mp157c-ev1-m4-examples-sdcard"
UBOOT_EXTLINUX_LABELS_mp157c-ev1_sdcard-optee += "stm32mp157c-ev1-m4-examples-sdcard-optee"
# Define FDT overrides for M4 labels
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-m4-examples-emmc         = "/stm32mp157c-ev1-m4-examples.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-m4-examples-emmc-optee   = "/stm32mp157c-ev1-m4-examples.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-m4-examples-nand         = "/stm32mp157c-ev1-m4-examples.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-m4-examples-nor-sdcard   = "/stm32mp157c-ev1-m4-examples.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-m4-examples-nor-emmc     = "/stm32mp157c-ev1-m4-examples.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-m4-examples-sdcard       = "/stm32mp157c-ev1-m4-examples.dtb"
UBOOT_EXTLINUX_FDT_stm32mp157c-ev1-m4-examples-sdcard-optee = "/stm32mp157c-ev1-m4-examples.dtb"
# Define ROOT overrides for M4 labels
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-m4-examples-emmc         = "${EXTLINUX_ROOT_EMMC}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-m4-examples-emmc-optee   = "${EXTLINUX_ROOT_EMMC_OPTEE}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-m4-examples-nand         = "${EXTLINUX_ROOT_NAND}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-m4-examples-nor-sdcard   = "${EXTLINUX_ROOT_NORSDCARD}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-m4-examples-nor-emmc     = "${EXTLINUX_ROOT_NOREMMC}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-m4-examples-sdcard       = "${EXTLINUX_ROOT_SDCARD}"
UBOOT_EXTLINUX_ROOT_stm32mp157c-ev1-m4-examples-sdcard-optee = "${EXTLINUX_ROOT_SDCARD_OPTEE}"
