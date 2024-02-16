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
    dbgprintf("kernel_page_directory=%x\n", m_kernel_page_directory[0]);
    dbgprintf("kernel_page_directory=%x\n", m_kernel_page_directory[1]);
    dbgprintf("kernel_zone=%x\n", m_pmm.kernel_zone().bitmap().data());

    m_pmm.kernel_zone().bitmap().set(1, true);
    m_pmm.kernel_zone().bitmap().set(3, true);
    m_pmm.kernel_zone().bitmap().set(5, true);
    m_pmm.kernel_zone().bitmap().set(7, true);
}

MemoryManager::MemoryManager()
{
    s_the = this;
}
