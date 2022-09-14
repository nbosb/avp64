#!/bin/bash
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
 
SW_DIR=/home/tutorial/tutorial/sw
LINUX_DIR=/home/tutorial/tutorial/linux-4.19.4
VM_LINUX=$SW_DIR/arm64_tutorial/linux/vmlinux-4.19.4
BUILD_DEBUG="$DIR/build/debug"
BUILD_RELEASE="$DIR/build/release"
 
rm -rf "$BUILD_DEBUG/sw"
rm -rf "$BUILD_RELEASE/sw"
 
mkdir -p "$BUILD_DEBUG"
mkdir -p "$BUILD_RELEASE"
 
AVP64_TUTORIAL_DEST_DIR_DEBUG="$BUILD_DEBUG/sw/"
 
ln -s $SW_DIR "$BUILD_DEBUG/"
ln -s $SW_DIR "$BUILD_RELEASE/"

sed 's@$VM_LINUX@'"$VM_LINUX"'@g; s@$LINUX_BUILD_DIR@'"$LINUX_DIR"'@g' $DIR/gdb_script.in > $DIR/gdb_script
