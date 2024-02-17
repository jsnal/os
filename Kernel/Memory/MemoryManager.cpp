#include <Logger.h>
#include <Memory/MemoryManager.h>
#include <Memory/PMM.h>
#include <Memory/Types.h>
#include <panic.h>

#define DEBUG_TAG "MemoryManager"

static MemoryManager* s_the;

MemoryManager& MemoryManager::the()
{
    return *s_the;
}

void MemoryManager::init(u32* boot_page_directory, const multiboot_information_t* multiboot)
{
    if (!(multiboot->flags & MULTIBOOT_FLAGS_MMAP)) {
        panic("Invaild memory map given by multiboot header\n");
    }

    s_the->m_kernel_page_directory = reinterpret_cast<PageDirectoryEntry*>(boot_page_directory);
    s_the->m_kernel_page_table = reinterpret_cast<PageTableEntry*>((u8*)boot_page_directory + PAGE_SIZE);
    s_the->init(multiboot);
}

void MemoryManager::init(const multiboot_information_t* multiboot)
{
    m_pmm = PMM(multiboot);
    dbgprintf("kernel_page_directory=%x\n", m_kernel_page_directory);
    dbgprintf("kernel_page_table=%x\n", m_kernel_page_table);
    dbgprintf("kernel_page_directory[0]=%x\n", m_kernel_page_directory[0]);
    dbgprintf("kernel_page_directory[768]=%x\n", m_kernel_page_directory[758]);
    dbgprintf("user_zone bitmap=%x\n", m_pmm.kernel_zone().bitmap().data());

    dbgprintf("kernel_zone_lower=%x\n", m_pmm.kernel_zone().lower_address());
    dbgprintf("kernel_zone_upper=%x\n", m_pmm.kernel_zone().upper_address());
    dbgprintf("user_zone_lower=%x\n", m_pmm.user_zone().lower_address());
    dbgprintf("user_zone_upper=%x\n", m_pmm.user_zone().upper_address());

    PhysicalAddress a1 = m_pmm.kernel_zone().allocate_frame(0x00100000, 8);
    // m_pmm.kernel_zone().free_frame(a5);
}

MemoryManager::MemoryManager()
{
    s_the = this;
}
