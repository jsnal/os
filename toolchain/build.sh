#!/usr/bin/env bash

set -eo pipefail

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

ARCH="i686"
PREFIX="$DIR/local"
TARGET="$ARCH-elf"
PATH="$PREFIX/bin:$PATH"

BINUTILS_VERSION="2.37"
BINUTILS_NAME="binutils-$BINUTILS_VERSION"
BINUTILS_PKG="${BINUTILS_NAME}.tar.gz"
BINUTILS_BASE_URL="https://ftp.gnu.org/gnu/binutils"

GCC_VERSION="10.2.0"
GCC_NAME="gcc-$GCC_VERSION"
GCC_PKG="${GCC_NAME}.tar.gz"
GCC_BASE_URL="https://ftp.gnu.org/gnu/gcc"

if [ ! -d ${BINUTILS_NAME} ]; then
  mkdir $BINUTILS_NAME
  pushd $BINUTILS_NAME
    echo "Downloading binutils"
    curl -LO "$BINUTILS_BASE_URL/$BINUTILS_PKG"

    echo "Extrating binutils"
    # if [ -d ${BINUTILS_NAME} ]; then
    #   rm -rf "${BINUTILS_NAME}"
    #   rm -rf "$DIR/local/$BINUTILS_NAME"
    # fi
    tar -xzf ${BINUTILS_PKG}
    ${BINUTILS_NAME}/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
    make -j
    make install
  popd
fi

if [ ! -d ${GCC_NAME} ]; then
  mkdir $GCC_NAME
  pushd $GCC_NAME
    which -- $TARGET-as || echo $TARGET-as is not in the PATH
    echo "Downloading gcc"
    curl -LO "$GCC_BASE_URL/$GCC_NAME/$GCC_PKG"

    echo "Extrating gcc"
    tar -xzf ${GCC_PKG}
    ${GCC_NAME}/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c --without-headers
    make all-gcc
    make all-target-libgcc
    make install-gcc
    make install-target-libgcc
  popd
fi
