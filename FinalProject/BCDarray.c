#include "stm32f4xx.h"
int main(void) {
    RCC->AHB1ENR |=  5;             /* enable GPIOC clock */
    
    GPIOC->MODER =  0x00005555;    /* set pin to output mode */
		GPIOA->MODER =  0x00000000;
		GPIOC->PUPDR =  0x00550000;		 /* pull up switch pins */
		unsigned char bcdo[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x40,0x40,0x40,0x40,0x40,0x40};
		int g = 1;
    while(g) {
				volatile unsigned int swInput = GPIOC->IDR;
			  swInput = swInput >> 8;
				swInput = (~swInput) & 0x0F;
				GPIOC->ODR = bcdo[swInput];
				
    }
}
