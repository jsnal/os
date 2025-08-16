# Toolchain

Contains the build scripts and patches to build the toolchain.

## Building

1. Install dependencies

```
apt install autoconf automake bison flex libgmp3-dev libmpc-dev libmpfr-dev texinfo libisl-dev
```

> autoconf 2.69 is required and may need to be built from source

3. Build

```
$ ./build_tools.sh
```

## Generating Patches

1. Download the tarball of the project

```
curl -LO https://xyz/<NAME>.tar.gz
```
2. Untar and create two copies

```
tar zxf <NAME>.tar.gz
cp -r <NAME> <NAME>-clean
```

3. Make necessary changes to the copy that is not marked '-clean'
4. Generate the patch

```
diff -uraN <NAME>-clean <NAME> > <NAME>.patch
```
