#-------------------------------------------------------------------------------
# Copyright (c) 2025 STMicroelectronics. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------
project(stm32cube VERSION 1.0 LANGUAGES CXX)

if (TARGET stm32cube_header)
    return()
endif()

set(STM32CUBE_CMSIS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS")
set(STM32CUBE_CMSIS_CORE_PATH "${STM32CUBE_CMSIS_PATH}/Core")
set(STM32CUBE_CMSIS_DEVICE_PATH "${STM32CUBE_CMSIS_PATH}/Device/ST/STM32MP2xx")
set(STM32CUBE_HAL_PATH ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32MP2xx_HAL_Driver)

add_library(stm32cube_header INTERFACE)

target_include_directories(stm32cube_header
    INTERFACE
        $<BUILD_INTERFACE:${STM32CUBE_CMSIS_PATH}/Include>
	$<BUILD_INTERFACE:${STM32CUBE_CMSIS_CORE_PATH}/Include>
	$<BUILD_INTERFACE:${STM32CUBE_CMSIS_DEVICE_PATH}/Include>
	$<BUILD_INTERFACE:${STM32CUBE_HAL_PATH}/Inc>
)

add_library(stm32cube::header ALIAS stm32cube_header)
