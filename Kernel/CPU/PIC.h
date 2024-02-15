#pragma once

#include <stdint.h>

namespace PIC {

void mask(uint32_t mask);

void unmask(uint32_t mask);

void eoi(uint8_t);

void init();

void disable();

}
