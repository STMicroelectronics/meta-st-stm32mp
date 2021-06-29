# Configure default mode (All supported device type)
TF_A_EXTRACONF_LEGACY += "STM32MP_SDMMC=1"
TF_A_EXTRACONF_LEGACY += "STM32MP_EMMC=1"
TF_A_EXTRACONF_LEGACY += "STM32MP_SPI_NOR=1"
TF_A_EXTRACONF_LEGACY += "STM32MP_RAW_NAND=1"
TF_A_EXTRACONF_LEGACY += "STM32MP_SPI_NAND=1"
TF_A_EXTRACONF_LEGACY += "STM32MP_USE_STM32IMAGE=1"

# Define config for each TF_A_CONFIG
#   TF_A_CONFIG[config] ?= "<list of devicetree>,<extra opt flags>,<binary basename (default: 'tf-a')>,<make target (default: 'all')>,<type of binary to deploy: [bl2 bl31 bl32 fwconfig] (default 'bl2')>"

TF_A_CONFIG[optee]   ?= "${STM32MP_DEVICETREE},AARCH32_SP=optee ${@bb.utils.contains('MACHINE_FEATURES', 'fip', '', '${TF_A_EXTRACONF_LEGACY}', d)},,${@bb.utils.contains('MACHINE_FEATURES', 'fip', bb.utils.contains('FIP_BL31_ENABLE', '1', 'bl31 dtbs', 'dtbs', d), '', d)},${@bb.utils.contains('MACHINE_FEATURES', 'fip', bb.utils.contains('FIP_BL31_ENABLE', '1', 'bl31 fwconfig', 'fwconfig', d), '', d)}"
TF_A_CONFIG[trusted] ?= "${STM32MP_DEVICETREE},AARCH32_SP=sp_min ${@bb.utils.contains('MACHINE_FEATURES', 'fip', '', '${TF_A_EXTRACONF_LEGACY}', d)},,${@bb.utils.contains('MACHINE_FEATURES', 'fip', 'bl32 dtbs', '', d)},${@bb.utils.contains('MACHINE_FEATURES', 'fip', 'bl32 fwconfig', '', d)}"

TF_A_CONFIG[serialboot] ?= "${STM32MP_DEVICETREE},AARCH32_SP=sp_min STM32MP_UART_PROGRAMMER=1 STM32MP_USB_PROGRAMMER=1 STM32MP_USE_STM32IMAGE=1"

TF_A_CONFIG[emmc]    ?= "${DEVICE_BOARD_ENABLE_EMMC},STM32MP_EMMC=1"
TF_A_CONFIG[nand]    ?= "${DEVICE_BOARD_ENABLE_NAND},STM32MP_RAW_NAND=1 ${@'STM32MP_FORCE_MTD_START_OFFSET=${TF_A_MTD_START_OFFSET_NAND}' if ${TF_A_MTD_START_OFFSET_NAND} else ''}"
TF_A_CONFIG[nor]     ?= "${DEVICE_BOARD_ENABLE_NOR},STM32MP_SPI_NOR=1 ${@'STM32MP_FORCE_MTD_START_OFFSET=${TF_A_MTD_START_OFFSET_NOR}' if ${TF_A_MTD_START_OFFSET_NOR} else ''}"
TF_A_CONFIG[sdcard]  ?= "${DEVICE_BOARD_ENABLE_SDCARD},STM32MP_SDMMC=1"
TF_A_CONFIG[spinand] ?= "${DEVICE_BOARD_ENABLE_SPINAND},STM32MP_SPI_NAND=1 ${@'STM32MP_FORCE_MTD_START_OFFSET=${TF_A_MTD_START_OFFSET_SPINAND}' if ${TF_A_MTD_START_OFFSET_SPINAND} else ''}"
TF_A_CONFIG[uart]    ?= "${STM32MP_DEVICETREE},STM32MP_UART_PROGRAMMER=1"
TF_A_CONFIG[usb]     ?= "${STM32MP_DEVICETREE},STM32MP_USB_PROGRAMMER=1"

# Define configuration for SSP
TF_A_CONFIG[uart-ssp] ?= "${STM32MP_DEVICETREE},STM32MP_UART_PROGRAMMER=1 STM32MP_SSP=1,tf-a-ssp"
TF_A_CONFIG[usb-ssp]  ?= "${STM32MP_DEVICETREE},STM32MP_USB_PROGRAMMER=1 STM32MP_SSP=1,tf-a-ssp"
