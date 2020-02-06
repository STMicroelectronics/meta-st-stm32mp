#!/bin/sh

counter=1
DIRECTORY=/var/crash
DEPTH=5

mkdir -p "$DIRECTORY"
cd ${DIRECTORY}

MAX_HISTORIC=$(ls -1 | wc -l)
if [ $MAX_HISTORIC -gt $DEPTH ];
then
    #cleanup history
    ls -1 | sort -g | head -n $(($MAX_HISTORIC -$DEPTH +1)) | xargs --no-run-if-empty rm
fi

#get counter value
count=`ls -1 | sort -g | tail -n 1 | cut -d '_' -f 1`
count=$(($count + 1))

#register new dump
timestamp=$(date +%F_%H-%M-%S)
filename=${count}_m4-fw-error_${timestamp}.dump
cat /sys/${DEVPATH}/data > ${filename}
echo 1 > /sys/${DEVPATH}/data

# synchronize filesystem
sync
cd -
