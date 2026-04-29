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

    if (d.getVar('ST_PARTITIONS_LABELS') or "").split():
        raise bb.parse.SkipRecipe("You cannot use ST_PARTITIONS_LABELS as it is internal for var expansion.")
    if (d.getVar('ST_PARTITIONS_MOUNTS_POINTS') or "").split():
        raise bb.parse.SkipRecipe("You cannot use ST_PARTITIONS_MOUNTS_POINTS as it is internal for var expansion.")
    if (d.getVar('ST_PARTITIONS_SIZES') or "").split():
        raise bb.parse.SkipRecipe("You cannot use ST_PARTITIONS_SIZES as it is internal for var expansion.")
    if (d.getVar('ST_PARTITIONS_NOT_SUPPORTED_FS') or "").split():
        raise bb.parse.SkipRecipe("You cannot use ST_PARTITIONS_NOT_SUPPORTED_FS as it is internal for var expansion.")

    if (d.getVar('ST_PARTITIONS_PARTITION_FS_SUPPORTED') or "").split():
        raise bb.parse.SkipRecipe("You cannot use ST_PARTITIONS_PARTITION_FS_SUPPORTED as it is internal for var expansion.")


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
                    if len(items) > 7:
                        bb.fatal('[PARTITIONS_IMAGES] Only image,label,mountpoint,size,type,[copy],[limit fs] can be specified! {}={}'.format(len(items), items) )
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

                    # for st splitted partitions
                    bb.debug(1, "Appending '%s' to ST_PARTITIONS_LABELS" % items[1])
                    d.appendVar('ST_PARTITIONS_LABELS', items[1] + ',')

                    bb.debug(1, "Appending '%s' to ST_PARTITIONS_MOUNTS_POINTS" % items[2])
                    d.appendVar('ST_PARTITIONS_MOUNTS_POINTS', items[2] + ',')

                    bb.debug(1, "Appending '%s' to ST_PARTITIONS_SIZES" % items[3])
                    d.appendVar('ST_PARTITIONS_SIZES', items[3] + ',')
                    if len(items) > 6:
                        # indicate is a fs is not suppoted by a specific partition
                        bb.debug(1, "Appending '%s' to ST_PARTITIONS_NOT_SUPPORTED_FS" % items[6])
                        d.appendVar('ST_PARTITIONS_NOT_SUPPORTED_FS', items[6] + ',')
                    else:
                        d.appendVar('ST_PARTITIONS_NOT_SUPPORTED_FS', ',')
                    break

    images_multiubi_depends = d.getVar('STM32MP_UBI_VOLUME_IMAGE_DEPENDS') or ""
    distro = d.getVar('DISTRO') or ""
    if len(images_multiubi_depends) > 0:
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
                if current_image_name not in images_multiubi_depends:
                    for img_dep in images_multiubi_depends.split():
                        bb.debug(1, "Appending %s image build to 'do_image' depends tasks." % img_dep)
                        d.appendVarFlag('do_image', 'depends', ' %s:do_populate_lic_deploy' % img_dep.replace("-%s" % distro, ''))
    images_depends = d.getVar('STM32MP_IMAGE_DEPENDS') or ""
    if len(images_depends) > 0:
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
                if current_image_name not in images_depends:
                    for img_dep in images_depends.split():
                        bb.debug(1, "Appending %s image build to 'do_image' depends tasks." % img_dep)
                        d.appendVarFlag('do_image', 'depends', ' %s:do_populate_lic_deploy' % img_dep.replace("-%s" % distro, ''))

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
                    bb.debug(1, "Set DEPLOY_BUILDINFO_FILE to '1' to allow to deploy build info file for rootfs build.")
                    d.setVar('DEPLOY_BUILDINFO_FILE', '1')

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
