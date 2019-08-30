#ifndef __LIB_H__
#define __LIB_H__
/*****************************************************************************/
#include "si_toolchain.h"
#include "compiler_defs.h"
/*****************************************************************************/
#define CONFIG_MB_RTU_SLAVE_TICK							1000//1KHz
#define CONFIG_MB_RTU_SLAVE_BUFFER_SIZE						64
#define CONFIG_UART_BAUDRATE     							115200// UART0 Baudrate (bps)
#define CONFIG_ADC_SAMPLE_LENGTH							64
#define CONFIG_NVRAM_SIZE									16
#define CONFIG_MB_RTU_SLAVE_TIMEOUT							50
#define CONFIG_MB_RTU_SLAVE_ADDRESS							0x01
#define ADC_PGA_GAIN_1										0x00
#define ADC_PGA_GAIN_2										0x01
#define ADC_PGA_GAIN_4										0x02
#define ADC_PGA_GAIN_8										0x03
#define ADC_PGA_GAIN_16										0x04
#define ADC_PGA_GAIN_32										0x05
#define ADC_PGA_GAIN_64										0x06
#define ADC_PGA_GAIN_128									0x07
/*****************************************************************************/
#ifdef C8051F350
#include "C8051F350_defs.h"
#define CONFIG_MDCLK        								2457600// Modulator Clock (Hz)
#define CONFIG_OWR          								10// desired Output Word Rate in Hz
#define CONFIG_VREF         								2444UL// Internal VREF (x 10^-2 V)
//#define CONFIG_SYSCLK										22118400//SYSCLK frequency (Hz)
#define CONFIG_SYSCLK										24500000//SYSCLK frequency (Hz)
#endif
/*****************************************************************************/
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <INTRINS.H>
#include <ctype.h>
#include <LIMITS.H>
#include <math.h>
#include "endian.h"
#include "stdbool.h"
#include "stdint.h"
#include "modbus.h"
#include "modbusPort.h"
#include "modbusAdr.h"
#endif

