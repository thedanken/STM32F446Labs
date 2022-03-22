            EXPORT  __Vectors					; Interrupts
            EXPORT  Reset_Handler				; Sub-routine
            AREA    vectors, CODE, READONLY		;
			
			;
__Vectors   DCD     0x10010000  ; 0x20008000 	; Top of Stack
            DCD     Reset_Handler            	; Reset Handler
			
RCC_AHB1ENR equ 0x40023830
GPIOA_MODER equ 0x40020000
GPIOC_MODER equ 0x40020800
GPIOC_ODR   equ 0x40020814
GPIOC_IDR	equ 0x40020810
GPIOC_PUPR	equ 0x4002080C
	
			AREA    PROG, CODE, READONLY
Reset_Handler				; Reset_Handler MUST BE ALL THE WAY TO THE LEFT

			ldr 	r4, =RCC_AHB1ENR		;GPIOA, GPIOC clock enable
			mov 	r5, #5
			str		r5, [r4]
			
			ldr		r4, =GPIOC_MODER		; Set proper input, output for GPIOC
			ldr		r5, =0x00005555
			str		r5, [r4]
			
			ldr		r4, =GPIOC_PUPR			; Pulls up switches for BCD conversion
			ldr		r5, =0x00550000
			str		r5, [r4]
			
			ldr		r0, =GPIOC_IDR			; Load addresses for IDR, ODR, seven segment lookup table
			ldr		r1, =GPIOC_ODR
			adr		r2, sevSeg				
			
loop		ldr		r3, [r0]				; Read port C, store in register 3
			ror		r3, r3, #8				; Shift register 3 right eight bits
			mvn		r3, r3
			and		r3, r3, #0xF			; Mask register 3 down to four bits
			
			ldr		r4, [r2,r3]				; Stores data from table at r2 with offset r3 into r4
			strb	r4, [r1]
			
			b		loop
			align
sevSeg		DCB		0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x40,0x40,0x40,0x40,0x40,0x40

			end