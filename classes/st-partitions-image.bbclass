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
PARTITIONS_IMAGE ?= ""

python __anonymous () {
    if d.getVar('ENABLE_PARTITIONS_IMAGE') != "1":
        return

    # Init partition list from PARTITIONS_IMAGE
    image_partitions = (d.getVar('PARTITIONS_IMAGE') or "").split()

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

                # We need to append partition images generation only to image
                # that are not one of the defined partitions and not the InitRAMFS image.
                # Without this check we would create circular dependency
                if current_image_name not in image_partitions and current_image_name != initramfs:
                    for partition in image_partitions:
                        d.appendVarFlag('do_image_complete', 'depends', ' %s:do_image_complete' % partition)
}

image_rootfs_image_clean_task () {
    for name in ${PARTITIONS_IMAGE};
    do
        if `echo ${IMAGE_NAME} | grep -q $name` ;
        then
            return;
        fi
    done
    bbnote "Clean mount point on ${IMAGE_NAME}:"
    LIST=`ls -l ${IMAGE_ROOTFS}`
    for dir in ${PARTITIONS_MOUNTPOINT_IMAGE};
    do
        bbnote "$dir on ${IMAGE_NAME} are cleanned because it's a mount point."
        rm -rf ${IMAGE_ROOTFS}/$dir/*
    done
}
IMAGE_PREPROCESS_COMMAND_append = " image_rootfs_image_clean_task; "
