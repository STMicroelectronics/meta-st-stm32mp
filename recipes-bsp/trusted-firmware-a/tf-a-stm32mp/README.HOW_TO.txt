Compilation of TF-A (Trusted Firmware-A):
1. Pre-requisite
2. Initialise cross-compilation via SDK
3. Prepare tf-a source code
4. Management of tf-a source code
5. Compile tf-a source code
6. Update software on board

1. Pre-requisite:
-----------------
OpenSTLinux SDK must be installed.

For tf-a build you need to install:
- git:
    Ubuntu: sudo apt-get install git-core gitk
    Fedora: sudo yum install git

If you have never configured you git configuration:
    $ git config --global user.name "your_name"
    $ git config --global user.email "your_email@example.com"

2. Initialise cross-compilation via SDK:
---------------------------------------
Source SDK environment:
    $ source <path to SDK>/environment-setup-cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi

To verify if your cross-compilation environment have put in place:
    $ set | grep CROSS
    CROSS_COMPILE=arm-ostl-linux-gnueabi-

Warning: the environment are valid only on the shell session where you have
sourced the sdk environment.

3. Prepare tf-a source:
------------------------
If you have the tarball and the list of patch then you must extract the tarball
and apply the patch.
    $> tar xfz ##BP##-##PR##.tar.gz
A new directory containing tf-a standard source code will be created, go into it:
    $> cd ##BP##

NB: if there is no git management on source code and you would like to have a
git management on the code see section 4 [Management of tf-a source code]
    if there is some patch, please apply it on source code
    $> for p in `ls -1 ../*.patch`; do patch -p1 < $p; done

4. Management of tf-a source code:
-----------------------------------
If you like to have a better management of change made on tf-a source, you
can use git:
    $ cd <directory to tf-a source code>
    $ test -d .git || git init . && git add . && git commit -m "tf-a source code" && git gc
    $ git checkout -b WORKING
    $ for p in `ls -1 <path to patch>/*.patch`; do git am $p; done

NB: you can use directly the source from the community:
    URL: git://github.com/ARM-software/arm-trusted-firmware.git
    Branch: ##GIT_BRANCH##
    Revision: ##GIT_SRCREV##

    $ git clone git://github.com/ARM-software/arm-trusted-firmware.git -b ##GIT_BRANCH##
    $ cd <directory to tf-a source code>
    $ git checkout -b WORKING ##GIT_SRCREV##
    $ for p in `ls -1 <path to patch>/*.patch`; do git am $p; done

5. Build tf-a source code:
--------------------------------
To compile tf-a source code
    $> make -f $PWD/../Makefile.sdk all
or for a specific config :
    $ make -f $PWD/../Makefile.sdk TFA_DEVICETREE=stm32mp157c-ev1 TF_A_CONFIG=trusted ELF_DEBUG_ENABLE='1' all

NB: TFA_DEVICETREE flag must be set to switch to correct board configuration.

Files generated should be as follow:
 #> ../build/*/tf-a-*.stm32

6. Update software on board:
----------------------------
6.1. partitioning of binaries:
-----------------------------
TF-A build provide a binary named "tf-a-*.stm32" which MUST be copied on a
dedicated partition named "fsbl1"

6.2. Update via SDCARD:
-----------------------
Copy the binary (tf-a-*.stm32) on the dedicated partition, on SDCARD/USB disk
the partition "fsbl1" are the partition 1:
 - SDCARD: /dev/mmcblkXp1 (where X is the instance number)
 - SDCARD via USB reader: /dev/sdX1 (where X is the instance number)
    $ dd if=<tf-a binary> of=/dev/<device partition> bs=1M conv=fdatasync

FAQ: to found the partition associated to a specific label, just plug the
SDCARD/USB disk on your PC and call the following command:
    $ ls -l /dev/disk/by-partlabel/
total 0
lrwxrwxrwx 1 root root 10 Jan 17 17:38 bootfs -> ../../mmcblk0p4
lrwxrwxrwx 1 root root 10 Jan 17 17:38 fsbl1 -> ../../mmcblk0p1     ➔ FSBL (TF-A)
lrwxrwxrwx 1 root root 10 Jan 17 17:38 fsbl2 -> ../../mmcblk0p2     ➔ FSBL backup (TF-A backup – same content as FSBL)
lrwxrwxrwx 1 root root 10 Jan 17 17:38 rootfs -> ../../mmcblk0p5
lrwxrwxrwx 1 root root 10 Jan 17 17:38 ssbl -> ../../mmcblk0p3      ➔ SSBL (U-Boot)
lrwxrwxrwx 1 root root 10 Jan 17 17:38 userfs -> ../../mmcblk0p6

6.3. Update via USB mass storage on U-boot:
-------------------------------------------
* Plug the SDCARD on Board.
* Start the board and stop on U-boot shell:
 Hit any key to stop autoboot: 0
 STM32MP>
* plug an USB cable between the PC and the board via USB OTG port.
* On U-Boot shell, call the usb mass storage functionnality:
 STM32MP> ums 0 mmc 0
 ums <USB controller> <dev type: mmc|usb> <dev[:part]>
  ex.:
For SDCARD:      ums 0 mmc 0
For USB disk:    ums 0 usb 0

* Follow section 6.2 to put tf-a-*.stm32 on SDCARD/USB disk