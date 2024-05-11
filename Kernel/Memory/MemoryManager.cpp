#include <Kernel/CPU/IDT.h>
#include <Kernel/Logger.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Memory/PMM.h>
#include <Kernel/Memory/Types.h>
#include <Kernel/panic.h>
#include <Universal/Assert.h>
#include <Universal/Number.h>
#include <Universal/Stdlib.h>

using namespace Memory;

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
    if (g_kernel_end > 3 * MB) {
        panic("Kernel image is greater then 3 MB\n");
    }

    m_kernel_page_directory = PageDirectory::create_kernel_page_table(boot_page_directory);
    m_kernel_page_table = reinterpret_cast<PageTableEntry*>((u8*)boot_page_directory + Types::PageSize);

    // Physical Memory Layout (4 MiB):
    //     Kernel Image (Varies)
    //     kmalloc Space (1 MiB)
    //     User physical memory region bitmap
    //     Kernel physical memory region bitmap
    //     Free kernel pages

    u32 physical_region_base = 0;
    u32 physical_region_length = 0;

    dbgprintf("MemoryMananger", "System Memory Map: lower_mem=%d KiB upper_mem=%d MiB\n", multiboot->memory_lower, multiboot->memory_upper / 1024);
    for (u32 position = 0, i = 0; position < multiboot->memory_map_length; position += sizeof(multiboot_mmap_t), i++) {
        multiboot_mmap_t* mmap = multiboot->memory_map + i;

        dbgprintf("PMM", "  %x%x:%x%x %d (%s)\n",
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
            if (page_base < 3 * MB) {
                continue;
            }

            if (page_base >= 3 * MB && page_base <= 4 * MB) {
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

    m_pmm = new PMM(multiboot);

    // dbgprintf("MemoryManager", "kernel_page_directory=%x\n", m_kernel_page_directory);
    // dbgprintf("MemoryManager", "kernel_page_table=%x\n", m_kernel_page_table);
    // dbgprintf("MemoryManager", "kernel_page_directory[0]=%x\n", m_kernel_page_directory.entries()[0]);
    // dbgprintf("MemoryManager", "kernel_page_directory[768]=%x\n", m_kernel_page_directory.entries()[768]);
    // dbgprintf("MemoryManager", "kernel_zone bitmap=%x\n", pmm().kernel_region().bitmap().data());
    // dbgprintf("MemoryManager", "user_zone bitmap=%x\n", pmm().user_region().bitmap().data());

    m_kernel_virtual_region = Region<VirtualAddress>((u32)&g_kernel_end, (KERNEL_VIRTUAL_BASE + KERNEL_REGION_LENGTH) - (u32)&g_kernel_end);

    // auto& pte = get_page_table_entry(m_kernel_page_directory, 0xD03FF000, true);
    // ASSERT(map_kernel_page(0xC03FF000, 0x000B8000).is_ok());

    // dbgprintf("pte: %x\n", pte.physical_page_base());
    // dbgprintf("pte: %x\n", pte.address());
}

PageTableEntry& MemoryManager::get_page_table_entry(PageDirectory& page_directory, VirtualAddress virtual_address, bool is_kernel)
{
    u16 page_directory_index = PAGE_DIRECTORY_INDEX(virtual_address);
    u16 page_table_index = PAGE_TABLE_INDEX(virtual_address);
    PageDirectoryEntry& page_directory_entry = page_directory.entries()[page_directory_index];

    if (!page_directory_entry.is_present()) {
        auto page_table = is_kernel ? pmm().kernel_region().allocate_frame() : pmm().user_region().allocate_frame();

        memset(page_table.value().ptr(), 0, Types::PageSize);

        ASSERT(page_table.is_ok());

        dbgprintf("MemoryManager", "Allocated new page table at 0x%x\n", page_table.value());

        page_directory_entry.set_page_table_base(page_table.value().get());
        page_directory_entry.set_user_supervisor(is_kernel);
        page_directory_entry.set_present(true);
        page_directory_entry.set_read_write(true);
        page_directory.entries()[page_directory_index] = page_table.value().get();
    }

    return page_directory_entry.page_table_base()[page_table_index];
}

ResultOr<VirtualAddress> MemoryManager::map_kernel_region(PhysicalAddress physical_address, size_t size)
{
    u32 pages_needed = ceiling_divide(size, Types::PageSize);
    auto result = m_kernel_virtual_region.allocate_contiguous_frame(pages_needed);
    if (result.is_error()) {
        dbgprintf("MemoryManager", "Unable to map kernel region of %u bytes (%u pages)\n", size, pages_needed);
        return result.error();
    }

    dbgprintf("MemoryManager", "Found %d contiguous pages @ 0x%x\n", pages_needed, result.value());

    return Result(Result::Failure);
}

Result MemoryManager::unmap_kernel_region(PhysicalAddress, size_t)
{
    return Result::OK;
}

Result MemoryManager::map_kernel_page(VirtualAddress virtual_address, PhysicalAddress physical_address)
{
    if (virtual_address.get() < KERNEL_VIRTUAL_BASE) {
        return Types::AddressOutOfRange;
    }

    PageTableEntry& page_table_entry = get_page_table_entry(m_kernel_page_directory, virtual_address, true);
    page_table_entry.set_physical_page_base(physical_address);
    page_table_entry.set_present(true);
    page_table_entry.set_read_write(true);
    page_table_entry.set_user_supervisor(true);

    invalidate_page(virtual_address);

    dbgprintf("MemoryManager", "Mapped virtual address 0x%x to physical address 0x%x\n", virtual_address, physical_address);
    return Result::OK;
}

Result MemoryManager::unmap_kernel_page(VirtualAddress virtual_address)
{
    if (virtual_address.get() < KERNEL_VIRTUAL_BASE) {
        return Types::AddressOutOfRange;
    }

    u16 page_directory_index = PAGE_DIRECTORY_INDEX(virtual_address);
    u16 page_table_index = PAGE_TABLE_INDEX(virtual_address);
    PageDirectoryEntry& page_directory_entry = m_kernel_page_directory.entries()[page_directory_index];

    if (!page_directory_entry.is_present()) {
        return Types::AddressOutOfRange;
    }

    page_directory_entry.page_table_base()[page_table_index].set_present(false);
    return Result::OK;
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

void MemoryManager::invalidate_page(VirtualAddress address)
{
    asm volatile("invlpg [%0]"
                 :
                 : "r"(address.get()));
}
