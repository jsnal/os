#pragma once

#include <Universal/Types.h>
#include <stdint.h>

namespace PIC {

void mask(u32 mask);

void unmask(u32 mask);

void eoi(u8);

u8 read_isr();

void init();

void disable();

}
