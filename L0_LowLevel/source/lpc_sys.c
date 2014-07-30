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

#include <stdio.h>

#include "lpc_sys.h"
#include "wireless.h"
#include "lpc_timers.h"

#include "FreeRTOS.h"
#include "task.h"



/// Periodic interrupt at this value occurs until FreeRTOS starts, and then the frequency is decreased
#define LPC_SYS_TIME_1000US     (1 * 1000)

/// We set this to the initial value, and decrease the frequency later if FreeRTOS starts to run
uint32_t g_periodic_isr_time_value_us = LPC_SYS_TIME_1000US;

/// Timer overflow interrupt will increment this upon the last UINT32 value, 16-bit is enough for many years!
static volatile uint16_t g_timer_rollover_count = 0;

/// Pointer to the timer struct based on g_sys_timer_source
LPC_TIM_TypeDef *gp_timer_ptr = NULL;

/**
 * The timer to use for timer and background services.
 * We can use any timer, but timer1 is required for the IR receiver's signal decoding logic.
 */
static const lpc_timer_t g_sys_timer_source = SYS_CFG_SYS_TIMER;



/**
 * This function is called periodically by the timer ISR
 */
static inline void sys_background_service(void)
{
    /* FreeRTOS task is used to service the wireless_service() function, otherwise if FreeRTOS
     * is not running, timer ISR will call this function to carry out mesh networking logic.
     */
    wireless_service();

    /* I used a small hack to reset base priority to zero periodically incorrectly assuming that
     * FreeRTOS API sets the base priority to non-zero, but it turned out that the problem was
     * that the vPortEnterCritical() and vPortExitCritical() functions were using the critical
     * nesting count variable that was not initialized to zero by the port.c file!
     */
     // __set_BASEPRI(0);
}

void lpc_sys_setup_system_timer(void)
{
    enum {
        mr0_intr = (UINT32_C(1) << 0),
        mr1_intr = (UINT32_C(1) << 3),
        mr2_intr = (UINT32_C(1) << 6),
    };

    // Get the IRQ number of the timer to enable the interrupt
    const IRQn_Type timer_irq = lpc_timer_get_irq_num(g_sys_timer_source);

    // Initialize the timer structure pointer
    gp_timer_ptr = lpc_timer_get_struct(g_sys_timer_source);

    // Setup the timer to tick with a fine-grain resolution
    const uint32_t one_micro_second = 1;
    lpc_timer_enable(g_sys_timer_source, one_micro_second);

    /**
     * MR0: Setup the match register to take care of the overflow.
     * Upon the roll-over, we increment the roll-over count and the timer restarts from zero.
     */
    gp_timer_ptr->MR0 = UINT32_MAX;

    // MR1: Setup the periodic interrupt to do background processing
    gp_timer_ptr->MR1 = g_periodic_isr_time_value_us;

#if (1 == SYS_CFG_SYS_TIMER)
    // MR2: IR code timeout when timer1 is used since IR receiver is tied to timer1 capture pin
    gp_timer_ptr->MR2 = 0;
#else
    #warning "IR receiver will not work unless SYS_CFG_SYS_TIMER uses TIMER1, so set it to 1 if possible"
#endif

    /* Not using this anymore, see the note at sys_background_service() */
    // NVIC_SetPriority(timer_irq, IP_above_freertos);

    // Enable the interrupts
    // TODO Setup capture interrupt for IR receiver
#if (1 == SYS_CFG_SYS_TIMER)
    gp_timer_ptr->MCR = (mr0_intr | mr1_intr | mr2_intr);
#else
    gp_timer_ptr->MCR = (mr0_intr | mr1_intr);
#endif

    NVIC_EnableIRQ(timer_irq);
}

uint64_t sys_get_uptime_us(void)
{
    uint32_t before    = 0;
    uint32_t after     = 0;
    uint32_t rollovers = 0;

    /**
     * Loop until we can safely read both the rollover value and the timer value.
     * When the timer rolls over, the TC value will start from zero, and the "after"
     * value will be less than the before value in which case, we will loop again
     * and pick up the new rollover count.  This avoid critical section and simplifies
     * the logic of reading higher 16-bit (roll-over) and lower 32-bit (timer value).
     */
    do
    {
        before    = gp_timer_ptr->TC;
        rollovers = g_timer_rollover_count;
        after     = gp_timer_ptr->TC;
    } while (after < before);

    // each rollover is 2^32 or UINT32_MAX
    return (((uint64_t)rollovers << 32) + after);
}

/**
 * Actual ISR function (@see startup.cpp)
 */
#if (0 == SYS_CFG_SYS_TIMER)
void TIMER0_IRQHandler()
#elif (1 == SYS_CFG_SYS_TIMER)
void TIMER1_IRQHandler()
#elif (2 == SYS_CFG_SYS_TIMER)
void TIMER2_IRQHandler()
#elif (3 == SYS_CFG_SYS_TIMER)
void TIMER3_IRQHandler()
#else
#error "SYS_CFG_SYS_TIMER must be between 0-3 inclusively"
void TIMERX_BAD_IRQHandler()
#endif
{
    enum {
        timer_mr0_intr   = (1 << 0),
        timer_mr1_intr   = (1 << 1),
        timer_mr2_intr   = (1 << 2),
        timer_mr3_intr   = (1 << 3),
        timer_capt0_intr = (1 << 4),
        timer_capt1_intr = (1 << 5),
    };

    const uint32_t intr_reason = gp_timer_ptr->IR;

#if (1 == SYS_CFG_SYS_TIMER)
    /* TODO: Call capture ISR callback for IR receiver */
    if (intr_reason & timer_capt0_intr)
    {
        gp_timer_ptr->IR = timer_capt0_intr;

        // Setup timeout of IR signal (unless we reset it again)
        gp_timer_ptr->MR2 = 10000 + gp_timer_ptr->TC;
    }
    /* TODO MR2: End of IR capture (no IR capture after initial IR signal) */
    else if (intr_reason & timer_mr2_intr)
    {
        gp_timer_ptr->IR = timer_mr2_intr;
    }
    /* MR0 is used for the timer rollover count */
    else
#endif
    if(intr_reason & timer_mr0_intr)
    {
        gp_timer_ptr->IR = timer_mr0_intr;
        ++g_timer_rollover_count;
    }
    /* MR1 is used as a periodic interrupt to service background tasks and reset watchdog */
    else if(intr_reason & timer_mr1_intr)
    {
        gp_timer_ptr->IR = timer_mr1_intr;

        // Setup next interrupt's match value (rollover is okay)
        gp_timer_ptr->MR1 += g_periodic_isr_time_value_us;

        // Service the background task if FreeRTOS is not running
        if ((taskSCHEDULER_RUNNING != xTaskGetSchedulerState())){
            sys_background_service();
        }
        /**
         * If FreeRTOS is running, then decrease the frequency of the periodic ISR because
         * mesh service is performed by FreeRTOS task, and the only other thing we need to
         * do periodically is the watchdog feed.
         */
        else if (g_periodic_isr_time_value_us != LPC_SYS_TIME_1000US) {
             g_periodic_isr_time_value_us = 1000 * LPC_SYS_TIME_1000US;
        }

        /* If no one feeds watchdog interrupt, we will watchdog reset.  We are using a periodic ISR
         * to feed watchdog because if a critical exception hits, it will enter while(1) loop inside
         * the interrupt, and since watchdog won't reset, it will trigger system reset.
         */
        sys_watchdog_feed();
    }
    else
    {
        // Unexpected interrupt, so stay here to trigger watchdog interrupt
        puts("Unexpected ISR call at lpc_sys.c");
        while (1) {
            ;
        }
    }
}

void sys_get_mem_info_str(char buffer[280])
{
    sys_mem_t info = sys_get_mem_info();
    sprintf(buffer,
            "Memory Information:\n"
            "Global Used   : %5d\n"
            "malloc Used   : %5d\n"
            "malloc Avail. : %5d\n"
            "System Avail. : %5d\n"

            "Next Heap ptr    : 0x%08X\n"
            "Last sbrk() ptr  : 0x%08X\n"
            "Last sbrk() size : %u\n"
            "Num  sbrk() calls: %u\n",

            (int)info.used_global, (int)info.used_heap, (int)info.avail_heap, (int)info.avail_sys,
            (unsigned int)info.next_malloc_ptr,
            (unsigned int)info.last_sbrk_ptr,
            (unsigned int)info.last_sbrk_size,
            (unsigned int)info.num_sbrk_calls);
}
