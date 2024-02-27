#include <Kernel/Logger.h>
#include <Kernel/Memory/PMM.h>
#include <Kernel/kmalloc.h>
#include <Universal/Assert.h>
#include <Universal/Stdlib.h>

#define KMALLOC_FOREVER_SIZE (KB * 512)

__attribute__((section(".heap"))) static u8 kmalloc_initial_heap[KMALLOC_INITIAL_HEAP_SIZE];

static KmallocTracker* s_kmalloc_tracker;
alignas(KmallocTracker) static u8 s_kmalloc_tracker_heap[sizeof(KmallocTracker)];

void* operator new(size_t, void* ptr)
{
    return ptr;
}

void* operator new(size_t size)
{
    void* ptr = kmalloc(size);
    ASSERT(ptr != nullptr);
    return ptr;
}

void* operator new[](size_t size)
{
    void* ptr = kmalloc(size);
    ASSERT(ptr != nullptr);
    return ptr;
}

void operator delete(void* ptr)
{
    kfree(ptr);
}

void operator delete[](void* ptr)
{
    kfree(ptr);
}

void KmallocTracker::add_heap(u8* heap, size_t heap_size)
{
    // TODO: This should append to a list, not override the original heap
    m_heap = Heap<KMALLOC_HEAP_CHUNK_SIZE>(heap, heap_size);
}

void* kmalloc(size_t size)
{
    ASSERT(s_kmalloc_tracker != nullptr);
    void* address = s_kmalloc_tracker->heap().allocate(size);
    dbgprintf("kmalloc", "%d byte allocation @ 0x%x\n", size, address);
    return address;
}

void* kcalloc(size_t size)
{
    ASSERT(s_kmalloc_tracker != nullptr);
    void* address = s_kmalloc_tracker->heap().allocate(size);
    dbgprintf("kmalloc", "%d byte allocation @ 0x%x\n", size, address);
    memset(address, 0, size);
    return address;
}

void kfree(void* ptr)
{
    dbgprintf("kmalloc", "free @ 0x%x", ptr);
    s_kmalloc_tracker->heap().deallocate(ptr);
}

void kmalloc_init()
{
    s_kmalloc_tracker = new (s_kmalloc_tracker_heap) KmallocTracker(kmalloc_initial_heap, KMALLOC_INITIAL_HEAP_SIZE);
    dbgprintf("kmalloc", "Initialzed kmalloc: 0x%x, total_chunks=%d\n", s_kmalloc_tracker, s_kmalloc_tracker->heap().total_chunks());
}
