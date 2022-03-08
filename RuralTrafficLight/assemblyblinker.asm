;*******************************************************
;Assembly program using Assembly to toggle the green LED
;   on STM32F446RE Nucleo64 board at 1 Hz
;*******************************************************
			; All 5 uncommented top are absolutely needed. Copy-paste to each.
            EXPORT  __Vectors					; Interrupts
            EXPORT  Reset_Handler				; Sub-routine
            AREA    vectors, CODE, READONLY		; Defines vectors area, and that it's read only.
			
			;
__Vectors   DCD     0x10010000  ; 0x20008000 	; Top of Stack
            DCD     Reset_Handler            	; Reset Handler
			; DCD - Define Constant, Double

			; Physical Addresses
			; Clock enabler, enables on ABH1 bus
RCC_AHB1ENR equ 0x40023830
			
			; Address for registers
			; 0x4002XXXX - Peripheral Bus
			
			; 0xXXXXAAXX - Specific to the port being used
			; 0xXXXX00XX - Port A
			; 0xXXXX04XX - Port B
			; 0xXXXX08XX - Port C
			; 0xXXXX0CXX - Port D
			
			; 0xXXXXXXAA - Offset relates to specific register function
GPIOA_MODER equ 0x40020000
			; ODR is used for writing
GPIOA_ODR   equ 0x40020014

			; Defines the program start, and that it's read only.
            AREA    PROG, CODE, READONLY
Reset_Handler				; Reset_Handler MUST BE ALL THE WAY TO THE LEFT
            ldr     r4, =RCC_AHB1ENR    	; enable GPIOA clock
			mov	    r5, #1
            str     r5, [r4]
            
            ldr     r4, =GPIOA_MODER    	; load GPIOA mode register
            ldr     r5, =0x00000400     	; set PA5 as output
            str     r5, [r4]				; write register 5 to GPIOA mode register address stored in register 4

L1          ldr     r4, =GPIOA_ODR
            mov     r5, #0x20     			; turn on LED
            str     r5, [r4]
            
			mov     r0, #1000
            bl      delay					; branch and return to this
            
            ldr     r4, =GPIOA_ODR
            mov     r5, #0x00    			; turn off LED
            str     r5, [r4]
            
			mov     r0, #1000
            bl      delay
            b       L1                 		; loop forever

;delay milliseconds in R0
delay       ldr     r1, =5325
DL1         subs    r1, r1, #1			; subtracting r1 by 1, to count down.
            bne     DL1					; branch if not zero
            subs    r0, r0, #1
            bne     delay				; branch if not zero
            bx      lr					; branch exit subroutine

			end							; stops compilation