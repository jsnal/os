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
    ELF(SharedPtr<FileDescriptor> fd)
        : m_fd(fd)
    {
    }

    UniquePtr<ELFHeader> read_header();

    ResultReturn<Array<ELFProgramHeader>> read_program_headers(ELFHeader&);

private:
    SharedPtr<FileDescriptor> m_fd;
};
