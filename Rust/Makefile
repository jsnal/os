SOURCE:=$(dir $(realpath $(lastword $(MAKEFILE_LIST))))
BUILD?=$(CURDIR)

LDFILE=$(SOURCE)/linker/i686.ld
LOCKFILE=$(SOURCE)/Cargo.lock
MANIFEST=$(SOURCE)/Cargo.toml
TARGET_SPEC=$(SOURCE)/i686-unknown-kernel.json

all: $(BUILD)/kernel.bin $(BUILD)/kernel.sym

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
		-Clink-arg=-T -Clink-arg="$(LDFILE)" \
		-C link-arg=-z -Clink-arg=max-page-size=0x1000 \
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
