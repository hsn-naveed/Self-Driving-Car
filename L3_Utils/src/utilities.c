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
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "utilities.h"
#include "LPC17xx.h"
#include "sys_config.h" // TIMER0_US_PER_TICK
#include "lpc_sys.h"

#include "rtc.h"
#include "ff.h"
#include "fault_registers.h"



/**
 * Delays in microseconds
 * @param delayMicroSec The delay in microseconds
 */
void delay_us(unsigned int microsec)
{
    const unsigned int ticksForThisDelay = microsec / TIMER0_US_PER_TICK;
    const unsigned int targetTick = LPC_TIM0->TC + ticksForThisDelay;

    // Take care of TC overflow case by allowing TC value to go beyond
    // targetTick, and then the next loop will handle the rest.
    if(targetTick < LPC_TIM0->TC) {
        while(LPC_TIM0->TC > targetTick) {
            ;
        }
    }

    // Wait until the TC value reaches targetTick
    while(LPC_TIM0->TC < targetTick) {
        ;
    }
}

/**
 * Delays in milliseconds
 * @param delayMilliSec The delay in milliseconds.
 */
void delay_ms(unsigned int millisec)
{
    /**
     * If FreeRTOS is running, we should sleep the calling task instead
     * of polling using the full CPU.
     */
    if(is_freertos_running())
    {
        vTaskDelay(OS_MS(millisec));
    }
    else
    {
        delay_us(1000 * millisec);
    }
}

char is_freertos_running()
{
    return (taskSCHEDULER_RUNNING == xTaskGetSchedulerState());
}

void log_boot_info(const char *pExtraInfo)
{
    char logMsg[256] = { 0 };

    /* Get timestamp without newline */
    char dateTime[32] = { 0 };
    strcpy(dateTime, rtc_get_date_time_str());
    dateTime[strlen(dateTime)-1] = '\0';

    if(boot_watchdog_recover == sys_get_boot_type()) {
        sprintf(logMsg, "%s: BAD BOOT (%.128s): PC: 0x%08X LR: 0x%08X PSR: 0x%08X\n",
                        dateTime, pExtraInfo,
                        (unsigned int)FAULT_PC, (unsigned int)FAULT_LR, (unsigned int)FAULT_PSR);
    }
    else {
        sprintf(logMsg, "%s: NORMAL BOOT (%.128s)\n", dateTime, pExtraInfo);
    }

    FIL file;
    unsigned int bytesWritten = 0;
    if (FR_OK == f_open(&file, LOG_BOOT_INFO_FILENAME, FA_OPEN_ALWAYS | FA_WRITE))
    {
        if(FR_OK == f_lseek(&file, f_size(&file))) {
            f_write(&file, logMsg, strlen(logMsg), &bytesWritten);
        }
        f_close(&file);
    }

    if(strlen(logMsg) != bytesWritten) {
        puts("Error writing boot info");
    }
}
