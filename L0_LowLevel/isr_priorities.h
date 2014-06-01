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

/**
 * @file
 * @brief Interrupt Priorities used in the project
 */
#ifndef INTR_PRIORITIES_H__
#define INTR_PRIORITIES_H__
#ifdef __cplusplus
extern "C" {
#endif



/**
 * Interrupt Priorities
 *  0 - Highest
 * 31 - Lowest
 *
 * Interrupts used in this project must NOT use higher priority than IP_SYSCALL, so
 * if IP_SYSCALL is set to 5, then interrupt priority must be higher number than 5
 * in order to set it to lower priority.
 *
 * FromISR() FreeRTOS API cannot be called from interrupts that have higher priority
 * than IP_SYSCALL, which means that:
 *  - YOUR ISR must have numerically equal to or HIGHER value than IP_SYSCALL
 *
 * The interrupts that use higher priority than IP_SYSCALL MUST NOT USE FREERTOS API.
 * These higher priority interrupts (even higher than FreeRTOS kernel) can be used
 * to specify "super" priority above everything else in the system.
 */
typedef enum
{
    /**
     * @{ FreeRTOS Interrupt Priorities
     * Be careful changing these because interrupt priorities set below
     * must fall in between these two priorities.
     */
        IP_KERNEL = 31,
        IP_SYSCALL = 5, /* Do not use FreeRTOS API for ISR priorities below this */
    /** @} */

    /**
     * @{ Interrupt Priorities
     * These are used at to set default priorities before main() is called.
     * All interrupts use default priority unless otherwise stated in this enum.
     *
     * If you don't want interrupts to nest, set them to the same priority as IP_DEFAULT
     */
        /* Higher priority interrupts than FreeRTOS (no FreeRTOS API) */

        /**
         * RIT must be higher priority than FreeRTOS because if FreeRTOS is not running
         * and FreeRTOS API is used such as Queues for UART, then it leaves the interrupts
         * disabled for all interrupts with lower priority than IP_SYSCALL.  So this RIT
         * interrupt is used to reset the interrupt mask
         */
        IP_RIT  = IP_SYSCALL - 1,

        /* Suggested interrupt priorities for popular peripherals */
        IP_Default = 20,          /**< Default priority of most interrupts */
        IP_ssp  = IP_Default - 5, /**< SSP can be super fast, so needs higher priority */
        IP_can  = IP_Default - 4, /**< CAN can be fast, so use higher priority than other communication BUSes */
        IP_eint = IP_Default - 3, /**< User probably needs fast EINT than UART or I2C */
        IP_i2c  = IP_Default - 2, /**< I2C set to higher priority than UART */
        IP_uart = IP_Default - 1, /**< UART set to higher priority than default */

        /* Rest of the interrupts probably don't need a fast response so set them
         * to default priority.  You don't want to overcomplicate a system by
         * changing too many priorities unless absolutely needed.
         */
        IP_watchdog = IP_Default,
        IP_timers = IP_Default,
        IP_pwm1 = IP_Default,
        IP_pll = IP_Default,
        IP_spi = IP_Default,
        IP_rtc = IP_Default,
        IP_adc = IP_Default,
        IP_bod = IP_Default,
        IP_usb = IP_Default,
        IP_dma = IP_Default,
        IP_i2s = IP_Default,
        IP_enet = IP_Default,
        IP_mcpwm = IP_Default,
        IP_qei    = IP_Default,
        IP_pll1   = IP_Default,
        IP_usbact = IP_Default,
        IP_canact = IP_Default,
    /** @} */
} intr_priorities_t;



#ifdef __cplusplus
}
#endif
#endif
