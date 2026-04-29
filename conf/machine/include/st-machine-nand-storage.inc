#@DESCRIPTION: Common machine definition for NAND

STM32MP_MACHINE_HAVE_NAND ??= "${@bb.utils.contains_any('BOOTDEVICE_LABELS', \
                ['nand-4-256-1024', 'nand-4-256-512', 'nand-2-128-256', \
                 'nand-4-256-1024-sdcard', 'nand-4-256-512-sdcard', 'nand-2-128-256-sdcard', \
                 'nand-custom', 'nand-custom-sdcard'], '1', '0', d)}"

STM32MP_MACHINE_HAVE_NAND_ONLY ??= "${@bb.utils.contains_any('BOOTDEVICE_LABELS', \
                ['nand-4-256-1024', 'nand-4-256-512', 'nand-2-128-256', \
                 'nand-custom'], '1', '0', d)}"

STM32MP_MACHINE_HAVE_NAND_SDCARD ??= "${@bb.utils.contains_any('BOOTDEVICE_LABELS', \
                ['nand-4-256-1024-sdcard', 'nand-4-256-512-sdcard', 'nand-2-128-256-sdcard', \
                 'nand-custom-sdcard'], '1', '0', d)}"


# UBI Configuration
IMAGE_CLASSES += "image_types-stnand-uboot-ubi"

# Define UBI labels to build
#   Naming rules for UBI partitions:
#      nand_<PageSize>_<BlockSize>-<StorageSize>
#      nor_<BlockSize>
#   Like that a same UBI partition can be used for severals NAND/NOR providers
MULTIUBI_BUILD += "${@bb.utils.contains_any('BOOTDEVICE_LABELS', 'nand-4-256-1024 nand-4-256-1024-sdcard', 'nand_4_256_1024', '', d)}"
MULTIUBI_BUILD += "${@bb.utils.contains_any('BOOTDEVICE_LABELS', 'nand-2-128-256 nand-2-128-256-sdcard', 'nand_2_128_256', '', d)}"
MULTIUBI_BUILD += "${@bb.utils.contains_any('BOOTDEVICE_LABELS', 'nand-4-256-512 nand-4-256-512-sdcard', 'nand_4_256_512', '', d)}"
MULTIUBI_BUILD += "${@bb.utils.contains_any('BOOTDEVICE_LABELS', 'nand-custom nand-custom-sdcard', 'nand_custom', '', d)}"

# Nand supported:
# - nand-4-256-1024
# - nand-4-256-512
# - nand-2-128-25
# - nand_custom

# ------------------------------------------
# -------- nand-4-256-1024 -----------------
# UBI Args for NAND by default on MB1262 (Micron MT29F8G16ABACAH4) 1GB
# LEB = BLOCK_SIZE - (2 * page size): 256*1024 - (2*4096)
MKUBIFS_ARGS_nand_4_256_1024 = "--min-io-size 4096 --leb-size 253952 --max-leb-cnt 4096"
UBINIZE_ARGS_nand_4_256_1024 = "--min-io-size 4096 --peb-size 256KiB"
# Set extra size required for UBIFS volume size (KiB)
EXTRA_UBIFS_SIZE_nand_4_256_1024 = "2304"

# ------------------------------------------
# -------- nand-4-256-512 ------------------
# UBI Args for SPI NAND by default on MB1262 (Micron MT29F2G01ABA) 256MB
# LEB = BLOCK_SIZE - (2 * page size): 128*1024 - (2*2048)
MKUBIFS_ARGS_nand_2_128_256 = "--min-io-size 2048 --leb-size 126976 --max-leb-cnt 2048"
UBINIZE_ARGS_nand_2_128_256 = "--min-io-size 2048 --peb-size 128KiB"
# Set extra size required for UBIFS volume size (KiB)
EXTRA_UBIFS_SIZE_nand_2_128_256 = "1408"

# ------------------------------------------
# -------- nand-2-128-25  ------------------
# UBI Args for SPI NAND by default on MB1730 (TOSHIBA TC58CVG2S0HRAIJ) 512MB
# LEB = BLOCK_SIZE - (2 * page size): 256*1024 - (2*4096)
MKUBIFS_ARGS_nand_4_256_512 = "--min-io-size 4096 --leb-size 253952 --max-leb-cnt 4096"
UBINIZE_ARGS_nand_4_256_512 = "--min-io-size 4096 --peb-size 256KiB"
# Set extra size required for UBIFS volume size (KiB)
EXTRA_UBIFS_SIZE_nand_4_256_512 = "1408"

# ------------------------------------------
# -------- nand-custom    ------------------
# UBI Args for custom nand, Need to use CUSTUM_NAND_CONFIG = "<X>-<Y>-<Z>"
# which is <PageSize>_<BlockSize>-<StorageSize>
CUSTOM_NAND_CONFIG ??= "4-256-1024"
# type of nand, could be fmc or spi
CUSTOM_NAND_TYPE ??= "fmc"
# LEB = BLOCK_SIZE - (2 * page size): 256*1024 - (2*4096)
MKUBIFS_ARGS_nand_custom ??= "--min-io-size 4096 --leb-size 253952 --max-leb-cnt 4096"
UBINIZE_ARGS_nand_custom ??= "--min-io-size 4096 --peb-size 256KiB"
# Set extra size required for UBIFS volume size (KiB)
# For more explanation about this variable, read associated wiki page:
# https://wiki.st.com/stm32mpu/wiki/How_to_add_a_new_parallel_or_serial_NAND_flash_memory_device_in_Yocto
EXTRA_UBIFS_SIZE_nand_custom ??= "0"

