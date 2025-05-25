#!/bin/sh

DIR=$(dirname $(cd -P -- "$(dirname -- "$0")" && pwd -P))
TOOLCHAIN_BIN="$DIR/Toolchain/local/bin"
export PATH="$TOOLCHAIN_BIN:$PATH"
