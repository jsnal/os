/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Types.h>

#define EXT2_SUPER_BLOCK_SIGNATURE 0xEF53

#define EXT2_BAD_INO 1
#define EXT2_ROOT_INO 2

#define BLOCK_SIZE(s) (1024 << s)
#define FRAGMENT_SIZE(s) (1024 << s)

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

struct [[gnu::packed]] Ext2BlockGroupDescriptor {
    u32 block_usage_bitmap;
    u32 inode_usage_bitmap;
    u32 inode_table;
    u16 free_blocks;
    u16 free_inodes;
    u16 used_directories_count;
    u8 unused[14];
};

struct [[gnu::packed]] Ext2Inode {
    u16 mode;
    uid_t uid;
    u32 size;
    u32 atime;
    u32 ctime;
    u32 mtime;
    u32 dtime;
    gid_t gid;
    u16 hard_links;
    u32 logical_blocks;
    u32 flags;
    u32 os_specific_1;
    u32 block_pointers[12];
    u32 s_pointer;
    u32 d_pointer;
    u32 t_pointer;
    u32 generation;
    u32 file_acl;
    u32 dir_acl;
    u32 fragment_address;
    u32 os_specific_2[3];
};

struct [[gnu::packed]] Ext2Directory {
    u32 inode;
    u16 size;
    u8 name_length;
    u8 type;
};
