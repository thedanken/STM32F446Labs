#include "stm32f4xx.h"

void delayMs(int n);

int main(void) {
    RCC->AHB1ENR |=  1;             /* enable GPIOA clock */
		RCC->AHB1ENR |=  4;                 /* enable GPIOC clock */
    
    GPIOA->MODER &= ~0x00000C00;    /* clear pin mode */
    GPIOA->MODER |=  0x00055500;    /* set pin to output mode */
		GPIOA->PUPDR |=  0X00000005;

    while(1) {
        GPIOA->ODR = 0x00000210;  /* turn on LED */
        delayMs(500);
				while((GPIOA->IDR & 0x0001)){}	/*East-West switch pressed*/
        GPIOA->ODR = 0x00000220;  /* turn off LED */
        delayMs(2000);
				GPIOA->ODR = 0x000000C0;  /* turn on LED */
        delayMs(500);
				while((GPIOA->IDR & 0x0002)) {} /*North-South switch pressed*/
        GPIOA->ODR = 0x00000140;  /* turn off LED */
        delayMs(2000);
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
