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
    $ git config --global user.name "your_name"
    $ git config --global user.email "your_email@example.com"

2. Initialize cross-compilation via SDK:
---------------------------------------
* Source SDK environment:
    $ source <path to SDK>/environment-setup-cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi

* To verify if you cross-compilation environment are put in place:
    $ set | grep CROSS
    CROSS_COMPILE=arm-ostl-linux-gnueabi-

Warning: the environment are valid only on the shell session where you have
         sourced the sdk environment.

3. Prepare U-Boot source:
------------------------

Extract the sources from tarball, for example:
$ tar xfJ SOURCES-st-image-weston-openstlinux-weston-stm32mp1-*.tar.xz

In the U-Boot source directory (sources/*/##BP##-##PR##),
you have one U-Boot source tarball, the patches and one Makefile:
   - ##BP##-##PR##.tar.gz
   - 000*.patch
   - Makefile.sdk

NB: if you would like to have a git management on the code see
    section 4 [Management of U-Boot source code with GIT]

Then you must extract the tarball and apply the patch:

    $> tar xfz ##BP##-##PR##.tar.gz
    $> cd ##BP##
    $> for p in `ls -1 ../*.patch`; do patch -p1 < $p; done

4. Management of U-Boot source code with GIT
--------------------------------------------
If you like to have a better management of change made on U-Boot source,
you have 3 solutions to use git

4.1 Get STMicroelectronics U-Boot source from GitHub

    URL: https://github.com/STMicroelectronics/u-boot.git
    Branch: ##ARCHIVER_ST_BRANCH##
    Revision: ##ARCHIVER_ST_REVISION##

    $ git clone https://github.com/STMicroelectronics/u-boot.git
    $ git checkout -b WORKING ##ARCHIVER_ST_REVISION##

4.2 Create Git from tarball

    $ tar xfz ##BP##-##PR##.tar.gz
    $ cd ##BP##
    $ test -d .git || git init . && git add . && git commit -m "U-Boot source code" && git gc
    $ git checkout -b WORKING
    $ for p in `ls -1 ../*.patch`; do git am $p; done

4.3 Get Git from community and apply STMicroelectronics patches

    URL: git://git.denx.de/u-boot.git
    Branch: ##ARCHIVER_COMMUNITY_BRANCH##
    Revision: ##ARCHIVER_COMMUNITY_REVISION##

    $ git clone git://git.denx.de/u-boot.git
or
    $ git clone http://git.denx.de/u-boot.git

    $ cd u-boot
    $ git checkout -b WORKING ##ARCHIVER_COMMUNITY_REVISION##
    $ for p in `ls -1 ../*.patch`; do git am $p; done

5. Compilation U-Boot source code:
----------------------------------
To compile U-Boot source code, first move to U-Boot source:
    $ cd ##BP##
    or
    $ cd u-boot

5.1 Compilation for one target (one defconfig, one device tree) - and no FIP

    see <U-Boot source>/board/st/stm32mp1/README for details

    $ make stm32mp15_<config>_defconfig
    $ make DEVICE_TREE=<device tree> all

    example:

    a) trusted boot on ev1
	$ make stm32mp15_trusted_defconfig
	$ make DEVICE_TREE=stm32mp157c-ev1 all

    b) basic boot on dk2
	$ make stm32mp15_basic_defconfig
	$ make DEVICE_TREE=stm32mp157c-dk2 all

5.2 Compilation for several targets: use Makefile.sdk (with FIP)
Calls the specific 'Makefile.sdk' provided to compile U-Boot:
  - Display 'Makefile.sdk' file default configuration and targets:
    $  make -f $PWD/../Makefile.sdk help
As mentionned in help, OpenSTLinux has activated FIP by default, so the FIP_artifacts should be specified
  - In case of using SOURCES-xxxx.tar.gz of Developer package the FIP_DEPLOYDIR_ROOT should be set as below:
    $> export FIP_DEPLOYDIR_ROOT=$PWD/../../FIP_artifacts

  - Compile default U-Boot configuration:
    $> make -f $PWD/../Makefile.sdk all

Default U-Boot configuration is done in 'Makefile.sdk' file through specific variables
'BOOT_CONFIG', 'UBOOT_DEFCONFIG', 'UBOOT_BINARY' and 'UBOOT_DEVICETREE':
  - 'UBOOT_CONFIG' is the name to append to U-boot binaries (ex: 'trusted', etc).
    ex: UBOOT_CONFIG=trusted
  - 'UBOOT_DEFCONFIG' is the name of U-Boot defconfig to build
    ex: UBOOT_DEFCONFIG=stm32mp15_trusted_defconfig
  - 'UBOOT_BINARY' is the U-Boot binary to export (ex: 'u-boot.dtb', 'u-boot.img', etc)
    ex: UBOOT_BINARY=u-boot.dtb
  - 'UBOOT_DEVICETREE' is a list of device tree to build, using 'space' as separator.
    ex: UBOOT_DEVICETREE="<devicetree1> <devicetree2>"

The generated FIP images are available in $FIP_DEPLOYDIR_ROOT/fip

You can override the default U-Boot configuration if you specify these variables:
  - Compile default U-Boot configuration but applying specific devicetree(s):
    $ make -f $PWD/../Makefile.sdk all DEVICETREE="<devicetree1> <devicetree2>"
  - Compile for a specific U-Boot configuration:
    $ make -f $PWD/../Makefile.sdk all UBOOT_CONFIG=trusted UBOOT_DEFCONFIG=stm32mp15_trusted_defconfig UBOOT_BINARY=u-boot.dtb DEVICETREE=stm32mp157f-dk2

6. Update software on board:
----------------------------
Please use STM32CubeProgrammer and only tick the ssbl-boot and fip partition (more informations on the wiki website http://wiki.st.com/stm32mpu)

