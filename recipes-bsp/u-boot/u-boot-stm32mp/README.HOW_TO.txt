Compilation of U-Boot:
1. Pre-requisite
2. Initialize cross-compilation via SDK
3. Prepare U-Boot source code
4. Manage of U-Boot source code with GIT
5. Enable support for fastboot support (optional)
6. Compile U-Boot source code
7. Update software on board
8. Update starter package with U-Boot compilation outputs
9. Example of compilation usage

----------------
1. Pre-requisite
----------------
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

External device tree is extracted. If this is not the case, please follow the
README_HOW_TO.txt in ../external-dt.

---------------------------------------
2. Initialize cross-compilation via SDK
---------------------------------------
* Source SDK environment:
    $ source <path to SDK>/environment-setup

* To verify that your cross-compilation environment is set-up correctly:
    $ set | grep CROSS_COMPILE

  If the variable CROSS_COMPILE has a value:
   - arm-ostl-linux-gnueabi- for 32 bits architecture (for example STM32MP1)
   - aarch64-ostl-linux- for 64 bits architecture (for example STM32MP2)
  Then everything is set-up correctly

Warning: the environment are valid only on the shell session where you have
         sourced the sdk environment.

------------------------
3. Prepare U-Boot source
------------------------
If not already done, extract the sources from Developer Package tarball, for example:
    $ tar xf en.SOURCES-stm32mp*-*.tar.xz

In the U-Boot source directory (sources/*/##BP##-##PR##),
you have one U-Boot source tarball, the patches and one Makefile:
   - ##BP##-##PR##.tar.xz
   - 00*.patch
   - Makefile.sdk.##MACHINE##

If you would like to have a git management for the source code move to
to section 4 [Management of U-Boot source code with GIT].

Otherwise, to manage U-Boot source code without git, you must extract the
tarball now and apply the patch:

    $> tar xf ##BP##-##PR##.tar.xz
    $> cd ##BP##
    $> for p in `ls -1 ../*.patch`; do patch -p1 < $p; done

You can now move to section 5 [Enable support for fastboot support].

-------------------------------------
4. Manage U-Boot source code with GIT
-------------------------------------
If you like to have a better management of change made on U-Boot source,
you have 3 solutions to use git

4.1 Get STMicroelectronics U-Boot source from GitHub
----------------------------------------------------
    URL: https://github.com/STMicroelectronics/u-boot.git
    Branch: ##ARCHIVER_ST_BRANCH##
    Revision: ##ARCHIVER_ST_REVISION##

    $ git clone https://github.com/STMicroelectronics/u-boot.git
    $ git checkout -b WORKING ##ARCHIVER_ST_REVISION##

4.2 Create Git from tarball
---------------------------
    $ tar xf ##BP##-##PR##.tar.xz
    $ cd ##BP##
    $ test -d .git || git init . && git add . && git commit -m "U-Boot source code" && git gc
    $ git checkout -b WORKING
    $ for p in `ls -1 ../*.patch`; do git am $p; done

4.3 Get Git from DENX community and apply STMicroelectronics patches
---------------------------------------------------------------
    URL: git://git.denx.de/u-boot.git
    Branch: ##ARCHIVER_COMMUNITY_BRANCH##
    Revision: ##ARCHIVER_COMMUNITY_REVISION##

    $ git clone git://git.denx.de/u-boot.git
or
    $ git clone http://git.denx.de/u-boot.git

    $ cd u-boot
    $ git checkout -b WORKING ##ARCHIVER_COMMUNITY_REVISION##
    $ for p in `ls -1 <path to patch>/*.patch`; do git am $p; done

-------------------------------------------------
5. Enable support for fastboot support (optional)
-------------------------------------------------
If you want to use default U-Boot support,
then you can move to section 6 [Compile U-Boot source code].

Fastboot is a feature in U-Boot which can improve a lot the speed of binary downloading.
Fastboot is, for the moment, only supported on SD-Card and eMMC. And only one mode is
supported at a given time. It means that if your U-Boot supports fastboot on SD-Card
then it's not availabe on eMMC. To support it on eMMC, you must use another U-BOOT.

To enable fastboot on the right mass storage, simply apply the right fragment
into the right defconfig:
  - fastboot on SD-Card for stm32mp15:
    $ cat $PWD/../fragment-04-fastboot_mmc0.fb_cfg >> configs/stm32mp15_defconfig
  - fastboot on eMMC for stm32mp15:
    $ cat $PWD/../fragment-05-fastboot_mmc1.fb_cfg >> configs/stm32mp15_defconfig
  - fastboot on SD-Card for stm32mp13:
    $ cat $PWD/../fragment-04-fastboot_mmc0.fb_cfg >> configs/stm32mp13_defconfig
  - fastboot on eMMC for stm32mp13:
    $ cat $PWD/../fragment-05-fastboot_mmc1.fb_cfg >> configs/stm32mp13_defconfig

Then build U-Boot as explained in chapter 6.

-----------------------------
6. Compile U-Boot source code
-----------------------------
According to your needs, there are 2 propositions to generate U-Boot artifacts:

6.1 Updating Starter Package artifacts
----------------------------------------
If not already done, extract the artifacts from Starter Package tarball, for example:
    $ tar xf en.FLASH-##MACHINE##-*.tar.xz
Move to Starter Package root folder,
    $ cd <your_starter_package_dir_path>
Cleanup Starter Package from original U-Boot artifacts first
    $ rm -rf images/##MACHINE##/u-boot/*
    $ rm -rf images/##MACHINE##/fip/*
Configure the DEPLOYDIR path to Starter Package U-Boot artifacts folder
    $ export DEPLOYDIR=<your_starter_package_dir_path>/images/##MACHINE##/u-boot
Since OpenSTLinux activates FIP by default, FIP_artifacts directory path must be specified before launching compilation
    $ export FIP_DEPLOYDIR_ROOT=<your_starter_package_dir_path>/images/##MACHINE##

You can now move to section 6.3 [Generating U-Boot artifacts].

6.2 Creating your own bootloader artifacts (development use case)
-------------------------------------------------------------------
With this configuration you will need to generate one by one all bootloader artifacts first before being able to generate
the FIP artifacts. And for that you need to share the same root folder for all bootloader compilation under Developer Package
Since OpenSTLinux activates FIP by default, FIP_artifacts directory path must be specified before launching compilation
    $> export FIP_DEPLOYDIR_ROOT=<bootloader artifacts location>
Make sure to configure then the DEPLOYDIR path accordingly:
    $> export DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot

You can now move to section 6.3 [Generating U-Boot artifacts].

6.3 Generating U-Boot artifacts
-------------------------------
To use the external device tree feature, EXTDT_DIR variable must be set to the root location of external DT
as specified in the README.HOW_TO.txt of external-dt
    $> export EXTDT_DIR=<external DT location>

To list U-Boot source code compilation configurations:
    $ cd <directory to U-Boot source code>
    $ make -f $PWD/../Makefile.sdk.##MACHINE## help

There are different targets for U-Boot compilation:

- Generate U-Boot binaries
  Using default configuration, you just need to launch the 'uboot' target:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## clean
    $ make -f $PWD/../Makefile.sdk.##MACHINE## uboot
  Example below to compile for a specific U-Boot configuration:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp15_defconfig UBOOT_BINARY=u-boot.dtb DEVICE_TREE=stm32mp157f-dk2 clean
    $ make -f $PWD/../Makefile.sdk.##MACHINE## UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp15_defconfig UBOOT_BINARY=u-boot.dtb DEVICE_TREE=stm32mp157f-dk2 uboot
  The build results for this component are available in <DEPLOYDIR>.

- Generate FIP binaires
  Make sure to have all bootloader binaries (TF-A, U-Boot and optee-os) available in <FIP_DEPLOYDIR_ROOT> folder before launching the build
  Using default configuration, you just need to launch the 'fip' target:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## fip
  Example below to compile for a specific U-Boot configuration:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp15_defconfig UBOOT_BINARY=u-boot.dtb DEVICE_TREE=stm32mp157f-dk2 fip
  The build results for this component are available in <FIP_DEPLOYDIR_ROOT>/fip

- Generate all U-Boot artifacts in a row
  Make sure to have all other bootloader binaries (TF-A and optee-os) available in <FIP_DEPLOYDIR_ROOT> folder before launching the build
  Using default configuration, you just need to launch the 'fip' target:
    $> make -f $PWD/../Makefile.sdk.##MACHINE## clean
    $> make -f $PWD/../Makefile.sdk.##MACHINE## all
  Example below to compile for a specific U-Boot configuration:
    $ make -f $PWD/../Makefile.sdk.##MACHINE## UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp15_defconfig UBOOT_BINARY=u-boot.dtb DEVICE_TREE=stm32mp157f-dk2 clean
    $ make -f $PWD/../Makefile.sdk.##MACHINE## UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp15_defconfig UBOOT_BINARY=u-boot.dtb DEVICE_TREE=stm32mp157f-dk2 all
  The build results for this component are available in <DEPLOYDIR> and <FIP_DEPLOYDIR_ROOT>/fip

---------------------------
7. Update software on board
---------------------------
Please use STM32CubeProgrammer and only tick the ssbl-boot and fip partition (more informations on the wiki website http://wiki.st.com/stm32mpu)

---------------------------
8. Generate new Starter Package with U-Boot compilation outputs
---------------------------
If not already done, extract the artifacts from Starter Package tarball, for example:
    # tar xf en.FLASH-stm32mp*-*.tar.xz

Move to Starter Package root folder,
    #> cd <your_starter_package_dir_path>
Cleanup Starter Package from original U-Boot artifacts first
    #> rm -rf images/stm32mp*/fip/*
Update Starter Package with new fip artifacts from <FIP_DEPLOYDIR_ROOT>/fip folder:
    #> cp -rvf $FIP_DEPLOYDIR_ROOT/fip/* images/stm32mp*/fip/

Then the new Starter Package is ready to use for "Image flashing" on board (more information on wiki website https://wiki.st.com/stm32mpu).

-------------------------------
9. Example of compilation usage
-------------------------------
    $@E> cd ##BP##-##PR##
    $@E> tar xf ##BP##-##PR##.tar.xz
    $@E> cd ##BP##
    $@E> for p in `ls -1 ../*.patch`; do patch -p1 < $p; done
    $@S> export FIP_DEPLOYDIR_ROOT=<your_deploy_dir_path>
    $ cd ..
    $ cd ..
    $@P> cd ##BP##-##PR##
    $@P> cd ##BP##

##CASE_stm32mp1##    "your_board_name" is like stm32mp157f-dk2 or stm32mp135f-mx-mycustomboard
##CASE_stm32mp1##    "your_soc_name" is like stm32mp15 or stm32mp13
##CASE_stm32mp1##    For runtime binaries
##CASE_stm32mp1##    $@C> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=default UBOOT_DEFCONFIG=<uboot_defconfig> DEVICE_TREE=<your_board_name> EXTDT_DIR=<externaldt_path> EXTDT_DIR_UBOOT=<externaldt_uboot_path> uboot
##CASE_stm32mp1##    For flashing binaries
##CASE_stm32mp1##    $@PC> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=programmer UBOOT_DEFCONFIG=<uboot_defconfig> DEVICE_TREE=<your_board_name> EXTDT_DIR=<externaldt_path> EXTDT_DIR_UBOOT_SERIAL=<externaldt_uboot_path> uboot
##CASE_stm32mp1##    Example with external dt:
##CASE_stm32mp1##      Example for runtime binaries
##CASE_stm32mp1##        $MP15> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp15_defconfig DEVICE_TREE=stm32mp157f-dk2 EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_UBOOT=ca7-td/u-boot uboot
##CASE_stm32mp1##        $MP13> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp13_defconfig DEVICE_TREE=stm32mp135f-dk  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_UBOOT=ca7-td/u-boot uboot
##CASE_stm32mp1##      Example for flashing binaries
##CASE_stm32mp1##        $MP15> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=programmer UBOOT_DEFCONFIG=stm32mp15_defconfig DEVICE_TREE=stm32mp157f-dk2 EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_UBOOT_SERIAL=ca7-td/u-boot uboot
##CASE_stm32mp1##        $MP13> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=programmer UBOOT_DEFCONFIG=stm32mp13_defconfig DEVICE_TREE=stm32mp135f-dk  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_UBOOT_SERIAL=ca7-td/u-boot uboot
##CASE_stm32mp1##    Example with CubeMx devicetree:
##CASE_stm32mp1##      Example for runtime binaries
##CASE_stm32mp1##        $MP13> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp15_defconfig DEVICE_TREE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_UBOOT=DeviceTree/<your_cubemx_project_name>/u-boot uboot
##CASE_stm32mp1##        $MP15> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp13_defconfig DEVICE_TREE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_UBOOT=CA7/DeviceTree/<your_cubemx_project_name>/u-boot uboot
##CASE_stm32mp1##      Example for flashing binaries
##CASE_stm32mp1##        $MP13> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=programmer UBOOT_DEFCONFIG=stm32mp13_defconfig DEVICE_TREE=<your_CUBE_MX_board_name>  EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_UBOOT_SERIAL=DeviceTree/<your_cubemx_project_name>/u-boot uboot
##CASE_stm32mp1##        $MP15> make -f $PWD/../Makefile.sdk.stm32mp1 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=programmer UBOOT_DEFCONFIG=stm32mp13_defconfig DEVICE_TREE=<your_CUBE_MX_board_name>  EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_UBOOT_SERIAL=CA7/DeviceTree/<your_cubemx_project_name>/u-boot uboot
##CASE_stm32mp2##    "your_board_name" is like stm32mp257f-dk or stm32mp215f-mx-mycustomboard
##CASE_stm32mp2##    "your_soc_name" is like stm32mp25 or stm32mp23 or stm32mp21
##CASE_stm32mp2##    For runtime binaries
##CASE_stm32mp2##    $@C> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=default UBOOT_DEFCONFIG=<uboot_defconfig> DEVICE_TREE=<your_board_name> EXTDT_DIR=<externaldt_path> EXTDT_DIR_UBOOT=<externaldt_uboot_path> uboot
##CASE_stm32mp2##    For flashing binaries
##CASE_stm32mp2##    $@PC> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=programmer UBOOT_DEFCONFIG=<uboot_defconfig> DEVICE_TREE=<your_board_name> EXTDT_DIR=<externaldt_path> EXTDT_DIR_UBOOT=<externaldt_uboot_path> uboot
##CASE_stm32mp2##    Example with external dt:
##CASE_stm32mp2##      Example for runtime binaries
##CASE_stm32mp2##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp21_defconfig DEVICE_TREE=stm32mp215f-dk  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_UBOOT=ca35-td/u-boot uboot
##CASE_stm32mp2##        $MP23> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp23_defconfig DEVICE_TREE=stm32mp235f-dk  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_UBOOT=ca35-td/u-boot uboot
##CASE_stm32mp2##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp25_defconfig DEVICE_TREE=stm32mp257f-ev1 EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_UBOOT=ca35-td/u-boot uboot
##CASE_stm32mp2##      Example for flashing binaries
##CASE_stm32mp2##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=programmer UBOOT_DEFCONFIG=stm32mp21_defconfig DEVICE_TREE=stm32mp215f-dk  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_UBOOT_SERIAL=ca35-td/u-boot uboot
##CASE_stm32mp2##        $MP23> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=programmer UBOOT_DEFCONFIG=stm32mp23_defconfig DEVICE_TREE=stm32mp235f-dk  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_UBOOT_SERIAL=ca35-td/u-boot uboot
##CASE_stm32mp2##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=programmer UBOOT_DEFCONFIG=stm32mp25_defconfig DEVICE_TREE=stm32mp257f-ev1 EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_UBOOT_SERIAL=ca35-td/u-boot uboot
##CASE_stm32mp2##    Example with CubeMx devicetree:
##CASE_stm32mp2##      Example for runtime binaries
##CASE_stm32mp2##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp21_defconfig DEVICE_TREE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_UBOOT=CA35/DeviceTree/<your_cubemx_project_name>/u-boot uboot
##CASE_stm32mp2##        $MP23> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp23_defconfig DEVICE_TREE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_UBOOT=CA35/DeviceTree/<your_cubemx_project_name>/u-boot uboot
##CASE_stm32mp2##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp25_defconfig DEVICE_TREE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_UBOOT=CA35/DeviceTree/<your_cubemx_project_name>/u-boot uboot
##CASE_stm32mp2##      Example for flashing binaries
##CASE_stm32mp2##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=programmer UBOOT_DEFCONFIG=stm32mp21_defconfig DEVICE_TREE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_UBOOT_SERIAL=CA35/DeviceTree/<your_cubemx_project_name>/u-boot uboot
##CASE_stm32mp2##        $MP23> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=programmer UBOOT_DEFCONFIG=stm32mp23_defconfig DEVICE_TREE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_UBOOT_SERIAL=CA35/DeviceTree/<your_cubemx_project_name>/u-boot uboot
##CASE_stm32mp2##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2 DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=programmer UBOOT_DEFCONFIG=stm32mp25_defconfig DEVICE_TREE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_UBOOT_SERIAL=CA35/DeviceTree/<your_cubemx_project_name>/u-boot uboot
##CASE_stm32mp2-m33td##    "your_board_name" is like stm32mp257f-dk or stm32mp215f-mx-mycustomboard
##CASE_stm32mp2-m33td##    "your_soc_name" is like stm32mp25 or stm32mp23 or stm32mp21
##CASE_stm32mp2-m33td##    For runtime binaries
##CASE_stm32mp2-m33td##    $@C> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=default UBOOT_DEFCONFIG=<uboot_defconfig> DEVICE_TREE=<uboot_dt_name> EXTDT_DIR=<externaldt_path> EXTDT_DIR_UBOOT=<externaldt_uboot_path> uboot
##CASE_stm32mp2-m33td##    For flashing binaries
##CASE_stm32mp2-m33td##    $@PC> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=programmer UBOOT_DEFCONFIG=<uboot_defconfig> DEVICE_TREE=<uboot_programmer_dt_name> EXTDT_DIR=<externaldt_path> EXTDT_DIR_UBOOT_SERIAL=<externaldt_uboot_programmer_path> uboot
##CASE_stm32mp2-m33td##    Example with external dt:
##CASE_stm32mp2-m33td##      Example for runtime binaries
##CASE_stm32mp2-m33td##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp21_defconfig DEVICE_TREE=stm32mp215f-dk-cm33tdcid-ostl-sdcard  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_UBOOT=cm33-td/u-boot uboot
##CASE_stm32mp2-m33td##        $MP23> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp23_defconfig DEVICE_TREE=stm32mp235f-dk-cm33tdcid-ostl-sdcard  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_UBOOT=cm33-td/u-boot uboot
##CASE_stm32mp2-m33td##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp25_defconfig DEVICE_TREE=stm32mp257f-ev1-cm33tdcid-ostl-emmc EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_UBOOT=cm33-td/u-boot uboot
##CASE_stm32mp2-m33td##      Example for flashing binaries
##CASE_stm32mp2-m33td##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=programmer UBOOT_DEFCONFIG=stm32mp21_defconfig DEVICE_TREE=stm32mp235f-dk-cm33tdcid-ostl-serial-ca35tdcid  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_UBOOT_SERIAL=cm33-td/u-boot uboot
##CASE_stm32mp2-m33td##        $MP23> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=programmer UBOOT_DEFCONFIG=stm32mp23_defconfig DEVICE_TREE=stm32mp215f-dk-cm33tdcid-ostl-serial-ca35tdcid  EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_UBOOT_SERIAL=cm33-td/u-boot uboot
##CASE_stm32mp2-m33td##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=programmer UBOOT_DEFCONFIG=stm32mp25_defconfig DEVICE_TREE=stm32mp257f-ev1-cm33tdcid-ostl-serial-ca35tdcid EXTDT_DIR=$EXTDT_DIR EXTDT_DIR_UBOOT_SERIAL=cm33-td/u-boot uboot
##CASE_stm32mp2-m33td##    Example with CubeMx devicetree:
##CASE_stm32mp2-m33td##      Example for runtime binaries
##CASE_stm32mp2-m33td##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp21_defconfig DEVICE_TREE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_UBOOT=CA35/DeviceTree/<your_cubemx_project_name>/u-boot uboot
##CASE_stm32mp2-m33td##        $MP23> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp23_defconfig DEVICE_TREE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_UBOOT=CA35/DeviceTree/<your_cubemx_project_name>/u-boot uboot
##CASE_stm32mp2-m33td##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=default UBOOT_DEFCONFIG=stm32mp25_defconfig DEVICE_TREE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_UBOOT=CA35/DeviceTree/<your_cubemx_project_name>/u-boot uboot
##CASE_stm32mp2-m33td##      Example for flashing binaries
##CASE_stm32mp2-m33td##        $MP21> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=programmer UBOOT_DEFCONFIG=stm32mp21_defconfig DEVICE_TREE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_UBOOT_SERIAL=ExtMemLoader/DeviceTree/<your_cubemx_project_name>/u-boot uboot
##CASE_stm32mp2-m33td##        $MP23> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=programmer UBOOT_DEFCONFIG=stm32mp23_defconfig DEVICE_TREE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_UBOOT_SERIAL=ExtMemLoader/DeviceTree/<your_cubemx_project_name>/u-boot uboot
##CASE_stm32mp2-m33td##        $MP25> make -f $PWD/../Makefile.sdk.stm32mp2-m33td DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/u-boot UBOOT_CONFIG=programmer UBOOT_DEFCONFIG=stm32mp25_defconfig DEVICE_TREE=<your_CUBE_MX_board_name> EXTDT_DIR=<cubemx_output_dir> EXTDT_DIR_UBOOT_SERIAL=ExtMemLoader/DeviceTree/<your_cubemx_project_name>/u-boot uboot

