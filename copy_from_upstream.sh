#!/bin/bash


# This little script copies modified code from an "upstream" folder
# if the file is different. Files in the "ignore" list will be skipped.
# Argument #1 is the path to the root of the upstream folder, so you
# use this script inside the folder of the "new" xrtm as:

# ./copy_from_upstream.sh PATH_TO_UPSTREAM_XRTM

UPSTREAM_DIR=$1


# Skip list
skip_list=("./copy_from_upstream.sh" "./make.inc.example" "./interfaces/Makefile")


for fname in `find . -type f -not -path "./.git/*"`; do

    skip_found=0
    # Check if this file is in the skip list
    for element in ${skip_list[@]}; do
        if [[ ${element} == ${fname} ]]; then
            skip_found=1
            break
        fi
    done

    if [[ ${skip_found} -eq 1 ]]; then
        echo "${fname} is in skip list! Skipping."
        continue
    fi

    # Check if file exists in upstream path. If not, continue
    if [ -e ${UPSTREAM_DIR}/${fname} ]; then
        true
        # File exists, keep going
    else
        echo "File ${fname} does not exist! Skipping check."
        continue
    fi

    # Check if file is different
    if diff -q ${fname} ${UPSTREAM_DIR}/${fname} >/dev/null 2>&1; then
        # Files are the same
        continue
    else
        echo "File ${fname} differs!"
    fi

    # Copy file over!
    cp ${UPSTREAM_DIR}/${fname} ${fname}

done
