Compilation of Optee-os (Trusted Execution Environment):
1. Pre-requisite
2. Initialise cross-compilation via SDK
3. Prepare optee-os source code
4. Management of optee-os source code
5. Compile optee-os source code
6. Update software on board

1. Pre-requisite:
-----------------
OpenSTLinux SDK must be installed.

For optee-os build you need to install:
- Wand python and/or python crypto package
    Ubuntu: sudo apt-get install python-wand python-crypto python-pycryptopp
    Fedora: sudo yum install python-wand python-crypto
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

3. Prepare optee-os source:
------------------------
If you have the tarball and the list of patch then you must extract the
tarball and apply the patch.
    $> tar xfz ##BP##-##PR##.tar.gz
A new directory containing optee standard source code will be created, go into it:
    $> cd ##BP##

NB: if there is no git management on source code and you would like to have a git management
on the code see section 4 [Management of optee-os source code]
    if there is some patch, please apply it on source code
    $> for p in `ls -1 ../*.patch`; do patch -p1 < $p; done

4. Management of optee-os source code:
-----------------------------------
If you like to have a better management of change made on optee-os source, you
can use git:
    $ cd <optee-os source>
    $ test -d .git || git init . && git add . && git commit -m "optee-ossource code" && git gc
    $ git checkout -b WORKING
    $ for p in `ls -1 <path to patch>/*.patch`; do git am $p; done

MANDATORY: You must update sources
    $ cd <directory to optee-os source code>
    $ chmod 755 scripts/bin_to_c.py

NB: you can use directly the source from the community:
    URL: git://github.com/OP-TEE/optee_os.git
    Branch: ##GIT_BRANCH##
    Revision: ##GIT_SRCREV##

    $ git clone git://github.com/OP-TEE/optee_os.git
    $ cd <optee-os source>
    $ git checkout -b WORKING ##GIT_SRCREV##
    $ for p in `ls -1 <path to patch>/*.patch`; do git am $p; done

MANDATORY: You must update sources
    $ cd <directory to optee-os source code>
    $ chmod 755 scripts/bin_to_c.py

5. Build optee-os source code:
--------------------------------
To compile optee-os source code
    $> make -f $PWD/../Makefile.sdk
or for a specific config :
    $ make -f $PWD/../Makefile.sdk CFG_EMBED_DTB_SOURCE_FILE=stm32mp157c-ev1

Binaries generated are as follow: 
##> PWD/../build/tee-*-optee.stm32


6. Update software on board:
----------------------------
6.1. partitioning of binaries:
-----------------------------
Using the above command, the OP-TEE provides 3 binary files which MUST
be loaded in their respective partition as listed below:
- "tee-header-*-optee.stm32" in "teeh" partition
- "tee-pageable-*-optee.stm32" in "teed" partition
- "tee-pager-*-optee.stm32" in "teex" partition

6.2. Update via SDCARD:
-----------------------
Copy each binary to its dedicated partition, on SDCARD/USB disk
the OP-TEE partitions are the partitions 4/5/6:
 - SDCARD: /dev/mmcblkXp4 /dev/mmcblkXp5 /dev/mmcblkXp6
           (where X is the instance number)
 - SDCARD via USB reader: /dev/sdX4 /dev/sdX5 /dev/sdX6
                          (where X is the instance identifier)
So, for each binary:
$ dd if=<op-tee binary> of=/dev/<device partition> bs=1M conv=fdatasync

6.3. Update via USB mass storage on U-boot:
-------------------------------------------
* Plug the SDCARD on Board.
* Start the board and stop on U-boot shell:
 Hit any key to stop autoboot: 0
  STM32MP>
* plug an USB cable between the PC and the board via USB OTG port.
* On U-Boot shell, call the USB mass storage functionnality:
 STM32MP> ums 0 mmc 0

 ums <USB controller> <dev type: mmc|usb> <dev[:part]>
  ex.:
For SDCARD:      ums 0 mmc 0
For USB disk:    ums 0 usb 0

* Follow section 6.2 to load the "tee-*-optee.stm32" image files in the target
  partitions /dev/sd<X><Y>.



FAQ: Partitions identification

To find the partition associated to a specific label, connect the
SDCARD to your PC or run on target U-boot 'ums' command
and list /dev/disk/by-partlabel/ content, i.e:

  $ ls -l /dev/disk/by-partlabel/
  total 0
  lrwxrwxrwx 1 root root 15 Jan 23 19:11 bootfs -> ../../mmcblk0p7
  lrwxrwxrwx 1 root root 15 Jan 23 19:11 fsbl1 -> ../../mmcblk0p1     # FSBL (TF-A)
  lrwxrwxrwx 1 root root 15 Jan 23 19:11 fsbl2 -> ../../mmcblk0p2     # FSBL backup (TF-A backup – same content as FSBL)
  lrwxrwxrwx 1 root root 15 Jan 23 19:11 rootfs -> ../../mmcblk0p9
  lrwxrwxrwx 1 root root 15 Jan 23 19:11 ssbl -> ../../mmcblk0p3      # SSBL (U-Boot)
  lrwxrwxrwx 1 root root 15 Jan 23 19:11 teed -> ../../mmcblk0p5      # TEED (OP-TEE tee-pageable)
  lrwxrwxrwx 1 root root 15 Jan 23 19:11 teeh -> ../../mmcblk0p4      # TEEH (OP-TEE tee-header)
  lrwxrwxrwx 1 root root 15 Jan 23 19:11 teex -> ../../mmcblk0p6      # TEEX (OP-TEE tee-pager)
  lrwxrwxrwx 1 root root 16 Jan 23 19:11 userfs -> ../../mmcblk0p10
  lrwxrwxrwx 1 root root 15 Jan 23 19:11 vendorfs -> ../../mmcblk0p8