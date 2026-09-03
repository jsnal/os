#!/usr/bin/env sh

DIR=$(dirname $(cd -P -- "$(dirname -- "$0")" && pwd -P))

die() {
  echo "die: $*"
  exit 1
}

if [ "$(id -u)" != 0 ]; then
  exec sudo -E -- "$0" "$@" || die "this script needs to run as root"
else
  : "${SUDO_UID:=0}" "${SUDO_GID:=0}"
fi

# echo $PATH

GRUB_BIN=$(command -v grub-install 2>/dev/null) || true
[ -z "$GRUB_BIN" ] && GRUB_BIN=$(command -v grub2-install 2>/dev/null) || true
[ -z "$GRUB_BIN" ] && die "grub-install or grub2-install binary not found"
echo "using grub-install at ${GRUB_BIN}"

HEADS=16
SECTORS=63
BYTES_PER_SECTOR=512
DISK_SIZE_MB=32

BYTES=$(($HEADS*$SECTORS*$BYTES_PER_SECTOR))
CYLINDERS=$((($DISK_SIZE_MB*1000*1024)/$BYTES))

echo "setting up disk image..."
dd if=/dev/zero of=kernel.img bs=${BYTES}c count="${CYLINDERS:-50}" status=none || die "unable to create disk image"
chown "$SUDO_UID":"$SUDO_GID" kernel.img || die "unable to adjust permissions on disk image"
echo "done"

echo "creating loopback device..."
LOOPBACK=$(losetup --find --partscan --show kernel.img)
[ -z "$LOOPBACK" ] && die "unable to mount loopback device"
echo "loopback device is at ${LOOPBACK}"
echo "done"

cleanup() {
  if [ -d mnt ]; then
    echo "unmounting filesystem..."
    umount mnt || ( sleep 1 && sync && umount mnt )
    rm -rf mnt
    echo "done"
  fi

  if [ -e "${LOOPBACK}" ]; then
    echo "cleaning up loopback device..."
    losetup -d ${LOOPBACK}
    echo "done"
  fi
}
trap cleanup EXIT

echo "creating partition table..."
parted -s "${LOOPBACK}" mklabel msdos mkpart primary ext2 32k 100% -a minimal set 1 boot on || die "unable to partition disk"
echo "done"

echo "removing old filesystem... "
dd if=/dev/zero of="${LOOPBACK}p1" bs=1M count=1 status=none || die "unable to destroy old filesystem"
echo "done"

echo "creating new filesystem... "
mke2fs -q -I 128 -b 1024 "${LOOPBACK}p1" 16M || die "unable to create filesystem"
echo "done"

echo "mounting filesystem... "
mkdir -p mnt
mount "${LOOPBACK}p1" mnt/ || die "unable to mount filesystem"
echo "done"

echo "copying kernel... "
mkdir -p mnt/boot
sudo cp -v "$DIR/kernel.bin" mnt/boot/kernel
echo "done"

# TODO: add this back
# $DIR/Meta/build_fs.sh

echo "installing grub..."
$GRUB_BIN --boot-directory=mnt/boot --target=i386-pc --modules="ext2 part_msdos" "${LOOPBACK}"
cat > mnt/boot/grub/grub.cfg << 'EOF'
timeout=0
menuentry 'OS' {
  multiboot /boot/kernel
  boot
}
EOF
echo "done"
