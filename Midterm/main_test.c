#include <msp430.h>
#include "LED.h"
#include "spi.h"
#include "serial_flash.h"

/**
 * @author nk5s
 *
 * Project 05
 *
 */

// Function prototypes
void ConfigureClockModule(void);

void main(void) {
	// Stop the watchdog timer, and configure the clock module.
	WDTCTL = WDTPW + WDTHOLD;
	ConfigureClockModule();
	InitializeLEDPortPins();
    InitializeSPI();
    InitializeSerialFlash();


    /* Send Test
    char test0f = 0x05;
    char testaf = 0xaf;

	// Infinite loop
	while (1) {
		SPISendByte(test0f);
		SPISendByte(testaf);
		_delay_cycles(100000); // 0.1 seconds
	}
	*/

    volatile unsigned int IDU3 = 0;
    volatile unsigned int IDU2 = 0;
    volatile unsigned char StatusRegister_U2 = 0;
    volatile unsigned char StatusRegister_U3 = 0;

    //unsigned long testLong = 0xabcdef12;

    //SPISendAddress(testLong);

    while(1){

		IDU2 = ReadFlashMemoryID(FLASH_MEMORY_U2);
		IDU3 = ReadFlashMemoryID(FLASH_MEMORY_U3);
		SetBlockProtection(FULL, FLASH_MEMORY_U2);
		SetBlockProtection(FULL, FLASH_MEMORY_U3);
		StatusRegister_U2 = ReadFlashMemoryStatusRegister(FLASH_MEMORY_U2);
		StatusRegister_U3 = ReadFlashMemoryStatusRegister(FLASH_MEMORY_U3);
		_delay_cycles(100000); // 0.1 seconds
    }
}

void ConfigureClockModule(void){
    // Configure Digitally Controlled Oscillator (DCO) using factory calibrations.
	DCOCTL  = CALDCO_16MHZ;
	BCSCTL1 = CALBC1_16MHZ;
}


