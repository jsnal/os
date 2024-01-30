#!/usr/bin/env bash

set -eo pipefail

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

ARCH="i686"
PREFIX="$DIR/local"
TARGET="$ARCH-pc-os"
PATH="$PREFIX/bin:$PATH"

BINUTILS_VERSION="2.32"
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
    tar -xzf ${BINUTILS_PKG}

    echo "Patching binutils"
    pushd $BINUTILS_NAME
      patch -p1 < ../../binutils-2.32.patch
      pushd ld
        autoreconf
        automake
      popd
    popd

    echo "Building binutils"
    ${BINUTILS_NAME}/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror
    make all -j4
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

    echo "Patching gcc"
    pushd $GCC_NAME
      patch -p1 < ../../gcc-10.2.0.patch

      echo "Downloading dependencies"
      ./contrib/download_prerequisites
    popd

    echo "Building gcc"
    ${GCC_NAME}/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c --without-headers
    make all-gcc -j4
    make all-target-libgcc -j4
    make install-gcc -j4
    make install-target-libgcc -j4
  popd
fi
