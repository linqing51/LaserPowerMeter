#include "InitDevice.h"
#include "lib.h"
/*****************************************************************************/
long xdata sampleArray[CONFIG_ADC_SAMPLE_LENGTH];
long xdata offsetZero;//¡„µ„∆Ø“∆
volatile long sampleAvage;
uint8_t sampleIndex;
#define MODBUS_F03_ADCRAW_L										0
#define MODBUS_F03_ADCRAM_H										1
#define MODBUS_F03_ADCAVG_L										2
#define MODBUS_F03_ADCAVG_H										3
#define MODBUS_F03_GAIN											4
#define MODBUS_F03_POWER										5

#define MODBUS_F03_AUTO_GAIN
#define MODBUS_F01_ZREO											0
/*****************************************************************************/
//y = Intercept + B1*x^1 + B2*x^2 + B3*x^3
fp64_t Notes_Intercept = -0.16355;
fp64_t Notes_B1 = 1.82066E-5;
fp64_t Notes_B2 = -2.75859E-13;
fp64_t Notes_B3 = 7.12301E-20;
/*****************************************************************************/
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
	fp64_t sum = 0;
	fp64_t ftemp0, ftemp1, ftemp2, ftemp3;
	uint8_t i;
	int32_t ADC_OutputVal = 0;//Concatenated ADC output value
	offsetZero = 0;
	while(!AD0INT);//Wait till conversion complete
	AD0INT = 0;//Clear ADC0 conversion complete flag
	ADC_OutputVal = (uint8_t)ADC0H;
	ADC_OutputVal <<= 16;
	ADC_OutputVal += (int32_t)ADC0L + ((int32_t)ADC0M << 8);
	sampleArray[sampleIndex] = ADC_OutputVal;
	sampleIndex ++;
	sampleIndex = sampleIndex % CONFIG_ADC_SAMPLE_LENGTH;
	for(i = 0; i< CONFIG_ADC_SAMPLE_LENGTH;i ++){
		sum += (fp64_t)sampleArray[i];
	}
	sampleAvage = (int32_t)(sum / CONFIG_ADC_SAMPLE_LENGTH);
	ftemp0 = pow((fp64_t)sampleAvage, 3) * Notes_B3;
	ftemp1 = pow((fp64_t)sampleAvage, 2) * Notes_B2;
	ftemp2 = (fp64_t)sampleAvage * Notes_B1;
	ftemp3 = ftemp0 + ftemp1 + ftemp2 + Notes_Intercept;
	if(ftemp3 >= 100){
		ftemp3 = 100;
	}
	if(ftemp3 <= -100){
		ftemp3 = -100;
	}
	NVRAM[MODBUS_F03_POWER] = (int16_t)(ftemp3 * 100);
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
//		switch(NVRAtM[MODBUS_F03_GAIN]){
//			case ADC_PGA_GAIN_1:{
//				break;
//			}	
//			case ADC_PGA_GAIN_2:{
//				break;
//			}
//			case ADC_PGA_GAIN_4:{
//				break;
//			}
//			case ADC_PGA_GAIN_8:{
//				break;
//			}
//			case ADC_PGA_GAIN_16:{
//				break;
//			}			
//			case ADC_PGA_GAIN_32:{
//				break;
//			}
//			case ADC_PGA_GAIN_64:{
//				break;
//			}
//			case ADC_PGA_GAIN_128:{
//				break;
//			}
//			default:break;
//		}
		modbusPorcess();
	}
}
