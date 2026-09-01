#!/bin/sh

if [ -e /usr/sbin/ifplugd ]; then
    /usr/sbin/ifplugd -I -u0 -d10
fi
