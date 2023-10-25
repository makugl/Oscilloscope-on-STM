;******************************************************************************
; Exports & Imports
;******************************************************************************

				IMPORT __main [CODE]
				EXPORT Reset_Handler
				EXPORT __Vectors


;******************************************************************************
; Constants
;******************************************************************************

EXT_INT_SIZE    EQU 268   ; 67 Interrupts x 4 Bytes = 268 Bytes
STACK_SIZE      EQU 0x200 ; StackSize (Total RAM 4096 Bytes)


;******************************************************************************
; Data Area: Stack
;******************************************************************************

                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   STACK_SIZE
__initial_sp    ; This label defines the initial stack pointer


;******************************************************************************
; Data Area: Vector Table
;******************************************************************************

                AREA    VectorTable, DATA, READONLY, ALIGN=3
__Vectors
                DCD     __initial_sp                   ; Top of Stack
                DCD     Reset_Handler                  ; Reset Handler
                DCD     NMI_Handler                    ; NMI Handler
                DCD     HardFault_Handler              ; Hard Fault Handler
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     SVC_Handler                    ; SVCall Handler
                DCD     0                              ; Reserved
                DCD     0                              ; Reserved
                DCD     PendSV_Handler                 ; PendSV Handler
                DCD     SysTick_Handler                ; SysTick Handler
                SPACE   EXT_INT_SIZE                   ; External Interrupts


;******************************************************************************
; Code Area: Default Handlers
;******************************************************************************

                AREA    Startup, CODE, READONLY

Reset_Handler   PROC
				ENTRY
                  LDR     R0, =__main
                  BX      R0
                ENDP

HardFault_Handler
SVC_Handler
PendSV_Handler
SysTick_Handler
NMI_Handler     PROC
                 B .
                ENDP


;******************************************************************************
; End of File
;******************************************************************************
				ALIGN
                END