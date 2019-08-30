#ifndef __MODBUS_H__
#define __MODBUS_H__
/*****************************************************************************/
#include "ModbusPort.h"
/*****************************************************************************/
extern volatile uint16_t modbusTimerValue;
extern volatile uint8_t modbusReceiveCounter;// Collected data number
extern volatile uint8_t modbusReceiveBuffer[CONFIG_MB_RTU_SLAVE_BUFFER_SIZE];// Buffer to collect data from hardware
extern xdata volatile int16_t NVRAM[CONFIG_NVRAM_SIZE];
/*****************************************************************************/
extern void InitModbus(uint8_t modbusSlaveAddress);
extern void modbusPorcess(void);
extern void modBusTimerInitialise(void);
extern void modBusUartPutch(uint8_t c);
extern uint8_t modBusUartString(uint8_t *s, uint16_t Length);
extern void receiveInterrupt(uint8_t Data);
extern void modBusUartInitialise(void);

#endif
