inherit core-image

# Disable flashlayout generation for the partition image as this is supposed
# to be done only for complete image
ENABLE_FLASHLAYOUT_CONFIG = "0"

# Disable image license summary generation for the partition image as this is
# supposed to be done only for complete image
ENABLE_IMAGE_LICENSE_SUMMARY = "0"

# Remove WIC image generation for the partition image
IMAGE_FSTYPES:remove = "${WKS_IMAGE_FSTYPES}"

# Append DISTRO to image name even if we're not using ST distro setting
# Mandatory to ease flashlayout file configuration
IMAGE_BASENAME:append = "${@'' if 'openstlinuxcommon' in OVERRIDES.split(':') else '-${DISTRO}'}"

# Reset image feature
IMAGE_FEATURE = ""

# Reset PACKAGE_INSTALL to avoid getting installed packages added in machine through IMAGE_INSTALL:append:
PACKAGE_INSTALL = ""

# Reset LINGUAS_INSTALL to avoid getting installed any locale-base package
LINGUAS_INSTALL = ""
IMAGE_LINGUAS = ""

# Reset LDCONFIG to avoid runing ldconfig on image.
LDCONFIGDEPEND = ""

# Remove from IMAGE_PREPROCESS_COMMAND useless buildinfo
IMAGE_PREPROCESS_COMMAND:remove = "buildinfo;"
# Remove from IMAGE_PREPROCESS_COMMAND the prelink_image as it could be run after
# we clean rootfs folder leading to cp error if '/etc/' folder is missing:
#   cp: cannot create regular file
#   ‘/local/YOCTO/build/tmp-glibc/work/stm32mp1-openstlinux_weston-linux-gnueabi/st-image-userfs/1.0-r0/rootfs/etc/prelink.conf’:
#   No such file or directory
IMAGE_PREPROCESS_COMMAND:remove = "prelink_image;"

IMAGE_PREPROCESS_COMMAND:append = "reformat_rootfs;"

# Cleanup rootfs newly created
reformat_rootfs() {
    if [ -d ${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT} ]; then
        bbnote "Mountpoint ${IMAGE_PARTITION_MOUNTPOINT} found in ${IMAGE_ROOTFS}"
        bbnote ">>> Remove all files and folder except ${IMAGE_PARTITION_MOUNTPOINT}"
        TARGETROOTFS=${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT}
        while [ "${TARGETROOTFS}" != "${IMAGE_ROOTFS}" ]
        do
            find $(dirname ${TARGETROOTFS})/ -mindepth 1 ! -regex "^${TARGETROOTFS}\(/.*\)?" -delete
            TARGETROOTFS=$(dirname ${TARGETROOTFS})
        done
        bbnote ">>> Move ${IMAGE_PARTITION_MOUNTPOINT} contents to ${IMAGE_ROOTFS}"
        mv ${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT}/* ${IMAGE_ROOTFS}/
        bbnote ">>> Remove remaining ${IMAGE_PARTITION_MOUNTPOINT} folder"
        # Remove empty boot folder
        TARGETROOTFS=${IMAGE_ROOTFS}${IMAGE_PARTITION_MOUNTPOINT}
        while [ "${TARGETROOTFS}" != "${IMAGE_ROOTFS}" ]
        do
            bbnote ">>> Delete ${TARGETROOTFS}"
            rm -rf ${TARGETROOTFS}/
            TARGETROOTFS=$(dirname ${TARGETROOTFS})
        done
    else
        bbnote "${IMAGE_PARTITION_MOUNTPOINT} folder not available in rootfs folder, no reformat done..."
    fi
}
