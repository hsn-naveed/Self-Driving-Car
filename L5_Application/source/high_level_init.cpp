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

#include <stdio.h>          // printf
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "lpc_sys.h"
#include "sys_config.h"     // CPU Clock Configuration
#include "utilities.h"
#include "chip_info.h"

#include "fat/disk/sd.h"        // Initialize SD Card Pins for CS, WP, and CD
#include "fat/disk/spi_flash.h" // Initialize Flash CS pin

#include "rtc.h"             // RTC init
#include "i2c2.hpp"          // I2C2 init
#include "adc0.h"            // ADC0 init
#include "ssp0.h"            // SPI-0 init
#include "ssp1.h"            // SPI-1 init

#include "storage.hpp"       // Mount Flash & SD Storage
#include "bio.h"             // Init io signals
#include "io.hpp"            // Board IO peripherals

#include "wireless.h"
#include "fault_registers.h"
#include "c_tlm_comp.h"



static void hl_print_line() { puts("------------------------------------------------------"); }
static bool hl_mount_storage(FileSystemObject& drive, const char* pDescStr);
static bool hl_init_board_io(void);
static void hl_wireless_set_addr_from_file(void);
static void hl_handle_board_id(void);
static void hl_show_prog_info(void);



/**
 * Initializes the High Level System such as IO Pins and Drivers
 */
void high_level_init(void)
{
    /**
     * Set-up the timer so that delay_ms(us) functions will work.
     * This timer is also used by FreeRTOS run time statistics.
     * This is needed early since SD card init() also relies on system timer services.
     */
    lpc_sys_setup_system_timer();

    // Initialize all board pins (early) that are connected internally.
    board_io_pins_initialize();
    
    /**
     * Initialize the Peripherals used in the system
     * I2C2 : Used by LED Display, Acceleration Sensor, Temperature Sensor
     * ADC0 : Used by Light Sensor
     * SPI0 : Used by Nordic
     * SPI1 : Used by SD Card & External SPI Flash Memory
     */
    adc0_init();
    ssp1_init();
    ssp0_init(SPI0_CLOCK_SPEED_MHZ);
    if (!I2C2::getInstance().init(I2C2_CLOCK_SPEED_KHZ)) {
        puts("ERROR: Possible short on SDA or SCL wire (I2C2)!");
    }

    #if ENABLE_TELEMETRY
        /* Add default telemetry components */
        tlm_component_add("disk");
        tlm_component_add("debug");
    #endif

    /* Initialize nordic wireless mesh network before setting up sys_setup_rit()
     * callback since it may access NULL function pointers.
     * @warning Need SSP0 init before initializing nordic wireless.
     */
    if (!wireless_init()) {
        puts("ERROR: Failed to initialize wireless");
    }

    /**
     * Intentional delay here because this gives the user some time to
     * close COM Port at Hyperload and Open it at Hercules.
     * Since RIT is setup to reset watchdog, we can delay without watchdog reset.
     */
    delay_ms(STARTUP_DELAY_MS);
    hl_print_line();

    /* Print boot info */
#if USE_REDUCED_PRINTF
    const unsigned int cpuClock = sys_get_cpu_clock();
    const unsigned int sig = cpuClock / (1000 * 1000);
    const unsigned int fraction = (cpuClock - (sig*1000*1000)) / 1000;
    printf("System Boot @ %u.%u Mhz\n", sig, fraction);
#else
    printf("System Boot @ %.3f Mhz\n", sys_get_cpu_clock() / (1000 * 1000.0f));
#endif

    if(boot_watchdog_recover == sys_get_boot_type()) {
        char taskName[sizeof(FAULT_LAST_RUNNING_TASK_NAME) * 2] = { 0 };
        memcpy(&taskName[0], (void*) &(FAULT_LAST_RUNNING_TASK_NAME), sizeof(FAULT_LAST_RUNNING_TASK_NAME));

        hl_print_line();
        printf("System rebooted after crash.  Relevant info:\n"
               "PC: 0x%08X.  LR: 0x%08X.  PSR: 0x%08X\n"
               "Possible last running OS Task: '%s'\n",
                (unsigned int)FAULT_PC, (unsigned int)FAULT_LR, (unsigned int)FAULT_PSR,
                taskName);
        hl_print_line();
        delay_ms(10 * 1000);
    }

    /**
     * If Flash is not mounted, it is probably a new board and the flash is not
     * formatted so format it, alert the user, and try to re-mount it
     */
    if(!hl_mount_storage(Storage::getFlashDrive(), " Flash "))
    {
        printf("FLASH not formatted, formatting now ... ");
        printf("%s\n", FR_OK == Storage::getFlashDrive().format() ? "Done" : "Error");

        if (!hl_mount_storage(Storage::getFlashDrive(), " Flash "))
        {
            printf("SPI FLASH is possibly damaged!\n");
            printf("Page size: %u\n", (unsigned) flash_get_page_size());
            printf("Mem  size: %u (raw bytes)\n", (unsigned) (flash_get_page_count() * flash_get_page_size()));
        }
    }

    hl_mount_storage(Storage::getSDDrive(), "SD Card");

	/* After SD card is initted, set desired speed for spi1 */
    ssp1_set_max_clock(SPI1_CLOCK_SPEED_MHZ);
    hl_print_line();

    #if LOG_BOOT_INFO_TO_FILE
    log_boot_info(__DATE__);
    #endif

    /* Initialize all sensors of this board. */
    if(!hl_init_board_io()) {
        hl_print_line();
        LD.setLeftDigit('-');
        LD.setRightDigit('-');
        LE.setAll(0xFF);
    }
    else {
        LD.setNumber(TS.getFarenheit());
    }

    /* After Flash memory is mounted, try to set node address from a file */
    hl_wireless_set_addr_from_file();
    srand(LS.getRawValue() + time(NULL));

    // Display CPU speed in Mhz on LED display
    // LD.setNumber(sys_get_cpu_clock()/(1000*1000));

    /* Print memory information before we call main() */
    do {
        char buff[512] = { 0 };
        sys_get_mem_info_str(buff);
        printf("%s", buff);
        hl_print_line();
    } while(0);

    hl_handle_board_id();
    hl_show_prog_info();
    hl_print_line();

    puts("Calling your main()");
    hl_print_line();
}

static bool hl_mount_storage(FileSystemObject& drive, const char* pDescStr)
{
    unsigned int totalKb = 0;
    unsigned int availKb = 0;
    const char st = drive.mount();
    bool mounted = (0 == st);

    if(mounted && FR_OK == drive.getDriveInfo(&totalKb, &availKb))
    {
        const char *size = (totalKb < (32*1024)) ? "KB" : "MB";
        unsigned int div = (totalKb < (32*1024)) ? 1 : 1024;

        printf("%s: OK -- Capacity %-5d%s, Available: %-5u%s\n",
               pDescStr, totalKb/div, size, availKb/div, size);
    }
    else {
        printf("%s: Error or not present.  Error #%i, Mounted: %s\n", pDescStr, st, mounted ? "Yes" : "No");
        mounted = false;
    }

    return mounted;
}

static bool hl_init_board_io(void)
{
    bool success = true;

    if(!AS.init()) { puts("ERROR: Acceleration Sensor"); success = false; }
    if(!TS.init()) { puts("ERROR: Temperature Sensor"); success = false; }
    if(!LD.init()) { puts("ERROR: 7-Segment Display"); success = false; }

    /* These devices don't have a way to check if init() failed */
    IS.init(); // IR sensor
    LS.init(); // Light sensor
    LE.init(); // LEDs
    SW.init(); // Switches

    /* Turn off all LEDs */
    LE.setAll(0);

    return success;
}

static void hl_wireless_set_addr_from_file(void)
{
    uint8_t wireless_node_addr = WIRELESS_NODE_ADDR;
    char nAddrStr[8] = { 0 };

    if (FR_OK == Storage::read(WIRELESS_NODE_ADDR_FILE, nAddrStr, sizeof(nAddrStr)-1, 0)) {
        wireless_node_addr = atoi(nAddrStr);
        if (0 == wireless_node_addr || MESH_BROADCAST_ADDR == wireless_node_addr) {
            printf("Invalid node address (%s) specified in %s\n", nAddrStr, WIRELESS_NODE_ADDR_FILE);
        }
        else {
            printf("Wireless node addr set to %i from '%s' file\n",
                    wireless_node_addr, WIRELESS_NODE_ADDR_FILE);
            mesh_set_node_address(wireless_node_addr);
        }
    }
}

static void hl_handle_board_id(void)
{
    const uint8_t buttons_to_program_id = (1 << 3) | (1 << 0);
    const char notProgrammed = 0xFF;
    char board_id_on_spi_flash[128] = { 0 };
    flash_read_permanent_id(board_id_on_spi_flash);

    if (notProgrammed != board_id_on_spi_flash[0]) {
        printf("Board ID is: '%s' (0x%02X)\n", board_id_on_spi_flash, (board_id_on_spi_flash[0] & 0xFF));
    }
    else if (SW.getSwitchValues() == buttons_to_program_id) {
        printf("Enter a board ID (64 chars max): \n");
        scanf("%64s", &board_id_on_spi_flash[0]);

        printf("Board ID to program: '%s'\n", board_id_on_spi_flash);
        printf("Enter 'Y' to confirm.  BOARD ID CANNOT BE CHANGED AND IS PERMANENT\n");

        char confirm[4] = { 0 };
        scanf("%3s", confirm);

        if ('Y' == confirm[0]) {
            flash_write_permanent_id(board_id_on_spi_flash);
        }
        else {
            puts("Board ID not programmed");
        }
    }
    else {
        puts("You can program a PERMANENT ID of your board.");
        puts("To do this, hold SW1 and SW4 and reset the board.");
    }
}

static void hl_show_prog_info(void)
{
    const unsigned int prog_count   = chip_get_prog_count();
    const unsigned int prog_modify  = chip_get_prog_modify_count();
    const unsigned int prog_max_kb  = chip_get_prog_max() / 1024;
    const unsigned int prog_min_kb  = chip_get_prog_min() / 1024;
    const unsigned int prog_time_ms = chip_get_prog_time_ms();

    printf("CPU flash altered/programmed counts: %u/%u\n", prog_modify, prog_count);
    printf("CPU programmed flash (min/max): %uKb - %uKb\n", prog_min_kb, prog_max_kb);
    printf("Last programming took %u ms\n", prog_time_ms);
}
