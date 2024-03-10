/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Types.h>

struct [[gnu::packed]] Ext2Superblock {
    u32 total_inodes;
    u32 total_blocks;
    u32 superuser_blocks;
    u32 free_blocks;
    u32 free_inodes;
    u32 superblock_block;
    u32 block_size;
    u32 fragment_size;
    u32 blocks_per_group;
    u32 fragments_per_group;
    u32 inodes_per_group;
    u32 last_mount;
    u32 last_write;
    u16 times_mounted;
    u16 mounts_allowed;
    u16 signature;
    u16 state;
    u16 error_action;
    u16 version_minor;
    u32 last_check;
    u32 check_interval;
    u32 os_id;
    u32 version_major;
    u16 reserved_user;
    u16 reserved_group;

    // Extended fields
    u32 first_inode;
    u16 inode_size;
    u16 superblock_group;
    u32 optional_features;
    u32 required_features;
    u32 ro_features;
    u8 filesystem_id[16];
    u8 volume_name[16];
    u8 last_mount_path[64];
    u32 compression;
    u8 file_prealloc_blocks;
    u8 directory_prealloc_blocks;
    u16 unused;
    u8 journal_id[16];
    u32 journal_inode;
    u32 journal_device;
    u32 orphan_inode_head;
    u8 extra[276];
};
