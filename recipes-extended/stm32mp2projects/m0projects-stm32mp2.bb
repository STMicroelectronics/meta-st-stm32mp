SUMMARY = "STM32MP2 Firmware examples for CM0"
LICENSE = "Apache-2.0 & MIT & BSD-3-Clause"

require recipes-extended/stm32mp2projects/mxxprojects-stm32mp2-common.inc
require recipes-extended/stm32mp2projects/m0projects.inc

PROJECTS_LIST_M0 = " \
    STM32MP257F-EV1/Demonstrations/CM0PLUS_DEMO \
    STM32MP257F-DK/Demonstrations/CM0PLUS_DEMO \
"

# Define default board reference for M0
M0_BOARDS  += " STM32MP257F-EV1 "
M0_BOARDS  += " STM32MP257F-DK "

