#!/bin/bash -
#===============================================================================
#
#          FILE: create_sdcard_from_flashlayout.sh
#
#         USAGE: ./create_sdcard_from_flashlayout.sh
#
#   DESCRIPTION: generate raw image with information from flash layout
#
# SPDX-License-Identifier: MIT
#        AUTHOR: Christophe Priouzeau (christophe.priouzeau@st.com),
#  ORGANIZATION: STMicroelectronics
#     COPYRIGHT: Copyright (C) 2017, STMicroelectronics - All Rights Reserved
#       CREATED: 11/22/2017 15:03
#      REVISION:  ---
#===============================================================================

PRE_REQUISITE_TOOLS=" \
	sgdisk \
	du \
	dd \
"

unset FLASHLAYOUT_data
unset FLASHLAYOUT_filename
unset FLASHLAYOUT_rawname
unset FLASHLAYOUT_filename_path
unset FLASHLAYOUT_prefix_image_path
unset FLASHLAYOUT_number_of_line

declare -A FLASHLAYOUT_data

SDCARD_TOKEN=mmc0

# Size of 5GB
DEFAULT_RAW_SIZE=${SDCARD_SIZE:-5120}

# size of 768MB
DEFAULT_ROOTFS_PARTITION_SIZE=${ROOTFS_SIZE:-752640}

# 32 MB of Padding on B
DEFAULT_PADDING_SIZE=33554432

# fip
DEFAULT_FIP_TYPEUUID=19d5df83-11b0-457b-be2c-7559c13142a5
DEFAULT_FIP_A_PARTUUID=4fd84c93-54ef-463f-a7ef-ae25ff887087
DEFAULT_FIP_B_PARTUUID=09c54952-d5bf-45af-acee-335303766fb3
# metadata
DEFAULT_FWU_MDATA_TYPEUUID=8a7a84a0-8387-40f6-ab41-a8b9a5a60d23
DEFAULT_FWU_MDATA_A_PARTUUID=5f91c128-e120-48d7-bb64-9fa9aed4c7e3
DEFAULT_FWU_MDATA_B_PARTUUID=f469f981-5985-4206-8fb3-839956035a65
# u-boot env
DEFAULT_UBOOT_ENV_TYPEUUID=3de21764-95bd-54bd-a5c3-4abe786f38a8
# fs
DEFAULT_BOOTFS_TYPEUUID=0fc63daf-8483-4772-8e79-3d69d8477de4
DEFAULT_ROOTFS_TYPEUUID=0fc63daf-8483-4772-8e79-3d69d8477de4
# bootfs
DEFAULT_BOOTFS_SDCARD_PARTUUID=c9b01b31-b604-4c2a-95b4-e23ab45fd0c5
DEFAULT_BOOTFS_SDCARD_A_PARTUUID=c9b01b31-b604-4c2a-95b4-e23ab45fd0c5
DEFAULT_BOOTFS_SDCARD_B_PARTUUID=67a73cea-322c-42c0-b477-6ef0039fefed
# vendorfs
DEFAULT_VENDORFS_SDCARD_PARTUUID=8abdaae5-d940-463a-8326-a2c6af788fe6
DEFAULT_VENDORFS_SDCARD_A_PARTUUID=8abdaae5-d940-463a-8326-a2c6af788fe6
DEFAULT_VENDORFS_SDCARD_B_PARTUUID=d168dbce-65f3-423c-86af-a11528890b30
# rootfs
DEFAULT_ROOTFS_SDCARD_PARTUUID=e91c4e10-16e6-4c0e-bd0e-77becf4a3582
DEFAULT_ROOTFS_SDCARD_A_PARTUUID=e91c4e10-16e6-4c0e-bd0e-77becf4a3582
DEFAULT_ROOTFS_SDCARD_B_PARTUUID=360db83f-c038-4e10-ae79-1dce8846d2f0
# userfs
DEFAULT_USERFS_SDCARD_PARTUUID=2d750516-056c-4fae-9d73-f3141f1cffb4
DEFAULT_USERFS_SDCARD_A_PARTUUID=2d750516-056c-4fae-9d73-f3141f1cffb4
DEFAULT_USERFS_SDCARD_B_PARTUUID=7e036063-979d-4000-949d-33ab3e987de0

# Columns name on FLASHLAYOUT_data
COL_SELECTED_OPT=0
COL_PARTID=1
COL_PARTNAME=2
COL_PARTYPE=3
COL_IP=4
COL_OFFSET=5
COL_BIN2FLASH=6
COL_BIN2BOOT=7
COL_PARTITIONSIZE=8
COL_MAX=9

# SELECTED/OPT variable meaning:
# - : boot stage
# P: programme
# E: erase
# D: delete

WARNING_TEXT=""

# to specify the device on which the raw images would be flashed
# example: DEVICE=sdb ./create_sdcard_from_flashlayout.sh <tsv file>
DEFAULT_DEVICE=${DEVICE:-mmcblk0}
if $(echo "${DEFAULT_DEVICE}" | grep -q "mmcblk")
then
	DEFAULT_DEVICE_PART="${DEFAULT_DEVICE}p"
else
	DEFAULT_DEVICE_PART="${DEFAULT_DEVICE}"
fi

_COMPRESS_RAW_IMAGE=0
_FORCE_ROOTFS_SIZE=0

# -----------------------------------
die(){
	echo "$1" 1>&2
	exit 1
}

_REDIRECT="/dev/stdout"
if [ ! "$DEBUG" ];
then
	_REDIRECT="/dev/null"
	exec 2> /dev/null
fi

debug() {
	if [ "$DEBUG" ];
	then
		echo ""
		echo "[DEBUG]: $*"
	fi
}

function exec_print() {
	if [ "$DEBUG" ];
	then
		echo ""
		echo "[DEBUG EXEC]: $*"
	fi
}

function selection_test() {
	_result=1
	_select=$1
	shift
	#debug "selection_test: ref=$_select <$@>"
	while test $# != 0
	do
		#debug "selection_test: test between <${_select}> and <$1>"
		if [ "${_select}" == "$1" ]; then
			#debug "selection_test: test TRUE"
			_result=0
			break;
		fi
		shift
	done
	return $_result
}
tools_check() {
	for tools in ${PRE_REQUISITE_TOOLS}; do
		if [ "$(command -v "$tools" | wc -l )" -eq 0 ]; then
			echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
			echo "[ERROR]: $tools cannot be found on your system."
			echo "         Please check corresponding package is installed or your PATH variable is set correctly."
			echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
			exit 1
		fi
	done
}

function calculate_size_between_offset() {
	local offset1=$1
	local offset2=$2

	offset1=${offset1//0x/}
	offset1=$(echo ${offset1} | tr '[:lower:]' '[:upper:]')
	offset1_b=$(echo "obase=10; ibase=16; $offset1" | bc)

	offset2=${offset2//0x/}
	offset2=$(echo ${offset2} | tr '[:lower:]' '[:upper:]')
	offset2_b=$(echo "obase=10; ibase=16; $offset2" | bc)

	echo $((offset2_b - offset1_b))
}
# Read Flash Layout file and put information on array: FLASHLAYOUT_data
function read_flash_layout() {
	local i=0
	declare -a flashlayout_data     # Create an indexed array (necessary for the read command).
	FLASHLAYOUT_number_of_line=$(wc -l "$FLASHLAYOUT_filename" | cut -sf 1 -d ' ')
	debug "Number of line: $FLASHLAYOUT_number_of_line"
	while read -ra flashlayout_data; do
		selected=${flashlayout_data[0]}
		if selection_test "$selected" P E PD DP PE PED;
		then
			# Selected=
			FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]=${flashlayout_data[0]}
			# PartId
			FLASHLAYOUT_data[$i,$COL_PARTID]=${flashlayout_data[1]}
			#PartName
			FLASHLAYOUT_data[$i,$COL_PARTNAME]=${flashlayout_data[2]}
			#PartType
			FLASHLAYOUT_data[$i,$COL_PARTYPE]=${flashlayout_data[3]}
			#IP
			FLASHLAYOUT_data[$i,$COL_IP]=${flashlayout_data[4]}
			#Offset
			FLASHLAYOUT_data[$i,$COL_OFFSET]=${flashlayout_data[5]}
			#Bin2flash
			FLASHLAYOUT_data[$i,$COL_BIN2FLASH]=${flashlayout_data[6]}
			#Bin2boot
			FLASHLAYOUT_data[$i,$COL_BIN2BOOT]=${flashlayout_data[7]}
			# partition size (not calculated)
			FLASHLAYOUT_data[$i,$COL_PARTITIONSIZE]=0
			i=$((i+1))

			debug "READ: ${flashlayout_data[0]} ${flashlayout_data[1]} ${flashlayout_data[2]} ${flashlayout_data[3]} ..."
		fi
	done < "$FLASHLAYOUT_filename"

	for j in $(seq 1 $((i-1)))
	do
		off1=${FLASHLAYOUT_data[$((j-1)),$COL_OFFSET]}
		off2=${FLASHLAYOUT_data[$j,$COL_OFFSET]}
		FLASHLAYOUT_data[$((j-1)),$COL_PARTITIONSIZE]=$(calculate_size_between_offset $off1 $off2)
	done
	# manage last partition
	if [ "${FLASHLAYOUT_data[$((i-1)),$COL_PARTNAME]}" = "empty" ]; then
		# empty partition use to limit the size of partitions to flash
		FLASHLAYOUT_number_of_line=$((i-1))
	else
		last_partname=${FLASHLAYOUT_data[$((i-1)),$COL_PARTNAME]}
		is_ab=$(echo $last_partname | sed "s#.*[-_]\(b\)#\1#")
		if [ "$is_ab" = "b" ]; then
			# see partition before to now size of partition
			FLASHLAYOUT_data[$((i-1)),$COL_PARTITIONSIZE]=${FLASHLAYOUT_data[$((i-2)),$COL_PARTITIONSIZE]}
		fi
		FLASHLAYOUT_number_of_line=$i
	fi
}

function debug_dump_flashlayout_data_array() {
	columns=$COL_MAX
	for i in $(seq 0 $((FLASHLAYOUT_number_of_line -1)))
	do
		for ((j=0;j<columns;j++)) do
			echo -n " ${FLASHLAYOUT_data[$i,$j]}"
		done
		echo ""
	done
}

function calculate_max_size_from_flashlayout() {
	# get last image
	# get image and size of image
	# offset + size = maxsize module 1GB
	debug "Calculate Max size from Flashlayout"
	local last_index=$((FLASHLAYOUT_number_of_line -1))
	local last_offset=${FLASHLAYOUT_data[$last_index,$COL_OFFSET]}
	local last_image=${FLASHLAYOUT_data[$last_index,$COL_BIN2FLASH]}
	local last_partname=${FLASHLAYOUT_data[$last_index,$COL_PARTNAME]}
	
	if [ "$last_image" = "none" ]; then
		is_ab=$(echo $last_partname | sed "s#.*[-_]\(b\)#\1#")
		if [ "$is_ab" = "b" ]; then
			# a/b partition detected, need to take the previous image
			local index=$(($last_index - 1))
			last_image=${FLASHLAYOUT_data[$index,$COL_BIN2FLASH]}
			#image_size=$(du -Lb "$FLASHLAYOUT_prefix_image_path/$last_image" | tr '\t' ' ' | cut -d ' ' -f1)
			# use partition size calculated previously instead of binary size
			image_size=${FLASHLAYOUT_data[$index,$COL_PARTITIONSIZE]}
			image_size_mb=$(du -Lh "$FLASHLAYOUT_prefix_image_path/$last_image" | tr '\t' ' ' | cut -d ' ' -f1)
		else
			# use only offset to define size of raw image			
			image_size=$((512*1024*1024))
			image_size_mb="0M"
		fi
	else
		image_size=$(du -Lb "$FLASHLAYOUT_prefix_image_path/$last_image" | tr '\t' ' ' | cut -d ' ' -f1)
		image_size_mb=$(du -Lh "$FLASHLAYOUT_prefix_image_path/$last_image" | tr '\t' ' ' | cut -d ' ' -f1)
	fi

	offset=${last_offset//0x/}
	offset=$(echo ${offset} | tr '[:lower:]' '[:upper:]')
	offset_b=$(echo "obase=10; ibase=16; $offset" | bc)

	debug "   Last Image : $last_image"
	debug "   Last Image size : $image_size_mb ($image_size)"
	size=$(($offset_b + $image_size))
	gb=$((1024*1024*1024))
	half_gb=$((512*1024*1024))
	max_size=$(( (($size+$gb) / $gb) * 1024))
	modulo_size=$(( ($max_size * 1024 * 1024) - $size ))
	if [ $modulo_size -lt $half_gb ]; then
		max_size=$(( (($size+$gb+$half_gb) / $gb) * 1024))
	fi
	debug "   Max size from Flashlayout : $max_size MB"

	FLASHLAYOUT_max_size_requested=$max_size
}

# Verify and precise the path to image specified on Flash layout
function get_last_image_path() {
	local i=0
	last_image=""
	for i in $(seq 0 $((FLASHLAYOUT_number_of_line - 1)))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		partName=${FLASHLAYOUT_data[$i,$COL_PARTNAME]}
		bin2flash=${FLASHLAYOUT_data[$i,$COL_BIN2FLASH]}

		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			case "$selected" in
			1|P|D|PD|DP|PE|PED)
				if $(echo "$partName" | grep -q "rootfs[-_]b");
				then
					continue
				fi

				if $(echo "$partName" | grep -q "rootfs[-_]a");
				then
					last_image=$bin2flash
				else
					if $(echo "$partName" | grep -q "rootfs");
					then
						last_image=$bin2flash
					fi
				fi
				;;
			*)
				;;
			esac
		fi
		i=$((i+1))
	done
	if [ -n "$last_image" ];
	then
		if [ -f "$FLASHLAYOUT_filename_path/$last_image" ];
		then
			FLASHLAYOUT_prefix_image_path="$FLASHLAYOUT_filename_path"
		elif [ -f "$FLASHLAYOUT_filename_path/../$last_image" ];
		then
			FLASHLAYOUT_prefix_image_path="$FLASHLAYOUT_filename_path/.."
		elif [ -f "$FLASHLAYOUT_filename_path/../../$last_image" ];
		then
			FLASHLAYOUT_prefix_image_path="$FLASHLAYOUT_filename_path/../.."
		elif [ -f "$FLASHLAYOUT_filename_path/../../../$last_image" ];
		then
			FLASHLAYOUT_prefix_image_path="$FLASHLAYOUT_filename_path/../../.."
		else
			echo "[ERROR]: do not found image associated to this FLash layout on the directory:"
			echo "[ERROR]:    $FLASHLAYOUT_filename_path"
			echo "[ERROR]: or $FLASHLAYOUT_filename_path/.."
			echo "[ERROR]: or $FLASHLAYOUT_filename_path/../.."
			echo "[ERROR]: or $FLASHLAYOUT_filename_path/../../.."
			echo ""
			exit 0
		fi
	else
		FLASHLAYOUT_prefix_image_path="."
	fi
}

# -------------------------------
# calculate number of parition enable
function calculate_number_of_partition() {
	num=0
	for i in $(seq 0 $((FLASHLAYOUT_number_of_line - 1)))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		if [ "$ip" == "$SDCARD_TOKEN" ]
		then
			if selection_test "$selected" P E PD DP PE PED;
			then
				num=$((num+1))
			fi
		fi
	done

	echo "$num"
}

# ----------------------------------------
# move_partition_offset <begin_index_to_change> <new offset_b>
function move_partition_offset() {
	ind=$1
	new_offset=$2
	offset_hexa=$(printf "%x\n" "$new_offset")

	for ((k=ind;k<FLASHLAYOUT_number_of_line;k++))
	do
		selected=${FLASHLAYOUT_data[$k,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$k,$COL_IP]}

		if [ "$ip" == "$SDCARD_TOKEN" ]
		then
			if selection_test "$selected" P E PD DP PE PED;
			then
				#calculate actual size of partition (before update)
				# in case of last partition, we doesn't take care of tmp_next_offset
				# because there is no other partition to move.
				tmp_next_offset=${FLASHLAYOUT_data[$((k+1)),$COL_OFFSET]}
				tmp_cur_offset=${FLASHLAYOUT_data[$k,$COL_OFFSET]}
				tmp_partition_size=$((tmp_next_offset - tmp_cur_offset))

				#set new offset
				offset_hexa=$(printf "0x%x\n" "$new_offset")

				debug "${FLASHLAYOUT_data[$k,$COL_PARTNAME]}: Change Offset from ${FLASHLAYOUT_data[$k,$COL_OFFSET]}" \
					" to $offset_hexa"
				FLASHLAYOUT_data[$k,$COL_OFFSET]=$offset_hexa

				#calculate offset of next partition
				new_offset=$((new_offset + tmp_partition_size))
			fi
		fi
	done
}

# ----------------------------------------
function generate_gpt_partition_table_from_flash_layout() {
	local j=1
	local p=0
	local index_of_rootfs=20
	local index_of_rootfs_a=20
	local index_of_rootfs_b=20
	new_next_partition_offset_b=0
	number_of_partition=$( calculate_number_of_partition )

	exec_print "sgdisk -og -a 1 '$FLASHLAYOUT_rawname'"
	sgdisk -og -a 1 "$FLASHLAYOUT_rawname" &> ${_REDIRECT} || die "SGDISK: generate gpt partition table"

	echo "Create partition table:"

	for i in $(seq 0 $((FLASHLAYOUT_number_of_line-1)))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		partId=${FLASHLAYOUT_data[$i,$COL_PARTID]}
		partName=${FLASHLAYOUT_data[$i,$COL_PARTNAME]}
		partType=${FLASHLAYOUT_data[$i,$COL_PARTYPE]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		offset=${FLASHLAYOUT_data[$i,$COL_OFFSET]}
		bin2flash=${FLASHLAYOUT_data[$i,$COL_BIN2FLASH]}
		debug "DUMP Process for $partName partition"

		case "$selected" in
		P|E|1|PD|DP|PE|PED)
			# partition are present and must be created
			;;
		*)
			continue
			;;
		esac

		case "$partName" in
		boot|bootfs)
			# add boot flags on gpt parition
			extrafs_param=" -A $j:set:2"
			extrafs_param="$extrafs_param -u $j:${DEFAULT_BOOTFS_SDCARD_PARTUUID}"
			display_info="$display_info $j"
			;;
		bootfs-a|bootfs_a)
			extrafs_param="-u $j:${DEFAULT_BOOTFS_SDCARD_A_PARTUUID}"
			display_info="$display_info $j"
			;;
		bootfs-b|bootfs_b)
			extrafs_param="-u $j:${DEFAULT_BOOTFS_SDCARD_B_PARTUUID}"
			display_info="$display_info $j"
			;;
		vendorfs)
			extrafs_param="-u $j:${DEFAULT_VENDORFS_SDCARD_PARTUUID}"
			display_info="$display_info $j"
			;;
		vendorfs-a|vendorfs_a)
			extrafs_param="-u $j:${DEFAULT_VENDORFS_SDCARD_A_PARTUUID}"
			display_info="$display_info $j"
			;;
		vendorfs-b|vendorfs_b)
			extrafs_param="-u $j:${DEFAULT_VENDORFS_SDCARD_B_PARTUUID}"
			display_info="$display_info $j"
			;;
		rootfs)
			# add rootfs PARTUUID flags
			extrafs_param=" -u $j:${DEFAULT_ROOTFS_SDCARD_PARTUUID}"
			display_info="$display_info $j"
			;;
		rootfs-a|rootfs_a)
			extrafs_param="-u $j:${DEFAULT_ROOTFS_SDCARD_A_PARTUUID}"
			display_info="$display_info $j"
			;;
		rootfs-b|rootfs_b)
			extrafs_param="-u $j:${DEFAULT_ROOTFS_SDCARD_B_PARTUUID}"
			display_info="$display_info $j"
			;;
		userfs)
			extrafs_param="-u $j:${DEFAULT_USERFS_SDCARD_PARTUUID}"
			display_info="$display_info $j"
			;;
		userfs-a|userfs_a)
			extrafs_param="-u $j:${DEFAULT_USERFS_SDCARD_A_PARTUUID}"
			display_info="$display_info $j"
			;;
		userfs-b|userfs_b)
			extrafs_param="-u $j:${DEFAULT_USERFS_SDCARD_B_PARTUUID}"
			display_info="$display_info $j"
			;;

		fip-a*)
			# add fip-a PARTUUID flags
			extrafs_param=" -u $j:${DEFAULT_FIP_A_PARTUUID}"
			display_info="$display_info  $j"
			;;
		fip-b*)
			# add fip-b PARTUUID flags
			extrafs_param=" -u $j:${DEFAULT_FIP_B_PARTUUID}"
			display_info="$display_info  $j"
			;;
		metadata1)
			extrafs_param=" -u $j:${DEFAULT_FWU_MDATA_A_PARTUUID}"
			display_info="$display_info  $j"
			;;
		metadata2)
			extrafs_param=" -u $j:${DEFAULT_FWU_MDATA_B_PARTUUID}"
			display_info="$display_info  $j"
			;;
		*)
			extrafs_param=""
			;;
		esac

		# get size of image to put on partition
		if [ -n "$bin2flash" ];
		then
			if [ -e "$FLASHLAYOUT_prefix_image_path/$bin2flash" ];
			then
				image_size=$(du -Lb "$FLASHLAYOUT_prefix_image_path/$bin2flash" | tr '\t' ' ' | cut -d ' ' -f1)
				image_size_in_mb=$((image_size/1024/1024))
			else
				image_size=0
				image_size_in_mb=0
			fi
		else
			image_size=0
			image_size_in_mb=0
		fi

		# get offset
		#offset=$(echo $offset | sed -e "s/0x//")
		offset=${offset//0x/}
		offset=$(echo ${offset} | tr '[:lower:]' '[:upper:]')
		offset_b=$(echo "obase=10; ibase=16; $offset" | bc)

		offset=$((2 * offset_b / 1024))

		if [ $p -ne $((number_of_partition -1)) ];
		then
			# get the begin offset of next partition
			next_offset=${FLASHLAYOUT_data[$((i+1)),$COL_OFFSET]}
			next_offset=${next_offset//0x/}
			next_offset_b=$((16#$next_offset))
			if $(echo "$partName" | grep -q "rootfs");
			then
				if [ ${_FORCE_ROOTFS_SIZE} -eq 1 ]; then
					#force the size of rootfs parition to 768MB
					new_next_partition_offset_b=$((offset_b + 1024*DEFAULT_ROOTFS_PARTITION_SIZE))
					next_offset_b=$new_next_partition_offset_b

					move_partition_offset $((i+1)) $new_next_partition_offset_b
					FLASHLAYOUT_data[$i,$COL_PARTITIONSIZE]=$((new_next_partition_offset_b-offset_b))
				fi
				if [ "$partName" == "rootfs" ]; then
					index_of_rootfs=$i
				fi
				if [ "$partName" == "rootfs-a" -o "$partName" == "rootfs_a" ]; then
					index_of_rootfs_a=$i
				fi
				if [ "$partName" == "rootfs-b" -o "$partName" == "rootfs_b" ]; then
					index_of_rootfs_b=$i
				fi
			fi

			if [ "$i" -gt "$index_of_rootfs" ];
			then
				if [ $((next_offset_b + image_size)) -gt $((DEFAULT_RAW_SIZE * 1024*1024)) ]
				then
					echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
					echo "[ERROR]: The rootfs and/or other partitions doesn't enter on a SDCARD size of $DEFAULT_RAW_SIZE MB"
					echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
					exit 1
				fi
			fi
			if [ "$i" -gt "$index_of_rootfs_a" ];
			then
				if [ $((next_offset_b + image_size)) -gt $((DEFAULT_RAW_SIZE * 1024*1024)) ]
				then
					echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
					echo "[ERROR]: The rootfs_A and/or other partitions doesn't enter on a SDCARD size of $DEFAULT_RAW_SIZE MB"
					echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
					exit 1
				fi
			fi
			if [ "$i" -gt "$index_of_rootfs_b" ];
			then
				if [ $((next_offset_b + image_size)) -gt $((DEFAULT_RAW_SIZE * 1024*1024)) ]
				then
					echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
					echo "[ERROR]: The rootfs_B and/or other partitions doesn't enter on a SDCARD size of $DEFAULT_RAW_SIZE MB"
					echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
					exit 1
				fi
			fi
			next_offset=$((2 * next_offset_b / 1024))
			next_offset=$((next_offset -1))
			if [ $next_offset -eq -1 ];
			then
				next_offset=" "
				next_offset_b="0"
			fi
		else
			next_offset=" "
			next_offset_b="0"
		fi

		# calculate the size of partition
		partition_size=$((next_offset_b - offset_b))
		if [ $partition_size -lt 0 ];
		then
			partition_size=0
		fi

		if [ "$i" -ne $((FLASHLAYOUT_number_of_line -1)) ];
		then
			free_size=$((partition_size - image_size))
		else
			if [ ${FLASHLAYOUT_data[$i,$COL_PARTITIONSIZE]} -ne 0 ]; then
				free_size=1
				partition_size=${FLASHLAYOUT_data[$i,$COL_PARTITIONSIZE]}
				next_offset=$((2 * (offset_b + partition_size) / 1024))
			next_offset=$((next_offset -1))
			else
			    free_size=0
				partition_size=0
			fi
		fi

		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			debug "   DUMP selected  $selected"
			#debug "   DUMP partId    $partId"
			debug "   DUMP partName  $partName"
			#debug "   DUMP partType  $partType"
			#debug "   DUMP ip        $ip"
			debug "   DUMP offset    ${FLASHLAYOUT_data[$i,$COL_OFFSET]} ($offset)"
			#debug "   DUMP bin2flash $bin2flash"
			debug "   DUMP image size     $image_size"
			debug "   DUMP init partition size ${FLASHLAYOUT_data[$i,$COL_PARTITIONSIZE]}"
			debug "   DUMP partition size $partition_size"
			debug "   DUMP free size      $free_size "
			if selection_test "$selected" P E PD DP PE PED;
			then
				if [ $free_size -lt 0 ];
				then
					if $(echo "$partName" | grep -q "rootfs");
					then
						if [ ${_FORCE_ROOTFS_SIZE} -eq 1 ]; then
							echo "[WARNING]: IMAGE TOO BIG [$partName:$bin2flash $image_size B [requested $partition_size B]"
							echo "[WARNING]: try to move last partition"
							# rootfs are too big for the partition, we increase the size of
							# partition of real rootfs image size + DEFAULT_PADDING_SIZE
							new_next_partition_offset_b=$((offset_b + image_size + DEFAULT_PADDING_SIZE))

							move_partition_offset $((i+1)) $new_next_partition_offset_b

							if [ $new_next_partition_offset_b -gt $((DEFAULT_RAW_SIZE * 1024*1024)) ]
							then
								echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
								echo "[ERROR]: IMAGE TOO BIG [$partName:$bin2flash $image_size_in_mb MB [requested $partition_size B]"
								echo "[ERROR]: IMAGE + OFFSET of rootfs partition are superior of SDCARD size ($DEFAULT_RAW_SIZE)"
								echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
								exit 1
							fi
							next_offset=$((2 * new_next_partition_offset_b / 1024))
							next_offset=$((next_offset -1))
						fi
					else
						echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
						echo "[ERROR]: IMAGE TOO BIG [$partName:$bin2flash $image_size_in_mb MB [requested $partition_size B]]"
						echo "[ERROR]: sdcard size requested: $DEFAULT_RAW_SIZE MB"
						echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
						exit 1
					fi
				fi

				if [ $p -eq $((number_of_partition -1)) ];
				then
					temp_end_offset_b=$((offset_b + image_size))
					if [ $temp_end_offset_b -gt $((DEFAULT_RAW_SIZE * 1024*1024)) ];
					then
						echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
						echo "[ERROR]: IMAGE TOO BIG [$partName:$bin2flash $image_size_in_mb MB]"
						echo "[ERROR]: There is not enough place on last partition($partName)"
						echo "[ERROR]: sdcard size requested: $DEFAULT_RAW_SIZE MB"
						echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
						exit 1
					fi
				fi
				case $partType in
				Binary)
					# Linux reserved: 0x8301
					gpt_code="8301"
					;;
				FIP)
					# FIP specific TYPE UUID
					gpt_code=$DEFAULT_FIP_TYPEUUID
					;;
				FWU_MDATA)
					# TF-A firmware update metadata TYPE UUID
					gpt_code=$DEFAULT_FWU_MDATA_TYPEUUID
					;;
				System|FileSystem) #FileSystem
					# Linux File system: 0x8300
					gpt_code="8300"
					case "$partName" in
					boot|bootfs|bootfs_a|bootfs_b|bootfs-a|bootfs-b)
						gpt_code=$DEFAULT_BOOTFS_TYPEUUID
						;;
					vendorfs|vendorfs_a|vendorfs_b|vendorfs-a|vendorfs-b)
						gpt_code=$DEFAULT_ROOTFS_TYPEUUID
						;;
					rootfs|rootfs_a|rootfs_b|rootfs-a|rootfs-b)
						gpt_code=$DEFAULT_ROOTFS_TYPEUUID
						;;
					userfs|userfs_a|userfs_b|userfs-a|userfs-b)
						gpt_code=$DEFAULT_ROOTFS_TYPEUUID
						;;
					esac
					;;
				ESP)
					# ESP
					gpt_code="ef00"
					;;
				ENV)
					# U-Boot enviromnent
					gpt_code=$DEFAULT_UBOOT_ENV_TYPEUUID
					;;
				*)
					echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
					echo "[ERROR]: invalid partition type:  $partType for $partName"
					echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
					exit 1
					;;
				esac

				printf "part %d: %8s ..." $j "$partName"
				exec_print "sgdisk -a 1 -n $j:$offset:$next_offset -c $j:$partName -t $j:$gpt_code $extrafs_param '$FLASHLAYOUT_rawname'"
				sgdisk -a 1 -n $j:$offset:$next_offset -c $j:$partName -t $j:$gpt_code $extrafs_param "$FLASHLAYOUT_rawname" &> ${_REDIRECT} || die "SGDISK: create gpt parition $$partName"
				partition_size=$(sgdisk -p "$FLASHLAYOUT_rawname" | grep "$partName" | grep -v "\-$partName" | grep -v "First usable" | awk '{ print $4}')
				partition_size_type=$(sgdisk -p "$FLASHLAYOUT_rawname" | grep "$partName" | grep -v "\-$partName" | grep -v "First usable" | awk '{ print $5}')
				printf "\r[CREATED] part %02d: %10s [partition size %s %s]\n" $j "$partName"  "$partition_size" "$partition_size_type"

			j=$((j+1))
			fi
		p=$((p+1))
		fi
	done

	echo ""
	echo "Partition table from '$FLASHLAYOUT_rawname'"
	exec_print "sgdisk -p '$FLASHLAYOUT_rawname'"
	sgdisk -p "$FLASHLAYOUT_rawname" &> ${_REDIRECT}
	for info in $display_info;
	do
		echo ""
		exec_print "sgdisk '$FLASHLAYOUT_rawname' -i $info"
		sgdisk "$FLASHLAYOUT_rawname" -i $info
	done
	echo ""
}

function generate_empty_raw_image() {
	# Initialize image file (due to bs we force seek on K)
	echo "Create Raw empty image: '$FLASHLAYOUT_rawname' of ${DEFAULT_RAW_SIZE}MB"
	exec_print "dd if=/dev/zero of='$FLASHLAYOUT_rawname' bs=1024 count=0 seek=${DEFAULT_RAW_SIZE}K"
	dd if=/dev/zero of="$FLASHLAYOUT_rawname" bs=1024 count=0 seek=${DEFAULT_RAW_SIZE}K &> ${_REDIRECT} || die "DD: error during creation of empty raw image"
}

function populate_gpt_partition_table_from_flash_layout() {
	local i=1
	local j=1
	echo "Populate raw image with image content:"

	for i in $(seq 0 $((FLASHLAYOUT_number_of_line-1)))
	do
		#debug "DUMP LINE=${FLASHLAYOUT_data[$i]}"
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		partId=${FLASHLAYOUT_data[$i,$COL_PARTID]}
		partName=${FLASHLAYOUT_data[$i,$COL_PARTNAME]}
		partType=${FLASHLAYOUT_data[$i,$COL_PARTYPE]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		offset=${FLASHLAYOUT_data[$i,$COL_OFFSET]}
		bin2flash=${FLASHLAYOUT_data[$i,$COL_BIN2FLASH]}

		offset=$(printf "%d\n" $offset)

		debug "   DUMP $selected $partId $partName $partType"
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			#debug "   DUMP selected  $selected"
			#debug "   DUMP partId    $partId"
			debug "   DUMP partName  $partName"
			#debug "   DUMP partType  $partType"
			#debug "   DUMP ip        $ip"
			debug "   DUMP offset    $offset"
			#debug "   DUMP bin2flash $bin2flash"
			if selection_test "$selected" P PD DP PE PED;
			then
				# Populate only the partition in "P"
				if [ -e "$FLASHLAYOUT_prefix_image_path/$bin2flash" ];
				then
					printf "part %02d: %10s, image: %s ..." $j "$partName" "$bin2flash"
					exec_print "dd if=$FLASHLAYOUT_prefix_image_path/$bin2flash of='$FLASHLAYOUT_rawname' conv=fdatasync,notrunc seek=1 bs=$offset"
					dd if="$FLASHLAYOUT_prefix_image_path/$bin2flash" of="$FLASHLAYOUT_rawname" conv=fdatasync,notrunc seek=1 bs=$offset &> ${_REDIRECT}
					printf "\r[ FILLED ] part %02d: %10s, image: %s \n" $j "$partName" "$bin2flash"
				else
					if [ ! "$(basename "$FLASHLAYOUT_prefix_image_path/$bin2flash")" = "none" ];
					then
						printf "\r[UNFILLED] part %02d: %10s, image: %s (not present) \n" $j "$partName" "$bin2flash"
						echo "   [WARNING]: THE FILE $FLASHLAYOUT_prefix_image_path/$bin2flash ARE NOT PRESENT."
						echo "   [WARNING]: THE PARTITION $partName ARE NOT FILL."
						WARNING_TEXT+="[WARNING]: THE PARTITION $partName ARE NOT FILL (file $FLASHLAYOUT_prefix_image_path/$bin2flash are not present) #"
					fi
				fi
				j=$((j+1))
			else
				if selection_test "$selected" E;
				then
					printf "\r[UNFILLED] part %02d: %10s, \n" $j "$partName"
					j=$((j+1))
				fi
			fi
		fi
	done
}

# ----------------------------------------
# ----------------------------------------
function print_schema_on_infofile() {
	local j=1
	local i=1
	# print schema of partition
	i=1
	for i in $(seq 0 $((FLASHLAYOUT_number_of_line-1)))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if selection_test "$selected" P E PD DP PE PED;
			then
				echo -n "==============" >> "$FLASHLAYOUT_infoname"
			fi
		fi
	done
	echo "=" >> "$FLASHLAYOUT_infoname"

	#empty line
	for i in $(seq 0 $((FLASHLAYOUT_number_of_line-1)))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if selection_test "$selected" P E PD DP PE PED;
			then
				echo -n "=             " >> "$FLASHLAYOUT_infoname"
			fi
		fi
	done
	echo "=" >> "$FLASHLAYOUT_infoname"
	# part name
	for i in $(seq 0 $((FLASHLAYOUT_number_of_line-1)))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		partName=${FLASHLAYOUT_data[$i,$COL_PARTNAME]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if selection_test "$selected" P E PD DP PE PED;
			then
				printf "=  %09s  " "$partName" >> "$FLASHLAYOUT_infoname"
			fi
		fi
	done
	echo "=" >> "$FLASHLAYOUT_infoname"
	#empty
	for i in $(seq 0 $((FLASHLAYOUT_number_of_line-1)))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if selection_test "$selected" P E PD DP PE PED;
			then
				echo -n "=             " >> "$FLASHLAYOUT_infoname"
			fi
		fi
	done
	echo "=" >> "$FLASHLAYOUT_infoname"
	# partition number
	for i in $(seq 0 $((FLASHLAYOUT_number_of_line-1)))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if selection_test "$selected" P E PD DP PE PED;
			then
				printf "= %09s%-2d " "${DEFAULT_DEVICE_PART}" "$j"  >> "$FLASHLAYOUT_infoname"
				j=$((j+1))
			fi
		fi
	done
	echo "=" >> "$FLASHLAYOUT_infoname"
	j=1
	for i in $(seq 0 $((FLASHLAYOUT_number_of_line-1)))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if selection_test "$selected" P E PD DP PE PED;
			then
				printf "=      (%-2d)   " $j>> "$FLASHLAYOUT_infoname"
				j=$((j+1))
			fi
		fi
	done
	echo "=" >> "$FLASHLAYOUT_infoname"
	for i in $(seq 0 $((FLASHLAYOUT_number_of_line-1)))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if selection_test "$selected" P E PD DP PE PED;
			then
				echo -n "=             " >> "$FLASHLAYOUT_infoname"
			fi
		fi
	done
	echo "=" >> "$FLASHLAYOUT_infoname"
	for i in $(seq 0 $((FLASHLAYOUT_number_of_line-1)))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if selection_test "$selected" P E PD DP PE PED;
			then
				echo -n "==============" >> "$FLASHLAYOUT_infoname"
			fi
		fi
	done
	echo "=" >> "$FLASHLAYOUT_infoname"
	# print legend of partition
	j=1
	for i in $(seq 0 $((FLASHLAYOUT_number_of_line-1)))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		partName=${FLASHLAYOUT_data[$i,$COL_PARTNAME]}
		bin2flash=${FLASHLAYOUT_data[$i,$COL_BIN2FLASH]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if selection_test "$selected" P E PD DP PE PED;
			then
				{
					echo "($j):"
					echo "    Device: /dev/${DEFAULT_DEVICE_PART}$j"
					echo "    Label:  $partName"
					if [ -n "$bin2flash" ];
					then
						echo "    Image:  $bin2flash"
					else
						echo "    Image:"
				    fi
				}  >> "$FLASHLAYOUT_infoname"
				j=$((j+1))
			fi
		fi
	done
}

function print_populate_on_infofile() {
	local j=1
	for i in $(seq 0 $((FLASHLAYOUT_number_of_line-1)))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		partName=${FLASHLAYOUT_data[$i,$COL_PARTNAME]}
		bin2flash=${FLASHLAYOUT_data[$i,$COL_BIN2FLASH]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if selection_test "$selected" P E PD DP PE PED;
			then
				if selection_test "$selected" E;
				then
					echo "- Populate partition $partName (/dev/${DEFAULT_DEVICE_PART}$j)" >> "$FLASHLAYOUT_infoname"
					if [ -n "$bin2flash" ];
					then
						echo "    dd if=$bin2flash of=/dev/${DEFAULT_DEVICE_PART}$j bs=1M conv=fdatasync status=progress" >> "$FLASHLAYOUT_infoname"
					else
						echo "    dd if=<raw image of $partName> of=/dev/${DEFAULT_DEVICE_PART}$j bs=1M conv=fdatasync status=progress" >> "$FLASHLAYOUT_infoname"
					fi
				else
					echo "- Populate partition $partName (/dev/${DEFAULT_DEVICE_PART}$j)" >> "$FLASHLAYOUT_infoname"
					echo "    dd if=$bin2flash of=/dev/${DEFAULT_DEVICE_PART}$j bs=1M conv=fdatasync status=progress" >> "$FLASHLAYOUT_infoname"
				fi
				echo "" >> "$FLASHLAYOUT_infoname"
				j=$((j+1))
			fi
		fi
	done
}

function print_mount_on_infofile() {
	local j=1
	for i in $(seq 0 $((FLASHLAYOUT_number_of_line-1)))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		partName=${FLASHLAYOUT_data[$i,$COL_PARTNAME]}
		partType=${FLASHLAYOUT_data[$i,$COL_PARTYPE]}
		bin2flash=${FLASHLAYOUT_data[$i,$COL_BIN2FLASH]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if selection_test "$selected" P E PD DP PE PED;
			then
				if selection_test "$partType" "System" "FileSystem";
				then
					echo "- Mount manually partition $partName (/dev/${DEFAULT_DEVICE_PART}$j)" >> "$FLASHLAYOUT_infoname"
					echo "    udiskctl mount -b /dev/disk/by-partlabel/$partName" >> "$FLASHLAYOUT_infoname"
					echo "" >> "$FLASHLAYOUT_infoname"
				fi
				j=$((j+1))
			fi
		fi
	done
}

function create_info() {

cat > "$FLASHLAYOUT_infoname"  << EOF
This file describe How to update manually the partition of SDCARD:
1. SDCARD schema of partition
2. How to populate each partition
3. How to update the kernel/devicetree

1. SDCARD schema of partition:
------------------------------

EOF
print_schema_on_infofile

cat >> "$FLASHLAYOUT_infoname"  << EOF

2. How to populate each partition
---------------------------------
EOF

print_populate_on_infofile

cat >> "$FLASHLAYOUT_infoname"  << EOF

3. How to mount manually each partition
---------------------------------
EOF

print_mount_on_infofile

cat >> "$FLASHLAYOUT_infoname"  << EOF

4. How to update the kernel/devicetree
--------------------------------------
The kernel and devicetree are present on "boot" partition.
To change kernel and devicetree, you can copy the file on this partitions:
- plug SDCARD on your PC
- copy kernel uImage on SDCARD
   sudo cp uImage /media/\$USER/bootfs/
- copy devicetree uImage on SDCARD
   sudo cp stm32mp*.dtb /media/\$USER/bootfs/
- umount partitions of SDCARD
   sudo umount /media/\$USER/bootfs/
   (dont't forget to umount the other partitions of SDCARD:
   sudo umount \`lsblk --list | grep ${DEFAULT_DEVICE} | grep part | gawk '{ print \$7 }' | tr '\\n' ' '\`
   )

EOF

}
# ----------------------------------------
# ----------------------------------------

function print_info() {
	echo ""
	echo "###########################################################################"
	echo "###########################################################################"
	echo ""
	echo "RAW IMAGE generated: '$FLASHLAYOUT_rawname'"
	echo ""
	echo "WARNING: before to use the command dd, please umount all the partitions"
	echo "	associated to SDCARD."
	echo "    sudo umount \`lsblk --list | grep ${DEFAULT_DEVICE} | grep part | gawk '{ print \$7 }' | tr '\\n' ' '\`"
	echo ""
	echo "To put this raw image on sdcard:"
	echo "    sudo dd if='$FLASHLAYOUT_rawname' of=/dev/${DEFAULT_DEVICE} bs=8M conv=fdatasync status=progress"
	echo ""
	echo "(${DEFAULT_DEVICE} can be replaced by:"
	echo "     sdX if it's a device dedicated to receive the raw image "
	echo "          (where X can be a, b, c, d, e)"
	echo ""
	echo "To mount bootfs partition:"
	echo "     udisksctl mount -b /dev/disk/by-partlabel/bootfs"
	echo ""
	echo "After the dd command you can verify if copy are correctly done and partitions take into account"
	echo "     sgdisk /dev/${DEFAULT_DEVICE} -p"
	echo "     sgdisk /dev/${DEFAULT_DEVICE} -v"
	echo "if '-v' command indicate a problem, please execute the following command:"
	echo "     sgdisk /dev/${DEFAULT_DEVICE} -e"
	echo ""
	echo "###########################################################################"
	echo "###########################################################################"
}

function print_warning() {
	if [ -n "$WARNING_TEXT" ];
	then
		echo ""
		echo "???????????????????????????????????????????????????????????????????????????"
		echo "???????????????????????????????????????????????????????????????????????????"
		OLD_IFS=$IFS
		IFS=$'\n'
		for t in $(echo "$WARNING_TEXT" | tr '#' '\n');
		do
			echo "$t"
		done
		IFS=$OLD_IFS
		echo "[WARNING]: IT'S POSSIBLE, THE BOARD DOES NOT BOOT CORRECTLY DUE TO "
		echo "           FILE(s) NOT PRESENT."
		echo "???????????????????????????????????????????????????????????????????????????"
		echo "???????????????????????????????????????????????????????????????????????????"
	fi
}

function usage() {
	echo ""
	echo "Help:"
	echo "   $0 [-h|--help] [--compress] <FlashLayout file>"
	echo ""
	echo "   -h    :		this help"
	echo "   --help:		this help"
	echo "   --compress:		compress the raw image generated"
	echo "   --force-rootfs:	force to use predefined rootfs size ($((DEFAULT_ROOTFS_PARTITION_SIZE / 1024)) MB)"
	echo ""
	echo "By setting SDCARD_SIZE on shell environment or calling the script with it you can limit the size of RAW sdcard"
	echo "SDCARD_SIZE=<value on MB>"
	echo "ex.: SDCARD_SIZE=2048 ./script/create_sdcard_from_flashlayout.sh <flashlayout>"
	echo " this example limit the size of sdcard to 2GB (2048MB)"
	echo ""
	echo "By setting DEVICE on shell environment or calling the script with it you can customize the command"
	echo "ex.: DEVICE=sdb ./script/create_sdcard_from_flashlayout.sh <flashlayout>"
	exit 1
}
# ------------------
#        Main
# ------------------

# check opt args
if [ $# -gt 1 ]; then
	while test $# != 1
	do
		case "$1" in
		--help|-h)
			usage
			return 0
			;;
		--compress)
			_COMPRESS_RAW_IMAGE=1
			;;
		--force-rootfs)
			_FORCE_ROOTFS_SIZE=1
			;;
		-*)
			echo "Wrong parameter: $1"
			usage
			return 1
			;;
		esac
		shift
	done
else
	if [ $# -eq 1 ];
	then
		case "$1" in
		--help|-h)
			usage
			return 0
			;;
		--compress)
			_COMPRESS_RAW_IMAGE=1
			;;
		--force-rootfs)
			_FORCE_ROOTFS_SIZE=1
			;;
		-*)
			echo "Wrong parameter: $1"
			usage
			return 1
			;;
		esac
	else
		echo "[ERROR]: bad number of parameters"
		echo ""
		usage
	fi
fi

if [ $# -ne 1 ];
then
	echo "[ERROR]: bad number of parameters"
	echo ""
	usage
fi

tools_check

FLASHLAYOUT_filename=$1
FLASHLAYOUT_filename_path=$(dirname "$FLASHLAYOUT_filename")
FLASHLAYOUT_filename_name=$(basename "$FLASHLAYOUT_filename")
FLASHLAYOUT_dirname=$(basename "$FLASHLAYOUT_filename_path")

if [ ! -e ${FLASHLAYOUT_filename} ]; then
	echo ""
	echo "[ERROR]: The Flashlayout file: ${FLASHLAYOUT_filename} doesn't exist"
	echo "[ERROR]: Please use a valid flashlayout file."
	usage
fi

_extension="${FLASHLAYOUT_filename##*.}"
if [ ! "$_extension" == "tsv" ];
then
	echo ""
	echo "[ERROR]: bad extension of Flashlayout file."
	echo "[ERROR]: the flashlayout must have a tsv extension."
	usage
fi
# File have a correct extension
#
if $(echo "$FLASHLAYOUT_dirname" | grep -q flashlayout)
then
	# add directory name as prefix for raw image
	new_filename=$(echo "$FLASHLAYOUT_dirname/$FLASHLAYOUT_filename_name" | sed -e "s|/|_|g")
	filename_for_raw_to_use="$FLASHLAYOUT_filename_path/$new_filename"
else
	filename_for_raw_to_use="$FLASHLAYOUT_filename"
fi
FLASHLAYOUT_rawname=$(basename "$filename_for_raw_to_use" | sed -e "s/tsv/raw/")
FLASHLAYOUT_infoname=$(basename "$filename_for_raw_to_use" | sed -e "s/tsv/how_to_update.txt/")

# check if flashlayout have sdcard name
if [ "$(grep -ic "$SDCARD_TOKEN" "$FLASHLAYOUT_filename")" -eq 0 ];
then
	echo ""
	echo "[WARNING]: THE FLASHLAYOUT NAME DOES NOT CONTAINS SDCARD REFERENCE."
	echo "[WARNING]: SDCARD TYPE = $SDCARD_TOKEN"
	echo "[WARNING]: FILE=$FLASHLAYOUT_filename"
	echo "Terminated without generated raw file."
	exit 0
fi

read_flash_layout
#debug_dump_flashlayout_data_array
get_last_image_path

calculate_max_size_from_flashlayout

if [ $DEFAULT_RAW_SIZE -lt $FLASHLAYOUT_max_size_requested ]; then
	echo ""
	echo "The Size of raw image calculated with Flashlayout content is "
	echo "superior of default sdcard size defined (SDCARD_SIZE=$DEFAULT_RAW_SIZE)"
	echo ""
	echo "You can force the size of final RAW image like following command:"
	echo "SDCARD_SIZE=$FLASHLAYOUT_max_size_requested $0 $@"
	echo "(SDCARD_SIZE is on MB)"
	echo ""
	echo "Would you like to use new sdcard size calculated to generate raw image ? [Y/n]"
	read -r answer
	if [[ "$answer" =~ ^[Yy]+[ESes]* ]]; then
		DEFAULT_RAW_SIZE=$FLASHLAYOUT_max_size_requested
	elif [ "${answer}" = "" ]; then
		DEFAULT_RAW_SIZE=$FLASHLAYOUT_max_size_requested
	else
		echo "[ERROR] the size of sdcard target is to small for raw image generation"
		echo "[ERROR] Using (SDCARD_SIZE=$DEFAULT_RAW_SIZE) is not suffisant to fill the content described on Flashlayout"
		echo "[ERROR] Please increase size:"
		echo "[ERROR] ex.: SDCARD_SIZE=$FLASHLAYOUT_max_size_requested $0 $@"
		exit 0
	fi
fi

#put the raw image generate near the binaries images
FLASHLAYOUT_rawname="$FLASHLAYOUT_prefix_image_path/$FLASHLAYOUT_rawname"
FLASHLAYOUT_infoname="$FLASHLAYOUT_prefix_image_path/$FLASHLAYOUT_infoname"

# erase previous raw image
if [ -f "$FLASHLAYOUT_rawname" ];
then
	echo ""
	echo "[WARNING]: A previous raw image are present on this directory"
	echo "[WARNING]:    '$FLASHLAYOUT_rawname'"
	echo "[WARNING]: would you like to continue and overwrite it: [Y/n]"
	read -r answer
	if [[ "$answer" =~ ^[Yy]+[ESes]* ]]; then
		rm -f "$FLASHLAYOUT_rawname" "$FLASHLAYOUT_infoname"
	elif [ "${answer}" = "" ]; then
		rm -f "$FLASHLAYOUT_rawname" "$FLASHLAYOUT_infoname"
	else
		exit 0
	fi
fi

debug "DUMP FlashLayout name:      $FLASHLAYOUT_filename"
debug "DUMP FlashLayout dir path:  $FLASHLAYOUT_filename_path"
debug "DUMP images dir path:       $FLASHLAYOUT_prefix_image_path"
debug "DUMP RAW SDCARD image name: $FLASHLAYOUT_rawname"

generate_empty_raw_image
generate_gpt_partition_table_from_flash_layout ""
populate_gpt_partition_table_from_flash_layout ""

if [ ${_COMPRESS_RAW_IMAGE} -eq 1 ]; then
	echo "Compress Raw image"
	rm -f "${FLASHLAYOUT_rawname}".xz
	xz -z -v "$FLASHLAYOUT_rawname"
fi
create_info
print_info
print_warning
