Compilation of the gcnano kernel module:
1. Pre-requisite
2. Initialize cross-compilation via SDK
3. Prepare gcnano kernel module source
4. Manage gcnano kernel module source code with GIT
5. Compile the gcnano kernel module
6. Update software on board
7. Update Starter Package with gcnano kernel module compilation outputs

----------------
1. Pre-requisite
----------------
 - OpenSTLinux SDK must be installed.
 - Linux Kernel source must be installed and built

---------------------------------------
2. Initialize cross-compilation via SDK
---------------------------------------
Source SDK environment:
    $ source <path to SDK>/environment-setup-cortexa7t2hf-neon-vfpv4-ostl-linux-gnueabi

To verify if your cross-compilation environment have put in place:
    $ set | grep CROSS
    CROSS_COMPILE=arm-ostl-linux-gnueabi-

Warning: the environment variables are valid only on the shell session where you have
sourced the sdk environment.

------------------------
3. Prepare gcnano kernel module source
------------------------
If not already done, extract the sources from Developer Package tarball, for example:
    $ tar xf en.SOURCES-stm32mp*-*.tar.xz

In the gcnano driver source directory (sources/*/##BP##-##PR##),
you have one gcnano kernel module source tarball:
   - ##BP##-##PR##.tar.xz

If you would like to have a git management for the source code move to
to section 4 [Management of gcnano kernel module source code with GIT].

Otherwise, to manage gcnano kernel module source code without git, you must extract the
tarball now:

    $> tar xf ##BP##-##PR##.tar.xz
    $> cd ##BP##

You can now move to section 5 [Compile gcnano kernel module source code].

-------------------------------------
4. Manage gcnano kernel module source code with GIT
-------------------------------------
If you like to have a better management of change made on gcnano kernel module source,
you have 2 solutions to use git

4.1 Get STMicroelectronics gcnano kernel module source from GitHub
----------------------------------------------------
    URL: https://github.com/STMicroelectronics/gcnano-binaries.git
    Branch: ##ARCHIVER_ST_BRANCH##
    Revision: ##ARCHIVER_ST_REVISION##

    $ git clone https://github.com/STMicroelectronics/gcnano-binaries.git
    $ git checkout -b WORKING ##ARCHIVER_ST_REVISION##

4.2 Create Git from tarball
---------------------------
    $ tar xf ##BP##-##PR##.tar.xz
    $ cd ##BP##
    $ test -d .git || git init . && git add . && git commit -m "gcnano kernel module source code" && git gc
    $ git checkout -b WORKING

----------------
5. Compile the gcnano kernel module
----------------
OpenSTLinux SDK must be installed.

* Compile and install the gcnano kernel module
    * First define KERNEL_BUILDDIR variable to expose kernel source code
    $> KERNEL_BUILDDIR="../../linux-stm32mp-<KERNELVERSION>-stm32mp2-alpha-<RELEASE>/build"
    * Select the platform between "st-mp1" or "st-mp2"
    $> SOC_PLATFORM=st-mp<X>
    * Build kernel module
    $> make SOC_PLATFORM=${SOC_PLATFORM} DEBUG=0 O="${KERNEL_BUILDDIR}" M="${PWD}" AQROOT="${PWD}" -C ${KERNEL_BUILDDIR}

---------------------------
6. Update software on board
---------------------------

6.1 Update via network
----------------------
    Copy kernel modules:
    $ scp galcore.ko root@<ip of board>:/lib/modules/<kernel version>/extra

    Generate a list of module dependencies (modules.dep) and a list of symbols
    provided by modules (modules.symbols):
    $ ssh root@<ip of board> /sbin/depmod -a
    Synchronize data on disk with memory
    $ ssh root@<ip of board> sync
    Reboot the board in order to take update into account
    $ ssh root@<ip of board> reboot

---------------------------
7. Update Starter Package with gcnano kernel module compilation outputs
---------------------------

If not already done, extract the artifacts from Starter Package tarball, for example:
    # tar xf en.FLASH-stm32mp*-*.tar.xz

Update Starter package with just compiled kernel module galcore.ko:
    #> mkdir -p <your_starter_package_dir_path>/rootfs_mounted
    #> sudo mount -o loop <your_starter_package_dir_path>/images/stm32mp*/st-image-weston-openstlinux-weston-stm32mp*.ext4 <your_starter_package_dir_path>/rootfs_mounted
    #> sudo mkdir -p <your_starter_package_dir_path>/rootfs_mounted/lib/modules/*/extra

Cleanup Starter Package from original gcnano kernel module artifacts first
    #> sudo rm -vf <your_starter_package_dir_path>/rootfs_mounted/lib/modules/*/extra/galcore.ko

    #> sudo cp -vf */galcore.ko  <your_starter_package_dir_path>/rootfs_mounted/lib/modules/*/extra
    #> sudo depmod -a -b <your_starter_package_dir_path>/rootfs_mounted $(\ls <your_starter_package_dir_path>/rootfs_mounted/lib/modules)
    #> sudo umount <your_starter_package_dir_path>/rootfs_mounted
    #> rmdir <your_starter_package_dir_path>/rootfs_mounted
