/// # Notes
/// * https://www.gnu.org/software/grub/manual/multiboot/multiboot.html

/// The magic field should contain this.
pub const HEADER_MAGIC: u32 = 0x1BADB002;

/// This should be in %eax.
pub const BOOTLOADER_MAGIC: u32 = 0x2BADB002;

#[repr(C, packed)]
pub struct Header {
    /// Must be MULTIBOOT_MAGIC.
    pub magic: u32,

    /// Feature flags.
    pub flags: u32,

    /// The above fields plus this one must equal 0 mod 2^32.
    pub checksum: u32,

    /// These are only valid if MULTIBOOT_AOUT_KLUDGE is set.
    pub header_addr: u32,
    pub load_addr: u32,
    pub load_end_addr: u32,
    pub bss_end_addr: u32,
    pub entry_addr: u32,

    /// These are only valid if MULTIBOOT_VIDEO_MODE is set.
    pub mode_type: u32,
    pub width: u32,
    pub height: u32,
    pub depth: u32,
}

#[repr(C, packed)]
pub struct Info {
    /// Multiboot info version number.
    pub flags: u32,

    /// Available memory from BIOs.
    pub mem_lower: u32,
    pub mem_upper: u32,

    /// "root" partition.
    pub boot_device: u32,

    /// Kernel command line.
    pub cmdline: u32,

    /// Boot-Module list.
    pub mods_count: u32,
    pub mods_addr: u32,

    /// Reserved for a.out symbol/ELF header tables.
    pub reserved1: u32,
    pub reserved2: u32,
    pub reserved3: u32,
    pub reserved4: u32,

    /// Memory Mapping buffer.
    pub mmap_length: u32,
    pub mmap_addr: u32,

    /// Drive Info buffer.
    pub drives_length: u32,
    pub drives_addr: u32,

    /// ROM configuration table.
    pub config_table: u32,

    /// Boot Loader Name
    pub boot_loader_name: u32,

    /// APM table.
    pub apm_table: u32,

    /// Video.
    pub vbe_control_info: u32,
    pub vbe_mode_info: u32,
    pub vbe_mode: u16,
    pub vbe_interface_seg: u16,
    pub vbe_interface_off: u16,
    pub vbe_interface_len: u16,

    pub framebuffer_addr: u64,
    pub framebuffer_pitch: u32,
    pub framebuffer_width: u32,
    pub framebuffer_height: u32,
    pub framebuffer_bpp: u8,
    // TODO: More framebuffer stuff here.
}
