#include <Kernel/Logger.h>
#include <Kernel/Memory/PMM.h>
#include <Kernel/kmalloc.h>
#include <Universal/Assert.h>
#include <Universal/Stdlib.h>

#define DEBUG_TAG "kmalloc"

#define KMALLOC_FOREVER_SIZE (KB * 512)
#define KMALLOC_INITIAL_HEAP_SIZE (MB * 1)

__attribute__((section(".heap"))) static u8 kmalloc_initial_heap[KMALLOC_INITIAL_HEAP_SIZE];

static KmallocTracker* s_kmalloc_tracker;
alignas(KmallocTracker) static u8 s_kmalloc_tracker_heap[sizeof(KmallocTracker)];

static u8* s_kmalloc_forever_pointer;
static u8* s_kmalloc_forever_end;

inline void* operator new(size_t, void* ptr) { return ptr; }

void KmallocTracker::add_heap(u8* heap, size_t heap_size)
{
    // TODO: This should append to a list, not override the original heap
    m_heap = Heap<KMALLOC_HEAP_CHUNK_SIZE>(heap, heap_size);
}

void kmalloc_init()
{
    s_kmalloc_forever_pointer = (u8*)Memory::Types::page_round_up((u32)&g_kernel_end);
    s_kmalloc_forever_end = s_kmalloc_forever_pointer + KMALLOC_FOREVER_SIZE;

    s_kmalloc_tracker = new (s_kmalloc_tracker_heap) KmallocTracker(kmalloc_initial_heap, KMALLOC_INITIAL_HEAP_SIZE);

    dbgprintf("Allocated s_kmalloc_tracker %x, total_chunks=%d, chunks=%x, bitmap=%x\n",
        s_kmalloc_tracker, s_kmalloc_tracker->heap().total_chunks(), s_kmalloc_tracker->heap().chunks(), s_kmalloc_tracker->heap().bitmap());

    dbgprintf("kmalloc_forever_pointer=%x, kmalloc_forever_end=%x\n", s_kmalloc_forever_pointer, s_kmalloc_forever_end);
}

void* kmalloc(size_t size)
{
    ASSERT(s_kmalloc_tracker != nullptr);
    void* address = s_kmalloc_tracker->heap().allocate(size);
    dbgprintf("%d byte allocation @ 0x%x\n", size, address);
    return address;
}

void kmalloc_free(void* ptr)
{
    s_kmalloc_tracker->heap().deallocate(ptr);
}

void* kmalloc_forever(size_t size)
{
    ASSERT(size != 0 && size <= KMALLOC_FOREVER_SIZE && (s_kmalloc_forever_pointer + size) <= s_kmalloc_forever_end);
    u8* address = s_kmalloc_forever_pointer;
    s_kmalloc_forever_pointer += size;
    return address;
}
