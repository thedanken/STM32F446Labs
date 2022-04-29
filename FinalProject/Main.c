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
PB0 or PB4		- Pin 7  (a LED) - PB4 died on board, remapped. PB4 still retains output and is nicer to wire.
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
PC-13	- Read Mode Switches

Mode Switches

PC0		-	Pin 1
PC1		- Pin 2
PC2		- Pin 3
PC3		- Pin 4

Delay numbers are hexidecimal.

 */

#include "stm32f4xx.h"
#include "string.h"
#include "stdbool.h"

// Outputs for different lights
#define NSG	0x40
#define NSY	0x20
#define NSR	0x10
#define EWG	0x200
#define EWY	0x100
#define EWR	0x80

#define EWsw 0x01
#define NSsw 0x02

#define RS 0x04     /* PB2 mask for reg select */
#define EN 0x08     /* PB3 mask for enable */

void delayMs(int n);
void LCD_nibble_write(char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(char data);
void LCD_init(void);
void LCD_string(char data[]);
void crosswalk(void);
void PORTS_init(void);
// Traffic light modes
void regLight(void);	// All other traffic light modes
void flashRR(void);
void flashRY(void);	// NSR, EWY
void flashYR(void);	// NSY, EWR

// Global Variables
bool newMode;
int greenDelay;	// how long for a green light (min for rural)
int yellowDelay;	// how long for a yellow light
int redDelay;	// how long for a red light
bool CWset;	// crosswalk flag
int nextMode;	// assigned operating mode
int curMode;	// current operating mode
int curPanel;


int main(void) {
    /* initialize LCD controller */
    LCD_init();
	
    GPIOB->MODER |=  0x00555501;    /* set pin to output mode */
	
		char output[] = "Traffic Light";
    LCD_string(output);
	
		GPIOA->MODER |=  0x00055500;    /* set pin to output mode */
		GPIOA->PUPDR |=  0X00000005;
		newMode = false;
		greenDelay = 1000;
		yellowDelay = 500;
		redDelay = 500;
		CWset = 0;	// crosswalk flag
		curMode = 0x0B;
	/*
		while(1)
		{
        GPIOA->ODR = NSG;
        delayMs(500);
        GPIOA->ODR = NSY;
        delayMs(500);
        GPIOA->ODR = NSR;
        delayMs(500);
        GPIOA->ODR = EWG;
        delayMs(500);
        GPIOA->ODR = EWY;
        delayMs(500);
        GPIOA->ODR = EWR;
        delayMs(500);
		}
		*/
		
    while(1) {
			if((curMode & 0x04))
			{
				if(curMode & 0x01)
				{
					flashRY();
				}
				else if(curMode & 0x02)
				{
					flashRR();
				}
				else
				{
					flashYR();
				}
				
			}
			else
			{
				regLight();
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
// Modes for traffic light patterns
void regLight(void)
{
	if(!(curMode & 0x08))
	{
		// Block cw interrupt here
	}
	while(!(newMode))
	{
		GPIOA->ODR = (NSG | EWR);
        delayMs(greenDelay);
			
				if((curMode & 0x02)&&!(CWset))
				{
					while((GPIOA->IDR & EWsw)){}
				}
				
        GPIOA->ODR = (NSY | EWR);
        delayMs(yellowDelay);
				
        if((curMode & 0x08)||(CWset))
				{
					GPIOA->ODR = (NSR | EWR);
					if(!(CWset))
					{
						delayMs(redDelay);
					}
					else
					{
						crosswalk();
					}
				}
				
				GPIOA->ODR = (NSR | EWG);
        delayMs(greenDelay);
				
				if((curMode & 0x02)&&!(CWset))
				{
					while((GPIOA->IDR & NSsw)) {}
				}
        GPIOA->ODR = (NSR | EWY);
        delayMs(yellowDelay);
				
				if((curMode & 0x08)||(CWset))
				{
					GPIOA->ODR = (NSR | EWR);
					if(!(CWset))
					{
						delayMs(redDelay);
					}
					else
					{
						crosswalk();
					}
				}
	}
	GPIOA->ODR = (NSR | EWR);
	curMode = nextMode;
	// Built-in safety delay, rather than suddenly switching lights.
	delayMs(500);
}

void flashRR(void)
{
	while(!(newMode))
	{
		GPIOA->ODR = NSR;
		delayMs(500);
		GPIOA->ODR = EWR;
		delayMs(500);
	}
	GPIOA->ODR = (NSR | EWR);
	curMode = nextMode;
	delayMs(500);
}
void flashRY(void)
{
	while(!(newMode))
	{
		GPIOA->ODR = NSR;
		delayMs(500);
		GPIOA->ODR = EWY;
		delayMs(500);
	}
	GPIOA->ODR = (NSR | EWR);
	curMode = nextMode;
	delayMs(500);
}
void flashYR(void)
{
	while(!(newMode))
	{
		GPIOA->ODR = NSY;
		delayMs(500);
		GPIOA->ODR = EWR;
		delayMs(500);
	}
	GPIOA->ODR = (NSR | EWR);
	curMode = nextMode;
	delayMs(500);
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

void crosswalk(void)
{
		LCD_string("Crosswalk happened");
		unsigned int bcdo[] = {0x6F1,0x7F1,0x71,0x7D1,0x6D1,0x660,0x4F1,0x5B1,0x60,0x3F1,0x00,0x3F1,0x00,0x3F1,0x00};
		for(int i=0; i<=14;i++)
		{
				GPIOB->ODR = bcdo[i];  /* turn on 7SD */
				delayMs(500);
		}
		LCD_command(1);
		LCD_string("Traffic Light");
}

void LCD_string(char data[]) {
	for(int i=0; i < strlen(data); i++)
	{
		LCD_data(data[i]);
	}
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