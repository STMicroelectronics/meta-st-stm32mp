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
    $> cd ##BP##
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

If not already done, extract the artifacts from Starter Package tarball, for example:
    # tar xf en.FLASH-stm32mp1-*.tar.xz

Update Starter package with just compiled kernel module galcore.ko:
Move to Starter Package root folder,
    #> mkdir -p <your_starter_package_dir_path>/rootfs_mounted
    #> sudo mount -o loop <your_starter_package_dir_path>/images/stm32mp1/st-image-weston-openstlinux-weston-stm32mp1.ext4 <your_starter_package_dir_path>/rootfs_mounted
    #> sudo mkdir -p <your_starter_package_dir_path>/rootfs_mounted/lib/modules/*/extra
    #> sudo cp -vf */galcore.ko  <your_starter_package_dir_path>/rootfs_mounted/lib/modules/*/extra
    #> sudo depmod -a -b <your_starter_package_dir_path>/rootfs_mounted $(\ls <your_starter_package_dir_path>/rootfs_mounted/lib/modules)
    #> sudo umount <your_starter_package_dir_path>/rootfs_mounted
    #> rmdir <your_starter_package_dir_path>/rootfs_mounted
