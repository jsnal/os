#include <Kernel/CPU/IDT.h>
#include <Kernel/Logger.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Memory/Types.h>
#include <Kernel/panic.h>
#include <Universal/Assert.h>
#include <Universal/Number.h>
#include <Universal/Stdlib.h>

using namespace Memory;

extern u32 g_kernel_end;

MemoryManager& MemoryManager::the()
{
    static MemoryManager s_the;
    return s_the;
}

MemoryManager::MemoryManager()
{
}

static void page_fault_exception_handler(const InterruptFrame&)
{
    u32 fault_address;
    asm volatile("mov %0, cr2"
                 : "=r"(fault_address));

    if (fault_address == 0x0) {
        panic("Dereference of null pointer caused page fault\n");
    }
    panic("Page fault at %x\n", fault_address);
}

void MemoryManager::init(u32* boot_page_directory, const multiboot_information_t* multiboot)
{
    if (!(multiboot->flags & MULTIBOOT_FLAGS_MMAP)) {
        panic("Invaild memory map given by multiboot header\n");
    }

    the().internal_init(boot_page_directory, multiboot);
    IDT::register_exception_handler(EXCEPTION_PAGE_FAULT, page_fault_exception_handler);
}

void MemoryManager::internal_init(u32* boot_page_directory, const multiboot_information_t* multiboot)
{
    if (g_kernel_end > KERNEL_IMAGE_LENGTH) {
        panic("Kernel image is too large!\n");
    }

    m_kernel_page_directory = PageDirectory::create_kernel_page_table(PhysicalAddress(reinterpret_cast<u32>(boot_page_directory)));
    m_kernel_page_table = reinterpret_cast<PageTableEntry*>((u8*)boot_page_directory + Types::PageSize);

    // Physical Memory Layout (4 MiB):
    //     Kernel Image (Varies)
    //     kmalloc Space (1 MiB)
    //     User physical memory region bitmap
    //     Kernel physical memory region bitmap
    //     Free kernel pages

    protected_map(*m_kernel_page_directory, 0, Types::PageSize);

    u32 physical_region_base = 0;
    u32 physical_region_length = 0;

    dbgprintf("MemoryManager", "Kernel image: 0x%x - 0x%x (%u KiB)\n", KERNEL_VIRTUAL_BASE, &g_kernel_end, ((u32)&g_kernel_end - KERNEL_VIRTUAL_BASE) / 1024);
    dbgprintf("MemoryManager", "System Memory Map: lower_mem=%d KiB upper_mem=%d MiB\n", multiboot->memory_lower, multiboot->memory_upper / 1024);
    for (u32 position = 0, i = 0; position < multiboot->memory_map_length; position += sizeof(multiboot_mmap_t), i++) {
        multiboot_mmap_t* mmap = multiboot->memory_map + i;

        dbgprintf("MemoryManager", "  %x%x:%x%x %d (%s)\n",
            (u32)(mmap->base_address >> 32),
            (u32)(mmap->base_address & 0xffffffff),
            (u32)(mmap->length >> 32),
            (u32)(mmap->length & 0xffffffff),
            mmap->type,
            mmap->type == 1 ? "available" : "reserved");

        if (mmap->type != MULTIBOOT_MEMORY_AVAILABLE || mmap->base_address < (1 * MB)) {
            continue;
        }

        u32 address_remainder = (u32)(mmap->base_address % Types::PageSize);
        u32 length_remainder = (u32)(mmap->length % Types::PageSize);

        if (address_remainder != 0) {
            dbgprintf("MemoryManager", "  Region does not start on page boundary, correcting by %d bytes\n", address_remainder);
            address_remainder = Types::PageSize - address_remainder;
            mmap->base_address += address_remainder;
            mmap->length -= address_remainder;
        }

        if (length_remainder != 0) {
            dbgprintf("MemoryManager", "  Region does not end on page boundary, correcting by %d bytes\n", length_remainder);
            mmap->length -= length_remainder;
        }

        if (mmap->length < Types::PageSize) {
            dbgprintf("MemoryManager", "  Region is smaller than a page... skipping");
            continue;
        }

        physical_region_base = (u32)(mmap->base_address & 0xffffffff);
        physical_region_length = (u32)(mmap->length & 0xffffffff);

        SharedPtr<PhysicalRegion> current_region;
        bool current_region_is_kernel = false;

        for (size_t page_base = physical_region_base; page_base < (physical_region_base + physical_region_length); page_base += Types::PageSize) {
            auto address = PhysicalAddress(page_base);

            // Assuming this is Kernel image for now
            if (page_base < KERNEL_IMAGE_LENGTH) {
                continue;
            }

            if (page_base >= KERNEL_IMAGE_LENGTH && page_base < KERNEL_IMAGE_LENGTH + (1 * MB)) {
                if (!current_region_is_kernel || current_region.is_null() || current_region->upper().offset(Types::PageSize) != address) {
                    m_kernel_physical_regions.add_last(PhysicalRegion::create(address, address));
                    current_region = m_kernel_physical_regions.last();
                    current_region_is_kernel = true;
                } else {
                    current_region->expand(current_region->lower(), address);
                }
            } else {
                if (current_region_is_kernel || current_region.is_null() || current_region->upper().offset(Types::PageSize) != address) {
                    m_user_physical_regions.add_last(PhysicalRegion::create(address, address));
                    current_region = m_user_physical_regions.last();
                    current_region_is_kernel = false;
                } else {
                    current_region->expand(current_region->lower(), address);
                }
            }
        }
    }

    dbgprintf_if(DEBUG_MEMORY_MANAGER, "MemoryManager", "Physical Kernel Regions:\n");
    for (size_t i = 0; i < m_kernel_physical_regions.size(); i++) {
        m_kernel_physical_regions[i]->commit();
        dbgprintf_if(DEBUG_MEMORY_MANAGER, "MemoryManager", "  Region %u: 0x%x:0x%x\n", i, m_kernel_physical_regions[i]->lower(), m_kernel_physical_regions[i]->upper());
    }

    dbgprintf_if(DEBUG_MEMORY_MANAGER, "MemoryManager", "Physical User Regions:\n");
    for (size_t i = 0; i < m_user_physical_regions.size(); i++) {
        m_user_physical_regions[i]->commit();
        dbgprintf_if(DEBUG_MEMORY_MANAGER, "MemoryManager", "  Region %u: 0x%x:0x%x\n", i, m_user_physical_regions[i]->lower(), m_user_physical_regions[i]->upper());
    }
}

PhysicalAddress MemoryManager::allocate_physical_kernel_page()
{
    ResultOr<PhysicalAddress> page_result;
    for (int i = 0; i < m_kernel_physical_regions.size(); i++) {
        page_result = m_kernel_physical_regions[i]->allocate_page();
        if (page_result.is_ok()) {
            break;
        }
    }

    ASSERT(page_result.is_ok());
    memset(page_result.value().ptr(), 0, Types::PageSize);
    return page_result.value();
}

UniquePtr<VirtualRegion> MemoryManager::allocate_kernel_region(size_t size)
{
    auto address_range = m_kernel_page_directory->address_allocator().allocate(size);
    ASSERT(address_range.is_ok());

    auto virtual_region = VirtualRegion::create_kernel_region(address_range.value(), VirtualRegion::Read | VirtualRegion::Write | VirtualRegion::Execute);
    dbgprintf_if(DEBUG_MEMORY_MANAGER, "MemoryManager", "Allocated Kernel region from 0x%x to 0x%x\n", virtual_region->lower(), virtual_region->upper());
    virtual_region->map(*m_kernel_page_directory);
    return virtual_region;
}

UniquePtr<VirtualRegion> MemoryManager::allocate_kernel_region_at(PhysicalAddress physical_address, size_t size)
{
    auto address_range = m_kernel_page_directory->address_allocator().allocate(size);
    ASSERT(address_range.is_ok());

    auto virtual_region = VirtualRegion::create_kernel_region_at(physical_address, address_range.value(), VirtualRegion::Read | VirtualRegion::Write | VirtualRegion::Execute);
    dbgprintf_if(DEBUG_MEMORY_MANAGER, "MemoryManager", "Allocated Kernel region from 0x%x to 0x%x\n", virtual_region->lower(), virtual_region->upper());
    virtual_region->map(*m_kernel_page_directory);
    return virtual_region;
}

void MemoryManager::free_kernel_region(PhysicalAddress physical_address, size_t size)
{
    ASSERT(false);
}

void MemoryManager::protected_map(PageDirectory& page_directory, VirtualAddress virtual_address, size_t length)
{
    ASSERT(virtual_address.is_page_aligned());

    for (size_t i = 0; i < length; i += Types::PageSize) {
        auto page_table_entry_address = virtual_address.offset(i);
        auto& page_table_entry = get_page_table_entry(*m_kernel_page_directory, page_table_entry_address, true);
        page_table_entry.set_physical_page_base(page_table_entry_address.get());
        page_table_entry.set_user_supervisor(false);
        page_table_entry.set_present(false);
        page_table_entry.set_read_write(false);
    }
    flush_tlb();
}

PageTableEntry& MemoryManager::get_page_table_entry(PageDirectory& page_directory, VirtualAddress virtual_address, bool is_kernel)
{
    u16 page_directory_index = PAGE_DIRECTORY_INDEX(virtual_address);
    u16 page_table_index = PAGE_TABLE_INDEX(virtual_address);
    PageDirectoryEntry& page_directory_entry = page_directory.entries()[page_directory_index];

    if (!page_directory_entry.is_present()) {
        auto page_table = allocate_physical_kernel_page();
        memset(page_table.ptr(), 0, Types::PageSize);

        dbgprintf_if(DEBUG_MEMORY_MANAGER, "MemoryManager", "Allocated page table @ 0x%x\n", page_table);

        page_directory_entry.set_page_table_base(page_table.get());
        page_directory_entry.set_user_supervisor(is_kernel);
        page_directory_entry.set_present(true);
        page_directory_entry.set_read_write(true);
    }

    return page_directory_entry.page_table_base()[page_table_index];
}

void MemoryManager::add_vm_object(VMObject& vm_object)
{
    m_vm_objects.add_last(&vm_object);
}

void MemoryManager::remove_vm_object(VMObject& vm_object)
{
    m_vm_objects.remove(&vm_object);
}

void MemoryManager::add_virtual_region(VirtualRegion& virtual_region)
{
    if (virtual_region.upper() >= KERNEL_VIRTUAL_BASE) {
        m_kernel_virtual_regions.add_last(&virtual_region);
    } else {
        m_user_virtual_regions.add_last(&virtual_region);
    }
}

void MemoryManager::remove_virtual_region(VirtualRegion& virtual_region)
{
    if (virtual_region.upper() >= KERNEL_VIRTUAL_BASE) {
        m_kernel_virtual_regions.remove(&virtual_region);
    } else {
        m_user_virtual_regions.remove(&virtual_region);
    }
}

void MemoryManager::flush_tlb()
{
    asm volatile("mov eax, cr3; \
                  mov cr3, eax");
}
