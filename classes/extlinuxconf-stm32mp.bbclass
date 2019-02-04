# Copyright (C) 2017, STMicroelectronics - All Rights Reserved
# Released under the MIT license (see COPYING.MIT for the terms)
#
# --------------------------------------------------------------------
# Extract from openembedded-core 'uboot-extlinux-config.bbclass' class
# --------------------------------------------------------------------
# External variables:
#
# UBOOT_EXTLINUX_CONSOLE           - Set to "console=ttyX" to change kernel boot
#                                    default console.
# UBOOT_EXTLINUX_LABELS            - A list of targets for the automatic config.
# UBOOT_EXTLINUX_KERNEL_ARGS       - Add additional kernel arguments.
# UBOOT_EXTLINUX_KERNEL_IMAGE      - Kernel image name.
# UBOOT_EXTLINUX_FDTDIR            - Device tree directory.
# UBOOT_EXTLINUX_FDT               - Device tree file.
# UBOOT_EXTLINUX_INITRD            - Indicates a list of filesystem images to
#                                    concatenate and use as an initrd (optional).
# UBOOT_EXTLINUX_MENU_DESCRIPTION  - Name to use as description.
# UBOOT_EXTLINUX_ROOT              - Root kernel cmdline.
# UBOOT_EXTLINUX_TIMEOUT           - Timeout before DEFAULT selection is made.
#                                    Measured in 1/10 of a second.
# UBOOT_EXTLINUX_DEFAULT_LABEL     - Target to be selected by default after
#                                    the timeout period
#
# If there's only one label system will boot automatically and menu won't be
# created. If you want to use more than one labels, e.g linux and alternate,
# use overrides to set menu description, console and others variables.
#
# --------------------------------------------------------------------
# STM32MP specific implementation
# --------------------------------------------------------------------
# Append new mechanism to allow multi 'extlinux.conf' file generation.
#   - multiple targets case:
#     each 'extlinux.conf' file generated is created under specific path:
#       '${B}/<UBOOT_EXTLINUX_BOOTPREFIXES>extlinux/extlinux.conf'
#   - simple target case:
#     the 'extlinux.conf' file generated is created under default path:
#       '${B}/extlinux/extlinux.conf'
#
# New external variables added:
# UBOOT_EXTLINUX_TARGETS           - A list of targets for multi config file
#                                    generation
# UBOOT_EXTLINUX_BOOTPREFIXES      - Bootprefix used in uboot script to select
#                                    extlinux.conf file to use
#
# --------------------------------------------------------------------
# Output example:
# --------------------------------------------------------------------
# Following 'extlinux.conf' files are generated under ${UBOOT_EXTLINUX_INSTALL_DIR}:
#   ${UBOOT_EXTLINUX_BOOTPREFIXES_${UBOOT_EXTLINUX_TARGETS}[0]}extlinux/extlinux.conf
#   ${UBOOT_EXTLINUX_BOOTPREFIXES_${UBOOT_EXTLINUX_TARGETS}[1]}extlinux/extlinux.conf
#
# File content (${UBOOT_EXTLINUX_BOOTPREFIXES_${UBOOT_EXTLINUX_TARGETS}[0]}extlinux/exlinux.conf):
#   menu title Select the boot mode
#   TIMEOUT ${UBOOT_EXTLINUX_TIMEOUT}
#   DEFAULT ${UBOOT_EXTLINUX_DEFAULT_LABEL_${UBOOT_EXTLINUX_TARGETS}[0]}
#   LABEL ${UBOOT_EXTLINUX_LABELS_${UBOOT_EXTLINUX_TARGETS}[0]}[0]
#       KERNEL ${UBOOT_EXTLINUX_KERNEL}     < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_KERNEL_${IMAGE_UBOOT_EXTLINUX_LABELS}[0]}  >
#       FDT ${UBOOT_EXTLINUX_FDT}           < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_FDT_${IMAGE_UBOOT_EXTLINUX_LABELS}[0]}     >
#       APPEND ${UBOOT_EXTLINUX_ROOT}       < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_ROOT_${IMAGE_UBOOT_EXTLINUX_LABELS}[0]}    >
#   LABEL ${UBOOT_EXTLINUX_LABELS_${UBOOT_EXTLINUX_TARGETS}[0]}[1]
#       KERNEL ${UBOOT_EXTLINUX_KERNEL}     < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_KERNEL_${IMAGE_UBOOT_EXTLINUX_LABELS}[1]}  >
#       FDT ${UBOOT_EXTLINUX_FDT}           < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_FDT_${IMAGE_UBOOT_EXTLINUX_LABELS}[1]}     >
#       APPEND ${UBOOT_EXTLINUX_ROOT}       < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_ROOT_${IMAGE_UBOOT_EXTLINUX_LABELS}[1]}    >
#
# File content (${UBOOT_EXTLINUX_BOOTPREFIXES_${UBOOT_EXTLINUX_TARGETS}[0]}extlinux/exlinux.conf):
#   menu title Select the boot mode
#   TIMEOUT ${UBOOT_EXTLINUX_TIMEOUT}
#   DEFAULT ${UBOOT_EXTLINUX_DEFAULT_LABEL_${UBOOT_EXTLINUX_TARGETS}[1]}
#   LABEL ${UBOOT_EXTLINUX_LABELS_${UBOOT_EXTLINUX_TARGETS}[1]}[0]
#       KERNEL ${UBOOT_EXTLINUX_KERNEL}     < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_KERNEL_${IMAGE_UBOOT_EXTLINUX_LABELS}[0]}  >
#       FDT ${UBOOT_EXTLINUX_FDT}           < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_FDT_${IMAGE_UBOOT_EXTLINUX_LABELS}[0]}     >
#       APPEND ${UBOOT_EXTLINUX_ROOT}       < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_ROOT_${IMAGE_UBOOT_EXTLINUX_LABELS}[0]}    >
#   LABEL ${UBOOT_EXTLINUX_LABELS_${UBOOT_EXTLINUX_TARGETS}[1]}[1]
#       KERNEL ${UBOOT_EXTLINUX_KERNEL}     < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_KERNEL_${IMAGE_UBOOT_EXTLINUX_LABELS}[1]}  >
#       FDT ${UBOOT_EXTLINUX_FDT}           < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_FDT_${IMAGE_UBOOT_EXTLINUX_LABELS}[1]}     >
#       APPEND ${UBOOT_EXTLINUX_ROOT}       < OR OVERRIDE WITH :    ${UBOOT_EXTLINUX_ROOT_${IMAGE_UBOOT_EXTLINUX_LABELS}[1]}    >
# --------------------------------------------------------------------

UBOOT_EXTLINUX_TARGETS ?= ""

UBOOT_EXTLINUX_CONSOLE ??= "console=${console}"
UBOOT_EXTLINUX_LABELS ??= "linux"
UBOOT_EXTLINUX_FDT ??= ""
UBOOT_EXTLINUX_FDTDIR ??= "../"
UBOOT_EXTLINUX_KERNEL_IMAGE ?= "/${KERNEL_IMAGETYPE}"
UBOOT_EXTLINUX_KERNEL_ARGS ?= "rootwait rw"
UBOOT_EXTLINUX_TIMEOUT ?= "20"

UBOOT_EXTLINUX_CONFIGURE_FILES ??= ""

python update_extlinuxconf_targets() {
    """
    Append dynamically to UBOOT_EXTLINUX_TARGETS new target list generated from
    config flag list (UBOOT_EXTLINUX_CONFIG_FLAGS) and supported devicetree list
    for each flag (UBOOT_EXTLINUX_DEVICEFLAG_xxx)
    """
    import re

    default_targets = d.getVar('UBOOT_EXTLINUX_CONFIGURED_TARGETS')
    if not default_targets:
        bb.fatal("UBOOT_EXTLINUX_CONFIGURED_TARGETS not defined, please update your config")
    if not default_targets.strip():
        bb.fatal("No UBOOT_EXTLINUX_CONFIGURED_TARGETS list defined, nothing to do")
    bb.note('UBOOT_EXTLINUX_CONFIGURED_TARGETS: %s' % default_targets)

    config_flags = d.getVar('UBOOT_EXTLINUX_CONFIG_FLAGS')
    if not config_flags:
        bb.fatal("UBOOT_EXTLINUX_CONFIG_FLAGS not defined, please update your config")
    if not config_flags.strip():
        bb.fatal("No UBOOT_EXTLINUX_CONFIG_FLAGS list defined, nothing to do")
    bb.note('UBOOT_EXTLINUX_CONFIG_FLAGS: %s' % config_flags)

    for config_label in config_flags.split():
        bb.note('*** Loop for config_label: %s' % config_label)
        devicetree_list = d.getVar('UBOOT_EXTLINUX_DEVICEFLAG_%s' % config_label) or ''
        if devicetree_list is None:
            continue
        for devicetree in devicetree_list.split():
            bb.note('*** Loop for devicetree: %s' % devicetree)
            target_prefix = re.match('^stm32(.*)$', devicetree)
            new_target = target_prefix.group(1) + '_' + config_label
            bb.note('>>> New target label: %s' % new_target)
            if not new_target in default_targets.split():
                bb.note('Computed target: "%s" is not part of UBOOT_EXTLINUX_CONFIGURED_TARGETS: %s' % (new_target, default_targets))
                bb.note('Target not append to UBOOT_EXTLINUX_TARGETS')
                continue
            # Append target to UBOOT_EXTLINUX_TARGETS list
            d.appendVar('UBOOT_EXTLINUX_TARGETS', ' ' + new_target)
            bb.note('>>> Append %s to UBOOT_EXTLINUX_TARGETS' % new_target)
    bb.note('>>> UBOOT_EXTLINUX_TARGETS (updated): %s' % d.getVar('UBOOT_EXTLINUX_TARGETS'))
}

python do_create_multiextlinux_config() {
    targets = d.getVar('UBOOT_EXTLINUX_TARGETS')
    if not targets:
        bb.fatal("UBOOT_EXTLINUX_TARGETS not defined, nothing to do")
    if not targets.strip():
        bb.fatal("No targets, nothing to do")

    for target in targets.split():

        localdata = bb.data.createCopy(d)
        overrides = localdata.getVar('OVERRIDES')
        if not overrides:
            bb.fatal('OVERRIDES not defined')
        localdata.setVar('OVERRIDES', target + ':' + overrides)

        # Initialize labels from localdata to allow target override
        labels = localdata.getVar('UBOOT_EXTLINUX_LABELS')
        if not labels:
            bb.fatal("UBOOT_EXTLINUX_LABELS not defined, nothing to do")
        if not labels.strip():
            bb.fatal("No labels, nothing to do")

        # Initialize subdir for extlinux.conf file location
        if len(targets.split()) > 1:
            bootprefix = localdata.getVar('UBOOT_EXTLINUX_BOOTPREFIXES') or ""
            subdir = bootprefix + 'extlinux'
        else:
            subdir = 'extlinux'

        # Initialize config file
        cfile = os.path.join(d.getVar('B'), subdir , 'extlinux.conf')

        # Create extlinux folder
        bb.utils.mkdirhier(os.path.dirname(cfile))

        # ************************************************************
        # Copy/Paste extract of 'do_create_extlinux_config()' function
        # from openembedded-core 'uboot-extlinux-config.bbclass' class
        # ************************************************************
        try:
            with open(cfile, 'w') as cfgfile:
                cfgfile.write('# Generic Distro Configuration file generated by OpenEmbedded\n')

                if len(labels.split()) > 1:
                    cfgfile.write('menu title Select the boot mode\n')

                spashscreen_name = localdata.getVar('UBOOT_SPLASH_IMAGE')
                if not spashscreen_name:
                    bb.warn('UBOOT_SPLASH_IMAGE not defined')
                else:
                    cfgfile.write('MENU BACKGROUND ../%s.bmp\n' % (spashscreen_name))

                timeout =  localdata.getVar('UBOOT_EXTLINUX_TIMEOUT')
                if timeout:
                    cfgfile.write('TIMEOUT %s\n' % (timeout))

                if len(labels.split()) > 1:
                    default = localdata.getVar('UBOOT_EXTLINUX_DEFAULT_LABEL')
                    if default:
                        cfgfile.write('DEFAULT %s\n' % (default))

                for label in labels.split():
                    # **********************************************
                    # Add localdata reset to fix var expansion issue
                    # **********************************************
                    localdata = bb.data.createCopy(d)

                    overrides = localdata.getVar('OVERRIDES')
                    if not overrides:
                        bb.fatal('OVERRIDES not defined')

                    localdata.setVar('OVERRIDES', label + ':' + overrides)

                    extlinux_console = localdata.getVar('UBOOT_EXTLINUX_CONSOLE')

                    menu_description = localdata.getVar('UBOOT_EXTLINUX_MENU_DESCRIPTION')
                    if not menu_description:
                        menu_description = label

                    root = localdata.getVar('UBOOT_EXTLINUX_ROOT')
                    if not root:
                        bb.fatal('UBOOT_EXTLINUX_ROOT not defined')

                    kernel_image = localdata.getVar('UBOOT_EXTLINUX_KERNEL_IMAGE')
                    fdtdir = localdata.getVar('UBOOT_EXTLINUX_FDTDIR')

                    fdt = localdata.getVar('UBOOT_EXTLINUX_FDT')

                    if fdt:
                        cfgfile.write('LABEL %s\n\tKERNEL %s\n\tFDT %s\n' %
                                     (menu_description, kernel_image, fdt))
                    elif fdtdir:
                        cfgfile.write('LABEL %s\n\tKERNEL %s\n\tFDTDIR %s\n' %
                                     (menu_description, kernel_image, fdtdir))
                    else:
                        cfgfile.write('LABEL %s\n\tKERNEL %s\n' % (menu_description, kernel_image))

                    kernel_args = localdata.getVar('UBOOT_EXTLINUX_KERNEL_ARGS')

                    initrd = localdata.getVar('UBOOT_EXTLINUX_INITRD')
                    if initrd:
                        cfgfile.write('\tINITRD %s\n'% initrd)

                    kernel_args = root + " " + kernel_args
                    cfgfile.write('\tAPPEND %s %s\n' % (kernel_args, extlinux_console))

        except OSError:
            bb.fatal('Unable to open %s' % (cfile))
}
addtask create_multiextlinux_config before do_compile

do_create_multiextlinux_config[dirs] += "${B}"
do_create_multiextlinux_config[cleandirs] += "${B}"
do_create_multiextlinux_config[prefuncs] += "update_extlinuxconf_targets"
do_create_multiextlinux_config[file-checksums] += "${UBOOT_EXTLINUX_CONFIGURE_FILES}"
