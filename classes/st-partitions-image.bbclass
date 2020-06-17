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
                            bb.debug(1, "Set IMAGE_ROOTFS_SIZE to %s for %s partition image." % (items[3], items[0]))
                            d.setVar('IMAGE_ROOTFS_SIZE_pn-%s' % d.expand(items[0]), items[3])
                        else:
                            bb.fatal('[PARTITIONS_CONFIG] Missing size setting for %s image' % items[0])

                        # Manage IMAGE_SUMMARY_LIST configuration according to PARTITION_CONFIG set
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
                        bb.debug(1, "Appending %s image build to 'do_image_complete' depends tasks." % partition)
                        d.appendVarFlag('do_image_complete', 'depends', ' %s:do_image_complete' % partition)
                    bb.debug(1, "Appending 'image_rootfs_image_clean_task' to IMAGE_PREPROCESS_COMMAND.")
                    d.appendVar('IMAGE_PREPROCESS_COMMAND', 'image_rootfs_image_clean_task;')
                    # Manage multiubi volume build enable for current image
                    if bb.utils.contains('IMAGE_FSTYPES', 'stmultiubi', True, False, d) and d.getVar('ENABLE_MULTIVOLUME_UBI') == "1":
                        bb.debug(1, "Appending 'st_multivolume_ubifs' to IMAGE_POSTPROCESS_COMMAND.")
                        d.appendVar('IMAGE_POSTPROCESS_COMMAND', 'st_multivolume_ubifs;')
}

image_rootfs_image_clean_task() {
    bbnote "PARTITIONS_IMAGE"
    bbnote ">>> ${PARTITIONS_IMAGE}"
    bbnote "PARTITIONS_MOUNTPOINT"
    bbnote ">>> ${PARTITIONS_MOUNTPOINT}"
    unset i j
    for img in ${PARTITIONS_IMAGE}; do
        i=$(expr $i + 1);
        for part in ${PARTITIONS_MOUNTPOINT}; do
            j=$(expr $j + 1);
            if [ $j -eq $i ]; then
                bbnote "Expecting to clean folder:"
                bbnote ">>> ${IMAGE_ROOTFS}/$part"
                if [ -d ${IMAGE_ROOTFS}/$part ]; then
                    rm -rf ${IMAGE_ROOTFS}/$part/*
                    bbnote ">>> DONE"
                else
                    bbnote ">>> NOT DONE : $part folder doesn't exist in image rootfs"
                fi
            fi
        done
        unset j
    done
    unset i
}
