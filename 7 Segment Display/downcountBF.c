#include "stm32f4xx.h"

void delayMs(int n);

int main(void) {
    RCC->AHB1ENR |=  4;             /* enable GPIOA,GPIOC clock */
    
    GPIOC->MODER =  0x00005555;    /* set pin to output mode */

    while(1) {
        GPIOC->ODR =  0x6F;  /* turn on '9' */
        delayMs(1000);
        GPIOC->ODR =  0x7F;  /* turn on '8' */
        delayMs(1000);
        GPIOC->ODR =  0x07;  /* turn on '7' */
        delayMs(1000);
        GPIOC->ODR =  0x7D;  /* turn on '6' */
        delayMs(1000);
        GPIOC->ODR =  0x6D;  /* turn on '5' */
        delayMs(1000);
        GPIOC->ODR =  0x66;  /* turn on '4' */
        delayMs(1000);
        GPIOC->ODR =  0x4F;  /* turn on '3' */
        delayMs(1000);
        GPIOC->ODR =  0x5B;  /* turn on '2' */
        delayMs(1000);
        GPIOC->ODR =  0x06;  /* turn on '1' */
        delayMs(1000);
        GPIOC->ODR =  0x3F;  /* turn on '0' */
        delayMs(1000);
        GPIOC->ODR =  0x00;  /* turn off */
        delayMs(1000);
        GPIOC->ODR =  0x3F;  /* turn on '0' */
        delayMs(1000);
        GPIOC->ODR =  0x00;  /* turn off */
        delayMs(1000);
        GPIOC->ODR =  0x3F;  /* turn on '0' */
        delayMs(1000);
        GPIOC->ODR =  0x00;  /* turn off */
        delayMs(1000);
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
