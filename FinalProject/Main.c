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
PA12, PB0, PB12 or PB4		- Pin 7  (a LED) - PB4 died on board, remapped. PB4 still retains output and is nicer to wire.
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
#define greenDelay	2000  // how long for a green light (min for rural)
#define ryDelay 		1000	// how long for a red and yellow light

void delayMs(int n);
void LCD_nibble_write(char data, unsigned char control);
void LCD_command(unsigned char command);
void LCD_data(char data);
void LCD_init(void);
void LCD_string(char data[]);
void panelInput(void);
void PORTS_init(void);
// Traffic light modes
void Light(void);	
void flash(void);
void flashYR(void);
void flashRY(void);

// Global Variables
bool CWset;	// crosswalk flag
bool newMode = false;
unsigned volatile int nextMode;	// assigned operating mode
unsigned int curMode;	// current operating mode
static unsigned int bcdo[] = {0x6F1,0x7F1,0x71,0x7D1,0x6D1,0x660,0x4F1,0x5B1,0x60,0x3F1,0x00,0x3F1,0x00,0x3F1,0x00};
static unsigned int bcdo1[] = {0x1000,0x1000,0x1000,0x1000,0x1000, 0,0x1000,0x1000, 0,0x1000, 0,0x1000, 0,0x1000, 0};
static char modeName[12][21] = { 
	"Current: Urban      ",
	"Current: Urban X    ",
	"Current: Rural      ",
	"Current: Rural X    ",
	"Current: Flash NSY  ",
	"Current: Flash EWY  ",
	"Current: Flash Red  ",
	"",
	"Current: Red Urban  ",
	"Current: Red Urban X",
	"Current: Red Rural X",
	"Current: Red Rural X"
};
static char time[16][3]= {
	"0.1",
	"0.2",
	"0.3",
	"0.4",
	"0.5",
	"0.6",
	"0.7",
	"0.8",
	"0.9",
	"1.0",
	"1.1",
	"1.2",
	"1.3",
	"1.4",
	"1.5",
	"1.6"
};


int main(void) {
    /* initialize LCD controller */
    LCD_init();
	
    GPIOB->MODER |=  0x00555500;    /* set pin to output mode */
	
		GPIOA->MODER |=  0x01055500;    /* set pin to output mode */
		GPIOC->PUPDR |=	 0x00000055;
		GPIOA->PUPDR |=  0x00000005;
		GPIOB->PUPDR |=  0x00000001;
	
		__disable_irq();                    /* global disable IRQs */

    RCC->APB2ENR |= 0x4000;             /* enable SYSCFG clock */

    /* configure PC13, PB0 for push button interrupt */
    GPIOC->MODER &= ~0x0C000000;        /* clear pin mode to input mode */
    
    SYSCFG->EXTICR[3] &= ~0x00F0;       /* clear port selection for EXTI13 */
    SYSCFG->EXTICR[3] |= 0x0020;        /* select port C for EXTI13 */

    SYSCFG->EXTICR[0] &= ~0x000F;       // clear port selection for EXTI0
    SYSCFG->EXTICR[0] |= 0x0001;        // select port B for EXTI0

    EXTI->IMR |= 0x2001;                /* unmask EXTI13 */
    EXTI->FTSR |= 0x2001;               /* select falling edge trigger */

//    NVIC->ISER[1] = 0x00000100;         /* enable IRQ40 (bit 8 of ISER[1]) */
    NVIC_EnableIRQ(EXTI15_10_IRQn);
    NVIC_EnableIRQ(EXTI0_IRQn);	
    
    __enable_irq();                     /* global enable IRQs */
	
		curMode = 0x04;
		
    while(1) {
			LCD_command(1);
			delayMs(10);
			LCD_string(modeName[curMode]);
			delayMs(10);
			if((curMode & 0x04))
			{
				if(curMode & 0x01)
				{
					flashRY();
				}
				else if(curMode & 0x02)
				{
					flash();
				}
				else
				{
					flashYR();
				}
			}
			else
			{
				Light();
			}
    }
}
void EXTI15_10_IRQHandler(void) {
	delayMs(250);
	CWset = true;
	SysTick->CTRL= 0x05;
	EXTI->PR = 0x2000;
}

void EXTI0_IRQHandler(void) {
	delayMs(250);
	nextMode = (~(GPIOC->IDR) & 0x0F);
	LCD_string(modeName[nextMode]);
	newMode = true;
	SysTick->CTRL= 0x05;
	EXTI->PR = 0x0001;
}

// Modes for traffic light patterns
void Light(void)
{
	newMode = false;
	while(!(newMode))
	{
				GPIOA->ODR = (NSG | EWR);
        delayMs(greenDelay);
				if(curMode & 0x02){
					while((GPIOA->IDR & EWsw) && !(CWset)) {}
				}
        GPIOA->ODR = (NSY | EWR);
        delayMs(ryDelay);
				
        if((curMode & 0x08)||(CWset))
				{
					GPIOA->ODR = (NSR | EWR);
					if(!(CWset))
					{
						delayMs(ryDelay);
					}
					else
					{
						for(int i=0; i<=14;i++)
						{
							GPIOB->ODR = bcdo[i];  /* turn on 7SD */
							GPIOA->ODR = bcdo1[i];
							delayMs(1000);
						}
					}
					CWset = false;
				}
				
				GPIOA->ODR = (NSR | EWG);
        delayMs(greenDelay);
				
				if(curMode & 0x02){
					while((GPIOA->IDR & NSsw) && !(CWset)) {}
				}
        GPIOA->ODR = (NSR | EWY);
        delayMs(ryDelay);
				
				if((curMode & 0x08)||(CWset))
				{
					GPIOA->ODR = (NSR | EWR);
					if(!(CWset))
					{
						delayMs(ryDelay);
					}
					else
					{
						for(int i=0; i<=14;i++)
						{
							GPIOB->ODR = bcdo[i];  /* turn on 7SD */
							GPIOA->ODR = bcdo1[i];
							delayMs(1000);
						}
					}
					CWset = false;
				}
			}
	GPIOA->ODR = (NSR | EWR);
	curMode = nextMode;
	delayMs(500);
}


void flash(void)
{
	newMode = false;
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
	newMode = 0;
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
	newMode = 0;
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

// delay n milliseconds (16 MHz CPU clock) 
void delayMs(int n) {
    int i;

    // Configure SysTick 
    SysTick->LOAD = 16000;  // reload with number of clocks per millisecond
    SysTick->VAL = 0;       // clear current value register
    SysTick->CTRL = 0x5;    // Enable the timer

    for(i = 0; i < n; i++) {
        while((SysTick->CTRL & 0x10000) == 0) // wait until the COUNTFLAG is set
            { }
    }
    SysTick->CTRL = 0;      // Stop the timer (Enable = 0)
}