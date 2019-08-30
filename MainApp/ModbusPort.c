#include "modbusPort.h"
/*****************************************************************************/
static uint8_t *pSendBuf;
/*****************************************************************************/
static uint16_t modbusUartSendLengh;
static uint16_t modbusUartSendNum;
/*****************************************************************************/
void modBusTimerInitialise(void){//初始化MODBUS计时器 1mS TIMER1	
	uint16_t temp;
	temp = (uint16_t)(65536 - (CONFIG_SYSCLK / 12 / CONFIG_MB_RTU_SLAVE_TICK));	
	CKCON &= ~0x60;// Timer2 uses SYSCLK/12
	TMR2CN &= ~0x01;
	TMR2RL = temp;// Reload value to be used in Timer2
	TMR2 = TMR2RL;// Init the Timer2 register
	TMR2CN = 0x04;// Enable Timer2 in auto-reload mode
	ET2 = 1;// Timer2 interrupt enabled
}
void modBusUartInitialise(void){// UART Initialize for Microconrollers, yes you can use another phsycal layer!
	SCON0 = 0x10;// 8-bit variable bit rate
	if(CONFIG_SYSCLK / CONFIG_UART_BAUDRATE / 2 / 256 < 1){
	  TH1 = -(CONFIG_SYSCLK / CONFIG_UART_BAUDRATE / 2);
	  CKCON |=  0x08;                     // T1M = 1; SCA1:0 = xx
	} else if (CONFIG_SYSCLK / CONFIG_UART_BAUDRATE / 2 / 256 < 4){
	  TH1 = -(CONFIG_SYSCLK / CONFIG_UART_BAUDRATE/ 2 / 4);
	  CKCON &= ~0x0B;                     // T1M = 0; SCA1:0 = 01
	  CKCON |=  0x01;
	} else if (CONFIG_SYSCLK / CONFIG_UART_BAUDRATE / 2 / 256 < 12){
	  TH1 = -(CONFIG_SYSCLK / CONFIG_UART_BAUDRATE / 2 / 12);
	  CKCON &= ~0x0B;                     // T1M = 0; SCA1:0 = 00
	} else {
	  TH1 = -(CONFIG_SYSCLK / CONFIG_UART_BAUDRATE / 2 / 48);
	  CKCON &= ~0x0B;                     // T1M = 0; SCA1:0 = 10
	  CKCON |=  0x02;
	}
	TL1 = TH1;                             // init Timer1
	TMOD &= ~0xf0;                         // TMOD: timer 1 in 8-bit autoreload
	TMOD |=  0x20;
	TR1 = 1;                               // START Timer1
	TI0 = 1;                               // Indicate TX0 ready
	IP = 0x10;//UART0 高中断级别
	ES0 = 1;
}
uint8_t modBusUartString(uint8_t *s, uint16_t  Length){// This is used for send string, better to use DMA for it ;)
	pSendBuf = s;	
	modbusUartSendLengh = Length;
	modbusUartSendNum = 0x0;
	TI0 = 1;//开始发送			
	return true;
}
void receiveInterrupt(uint8_t Data){//Call this function into your UART Interrupt. Collect data from it!
    modbusReceiveBuffer[modbusReceiveCounter] = Data;
    modbusReceiveCounter ++;
    if(modbusReceiveCounter > CONFIG_MB_RTU_SLAVE_BUFFER_SIZE){  
        modbusReceiveCounter = 0;
	}
    modbusTimerValue = 0;
}
/******************************************************************************/
static void modbusHandle() interrupt INTERRUPT_TIMER2
{//硬件计时器TIMER1中断函数 1mS
	TF2H = 0;//Reset Interrupt
	modbusTimerValue ++;
}
void UartIsr(void) interrupt INTERRUPT_UART0 {//UART0中断
	if(RI0){
		RI0 = 0;	
		receiveInterrupt(SBUF0);
	}
	if(TI0){
		TI0 = 0;
		if(modbusUartSendNum < modbusUartSendLengh){//
            SBUF0 = *(pSendBuf + modbusUartSendNum);
			modbusUartSendNum ++;						                   
		}
	}	
}

