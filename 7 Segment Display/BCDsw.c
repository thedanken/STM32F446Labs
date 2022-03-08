#include "stm32f4xx.h"
int main(void) {
    RCC->AHB1ENR |=  5;             /* enable GPIOA,GPIOC clock */
    
    GPIOC->MODER =  0x00005555;    /* set pin to output mode */
		GPIOA->MODER =  0x00000000;
		GPIOC->PUPDR =  0x00550000;

    while(1) {
				volatile unsigned int swInput = (GPIOC->IDR & 0x0000F00);
			  switch(swInput)
				{
					case 0x00000000:
						GPIOC->ODR =  0x3F;  /* turn on '0' */
						break;
					
					case 0x00000100:
						GPIOC->ODR =  0x06;  /* turn on '1' */
						break;
					
					case 0x00000200:
						GPIOC->ODR =  0x5B;  /* turn on '2' */
						break;
					
					case 0x00000300:
						GPIOC->ODR =  0x4F;  /* turn on '3' */
						break;
				
					case 0x00000400:
						GPIOC->ODR =  0x66;  /* turn on '4' */
						break;
					
					case 0x00000500:
						GPIOC->ODR =  0x6D;  /* turn on '5' */
						break;
					
					case 0x00000600:
						GPIOC->ODR =  0x7D;  /* turn on '6' */
						break;
				
					case 0x00000700:
						GPIOC->ODR =  0x07;  /* turn on '7' */
						break;
				
					case 0x00000800:
						GPIOC->ODR =  0x7F;  /* turn on '8' */
						break;
				
					case 0x00000900:
						GPIOC->ODR =  0x6F;  /* turn on '9' */
						break;
					
					default:
						GPIOC->ODR =  0x00;  /* turn off */
				}
    }
}
