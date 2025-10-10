SUMMARY = "STM32MP2 Firmware examples for CM33"
LICENSE = "Apache-2.0 & MIT & BSD-3-Clause"

require recipes-extended/stm32mp2projects/mxxprojects-stm32mp2-common.inc
# to get tf-m parameters
require recipes-bsp/trusted-firmware-m/tf-m-stm32mp-config.inc
require recipes-extended/stm32mp2projects/m33projects.inc

PROJECTS_LIST_M33 = " \
    STM32MP257F-EV1/Demonstrations/USBPD_DRP_UCSI \
    STM32MP257F-EV1/Demonstrations/LowPower_SRAM_Demo \
    STM32MP257F-DK/Demonstrations/USBPD_DRP_UCSI \
    STM32MP235F-DK/Demonstrations/USBPD_DRP_UCSI \
    STM32MP215F-DK/Demonstrations/OpenAMP/OpenAMP_TTY_echo \
"

# WARNING: You MUST put only one project on DEFAULT_COPRO_FIRMWARE per board
# If there is several project defined for the same board while you MUST have issue at runtime
# (not the correct project could be executed).
DEFAULT_COPRO_FIRMWARE = "STM32MP257F-EV1/Demonstrations/USBPD_DRP_UCSI"
DEFAULT_COPRO_FIRMWARE += "STM32MP257F-DK/Demonstrations/USBPD_DRP_UCSI"
DEFAULT_COPRO_FIRMWARE += "STM32MP235F-DK/Demonstrations/USBPD_DRP_UCSI"

# Define default board reference for M33
M33_BOARDS += " STM32MP257F-EV1 STM32MP257F-DK STM32MP235F-DK "
M33_BOARDS += " STM32MP215F-DK "
