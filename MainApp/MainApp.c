#include "InitDevice.h"
#include "lib.h"
/*****************************************************************************/
long xdata sampleArray[CONFIG_ADC_SAMPLE_LENGTH];
uint8_t sampleIndex;
#define MODBUS_F03_ADCRAW_L										0
#define MODBUS_F03_ADCRAM_H										1
#define MODBUS_F03_ADCAVG_L										2
#define MODBUS_F03_ADCAVG_H										3
#define MODBUS_F03_GAIN											4
#define MODBUS_F03_POWER										5

#define MODBUS_F03_AUTO_GAIN
#define MODBUS_F
/*****************************************************************************/
long code correctionTable[100] = {
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
};

void adcInit (uint8_t gain){
	memset((uint8_t*)sampleArray, 0x00, CONFIG_ADC_SAMPLE_LENGTH * 2);
	sampleIndex = 0;
	ADC0MD = 0x0;// clear bits (put ADC0 in IDLE mode)	
	ADC0CN = 0x00;//
	ADC0CN |= (gain & 0x07);
	//ADC0CN |= 0 << 3;//ADC Burnout current sources enabled.
	ADC0CN |= 1 << 4;//ADC operates in Bipolar mode (2's compliment result).
	ADC0CF = 0x00;//Interrupts upon SINC3 filter output and uses internal VREF
	ADC0CLK = (CONFIG_SYSCLK / CONFIG_MDCLK)-1;// Generate MDCLK for modulator.Ideally MDCLK = 2.4576MHz
	ADC0DEC = ((unsigned long) CONFIG_MDCLK / (unsigned long) CONFIG_OWR / (unsigned long) 128) - 1;
	ADC0BUF = 0x00;// Turn off Input Buffers
	ADC0MUX = 0x00;//AIN1->-
	ADC0MUX = 1 << 4;//AIN0->+
	EIE1 &= ~0x08;// Disable ADC0 interrupts
	ADC0MD = 0;// Start internal calibration	
	ADC0MD |= 1 << 7;//ADC0 Enabled. ADC is active and ready to perform calibrations or conversions	
	ADC0MD |= 1 << 0;//Full Internal Calibration (offset and gain).   
	while(AD0CALC != 1);//Wait until calibration is complete
	AD0INT = 0;// clear pending sample indication
	ADC0MD = 0;// clear bits (put ADC0 in IDLE mode)
	ADC0MD |= 1 << 7;
	ADC0MD |= 0x03;
	EIE1   |= 0x08;                     // Enable ADC0 Interrupts
}
void ADC0_ISR (void) interrupt INTERRUPT_ADC0{
	double sum = 0;
	long sampleAvage = 0;
	char i;
	volatile long ADC_OutputVal = 0;//Concatenated ADC output value
	while(!AD0INT);//Wait till conversion complete
	AD0INT = 0;//Clear ADC0 conversion complete flag
	ADC_OutputVal = (char)ADC0H;
	ADC_OutputVal <<= 16;
	ADC_OutputVal += (long)ADC0L + ((long)ADC0M << 8);
	sampleArray[sampleIndex] = ADC_OutputVal;
	sampleIndex ++;
	sampleIndex = sampleIndex % CONFIG_ADC_SAMPLE_LENGTH;
	for(i = 0; i< CONFIG_ADC_SAMPLE_LENGTH;i ++){
		sum += (double)sampleArray[i];
	}
	sampleAvage = (long)(sum / CONFIG_ADC_SAMPLE_LENGTH);
	NVRAM[MODBUS_F03_ADCRAW_L] = (uint16_t)(ADC_OutputVal & 0xFFFF);
	NVRAM[MODBUS_F03_ADCRAM_H] = (uint16_t)((ADC_OutputVal >> 16) & 0xFFFF);
	NVRAM[MODBUS_F03_ADCAVG_L] = (uint16_t)(sampleAvage & 0xFFFF);
	NVRAM[MODBUS_F03_ADCAVG_H] = (uint16_t)((sampleAvage >> 16) & 0xFFFF);
}

void main(void){
	initDevice();		
	adcInit(ADC_PGA_GAIN_1);
	InitModbus(CONFIG_MB_RTU_SLAVE_ADDRESS);
	EA = 1;
	while(1){
		switch(NVRAM[MODBUS_F03_GAIN]){
			case ADC_PGA_GAIN_1:{
				break;
			}	
			case ADC_PGA_GAIN_2:{
				break;
			}
			case ADC_PGA_GAIN_4:{
				break;
			}
			case ADC_PGA_GAIN_8:{
				break;
			}
			case ADC_PGA_GAIN_16:{
				break;
			}			
			case ADC_PGA_GAIN_32:{
				break;
			}
			case ADC_PGA_GAIN_64:{
				break;
			}
			case ADC_PGA_GAIN_128:{
				break;
			}
			default:break;
		}
		modbusPorcess();
	}
}
