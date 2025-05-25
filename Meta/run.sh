#!/bin/sh

DIR=$(dirname $(cd -P -- "$(dirname -- "$0")" && pwd -P))
EMULATOR="$1"
NETWORK_ARGUMENTS="-netdev user,id=net0 -device e1000,netdev=net0"
EXTRA_ARGUMENTS=""

die() {
  echo "die: $*"
  exit 1
}

usage() {
    echo "Usage: run <EMULATOR> [OPTION]..."
    echo ""
    echo "Emulator types:"
    echo "  bochs                      Use the Bochs emulator"
    echo "  qemu                       Use the QEMU emulator"
    echo ""
    echo "Options:"
    echo "  --monitor, -m              Enable monitor console. QEMU only"
    echo "  --gdb, -g                  Enable remote gdb. QEMU only"
    echo "  --tap, -t                  Enable networking tap device. QEMU only"
    echo "  --help, -h                 Display this help page"
}

if [ "$(id -u)" != 0 ]; then
  exec sudo -E -- "$0" "$@" || die "this script needs to run as root"
else
  : "${SUDO_UID:=0}" "${SUDO_GID:=0}"
fi

. "$DIR/Meta/env.sh"

eval set -- "$(getopt -o mgth -l monitor,gdb,tap,help --name "$0" -- "$@")"

while true; do
    case "$1" in
        -m|--monitor)
            EXTRA_ARGUMENTS="$EXTRA_ARGUMENTS -monitor tcp:127.0.0.1:55555,server,nowait"
            echo 'enabling qemu monitor...'
            ;;
        -g|--gdb)
            EXTRA_ARGUMENTS="$EXTRA_ARGUMENTS -s -S"
            echo 'enabling gdb...'
            ;;
        -t|--tap)
            NETWORK_ARGUMENTS="-netdev tap,ifname=tap0,id=br0,script=no,downscript=no -device e1000,netdev=br0"
            echo 'enabling network tap...'
            ;;
        -h|--help)
            usage
            exit 0
            ;;
        --)
            shift;
            break
            ;;
    esac

    shift
done

if [ "$EMULATOR" = "bochs" ]; then
    bochs -f $DIR/Meta/bochsrc.txt -q || die "unable to run bochs"
elif [ "$EMULATOR" = "qemu" ]; then
    qemu-system-i386 \
        $EXTRA_ARGUMENTS \
        $NETWORK_ARGUMENTS \
        -rtc base=utc \
        -m 128M \
        -serial stdio \
        -d cpu_reset,guest_errors \
        -drive file=$DIR/Kernel.img,format=raw,index=0,media=disk \
        || die "unable to run qemu"
else
    usage
    die "'$1' is not a recongized emulator"
fi
