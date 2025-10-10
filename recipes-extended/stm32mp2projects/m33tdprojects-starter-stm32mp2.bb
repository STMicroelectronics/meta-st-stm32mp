SUMMARY = "STM32MP2 Firmware examples for CM33TD "
LICENSE = "Apache-2.0 & MIT & BSD-3-Clause"

require recipes-extended/stm32mp2projects/mxxprojects-stm32mp2-common.inc
# to get tf-m parameters
require recipes-bsp/trusted-firmware-m/tf-m-stm32mp-config.inc
require recipes-extended/stm32mp2projects/m33tdprojects.inc

# ---------------------------------
# Configure archiver use
# ---------------------------------
include ${@oe.utils.ifelse(d.getVar('ST_ARCHIVER_ENABLE') == '1', 'm33tdprojects-starter-stm32mp2-archiver.inc','')}

