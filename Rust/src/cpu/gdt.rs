use u64 as EntryRaw;

#[repr(C, packed)]
struct Entry {
    limit_low: u16,
    base_low: u16,
    base_middle: u8,
    access: u8,
    flags_and_limit_high: u8,
    base_high: u8,
}

impl Entry {
    const fn new() -> Self {
        Self {
            limit_low: 0,
            base_low: 0,
            base_middle: 0,
            access: 0,
            flags_and_limit_high: 0,
            base_high: 0,
        }
    }

    fn set_entry(&mut self, base: u32, limit: u32, access: u8, flags: u8) {
        self.base_low = (base & 0xFFFF) as u16;
        self.base_middle = ((base >> 16) & 0xFF) as u8;
        self.base_high = ((base >> 24) & 0xFF) as u8;
        self.limit_low = (limit & 0xFFFF) as u16;
        self.flags_and_limit_high = ((limit >> 16) & 0x0F) as u8;
        self.flags_and_limit_high |= flags & 0x0F;
        self.access = access;
    }
}

#[repr(C, packed)]
struct TablePointer {
    limit: u16,
    base: u32,
}

lazy_static! {}

pub fn init() {}
