/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <Kernel/Memory/VirtualRegion.h>
#include <Kernel/POSIX.h>
#include <Kernel/Process/ELF.h>

ResultReturn<UniquePtr<ELF>> ELF::create(SharedPtr<FileDescriptor> fd)
{
    auto elf = make_unique_ptr<ELF>(fd);

    int seek_ret = fd->seek(0, SEEK_SET);
    if (seek_ret < 0) {
        return Result(seek_ret);
    }

    int read_ret = fd->read(reinterpret_cast<u8*>(&elf->m_header), sizeof(ELFHeader));
    if (read_ret < 0) {
        return Result(read_ret);
    }

    if (!IS_ELF(elf->header())) {
        return Result(Result::Failure);
    }

    return elf;
}

ResultReturn<Array<ELFProgramHeader>> ELF::read_program_headers()
{
    int seek_ret = m_fd->seek(m_header.e_phoff, SEEK_SET);
    if (seek_ret < 0) {
        return Result(seek_ret);
    }

    Array<ELFProgramHeader> program_headers(m_header.e_phnum);
    int nread = m_fd->read(program_headers.ptr(), m_header.e_phnum * m_header.e_phentsize);
    if (nread < 0) {
        return Result(nread);
    }

    return program_headers;
}

u8 ELF::program_flags_to_access(u32 flags)
{
    u8 access = 0;
    if (flags & PF_R) {
        access |= VirtualRegion::Read;
    }
    if (flags & PF_W) {
        access |= VirtualRegion::Write;
    }
    if (flags & PF_X) {
        access |= VirtualRegion::Execute;
    }
    return access;
}
