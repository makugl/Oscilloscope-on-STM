;******************************************************************************
; File:      STM32F072VB.s
; Target:    STM32F072VB
; Toolchain: MDK-Lite 5.24.1 / armasm.exe V5.06 update 5 (build 528)
;******************************************************************************
; [RM0091] "RM0091 Reference manual",
; January 2017, DocID018940 Rev 9, www.st.com
;
; [PM0215] "PM0215 Programming manual"
; April 2012, Doc ID 022979 Rev 1, www.st.com
;
; [armasm] "ARM® Compiler armasm User Guide"
; 2010-2016, DUI0473M, ARM Limited
;******************************************************************************
; The boot mode configuration is latched on the 4th rising edge of SYSCLK after
; a reset. After this startup delay has elapsed, the CPU fetches the
; top-of-stack value from address 0x00000000, then starts code execution from
; the boot memory at 0x00000004. The main Flash memory is aliased in the boot
; memory space (0x00000000), but still accessible from its original memory
; space (0x08000000). In other words, the Flash memory contents can be accessed
; starting from address 0x00000000 or 0x08000000.
; [RM0091], 2.5 Boot configuration
;******************************************************************************


;******************************************************************************
; Exports & Imports
;******************************************************************************

                IMPORT __main [CODE]
				EXPORT Reset_Handler
				EXPORT __Vectors
				EXPORT __initial_sp


;******************************************************************************
; Data Area: Vector Table
;******************************************************************************
; The vector table contains the reset value of the stack pointer, and the start
; addresses, also
; called exception vectors, for all exception handlers.
; On system reset, the vector table is fixed at address 0x00000000.
; The least-significant bit of each vector must be 1, indicating that the
; exception handler is Thumb code.
; [PM0215], 2.3.4 Vector table
;
; [RM0091] 11.1.3 Interrupt and exception vectors
;
; The STM32F072VB only supports Thumb(-2) instructions; therefore this is not
; a problem. To be sure we add the THUMB instruction to the implementation of
; exception handlers.
;******************************************************************************

                AREA    VectorTable, DATA, READONLY, ALIGN=2
__Vectors
                DCD     __initial_sp     ; Top of Stack
                DCD     Reset_Handler     ; Reset
                DCD     DefaultHandler   ; NMI
                DCD     DefaultHandler   ; HardFault
                SPACE   7*4              ; Reserved (7x)
                DCD     DefaultHandler   ; SVCall
                SPACE   2*4              ; Reserved (2x)
                DCD     DefaultHandler   ; PendSV
                DCD     DefaultHandler   ; SysTick
                DCD     DefaultHandler   ; WWDG
                DCD     DefaultHandler   ; PVD_VDDIO2
                DCD     DefaultHandler   ; RTC
                DCD     DefaultHandler   ; FLASH
                DCD     DefaultHandler   ; RCC_CRS
                DCD     DefaultHandler   ; EXTI0_1
                DCD     DefaultHandler   ; EXTI2_3
                DCD     DefaultHandler   ; EXTI4_15
                DCD     DefaultHandler   ; TSC
                DCD     DefaultHandler   ; DMA_CH1
                DCD     DefaultHandler   ; DMA_CH2_3, DMA2_CH1_2
                DCD     DefaultHandler   ; DMA_CH4_5_6_7, DMA2_CH3_4_5
                DCD     DefaultHandler   ; ADC_COMP
                DCD     DefaultHandler   ; TIM1_BRK_UP_TRG_COM
                DCD     DefaultHandler   ; TIM1_CC
                DCD     DefaultHandler   ; TIM2
                DCD     DefaultHandler   ; TIM3
                DCD     DefaultHandler   ; TIM6_DAC
                DCD     DefaultHandler   ; TIM7
                DCD     DefaultHandler   ; TIM14
                DCD     DefaultHandler   ; TIM15
                DCD     DefaultHandler   ; TIM16
                DCD     DefaultHandler   ; TIM17
                DCD     DefaultHandler   ; I2C1
                DCD     DefaultHandler   ; I2C2
                DCD     DefaultHandler   ; SPI1
                DCD     DefaultHandler   ; SPI2
                DCD     DefaultHandler   ; USART1
                DCD     DefaultHandler   ; USART2
                DCD     DefaultHandler   ; USART3_4_5_6_7_8
                DCD     DefaultHandler   ; CEC_CAN
                DCD     DefaultHandler   ; USB


;******************************************************************************
; Data Area: Stack
;******************************************************************************
; The processor uses a full descending stack. This means the stack pointer
; indicates the last stacked item on the stack memory. When the processor
; pushes a new item onto the stack, it decrements the stack pointer and then
; writes the item to the new memory location.
; [PM0215], 2.1.2 Stacks
;******************************************************************************

                AREA    Stack, NOINIT, READWRITE, ALIGN=3

                SPACE   4096 ; Stack size in bytes (Total SRAM: 16 kB)
__initial_sp


;******************************************************************************
; Code Area: Exception Handlers
;******************************************************************************

                AREA    ExceptionHandlers, CODE, READONLY


;------------------------------------------------------------------------------
; Reset exception handler implementation.
;------------------------------------------------------------------------------

Reset_Handler    PROC

                  ; The ENTRY directive declares an entry point to a program.
                  ; If the program contains multiple entry points, then you
                  ; must select one of them. You do this by exporting the
                  ; symbol for the ENTRY directive that you want to use as the
                  ; entry point, then using the armlink --entry option to
                  ; select the exported symbol.
                  ENTRY

                  ; Interpret subsequent instructions as Thumb instructions.
                  THUMB

                  ; Branch to the imported address SystemStart
                  ;
                  ; The assembler converts an LDR Rd,=label
                  ; pseudo-instruction by:
                  ; * Placing the address of label in a literal pool (a portion
                  ;   of memory embedded in the code to hold constant values).
                  ; * Generating a PC-relative LDR instruction that reads the
                  ;   address from the literal pool
                  ; [armasm], 4.11 Load addresses to a register using LDR Rd, =label
                  LDR     R0, =__main
                  BX      R0

                  ; Assemble the current literal pool (contains the value
                  ; of SystemStart).
                  LTORG

                ENDP


;------------------------------------------------------------------------------
; Default exception handler implementation.
;------------------------------------------------------------------------------

DefaultHandler  PROC

                  ; Interpret subsequent instructions as Thumb instructions.
                  THUMB

                  ; Branch to current line; inifinite loop.
                  B .

                ENDP


;******************************************************************************
; End of File
;******************************************************************************

                END