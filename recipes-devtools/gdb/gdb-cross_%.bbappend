FILESEXTRAPATHS:prepend:stm32mpcommon := "${THISDIR}/files:"

#Add scripts for gdb/openocd/eclipse
SRC_URI:append:stm32mpcommon = " \
    file://gdbinit \
"

# Enable tui for the "layout" command.
PACKAGECONFIG:append:stm32mpcommon = " tui"

do_install:append:stm32mpcommon() {
   install -d ${D}/${bindir}/
   cp -a ${WORKDIR}/gdbinit ${D}/${bindir}/
}

