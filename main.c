#include <msp430.h>
#include "LED.h"
#include "spi.h"
#include "serial_flash.h"

// Used with the LFSR function
#define DIVISOR_POLYNOMIAL 0xB400
#define INITIAL_LFSR_STATE 0x0001
#define LFSR_TYPE unsigned int

// Local prototypes
void ConfigureClockModule();
LFSR_TYPE LFSR(LFSR_TYPE CurrentState, LFSR_TYPE Divisor);
void CheckSumFlashMemoryTest(unsigned char ComponentNumber, unsigned char ProgrammingMethod,
		unsigned char EraseMode, LFSR_TYPE InitialLFSRState, LFSR_TYPE* LFSRStateAfterRead,
		LFSR_TYPE* LFSRStateAfterWrite);

// Use for read/write flash memory tests.
#define TRUE 1
#define FALSE 0
#define BUFFER_SIZE 2
#define NUMBER_OF_TESTS 6

void main(void)
{
	unsigned char TestNumber;
	unsigned char NumberOfTestsPassed;
	volatile unsigned int ID_U3;
	volatile unsigned int ID_U2;
	volatile unsigned char StatusRegister_U3;
	volatile unsigned char StatusRegister_U2;
	LFSR_TYPE LFSRStateAfterRead_U3,LFSRStateAfterWrite_U3;
	LFSR_TYPE LFSRStateAfterRead_U2,LFSRStateAfterWrite_U2;
	unsigned char TestHasNotFailed[NUMBER_OF_TESTS];

	// Stop the watchdog timer, and configure the clock module.
	WDTCTL = WDTPW + WDTHOLD;
    ConfigureClockModule();

    // Initialize port pins.
	InitializeLEDPortPins();
    InitializeSPI();
    InitializeSerialFlash();

    // Begin by assuming all tests have not failed.
    for (TestNumber = 0; TestNumber < NUMBER_OF_TESTS; TestNumber++) TestHasNotFailed[TestNumber] = TRUE;
    TestNumber = 0;
    TURN_ON_LED2; // Green LED indicates test success.

    // Begin by reading the ID for each flash memory. The value should be 0xBF48 for both.
    ID_U3 = ReadFlashMemoryID(FLASH_MEMORY_U3); // Test 0
    if (ID_U3 != 0xBF48) {
    	TestHasNotFailed[TestNumber] = FALSE;
    }
	TestNumber++;
    ID_U2 = ReadFlashMemoryID(FLASH_MEMORY_U2);	// Test 1
    if (ID_U2 != 0xBF48) {
    	TestHasNotFailed[TestNumber] = FALSE;
    }
	TestNumber++;

    // Next, turn on block protection, and then read status register for each flash memory.
    // The value for each should be 0x0C.
    SetBlockProtection(FULL, FLASH_MEMORY_U3);
    SetBlockProtection(FULL, FLASH_MEMORY_U2);
    StatusRegister_U3 = ReadFlashMemoryStatusRegister(FLASH_MEMORY_U3);	// Test 2
    if (StatusRegister_U3 != 0x0C) {
    	TestHasNotFailed[TestNumber] = FALSE;
    }
	TestNumber++;
    StatusRegister_U2 = ReadFlashMemoryStatusRegister(FLASH_MEMORY_U2);	// Test 3
    if (StatusRegister_U2 != 0x0C) {
    	TestHasNotFailed[TestNumber] = FALSE;
    }
	TestNumber++;

    // Next, write pseudo-random data to each flash, and then read it to confirm flash read/write.
    CheckSumFlashMemoryTest(FLASH_MEMORY_U3,BYTE_PROGRAM,CHIP_ERASE,INITIAL_LFSR_STATE,
    		&LFSRStateAfterRead_U3,&LFSRStateAfterWrite_U3);			// Test 4
    if (LFSRStateAfterRead_U3 != LFSRStateAfterWrite_U3) {
    	TestHasNotFailed[TestNumber] = FALSE;
    }
	TestNumber++;
    CheckSumFlashMemoryTest(FLASH_MEMORY_U2,AAI_PROGRAM,~CHIP_ERASE,INITIAL_LFSR_STATE,
    		&LFSRStateAfterRead_U2,&LFSRStateAfterWrite_U2);			// Test 5
    if (LFSRStateAfterRead_U2 != LFSRStateAfterWrite_U2) {
    	TestHasNotFailed[TestNumber] = FALSE;
    }
	TestNumber++;

	// If all tests were successful, then indicate this by toggling the green LED.
	// Otherwise, toggle the red LED.
    for (NumberOfTestsPassed = 0, TestNumber = 0; TestNumber < NUMBER_OF_TESTS; TestNumber++)
    	NumberOfTestsPassed += TestHasNotFailed[TestNumber];
	while (TRUE) {
		if (NumberOfTestsPassed == NUMBER_OF_TESTS) {
			TOGGLE_LED2;
		}
		else {
			TOGGLE_LED1;
		}
		_delay_cycles(1000000);
	}
}

void ConfigureClockModule()
{
	// Configure Digitally Controlled Oscillator (DCO) using factory calibrations
	DCOCTL  = CALDCO_16MHZ;
	BCSCTL1 = CALBC1_16MHZ;
}

LFSR_TYPE LFSR(LFSR_TYPE CurrentState, LFSR_TYPE Divisor)
{
	LFSR_TYPE dividend;
	LFSR_TYPE lsb;

    /* In general, the polynomial exponents increase from left to right, whereas
     * the bit numbering increases from right to left:
     *
     * Bit:  1 1 1 1 1 1
     *       5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
     *
     *       b b b b b b b b b b b b b b b b, b = 0,1
     *
     * Poly: 0 1 2 3 4 5 6 7 8 9 1 1 1 1 1 1
     *                           0 1 2 3 4 5
     *
     * Thus, for the polynomial, 1 + x^11 + x^13 + x^14 + x^16:
     * Bit:  1 1 1 1 1 1
     *       5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
     *
     *       1 0 0 0 0 0 0 0 0 0 0 1 0 1 1 0
     *
     * Poly: 0 1 2 3 4 5 6 7 8 9 1 1 1 1 1 1
     *                           0 1 2 3 4 5
     */
    dividend = CurrentState;

    /* Get the value of the Least Significant Bit (LSB). Then, right-shift the dividend.
     *
     * The LSB represents the coefficient of x^(n-1) in the polynomial, and
     * right-shifting corresponds to multiplying the polynomial by x.  So, these
     * two operations essentially determine if the coefficent for x^n equals 1.
     */
    lsb = dividend & 1;
    dividend >>= 1;

    /* If the LSB == 1, then perform polynomial division using the XOR
     * operation (which corresponds to bitwise addition).
     *
     * The LSB equal to 1 corresponds to the coefficent for x^n equal to 1,
     * which means that the Divisor should be added to perform polynomial division.
     *
     * For example, if the dividend = x^15 (that is, 0x0001), then right-shifting
     * (that is, multiplying by x) prodces dividend = x^16.  So, perform polynomial
     * division:
     *                                 1
     *                                -----
     * 1 + x^11 + x^13 + x^14 + x^16 | x^16
     *                              +  x^16 + 1 + x^11 + x^13 + x^14
     *                                 -----------------------------
     *                                 1 + x^11 + x^13 + x^14
     */
    dividend ^= (-lsb) & Divisor;

    return dividend;
}

void CheckSumFlashMemoryTest(unsigned char ComponentNumber, unsigned char ProgrammingMethod,
		unsigned char EraseMode, LFSR_TYPE InitialLFSRState, LFSR_TYPE* LFSRStateAfterRead,
		LFSR_TYPE* LFSRStateAfterWrite)
{
	volatile char ReadWriteBuffer[BUFFER_SIZE];

	unsigned char i;
	unsigned long FlashAddress;
	unsigned char ByteDataValue;
	unsigned char FlashMemoryComponentNumber;

	// Begin by erasing entire flash memory, writing pseudo-random patterns to the
	// entire memory, and then reading back to confirm the write operations.
	FlashMemoryComponentNumber = ComponentNumber;
	SetBlockProtection(NONE, FlashMemoryComponentNumber);

	// Erase the flash memory, which must be performed before writing to flash.
	if (EraseMode == CHIP_ERASE) {
		ChipEraseFlashMemory(FlashMemoryComponentNumber);
	}
	else {

		// Erase half of the flash using sector-erase.
		for (FlashAddress = 0; FlashAddress < (FLASH_MEMORY_SIZE>>1); FlashAddress+=FLASH_SECTOR_SIZE) {
			SectorBlockEraseFlashMemory(FlashAddress,FlashMemoryComponentNumber,SECTOR_ERASE);
		}

		// Erase the remaining half using block-erase.
		for (FlashAddress = (FLASH_MEMORY_SIZE>>1); FlashAddress < FLASH_MEMORY_SIZE; FlashAddress+=BLOCK_ERASE) {
			SectorBlockEraseFlashMemory(FlashAddress,FlashMemoryComponentNumber,BLOCK_ERASE);
		}
	}

	// The programming method is selected by the value of ProgrammingMethod, and can either be
	// Auto-Address Increment (AAI, ProgrammingMethod = AAI_PROGRAM) or byte programming
	// (ProgrammingMethod = BYTE_PROGRAM).
	*LFSRStateAfterWrite = InitialLFSRState;
	if (ProgrammingMethod == BYTE_PROGRAM) {
		for (FlashAddress = 0; FlashAddress < FLASH_MEMORY_SIZE; FlashAddress+=2) {
			ByteDataValue = (unsigned char) *LFSRStateAfterWrite;
			ByteProgramFlashMemory(FlashAddress,ByteDataValue,FlashMemoryComponentNumber);
			ByteDataValue = (unsigned char) (*LFSRStateAfterWrite >> 8);
			ByteProgramFlashMemory((FlashAddress+1),ByteDataValue,FlashMemoryComponentNumber);
			*LFSRStateAfterWrite = LFSR(*LFSRStateAfterWrite,DIVISOR_POLYNOMIAL);
		}
	}
	else {
		for (FlashAddress = 0; FlashAddress < FLASH_MEMORY_SIZE; FlashAddress+=2) {
			ReadWriteBuffer[0] = (unsigned char) *LFSRStateAfterWrite;
			ReadWriteBuffer[1] = (unsigned char) (*LFSRStateAfterWrite >> 8);
			AAIProgramFlashMemory(FlashAddress,(unsigned char *) ReadWriteBuffer,2,FlashMemoryComponentNumber);
			*LFSRStateAfterWrite = LFSR(*LFSRStateAfterWrite,DIVISOR_POLYNOMIAL);
		}
	}

	for (i = 0; i < BUFFER_SIZE; i++) ReadWriteBuffer[i] = (char) 0;
	*LFSRStateAfterRead = 0;
	for (FlashAddress = 0; FlashAddress < FLASH_MEMORY_SIZE; FlashAddress+=2) {
		ReadFlashMemory(FlashAddress, (unsigned char *) ReadWriteBuffer,2,FlashMemoryComponentNumber,READ);
		*LFSRStateAfterRead = LFSR((LFSR_TYPE) ((ReadWriteBuffer[1] << 8)|ReadWriteBuffer[0]),DIVISOR_POLYNOMIAL);
	}
}

