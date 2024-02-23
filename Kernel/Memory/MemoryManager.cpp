#include <Kernel/CPU/IDT.h>
#include <Kernel/Logger.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Memory/PMM.h>
#include <Kernel/Memory/Types.h>
#include <Kernel/kmalloc.h>
#include <Kernel/panic.h>
#include <Universal/Assert.h>
#include <Universal/Stdlib.h>

#define DEBUG_TAG "MemoryManager"

using namespace Memory;

MemoryManager& MemoryManager::the()
{
    static MemoryManager s_the;
    return s_the;
}

MemoryManager::MemoryManager()
{
}

static void page_fault_interrupt_handler()
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
    IDT::register_interrupt_handler(ISR_PAGE_FAULT, page_fault_interrupt_handler);
}

void MemoryManager::internal_init(u32* boot_page_directory, const multiboot_information_t* multiboot)
{
    m_kernel_page_directory = PageDirectory::create_kernel_page_table(boot_page_directory);
    m_kernel_page_table = reinterpret_cast<PageTableEntry*>((u8*)boot_page_directory + Types::PageSize);

    kmalloc_init();

    m_pmm = new PMM(multiboot);

    dbgprintf("kernel_page_directory=%x\n", m_kernel_page_directory);
    dbgprintf("kernel_page_table=%x\n", m_kernel_page_table);
    dbgprintf("kernel_page_directory[0]=%x\n", m_kernel_page_directory.entries()[0]);
    dbgprintf("kernel_page_directory[768]=%x\n", m_kernel_page_directory.entries()[768]);
    dbgprintf("kernel_zone bitmap=%x\n", pmm().kernel_zone().bitmap().data());
    dbgprintf("user_zone bitmap=%x\n", pmm().user_zone().bitmap().data());

    // auto& pte = get_page_table_entry(m_kernel_page_directory, 0xD03FF000, true);
    ASSERT(map_kernel_page(0xC03FF000, 0x000B8000).is_ok());

    // dbgprintf("pte: %x\n", pte.physical_page_base());
    // dbgprintf("pte: %x\n", pte.address());
}

PageTableEntry& MemoryManager::get_page_table_entry(PageDirectory& page_directory, VirtualAddress virtual_address, bool is_kernel)
{
    u16 page_directory_index = PAGE_DIRECTORY_INDEX(virtual_address);
    u16 page_table_index = PAGE_TABLE_INDEX(virtual_address);
    PageDirectoryEntry& page_directory_entry = page_directory.entries()[page_directory_index];

    if (!page_directory_entry.is_present()) {
        auto page_table = is_kernel ? pmm().kernel_zone().allocate_frame() : pmm().user_zone().allocate_frame();

        memset(page_table.value().ptr(), 0, Types::PageSize);

        ASSERT(page_table.is_ok());

        dbgprintf("Allocated new page table at 0x%x\n", page_table.value());

        page_directory_entry.set_page_table_base(page_table.value().get());
        page_directory_entry.set_user_supervisor(is_kernel);
        page_directory_entry.set_present(true);
        page_directory_entry.set_read_write(true);
        page_directory.entries()[page_directory_index] = page_table.value().get();
    }

    return page_directory_entry.page_table_base()[page_table_index];
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

    dbgprintf("Mapped virtual address 0x%x to physical address 0x%x\n", virtual_address, physical_address);
    return Result::OK;
}

Result MemoryManager::unmap_kernel_page(VirtualAddress virtual_address)
{
    return Result::OK;
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
