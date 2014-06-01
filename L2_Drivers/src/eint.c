/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */
#include <stdlib.h>
#include "eint.h"



/// Linked list structure of EINTs (External interrupts)
typedef struct eint3_entry {
    uint64_t pin_mask;          ///< P0 and P2 concatenated pin-mask
    void_func_t callback;       ///< Callback when interrupt occurs
    struct eint3_entry* next;   ///< The pointer to the next entry
} eint3_entry_t;

static eint3_entry_t *gp_eint3_list_head = NULL; ///< Instance of list of EINTS
static const uint8_t g_port2_offset = 32;        ///< Offset of Port2



/// Actual ISR Handler (mapped to startup file's interrupt vector function name)
#ifdef __cplusplus
extern "C" {
#endif
void EINT3_IRQHandler(void)
{
    uint64_t status = 0;
    eint3_entry_t *e = gp_eint3_list_head;

    /* Get status of PORT0 and PORT2, and store to 64-bit variable */
    status = (LPC_GPIOINT->IO2IntStatR | LPC_GPIOINT->IO2IntStatF);
    status <<= g_port2_offset;
    status |= (LPC_GPIOINT->IO0IntStatR | LPC_GPIOINT->IO0IntStatF);

    /* Loop through our list to find which pin triggered this interrupt */
    while (e && status) {
        /* If we find the pin, make the callback and clear the interrupt source*/
        if (e->pin_mask & status) {
            status &= ~(e->pin_mask);
            (e->callback)();

            /* Clear the interrupt based on either Port0 or Port2 */
            if (e->pin_mask & UINT32_MAX) {
                LPC_GPIOINT->IO0IntClr = e->pin_mask;
            }
            else {
                LPC_GPIOINT->IO2IntClr = (e->pin_mask >> g_port2_offset);
            }
        }
        e = e->next;
    };

    /* In case interrupt handler not attached correctly, clear all interrupts here */
    if (status) {
        LPC_GPIOINT->IO0IntClr = 0xFFFFFFFF;
        LPC_GPIOINT->IO2IntClr = 0xFFFFFFFF;
    }
}
#ifdef __cplusplus
}
#endif



/**
 * @param pin_number 0-31 for PORT0, and 32-63 for PORT2
 * @param type       The type of the interrupt (rising or falling)
 * @param func       The callback function.
 */
static void eint3_enable(uint8_t pin_num, eint_intr_t type, void_func_t func)
{
    const uint8_t last_pin_num = 63;
    eint3_entry_t *e = NULL;

    if (pin_num <= last_pin_num && NULL != func &&
        NULL != (e = malloc(sizeof(*e)))
       )
    {
        /* Insert new entry at the head of the list */
        e->callback = func;
        e->pin_mask = (UINT64_C(1) << pin_num);
        e->next = gp_eint3_list_head;
        gp_eint3_list_head = e;

        /* PORT0 */
        if (pin_num < g_port2_offset) {
            if (eint_falling_edge == type) LPC_GPIOINT->IO0IntEnF |= e->pin_mask;
            else  LPC_GPIOINT->IO0IntEnR |= e->pin_mask;
        }
        /* PORT2 */
        else {
            if (eint_falling_edge == type) LPC_GPIOINT->IO2IntEnF |= (e->pin_mask >> g_port2_offset);
            else LPC_GPIOINT->IO2IntEnR |= (e->pin_mask >> g_port2_offset);
        }

        /* EINT3 shares pin interrupts with Port0 and Port2 */
        NVIC_EnableIRQ(EINT3_IRQn);
    }
}

void eint3_enable_port0(uint8_t pin_num, eint_intr_t type, void_func_t func)
{
    eint3_enable((pin_num & 0x1F), type, func);
}

void eint3_enable_port2(uint8_t pin_num, eint_intr_t type, void_func_t func)
{
    eint3_enable((pin_num & 0x1F) + g_port2_offset, type, func);
}
