/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/Filesystem/FileDescriptor.h>
#include <Universal/Array.h>
#include <Universal/Types.h>
#include <Universal/UniquePtr.h>

#define EI_NIDENT 16
#define EI_MAGIC 0x464C457F

#define EI_MAG0 0
#define EI_MAG1 1
#define EI_MAG2 2
#define EI_MAG3 3

#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

#define IS_ELF(ehdr) ((ehdr).e_ident[EI_MAG0] == ELFMAG0 && (ehdr).e_ident[EI_MAG1] == ELFMAG1 && (ehdr).e_ident[EI_MAG2] == ELFMAG2 && (ehdr).e_ident[EI_MAG3] == ELFMAG3)

struct [[gnu::packed]] ELFHeader {
    unsigned char e_ident[EI_NIDENT];
    u16 e_type;
    u16 e_machine;
    u32 e_version;
    u32 e_entry;
    u32 e_phoff;
    u32 e_shoff;
    u32 e_flags;
    u16 e_ehsize;
    u16 e_phentsize;
    u16 e_phnum;
    u16 e_shentsize;
    u16 e_shnum;
    u16 e_shstrndx;
};

struct [[gnu::packed]] ELFProgramHeader {
    u32 p_type;
    u32 p_offset;
    u32 p_vaddr;
    u32 p_paddr;
    u32 p_filesz;
    u32 p_memsz;
    u32 p_flags;
    u32 p_align;
};

class ELF {
public:
    static ResultReturn<UniquePtr<ELF>> create(SharedPtr<FileDescriptor>);

    ELF(SharedPtr<FileDescriptor> fd)
        : m_fd(fd)
    {
    }

    const ELFHeader& header() const { return m_header; }

    ResultReturn<Array<ELFProgramHeader>> read_program_headers();

private:
    SharedPtr<FileDescriptor> m_fd;

    ELFHeader m_header;
};
