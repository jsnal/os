/*
 * cppruntime.cpp
 *
 *  Created on: Oct 2, 2008
 *      Author: kllrnohj
 */

namespace __cxxabiv1 {
/* guard variables */

/* The ABI requires a 64-bit type.  */
__extension__ typedef int __guard __attribute__((mode(__DI__)));

extern "C" int __cxa_guard_acquire(__guard*);
extern "C" void __cxa_guard_release(__guard*);
extern "C" void __cxa_guard_abort(__guard*);

extern "C" int __cxa_guard_acquire(__guard* g)
{
    return !*(char*)(g);
}

extern "C" void __cxa_guard_release(__guard* g)
{
    *(char*)g = 1;
}

extern "C" void __cxa_guard_abort(__guard*)
{
}
}

extern "C" void __cxa_pure_virtual()
{
}

extern "C" {
int __cxa_atexit(void (*f)(void*), void* p, void* d);
void __cxa_finalize(void* d);
};

void* __dso_handle;

struct object {
    void (*f)(void*);
    void* p;
    void* d;
};

struct object objects[32] = {};
unsigned int object_index = 0;

int __cxa_atexit(void (*f)(void*), void* p, void* d)
{
    if (object_index >= 32)
        return -1;
    objects[object_index].f = f;
    objects[object_index].p = p;
    objects[object_index].d = d;
    ++object_index;
    return 0;
}

/* This currently destroys all objects */
void __cxa_finalize(void*)
{
    unsigned int i = object_index;
    for (; i > 0; --i) {
        --object_index;
        objects[object_index].f(objects[object_index].p);
    }
}
