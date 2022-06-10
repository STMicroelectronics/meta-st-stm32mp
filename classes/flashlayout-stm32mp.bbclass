# -----------------------------------------------------------------------------
# This class allows to output in deploy folder, along with the built image, the
# 'flashlayout_${PN}' sub-folder, populated with the flashlayout files to use
# with our STM32MP programmer tool to load the binaries in target device and
# partitions.
#
# There are two configurations:
#   * static:
#       The user provides the flashlayout files to export with the built image.
#   * dynamic:
#       The user configures the different variables to generate the expected
#       flashlayout files.
#
# --------------------
# Static configuration
# --------------------
# Set ENABLE_FLASHLAYOUT_DEFAULT to '1'.
# Configure FLASHLAYOUT_DEFAULT_SRC with the static flashlayout file locations.
#
# Configuration example (machine file or local.conf):
#   ENABLE_FLASHLAYOUT_DEFAULT = "1"
#   FLASHLAYOUT_DEFAULT_SRC = "files/flashlayouts/FlashLayout_sdcard_stm32mp157c-ev1_sample.tsv"
#
# ---------------------
# Dynamic configuration
# ---------------------
# Set ENABLE_FLASHLAYOUT_DEFAULT to '0'.
# In order to automatically generate flashlayout files as well formated TSV file
# there are some variables to configure.
#
# Naming:
#   <FLASHLAYOUT_BASENAME>[_<FLASHLAYOUT_CONFIG_LABEL>][_<FLASHLAYOUT_TYPE_LABEL>-FLASHLAYOUT_BOOTSCHEME_LABEL].<FLASHLAYOUT_SUFFIX>
#
#   FLASHLAYOUT_BASENAME
#       Default to 'FlashLayout'
#   FLASHLAYOUT_CONFIG_LABEL
#       Set from FLASHLAYOUT_CONFIG_LABELS list (without any '_' in config labels)
#   FLASHLAYOUT_TYPE_LABEL
#       Set from FLASHLAYOUT_TYPE_LABELS list
#   FLASHLAYOUT_BOOTSCHEME_LABEL
#       Set from FLASHLAYOUT_BOOTSCHEME_LABELS list (without any '_' in bootscheme labels)
# Note that both are appended only when FLASHLAYOUT_TYPE_LABELS and FLASHLAYOUT_BOOTSCHEME_LABELS contain more than two labels.
#   FLASHLAYOUT_SUFFIX
#       Default to 'tsv'
#
# File content structure:
#   Opt     Id      Name    Type    IP      Offset  Binary
#   <FLASHLAYOUT_PARTITION_ENABLE>
#           <FLASHLAYOUT_PARTITION_ID>
#                   <FLASHLAYOUT_PARTITION_LABEL>
#                           <FLASHLAYOUT_PARTITION_TYPE>
#                                   <FLASHLAYOUT_PARTITION_DEVICE>
#                                           <FLASHLAYOUT_PARTITION_OFFSET>
#                                                   <FLASHLAYOUT_PARTITION_BIN2LOAD>
#
# Specific configuration:
#   FLASHLAYOUT_PARTITION_SIZE
#       If configured, it allows to compute the next offset to apply in
#       flashlayout file for the following partition.
#       Note that according to the device in use for the partition a specific
#       alignment size can be specified through DEVICE_ALIGNMENT_SIZE_<device>
#       var where <device> is the current FLASHLAYOUT_PARTITION_DEVICE
#
# Note that override is manage for 'FLASHLAYOUT_PARTITION_LABELS' list with:
#   - <bootscheme-label> from FLASHLAYOUT_BOOTSCHEME_LABELS' list
#   - <config-label> from 'FLASHLAYOUT_CONFIG_LABELS' list
# Priority assignment is:
#   It means the 'FLASHLAYOUT_PARTITION_LABELS' value can be overriden by setting:
#   FLASHLAYOUT_PARTITION_LABELS:<bootscheme-label>:<config-label>
#   FLASHLAYOUT_PARTITION_LABELS:<bootscheme-label>
#   FLASHLAYOUT_PARTITION_LABELS:<config-label>
#   FLASHLAYOUT_PARTITION_LABELS
#
# Another override mechanism is also implemented for all other partition variables:
#   FLASHLAYOUT_PARTITION_ENABLE
#   FLASHLAYOUT_PARTITION_ID
#   FLASHLAYOUT_PARTITION_TYPE
#   FLASHLAYOUT_PARTITION_DEVICE
#   FLASHLAYOUT_PARTITION_OFFSET
#   FLASHLAYOUT_PARTITION_BIN2LOAD
# We can override these variable with:
#   - <config-label> from 'FLASHLAYOUT_CONFIG_LABELS' list
#   - <bootscheme-label> from 'FLASHLAYOUT_BOOTSCHEME_LABELS' list
#   - <partition-label> from 'FLASHLAYOUT_PARTITION_LABELS' list
# Priority assignment is:
#   FLASHLAYOUT_PARTITION_xxx_<bootscheme-label>_<config-label>_<partition-label>
#   FLASHLAYOUT_PARTITION_xxx_<bootscheme-label>_<config-label>
#   FLASHLAYOUT_PARTITION_xxx_<bootscheme-label>_<partition-label>
#   FLASHLAYOUT_PARTITION_xxx_<bootscheme-label>
#   FLASHLAYOUT_PARTITION_xxx_<config-label>_<partition-label>
#   FLASHLAYOUT_PARTITION_xxx_<config-label>
#   FLASHLAYOUT_PARTITION_xxx_<partition-label>
#   FLASHLAYOUT_PARTITION_xxx
# -----------------------------------------------------------------------------

# Configure flashlayout file generation
ENABLE_FLASHLAYOUT_CONFIG ??= "1"
# Configure direct use of flashlayout file without automatic file generation
ENABLE_FLASHLAYOUT_DEFAULT ??= "0"
# Configure path for provided flashlayout file
FLASHLAYOUT_DEFAULT_SRC ??= ""
# Configure flashlayout file name default format
FLASHLAYOUT_BASENAME ??= "FlashLayout"
FLASHLAYOUT_SUFFIX   ??= "tsv"
# Configure flashlayout file generation for stm32wrapper4dbg
ENABLE_FLASHLAYOUT_CONFIG_WRAPPER4DBG ??= "0"
# Configure flashlayout file generation with multiple binary copy within partition
ENABLE_FLASHLAYOUT_PARTITION_BINCOPY ??= "0"

# Configure partition file extension
PARTITION_SUFFIX ??= ".ext4"

# Configure folders for flashlayout file generation
FLASHLAYOUT_DEPLOYDIR ?= "${DEPLOY_DIR}/images/${MACHINE}"
FLASHLAYOUT_TOPDIR ?= "${WORKDIR}/flashlayout-destdir/"
FLASHLAYOUT_SUBDIR ?= "flashlayout_${PN}"
FLASHLAYOUT_DESTDIR = "${FLASHLAYOUT_TOPDIR}/${FLASHLAYOUT_SUBDIR}"

# Init bootscheme and config labels
FLASHLAYOUT_BOOTSCHEME_LABELS ??= ""
FLASHLAYOUT_CONFIG_LABELS ??= ""
# Init partition image list (used to configure partitions)
FLASHLAYOUT_PARTITION_IMAGES ??= ""
# Init partition and type labels
#   Note: possible override with bootscheme and/or config
FLASHLAYOUT_PARTITION_LABELS   ??= ""
FLASHLAYOUT_TYPE_LABELS ??= ""
# Init flashlayout partition vars
#   Note: possible override with bootscheme and/or config and/or partition
FLASHLAYOUT_PARTITION_ENABLE ??= ""
FLASHLAYOUT_PARTITION_ID ??= ""
FLASHLAYOUT_PARTITION_TYPE ??= ""
FLASHLAYOUT_PARTITION_DEVICE ??= ""
FLASHLAYOUT_PARTITION_OFFSET ??= ""
FLASHLAYOUT_PARTITION_BIN2LOAD ??= ""
FLASHLAYOUT_PARTITION_SIZE ??= ""
FLASHLAYOUT_PARTITION_REPLACE_PATTERNS ??= ""
# Init single partition creation
FLASHLAYOUT_PARTITION_DUPLICATION ??= "1"

# The STM32CubeProgrammer supported ID range is:
#   0x00 to 0xFF
# Some IDs are reserved for internal usage on STM32CubeProgrammer and special
# management is implemented for binary with STM32 header. This means that for
# flashlayout files, available ID range is only:
#   0x01 to 0x0F for Boot partitions with STM32 header
#   0x10 to 0xF0 for User partitions programmed without header
# Note also that for FSBL and SSBL binaries loaded in RAM to program the devices
# there are two reserved IDs
#   0x01 for FSBL
#   0x03 for SSBL
FLASHLAYOUT_PARTITION_ID_START_BINARY ??= "0x04"
FLASHLAYOUT_PARTITION_ID_LIMIT_BINARY ??= "0x0F"
FLASHLAYOUT_PARTITION_ID_START_OTHERS ??= "0x10"
FLASHLAYOUT_PARTITION_ID_LIMIT_OTHERS ??= "0xF0"

# Init default config for empty or used partition for STM32CubeProgrammer
FLASHLAYOUT_PARTITION_ENABLE_PROGRAMM_EMPTY ??= "PED"

python __anonymous () {
    # -----------------------------------------------------------------------------
    # Make sure to add the flashlayout file creation after ROOTFS build
    # So we should identify image ROOTFS build and only the ROOTFS (for now)
    # As we know that PARTITIONS may be built as part of ROOTFS build, let's
    # avoid amending the partition images
    # -----------------------------------------------------------------------------
    if d.getVar('ENABLE_FLASHLAYOUT_CONFIG') == "1":
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
                # Init partition list from PARTITIONS_IMAGES
                image_partitions = []
                # Append image_partitions list with all configured partition images:

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
                                if len(items) > 0 and len(items) != 5:
                                        bb.fatal('[PARTITIONS_IMAGES] Only image,label,mountpoint,size,type can be specified!')
                                # Make sure that we're dealing with partition image and not rootfs image
                                if items[2] != '':
                                    # Mount point is available, so we're dealing with partition image
                                    # Append image to image_partitions list
                                    image_partitions.append(d.expand(items[0]))
                                break
                # We need to clearly identify ROOTFS build, not InitRAMFS/initRD one (if any), not partition one either
                if current_image_name not in image_partitions and current_image_name != initramfs and current_image_name not in initrd:
                    # We add the flashlayout file creation task just after the do_image_complete for ROOTFS build
                    bb.build.addtask('do_create_flashlayout_config', 'do_build', 'do_image_complete', d)
                    # We add also the function that feeds the FLASHLAYOUT_PARTITION_* vars
                    d.appendVarFlag('do_create_flashlayout_config', 'prefuncs', ' flashlayout_partition_config')
}

def expand_var(var, bootscheme, config, partition, d):
    """
    Compute and return 'var':
        0) Prepend 'partition' to default OVERRIDES
        1) Look for any 'bootscheme_config' expansion for 'var': 'var_bootscheme_config'
        2) Look for any 'bootscheme' expansion for 'var': 'var_bootscheme'
        3) Look for any 'config' expansion for 'var': 'var_config'
        4) Then look for any 'var' override
        5) Default 'var' to 'none' if not defined
    This is the priority order assignment for 'var'
    """
    # Append 'partition' to OVERRIDES
    localdata = bb.data.createCopy(d)
    overrides = localdata.getVar('OVERRIDES')
    if not overrides:
        bb.fatal('OVERRIDES not defined')
    localdata.setVar('OVERRIDES', partition + ':' + overrides)
    # Compute var according to priority assignment order defined above
    expanded_var = localdata.getVar('%s:%s:%s' % (var, bootscheme, config))
    if not expanded_var:
        expanded_var = localdata.getVar('%s:%s' % (var, bootscheme))
    if not expanded_var:
        expanded_var = localdata.getVar('%s:%s' % (var, config))
    if not expanded_var:
        expanded_var = localdata.getVar(var)
    if not expanded_var:
        expanded_var = "none"
    # Return expanded and/or overriden var value
    return expanded_var

def get_label_list(d, label, duplicate='1'):
    """
    Configure the label name list according to the proposed duplicate value
    """
    list = []
    if int(duplicate) > 1:
        for i in range(1, int(duplicate) + 1):
            list.append(label + str(i))
        bb.debug(1,">>> Partition duplication configure for %s with new sub-list: %s" % (label, list))
    else:
        list.append(label)
    # Return the label list
    return list

def get_device(bootscheme, config, partition, d):
    """
    This function returns the device configured from FLASHLAYOUT_PARTITION_DEVICE for
    the requested partition for label and bootscheme configured.
    The var FLASHLAYOUT_PARTITION_DEVICE can be configured through different scheme
        FLASHLAYOUT_PARTITION_DEVICE = '<device0>:<dev0part_0> <dev0part_1>,<device1>:<dev1part0>'
        FLASHLAYOUT_PARTITION_DEVICE = '<device0>:default,<device1>:<dev1part0> <dev1part1>,<device2>:<dev2part0>'
        FLASHLAYOUT_PARTITION_DEVICE = '<device0>'
    Then, to set the device for the current partition, the logic followed is:
        If the configuration provides a single device, then partition device is set
        to this value.
        Else,
            If the current partition is specified in any of the configured partition
            lists, the matching configured device is set as partition device.
            And if the current partition is not found, the default device configured
            is set as partition device.
    """
    # Set device configuration
    device_configs = expand_var('FLASHLAYOUT_PARTITION_DEVICE', bootscheme, config, partition, d)
    bb.debug(1, '>>> Selected FLASHLAYOUT_PARTITION_DEVICE: %s' % device_configs)

    if len(device_configs.split(',')) == 1:
        bb.debug(1, '>>> Only one device configuration set for %s partition for %s label for %s bootscheme' % (partition, config, bootscheme))
        device = device_configs.split(':')[0]
    else:
        bb.debug(1, '>>> Multiple device configurations set for %s partition for %s label for %s bootscheme' % (partition, config, bootscheme))
        # Init default_device and device to empty string
        default_device = ''
        device = ''
        for device_config in device_configs.split(','):
            cfg_devc = device_config.split(':')[0].strip()
            cfg_part = device_config.split(':')[1] or 'default'
            # Make sure configuration is correct
            if len(cfg_devc.split()) > 1:
                bb.fatal('Only one device configuration can be specified: found %s for %s partition for %s label for %s bootscheme' % (cfg_devc, partition, config, bootscheme))
            # Configure the default device configuration if any
            if cfg_part == 'default':
                if default_device != '':
                    bb.fatal('Found two "default" device configuration for %s partition for %s label for %s bootscheme in FLASHLAYOUT_PARTITION_DEVICE var' % (partition, config, bootscheme))
                default_device = cfg_devc
                bb.debug(1, '>>> Set default device configuration to %s' % default_device)
            else:
                # Find out if any device is configured for current partition
                for p in cfg_part.split():
                    if p == partition:
                        device = cfg_devc
                        break
        # If 'device' is still empty for current partition, check if we can apply default device configuration
        if device == '':
            if default_device == '':
                bb.fatal('Not able to get device configuration for %s partition for %s label for %s bootscheme' % (partition, config, bootscheme))
            else:
                bb.debug(1, '>>> Configure device to default device setting')
                device = default_device
    bb.debug(1, '>>> New device configured: %s' % device)
    # Return the value computed
    return device

def align_size(d, device, size, copy=1):
    """
    This function returns the size in KiB for the selected device making sure to
    align on erase block and taking into account the copy expected to fit for the
    original size set
    """
    # Make sure to use device name and not device type
    device_types = (d.getVar('DEVICE_STORAGE_TYPES') or "").split()
    if device in device_types:
        device = d.getVar('DEVICE:%s' % device) or ""
    # Get device alignment size
    alignment_size = d.getVar('DEVICE_ALIGNMENT_SIZE:%s' % device) or "none"
    if alignment_size == 'none':
        bb.fatal('Missing DEVICE_ALIGNMENT_SIZE:%s value' % device)
    # Check for default size alignment on erase block
    if ( int(size) * 1024 ) % int(alignment_size, 16) == 0:
        bb.debug(1, '>>> The partition size properly follows %s erase size' % alignment_size)
    else:
        bb.debug(1, '>>> The %s alignment size is: %s' % (device, alignment_size))
        floor_coef = ( int(size) * 1024 ) // int(alignment_size, 16)
        compute_size = ( floor_coef + 1 ) * int(alignment_size, 16) * int(copy)
        # Set size in KiB
        size = compute_size // 1024
    # Compute size with requested copy
    size = int(size) * int(copy)
    # Convert to string
    size = str(size)
    bb.debug(1, '>>> New partition size configured to follow %s alignment size: %s' % (alignment_size, size))
    # Return the computed size
    return size

def get_offset(new_offset, copy, current_device, bootscheme, config, partition, d):
    """
    This function returns a couple of strings: offset, next_offset
    The offset is the one to use in flashlayout file for the requested partition,
    and next_offset is the one to use in flashlayout for next partition (if any).

    The offset can be directly configured for the current partition through the
    FLASHLAYOUT_PARTITION_OFFSET variable. If this one is set to 'none' for the
    current partition, then we use the one provided through 'new_offset' if set,
    else we default to DEVICE_START_OFFSET_<device> one where <device> is feed from
    'current_device' input.

    The next_offset is computed by first getting the FLASHLAYOUT_PARTITION_SIZE for
    the current partition, and we make sure to align properly the next_offset
    according to the DEVICE_ALIGNMENT_SIZE_<device> where <device> is feed from
    'current_device' input and the number of copy expected to fit in partition.
    """
    import re
    # Get current_device alias
    device_alias = d.getVar('DEVICE:%s' % current_device) or ""
    # Set offset
    offset = expand_var('FLASHLAYOUT_PARTITION_OFFSET', bootscheme, config, partition, d)
    bb.debug(1, '>>> Selected FLASHLAYOUT_PARTITION_OFFSET: %s' % offset)
    # Set max offset
    max_offset = d.getVar('DEVICE_MAX_OFFSET:%s' % device_alias) or "none"
    bb.debug(1, '>>> Selected DEVICE_MAX_OFFSET: %s' % max_offset)
    if offset == 'none':
        if new_offset == 'none':
            bb.debug(1, '>>> No %s partition offset configured (%s device) for %s label for %s bootscheme, so default to default origin device one.' % (partition, current_device, config, bootscheme))
            start_offset = d.getVar('DEVICE_START_OFFSET:%s' % device_alias) or "none"
            if start_offset == 'none':
                bb.fatal('Missing DEVICE_START_OFFSET:%s value' % device_alias)
            offset = start_offset
        else:
            offset = new_offset
        bb.debug(1, '>>> New offset configured: %s' % offset)
    # Set next offset
    partition_size = expand_var('FLASHLAYOUT_PARTITION_SIZE', bootscheme, config, partition, d)
    bb.debug(1, '>>> Selected FLASHLAYOUT_PARTITION_SIZE: %s' % partition_size)
    if not partition_size.isdigit():
        bb.debug(1, 'No partition size provided for %s partition, %s label and %s bootscheme!' % (partition, config, bootscheme))
        next_offset = "none"
        max_offset = "none"
    else:
        if re.match('^0x.*$', offset):
            bb.debug(1, '>>> Current device is %s (%s alias), and %s copy is set' % (current_device, device_alias, copy))
            partition_size = align_size(d, device_alias, partition_size, copy)
            # Compute new offset value
            overall_size = int(offset, 16) + int(partition_size) * 1024
            next_offset = '0x{0:0{1}X}'.format(overall_size, 8)

            # Check if the next offset will exceed the size of the storage
            if max_offset != "none":
                if int(next_offset, 0) <= int(max_offset, 0):
                    # still some place, do not return max offset
                    max_offset = "none"
        else:
            next_offset = "none"
            max_offset = "none"
    bb.debug(1, '>>> New next_offset configured: %s' % next_offset)

    # Return offset, next offset and max offset
    return str(offset), str(next_offset), str(max_offset)

def get_binaryname(labeltype, device, bootscheme, config, partition, d):
    """
    Return proper binary name to use in flashlayout file by applying any specific
    computation (replacement, etc)
    Make sure also that binary is available on deploy folder
    """
    import re
    # Init binary_name for current configuration
    binary_name = expand_var('FLASHLAYOUT_PARTITION_BIN2LOAD', bootscheme, config, partition, d)
    bb.debug(1, '>>> Selected FLASHLAYOUT_PARTITION_BIN2LOAD: %s' % binary_name)
    # Set 'device' to alias name in lower case
    if device != 'none':
        device = d.getVar('DEVICE:%s' % device).lower()
    # Init pattern to look for with current config value
    update_patterns = '<BOOTSCHEME>;' + bootscheme
    update_patterns += ' ' + '<CONFIG>;' + config.replace("-","_")
    update_patterns += ' ' + '<DEVICE>;' + device
    update_patterns += ' ' + '<TYPE>;' + labeltype
    bb.debug(1, '>>> Default substitution patterns: %s' % update_patterns)

    replace_patterns = expand_var('FLASHLAYOUT_PARTITION_REPLACE_PATTERNS', bootscheme, config, partition, d)
    if replace_patterns != 'none':
        bb.debug(1, '>>> Substitution pattern addons: %s' % replace_patterns)
        # Append substitution patterns to update pattern list
        update_patterns += ' ' + replace_patterns
    # Apply pattern substitution to binary name
    for pattern in update_patterns.split():
        pattern2replace = pattern.split(';')[0]
        pattern2use = pattern.split(';')[1]
        if re.search(r'[-_]%s([-_.]|$)' % pattern2replace, binary_name):
            if pattern2use == "":
                # Remove pattern
                binary_name = re.sub(r'[-_]%s([-_.]|$)' % pattern2replace, r'\1', binary_name)
            else:
                # Replace pattern
                binary_name = re.sub(r'([-_])%s([-_.]|$)' % pattern2replace, r'\1%s\2' % pattern2use, binary_name)
    bb.debug(1, '>>> New binary name: %s' % binary_name)
    # Return binary_name value
    return binary_name

def flashlayout_search(d, files):
    search_path = d.getVar("BBPATH").split(":")
    for file in files.split():
        for p in search_path:
            file_path = os.path.join(p, file)
            if os.path.isfile(file_path):
                return (True, file_path)
    return (False, "")

python do_create_flashlayout_config() {
    import re
    import shutil

    # We check first if it is requested to generate any flashlayout files
    if d.getVar("ENABLE_FLASHLAYOUT_CONFIG") != "1":
        bb.debug(1, 'ENABLE_FLASHLAYOUT_CONFIG not enabled')
        return

    # Create destination folder for flashlayout files
    bb.utils.remove(d.getVar('FLASHLAYOUT_DESTDIR'), recurse=True)
    bb.utils.mkdirhier(d.getVar('FLASHLAYOUT_DESTDIR'))

    # We check if user as define a static flashlayout file to use instead of dynamic generation
    if d.getVar("ENABLE_FLASHLAYOUT_DEFAULT") == "1":
        bb.debug(1, 'ENABLE_FLASHLAYOUT_DEFAULT enabled')
        flashlayout_src = d.getVar("FLASHLAYOUT_DEFAULT_SRC")
        if not flashlayout_src:
            bb.fatal("FLASHLAYOUT_DEFAULT_SRC not defined, please set a proper value")
        if not flashlayout_src.strip():
            bb.fatal("No static flashlayout file configured, nothing to do")
        for fl_src in flashlayout_src.split():
            found, f = flashlayout_search(d, fl_src)
            if found:
                flashlayout_staticname=os.path.basename(f)
                flashlayout_file = os.path.join(d.getVar('FLASHLAYOUT_DESTDIR'), flashlayout_staticname)
                shutil.copy2(f, flashlayout_file)
                bb.debug(1, 'Copy %s to output file %s' % (f, flashlayout_file))
            else:
                bb.fatal("Configure static file: %s not found" % fl_src)
        return

    # Set bootschemes for partition var override configuration
    bootschemes = d.getVar('FLASHLAYOUT_BOOTSCHEME_LABELS')
    if not bootschemes:
        bb.fatal("FLASHLAYOUT_BOOTSCHEME_LABELS not defined, nothing to do")
    if not bootschemes.strip():
        bb.fatal("No bootschemes, nothing to do")
    # Make sure there is no '_' in FLASHLAYOUT_BOOTSCHEME_LABELS
    for bootscheme in bootschemes.split():
        if re.match('.*_.*', bootscheme):
            bb.fatal("Please remove all '_' for bootschemes defined in FLASHLAYOUT_BOOTSCHEME_LABELS")
    bb.debug(1, 'FLASHLAYOUT_BOOTSCHEME_LABELS: %s' % bootschemes)

    for bootscheme in bootschemes.split():
        bb.debug(1, '*** Loop for bootscheme label: %s' % bootscheme)
        # Get the different flashlayout config label
        configs = expand_var('FLASHLAYOUT_CONFIG_LABELS', bootscheme, '', '', d)
        # Make sure there is no '_' in FLASHLAYOUT_CONFIG_LABELS
        for config in configs.split():
            if re.match('.*_.*', config):
                bb.fatal("Please remove all '_' for configs defined in FLASHLAYOUT_CONFIG_LABELS")
        bb.debug(1, 'FLASHLAYOUT_CONFIG_LABELS: %s' % configs)

        if configs.strip() == 'none':
            bb.debug(1, "FLASHLAYOUT_CONFIG_LABELS is none, so no flashlayout file to generate.")
            continue
        # Create bootscheme subfolder for flashlayout files
        flashlayout_subfolder_path = os.path.join(d.getVar('FLASHLAYOUT_DESTDIR'), bootscheme)
        bb.utils.mkdirhier(flashlayout_subfolder_path)

        for config in configs.split():
            bb.debug(1, '*** Loop for config label: %s' % config)
            # Set labeltypes list
            labeltypes = expand_var('FLASHLAYOUT_TYPE_LABELS', bootscheme, config, '', d)
            bb.debug(1, 'FLASHLAYOUT_TYPE_LABELS: %s' % labeltypes)
            if labeltypes.strip() == 'none':
                bb.debug(1, "FLASHLAYOUT_TYPE_LABELS is none, so no flashlayout file to generate.")
                continue
            for labeltype in labeltypes.split():
                bb.debug(1, '*** Loop for label type: %s' % labeltype)
                # Init current label
                current_label = labeltype
                # Init flashlayout file name
                if config == 'none':
                    config_addons = ''
                else:
                    config_addons = '_' + config
                if len(labeltypes.split()) < 2 and len(bootschemes.split()) < 2:
                    labeltype_addons = ''
                else:
                    labeltype_addons = '_' + labeltype + '-' + bootscheme
                flashlayout_file = os.path.join(flashlayout_subfolder_path, d.expand("${FLASHLAYOUT_BASENAME}%s%s.${FLASHLAYOUT_SUFFIX}" % (config_addons, labeltype_addons)))
                # Get the partition list to write in flashlayout file
                partitions = expand_var('FLASHLAYOUT_PARTITION_LABELS', bootscheme, config, '', d)
                bb.debug(1, 'FLASHLAYOUT_PARTITION_LABELS: %s' % partitions)
                if partitions == 'none':
                    bb.debug(1, "FLASHLAYOUT_PARTITION_LABELS is none, so no flashlayout file to generate.")
                    continue
                # Generate flashlayout file for labeltype
                try:
                    with open(flashlayout_file, 'w') as fl_file:
                        # Write to flashlayout file the first line header
                        fl_file.write('#Opt\tId\tName\tType\tIP\tOffset\tBinary\n')
                        # Init partition id for binary and other
                        partition_id_bin = int(d.getVar("FLASHLAYOUT_PARTITION_ID_START_BINARY"), 16)
                        partition_id_binmax = int(d.getVar("FLASHLAYOUT_PARTITION_ID_LIMIT_BINARY"), 16)
                        partition_id_oth = int(d.getVar("FLASHLAYOUT_PARTITION_ID_START_OTHERS"), 16)
                        partition_id_othmax = int(d.getVar("FLASHLAYOUT_PARTITION_ID_LIMIT_OTHERS"), 16)
                        # Init partition next offset to 'none'
                        partition_nextoffset = "none"
                        # Init partition previous device to 'none'
                        partition_prevdevice = "none"
                        for part in partitions.split():
                            bb.debug(1, '*** Loop for partition: %s' % part)
                            # Init break and clean file switch
                            break_and_clean_file = '0'
                            # Init partition duplication count
                            partition_duplication = expand_var('FLASHLAYOUT_PARTITION_DUPLICATION', bootscheme, config, part, d)
                            if not partition_duplication.isdigit():
                                bb.fatal('Wrong configuration for FLASHLAYOUT_PARTITION_DUPLICATION: %s (bootscheme: %s, config: %s, partition: %s)' % (partition_duplication, bootscheme, config, part))

                            for partition in get_label_list(d, part, partition_duplication):
                                bb.debug(1, '>>> Set partition label name to : %s' % partition)
                                # Init partition settings
                                partition_enable = expand_var('FLASHLAYOUT_PARTITION_ENABLE', bootscheme, config, partition, d)
                                partition_name = partition
                                partition_type = expand_var('FLASHLAYOUT_PARTITION_TYPE', bootscheme, config, partition, d)
                                partition_id = expand_var('FLASHLAYOUT_PARTITION_ID', bootscheme, config, partition, d)
                                if partition_id == "none":
                                    # Compute partition_id
                                    if partition_type == 'Binary' or partition_type == 'FIP':
                                        # Make sure we're not getting wrong partition_id
                                        if partition_id_bin > partition_id_binmax:
                                            bb.fatal('Partition ID exceed %s limit for %s type: FLASHLAYOUT_PARTITION_ID = %s (bootscheme: %s, config: %s, partition: %s)' % (d.getVar("FLASHLAYOUT_PARTITION_ID_LIMIT_BINARY"), partition_type, partition_id, bootscheme, config, partition))
                                        partition_id = '0x{0:0{1}X}'.format(partition_id_bin, 2)
                                        partition_id_bin = partition_id_bin + 1
                                    else:
                                        # Make sure we're not getting wrong partition_id
                                        if partition_id_oth > partition_id_othmax:
                                            bb.fatal('Partition ID exceed %s limit for %s type: FLASHLAYOUT_PARTITION_ID = %s (bootscheme: %s, config: %s, partition: %s)' % (d.getVar("FLASHLAYOUT_PARTITION_ID_LIMIT_OTHERS"), partition_type, partition_id, bootscheme, config, partition))
                                        partition_id = '0x{0:0{1}X}'.format(partition_id_oth, 2)
                                        partition_id_oth = partition_id_oth + 1
                                partition_copy = expand_var('FLASHLAYOUT_PARTITION_COPY', bootscheme, config, partition, d)
                                if not partition_copy.isdigit():
                                    bb.fatal('Wrong configuration for FLASHLAYOUT_PARTITION_COPY: %s (bootscheme: %s, config: %s, partition: %s)' % (partition_copy, bootscheme, config, partition))
                                # Update partition type if needed
                                if int(partition_copy) > 1:
                                    partition_type += '(' + partition_copy + ')'
                                partition_device = get_device(bootscheme, config, partition, d)
                                # Reset partition_nextoffset to 'none' in case partition device has changed
                                if partition_device != partition_prevdevice:
                                    partition_nextoffset = "none"
                                # Save partition current device to previous one for next loop
                                partition_prevdevice = partition_device
                                # Get partition offset
                                partition_offset, partition_nextoffset, partition_maxoffset = get_offset(partition_nextoffset, partition_copy, partition_device, bootscheme, config, partition, d)
                                # Get binary name
                                partition_bin2load = get_binaryname(labeltype, partition_device, bootscheme, config, partition, d)
                                # Be verbose in log file
                                bb.debug(1, '>>> Layout inputs: %s' % fl_file.name)
                                bb.debug(1, '>>> FLASHLAYOUT_PARTITION_ENABLE:      %s' % partition_enable)
                                bb.debug(1, '>>> FLASHLAYOUT_PARTITION_ID:          %s' % partition_id)
                                bb.debug(1, '>>> FLASHLAYOUT_PARTITION_LABEL:       %s' % partition_name)
                                bb.debug(1, '>>> FLASHLAYOUT_PARTITION_TYPE:        %s' % partition_type)
                                bb.debug(1, '>>> FLASHLAYOUT_PARTITION_DEVICE:      %s' % partition_device)
                                bb.debug(1, '>>> FLASHLAYOUT_PARTITION_OFFSET:      %s' % partition_offset)
                                bb.debug(1, '>>> FLASHLAYOUT_PARTITION_BIN2LOAD:    %s' % partition_bin2load)
                                bb.debug(1, '>>> done')
                                # Check if the size will exceed the mass storage
                                if partition_maxoffset != "none" :
                                    bb.warn('>>> Cannot generate %s file: the end offset (%s) for %s partition exceeds the max offset (%s) for %s device.' % (os.path.basename(flashlayout_file), partition_nextoffset, partition, partition_maxoffset, partition_device))
                                    # Cleanup on-going tsv file
                                    break_and_clean_file = '1'
                                    break
                                # Check if binary is available in deploy folder
                                if partition_bin2load != 'none':
                                    bin2load_fullpath = os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), partition_bin2load)
                                    if not os.path.isfile(bin2load_fullpath):
                                        # Specific case for rootfs binary (not yet deployed)
                                        bin2load_fullpath = os.path.join(d.getVar('IMGDEPLOYDIR'), partition_bin2load)
                                        if not os.path.isfile(bin2load_fullpath):
                                            bb.warn('>>> Cannot generate %s file: the %s binary for %s partition is missing in deploy folder' % (os.path.basename(flashlayout_file), partition_bin2load, partition))
                                            # Cleanup on-going tsv file
                                            break_and_clean_file = '1'
                                            break
                                    # Check if the bin2load size will exceed the partition size
                                    if partition_nextoffset != 'none':
                                        bin2load_size = os.path.getsize(bin2load_fullpath)
                                        partition_size = int(partition_nextoffset, 16) - int(partition_offset, 16)
                                        if bin2load_size > partition_size:
                                            bb.warn('>>> Cannot generate %s file: the %s binary size (%s) for %s partition exceeds the partition size (%s).' % (os.path.basename(flashlayout_file), partition_bin2load, bin2load_size, partition, partition_size))
                                            # Cleanup on-going tsv file
                                            break_and_clean_file = '1'
                                            break
                                # Get the supported labels for current storage device
                                partition_device_alias = d.getVar('DEVICE:%s' % partition_device) or ""
                                partition_type_supported_labels = d.getVar('DEVICE_BOARD_ENABLE:%s' % partition_device_alias) or "none"
                                # Check if partition type is supported for the current label
                                if partition_device != 'none' and current_label not in partition_type_supported_labels.split():
                                    bb.debug(1, '>>> FLASHLAYOUT_PARTITION_DEVICE (%s, alias %s) is not supported for current label (%s): partition %s not appended in flashlayout file' % (partition_device, partition_device_alias, current_label, partition_name))
                                    bb.debug(1, '>>> DEVICE_BOARD_ENABLE:%s: %s' % (partition_device_alias, partition_type_supported_labels))
                                    continue
                                # Write to flashlayout file the partition configuration
                                fl_file.write('%s\t%s\t%s\t%s\t%s\t%s\t%s\n' %
                                             (partition_enable, partition_id, partition_name, partition_type, partition_device, partition_offset, partition_bin2load))

                            # Abort on-going flashlayout file
                            if break_and_clean_file == "1":
                                break_and_clean_file = '0'
                                fl_file.close()
                                if os.path.exists(flashlayout_file):
                                    os.remove(flashlayout_file)
                                break
                except OSError:
                    bb.fatal('Unable to open %s' % (fl_file))

                if not os.path.exists(flashlayout_file):
                    # The tsv does not exist, so cannot generate the tsv for wrapper4dbg
                    continue

                if d.getVar("ENABLE_FLASHLAYOUT_CONFIG_WRAPPER4DBG") == "1":
                    bb.debug(1, '*** Loop for flashlayout for the wrapper for debug %s' % labeltype)

                    tmp_flashlayout_file = os.path.join(flashlayout_subfolder_path, "flashlayout.tmp")
                    debug_flashlayout = False

                    try:
                        with open(flashlayout_file, 'r') as fl_file:
                            try:
                                with open(tmp_flashlayout_file, 'w') as debug_fl_file:
                                    for line in fl_file:
                                        if re.match('^.*/tf-a.*$', line) :
                                            line_tmp = re.sub(r'(.*)/',r'\1/debug/debug-', line)
                                            filename = re.sub(r'.*[\t ](.*)$',r'\1', line_tmp).strip()
                                            if os.path.isfile(os.path.join(d.getVar('DEPLOY_DIR_IMAGE'), filename)):
                                                line = line_tmp
                                                debug_flashlayout = True

                                        debug_fl_file.write('%s' % (line))
                            except OSError:
                                bb.fatal('Unable to open %s' % (debug_fl_file))
                    except OSError:
                        bb.fatal('Unable to open %s' % (fl_file))
                    if debug_flashlayout:
                        flashlayout_wrapper4dbg_subfolder_path = os.path.join(d.getVar('FLASHLAYOUT_DESTDIR'), bootscheme, "debug")
                        bb.utils.mkdirhier(flashlayout_wrapper4dbg_subfolder_path)
                        # Wrapper4dbg output filename
                        debug_flashlayout_file = os.path.join(flashlayout_wrapper4dbg_subfolder_path,d.expand("debug-${FLASHLAYOUT_BASENAME}%s%s.${FLASHLAYOUT_SUFFIX}" % (config_addons, labeltype_addons)))
                        bb.debug(1, ">>> Update tf-a in %s" %  (debug_flashlayout_file))
                        os.rename(tmp_flashlayout_file, debug_flashlayout_file)
                    else:
                        os.remove(tmp_flashlayout_file)
}
do_create_flashlayout_config[dirs] = "${FLASHLAYOUT_DESTDIR}"

FLASHLAYOUT_DEPEND_TASKS ?= ""
do_create_flashlayout_config[depends] += "${FLASHLAYOUT_DEPEND_TASKS}"

SSTATETASKS += "do_create_flashlayout_config"
do_create_flashlayout_config[cleandirs] = "${FLASHLAYOUT_TOPDIR}"
do_create_flashlayout_config[sstate-inputdirs] = "${FLASHLAYOUT_TOPDIR}"
do_create_flashlayout_config[sstate-outputdirs] = "${FLASHLAYOUT_DEPLOYDIR}/"

python do_create_flashlayout_config_setscene () {
    sstate_setscene(d)
}
addtask do_create_flashlayout_config_setscene

def partImage2partConfig(config, fstype, d):
    """
    Convert PARTTIONS_IMAGES['config'] setting format to format expected to feed
    PARTITIONS_CONFIG[xxx].
    Manage <image_name> update respect to 'fstype' provided and apply the rootfs
    namming or standard partition image one.
        FROM: <image_name>,<partition_label>,<mountpoint>,<size>,<type>
        TO  : <binary_name>,<partition_label>,<size>,<type>
    """
    items = d.getVarFlag('PARTITIONS_IMAGES', config).split(',') or ""
    if len(items) != 5:
        bb.fatal('Wrong settings for PARTTIONS_IMAGES[%s] : %s' % (config, items))
    if items[2] != '':
        bin_name = items[0] + '-${DISTRO}-${MACHINE}' + '.' + fstype
    else:
        bin_name = items[0] + '-${MACHINE}' + '.' + fstype
    # Set string for PARTITIONS_CONFIG item: <binary_name>,<partlabel>,<size>,<type>
    part_format = bin_name + ',' + items[1] + ',' + items[3] + ',' + items[4]
    return part_format

python flashlayout_partition_config() {
    """
    Set the different flashlayout partition vars for the configure partition
    images.
    Based on PARTITIONS_CONFIG and PARTITIONS_BOOTLOADER_CONFIG
    feed FLASHLAYOUT_PARTITION_ vars for each 'config' and 'label':
        FLASHLAYOUT_PARTITION_ENABLE:<config>:<label>
        FLASHLAYOUT_PARTITION_BIN2LOAD:<config>:<label>
        FLASHLAYOUT_PARTITION_SIZE:<config>:<label>
        FLASHLAYOUT_PARTITION_TYPE:<config>:<label>
        FLASHLAYOUT_PARTITION_COPY:<config>:<label>
        FLASHLAYOUT_PARTITION_OFFSET:<config>:<label>
    """
    # Init partition and flashlayout configuration vars
    partitionconfig_list = 'PARTITIONS_CONFIG PARTITIONS_BOOTLOADER_CONFIG'

    for partconfvar in partitionconfig_list.split():
        partitionsconfigflags = d.getVarFlags(partconfvar)
        # The "doc" varflag is special, we don't want to see it here
        partitionsconfigflags.pop('doc', None)
        partitionsconfig = (d.getVar(partconfvar) or "").split()

        if len(partitionsconfig) > 0:
            for config in partitionsconfig:
                for f, v in partitionsconfigflags.items():
                    if config == f:
                        # Make sure to get var flag properly expanded
                        v = d.getVarFlag(partconfvar, config)
                        if not v.strip():
                            bb.fatal('[%s] Missing configuration for %s config' % (partconfvar, config))
                        for subconfigs in v.split():
                            bb.debug(1, '[%s] *** Loop for %s config with setting: %s' % (partconfvar, config, subconfigs))
                            items = subconfigs.split(',')
                            # Check for proper content
                            if len(items) < 4 or len(items) > 5:
                                bb.fatal('[%s] Only partdata,partlabel,size,type,copy can be specified!' % partconfvar)

                            # Init flashlayout label
                            if items[1] != '':
                                fl_label = d.expand(items[1])
                                bb.debug(1, "Init for flashlayout label to: %s" % fl_label)
                            else:
                                bb.fatal('[%s] Missing partlabel setting' % partconfvar)

                            # Init default partition_enable
                            partition_enable = d.getVar('FLASHLAYOUT_PARTITION_ENABLE')

                            # Init for partition duplication
                            if d.getVar('ENABLE_FLASHLAYOUT_PARTITION_BINCOPY') == '0':
                                if len(items) == 5 and items[4] != '':
                                    if d.getVar('FLASHLAYOUT_PARTITION_DUPLICATION:%s:%s' % (config, fl_label)):
                                        bb.debug(1,"FLASHLAYOUT_PARTITION_DUPLICATION:%s:%s is already set to: %s" % (config, fl_label, d.getVar('FLASHLAYOUT_PARTITION_DUPLICATION:%s:%s' % (config, fl_label))))
                                    else:
                                        bb.debug(1,"Set FLASHLAYOUT_PARTITION_DUPLICATION:%s:%s to %s" % (config, fl_label, items[4]))
                                        d.setVar('FLASHLAYOUT_PARTITION_DUPLICATION:%s:%s' % (config, fl_label), items[4])
                                else:
                                    bb.debug(1, "No partition duplication setting for %s label : default setting would applied..." % fl_label)
                                    duplicate_max = d.getVar('FLASHLAYOUT_PARTITION_DUPLICATION')
                            else:
                                bb.debug(1,"Set FLASHLAYOUT_PARTITION_DUPLICATION to 1")
                                d.setVar('FLASHLAYOUT_PARTITION_DUPLICATION', '1')

                            duplicate_max = expand_var('FLASHLAYOUT_PARTITION_DUPLICATION', '', config, fl_label, d)
                            if not duplicate_max.isdigit():
                                bb.fatal('[%s] Wrong configuration for FLASHLAYOUT_PARTITION_DUPLICATION: %s (config: %s, partition: %s)' % (partconfvar, duplicate_max, config, fl_label))

                            # Init label list and original label
                            fl_label_ori = fl_label
                            fl_label_list = get_label_list(d, fl_label, duplicate_max)

                            for fl_label in fl_label_list:
                                bb.debug(1,"Feed FLASHLAYOUT_PARTITION_* vars for label: %s" % fl_label)
                                if items[0] != '':
                                    if d.getVar('FLASHLAYOUT_PARTITION_BIN2LOAD:%s:%s' % (config, fl_label)):
                                        bb.debug(1, "FLASHLAYOUT_PARTITION_BIN2LOAD:%s:%s is already set to: %s." % (config, fl_label, d.getVar('FLASHLAYOUT_PARTITION_BIN2LOAD:%s:%s' % (config, fl_label))))
                                    elif d.getVar('FLASHLAYOUT_PARTITION_BIN2LOAD:%s:%s' % (config, fl_label_ori)):
                                        bb.debug(1, "Set FLASHLAYOUT_PARTITION_BIN2LOAD:%s:%s:to %s." % (config, fl_label, d.getVar('FLASHLAYOUT_PARTITION_BIN2LOAD:%s:%s' % (config, fl_label_ori))))
                                        d.setVar('FLASHLAYOUT_PARTITION_BIN2LOAD:%s:%s' % (config, fl_label), d.getVar('FLASHLAYOUT_PARTITION_BIN2LOAD:%s:%s' % (config, fl_label_ori)))
                                    else:
                                        bb.debug(1, "Set FLASHLAYOUT_PARTITION_BIN2LOAD:%s:%s to %s." % (config, fl_label, items[0]))
                                        d.setVar('FLASHLAYOUT_PARTITION_BIN2LOAD:%s:%s' % (config, fl_label), items[0])
                                    if d.getVar('FLASHLAYOUT_PARTITION_BIN2LOAD:%s:%s' % (config, fl_label)) == "":
                                        bb.debug(1, "FLASHLAYOUT_PARTITION_BIN2LOAD:%s:%s is empty: use '%s' as programm setting." % (config, fl_label, d.getVar('FLASHLAYOUT_PARTITION_ENABLE_PROGRAMM_EMPTY')))
                                        partition_enable = d.getVar('FLASHLAYOUT_PARTITION_ENABLE_PROGRAMM_EMPTY')
                                else:
                                    bb.debug(1, "No partdata setting for %s label : default setting would applied..." % fl_label)
                                    # Update partition enable to empty in case nothing to load
                                    if d.getVar('FLASHLAYOUT_PARTITION_BIN2LOAD') == "":
                                        bb.debug(1, "FLASHLAYOUT_PARTITION_BIN2LOAD is empty: use '%s' as programm setting." % d.getVar('FLASHLAYOUT_PARTITION_ENABLE_PROGRAMM_EMPTY'))
                                        partition_enable = d.getVar('FLASHLAYOUT_PARTITION_ENABLE_PROGRAMM_EMPTY')
                                if items[2] != '':
                                    if d.getVar('FLASHLAYOUT_PARTITION_SIZE:%s:%s' % (config, fl_label)):
                                        bb.debug(1, "FLASHLAYOUT_PARTITION_SIZE:%s:%s is already set to: %s." % (config, fl_label, d.getVar('FLASHLAYOUT_PARTITION_SIZE:%s:%s' % (config, fl_label))))
                                    elif d.getVar('FLASHLAYOUT_PARTITION_SIZE:%s:%s' % (config, fl_label_ori)):
                                        bb.debug(1, "Set FLASHLAYOUT_PARTITION_SIZE:%s:%s to %s." % (config, fl_label, d.getVar('FLASHLAYOUT_PARTITION_SIZE:%s:%s' % (config, fl_label_ori))))
                                        d.setVar('FLASHLAYOUT_PARTITION_SIZE:%s:%s' % (config, fl_label), d.getVar('FLASHLAYOUT_PARTITION_SIZE:%s:%s' % (config, fl_label_ori)))
                                    else:
                                        bb.debug(1, "Set FLASHLAYOUT_PARTITION_SIZE:%s:%s to %s." % (config, fl_label, items[2]))
                                        d.setVar('FLASHLAYOUT_PARTITION_SIZE:%s:%s' % (config, fl_label), items[2])
                                else:
                                    bb.debug(1, "No size setting for %s label : default setting would applied..." % fl_label)
                                if items[3] != '':
                                    if d.getVar('FLASHLAYOUT_PARTITION_TYPE:%s:%s' % (config, fl_label)):
                                        bb.debug(1, "FLASHLAYOUT_PARTITION_TYPE:%s:%s is already set to: %s." % (config, fl_label, d.getVar('FLASHLAYOUT_PARTITION_TYPE:%s:%s' % (config, fl_label))))
                                    elif d.getVar('FLASHLAYOUT_PARTITION_TYPE:%s:%s' % (config, fl_label_ori)):
                                        bb.debug(1, "Set FLASHLAYOUT_PARTITION_TYPE:%s:%s to %s." % (config, fl_label, d.getVar('FLASHLAYOUT_PARTITION_TYPE:%s:%s' % (config, fl_label_ori))))
                                        d.setVar('FLASHLAYOUT_PARTITION_TYPE:%s:%s' % (config, fl_label), d.getVar('FLASHLAYOUT_PARTITION_TYPE:%s:%s' % (config, fl_label_ori)))
                                    else:
                                        bb.debug(1, "Set FLASHLAYOUT_PARTITION_TYPE:%s:%s to %s." % (config, fl_label, items[3]))
                                        d.setVar('FLASHLAYOUT_PARTITION_TYPE:%s:%s' % (config, fl_label), items[3])
                                else:
                                    bb.debug(1, "No PARTITION_TYPE setting for %s label: default setting would applied..." % fl_label)
                                if len(items) == 4:
                                    bb.debug(1, "No PARTITION_COPY setting for %s label : default setting would applied..." % fl_label)
                                elif items[4] != '':
                                    if d.getVar('FLASHLAYOUT_PARTITION_COPY:%s:%s' % (config, fl_label)):
                                        bb.debug(1, "FLASHLAYOUT_PARTITION_COPY:%s:%s is already set to: %s." % (config, fl_label, d.getVar('FLASHLAYOUT_PARTITION_COPY:%s:%s' % (config, fl_label))))
                                    else:
                                        if d.getVar('ENABLE_FLASHLAYOUT_PARTITION_BINCOPY') == '0':
                                            bb.debug(1, "Set FLASHLAYOUT_PARTITION_COPY:%s:%s to %s." % (config, fl_label, '1'))
                                            d.setVar('FLASHLAYOUT_PARTITION_COPY:%s:%s' % (config, fl_label), '1')
                                        else:
                                            bb.debug(1, "Set FLASHLAYOUT_PARTITION_COPY:%s:%s to %s." % (config, fl_label_ori, items[4]))
                                            d.setVar('FLASHLAYOUT_PARTITION_COPY:%s:%s' % (config, fl_label_ori), items[4])
                                else:
                                    bb.debug(1, "No PARTITION_COPY setting for %s label : default setting would applied..." % fl_label)
                                if d.getVar('FLASHLAYOUT_PARTITION_OFFSET:%s:%s' % (config, fl_label)):
                                    bb.debug(1, "FLASHLAYOUT_PARTITION_OFFSET:%s:%s is already set to: %s." % (config, fl_label, d.getVar('FLASHLAYOUT_PARTITION_OFFSET:%s:%s' % (config, fl_label))))
                                elif d.getVar('FLASHLAYOUT_PARTITION_OFFSET:%s:%s' % (config, fl_label_ori)):
                                    bb.debug(1, "Set FLASHLAYOUT_PARTITION_OFFSET:%s:%s to '%s'." % (config, fl_label, d.getVar('FLASHLAYOUT_PARTITION_OFFSET:%s:%s' % (config, fl_label_ori))))
                                    d.setVar('FLASHLAYOUT_PARTITION_OFFSET:%s:%s' % (config, fl_label), d.getVar('FLASHLAYOUT_PARTITION_OFFSET:%s:%s' % (config, fl_label_ori)))
                                else:
                                    bb.debug(1, "No specific override defined for FLASHLAYOUT_PARTITION_OFFSET on %s label : default setting would applied..." % fl_label)
                                if d.getVar('FLASHLAYOUT_PARTITION_ENABLE:%s:%s' % (config, fl_label)):
                                    bb.debug(1, "FLASHLAYOUT_PARTITION_ENABLE:%s:%s is already set to: %s." % (config, fl_label, d.getVar('FLASHLAYOUT_PARTITION_ENABLE:%s:%s' % (config, fl_label))))
                                elif d.getVar('FLASHLAYOUT_PARTITION_ENABLE:%s:%s' % (config, fl_label_ori)):
                                    bb.debug(1, "Set FLASHLAYOUT_PARTITION_ENABLE:%s:%s to '%s'." % (config, fl_label, d.getVar('FLASHLAYOUT_PARTITION_ENABLE:%s:%s' % (config, fl_label_ori))))
                                    d.setVar('FLASHLAYOUT_PARTITION_ENABLE:%s:%s' % (config, fl_label), d.getVar('FLASHLAYOUT_PARTITION_ENABLE:%s:%s' % (config, fl_label_ori)))
                                else:
                                    bb.debug(1, "Set FLASHLAYOUT_PARTITION_ENABLE:%s:%s to '%s'." % (config, fl_label, partition_enable))
                                    d.setVar('FLASHLAYOUT_PARTITION_ENABLE:%s:%s' % (config, fl_label), partition_enable)
                        break
}

# -----------------------------------------------------------------------------
# Manage specific var dependency:
# Because of local overrides within create_flashlayout_config() function, we
# need to make sure to add each variables to the vardeps list.
def get_duplicate_labels(d, part_config):
    """
    Return the list of new labels created to duplicate requested partition
    configuration according to the available FLASHLAYOUT_CONFIG_LABELS.
    """
    l = []
    for o in d.getVar('FLASHLAYOUT_CONFIG_LABELS').split():
        for conf in part_config.split():
            for subconfigs in d.getVarFlag(conf, o).split():
                items = subconfigs.split(',')
                if len(items) > 4:
                    if items[4] != '1':
                        for duplabel in get_label_list(d, items[1], items[4]):
                            l.append(duplabel)
    return ' '.join(dict.fromkeys(l))

FLASHLAYOUT_LABELS_VARS = "CONFIG_LABELS PARTITION_LABELS TYPE_LABELS"
FLASHLAYOUT_LABELS_OVERRIDES = "${FLASHLAYOUT_BOOTSCHEME_LABELS} ${FLASHLAYOUT_CONFIG_LABELS}"
FLASHLAYOUT_LABELS_OVERRIDES += "${@' '.join('%s:%s' % (b, c) for b in d.getVar('FLASHLAYOUT_BOOTSCHEME_LABELS').split() for c in d.getVar('FLASHLAYOUT_CONFIG_LABELS').split())}"
do_create_flashlayout_config[vardeps] += "${@' '.join(['FLASHLAYOUT:%s:%s' % (v, o) for v in d.getVar('FLASHLAYOUT_LABELS_VARS').split() for o in d.getVar('FLASHLAYOUT_LABELS_OVERRIDES').split()])}"

FLASHLAYOUT_PARTITION_VARS = "ENABLE ID TYPE DEVICE OFFSET BIN2LOAD SIZE REPLACE_PATTERNS"
FLASHLAYOUT_PARTITION_CONFIGURED = "${@' '.join(dict.fromkeys(' '.join('%s' % d.getVar('FLASHLAYOUT_PARTITION_LABELS:%s' % o) for o in d.getVar('FLASHLAYOUT_LABELS_OVERRIDES').split()).split()))}"
FLASHLAYOUT_PARTITION_CONFIGURED += "${@' '.join('%s' % l for l in get_duplicate_labels(d, 'PARTITIONS_BOOTLOADER_CONFIG PARTITIONS_CONFIG').split())}"
FLASHLAYOUT_PARTITION_OVERRIDES = "${FLASHLAYOUT_LABELS_OVERRIDES} ${FLASHLAYOUT_PARTITION_CONFIGURED}"
FLASHLAYOUT_PARTITION_OVERRIDES += "${@' '.join('%s:%s' % (o, p) for o in d.getVar('FLASHLAYOUT_LABELS_OVERRIDES').split() for p in d.getVar('FLASHLAYOUT_PARTITION_CONFIGURED').split())}"
do_create_flashlayout_config[vardeps] += "${@' '.join(['FLASHLAYOUT_PARTITION:%s:%s' % (v, o) for v in d.getVar('FLASHLAYOUT_PARTITION_VARS').split() for o in d.getVar('FLASHLAYOUT_PARTITION_OVERRIDES').split()])}"

FLASHLAYOUT_DEVICE_VARS = "ALIGNMENT_SIZE BOARD_ENABLE START_OFFSET MAX_OFFSET"
do_create_flashlayout_config[vardeps] += "${@' '.join(['DEVICE:%s:%s' % (v, o) for v in d.getVar('FLASHLAYOUT_DEVICE_VARS').split() for o in d.getVar('DEVICE_STORAGE_NAMES').split()])}"
