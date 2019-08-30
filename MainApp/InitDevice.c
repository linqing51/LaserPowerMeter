/////////////////////////////////////
//  Generated Initialization File  //
/////////////////////////////////////
#include "InitDevice.h"
// Peripheral specific initialization functions,
// Called from the Init_Device() function

void PCA_Init()
{
    PCA0MD    &= ~0x40;
    PCA0MD    = 0x00;
}

void Reset_Sources_Init()
{
    int i = 0;
    VDM0CN    = 0x80;
    for (i = 0; i < 350; i++);  // Wait 100us for initialization
    RSTSRC    = 0x06;
}
void Voltage_Reference_Init()
{
    REF0CN    = 0x03;
}

void Port_IO_Init()
{
    // P0.0  -  Skipped,     Open-Drain, Digital
    // P0.1  -  Skipped,     Open-Drain, Digital
    // P0.2  -  Skipped,     Open-Drain, Digital
    // P0.3  -  Skipped,     Open-Drain, Digital
    // P0.4  -  TX0 (UART0), Open-Drain, Digital
    // P0.5  -  RX0 (UART0), Open-Drain, Digital
    // P0.6  -  Unassigned,  Open-Drain, Digital
    // P0.7  -  Unassigned,  Push-Pull,  Digital

    // P1.0  -  Unassigned,  Open-Drain, Digital
    // P1.1  -  Unassigned,  Open-Drain, Digital
    // P1.2  -  Unassigned,  Open-Drain, Digital
    // P1.3  -  Unassigned,  Open-Drain, Digital
    // P1.4  -  Unassigned,  Open-Drain, Digital
    // P1.5  -  Unassigned,  Open-Drain, Digital
    // P1.6  -  Skipped,     Open-Drain, Digital
    // P1.7  -  Skipped,     Open-Drain, Digital

    P0MDOUT   = 0x80;
    P0SKIP    = 0x0F;
    P1SKIP    = 0xC0;
    XBR0      = 0x01;
    XBR1      = 0x40;

}

void Oscillator_Init()
{
	FLSCL     = 0x10;
    OSCICN    = 0x83;		

}


// Initialization function for device,
// Call Init_Device() from your main program
void initDevice(void)
{
	PCA_Init();
    Reset_Sources_Init();
    Voltage_Reference_Init();
    Port_IO_Init();
    Oscillator_Init();
}
