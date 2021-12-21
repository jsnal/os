#!/bin/sh

PATH="$PATH:/sbin"

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

GRUB_BIN=$(command -v grub-install 2>/dev/null) || true
if [ -z "$GRUB_BIN" ]; then
    GRUB_BIN=$(command -v grub2-install 2>/dev/null) || true
fi
if [ -z "$GRUB_BIN" ]; then
    echo "can't find a grub-install or grub2-install binary, oh no"
    exit 1
fi
echo "using grub-install at ${GRUB_BIN}"

DISK_SIZE=$(($(du -sm "$DIR/kernel/kernel.bin" | cut -f1) + 800))

echo "setting up disk image..."
dd if=/dev/zero of=os_grub.img bs=1M count="${DISK_SIZE:-800}" status=none || die "unable to create disk image"
chown "$SUDO_UID":"$SUDO_GID" os_grub.img || die "couldn't adjust permissions on disk image"
echo "done"

echo "creating loopback device..."
LOOPBACK=$(losetup --find --partscan --show os_grub.img)
if [ -z "$LOOPBACK" ]; then
    die "couldn't mount loopback device"
fi
echo "loopback device is at ${LOOPBACK}"
echo "done"

# cleanup() {
#   if [ -d mnt ]; then
#     echo "unmounting filesystem..."
#     umount mnt || ( sleep 1 && sync && umount mnt )
#     rm -rf mnt
#     echo "done"
#   fi
#
#   if [ -e "${LOOPBACK}" ]; then
#     echo "cleaning up loopback device..."
#     losetup -d ${LOOPBACK}
#     echo "done"
#   fi
# }
# trap cleanup EXIT

echo "creating partition table..."
parted -s "${LOOPBACK}" mklabel gpt mkpart BIOSBOOT ext3 1MiB 8MiB mkpart OS ext2 8MiB 290MiB set 1 bios_grub || die "couldn't partition disk"
echo "done"

echo "removing old filesystem... "
dd if=/dev/zero of="${LOOPBACK}p1" bs=1M count=1 status=none || die "couldn't destroy old filesystem"
echo "done"

echo "creating new filesystem... "
mke2fs -q -I 128 "${LOOPBACK}p1" || die "couldn't create filesystem"
echo "done"

echo "mounting filesystem... "
mkdir -p mnt
mount "${LOOPBACK}p1" mnt/ || die "couldn't mount filesystem"
mkdir -p mnt/boot
echo "done"

echo "building filesystem..."
sudo cp -v "$DIR/kernel/kernel.bin" mnt/boot/kernel
echo "done"

echo "installing grub..."
$GRUB_BIN --boot-directory=mnt/boot --target=i386-pc --modules="ext2 part_msdos" "${LOOPBACK}"
cp -v "$DIR/meta/grub.cfg" mnt/boot/grub/grub.cfg
echo "done"
