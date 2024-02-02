#!/bin/sh

DIR=$(dirname $(cd -P -- "$(dirname -- "$0")" && pwd -P))
TOOLCHAIN_BIN="$DIR/toolchain/local/bin"

echo "setting path $TOOLCHAIN_BIN"
export PATH="$TOOLCHAIN_BIN:$PATH"
