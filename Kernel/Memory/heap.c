#include <logger.h>
#include <memory/heap.h>
#include <memory/pmm.h>

#define DEBUG_TAG "Heap"

void init_heap(virtual_address_t heap_start)
{
    virtual_address_t address = pmm_allocate_frame(ZONE_KERNEL, virtual_to_physical(heap_start), HEAP_SIZE / PAGE_SIZE);
    dbgprintf("Initialized Heap: %x\n", address);
}
