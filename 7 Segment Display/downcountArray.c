#include "stm32f4xx.h"

void delayMs(int n);

int main(void) {
    RCC->AHB1ENR |=  4;             /* enable GPIOA,GPIOC clock */
    
    GPIOC->MODER =  0x00005555;    /* set pin to output mode */
		unsigned char bcdo[] = {0x6F,0x7F,0x07,0x7D,0x6D,0x66,0x4F,0x5B,0x06,0x3F,0x00,0x3F,0x00,0x3F,0x00};

    while(1) {
			for(int i=0;i<14;i++)
			{
					GPIOC->ODR =  bcdo[i];  /* turn on '9' */
					delayMs(500);
			}
    }
}

/* 16 MHz SYSCLK */
void delayMs(int n) {
    int i;

    /* Configure SysTick */
    SysTick->LOAD = 16000;  /* reload with number of clocks per millisecond */
    SysTick->VAL = 0;       /* clear current value register */
    SysTick->CTRL = 0x5;    /* Enable the timer */

    for(i = 0; i < n; i++) {
        while((SysTick->CTRL & 0x10000) == 0) /* wait until the COUNTFLAG is set */
            { }
    }
    SysTick->CTRL = 0;      /* Stop the timer (Enable = 0) */
}
