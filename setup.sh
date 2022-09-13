#!/bin/bash
##############################################################################
#                                                                            #
# Copyright 2020 Lukas Jünger                                                #
#                                                                            #
# This software is licensed under the MIT license.                           #
# A copy of the license can be found in the LICENSE file at the root         #
# of the source tree.                                                        #
#                                                                            #
#############################################################################

set -e
# Get directory of script itself
SOURCE="${BASH_SOURCE[0]}"
# resolve $SOURCE until the file is no longer a symlink
while [ -h "$SOURCE" ]; do
        DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"
        SOURCE="$(readlink "$SOURCE")"
        # if $SOURCE was a relative symlink, we need to resolve it relative to the
        # path where the symlink file was located
        [[ $SOURCE != /* ]] && SOURCE="$DIR/$SOURCE"
done
DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"


# If AVP64_SW is set, overwrite files with local version
if [ -z "$AVP64_SW" ]; then
    echo "Error, specify AVP64_SW environment variable"
    exit 0
fi

BUILD_DEBUG="$DIR/build/debug"
BUILD_RELEASE="$DIR/build/release"

rm -rf "$BUILD_DEBUG/sw"
rm -rf "$BUILD_RELEASE/sw"

AVP64_LINUX_BUILD="$AVP64_SW/linux/BUILD"
AVP64_LINUX_KERNEL_IMAGE="$AVP64_LINUX_BUILD/buildroot/output/linux/images/Image"
AVP64_LINUX_KERNEL_ELF="$AVP64_LINUX_BUILD/buildroot/output/linux/build/linux-4.19.4/vmlinux"
AVP64_LINUX_DTB="$AVP64_LINUX_BUILD/buildroot/output/linux/images/avp64_linux_tutorial.dtb"
AVP64_LINUX_SDCARD="$AVP64_LINUX_BUILD/buildroot/output/linux/images/sdcard.img"
AVP64_LINUX_BOOT="$AVP64_LINUX_BUILD/linux_bootcode/el3/boot.bin"

AVP64_TUTORIAL_DEST_DIR_DEBUG="$BUILD_DEBUG/sw/arm64_tutorial/linux"
AVP64_TUTORIAL_DEST_DIR_RELEASE="$BUILD_RELEASE/sw/arm64_tutorial/linux"
mkdir -p $AVP64_TUTORIAL_DEST_DIR_DEBUG
mkdir -p $AVP64_TUTORIAL_DEST_DIR_RELEASE
cp -rv "$AVP64_LINUX_KERNEL_IMAGE" "$AVP64_TUTORIAL_DEST_DIR_DEBUG/Image-4.19.4"
ln -s  "$AVP64_TUTORIAL_DEST_DIR_DEBUG/Image-4.19.4" "$AVP64_TUTORIAL_DEST_DIR_RELEASE/Image-4.19.4"
cp -rv "$AVP64_LINUX_KERNEL_ELF" "$AVP64_TUTORIAL_DEST_DIR_DEBUG/vmlinux-4.19.4"
ln -s  "$AVP64_TUTORIAL_DEST_DIR_DEBUG/vmlinux-4.19.4" "$AVP64_TUTORIAL_DEST_DIR_RELEASE/vmlinux-4.19.4"
cp -rv "$AVP64_LINUX_DTB" "$AVP64_TUTORIAL_DEST_DIR_DEBUG/linux.dtb"
ln -s  "$AVP64_TUTORIAL_DEST_DIR_DEBUG/linux.dtb" "$AVP64_TUTORIAL_DEST_DIR_RELEASE/linux.dtb"
cp -rv "$AVP64_LINUX_SDCARD" "$AVP64_TUTORIAL_DEST_DIR_DEBUG/sdcard.img"
chmod -w "$AVP64_TUTORIAL_DEST_DIR_DEBUG/sdcard.img"
ln -s  "$AVP64_TUTORIAL_DEST_DIR_DEBUG/sdcard.img" "$AVP64_TUTORIAL_DEST_DIR_RELEASE/sdcard.img"
cp -rv "$AVP64_LINUX_BOOT" "$AVP64_TUTORIAL_DEST_DIR_DEBUG/boot.bin"
ln -s  "$AVP64_TUTORIAL_DEST_DIR_DEBUG/boot.bin" "$AVP64_TUTORIAL_DEST_DIR_RELEASE/boot.bin"


VM_LINUX=$AVP64_TUTORIAL_DEST_DIR_DEBUG/vmlinux-4.19.4
LINUX_BUILD_DIR=$AVP64_LINUX_BUILD/buildroot/output/linux/build/linux-4.19.4

sed 's@$VM_LINUX@'"$VM_LINUX"'@g; s@$LINUX_BUILD_DIR@'"$LINUX_BUILD_DIR"'@g' $DIR/gdb_script.in > $DIR/gdb_script
