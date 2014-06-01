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
 * @brief This file provides the configurable parameters for your project.
 */
#ifndef SYSCONFIG_H_
#define SYSCONFIG_H_
#ifdef __cplusplus
extern "C" {
#endif



/** @{ Nordic wireless configuration
 * More settings can be configured at mesh_config.h
 * @warning
 *      The AIR data rate, and channel number must be consistent for your wireless
 *      nodes to talk to each other.  It seems that 2000kbps works better than 250kbps
 *      although slower data rate is supposed to get you higher range.
 *
 * @warning Go to   "PROJECT" --> "Clean"   if you change the settings here.
 */
#define WIRELESS_NODE_ADDR              106    ///< Any value from 1-254
#define WIRELESS_CHANNEL_NUM            2499   ///< 2402 - 2500 to avoid collisions among 2+ mesh networks
#define WIRELESS_AIR_DATARATE_KBPS      2000   ///< Air data rate, can only be 250, 1000, or 2000 kbps
#define WIRELESS_NODE_NAME             "node"  ///< Wireless node name
#define WIRELESS_RX_QUEUE_SIZE          3      ///< Number of payloads we can queue
#define WIRELESS_NODE_ADDR_FILE         "naddr"/**< If this file is present, node address will be set by
                                                     reading ASCII data from this file */
/** @} */



#define TERMINAL_USE_NRF_WIRELESS       0             ///< Terminal command can be sent through nordic wireless
#define TERMINAL_END_CHARS              {3, 3, 4, 4}  ///< The last characters sent after processing a terminal command
#define TERMINAL_USE_CAN_BUS_HANDLER    0             ///< CAN bus terminal command xxx


#define SPI1_CLOCK_SPEED_MHZ            24          ///< Max speed of SPI1 for SD Card and Flash memory
#define SPI0_CLOCK_SPEED_MHZ            8           ///< Nordic wireless requires 1-8Mhz max
#define I2C2_CLOCK_SPEED_KHZ            100         ///< 100Khz is standard I2C speed

#define STARTUP_DELAY_MS                2000        ///< Start-up delay in milliseconds
#define LOG_BOOT_INFO_TO_FILE           0           ///< Log a boot message to "boot.csv" file upon every boot
#define LOG_BOOT_INFO_FILENAME          "boot.csv"  ///< Boot info is stored at this filename
#define LOGGER_FILE_NAME                "log"       ///< Logger file name
#define DISK_TLM_NAME                   "disk"      ///< Filename to save "disk" telemetry variables
#define MAX_FILES_OPENED                3           ///< Maximum files that can be opened at once
#define ENABLE_TELEMETRY                0           ///< Enable telemetry system. C_FILE_IO forced enabled if enabled
#define ENABLE_C_FILE_IO                0           ///< Allow stdio fopen() fclose() to redirect to ff.h



/**
 * @returns actual System clock as calculated from PLL and Oscillator selection
 * @note The DESIRED_CPU_CLOCK macro defines "Desired" CPU clock, and doesn't guarantee
 *          this clock rate.  This function returns actual CPU clock of the system.
 */
unsigned int sys_get_cpu_clock();


/**
 * @{   Select the clock source:
 * - Internal Clock: 4Mhz  1% Tolerance
 * - External Clock: External Crystal
 * - RTC Clock     : 32.768Khz
 *
 * If the RTC clock is chosen as an input, then sys_clock.cpp will use the closest
 * PLL settings to get you the desired clock rate.  Due to PLL calculations, the
 * RTC PLL setting may delay your startup time so be patient.
 * 36864000 (36.864Mhz) is a good frequency to derive from RTC PLL since it
 * offers a perfect UART divider
 */
#define CLOCK_SOURCE_INTERNAL   0   ///< Just a constant, do not change
#define CLOCK_SOURCE_EXTERNAL   1   ///< Just a constant, do not change
#define CLOCK_SOURCE_RTC        2   ///< Just a constant, do not change

/// Select one of the clock sources from the options above
#define CLOCK_SOURCE		    CLOCK_SOURCE_INTERNAL
/** @} */

#define INTERNAL_CLOCK		(4  * 1000 * 1000UL)    ///< Do not change, this is the same on all LPC17XX
#define EXTERNAL_CLOCK      (12 * 1000 * 1000UL)    ///< Change according to your board specification
#define RTC_CLOCK           (32768UL)               ///< Do not change, this is the typical RTC crystal value

#define DESIRED_CPU_CLOCK	(48 * 1000 * 1000UL)    ///< Define the CPU speed you desire, must be between 1-100Mhz
#define FALLBACK_CPU_CLOCK  (24 * 1000 * 1000UL)    ///< Do not change.  This is the fall-back CPU speed if DESIRED_CPU_CLOCK cannot be attained

#define TIMER0_US_PER_TICK  (10)                    ///< Microseconds per tick for Timer0 that is used by FreeRTOS run-time stats

/**
 * Watchdog timeout in milliseconds
 * Value cannot be greater than 1,000,000 which is too large of a value
 * to set for a useful watchdog timer anyway.
 */
#define WATCHDOG_TIMEOUT_MS     (3 * 1000)



/**
 * @{ Set printf & scanf options - Do a clean build after changing this option
 *
 *  - 0 : Full printf from stdio.h --> Supports floating-point, but uses 15K more
 *        flash memory and about 300 bytes more RAM with newlib nano libraries.
 *  - 1 : printf from stdio.h without floating point printf/scanf
 */
#define USE_REDUCED_PRINTF        0     ///< Configure your printf version here
#define UART0_DEFAULT_RATE_BPS    38400 ///< UART0 is configured at this BPS by start-up code - before main()
#define UART0_TXQ_SIZE            1024  ///< UART0 transmit queue size before blocking starts to occur
/** @} */



/**
 * Valid years for RTC.
 * If RTC year is not found to be in between these,
 * RTC will reset to 1/1/2000 00:00:00
 */
#define RTC_VALID_RANGE_YEARS   {2010, 2025}



/**
 * Do not change anything here.
 * Telemetry needs C file I/O so force enable it
 */
#if (ENABLE_TELEMETRY)
#undef ENABLE_C_FILE_IO
#define ENABLE_C_FILE_IO 1
#endif



#ifdef __cplusplus
}
#endif
#endif /* SYSCONFIG_H_ */
