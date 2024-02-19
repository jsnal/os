#include <Kernel/Logger.h>
#include <Kernel/Memory/MemoryManager.h>
#include <Kernel/Memory/PMM.h>
#include <Kernel/Memory/Types.h>
#include <Kernel/kmalloc.h>
#include <Kernel/panic.h>

#define DEBUG_TAG "MemoryManager"

MemoryManager& MemoryManager::the()
{
    static MemoryManager s_the;
    return s_the;
}

MemoryManager::MemoryManager()
{
}

void MemoryManager::init(u32* boot_page_directory, const multiboot_information_t* multiboot)
{
    if (!(multiboot->flags & MULTIBOOT_FLAGS_MMAP)) {
        panic("Invaild memory map given by multiboot header\n");
    }

    the().internal_init(boot_page_directory, multiboot);
}

void MemoryManager::internal_init(u32* boot_page_directory, const multiboot_information_t* multiboot)
{
    m_kernel_page_directory = reinterpret_cast<PageDirectoryEntry*>(boot_page_directory);
    m_kernel_page_table = reinterpret_cast<PageTableEntry*>((u8*)boot_page_directory + PAGE_SIZE);

    kmalloc_init();

    m_pmm = new PMM(multiboot);

    dbgprintf("kernel_page_directory=%x\n", m_kernel_page_directory);
    dbgprintf("kernel_page_table=%x\n", m_kernel_page_table);
    dbgprintf("kernel_page_directory[0]=%x\n", m_kernel_page_directory[0]);
    dbgprintf("kernel_page_directory[768]=%x\n", m_kernel_page_directory[768]);
    dbgprintf("kernel_zone bitmap=%x\n", pmm().kernel_zone().lower_address());
    dbgprintf("user_zone bitmap=%x\n", pmm().user_zone().lower_address());
}
