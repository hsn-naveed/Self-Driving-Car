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

#include <stdio.h>           // printf()

#include "LPC17xx.h"
#include "sys_config.h"
#include "uart0_min.h"
#include "printf_lib.h"      // u0_dbg_put()

#include "lpc_sys.h"
#include "fault_registers.h"
#include "utilities.h"



#define WEAK __attribute__ ((weak))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))

#if defined (__cplusplus)
extern "C"
{
// The entry point for the C++ library startup
extern void __libc_init_array(void);
#endif

//*****************************************************************************
//
// Forward declaration of the default handlers. These are aliased.
// When the application defines a handler (with the same name), this will
// automatically take precedence over these weak definitions
//
//*****************************************************************************
void ResetISR(void);
WEAK void NMI_Handler(void);
WEAK void HardFault_Handler(void);
WEAK void MemManage_Handler(void);
WEAK void BusFault_Handler(void);
WEAK void UsageFault_Handler(void);
WEAK void SVC_Handler(void);
WEAK void DebugMon_Handler(void);
WEAK void PendSV_Handler(void);
WEAK void SysTick_Handler(void);
WEAK void IntDefaultHandler(void);

/**
 * Unwind works but not inside an ISR :(
 */
#define USE_EXPERIMENTAL_UNWIND 0
#if (1 == USE_EXPERIMENTAL_UNWIND)
#include <unwind.h> // GCC's internal unwinder, part of libgcc
_Unwind_Reason_Code trace_fcn(_Unwind_Context *ctx, void *d)
{
    int *depth = (int*)d;
    printf("%x,", _Unwind_GetIP(ctx));
    (*depth)++;
    return _URC_NO_REASON;
}
#endif

/**
 * HardFaultHandler_C:
 * This is called from the HardFault_HandlerAsm with a pointer the Fault stack
 * as the parameter. We can then read the values from the stack and place them
 * into local variables for ease of reading.
 * We then read the various Fault Status and Address Registers to help decode
 * cause of the fault.
 * The function ends with a BKPT instruction to force control back into the debugger
 */
void HardFault_HandlerC(unsigned long *hardfault_args){
        volatile unsigned int stacked_r0 ;
        volatile unsigned int stacked_r1 ;
        volatile unsigned int stacked_r2 ;
        volatile unsigned int stacked_r3 ;
        volatile unsigned int stacked_r12 ;
        volatile unsigned int stacked_lr ;
        volatile unsigned int stacked_pc ;
        volatile unsigned int stacked_psr ;

        stacked_r0 = ((unsigned long)hardfault_args[0]) ;
        stacked_r1 = ((unsigned long)hardfault_args[1]) ;
        stacked_r2 = ((unsigned long)hardfault_args[2]) ;
        stacked_r3 = ((unsigned long)hardfault_args[3]) ;
        stacked_r12 = ((unsigned long)hardfault_args[4]) ;
        stacked_lr = ((unsigned long)hardfault_args[5]) ;
        stacked_pc = ((unsigned long)hardfault_args[6]) ;
        stacked_psr = ((unsigned long)hardfault_args[7]) ;

        FAULT_EXISTS = FAULT_PRESENT_VAL;
        FAULT_PC = stacked_pc;
        FAULT_LR = stacked_lr - 1;
        FAULT_PSR = stacked_psr;

        sys_set_outchar_func(uart0_putchar);
        puts("\nSystem Crash!");
        printf("Registers: 0x%08X, 0x%08X, 0x%08X, 0x%08X, 0x%08X\n",
                stacked_r0, stacked_r1, stacked_r2, stacked_r3, stacked_r12);
        printf("LR: 0x%08X, PC: 0x%08X, PSR: 0x%08X\n", stacked_lr, stacked_pc, stacked_psr);

        delay_us(3 * 1000 * 1000);
        sys_reboot();

        #if (1 == USE_EXPERIMENTAL_UNWIND)
            int depth=0;
            printf("Instructions: ");
            _Unwind_Backtrace(&trace_fcn, &depth);
        #endif

        while(1);
}

//*****************************************************************************
//
// Forward declaration of the specific IRQ handlers. These are aliased
// to the IntDefaultHandler, which is a 'forever' loop. When the application
// defines a handler (with the same name), that will automatically take
// precedence over these weak definitions
//
//*****************************************************************************
void WDT_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIMER0_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIMER1_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIMER2_IRQHandler(void) ALIAS(IntDefaultHandler);
void TIMER3_IRQHandler(void) ALIAS(IntDefaultHandler);
void UART0_IRQHandler(void) ALIAS(IntDefaultHandler);
void UART1_IRQHandler(void) ALIAS(IntDefaultHandler);
void UART2_IRQHandler(void) ALIAS(IntDefaultHandler);
void UART3_IRQHandler(void) ALIAS(IntDefaultHandler);
void PWM1_IRQHandler(void) ALIAS(IntDefaultHandler);
void I2C0_IRQHandler(void) ALIAS(IntDefaultHandler);
void I2C1_IRQHandler(void) ALIAS(IntDefaultHandler);
void I2C2_IRQHandler(void) ALIAS(IntDefaultHandler);
void SPI_IRQHandler(void) ALIAS(IntDefaultHandler);
void SSP0_IRQHandler(void) ALIAS(IntDefaultHandler);
void SSP1_IRQHandler(void) ALIAS(IntDefaultHandler);
void PLL0_IRQHandler(void) ALIAS(IntDefaultHandler);
void RTC_IRQHandler(void) ALIAS(IntDefaultHandler);
void EINT0_IRQHandler(void) ALIAS(IntDefaultHandler);
void EINT1_IRQHandler(void) ALIAS(IntDefaultHandler);
void EINT2_IRQHandler(void) ALIAS(IntDefaultHandler);
void EINT3_IRQHandler(void) ALIAS(IntDefaultHandler);
void ADC_IRQHandler(void) ALIAS(IntDefaultHandler);
void BOD_IRQHandler(void) ALIAS(IntDefaultHandler);
void USB_IRQHandler(void) ALIAS(IntDefaultHandler);
void CAN_IRQHandler(void) ALIAS(IntDefaultHandler);
void DMA_IRQHandler(void) ALIAS(IntDefaultHandler);
void I2S_IRQHandler(void) ALIAS(IntDefaultHandler);
void ENET_IRQHandler(void) ALIAS(IntDefaultHandler);
void RIT_IRQHandler(void) ALIAS(IntDefaultHandler);
void MCPWM_IRQHandler(void) ALIAS(IntDefaultHandler);
void QEI_IRQHandler(void) ALIAS(IntDefaultHandler);
void PLL1_IRQHandler(void) ALIAS(IntDefaultHandler);
void USBActivity_IRQHandler(void) ALIAS(IntDefaultHandler);
void CANActivity_IRQHandler(void) ALIAS(IntDefaultHandler);

/**
 * FreeRTOS Interrupt Handlers
 */
extern void xPortSysTickHandler(void);
extern void xPortPendSVHandler(void);
extern void vPortSVCHandler(void);

/**
 * Stack top is defined by the Linker Script (loader.ld) which is set at
 * g_pfnVectors[] when CPU starts
 */
extern void _vStackTop(void);

#if defined (__cplusplus)
} // extern "C"
#endif

//*****************************************************************************
//
// The vector table.
// This relies on the linker script to place at correct location in memory.
//
//*****************************************************************************
extern void (* const g_pfnVectors[])(void);
__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) =
{
    // Core Level - CM3
        &_vStackTop,        // The initial stack pointer
        ResetISR,           // The reset handler
        NMI_Handler,        // The NMI handler
        HardFault_Handler,  // The hard fault handler
        MemManage_Handler,  // The MPU fault handler
        BusFault_Handler,   // The bus fault handler
        UsageFault_Handler, // The usage fault handler
        0,                  // Reserved
        0,                  // Reserved
        0,                  // Reserved
        0,                  // Reserved
        vPortSVCHandler,    // SVCall handler
        DebugMon_Handler,   // Debug monitor handler
        0,                  // Reserved
        xPortPendSVHandler, // The PendSV handler
        xPortSysTickHandler,// The SysTick handler

        // Chip Level - LPC17
        WDT_IRQHandler,         // 16, 0x40 - WDT
        TIMER0_IRQHandler,      // 17, 0x44 - TIMER0
        TIMER1_IRQHandler,      // 18, 0x48 - TIMER1
        TIMER2_IRQHandler,      // 19, 0x4c - TIMER2
        TIMER3_IRQHandler,      // 20, 0x50 - TIMER3
        UART0_IRQHandler,       // 21, 0x54 - UART0
        UART1_IRQHandler,       // 22, 0x58 - UART1
        UART2_IRQHandler,       // 23, 0x5c - UART2
        UART3_IRQHandler,       // 24, 0x60 - UART3
        PWM1_IRQHandler,        // 25, 0x64 - PWM1
        I2C0_IRQHandler,        // 26, 0x68 - I2C0
        I2C1_IRQHandler,        // 27, 0x6c - I2C1
        I2C2_IRQHandler,        // 28, 0x70 - I2C2
        SPI_IRQHandler,         // 29, 0x74 - SPI
        SSP0_IRQHandler,        // 30, 0x78 - SSP0
        SSP1_IRQHandler,        // 31, 0x7c - SSP1
        PLL0_IRQHandler,        // 32, 0x80 - PLL0 (Main PLL)
        RTC_IRQHandler,         // 33, 0x84 - RTC
        EINT0_IRQHandler,       // 34, 0x88 - EINT0
        EINT1_IRQHandler,       // 35, 0x8c - EINT1
        EINT2_IRQHandler,       // 36, 0x90 - EINT2
        EINT3_IRQHandler,       // 37, 0x94 - EINT3
        ADC_IRQHandler,         // 38, 0x98 - ADC
        BOD_IRQHandler,         // 39, 0x9c - BOD
        USB_IRQHandler,         // 40, 0xA0 - USB
        CAN_IRQHandler,         // 41, 0xa4 - CAN
        DMA_IRQHandler,         // 42, 0xa8 - GP DMA
        I2S_IRQHandler,         // 43, 0xac - I2S
        ENET_IRQHandler,        // 44, 0xb0 - Ethernet
        RIT_IRQHandler,         // 45, 0xb4 - RITINT
        MCPWM_IRQHandler,       // 46, 0xb8 - Motor Control PWM
        QEI_IRQHandler,         // 47, 0xbc - Quadrature Encoder
        PLL1_IRQHandler,        // 48, 0xc0 - PLL1 (USB PLL)
        USBActivity_IRQHandler, // 49, 0xc4 - USB Activity interrupt to wakeup
        CANActivity_IRQHandler, // 50, 0xc8 - CAN Activity interrupt to wakeup
    };

//*****************************************************************************
// Functions to carry out the initialization of RW and BSS data sections. These
// are written as separate functions rather than being inlined within the
// ResetISR() function in order to cope with MCUs with multiple banks of
// memory.
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void data_init(unsigned int romstart, unsigned int start, unsigned int len)
{
    unsigned int *pulDest = (unsigned int*) start;
    unsigned int *pulSrc = (unsigned int*) romstart;
    unsigned int loop;
    for (loop = 0; loop < len; loop = loop + 4)
        *pulDest++ = *pulSrc++;
}

__attribute__ ((section(".after_vectors")))
void bss_init(unsigned int start, unsigned int len)
{
    unsigned int *pulDest = (unsigned int*) start;
    unsigned int loop;
    for (loop = 0; loop < len; loop = loop + 4)
        *pulDest++ = 0;
}


//*****************************************************************************
extern unsigned int __data_section_table;
extern unsigned int __data_section_table_end;
//extern unsigned int __bss_section_table;
extern unsigned int __bss_section_table_end;



//*****************************************************************************
// Code Entry Point : The CPU RESET Handler
// This sets up the system and copies global memory contensts from FLASH to RAM
// and initializes C/C++ environment
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void ResetISR(void)
{
    // remove warning
    (void)g_pfnVectors;

    /**
     * The hyperload bootloader sets the MSP/PSP upon a true reset, which is when the
     * LPC17xx (Cortex-M3) sets the values of the stack pointer.  But since we are
     * booting after a bootloader, we have to manually setup the stack pointers ourselves.
     */
    do {
        const uint32_t topOfStack = (uint32_t) &_vStackTop;
        __set_PSP(topOfStack);
        __set_MSP(topOfStack);
    } while(0);

    do {
    // Copy data from FLASH to RAM
    unsigned int LoadAddr, ExeAddr, SectionLen;
    unsigned int *SectionTableAddr;

    // Load base address of Global Section Table
    SectionTableAddr = &__data_section_table;

    // Copy the data sections from flash to SRAM.
    while (SectionTableAddr < &__data_section_table_end)
    {
        LoadAddr = *SectionTableAddr++;
        ExeAddr = *SectionTableAddr++;
        SectionLen = *SectionTableAddr++;
        data_init(LoadAddr, ExeAddr, SectionLen);
    }
    // At this point, SectionTableAddr = &__bss_section_table;
    // Zero fill the bss segment
    while (SectionTableAddr < &__bss_section_table_end)
    {
        ExeAddr = *SectionTableAddr++;
        SectionLen = *SectionTableAddr++;
        bss_init(ExeAddr, SectionLen);
    }
    } while (0) ;

#if defined (__cplusplus)
    // Call C++ library initialization
    __libc_init_array();
#endif

    // Functions defined externally to this file:
    extern void low_level_init(void);
    extern void high_level_init(void);
    extern int main();

    low_level_init();   // Initialize minimal system, such as Clock & UART
    high_level_init();  // Initialize any user desired items
    main();             // Finally call main()

    // In case main() exits:
    uart0_init(UART0_DEFAULT_RATE_BPS);
    u0_dbg_put("main() should never exit on this system\n");
    while (1) {
        ;
    }
}

//*****************************************************************************
// Default exception handlers. Override the ones here by defining your own
// handler routines in your application code.
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void NMI_Handler(void)
{
    u0_dbg_put("NMI Fault\n");
    while (1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void HardFault_Handler(void)
{
    __asm("MOVS   R0, #4  \n"
            "MOV    R1, LR  \n"
            "TST    R0, R1  \n"
            "BEQ    _MSP    \n"
            "MRS    R0, PSP \n"
            "B      HardFault_HandlerC      \n"
            "_MSP:  \n"
            "MRS    R0, MSP \n"
            "B      HardFault_HandlerC      \n"
    ) ;
}
__attribute__ ((section(".after_vectors")))
void MemManage_Handler(void)
{
    u0_dbg_put("Mem Fault\n");
    while (1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void BusFault_Handler(void)
{
    u0_dbg_put("BUS Fault\n");
    while (1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void UsageFault_Handler(void)
{
    u0_dbg_put("Usage Fault\n");
    while (1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void SVC_Handler(void)
{
    u0_dbg_put("SVC Fault\n");
    while (1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void DebugMon_Handler(void)
{
    u0_dbg_put("DebugMon Fault\n");
    while (1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void PendSV_Handler(void)
{
    u0_dbg_put("Pend SV Fault\n");
    while (1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void SysTick_Handler(void)
{
    u0_dbg_put("Sys Tick Fault\n");
    while (1)
    {
    }
}

//*****************************************************************************
//
// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
//
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void IntDefaultHandler(void)
{
    u0_dbg_put("Unexpected ISR Fault\n");
    while (1)
    {
    }
}
