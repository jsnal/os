# OS

32-bit x86 operating system completely from scratch. The entire codebase is
written by me in C++ and Assembly, with the only exception being the bootloader.
The project serves as a practical exploration of the many algorithms,
mechanisms, protocols, and standards used to build a fully functioning operating
system.

## Features

* 32-bit kernel with pre-emptive multitasking
* Full-fledged userland with system calls and memory protection
* POSIX compatibility with libc, system calls, shell
* Virtual file system and ext2 file system
* TTY with escape sequences and 16-bit color
* Layer 2 and 3 networking
* Driver support for
  * E1000 network interface card
  * PATA disks
  * PS/2 keyboard
  * Generic PCI devices

## Running

```
./Meta/run.sh
```

## Grub

```
# sudo apt install grub-pc
```
