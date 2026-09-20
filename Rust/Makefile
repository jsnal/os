SOURCE:=$(dir $(realpath $(lastword $(MAKEFILE_LIST))))
BUILD?=$(CURDIR)

LDFILE=$(SOURCE)/linker/i686.ld
LOCKFILE=$(SOURCE)/Cargo.lock
MANIFEST=$(SOURCE)/Cargo.toml
TARGET_SPEC=$(SOURCE)/i686-unknown-kernel.json

QEMU=/mnt/c/Program\ Files/qemu/qemu-system-i386.exe
IMGFILE:=$(shell wslpath -aw kernel.img)

all: $(BUILD)/kernel.bin $(BUILD)/kernel.sym

run: all
	$(BUILD)/scripts/build_img.sh
	$(QEMU) \
		-rtc base=utc \
		-m 128M \
		-debugcon stdio \
		-monitor telnet:0.0.0.0:4444,server,nowait \
		-d cpu_reset,guest_errors \
		-drive file='$(IMGFILE)',format=raw,index=0,media=disk

test:
	cargo test --lib --manifest-path "$(MANIFEST)"

$(BUILD)/kernel.all: $(LDFILE) $(LOCKFILE) $(MANIFEST) $(TARGET_SPEC) $(shell find $(SOURCE) -name "*.rs" -or -name "*.s" -type f)
	cargo rustc \
		-vv \
		--bin kernel \
		--manifest-path "$(MANIFEST)" \
		--target "$(TARGET_SPEC)" \
		--release \
		-Zjson-target-spec \
		-Zbuild-std=core,alloc \
		-Zbuild-std-features=compiler-builtins-mem \
		-- \
		-Clink-arg=-T \
		-Clink-arg="$(LDFILE)" \
		-Clink-arg=-z \
		-Clink-arg=max-page-size=0x1000 \
		--emit link="$(BUILD)/kernel.all"

$(BUILD)/kernel.sym: $(BUILD)/kernel.all
	objcopy \
		--only-keep-debug \
		"$(BUILD)/kernel.all" \
		"$(BUILD)/kernel.sym"

$(BUILD)/kernel.bin: $(BUILD)/kernel.all
	objcopy \
		--strip-debug \
		"$(BUILD)/kernel.all" \
		"$(BUILD)/kernel.bin"
