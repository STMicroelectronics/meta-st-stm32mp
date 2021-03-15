# Appends partition images generation to image build
#
# The format to specify it, in the machine, is:
#
# PARTITIONS_IMAGE ??= "partition_image_name_1 partition_image_name_2"
#
# The partition generation might be disabled by resetting ENABLE_PARTITIONS_IMAGE var
# in an image recipe (for example)
#

ENABLE_PARTITIONS_IMAGE ?= "1"

PARTITIONS_CONFIG ??= ""
PARTITIONS_IMAGE ??= ""
PARTITIONS_MOUNTPOINT ??= ""

python __anonymous () {
    # We check first if it is requested to generate any partition images
    if d.getVar('ENABLE_PARTITIONS_IMAGE') != "1":
        bb.note('ENABLE_PARTITIONS_IMAGE not enabled')
        return

    # -----------------------------------------------------------------------------
    # Update the partition configuration set by user
    # -----------------------------------------------------------------------------
    partitionsconfigflags = d.getVarFlags('PARTITIONS_CONFIG')
    # The "doc" varflag is special, we don't want to see it here
    partitionsconfigflags.pop('doc', None)
    partitionsconfig = (d.getVar('PARTITIONS_CONFIG') or "").split()
    # Init image_summary_list
    image_summary_list = ''
    if len(partitionsconfig) > 0:
        for config in partitionsconfig:
            for f, v in partitionsconfigflags.items():
                if config == f:
                    items = v.split(',')
                    if items[0]:
                        if len(items) > 5:
                            raise bb.parse.SkipRecipe('Only image,label,mountpoint,size,type can be specified!')
                        # Make sure that we're dealing with partition image and not rootfs image
                        if len(items) > 2 and items[2]:
                            # Mount point available, so we're dealing with partition image
                            # PARTITIONS_IMAGE appending
                            bb.debug(1, "Appending '%s' to PARTITIONS_IMAGE." % items[0])
                            d.appendVar('PARTITIONS_IMAGE', ' ' + items[0])
                            # PARTITIONS_MOUNTPOINT appending
                            bb.debug(1, "Appending '%s' to PARTITIONS_MOUNTPOINT." % items[2])
                            d.appendVar('PARTITIONS_MOUNTPOINT', ' ' + items[2])

                        # Update IMAGE vars for each partition image
                        if items[1]:
                            bb.debug(1, "Set UBI_VOLNAME to %s for %s partition image." % (items[1], items[0]))
                            d.setVar('UBI_VOLNAME_pn-%s' % d.expand(items[0]), items[1])
                            if d.expand(items[1])[-2:] != 'fs':
                                bb.debug(1, "Set IMAGE_NAME_SUFFIX to '.%sfs' for %s partition image." % (items[1], items[0]))
                                d.setVar('IMAGE_NAME_SUFFIX_pn-%s' % d.expand(items[0]), '.' + items[1] + 'fs')
                            else:
                                bb.debug(1, "Set IMAGE_NAME_SUFFIX to '.%s' for %s partition image." % (items[1], items[0]))
                                d.setVar('IMAGE_NAME_SUFFIX_pn-%s' % d.expand(items[0]), '.' + items[1])
                        else:
                            bb.fatal('[PARTITIONS_CONFIG] Missing label setting for %s image' % items[0])
                        if items[2]:
                            bb.debug(1, "Set IMAGE_PARTITION_MOUNTPOINT to %s for %s partition image." % (items[2], items[0]))
                            d.setVar('IMAGE_PARTITION_MOUNTPOINT_pn-%s' % d.expand(items[0]), items[2])
                        if items[3]:
                            if items[2]:
                                # Mount point available, so we're dealing with partition image
                                bb.debug(1, "Set IMAGE_ROOTFS_SIZE to %s for %s partition image." % (items[3], items[0]))
                                d.setVar('IMAGE_ROOTFS_SIZE_pn-%s' % d.expand(items[0]), items[3])
                        else:
                            bb.fatal('[PARTITIONS_CONFIG] Missing size setting for %s image' % items[0])

                        # Manage IMAGE_SUMMARY_LIST configuration according to PARTITIONS_CONFIG set
                        if d.getVar('ENABLE_IMAGE_LICENSE_SUMMARY') == "1":
                            if not items[2]:
                                # Set '/' as default mountpoint for rootfs in IMAGE_SUMMARY_LIST
                                items[2] = '/'
                            image_summary_list += items[0] + ':' + items[2] + ';'

                        # Manage multiubi volume list STM32MP_UBI_VOLUME
                        if bb.utils.contains('IMAGE_FSTYPES', 'stmultiubi', True, False, d) and d.getVar('ENABLE_MULTIVOLUME_UBI') == "1":
                            bb.debug(1, "Appending '%s' image with %s size to STM32MP_UBI_VOLUME." % (items[0], items[3]))
                            d.appendVar('STM32MP_UBI_VOLUME', ' ' + items[0] + ':' + items[3])

                    else:
                        bb.fatal('[PARTITIONS_CONFIG] Missing image setting')

                    break

    # Reset IMAGE_LIST_SUMMARY with computed partition configuration
    if d.getVar('ENABLE_IMAGE_LICENSE_SUMMARY') == "1":
        bb.debug(1, "Set IMAGE_SUMMARY_LIST with configuration: %s." % image_summary_list)
        d.setVar('IMAGE_SUMMARY_LIST', image_summary_list)

    # Init partition list from PARTITIONS_IMAGE
    image_partitions = (d.getVar('PARTITIONS_IMAGE') or "").split()
    # -----------------------------------------------------------------------------
    # Make sure to append the partition build to current image target
    # -----------------------------------------------------------------------------
    if len(image_partitions) > 0:
        # Gather all current tasks
        tasks = filter(lambda k: d.getVarFlag(k, "task", True), d.keys())
        for task in tasks:
            # Check that we are dealing with image recipe
            if task == 'do_image_complete':
                # Init current image name
                current_image_name = d.getVar('PN') or ""
                # Init RAMFS image if any
                initramfs = d.getVar('INITRAMFS_IMAGE') or ""
                # Init INITRD image if any
                initrd = d.getVar('INITRD_IMAGE') or ""
                # We need to append partition images generation only to image
                # that are not one of the defined partitions and not the InitRAMFS image.
                # Without this check we would create circular dependency
                if current_image_name not in image_partitions and current_image_name != initramfs and current_image_name != initrd:
                    for partition in image_partitions:
                        bb.debug(1, "Appending %s image build to 'do_image' depends tasks." % partition)
                        # We need to make sure the manifest file is deployed as we need it for 'image_rootfs_image_clean_task'
                        d.appendVarFlag('do_image', 'depends', ' %s:do_populate_lic_deploy' % partition)
                    bb.debug(1, "Appending 'image_rootfs_image_clean_task' to IMAGE_PREPROCESS_COMMAND.")
                    d.appendVar('IMAGE_PREPROCESS_COMMAND', 'image_rootfs_image_clean_task;')
                    bb.debug(1, "Set DEPLOY_BUILDINFO_FILE to '1' to allow to deploy build info file for rootfs build.")
                    d.setVar('DEPLOY_BUILDINFO_FILE', '1')
                    # Manage multiubi volume build enable for current image
                    if bb.utils.contains('IMAGE_FSTYPES', 'stmultiubi', True, False, d) and d.getVar('ENABLE_MULTIVOLUME_UBI') == "1":
                        bb.debug(1, "Appending 'st_multivolume_ubifs' to IMAGE_POSTPROCESS_COMMAND.")
                        d.appendVar('IMAGE_POSTPROCESS_COMMAND', 'st_multivolume_ubifs;')
}

python image_rootfs_image_clean_task(){
    import re;
    import subprocess
    import shutil

    deploy_image_dir = d.expand("${DEPLOY_DIR}")
    machine = d.expand("${MACHINE}")
    distro = d.expand("${DISTRO}")
    img_rootfs = d.getVar('IMAGE_ROOTFS')
    partitionsconfigflags = d.getVarFlags('PARTITIONS_CONFIG')
    partitionsconfig = (d.getVar('PARTITIONS_CONFIG') or "").split()

    if len(partitionsconfig) == 0:
        bb.note('No partition image: nothing more to do...')
        return

    for config in partitionsconfig:
        for f, v in partitionsconfigflags.items():
            if config != f:
                continue

            items = v.split(',')
            _img_partition=d.expand(items[0])
            _img_mountpoint=d.expand(items[2])

            # Do not search for the rootfs
            if not items[2]:
                bb.note('Do not search for rootfs image')
                continue

            bb.note('Manage package check for %s mount point from %s partition image...' % (_img_partition, _img_mountpoint))

            part_dir=os.path.join(img_rootfs, re.sub(r"^/", "", _img_mountpoint))
            if not os.path.exists(part_dir):
                bb.note('The %s mountpoint is not populated on rootfs. Nothing to do.' % part_dir)
                continue

            # Discover all files in folder and sub-folder
            list_file = []
            for root, subfolder, files in os.walk(part_dir):
                for f in files:
                    list_file.append(re.sub(r"%s" % img_rootfs, "", os.path.join(root, f)))

            if not list_file:
                bb.note('No file found in current mount point %s: nothing to do' % part_dir)
                continue

            # Manifest file of the partition to check packages are in that partition
            manif_file = os.path.join(deploy_image_dir, "images", machine,
                         _img_partition + "-" + distro + "-" + machine + ".manifest")
            try:
                manifest_content = open(manif_file, "r")
                contents = manifest_content.read().splitlines()
                manifest_content.close()
                if not contents:
                    bb.fatal('Manifest associated to partition %s is empty.' \
                             ' No package verification can be on on that partition' % _img_partition)
            except Exception as e:
                bb.fatal("Unable to read %s file content: %s" % (manif_file, e))
            except IOError:
                bb.fatal("File %s does not exist" % (manif_file))

            # To speed up the process, save the list of processed files to avoid to check them again
            package_file_list = []

            for f in list_file:
                if f in package_file_list:
                    continue

                # Use oe-pkgdata-util to find the package providing a file
                cmd = ["oe-pkgdata-util",
                    "-p", d.getVar('PKGDATA_DIR'), "find-path", f ]
                try:
                    package = subprocess.check_output(cmd, stderr=subprocess.STDOUT).decode("utf-8").rstrip('\n')
                    package = re.sub(r":.*", "", package)
                except subprocess.CalledProcessError as e:
                    bb.fatal("Cannot check package for file %s" % (os.path.join(root, f)))

                if package:
                    # Use oe-pkgdata-util to list all files provided by a package
                    cmd = ["oe-pkgdata-util",
                        "-p", d.getVar('PKGDATA_DIR'), "list-pkg-files", package]
                    try:
                        package_filelist = subprocess.check_output(cmd, stderr=subprocess.STDOUT).decode("utf-8")
                        package_filelist = package_filelist.split()
                    except subprocess.CalledProcessError as e:
                        bb.fatal("Cannot read files inside package %s" % package)

                    # Save processed files
                    package_file_list = package_file_list + package_filelist

                    # Check the package is in the manifest of the partition
                    match = False
                    for line in contents:
                        if re.match('^%s ' % package, line):
                            match = True
                            break
                    if not match:
                        bb.warn("Package %s should belong to %s partition image. Did you add it into the right image?" % (package, _img_partition))

                else:
                    bb.warn("File %s is not in a package" % (os.path.join(root, f)))

            bb.note('Expecting to clean folder: %s' % part_dir)
            shutil.rmtree(part_dir)
            # directory is also removed. Re-create mount point
            os.mkdir(part_dir)
            bb.note('>>> Done')
}

# -----------------------------------------------------------------------------
# Append buildinfo() to allow to export to DEPLOYDIR the buildinfo file itself
# -----------------------------------------------------------------------------
DEPLOY_BUILDINFO_FILE ??= "0"

buildinfo_append() {
    if d.getVar('DEPLOY_BUILDINFO_FILE') != '1':
        return
    # Export build information to deploy dir
    import shutil
    buildinfo_srcfile=d.expand('${IMAGE_ROOTFS}${IMAGE_BUILDINFO_FILE}')
    buildinfo_dstfile=os.path.join(d.getVar('IMGDEPLOYDIR'), os.path.basename(d.getVar('IMAGE_BUILDINFO_FILE')) + '-' + d.getVar('IMAGE_LINK_NAME'))
    if os.path.isfile(buildinfo_srcfile):
        shutil.copy2(buildinfo_srcfile, buildinfo_dstfile)
    else:
        bb.warn('Not able to locate %s file in image rootfs %s' % (d.getVar('IMAGE_BUILDINFO_FILE'), d.getVar('IMAGE_ROOTFS')))
}
