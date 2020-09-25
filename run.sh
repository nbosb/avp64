#!/bin/bash

CONFIG=arm64_linux.cfg

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

export LD_LIBRARY_PATH=$DIR/BUILD/DEBUG/lib
$DIR/BUILD/DEBUG/bin/avp64 -f $DIR/BUILD/DEBUG/config/$CONFIG --log --log-debug
