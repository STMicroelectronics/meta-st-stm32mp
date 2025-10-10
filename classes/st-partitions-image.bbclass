# Appends partition images generation to image build
#
# The format to specify it, in the machine, is:
#
# PARTITIONS_IMAGES ??= "partition_image_name_1 partition_image_name_2"
#
# The partition generation might be disabled by resetting ENABLE_PARTITIONS_IMAGE var
# in an image recipe (for example)
#

ENABLE_PARTITIONS_IMAGE ??= "1"

ENABLE_IMAGE_LICENSE_SUMMARY ??= "0"
ENABLE_MULTIVOLUME_UBI ??= "0"

PARTITIONS_IMAGES ??= ""

python __anonymous () {
    # We check first if it is requested to generate any partition images
    if d.getVar('ENABLE_PARTITIONS_IMAGE') != "1":
        bb.note('ENABLE_PARTITIONS_IMAGE not enabled')
        return

    import re

    # -----------------------------------------------------------------------------
    # Update the partition configuration set by user
    # -----------------------------------------------------------------------------
    # Init partition list from PARTITIONS_IMAGES
    image_partitions = []
    # Init image_summary_list
    image_summary_list = ''

    partitionsconfigflags = d.getVarFlags('PARTITIONS_IMAGES')
    # The "doc" varflag is special, we don't want to see it here
    partitionsconfigflags.pop('doc', None)
    partitionsconfig = (d.getVar('PARTITIONS_IMAGES') or "").split()
    if len(partitionsconfig) > 0:
        for config in partitionsconfig:
            for f, v in partitionsconfigflags.items():
                if config == f:
                    items = v.split(',')
                    # Make sure about PARTITIONS_IMAGES contents
                    if len(items) > 6:
                        bb.fatal('[PARTITIONS_IMAGES] Only image,label,mountpoint,size,type,[copy] can be specified!')
                    if items[0] == '':
                        bb.fatal('[PARTITIONS_IMAGES] Missing image setting')
                    if items[1] == '':
                        bb.fatal('[PARTITIONS_IMAGES] Missing label setting for %s image' % items[0])
                    if items[3] == '':
                        bb.fatal('[PARTITIONS_IMAGES] Missing size setting for %s image' % items[0])
                    # Update IMAGE vars for each partition image
                    if items[2] != '':
                        # Mount point is available, so we're dealing with partition image
                        bb.debug(1, "Set IMAGE_PARTITION_MOUNTPOINT to %s for %s partition image." % (items[2], items[0]))
                        d.setVar('IMAGE_PARTITION_MOUNTPOINT:pn-%s' % d.expand(items[0]), items[2])
                        # Append image to image_partitions list
                        image_partitions.append(d.expand(items[0]))
                        bb.debug(1, "Set IMAGE_ROOTFS_SIZE to %s for %s partition image." % (items[3], items[0]))
                        d.setVar('IMAGE_ROOTFS_SIZE:pn-%s' % d.expand(items[0]), items[3])

                        if d.expand(items[1])[-2:] != 'fs':
                            bb.debug(1, "Set IMAGE_NAME_SUFFIX to '.%sfs' for %s partition image." % (items[1], items[0]))
                            d.setVar('IMAGE_NAME_SUFFIX:pn-%s' % d.expand(items[0]), '.' + items[1] + 'fs')
                        else:
                            bb.debug(1, "Set IMAGE_NAME_SUFFIX to '.%s' for %s partition image." % (items[1], items[0]))
                            d.setVar('IMAGE_NAME_SUFFIX:pn-%s' % d.expand(items[0]), '.' + items[1])
                        bb.debug(1, "Set UBI_SECTION to %s for %s partition image." % (items[0], items[0]))
                        if d.getVar('UBI_SECTION:pn-%s' % d.expand(items[0])):
                            bb.debug(1,"UBI_SECTION is already configured to '%s' for %s partition image." % (d.getVar('UBI_SECTION:pn-%s' % d.expand(items[0])), items[0]))
                        else:
                            bb.debug(1, "Set UBI_SECTION to %s for %s partition image." % (items[0], items[0]))
                            d.setVar('UBI_SECTION:pn-%s' % d.expand(items[0]), items[0])
                        bb.debug(1, "Set UBI_VOLNAME to %s for %s partition image." % (items[1], items[0]))
                        if d.getVar('UBI_VOLNAME:pn-%s' % d.expand(items[0])):
                            bb.debug(1,"UBI_VOLNAME is already configured to '%s' for %s partition image." % (d.getVar('UBI_VOLNAME:pn-%s' % d.expand(items[0])), items[0]))
                        else:
                            bb.debug(1, "Set UBI_VOLNAME to %s for %s partition image." % (items[1], items[0]))
                            d.setVar('UBI_VOLNAME:pn-%s' % d.expand(items[0]), items[1])
                    # Manage IMAGE_SUMMARY_LIST configuration according to PARTITIONS_IMAGE set
                    if d.getVar('ENABLE_IMAGE_LICENSE_SUMMARY') == "1":
                        if items[2] != '':
                            if d.expand(items[1])[-2:] != 'fs':
                                image_summary_list += items[0] + ':' + items[2] + ':' + items[1] + 'fs;'
                            else:
                                image_summary_list += items[0] + ':' + items[2] + ':' + items[1] + ';'
                        else:
                            # Set '/' as default mountpoint for rootfs in IMAGE_SUMMARY_LIST
                            if d.expand(items[1])[-2:] != 'fs':
                                image_summary_list += items[0] + ':' + '/' + ':' + items[1] + 'fs;'
                            else:
                                image_summary_list += items[0] + ':' + '/' + ':' + items[1] + ';'

                    break

    images_multiubi_depends = d.getVar('STM32MP_UBI_VOLUME_IMAGE_DEPENDS') or ""
    if len(images_multiubi_depends) > 0:
        for img_dep in images_multiubi_depends.split():
            image_partitions.append(img_dep)

    # Reset IMAGE_LIST_SUMMARY with computed partition configuration
    if d.getVar('ENABLE_IMAGE_LICENSE_SUMMARY') == "1":
        bb.debug(1, "Set IMAGE_SUMMARY_LIST with configuration: %s." % image_summary_list)
        d.setVar('IMAGE_SUMMARY_LIST', image_summary_list)

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
                initrd = d.getVar('INITRD_IMAGE_ALL') or d.getVar('INITRD_IMAGE') or ""
                # We need to append partition images generation only to image
                # that are not one of the defined partitions and not the InitRAMFS image.
                # Without this check we would create circular dependency
                if current_image_name not in image_partitions and current_image_name != initramfs and current_image_name not in initrd:
                    for partition in image_partitions:
                        bb.debug(1, "Appending %s image build to 'do_image' depends tasks." % partition)
                        # We need to make sure the manifest file is deployed as we need it for 'image_rootfs_image_clean_task'
                        d.appendVarFlag('do_image', 'depends', ' %s:do_populate_lic_deploy' % partition)
                    bb.debug(1, "Appending 'image_rootfs_image_clean_task' to IMAGE_PREPROCESS_COMMAND.")
                    d.appendVar('IMAGE_PREPROCESS_COMMAND', ' image_rootfs_image_clean_task ')
                    bb.debug(1, "Set DEPLOY_BUILDINFO_FILE to '1' to allow to deploy build info file for rootfs build.")
                    d.setVar('DEPLOY_BUILDINFO_FILE', '1')
                    # Manage multiubi volume build enable for current image
                    if bb.utils.contains('IMAGE_FSTYPES', 'multiubi', True, False, d) and d.getVar('ENABLE_MULTIVOLUME_UBI') == "1":
                        bb.debug(1, "Appending 'st_multivolume_ubifs' to IMAGE_POSTPROCESS_COMMAND.")
                        d.appendVar('IMAGE_POSTPROCESS_COMMAND', ' st_multivolume_ubifs ')

    # -----------------------------------------------------------------------------
    # Make sure that 'wic' image fstype is properly configured for partition image handling
    # -----------------------------------------------------------------------------
    if len(image_partitions) > 0:
        # Gather all current tasks
        tasks = filter(lambda k: d.getVarFlag(k, "task", True), d.keys())
        for task in tasks:
            # Check that we are dealing with image recipe
            if task == 'do_image':
                # Init current image name
                current_image_name = d.getVar('PN') or ""
                # We need to make sure that none 'wic' image fstypes are still
                # avaialbe in IMAGE_FSTYPES to avoid circular dependency
                if current_image_name in image_partitions:
                    for fstype in d.getVar('IMAGE_FSTYPES').split():
                        if re.match(r'^wic(\..*|$)', fstype):
                            bb.fatal('\n\
[PARTITIONS_IMAGES] Circular dependency issue found for partition image build!\n\
[PARTITIONS_IMAGES] Please make sure to add all related wic image type(s) to\n\
[PARTITIONS_IMAGES] WKS_IMAGE_FSTYPE var to properly remove them for partition\n\
[PARTITIONS_IMAGES] image build.\n\
[PARTITIONS_IMAGES] IMAGE_FSTYPES    : %s\n\
[PARTITIONS_IMAGES] WKS_IMAGE_FSTYPES: %s' % (d.getVar('IMAGE_FSTYPES'), d.getVar('WKS_IMAGE_FSTYPES')))
}

python image_rootfs_image_clean_task(){
    import re;
    import subprocess
    import shutil

    deploy_image_dir = d.expand("${DEPLOY_DIR}")
    machine = d.expand("${MACHINE}")
    image_machine_suffix = d.getVar('IMAGE_MACHINE_SUFFIX') or ""
    distro = d.expand("${DISTRO}")
    img_rootfs = d.getVar('IMAGE_ROOTFS')
    partitionsconfigflags = d.getVarFlags('PARTITIONS_IMAGES')
    partitionsconfig = (d.getVar('PARTITIONS_IMAGES') or "").split()

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
            _img_suffix=d.expand(items[1])

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
                         _img_partition + "-" + distro + image_machine_suffix + "."+ _img_suffix +".manifest")
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
                package = ""
                try:
                    package = subprocess.check_output(cmd, stderr=subprocess.STDOUT).decode("utf-8").rstrip('\n')
                    package = re.sub(r":.*", "", package)
                except subprocess.CalledProcessError as e:
                    bb.note("Cannot check package for file %s" % (os.path.join(root, f)))

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
                        if package == line.split()[0]:
                            match = True
                            break
                    if not match:
                        bb.note("Package %s should belong to %s partition image. Did you add it into the right image?" % (package, _img_partition))

                else:
                    bb.note("File %s is not in a package" % (os.path.join(root, f)))

            bb.note('Expecting to clean folder: %s' % part_dir)
            shutil.rmtree(part_dir)
            # directory is also removed. Re-create mount point
            os.mkdir(part_dir)
            bb.note('>>> Done')
}

# -----------------------------------------------------------------------------
# Manage to export to DEPLOYDIR the buildinfo file itself
# -----------------------------------------------------------------------------
DEPLOY_BUILDINFO_FILE ??= "0"

python extract_buildinfo() {
    if d.getVar('DEPLOY_BUILDINFO_FILE') == '1' and d.getVar('IMAGE_BUILDINFO_FILE'):
        # Export build information to deploy dir
        import shutil
        buildinfo_origin = d.getVar('IMAGE_BUILDINFO_FILE')
        rootfs_path = d.getVar('IMAGE_ROOTFS')
        buildinfo_srcfile = os.path.normpath(rootfs_path + '/' + buildinfo_origin)
        if os.path.isfile(buildinfo_srcfile):
            buildinfo_deploy = os.path.basename(d.getVar('IMAGE_BUILDINFO_FILE')) + '-' + d.getVar('IMAGE_LINK_NAME').replace(d.getVar('IMAGE_NAME_SUFFIX'), '')
            buildinfo_dstfile = os.path.join(d.getVar('IMGDEPLOYDIR'), buildinfo_deploy)
            shutil.copy2(buildinfo_srcfile, buildinfo_dstfile)
        else:
            bb.warn('Not able to locate %s file in image rootfs %s' % (buildinfo_origin, rootfs_path))
}
IMAGE_PREPROCESS_COMMAND += "extract_buildinfo;"
