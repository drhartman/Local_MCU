//*****************************************************************************
//
//*****************************************************************************
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "lm4f120h5qr.h"
#include "board_config.h"
#include "timer0.h"
#include "uart.h"
#include "i2c.h"


/******************************************************************************
 * Defines
 *****************************************************************************/
#define PORTA                 0x40004000
#define PORTB                 0x40005000
#define PORTC                 0x40006000
#define PORTD                 0x40007000
#define PORTE                 0x40024000
#define PORTF                 0x40025000

#define COLOFFSET			  3
#define BLUE				  0
#define	RED					  1
#define GREEN				  2



/******************************************************************************
 * Global Variables
 *****************************************************************************/
volatile GPIO_PORT *GpioPortA = (GPIO_PORT *)PORTA;
volatile GPIO_PORT *GpioPortB = (GPIO_PORT *)PORTB;
volatile GPIO_PORT *GpioPortC = (GPIO_PORT *)PORTC;
volatile GPIO_PORT *GpioPortD = (GPIO_PORT *)PORTD;
volatile GPIO_PORT *GpioPortE = (GPIO_PORT *)PORTE;
volatile GPIO_PORT *GpioPortF = (GPIO_PORT *)PORTF;
volatile UART_PERIPH *Uart3 = (UART_PERIPH *)UART3;
volatile I2C_PERIPH *i2c1 = (I2C_PERIPH *)I2C1;

//software represention of single board's LEDs
volatile uint8_t colorMatrix[8][24];
//software represention of single board's Sensors
//0 = off, 1 = on
volatile uint8_t sensorMatrix[4][4]; 

//////golobals used in Timer0////// can change to your liking
volatile bool      UpdateDisplay = false; 
volatile bool      CheckADC = false;
volatile uint16_t  DutyCycle=100;
volatile bool      CheckButton = false;
uint16_t      ucTensCount=0;
uint16_t      ucOnesCount=0;
volatile bool OneSecond = false;
extern volatile bool       resetISRCounts;
extern volatile uint16_t   DutyCycle;
//////golobals used in Timer0//////


//*****************************************************************************
// External Functions
//*****************************************************************************
extern void PLL_Init(void);
extern void EnableInterrupts(void);
extern void DisableInterrupts(void);
extern void sysTickInitInt(uint32_t count, uint8_t priority);
extern void output_grb(uint32_t gpPortAddr, uint8_t *matrixPtr);


/******************************************************************************
 * Functions
 *****************************************************************************/
 
void initPorts(void){
	uint32_t delay;
	
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0 | SYSCTL_RCGCGPIO_R1| SYSCTL_RCGCGPIO_R2 | SYSCTL_RCGCGPIO_R3 | SYSCTL_RCGCGPIO_R4 | SYSCTL_RCGCGPIO_R5;
	delay = SYSCTL_RCGCGPIO_R;
	
	//PORT A
	//Uart0 and I2C1
	GpioPortA->Direction = 0xFE;
	GpioPortA->AlternateFunctionSelect = 0xC3;
	GpioPortA->OpenDrainSelect = 0x80;
	GpioPortA->DigitalEnable = 0xC3;
	GpioPortA->PortControl |= (GPIO_PCTL_PA0_U0RX | GPIO_PCTL_PA1_U0TX);
	GpioPortA->PortControl |= (GPIO_PCTL_PA7_I2C1SDA | GPIO_PCTL_PA6_I2C1SCL);

	
	//PORT B
	//All digital output pins to the LEDs
	GpioPortB->Direction = 0xFF;	             //all outputs
	GpioPortB->AlternateFunctionSelect = 0;		 //regular gpio pin output
	GpioPortB->DigitalEnable |= 0xFF;		     //digital output
	GpioPortB->DriveSelect8mA |= 0xFF;		     //all pins get 8mA drive
	

	//PORT C
	//pin 6 is U3RX, pin 7 is U3TX
	//careful not to adjust programming pins
	GpioPortC->Direction &= 0xBF;
	GpioPortC->Direction |= 0x40;
	GpioPortC->AlternateFunctionSelect |= 0xC0;
	GpioPortC->DigitalEnable |= 0xC0;
	GpioPortC->PortControl |= (GPIO_PCTL_PC6_U3RX | GPIO_PCTL_PC7_U3TX);

	
	//PORT D
	//All pins used for Host Inturrupt from proximity controllers
	//Inturrupts configured to be on negative edge 
	GpioPortD->Direction = 0x00;
	GpioPortD->AlternateFunctionSelect = 0;
	GpioPortD->DigitalEnable |= 0xFF;
	GpioPortD->InterruptMask = 0;
	GpioPortD->InterruptSence = 0;
	GpioPortD->InterruptBothEdges = 0;
	GpioPortD->InterruptEvent = 0;
	GpioPortD->InterruptMask = 0xFF;

	//PORT E
	//UNUSED
	
	//PORT F
	//UNUSED
	
}

//*****************************************************************************
// Main Routine
//*****************************************************************************
int 
main(void)
{
    // Initialize the PLLs so the the main CPU frequency is 80MHz
    PLL_Init();

    // 80MHz/(100HZ * 8 * 100 PWM Steps) = 1000 CLK Cycles per Timer0 Interrupt    
    //      = 12.5uS
    timer0Init(1000, TIMER0_PERIODIC, TIMER0_ENABLE_INTS, TIMER0_PRIORITY);
	
	//Initialize GPIO ports
	initPorts();
    
    // Since PD7 is shared with the NMI, we need to clear the lock register and 
    // set the commit register so that all the pins alternate functions can
    // be used.
    GPIO_PORTD_LOCK_R = 0x4C4F434B;
    GPIO_PORTD_CR_R = 0xFF;

	//Initialize Peripherals
	InitializeUART(UART3);
	InitializeI2C(I2C1);
	

	while(1)
    {
		//EXAMPLE ON HOW TO CHANGE A SPECIFIC LED'S COLOR VALUE
		//I want led's row 4, column 7, blue value
		colorMatrix[4][7*COLOFFSET+BLUE] = 0x0F;

    }; 
}


