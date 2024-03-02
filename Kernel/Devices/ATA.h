/*
 * Copyright (c) 2024, Jason Long <jasonlongball@gmail.com>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Universal/Types.h>

struct [[gnu::packed]] ATAIdentity {
    struct {
        u8 : 1; // Reserved
        u8 : 1; // Retired
        u8 response_incomplete : 1;
        u8 : 3; // Retired
        u8 fixed_device : 1;
        u8 removable_media : 1;
        u8 : 7; // Retired
        u8 device_type : 1;
    } general_config;
    u16 num_cylinders;
    u16 specific_configuration;
    u16 num_heads;
    u16 retired_1[2];
    u16 sectors_per_track;
    u16 vendor_specific_1[3];
    u8 serial_number[20];
    u16 retired_2[2];
    u16 obselete_1;
    u16 firmware_revision[4];
    u16 model_number[20];
    u8 maximum_block_transfer;
    u8 vendor_specific_2;
    u16 trusted_computing;
    struct {
        u8 current_long_sector_physical_alignment : 2;
        u8 reserved_1 : 6;
        u8 dma_supported : 1;
        u8 lba_supported : 1;
        u8 iordy_disable : 1;
        u8 iordy_supported : 1;
        u8 reserved_2 : 1;
        u8 standby_support : 1;
        u8 reserved_3 : 2;
        u16 reserved_4;
    } capabilities;
    u16 obselete_2[2];
    u16 translation_fields_valid : 3;
    u16 reserved_3 : 5;
    u8 free_fall_control_sensitivity;
    u16 current_number_of_cylinders;
    u16 current_number_of_heads;
    u16 current_sectors_per_track;
    uint32_t current_sector_capacity;
    u8 current_multi_sector_setting;
    u8 multi_sector_setting_valid : 1;
    u8 reserved_4 : 3;
    u8 sanitize_supported : 1;
    u8 crypto_scramble_command_supported : 1;
    u8 overwrite_ext_command_supported : 1;
    u8 block_erase_ext_command_supported : 1;
    uint32_t user_addressable_sectors;
    u16 obselete_3;
    u8 multi_word_dma_support;
    u8 multi_word_dma_active;
    u8 advanced_pio_modes;
    u8 reserved_5;
    u16 minimum_mw_xfer_cycle_time;
    u16 recommended_mw_xfer_time;
    u16 minimum_pio_cycle_time;
    u16 minimum_pio_cycle_time_iordy;
    u16 additional_supported;
    u16 reserved_6[5];
    u8 queue_depth : 5;
    u16 sata_capabilities;
    u16 sata_additional_capabilities;
    u16 sata_features_supported;
    u16 sata_features_enabled;
    u16 major_version;
    u16 minor_version;
    u16 commands_supported[3];
    u16 commands_active[3];
    u8 udma_support;
    u8 udma_active;
    u16 security_timing[2];
    u8 apm_level;
    u8 reserved_7;
    u16 master_password_id;
    u16 hardware_reset_result;
    u8 acoustic_value;
    u8 recommended_acoustic_value;
    u16 stream_minimum_request_size;
    u16 stream_transfer_time_dma;
    u16 stream_access_latency_pio;
    u32 streaming_performance_granularity;
    u64 user_addressable_logical_sectors;
    u16 streaming_transfer_time;
    u16 dsm_cap;
    struct {
        u8 logical_sectors_per_physical_sector : 4;
        u8 reserved_1 : 8;
        u8 logical_sector_longer_than_256_words : 1;
        u8 multiple_logical_sectors_per_physical_sector : 1;
        u8 reserved_2 : 2;
    } physical_logical_sector_size;
    u16 inter_seek_delay;
    u16 world_wide_name[4];
    u16 world_wide_name_reserved[4];
    u16 technical_report_reserved;
    uint32_t words_per_logical_sector;
    u16 command_set_support_ext;
    u16 command_set_active_ext;
    u16 reserved_for_expanded_support_and_active[6];
    u8 msn_support : 2;
    u16 reserved_8 : 14;
    u16 security_status;
    u16 reserved_9[31];
    u16 cfa_power_mode;
    u16 reserved_for_cfa_1[7];
    u8 nominal_form_factor : 4;
    u16 reserved_10 : 12;
    struct {
        u8 supports_trim : 1;
        u16 reserved : 15;
    } data_set_management_features;
    u16 additional_product_id[4];
    u16 reserved_for_cfa_2[2];
    u16 current_media_serial_number[30];
    u16 sct_command_support;
    u16 reserved_11[2];
    struct {
        u16 alignment_of_logical_within_physical : 14;
        u8 word_209_supported : 1;
        u8 reserved : 1;
    } block_alignment;
    u16 write_read_verify_sector_mode_3_only[2];
    u16 write_read_verify_sector_mode_2_only[2];
    u16 nv_cache_capabilities;
    uint32_t nv_cache_size;
    u16 nominal_media_rotation_rate;
    u16 reserved_12;
    u16 nv_cache_options;
    u8 write_read_verify_sector_count_mode;
    u8 reserved_13;
    u16 reserved_14;
    struct {
        u16 major_version : 12;
        u8 transport_type : 4;
    } transport_major_version;
    u16 transport_minor_version;
    u16 reserved_15[6];
    u64 extended_number_of_user_addressable_sectors;
    u16 min_blocks_per_download_microcode_operation;
    u16 max_blocks_per_download_microcode_operation;
    u16 reserved_16[19];
    u8 signature;
    u8 checksum;
};
