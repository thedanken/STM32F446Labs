            EXPORT  __Vectors					; Interrupts
            EXPORT  Reset_Handler				; Sub-routine
            AREA    vectors, CODE, READONLY		;
			
			;
__Vectors   DCD     0x10010000  ; 0x20008000 	; Top of Stack
            DCD     Reset_Handler            	; Reset Handle
				
RCC_AHB1ENR equ 0x40023830
GPIOA_MODER equ 0x40020000
GPIOA_ODR   equ 0x40020014
			
			AREA    PROG, CODE, READONLY
Reset_Handler				; Reset_Handler MUST BE ALL THE WAY TO THE LEFT
            ldr     r4, =RCC_AHB1ENR    	; enable GPIOA clock
			mov	    r5, #1
            str     r5, [r4]
            
            ldr     r4, =GPIOA_MODER    	; load GPIOA mode register
            ldr     r5, =0x00055500     	; set PA5 as output
            str     r5, [r4]				; write register 5 to GPIOA mode register address stored in register 4

L1			ldr     r4, =GPIOA_ODR
			ldr     r5, =0x0210				; NSG, EWR
            str     r5, [r4]
            
			mov     r0, #5000
            bl      delay					; branch and return to this
            
			ldr     r4, =GPIOA_ODR
            ldr     r5, =0x0220    			; NSY, EWR
            str     r5, [r4]
            
			mov     r0, #2000
            bl      delay
			
			ldr     r4, =GPIOA_ODR
			ldr     r5, =0x00C0				; NSR, EWG
            str     r5, [r4]
            
			mov     r0, #5000
            bl      delay					; branch and return to this
            
			ldr     r4, =GPIOA_ODR
            ldr     r5, =0x0140    			; NSR, EWY
            str     r5, [r4]
            
			mov     r0, #2000
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