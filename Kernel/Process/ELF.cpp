/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Process/ELF.h>

UniquePtr<ELFHeader> ELF::read_header()
{
    int seek_ret = m_fd->seek(0, SEEK_SET);
    if (seek_ret < 0) {
        return nullptr;
    }

    UniquePtr<ELFHeader> header = make_unique_ptr<ELFHeader>();

    int nread = m_fd->read(reinterpret_cast<u8*>(header.ptr()), sizeof(ELFHeader));
    if (nread < 0) {
        return nullptr;
    }

    return header;
}

ResultReturn<Array<ELFProgramHeader>> ELF::read_program_headers(ELFHeader& header)
{
    int seek_ret = m_fd->seek(header.e_phoff, SEEK_SET);
    if (seek_ret < 0) {
        return Result(seek_ret);
    }

    dbgprintf("ELF", "Amount to read: %u\n", header.e_phnum * header.e_phentsize);

    dbgprintf("ELF", "e_phnum %u\n", header.e_phnum);
    Array<ELFProgramHeader> program_headers(header.e_phnum);
    int nread = m_fd->read(reinterpret_cast<u8*>(program_headers.raw_data()), header.e_phnum * header.e_phentsize);
    if (nread < 0) {
        return Result(nread);
    }

    return program_headers;
}
