;*******************************************************
;Assembly program using Assembly to read pushbutton switch
; (PC13) to control the the green LED (PA5)
;   on STM32F446RE Nucleo64 board 
;*******************************************************

            EXPORT  __Vectors
            EXPORT  Reset_Handler
            AREA    vectors, CODE, READONLY

__Vectors   DCD     0x10010000  ; 0x20008000 	; Top of Stack
            DCD     Reset_Handler            	; Reset Handler 

            AREA    PROG, CODE, READONLY
Reset_Handler
            ldr     r4, = 0x40023830		;RCC_AHB1ENR 
			mov	    r5, #5			; enable GPIOA&C clocks
            str     r5, [r4]
            
            ldr     r4, = 0x40020000 		;GPIOA_MODER
			ldr     r5, = 0x00000000 		; reset port A
			str     r5, [r4]
            ldr     r5, = 0x00000400     	; set PA5 as output
            str     r5, [r4]

			ldr     r4, = 0x40020800 		;GPIOC_MODER    
            ldr     r5, = 0x00000000     	; reset port C and set PC13 as input
            str     r5, [r4]

L1	    	nop
L2          ldr     r4, = 0x40020810		;GPIOC IDR
			ldr     r7, = 0x40020014		;GPIOA ODR
            ldr     r5 , [r4]			; read port C
			ldr     r6, =0x2000 		; createmask for bit 13
            tst	    r5, r6
            bne     led_off

			mov	    r8, #0x20
			str     r8, [r7]			;turn on LED
			b       L1

led_off     mov     r8, #0
			str     r8, [r7]			;turn off LED
			b	    L2
	  
	    end
	    