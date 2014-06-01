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

#include "FreeRTOSConfig.h"
#include "LPC17xx.h"


#if (1 == configGENERATE_RUN_TIME_STATS)
void vConfigureTimerForRunTimeStats( void )
{
    LPC_TIM0->TCR = 1;  // Enable Timer
    LPC_TIM0->CTCR = 0; // Increment on PCLK

    // Set CPU / 1 PCLK
    lpc_pclk(pclk_timer0, clkdiv_1);

    /**
     * Enable resolution of 2 uS per timer tick.
     * This provides 2uS * 2^32 = 140 minutes of runtime before counter over-flows
     */
    LPC_TIM0->PR = (sys_get_cpu_clock() * TIMER0_US_PER_TICK) / (1000*1000);
}
unsigned int uxGetTimerForRunTimeStats()
{
    return LPC_TIM0->TC;
}
void resetRunTimeCounter()
{
    LPC_TIM0->TC = 0;
}
#endif
