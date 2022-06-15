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

# Size of 2GB
#DEFAULT_RAW_SIZE=2048
# Size of 1.5GB
DEFAULT_RAW_SIZE=1536

# size of 768MB
DEFAULT_ROOTFS_PARTITION_SIZE=768432
# size of 1024MB
#DEFAULT_ROOTFS_PARTITION_SIZE=1232896


# 32 MB of Padding on B
DEFAULT_PADDING_SIZE=33554432

DEFAULT_SDCARD_PARTUUID=e91c4e10-16e6-4c0e-bd0e-77becf4a3582
DEFAULT_FIP_TYPEUUID=19d5df83-11b0-457b-be2c-7559c13142a5
DEFAULT_FIP_A_PARTUUID=4fd84c93-54ef-463f-a7ef-ae25ff887087
DEFAULT_FIP_B_PARTUUID=09c54952-d5bf-45af-acee-335303766fb3

# Columns name on FLASHLAYOUT_data
COL_SELECTED_OPT=0
COL_PARTID=1
COL_PARTNAME=2
COL_PARTYPE=3
COL_IP=4
COL_OFFSET=5
COL_BIN2FLASH=6
COL_BIN2BOOT=7

# SELECTED/OPT variable meaning:
# - : boot stage
# P: programme
# E: erase
# D: delete

WARNING_TEXT=""

usage() {
	echo "Usage: $0 <flashlayout>"
	exit 0
}

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
		eval "$@"
	else
		eval "$@" 2> /dev/null > /dev/null
	fi
}
function exec_display_print() {
	if [ "$DEBUG" ];
	then
		echo ""
		echo "[DEBUG EXEC]: $*"
		eval "$@"
	else
		eval "$@" 2> /dev/null
	fi
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

# Read Flash Layout file and put information on array: FLASHLAYOUT_data
function read_flash_layout() {
	local i=0
	declare -a flashlayout_data     # Create an indexed array (necessary for the read command).
	FLASHLAYOUT_number_of_line=$(wc -l "$FLASHLAYOUT_filename" | cut -sf 1 -d ' ')
	debug "Number of line: $FLASHLAYOUT_number_of_line"
	while read -ra flashlayout_data; do
		selected=${flashlayout_data[0]}
		if [ "$selected" == "P" ] || [ "$selected" == "E" ] || [ "$selected" == "PD" ] || [ "$selected" == "DP" ] || [ "$selected" == "PED" ];
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

			i=$((i+1))

			debug "READ: ${flashlayout_data[0]} ${flashlayout_data[1]} ${flashlayout_data[2]} ${flashlayout_data[3]} ..."
		fi
	done < "$FLASHLAYOUT_filename"

	FLASHLAYOUT_number_of_line=$i
}

function debug_dump_flashlayout_data_array() {
	columns=8
	for ((i=0;i<FLASHLAYOUT_number_of_line;i++)) do
		for ((j=0;j<columns;j++)) do
			echo -n " ${FLASHLAYOUT_data[$i,$j]}"
		done
		echo ""
	done
}

# Verify and precise the path to image specified on Flash layout
function get_last_image_path() {
	local i=0
	last_image=""
	for i in $(seq 0 $FLASHLAYOUT_number_of_line)
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		partName=${FLASHLAYOUT_data[$i,$COL_PARTNAME]}
		bin2flash=${FLASHLAYOUT_data[$i,$COL_BIN2FLASH]}

		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			case "$selected" in
			1|P|D|PD|DP|PED)
				if [ "$partName" == 'rootfs' ];
				then
					last_image=$bin2flash
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
	fi
}

# -------------------------------
# calculate number of parition enable
function calculate_number_of_partition() {
	num=0
	for ((i=0;i<FLASHLAYOUT_number_of_line;i++))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		if [ "$ip" == "$SDCARD_TOKEN" ]
		then
			if [ "$selected" == "P" ] || [ "$selected" == "E" ] || [ "$selected" == "PD" ] || [ "$selected" == "DP" ] || [ "$selected" == "PED" ];
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
			if [ "$selected" == "P" ] || [ "$selected" == "E" ] || [ "$selected" == "PD" ] || [ "$selected" == "DP" ] || [ "$selected" == "PED" ];
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
	new_next_partition_offset_b=0
	number_of_partition=$( calculate_number_of_partition )

	exec_print "sgdisk -og -a 1 $FLASHLAYOUT_rawname"

	echo "Create partition table:"

	for ((i=0;i<FLASHLAYOUT_number_of_line;i++))
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
		P|E|1|PD|DP|PED)
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
			;;
		rootfs)
			# add rootfs PARTUUID flags
			extrafs_param=" -u $j:${DEFAULT_SDCARD_PARTUUID}"
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
		offset_b=$((16#$offset))

		offset=$((2 * offset_b / 1024))

		if [ $p -ne $((number_of_partition -1)) ];
		then
			# get the begin offset of next partition
			next_offset=${FLASHLAYOUT_data[$((i+1)),$COL_OFFSET]}
			next_offset=${next_offset//0x/}
			next_offset_b=$((16#$next_offset))
			if [ "$partName" == "rootfs" ];
			then
				#force the size of rootfs parition to 768MB
				new_next_partition_offset_b=$((offset_b + 1024*DEFAULT_ROOTFS_PARTITION_SIZE))
				next_offset_b=$new_next_partition_offset_b

				move_partition_offset $((i+1)) $new_next_partition_offset_b
				index_of_rootfs=$i
			fi

			if [ $i -gt $index_of_rootfs ];
			then
				if [ $((next_offset_b + image_size)) -gt $((DEFAULT_RAW_SIZE * 1024*1024)) ]
				then
					echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
					echo "[ERROR]: The rootfs and/or other partitions doesn't enter on a SDCARD size of $DEFAULT_RAW_SIZE MB"
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

		if [ $i -ne $((FLASHLAYOUT_number_of_line -1)) ];
		then
			free_size=$((partition_size - image_size))
		else
			free_size=0
			partition_size=0
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
			debug "   DUMP partition size $partition_size"
			debug "   DUMP free size      $free_size "
			if [ "$selected" == "P" ] || [ "$selected" == "E" ] || [ "$selected" == "PD" ] || [ "$selected" == "DP" ] || [ "$selected" == "PED" ];
			then
				if [ $free_size -lt 0 ];
				then
					if [ "$partName" == "rootfs" ];
					then
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
					else
						echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
						echo "[ERROR]: IMAGE TOO BIG [$partName:$bin2flash $image_size_in_mb MB [requested $partition_size B]"
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
				*)
					# Linux File system: 0x8300
					gpt_code="8300"
					;;
				esac

				printf "part %d: %8s ..." $j "$partName"
				exec_print "sgdisk -a 1 -n $j:$offset:$next_offset -c $j:$partName -t $j:$gpt_code $extrafs_param $FLASHLAYOUT_rawname"
				partition_size=$(sgdisk -p "$FLASHLAYOUT_rawname" | grep $partName | grep -v "\-$partName" | awk '{ print $4}')
				partition_size_type=$(sgdisk -p "$FLASHLAYOUT_rawname" | grep $partName | grep -v "\-$partName" | awk '{ print $5}')
				printf "\r[CREATED] part %02d: %10s [partition size %s %s]\n" $j "$partName"  "$partition_size" "$partition_size_type"

			j=$((j+1))
			fi
		p=$((p+1))
		fi
	done

	echo ""
	echo "Partition table from $FLASHLAYOUT_rawname"
	exec_display_print "sgdisk -p $FLASHLAYOUT_rawname"
	for info in $display_info;
	do
		echo ""
		exec_display_print "sgdisk $FLASHLAYOUT_rawname -i $info"
	done
	echo ""
}

function generate_empty_raw_image() {
	# Initialize image file (due to bs we force seek on K)
	echo "Create Raw empty image: $FLASHLAYOUT_rawname of ${DEFAULT_RAW_SIZE}MB"
	exec_print "dd if=/dev/zero of=$FLASHLAYOUT_rawname bs=1024 count=0 seek=${DEFAULT_RAW_SIZE}K"
}

function populate_gpt_partition_table_from_flash_layout() {
	local i=1
	local j=1
	echo "Populate raw image with image content:"

	for ((i=0;i<FLASHLAYOUT_number_of_line;i++))
	do
		#debug "DUMP LINE=${FLASHLAYOUT_data[$i]}"
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		partId=${FLASHLAYOUT_data[$i,$COL_PARTID]}
		partName=${FLASHLAYOUT_data[$i,$COL_PARTNAME]}
		partType=${FLASHLAYOUT_data[$i,$COL_PARTYPE]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		offset=${FLASHLAYOUT_data[$i,$COL_OFFSET]}
		bin2flash=${FLASHLAYOUT_data[$i,$COL_BIN2FLASH]}

		offset=${offset//0x/}
		offset=$((16#$offset))

		debug "   DUMP $selected $partId $partName $partType"
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			#debug "   DUMP selected  $selected"
			#debug "   DUMP partId    $partId"
			#debug "   DUMP partName  $partName"
			#debug "   DUMP partType  $partType"
			#debug "   DUMP ip        $ip"
			#debug "   DUMP offset    $offset "
			#debug "   DUMP bin2flash $bin2flash"
			if [ "$selected" == "P" ] || [ "$selected" == "PD" ] || [ "$selected" == "DP" ]|| [ "$selected" == "PED" ];
			then
				# Populate only the partition in "P"
				if [ -e "$FLASHLAYOUT_prefix_image_path/$bin2flash" ];
				then
					printf "part %02d: %10s, image: %s ..." $j "$partName" "$bin2flash"
					exec_print "dd if=$FLASHLAYOUT_prefix_image_path/$bin2flash of=$FLASHLAYOUT_rawname conv=fdatasync,notrunc seek=1 bs=$offset"
					printf "\r[ FILLED ] part %02d: %10s, image: %s \n" $j "$partName" "$bin2flash"
				else
					if [ ! "$(basename $FLASHLAYOUT_prefix_image_path/$bin2flash)" = "none" ];
					then
						printf "\r[UNFILLED] part %02d: %10s, image: %s (not present) \n" $j "$partName" "$bin2flash"
						echo "   [WARNING]: THE FILE $FLASHLAYOUT_prefix_image_path/$bin2flash ARE NOT PRESENT."
						echo "   [WARNING]: THE PARTITION $partName ARE NOT FILL."
						WARNING_TEXT+="[WARNING]: THE PARTITION $partName ARE NOT FILL (file $FLASHLAYOUT_prefix_image_path/$bin2flash are not present) #"
					fi
				fi
				j=$((j+1))
			else
				if [ "$selected" == "E" ];
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
function print_shema_on_infofile() {
	local j=1
	local i=1
	# print schema of partition
	i=1
	for ((i=0;i<FLASHLAYOUT_number_of_line;i++))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if [ "$selected" == "P" ] || [ "$selected" == "E" ] || [ "$selected" == "PD" ] || [ "$selected" == "DP" ] || [ "$selected" == "PED" ];
			then
				echo -n "=============" >> "$FLASHLAYOUT_infoname"
			fi
		fi
	done
	echo "=" >> "$FLASHLAYOUT_infoname"

	#empty line
	for ((i=0;i<FLASHLAYOUT_number_of_line;i++))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if [ "$selected" == "P" ] || [ "$selected" == "E" ] || [ "$selected" == "PD" ] || [ "$selected" == "DP" ] || [ "$selected" == "PED" ];
			then
				echo -n "=            " >> "$FLASHLAYOUT_infoname"
			fi
		fi
	done
	echo "=" >> "$FLASHLAYOUT_infoname"
	# part name
	for ((i=0;i<FLASHLAYOUT_number_of_line;i++))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		partName=${FLASHLAYOUT_data[$i,$COL_PARTNAME]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if [ "$selected" == "P" ] || [ "$selected" == "E" ] || [ "$selected" == "PD" ] || [ "$selected" == "DP" ] || [ "$selected" == "PED" ];
			then
				printf "=  %08s  " "$partName" >> "$FLASHLAYOUT_infoname"
			fi
		fi
	done
	echo "=" >> "$FLASHLAYOUT_infoname"
	#empty
	for ((i=0;i<FLASHLAYOUT_number_of_line;i++))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if [ "$selected" == "P" ] || [ "$selected" == "E" ] || [ "$selected" == "PD" ] || [ "$selected" == "DP" ] || [ "$selected" == "PED" ];
			then
				echo -n "=            " >> "$FLASHLAYOUT_infoname"
			fi
		fi
	done
	echo "=" >> "$FLASHLAYOUT_infoname"
	# partition number
	for ((i=0;i<FLASHLAYOUT_number_of_line;i++))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if [ "$selected" == "P" ] || [ "$selected" == "E" ] || [ "$selected" == "PD" ] || [ "$selected" == "DP" ] || [ "$selected" == "PED" ];
			then
				printf "= %08s%-2d " "mmcblk0p" $j>> "$FLASHLAYOUT_infoname"
				j=$((j+1))
			fi
		fi
	done
	echo "=" >> "$FLASHLAYOUT_infoname"
	j=1
	for ((i=0;i<FLASHLAYOUT_number_of_line;i++))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if [ "$selected" == "P" ] || [ "$selected" == "E" ] || [ "$selected" == "PD" ] || [ "$selected" == "DP" ] || [ "$selected" == "PED" ];
			then
				printf "=    (%-2d)    " $j>> "$FLASHLAYOUT_infoname"
				j=$((j+1))
			fi
		fi
	done
	echo "=" >> "$FLASHLAYOUT_infoname"
	for ((i=0;i<FLASHLAYOUT_number_of_line;i++))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if [ "$selected" == "P" ] || [ "$selected" == "E" ] || [ "$selected" == "PD" ] || [ "$selected" == "DP" ] || [ "$selected" == "PED" ];
			then
				echo -n "=            " >> "$FLASHLAYOUT_infoname"
			fi
		fi
	done
	echo "=" >> "$FLASHLAYOUT_infoname"
	for ((i=0;i<FLASHLAYOUT_number_of_line;i++))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if [ "$selected" == "P" ] || [ "$selected" == "E" ] || [ "$selected" == "PD" ] || [ "$selected" == "DP" ] || [ "$selected" == "PED" ];
			then
				echo -n "=============" >> "$FLASHLAYOUT_infoname"
			fi
		fi
	done
	echo "=" >> "$FLASHLAYOUT_infoname"
	# print legend of partition
	j=1
	for ((i=0;i<FLASHLAYOUT_number_of_line;i++))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		partName=${FLASHLAYOUT_data[$i,$COL_PARTNAME]}
		bin2flash=${FLASHLAYOUT_data[$i,$COL_BIN2FLASH]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if [ "$selected" == "P" ] || [ "$selected" == "E" ] || [ "$selected" == "PD" ] || [ "$selected" == "DP" ] || [ "$selected" == "PED" ];
			then
				{
					echo "($j):"
					echo "    Device: /dev/mmcblk0p$j"
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
	for ((i=0;i<FLASHLAYOUT_number_of_line;i++))
	do
		selected=${FLASHLAYOUT_data[$i,$COL_SELECTED_OPT]}
		ip=${FLASHLAYOUT_data[$i,$COL_IP]}
		partName=${FLASHLAYOUT_data[$i,$COL_PARTNAME]}
		bin2flash=${FLASHLAYOUT_data[$i,$COL_BIN2FLASH]}
		if [ "$ip" == "$SDCARD_TOKEN" ];
		then
			if [ "$selected" == "P" ] || [ "$selected" == "E" ] || [ "$selected" == "PD" ] || [ "$selected" == "DP" ] || [ "$selected" == "PED" ];
			then
				if [ "$selected" == "E" ];
				then
					echo "- Populate partition $partName (/dev/mmcblk0p$j)" >> "$FLASHLAYOUT_infoname"
					if [ -n "$bin2flash" ];
					then
						echo "    dd if=$bin2flash of=/dev/mmcblk0p$j bs=1M conv=fdatasync status=progress" >> "$FLASHLAYOUT_infoname"
					else
						echo "    dd if=<raw image of $partName> of=/dev/mmcblk0p$j bs=1M conv=fdatasync status=progress" >> "$FLASHLAYOUT_infoname"
					fi
				else
					echo "- Populate partition $partName (/dev/mmcblk0p$j)" >> "$FLASHLAYOUT_infoname"
					echo "    dd if=$bin2flash of=/dev/mmcblk0p$j bs=1M conv=fdatasync status=progress" >> "$FLASHLAYOUT_infoname"
				fi
				echo "" >> "$FLASHLAYOUT_infoname"
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
print_shema_on_infofile

cat >> "$FLASHLAYOUT_infoname"  << EOF

2. How to populate each partition
---------------------------------
EOF

print_populate_on_infofile

cat >> "$FLASHLAYOUT_infoname"  << EOF

3. How to update the kernel/devicetree
--------------------------------------
The kernel and devicetree are present on "boot" partition.
To change kernel and devicetree, you can copy the file on this partitions:
- plug SDCARD on your PC
- copy kernel uImage on SDCARD
   sudo cp uImage /media/\$USER/bootfs/
- copy devicetree uImage on SDCARD
   sudo cp stm32mp1*.dtb /media/\$USER/bootfs/
- umount partitions of SDCARD
   sudo umount /media/\$USER/bootfs/
   (dont't forget to umount the other partitions of SDCARD:
   sudo umount \`lsblk --list | grep mmcblk0 | grep part | gawk '{ print \$7 }' | tr '\\n' ' '\`
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
	echo "RAW IMAGE generated: $FLASHLAYOUT_rawname"
	echo ""
	echo "WARNING: before to use the command dd, please umount all the partitions"
	echo "	associated to SDCARD."
	echo "    sudo umount \`lsblk --list | grep mmcblk0 | grep part | gawk '{ print \$7 }' | tr '\\n' ' '\`"
	echo ""
	echo "To put this raw image on sdcard:"
	echo "    sudo dd if=$FLASHLAYOUT_rawname of=/dev/mmcblk0 bs=8M conv=fdatasync status=progress"
	echo ""
	echo "(mmcblk0 can be replaced by:"
	echo "     sdX if it's a device dedicated to receive the raw image "
	echo "          (where X can be a, b, c, d, e)"
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
	echo "   $0 <FlashLayout file>"
	echo ""
	exit 1
}
# ------------------
#        Main
# ------------------
# check opt args
if [ $# -ne 1 ];
then
	echo "[ERROR]: bad number of parameters"
	echo ""
	usage
else
	tools_check

	FLASHLAYOUT_filename=$1
	FLASHLAYOUT_filename_path=$(dirname "$FLASHLAYOUT_filename")
	FLASHLAYOUT_filename_name=$(basename "$FLASHLAYOUT_filename")
	FLASHLAYOUT_dirname=$(basename "$FLASHLAYOUT_filename_path")

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
	if echo "$FLASHLAYOUT_dirname" | grep -q flashlayout
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

	#put the raw image generate near the binaries images
	FLASHLAYOUT_rawname=$FLASHLAYOUT_prefix_image_path/$FLASHLAYOUT_rawname
	FLASHLAYOUT_infoname=$FLASHLAYOUT_prefix_image_path/$FLASHLAYOUT_infoname

	# erase previous raw image
	if [ -f "$FLASHLAYOUT_rawname" ];
	then
		echo ""
		echo "[WARNING]: A previous raw image are present on this directory"
		echo "[WARNING]:    $FLASHLAYOUT_rawname"
		echo "[WARNING]: would you like to erase it: [Y/n]"
		read -r answer
		if [[ "$answer" =~ ^[Yy]+[ESes]* ]]; then
			rm -f "$FLASHLAYOUT_rawname" "$FLASHLAYOUT_infoname"
		fi
	fi

	debug "DUMP FlashLayout name:      $FLASHLAYOUT_filename"
	debug "DUMP FlashLayout dir path:  $FLASHLAYOUT_filename_path"
	debug "DUMP images dir path:       $FLASHLAYOUT_prefix_image_path"
	debug "DUMP RAW SDCARD image name: $FLASHLAYOUT_rawname"
fi

generate_empty_raw_image
generate_gpt_partition_table_from_flash_layout ""
populate_gpt_partition_table_from_flash_layout ""

create_info
print_info
print_warning
