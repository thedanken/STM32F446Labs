/* 
LCD Required Wires:
PB2			-	RS on LCD	(Pin 4)
PB3			-	EN on LCD	(Pin 6)
PC4-PC7	- D4-D7 on LCD	(Pins 10-14)
3.3V		- B+ on LCD	(Pin 15)
5V			- Pin 2 on LCD
GND			- Pin 1 on LCD, Pin 5 on LCD, Pin 16/17/18 on LCD (RGB LED)
VarResistor Middle Terminal - Pin 2 on LCD

7 Segment Display Required Wires:
PB4		- Pin 7  (a LED)
PB5		- Pin 6  (b LED)
PB6		- Pin 4  (c LED)
PB7		- Pin 2  (d LED)
PB8		- Pin 1  (e LED)
PB9		- Pin 9  (f LED)
PB10	- Pin 10 (g LED)
220 Ohm Resistor on all above
GND		- Pin 3, 8 (common ground)

LED Required Wires:
PA4		- NSR
PA5		- NSY
PA6		- NSG
PA7		- EWR
PA8		- EWY
PA9		- EWG
220 Ohm Resistor on all above
Wire other side to GND

Switch Required Wires:
PA0		-	EW Sw
PA1		-	NS Sw
PB0		- Pedestrian

Keypad Required Wires:

Row
PC0		-	Pin 5
PC1		- Pin 6
PC2		- Pin 7
PC3		- Pin 8

Column
PC8		- Pin 1
PC9		-	Pin 2
PC10	- Pin 3
PC11	- Pin 4

 1 2 3 A
 4 5 6 B
 7 8 9 C
 * 0 # D

 */

#include "stm32f4xx.h"

#define RS 0x04     /* PB2 mask for reg select */
#define EN 0x08     /* PB3 mask for enable */

void delayMs(int n);
void LCD_nibble_write(char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(char data);
void LCD_init(void);
void PORTS_init(void);


int main(void) {
    /* initialize LCD controller */
    LCD_init();
	
    LCD_data('T');
    LCD_data('r');
    LCD_data('a');
    LCD_data('f');
    LCD_data('f');
    LCD_data('i');
    LCD_data('c');
    LCD_data(' ');
    LCD_data('L');
    LCD_data('i');
    LCD_data('g');
    LCD_data('h');
    LCD_data('t');
	
		GPIOA->MODER |=  0x00055500;    /* set pin to output mode */
		GPIOA->PUPDR |=  0X00000005;
	
		int rrstate = 1;	// 6 State value
		int rural = 1;	// Road switches

    while(1) {
        GPIOA->ODR = 0x00000210;  /* NSG EWR */
        delayMs(500);
			
				if(rural == 1)
				{
					while((GPIOA->IDR & 0x0001)){}	/*EW switch pressed*/
				}
				
        GPIOA->ODR = 0x00000220;  /* NSY EWR */
        delayMs(2000);
				
        if(rrstate == 1)
				{
					GPIOA->ODR = 0x00000050;  /* NSY EWR */
					delayMs(2000);
				}
				
				GPIOA->ODR = 0x000000C0;  /* NSR EWG*/
        delayMs(500);
				
				if(rural == 1)
				{
					while((GPIOA->IDR & 0x0002)) {} /*North-South switch pressed*/
				}
        GPIOA->ODR = 0x00000140;  /* NSR EWY */
        delayMs(2000);
				
				if(rrstate == 1)
				{
					GPIOA->ODR = 0x00000050;  /* NSR EWR */
					delayMs(2000);
				}
    }
		/*
    while(1) {
        LCD_data('I');
        LCD_data(' ');
        LCD_data('M');
        LCD_data('O');
        LCD_data('V');
        LCD_data('E');
        LCD_data('D');
        LCD_data(' ');
        LCD_data('R');
        LCD_data('S');
        LCD_data(' ');
        LCD_data('&');
        LCD_data(' ');
        LCD_data('E');
        LCD_data('N');
        delayMs(1000);

        // LCD Clear
        LCD_command(1);
        delayMs(1000);
    }
		*/
}

/* initialize GPIOB/C then initialize LCD controller */
void LCD_init(void) {
    PORTS_init();

    delayMs(20);                /* LCD controller reset sequence */
    LCD_nibble_write(0x30, 0);
    delayMs(5);
    LCD_nibble_write(0x30, 0);
    delayMs(1);
    LCD_nibble_write(0x30, 0);
    delayMs(1);

    LCD_nibble_write(0x20, 0);  /* use 4-bit data mode */
    delayMs(1);
    LCD_command(0x28);          /* set 4-bit data, 2-line, 5x7 font */
    LCD_command(0x06);          /* move cursor right */
    LCD_command(0x01);          /* clear screen, move cursor to home */
    LCD_command(0x0F);          /* turn on display, cursor blinking */
}

void PORTS_init(void) {
    RCC->AHB1ENR |=  0x07;          /* enable GPIOB/C clock */

    /* PORTB 2 for LCD R/S */
    /* PORTB 3 for LCD EN */
    GPIOB->MODER &= ~0x000000A0;    /* clear pin mode */
    GPIOB->MODER |=  0x00000050;    /* set pin output mode */
    GPIOB->BSRR = 0x00080000;       /* turn off EN */

    /* PC4-PC7 for LCD D4-D7, respectively. */
    GPIOC->MODER &= ~0x0000FF00;    /* clear pin mode */
    GPIOC->MODER |=  0x00005500;    /* set pin output mode */
}

void LCD_nibble_write(char data, unsigned char control) {
    /* populate data bits */
    GPIOC->BSRR = 0x00F00000;       /* clear data bits */
    GPIOC->BSRR = data & 0xF0;      /* set data bits */

    /* set R/S bit */
    if (control & RS)
        GPIOB->BSRR = RS;
    else
        GPIOB->BSRR = RS << 16;

    /* pulse E */
    GPIOB->BSRR = EN;
    delayMs(0);
    GPIOB->BSRR = EN << 16;
}

void LCD_command(unsigned char command) {
    LCD_nibble_write(command & 0xF0, 0);    /* upper nibble first */
    LCD_nibble_write(command << 4, 0);      /* then lower nibble */

    if (command < 4)
        delayMs(2);         /* command 1 and 2 needs up to 1.64ms */
    else
        delayMs(1);         /* all others 40 us */
}

void LCD_data(char data) {
    LCD_nibble_write(data & 0xF0, RS);      /* upper nibble first */
    LCD_nibble_write(data << 4, RS);        /* then lower nibble */

    delayMs(1);
}

/* delay n milliseconds (16 MHz CPU clock) */
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