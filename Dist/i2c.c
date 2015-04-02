#include "i2c.h"

/*************************************************************************
 * Initializes an I2C peripheral
 *
 * This function assumes the the corresponding GPIO ports have already
 * been configured for thier alternate functions and the port control
 * register were configured correctly.
 ************************************************************************/
bool  InitializeI2C(uint32_t base)
{
	uint32_t delay;
	I2C_PERIPH *myPeriph = (I2C_PERIPH *)base;
	
	// Validate that a correct base address has been passed
	// Turn on the Clock Gating Register
	switch (base) 
	{
	  case I2C0 :
	    SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R0;
	    break;
	  case I2C1 :
	    SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R1;
	    break;
	  case I2C2 :
	    SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R2;
	    break;
	  case I2C3 :
	    SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_R3;
	    break;
	  default:
	    return false;
	}

	delay = SYSCTL_RCGC1_R;
  	delay = SYSCTL_RCGC1_R;
	
	//Master Enable	
	myPeriph->MasterConfig = I2C_MCR_MFE;

	//Timer Period
	//set up a I2C clk freq of 400KHz
	// TPR = (80MHz/(2*(6 + 4)*400000))-1
	myPeriph->MasterTimePeriod = 0x09;

	//Enable Interrupts
	myPeriph->MasterIntMask = 0x01;			
	
	return true;

}
