#ifndef __I2C_H__
#define __I2C_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include "lm4f120h5qr.h"

#define	I2C0 0x40020000
#define I2C1 0x40021000
#define I2C2 0x40022000
#define I2C3 0x40023000


typedef struct {
  volatile uint32_t    MasterSlaveAddr;         // + 0x000
  volatile uint32_t    MasterCtrlStat;          // + 0x004
  volatile uint32_t    MasterData;              // + 0x008
  volatile uint32_t    MasterTimePeriod;  		// + 0x00C		
  volatile uint32_t    MasterIntMask;           // + 0x010
  volatile uint32_t    MasterIntRawStat;        // + 0x014
  volatile uint32_t    MasterMaskIntStat;       // + 0x018
  volatile uint32_t    MasterIntClr;            // + 0x01C
  volatile uint32_t    MasterConfig;            // + 0x020
  volatile uint32_t    MasterClkLowTimeoutCnt;  // + 0x024
  volatile uint32_t    UNUSED0;        			// + 0x028 
  volatile uint32_t    MasterBusMonitor;        // + 0x02C
  volatile uint32_t    UNUSED1[2];              
  volatile uint32_t    MasterConfig2;           // + 0x038
  volatile uint32_t    UNUSED2[993];            
  volatile uint32_t    PeriphProp;              // + 0xFC0
  volatile uint32_t    PeriphConfig;            // + 0xFC4
} I2C_PERIPH;

//exported functions
bool  InitializeI2C(uint32_t base);

#endif

			