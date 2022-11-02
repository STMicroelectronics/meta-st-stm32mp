Compilation of kernel:
1. Pre-requisite
2. Initialize cross-compilation via SDK
3. Prepare kernel source code
4. Manage kernel source code
5. Configure kernel source code
6. Compile kernel source code
7. Update software on board
8. Update Starter Package with kernel compilation outputs

----------------
1. Pre-requisite
----------------
OpenSTLinux SDK must be installed.

For kernel build, you need to install:
- libncurses and libncursesw dev package libyaml-dev
    Ubuntu: sudo apt-get install libncurses5-dev libncursesw5-dev libyaml-dev
    Fedora: sudo yum install ncurses-devel libyaml-devel
- mkimage
    Ubuntu: sudo apt-get install u-boot-tools
    Fedora: sudo yum install u-boot-tools
- yaml (check dts)
    Ubuntu: sudo apt-get install libyaml-dev
    Fedora: sudo yum install libyaml-devel

Only if you like to have a git management of the code (see section 4
[Manage the kernel source code]):
- git:
    Ubuntu: sudo apt-get install git-core gitk
    Fedora: sudo yum install git

If you have never configured your git configuration, run the following commands:
    $ git config --global user.name "your_name"
    $ git config --global user.email "your_email@example.com"

---------------------------------------
2. Initialize cross-compilation via SDK
---------------------------------------
Source SDK environment:
    $ source <path to SDK>/environment-setup-cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi

To verify if your cross-compilation environment has been put in place correctly,
run the following command:
    $ set | grep CROSS
    CROSS_COMPILE=arm-ostl-linux-gnueabi-

Warning: the environment is valid only on the shell session where you have
sourced the SDK environment.

------------------------
3. Prepare kernel source
------------------------
If not already done, extract the sources from Developer Package tarball, for example:
    $ tar xf en.SOURCES-stm32mp1-*.tar.xz

In the kernel source directory (sources/*/##BP##-##PR##),
you have one kernel source tarball, the patches and one Makefile:
   - ##LINUX_TARNAME##.tar.xz
   - 00*.patch
   - Makefile.sdk

If you would like to have a git management for the source code move to
to section 4 [Management of kernel source code with GIT].

Otherwise, to manage kernel source code without git, you must extract the
tarball now and apply the patch:

    $> tar xf ##LINUX_TARNAME##.tar.xz
    $> cd ##LINUX_TARNAME##
    $> for p in `ls -1 ../*.patch`; do patch -p1 < $p; done

You can now move to section 5 [Configure kernel source code].

-------------------------------------
4. Manage kernel source code with GIT
-------------------------------------
If you like to have a better management of change made on kernel source, you
have 3 solutions to use git.

4.1 Get STMicroelectronics kernel source code from GitHub
---------------------------------------------------------
    URL: https://github.com/STMicroelectronics/linux.git
    Branch: ##ARCHIVER_ST_BRANCH##
    Revision: ##ARCHIVER_ST_REVISION##

    $ git clone https://github.com/STMicroelectronics/linux.git
    $ git checkout -b WORKING ##ARCHIVER_ST_REVISION##

4.2 Create Git from tarball
---------------------------
    $ cd <directory to kernel source code>
    $ test -d .git || git init . && git add . && git commit -m "new kernel" && git gc
    $ git checkout -b WORKING
    Apply patches:
    $ for p in `ls -1 ../*.patch`; do git am $p; done
  NB: this is the fastest way to get your kernel source code ready for development

4.3 Get Git from Linux kernel community and apply STMicroelectronics patches
---------------------------------------------------------------
    URL: https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
    Branch: ##ARCHIVER_COMMUNITY_BRANCH##
    Revision: ##ARCHIVER_COMMUNITY_REVISION##

    $ git clone https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
    $ cd linux-stable
    $ git checkout -b WORKING ##ARCHIVER_COMMUNITY_REVISION##
    $ for p in `ls -1 <path to patch>/*.patch`; do git am $p; done
  NB: this way is slightly slower than the tarball extraction but you get
      advantage of all git history.

4.4 Predefined kernel version vs auto-generated kernel version
--------------------------------------------------------------
If you are using git for managing your source code, kernel makefile get the SHA1
of current git and add it to kernel version number generated.
 ex.: 4.9.23-g3e866b0 (kernel version  + SHA1 of current git commit)
To bypass this auto-generation of kernel version number:
    $ cd <directory to kernel source code>
    $ echo "" > .scmversion
This file avoid to have a kernel version with SHA1:
- With scmversion file: 4.9.23
- Without scmversion file: 4.9.23-g3e866b0
This configuration allows to build new kernel from modified source code without
any issue when using the new kernel binary on target regarding any external
kernel module already available on target rootfs (as built without scmversion).

-------------------------------
5. Configure kernel source code
-------------------------------
There are two methods to configure and compile kernel source code:
- Inside kernel source tree directory
- Outside kernel source tree in a build directory
We highly preconized the build is a build directory method as:
- It avoids mixing files generated by the build with the source files inside
  same directories
- To remove all the files generated by the build, it's enough to remove the
  build directory
- You can build for different configurations in several build directories, e.g.:
    1) build in "build_1" for a first kernel configuration
    2) build in "build_2" for a second kernel configuration
    Then this leaves the 2 images available for tests

* Configure on a build directory (different of kernel source code directory)
    Here for example, build directory is located at the same level of kernel
    source code
    $ cd <directory to kernel source code>
    $> mkdir -p ../build
    $> make ARCH=arm O="$PWD/../build" multi_v7_defconfig fragment*.config

    If there are some fragments, apply them
    * manually one by one:
    $ scripts/kconfig/merge_config.sh -m -r -O $PWD/../build $PWD/../build/.config ../fragment-01-xxx.config
    $ scripts/kconfig/merge_config.sh -m -r -O $PWD/../build $PWD/../build/.config ../fragment-02-xxx.config
    ...
    $ yes '' | make ARCH=arm oldconfig O="$PWD/../build"
    * or, by loop:
    $> for f in `ls -1 ../fragment*.config`; do scripts/kconfig/merge_config.sh -m -r -O $PWD/../build $PWD/../build/.config $f; done
    $> yes '' | make ARCH=arm oldconfig O="$PWD/../build"

* Configure on the current source code directory
    $ cd <directory to kernel source code>
    $ make ARCH=arm multi_v7_defconfig fragment*.config

    If there are some fragments, apply them
    * manually one by one:
    $ scripts/kconfig/merge_config.sh -m -r .config ../fragment-01-xxxx.config
    $ scripts/kconfig/merge_config.sh -m -r .config ../fragment-02-xxxx.config
    ...
    $ yes '' | make oldconfig
    * or, by loop:
    $ for f in `ls -1 ../fragment*.config`; do scripts/kconfig/merge_config.sh -m -r .config $f; done
    $ yes '' | make ARCH=arm oldconfig

NB: Two types of fragments are provided:
    * official fragments (fragment-xxx.config)
    * optional fragments as example (optional-fragment-xxx.config) to add a
      feature not enabled by default.
    The order in which fragments are applied is determined by the number of the
    fragment filename (fragment-001, fragment-002, e.g.).
    Please pay special attention to the naming of your optional fragments to
    ensure you select the right features.

-----------------------------
6. Compile kernel source code
-----------------------------
You MUST compile from the directory on which the configuration has been done (i.e.
the directory which contains the '.config' file).

It's preconized to use the method with dedicated build directory for a better
managment of changes made on source code (as all build artifacts will be located
inside the dedicated build directory).

* Compile and install on a build directory (different of kernel source code directory)
    $ cd <directory to kernel source code>
    * Build kernel images (uImage and vmlinux) and device tree (dtbs)
    $> make ARCH=arm uImage vmlinux dtbs LOADADDR=0xC2000040 O="$PWD/../build"
    * Build kernel module
    $> make ARCH=arm modules O="$PWD/../build"
    * Generate output build artifacts
    $> make ARCH=arm INSTALL_MOD_PATH="$PWD/../build/install_artifact" modules_install O="$PWD/../build"
    $> mkdir -p $PWD/../build/install_artifact/boot/
    $> cp $PWD/../build/arch/arm/boot/uImage $PWD/../build/install_artifact/boot/
    $> cp $PWD/../build/arch/arm/boot/dts/st*.dtb $PWD/../build/install_artifact/boot/

    or

    $ cd <build directory>
    * Build kernel images (uImage and vmlinux) and device tree (dtbs)
    $ make ARCH=arm uImage vmlinux dtbs LOADADDR=0xC2000040
    * Build kernel module
    $ make ARCH=arm modules
    * Generate output build artifacts
    $ make ARCH=arm INSTALL_MOD_PATH="$PWD/../build/install_artifact" modules_install
    $ mkdir -p $PWD/../build/install_artifact/boot/
    $ cp $PWD/../build/arch/arm/boot/uImage $PWD/../build/install_artifact/boot/
    $ cp $PWD/../build/arch/arm/boot/dts/st*.dtb $PWD/../build/install_artifact/boot/

* Compile and install on the current source code directory
    $ cd <directory to kernel source code>
    * Build kernel images (uImage and vmlinux) and device tree (dtbs)
    $ make ARCH=arm uImage vmlinux dtbs LOADADDR=0xC2000040
    * Build kernel module
    $ make ARCH=arm modules
    * Generate output build artifacts
    $ make ARCH=arm INSTALL_MOD_PATH="$PWD/install_artifact" modules_install
    $ mkdir -p $PWD/install_artifact/boot/
    $ cp $PWD/arch/arm/boot/uImage $PWD/install_artifact/boot/
    $ cp $PWD/arch/arm/boot/dts/st*.dtb $PWD/install_artifact/boot/

Generated files are :
- $PWD/install_artifact/boot/uImage
- $PWD/install_artifact/boot/<stm32-boards>.dtb

---------------------------
7. Update software on board
---------------------------

7.1 Partitioning of binaries
----------------------------
* Bootfs:
  Bootfs contains the kernel and the devicetree.
* Rootfs:
  Rootfs contains the external kernel modules.
Please refer to User guide for more details.

7.2 Update via network
----------------------
* kernel + devicetree
    $ cd <path to install_artifact dir>/install_artifact
    if bootfs are not monted on target, mount it
        $ ssh root@<ip of board> df to see if there is a partition mounted on /boot
    else
        $ ssh root@<ip of board> mount <device corresponding to bootfs> /boot
    $ scp -r boot/* root@<ip of board>:/boot/
    $ ssh root@<ip of board> umount /boot

* kernel modules
    $ cd <path to install_artifact dir>/install_artifact
    Remove the link on install_artifact/lib/modules/<kernel version>/
    $ rm lib/modules/<kernel version>/source lib/modules/<kernel version>/build
    Optionally, strip kernel modules (to reduce the size of each kernel modules)
    $ find . -name "*.ko" | xargs $STRIP --strip-debug --remove-section=.comment --remove-section=.note --preserve-dates

    Copy kernel modules:
    $ scp -r lib/modules/* root@<ip of board>:/lib/modules/

    Generate a list of module dependencies (modules.dep) and a list of symbols
    provided by modules (modules.symbols):
    $ ssh root@<ip of board> /sbin/depmod -a
    Synchronize data on disk with memory
    $ ssh root@<ip of board> sync
    Reboot the board in order to take update into account
    $ ssh root@<ip of board> reboot

7.3 Update via SDCARD on your Linux PC
--------------------------------------
* kernel + devicetree
    $ cd <path to install_artifact dir>/install_artifact
    Verify sdcard are mounted on your Linux PC: /media/$USER/bootfs
    $ cp -r boot/* /media/$USER/bootfs/
    Depending of your Linux configuration, you may call the command under sudo
        $ sudo cp -r boot/* /media/$USER/bootfs/
    Don't forget to unmount properly sdcard

* kernel modules
    $ cd <path to install_artifact dir>/install_artifact
    Remove the link on install_artifact/lib/modules/<kernel version>/
    $ rm lib/modules/<kernel version>/source lib/modules/<kernel version>/build
    Optionally, strip kernel modules (to reduce the size of each kernel modules)
    $ find . -name "*.ko" | xargs $STRIP --strip-debug --remove-section=.comment --remove-section=.note --preserve-dates

    Verify sdcard are mounted on your Linux PC: /media/$USER/rootfs
    Copy kernel modules:
    $ cp -r lib/modules/* /media/$USER/rootfs/lib/modules/
    Depending of your Linux configuration, you may call the command under sudo
        $ sudo cp -r lib/modules/* /media/$USER/rootfs/lib/modules/
    Don't forget to unmount properly sdcard

    Generate a list of module dependencies (modules.dep) and a list of symbols
    provided by modules (modules.symbols):
    $ ssh root@<ip of board> depmod -a
    Synchronize data on disk with memory
    $ ssh root@<ip of board> sync
    Reboot the board in order to take update into account
    $ ssh root@<ip of board> reboot

7.4 Update via SDCARD on your BOARD (via U-Boot)
------------------------------------------------
You MUST configure first, via U-Boot, the board into usb mass storage:
* Plug the SDCARD on Board.
* Start the board and stop on U-boot shell:
    Hit any key to stop autoboot: 0
    STM32MP>
* plug an USB cable between the PC and the board via USB OTG port.
* On U-Boot shell, call the usb mass storage functionnality:
    STM32MP> ums 0 mmc 0
    ums <USB controller> <dev type: mmc|usb> <dev[:part]>
Example:
For SDCARD:    ums 0 mmc 0
For USB Disk:  ums 0 usb 0

* kernel + devicetree
    $ cd <path to install_artifact dir>/install_artifact
    Remove the link on install_artifact/lib/modules/<kernel version>/
    $ rm lib/modules/<kernel version>/source lib/modules/<kernel version>/build
    Optionally, strip kernel modules (to reduce the size of each kernel modules)
    $ find . -name "*.ko" | xargs $STRIP --strip-debug --remove-section=.comment --remove-section=.note --preserve-dates

    Verify sdcard mount point are mounted on your Linux PC: /media/$USER/bootfs
    $ cp -r boot/* /media/$USER/bootfs/
    Depending of your Linux configuration, you may call the command under sudo
        $ sudo cp -rf boot/* /media/$USER/bootfs/
    Don't forget to unmount properly sdcard
    Warning: kernel and device tree file name must be aligned between
    extlinux.conf file and file system.

* kernel modules
    $ cd <path to install_artifact dir>/install_artifact
    Remove the link on install_artifact/lib/modules/<kernel version>/
    $ rm lib/modules/<kernel version>/source lib/modules/<kernel version>/build
    Optionally, strip kernel modules (to reduce the size of each kernel modules)
    $ find . -name "*.ko" | xargs $STRIP --strip-debug --remove-section=.comment --remove-section=.note --preserve-dates

    Verify sdcard mount point are mounted on your Linux PC: /media/$USER/rootfs
    Copy kernel modules:
    $ cp -rf lib/modules/* /media/$USER/rootfs/lib/modules/
    Depending of your Linux configuration, you may call the command under sudo
        $ sudo cp -r lib/modules/* /media/$USER/rootfs/lib/modules/
    Don't forget to unmount properly sdcard

    At next runtime, don't forget to generate a list of module dependencies
    (modules.dep) and a list of symbols provided by modules (modules.symbols):
    $on board> depmod -a
    Synchronize data on disk with memory
    $on board> sync
    Reboot the board in order to take update into account
    $on board> reboot

7.5 Useful information
---------------------
* How to re-generate kernel database on board:
    $on board> depmod -a
    (don't forget to synchronize the filesystem before to reboot)
    $on board> sync

* How to see the list of external kernel modules loaded:
    $on board> lsmod

* How to see information about kernel module:
    $on board> modinfo /lib/modules/5.4.31/kernel/drivers/leds/led-class-flash.ko
filename:       /lib/modules/5.4.31/kernel/drivers/leds/led-class-flash.ko
license:        GPL v2
description:    LED Flash class interface
author:         Jacek Anaszewski <j.anaszewski@samsung.com>
depends:
intree:         Y
name:           led_class_flash
vermagic:       5.4.31 SMP preempt mod_unload modversions ARMv7 p2v8 

---------------------------
8. Update Starter Package with kernel compilation outputs
---------------------------

<-- Section under construction -->

If not already done, extract the artifacts from Starter Package tarball, for example:
    # tar xf en.FLASH-stm32mp1-*.tar.xz

Move to Starter Package root folder,
    #> cd <your_starter_package_dir_path>
Cleanup Starter Package from original kernel artifacts first
    #> echo "*** Section under construction ***"

<-- Section under construction -->
