Compilation of TF-A (Trusted Firmware-A):
1. Pre-requisite
2. Initialize cross-compilation via SDK
3. Prepare TF-A source code
4. Manage TF-A source code with GIT
5. Compile TF-A source code
6. Update software on board
7. Update starter package with TF-A compilation outputs

----------------
1. Pre-requisite
----------------
OpenSTLinux SDK must be installed.

For TF-A build you need to install:
- git:
    Ubuntu: sudo apt-get install git-core gitk
    Fedora: sudo yum install git

If you have never configured you git configuration:
    $ git config --global user.name "your_name"
    $ git config --global user.email "your_email@example.com"

---------------------------------------
2. Initialize cross-compilation via SDK
---------------------------------------
Source SDK environment:
    $ source <path to SDK>/environment-setup-cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi

To verify if your cross-compilation environment have put in place:
    $ set | grep CROSS
    CROSS_COMPILE=arm-ostl-linux-gnueabi-

Warning: the environment are valid only on the shell session where you have
sourced the sdk environment.

----------------------
3. Prepare TF-A source
----------------------
If not already done, extract the sources from Developer Package tarball, for example:
    $ tar xf en.SOURCES-stm32mp1-*.tar.xz

In the TF-A source directory (sources/*/##BP##-##PR##),
you have one TF-A source tarball, the patches and one Makefile:
   - ##BP##-##PR##.tar.xz
   - 00*.patch
   - Makefile.sdk

If you would like to have a git management for the source code move to
to section 4 [Management of TF-A source code with GIT].

Otherwise, to manage TF-A source code without git, you must extract the
tarball now and apply the patch:

    $> tar xf ##BP##-##PR##.tar.xz
    $> cd ##BP##
    $> for p in `ls -1 ../*.patch`; do patch -p1 < $p; done

You can now move to section 5 [Compile TF-A source code].

-----------------------------------
4. Manage TF-A source code with GIT
-----------------------------------
If you like to have a better management of change made on TF-A source, you
have 3 solutions to use git:

4.1 Get STMicroelectronics TF-A source from GitHub
--------------------------------------------------
    URL: https://github.com/STMicroelectronics/arm-trusted-firmware.git
    Branch: ##ARCHIVER_ST_BRANCH##
    Revision: ##ARCHIVER_ST_REVISION##

    $ git clone https://github.com/STMicroelectronics/arm-trusted-firmware.git
    $ git checkout -b WORKING ##ARCHIVER_ST_REVISION##

4.2 Create Git from tarball
---------------------------
    $ tar xf ##BP##-##PR##.tar.xz
    $ cd ##BP##
    $ test -d .git || git init . && git add . && git commit -m "tf-a source code" && git gc
    $ git checkout -b WORKING
    $ for p in `ls -1 ../*.patch`; do git am $p; done

4.3 Get Git from Arm Software community and apply STMicroelectronics patches
---------------------------------------------------------------
    URL: git://github.com/ARM-software/arm-trusted-firmware.git
    Branch: ##ARCHIVER_COMMUNITY_BRANCH##
    Revision: ##ARCHIVER_COMMUNITY_REVISION##

    $ git clone git://github.com/ARM-software/arm-trusted-firmware.git
    $ cd arm-trusted-firmware
    $ git checkout -b WORKING ##ARCHIVER_COMMUNITY_REVISION##
    $ for p in `ls -1 <path to patch>/*.patch`; do git am $p; done

---------------------------
5. Compile TF-A source code
---------------------------
Since OpenSTLinux activates FIP by default, FIP_artifacts directory path must be specified before launching compilation
  - In case of using SOURCES-xxxx.tar.gz of Developer package the FIP_DEPLOYDIR_ROOT must be set as below:
    $> export FIP_DEPLOYDIR_ROOT=$PWD/../../FIP_artifacts

The build results for this component are available in DEPLOYDIR (Default: $PWD/../deploy).
If needed, this deploy directory can be specified by adding "DEPLOYDIR=<your_deploy_dir_path>" compilation option to the build command line below.
The generated FIP images are available in <FIP_DEPLOYDIR_ROOT>/fip

To list TF-A source code compilation configurations:
    $ make -f $PWD/../Makefile.sdk help
To compile TF-A source code:
    $ make -f $PWD/../Makefile.sdk all
To compile TF-A source code for a specific config:
    $ make -f $PWD/../Makefile.sdk TF_A_DEVICETREE=stm32mp157c-ev1 TF_A_CONFIG=trusted ELF_DEBUG_ENABLE='1' all
        NB: TF_A_DEVICETREE flag must be set to switch to correct board configuration.
To compile TF-A source code and overwrite the default FIP artifacts with built artifacts:
    $> make -f $PWD/../Makefile.sdk DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware all

---------------------------
6. Update software on board
---------------------------
Please use STM32CubeProgrammer to update the boot partitions, find more informations on the wiki website https://wiki.st.com/stm32mpu

---------------------------
7. Update Starter Package with TF-A compilation outputs
---------------------------
If not already done, extract the artifacts from Starter Package tarball, for example:
    # tar xf en.FLASH-stm32mp1-*.tar.xz

Move to Starter Package root folder,
    #> cd <your_starter_package_dir_path>
Cleanup Starter Package from original TF-A artifacts first
    #> rm -rf images/stm32mp1/arm-trusted-firmware/*
    #> rm -rf images/stm32mp1/fip/*
Update Starter Package with new FSBL binaries from <DEPLOYDIR> folder
    #> DEPLOYDIR=$FIP_DEPLOYDIR_ROOT/arm-trusted-firmware && cp -rvf $DEPLOYDIR/* images/stm32mp1/arm-trusted-firmware/
        NB: if <DEPLOYDIR> has not been overide at compilation step, use default path: <tf-a source code folder>/../deploy
Update Starter Package with new fip artifacts from <FIP_DEPLOYDIR_ROOT>/fip folder:
    #> cp -rvf $FIP_DEPLOYDIR_ROOT/fip/* images/stm32mp1/fip/

Then the new Starter Package is ready to use for "Image flashing" on board (more information on wiki website https://wiki.st.com/stm32mpu).
