#include <Kernel/Logger.h>
#include <Kernel/Memory/PMM.h>
#include <Kernel/kmalloc.h>
#include <Universal/Assert.h>

#define DEBUG_TAG "kmalloc"

#define KMALLOC_FOREVER_SIZE (1 * MB)

#define KMALLOC_RESULT_OOM 1

static u8* s_kmalloc_forever_pointer;
static u8* s_kmalloc_forever_end;

void kmalloc_init()
{
    s_kmalloc_forever_pointer = (u8*)PAGE_ROUND_UP((u32)&g_kernel_end);
    s_kmalloc_forever_end = s_kmalloc_forever_pointer + KMALLOC_FOREVER_SIZE;

    dbgprintf("kmalloc_forever_pointer=%x, kmalloc_forever_end=%x\n", s_kmalloc_forever_pointer, s_kmalloc_forever_end);
}

void* kmalloc_forever(size_t size)
{
    ASSERT(size != 0 && size <= KMALLOC_FOREVER_SIZE && (s_kmalloc_forever_pointer + size) <= s_kmalloc_forever_end);
    u8* return_address = s_kmalloc_forever_pointer;
    s_kmalloc_forever_pointer += size;
    return return_address;
}
