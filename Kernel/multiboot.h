/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _MULTIBOOT_H_
#define _MULTIBOOT_H_

#include <stdint.h>

#define MULTIBOOT_MEMORY_AVAILABLE 1
#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
#define MULTIBOOT_BOOTLOADER_MAGIC 0x2BADB002
#define MULTIBOOT_FLAGS_BOOTLOADER_NAME (1 << 9)
#define MULTIBOOT_FLAGS_MMAP (1 << 6)

/**
 * Structure of a multiboot module.
 */
typedef struct multiboot_module {
    uint32_t module_start;
    uint32_t module_end;
    char* path;
    uint32_t reserved;
} __attribute__((packed)) multiboot_module_t;

/**
 * a.out symbol table
 */
typedef struct multiboot_aout_symbol_table {
    uint32_t tab_size;
    uint32_t str_size;
    uint32_t address;
    uint32_t reserved;
} __attribute__((packed)) multiboot_aout_symbol_table_t;

/**
 * ELF section header
 */
typedef struct multiboot_elf_section_header_table {
    uint32_t num;
    uint32_t size;
    uint32_t address;
    uint32_t shndx;
} __attribute__((packed)) multiboot_elf_section_header_table_t;

/**
 * Structure of one entry in the memory map.
 */
typedef struct multiboot_mmap {
    uint32_t size;
    uint64_t base_address;
    uint64_t length;
    uint32_t type;
} __attribute__((packed)) multiboot_mmap_t;

/**
 * Structure of one entry in the drive table.
 */
typedef struct multiboot_drives {
    uint32_t size;
    uint8_t number;
    uint8_t mode;
    uint16_t cylinders;
    uint8_t heads;
    uint8_t sectors;
    uint16_t ports;
} __attribute__((packed)) multiboot_drives_t;

/**
 * Structure of one entry in the APM table.
 */
typedef struct multiboot_apm {
    uint16_t version;
    uint16_t cseg;
    uint32_t offset;
    uint16_t cseg16;
    uint16_t dseg;
    uint16_t flags;
    uint16_t cseg_len;
    uint16_t cseg16_len;
    uint16_t dseg_len;
} __attribute__((packed)) multiboot_apm_struct_t;

/**
 * Structure that follows the multiboot standard.
 */
typedef struct multiboot_information {
    uint32_t flags;
    uint32_t memory_lower;
    uint32_t memory_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t modules_count;
    multiboot_module_t* modules;

    union {
        multiboot_aout_symbol_table_t aout_symbol_table;
        multiboot_elf_section_header_table_t elf_section_header_table;
    } tables;

    uint32_t memory_map_length;
    multiboot_mmap_t* memory_map;

    uint32_t drives_length;
    multiboot_drives_t* drives;

    void* config_table;
    const char* bootloader_name;
    multiboot_apm_struct_t* apm;

    // Followed by VBE and Framebuffer structures
} __attribute__((packed)) multiboot_information_t;

#endif
