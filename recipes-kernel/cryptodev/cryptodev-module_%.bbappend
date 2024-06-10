FILESEXTRAPATHS:prepend := "${THISDIR}/${PN}:"

do_create_runtime_spdx[depends] += "virtual/kernel:do_create_runtime_spdx"


