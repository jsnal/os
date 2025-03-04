#include <Kernel/kmalloc.h>
#include <Universal/Assert.h>
#include <Universal/Logger.h>
#include <Universal/Stdlib.h>

#define DEBUG_KMALLOC 0
#define KMALLOC_FOREVER_SIZE (KB * 512)

__attribute__((section(".heap"))) static u8 kmalloc_initial_heap[KMALLOC_INITIAL_HEAP_SIZE];

static KmallocTracker* s_kmalloc_tracker;
alignas(KmallocTracker) static u8 s_kmalloc_tracker_heap[sizeof(KmallocTracker)];

void KmallocTracker::add_heap(u8* heap, size_t heap_size)
{
    // TODO: This should append to a list, not override the original heap
    m_heap = Heap<KMALLOC_HEAP_CHUNK_SIZE>(heap, heap_size);
}

void* kmalloc(size_t size)
{
    ASSERT(s_kmalloc_tracker != nullptr);
    if (size == 0) {
        dbgprintf_if(DEBUG_KMALLOC, "kmalloc", "Ignoring 0 byte allocation\n");
        return 0;
    }

    void* address = s_kmalloc_tracker->heap().allocate(size);
    dbgprintf_if(DEBUG_KMALLOC, "kmalloc", "%d byte allocation @ 0x%x\n", size, address);
    return address;
}

void* kcalloc(size_t size)
{
    ASSERT(s_kmalloc_tracker != nullptr);
    if (size == 0) {
        dbgprintf_if(DEBUG_KMALLOC, "kmalloc", "Ignoring 0 byte allocation\n");
        return 0;
    }

    void* address = s_kmalloc_tracker->heap().allocate(size);
    dbgprintf_if(DEBUG_KMALLOC, "kmalloc", "%d byte allocation @ 0x%x\n", size, address);
    memset(address, 0, size);
    return address;
}

void kfree(void* ptr)
{
    dbgprintf_if(DEBUG_KMALLOC, "kmalloc", "free @ 0x%x\n", ptr);
    s_kmalloc_tracker->heap().deallocate(ptr);
}

void kmalloc_init()
{
    s_kmalloc_tracker = new (s_kmalloc_tracker_heap) KmallocTracker(kmalloc_initial_heap, KMALLOC_INITIAL_HEAP_SIZE);
    dbgprintf("kmalloc", "Initialized kmalloc: 0x%x, total_chunks=%d\n", s_kmalloc_tracker, s_kmalloc_tracker->heap().total_chunks());
}

void kmalloc_dump_statistics()
{
    dbgprintf("kmalloc", "%u chunks allocated, %u chunks free, %u total chunks\n", s_kmalloc_tracker->heap().allocated_chunks(), s_kmalloc_tracker->heap().free_chunks(), s_kmalloc_tracker->heap().total_chunks());
}
