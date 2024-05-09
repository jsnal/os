# Toolchain

1. Clone GCC and binutils into this directory and run `build.sh`

```
$ git clone git@github.com:jsnal/gcc.git
$ git clone git@github.com:jsnal/binutils-gdb.git
```
2. Install dependencies

```
apt install bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo libisl-dev
```

3. Build

```
$ ./build.sh
```
