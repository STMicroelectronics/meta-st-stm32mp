Compilation of the gcnano kernel module:
1. Pre-requisite
2. Compile the gcnano kernel module
3. Update software on board
4. Update Starter Package with gcnano kernel module compilation outputs

----------------
1. Pre-requisite
----------------
 - OpenSTLinux SDK must be installed.
 - Linux Kernel source must be installed and built

----------------
2. Compile the gcnano kernel module
----------------
OpenSTLinux SDK must be installed.

* Compile and install the gcnano kernel module
    $> tar xf ##BP##-##PR##.tar.xz
    $> cd ##GCNANO_TARNAME##
    $> KERNEL_BUILDDIR="../../linux-stm32mp-<KERNELVERSION>-stm32mp-<RELEASE>/build"
    * Build kernel module
    $> make ARCH=arm SOC_PLATFORM=st-mp1 DEBUG=0 O="${KERNEL_BUILDDIR}" M="${PWD}" AQROOT="${PWD}" -C ${KERNEL_BUILDDIR}

---------------------------
3. Update software on board
---------------------------

3.1 Update via network
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
4. Update Starter Package with gcnano kernel module compilation outputs
---------------------------

<-- Section under construction -->

If not already done, extract the artifacts from Starter Package tarball, for example:
    # tar xf en.FLASH-stm32mp1-*.tar.xz

Move to Starter Package root folder,
    #> cd <your_starter_package_dir_path>
Cleanup Starter Package from original gcnano kernel module artifacts first
    #> echo "*** Section under construction ***"

<-- Section under construction -->
