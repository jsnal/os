/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#ifndef _MEMORY_PAGING_H_
#define _MEMORY_PAGING_H_

#include <limits.h>
#include <multiboot.h>
#include <stdint.h>

void init_paging(uint32_t* kernel_page_directory, const multiboot_information_t* multiboot);

#endif
