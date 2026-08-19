#!/bin/bash
# SPDX-License-Identifier: MIT
#
# Validate bootup bank

get_active_index()
{
    rootfs_guid=$(sed -n 's/.*root=PARTUUID=\([^ ]*\).*/\1/p' < /proc/cmdline)

    for bank_index in 0 1; do
        for guid in $(fwumdata -l | awk -v bank="$bank_index" '
                $1 == "Bank" && $2 == bank ":" {
                    getline
                    if ($1 == "Image" && $2 == "GUID:")
                        print $3
                }
            '); do
                if [ "${guid}" = "${rootfs_guid}" ]; then
                    echo ${bank_index}
                    return 0;
                fi
        done
    done
}

active_index="$(get_active_index)"
bank_state=$(fwumdata | awk -v bank="${active_index}" '
        $1 == "Bank" && $2 == bank ":" {
            print $3
        }
    ')

if [ "${bank_state}" = "valid" ]; then
    fwumdata -s "${active_index}" accepted
fi

exit 0
