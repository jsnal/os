#ifndef CPU_PIC_H
#define CPU_PIC_H

#include <stdint.h>

#define PIC1_COMMAND 0x0020
#define PIC1_DATA 0x0021
#define PIC2_COMMAND 0x00A0
#define PIC2_DATA 0x00A1
#define PIC1_OFFSET 0x20
#define PIC2_OFFSET 0x28
#define PIC_EOI 0x20

/**
 * @brief mask off a specific IRQ. This can be used to disable an IRQ from firing.
 *
 * @param mask the mask
 */
void pic_mask(uint32_t mask);

/**
 * @brief unmask a specific IRQ. This can be used to enable an IRQ.
 *
 * @param mask the mask
 */
void pic_unmask(uint32_t mask);

void pic_eoi(uint8_t);

/*
 * @brief initialize the programmable interrupt controller. Maps to IRQ 32 to 47.
 */
void pic_init();

void pic_disable();

#endif
