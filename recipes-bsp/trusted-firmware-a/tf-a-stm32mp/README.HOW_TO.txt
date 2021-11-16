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
have 3 solutions to use git:

4.1 Get STMicroelectronics TF-A source from GitHub

    URL: https://github.com/STMicroelectronics/arm-trusted-firmware.git
    Branch: ##ARCHIVER_ST_BRANCH##
    Revision: ##ARCHIVER_ST_REVISION##

    $ git clone https://github.com/STMicroelectronics/arm-trusted-firmware.git
    $ git checkout -b WORKING ##ARCHIVER_ST_REVISION##

4.2 Create Git from tarball
    $ cd <directory to tf-a source code>
    $ test -d .git || git init . && git add . && git commit -m "tf-a source code" && git gc
    $ git checkout -b WORKING
    $ for p in `ls -1 <path to patch>/*.patch`; do git am $p; done

4.3 Get Git from community and apply STMicroelectronics patches

    URL: git://github.com/ARM-software/arm-trusted-firmware.git
    Branch: ##ARCHIVER_COMMUNITY_BRANCH##
    Revision: ##ARCHIVER_COMMUNITY_REVISION##

    $ git clone git://github.com/ARM-software/arm-trusted-firmware.git
    $ cd <directory to tf-a source code>
    $ git checkout -b WORKING ##ARCHIVER_COMMUNITY_REVISION##
    $ for p in `ls -1 <path to patch>/*.patch`; do git am $p; done

5. Build tf-a source code:
--------------------------------
Since OpenSTLinux has activated FIP by default, so the FIP_artifacts should be specified before launching compilation
  - In case of using SOURCES-xxxx.tar.gz of Developer package the FIP_DEPLOYDIR_ROOT should be set as below:
    $> export FIP_DEPLOYDIR_ROOT=$PWD/../../FIP_artifacts

To compile tf-a source code
    $> make -f $PWD/../Makefile.sdk all
or for a specific config :
    $ make -f $PWD/../Makefile.sdk TF_A_DEVICETREE=stm32mp157c-ev1 TF_A_CONFIG=trusted ELF_DEBUG_ENABLE='1' all

NB: TF_A_DEVICETREE flag must be set to switch to correct board configuration.

The generated FIP images are available in $FIP_DEPLOYDIR_ROOT/fip

6. Update software on board:
----------------------------
Please use STM32CubeProgrammer then only tick the boot partitions means patitions 0x1 to 0x6 (more informations on the wiki website http://wiki.st.com/stm32mpu)

