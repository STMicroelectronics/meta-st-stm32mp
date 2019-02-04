Compilation of U-Boot:
1. Pre-requisite
2. Initialise cross-compilation via SDK
3. Prepare U-Boot source code
4. Management of U-Boot source code
5. Compile U-Boot source code
6. Update software on board

1. Pre-requisite:
-----------------
OpenSTLinux SDK must be installed.

For U-Boot build you need to install:
* libncurses and libncursesw dev package
    - Ubuntu: sudo apt-get install libncurses5-dev libncursesw5-dev
    - Fedora: sudo yum install ncurses-devel
* git:
    - Ubuntu: sudo apt-get install git-core gitk
    - Fedora: sudo yum install git

If you have never configured you git configuration:
    $> git config --global user.name "your_name"
    $> git config --global user.email "your_email@example.com"

2. Initialise cross-compilation via SDK:
---------------------------------------
* Source SDK environment:
    $> source <path to SDK>/environment-setup-cortexa9hf-neon-openstlinux_weston-linux-gnueabi

* To verify if you cross-compilation environment are put in place:
    $> set | grep CROSS
    CROSS_COMPILE=arm-openstlinux_weston-linux-gnueabi-

Warning: the environment are valid only on the shell session where you have
         sourced the sdk environment.

3. Prepare U-Boot source:
------------------------
If you have the tarball and the list of patch then you must extract the
tarball and apply the patch.
    $> tar xfz <U-Boot source>.tar.gz
    or
    $> tar xfj <U-Boot source>.tar.bz2
    or
    $> tar xfJ <U-Boot source>.tar.xz
    $> cd <directory to U-Boot source code>

NB: if there is no git management on source code and you would like to have a
git management on the code see section 4 [Management of U-Boot source code]
    if there is some patch, please apply it on source code
    $> for p in `ls -1 <path to patch>/*.patch`; do patch -p1 < $p; done

4. Management of U-Boot source code:
-----------------------------------
If you like to have a better management of change made on U-Boot source, you
can use git:
    $> tar xfz <U-Boot source>.tar.gz
    $> cd <directory of U-Boot source code>
    $> test -d .git || git init . && git add . && git commit -m "U-Boot source code" && git gc
    $> git checkout -b WORKING
    $> for p in `ls -1 <path to patch>/*.patch`; do git am $p; done

NB: you can use directly the source from the community:
    URL: git://git.denx.de/u-boot.git
    Branch: ##GIT_BRANCH##
    Revision: ##GIT_SRCREV##

    $> git clone git://git.denx.de/u-boot.git
    $> cd <directory of U-Boot source code>
    $> git checkout -b WORKING ##GIT_SRCREV##
    $> for p in `ls -1 <path to patch>/*.patch`; do git am $p; done

5. Compilation U-Boot source code:
--------------------------------
To compile U-Boot source code, first move to U-Boot source:
    $> cd <directory to U-Boot source code>

You can call the specific 'Makefile.sdk' provided to compile U-Boot:
  - Display 'Makefile.sdk' file default configuration and targets:
    $> make -f $PWD/../Makefile.sdk help
  - Compile default U-Boot configuration:
    $> make -f $PWD/../Makefile.sdk all

Default U-Boot configuration is done in 'Makefile.sdk' file through two specific
variables 'DEVICE_TREE' and 'UBOOT_CONFIGS':
  - 'DEVICE_TREE' is a list of device tree to build, using 'space' as separator.
    ex: DEVICE_TREE="<devicetree1> <devicetree2>"
  - 'UBOOT_CONFIGS' is a list of '<defconfig>,<type>,<binary>' configurations,
        <defconfig> is the u-boot defconfig to use to build
        <type> is the name append to u-boot binaries (ex: 'trusted', 'basic', etc)
        <binary> is the u-boot binary to export (ex: 'u-boot.bin', 'u-boot.stm32', etc)
    ex: UBOOT_CONFIGS="<defconfig1>,basic,u-boot.bin <defconfig1>,trusted,u-boot.stm32"
You can override the default U-Boot configuration if you specify these variables:
  - Compile default U-Boot configuration but applying specific devicetree(s):
    $> make -f $PWD/../Makefile.sdk all DEVICE_TREE="<devicetree1> <devicetree2>"
  - Compile for a specific U-Boot configuration:
    $> make -f $PWD/../Makefile.sdk all UBOOT_CONFIGS=<u-boot defconfig>,<u-boot type>,<u-boot binary>
  - Compile for a specific U-Boot configuration and applying specific devicetree(s):
    $> make -f $PWD/../Makefile.sdk all UBOOT_CONFIGS=<u-boot defconfig>,<u-boot type>,<u-boot binary> DEVICE_TREE="<devicetree1> <devicetree2>"

6. Update software on board:
----------------------------
6.1. partitioning of binaries:
-----------------------------
There are two possible configurations available:
- Basic configuration
- Trusted configuration

U-Boot build provides binaries for each configuration:
- Basic configuration: U-Boot SPL and U-Boot imgage (for FSBL and SSBL)
- Trusted configuration: U-Boot binary with ".stm32" extension (for SSBL)

Basic configuration:
On this configuration, we use U-Boot SPL as First Stage Boot Loader (FSBL) and
U-Boot as Second Stage Boot Loader (SSBL).
U-Boot SPL (u-boot-spl*.stm32) MUST be copied on a dedicated partition named "fsbl1"
U-Boot image (u-boot*.img) MUST be copied on a dedicated partition named "ssbl"

Trusted configuration:
On this configuration, U-Boot is associated to Trusted Firmware (TF-A) and only
U-Boot image is used as Second Stage Boot Loader (SSBL).
TF-A binary (tf-a-*.stm32) MUST be copied on a dedicated partition named "fsbl1"
U-boot binary (u-boot*.stm32) MUST be copied on a dedicated partition named "ssbl"

6.2. Update via SDCARD:
-----------------------
Basic configuration
* u-boot-spl*.stm32
  Copy the binary on the dedicated partition, on SDCARD/USB disk the partition
  "fsbl1" is the partition 1:
  - SDCARD: /dev/mmcblkXp1 (where X is the instance number)
  - SDCARD via USB reader: /dev/sdX1 (where X is the instance number)
  dd if=<U-Boot SPL file> of=/dev/<device partition> bs=1M conv=fdatasync

* u-boot*.img
  Copy the binary on the dedicated partition, on SDCARD/USB disk the partition
  "ssbl" is the partition 4:
  - SDCARD: /dev/mmcblkXp3 (where X is the instance number)
  - SDCARD via USB reader: /dev/sdX3 (where X is the instance number)
  dd if=<U-Boot image file> of=/dev/<device partition> bs=1M conv=fdatasync

Trusted configuration
* tf-a-*.stm32
  Copy the binary on the dedicated partition, on SDCARD/USB disk the partition
  "fsbl1" is the partition 1:
  - SDCARD: /dev/mmcblkXp1 (where X is the instance number)
  - SDCARD via USB reader: /dev/sdX1 (where X is the instance number)
  dd if=<TF-A binary file> of=/dev/<device partition> bs=1M conv=fdatasync

* u-boot*.stm32
  Copy the binary on the dedicated partition, on SDCARD/USB disk the partition
  "ssbl" is the partition 4:
  - SDCARD: /dev/mmcblkXp3 (where X is the instance number)
  - SDCARD via USB reader: /dev/sdX3 (where X is the instance number)
  dd if=<U-Boot stm32 binary file> of=/dev/<device partition> bs=1M conv=fdatasync

FAQ: to found the partition associated to a specific label, just plug the
SDCARD/USB disk on your PC and call the following command:
  $> ls -l /dev/disk/by-partlabel/
total 0
lrwxrwxrwx 1 root root 10 Jan 17 17:38 bootfs -> ../../mmcblk0p4
lrwxrwxrwx 1 root root 10 Jan 17 17:38 fsbl1 -> ../../mmcblk0p1     ➔ FSBL (TF-A)
lrwxrwxrwx 1 root root 10 Jan 17 17:38 fsbl2 -> ../../mmcblk0p2     ➔ FSBL backup (TF-A backup – same content as FSBL)
lrwxrwxrwx 1 root root 10 Jan 17 17:38 rootfs -> ../../mmcblk0p5
lrwxrwxrwx 1 root root 10 Jan 17 17:38 ssbl -> ../../mmcblk0p3      ➔ SSBL (U-Boot)
lrwxrwxrwx 1 root root 10 Jan 17 17:38 userfs -> ../../mmcblk0p6

6.3. Update via USB mass storage on U-Boot:
-------------------------------------------
* Plug the SDCARD on Board.
* Start the board and stop on U-Boot shell:
 Hit any key to stop autoboot: 0
 STM32MP>
* plug an USB cable between the PC and the board via USB OTG port.
* On U-Boot shell, call the usb mass storage functionality:
 STM32MP> ums 0 mmc 0
 ums <USB controller> <dev type: mmc|usb> <dev[:part]>
  ex.:
    ums 0 mmc 0
    ums 0 usb 0

* Follow section 6.2 to put U-Boot SPL binary and U-Boot binary (*.img or *.stm32)
  on SDCARD/USB disk.
