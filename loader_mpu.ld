MEMORY
{
    /* Define each memory region */
    MFlash512 (rx) : ORIGIN = 64k, LENGTH = 448k
  
    /* 32k (Heap starts here) */
    RamLoc32 (rwx) : ORIGIN = 0x10000000, LENGTH = 0x8000
  
    /* 32k (Globals at bottom, stack on top), heap continues from here */
    RamAHB32 (rwx) : ORIGIN = 0x2007c000, LENGTH = 0x8000
}

/* Define any symbols */
__top_RamAHB32 = 0x2007c000 + 0x8000;

/* Variables used by FreeRTOS-MPU. */
_Privileged_Functions_Region_Size = 16K;
_Privileged_Data_Region_Size = 256;

__FLASH_segment_start__ = ORIGIN( MFlash512 );
__FLASH_segment_end__ = __FLASH_segment_start__ + LENGTH( MFlash512 );

__privileged_functions_start__ = ORIGIN( MFlash512 );
__privileged_functions_end__ = __privileged_functions_start__ + _Privileged_Functions_Region_Size;

__SRAM_segment_start__ = ORIGIN( RamAHB32 );
__SRAM_segment_end__ = __SRAM_segment_start__ + LENGTH( RamAHB32 );

__privileged_data_start__ = ORIGIN( RamAHB32 );
__privileged_data_end__ = ORIGIN( RamAHB32 ) + _Privileged_Data_Region_Size;    

ENTRY(isr_reset)

SECTIONS
{
    privileged_functions :
    {
        FILL(0xff)
        KEEP(*(.isr_vector))
        *(privileged_functions)
    } > MFlash512

    /* MAIN TEXT SECTION */ 
    .text : ALIGN(4)
    {
        /* Non privileged code kept out of the first 16K or flash. */
        . = __privileged_functions_start__ + _Privileged_Functions_Region_Size;
        
        /* Global Section Table */
        . = ALIGN(4) ;
        __section_table_start = .;
        __data_section_table = .;
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data)) ;
        LONG(  SIZEOF(.data));
        __data_section_table_end = .;
        __bss_section_table = .;
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        __bss_section_table_end = .;
        __section_table_end = . ;
        /* End of Global Section Table */

        *(.after_vectors*)
        
        *(.text*)
        *(.rodata .rodata.*)
        . = ALIGN(4);
        
        /* C++ constructors etc */
        . = ALIGN(4);
        KEEP(*(.init))
        
        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP (*(.preinit_array))
        __preinit_array_end = .;
        
        . = ALIGN(4);
        __init_array_start = .;
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        __init_array_end = .;
        
        KEEP(*(.fini));
        
        . = ALIGN(0x4);
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))
        
        . = ALIGN(0x4);
        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))
        /* End C++ */
        
    } > MFlash512

    /*
     * for exception handling/unwind - some Newlib functions (in common
     * with C++ and STDC++) use this.
     */
    .ARM.extab : ALIGN(4)
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > MFlash512
    __exidx_start = .;
    
    .ARM.exidx : ALIGN(4)
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > MFlash512
    __exidx_end = .;
    
/*
    .debug : ALIGN(4)
    {
        *(.debug* *debug.*)
    } > MFlash512
    
    .stabs : ALIGN(4)
    {
        *(.stabs* *stabs.*)
    } > MFlash512
*/

    _etext = .;
    
    /* zero initialized data */     
    privileged_data :
    {
        _bss = .;
        *(privileged_data)
        /* Non kernel data is kept out of the first 256 bytes of SRAM. */
    } > RamAHB32    

    . = ORIGIN( RamAHB32 ) + _Privileged_Data_Region_Size;
    
    /* MAIN DATA SECTION */
    .uninit_RESERVED : ALIGN(4)
    {
        KEEP(*(.bss.$RESERVED*))
    } > RamAHB32

    .data : ALIGN(4)
    {
        FILL(0xff)
        _data = .;
        *(vtable)
        *(.data*)
        . = ALIGN(4) ;
        _edata = .;
    } > RamAHB32 AT>MFlash512

    /* MAIN BSS SECTION */
    .bss : ALIGN(4)
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(end = .);
    } > RamAHB32
    
    PROVIDE(_pvHeapStart = .);
    PROVIDE(_vStackTop = __top_RamAHB32 - 32); /* Debugger and ISP may use 32-bytes of space? */
}
