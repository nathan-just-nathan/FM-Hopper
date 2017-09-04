#ifndef SPI_SERIAL_FLASH_H_
#define SPI_SERIAL_FLASH_H_

#include <msp430.h>
#include "spi.h"

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * USCI A0 Slave Transmit Enable (STE)
 * for SPI Serial Flash Memory on GPIO board,
 * component U3
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * GPIO      :  P1.5
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define FLASH_MEMORY_U3 3
#define USCIA0_STE_U3_BIT				BIT5
#define USCIA0_STE_U3_PORT				P1OUT
#define USCIA0_STE_U3_DDR				P1DIR
#define SET_ENABLE_U3_AS_AN_OUTPUT		USCIA0_STE_U3_DDR |= USCIA0_STE_U3_BIT
#define ENABLE_FLASH_MEMORY_U3 			USCIA0_STE_U3_PORT &= ~USCIA0_STE_U3_BIT // active low
#define DISABLE_FLASH_MEMORY_U3			USCIA0_STE_U3_PORT |= USCIA0_STE_U3_BIT

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * USCI A0 Slave Transmit Enable (STE)
 * for SPI Serial Flash Memory on GPIO board,
 * component U2
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * GPIO      :  P2.0
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define FLASH_MEMORY_U2 2
#define USCIA0_STE_U2_BIT				BIT0
#define USCIA0_STE_U2_PORT				P2OUT
#define USCIA0_STE_U2_DDR				P2DIR
#define SET_ENABLE_U2_AS_AN_OUTPUT		USCIA0_STE_U2_DDR |= USCIA0_STE_U2_BIT
#define ENABLE_FLASH_MEMORY_U2 			USCIA0_STE_U2_PORT &= ~USCIA0_STE_U2_BIT // active low
#define DISABLE_FLASH_MEMORY_U2			USCIA0_STE_U2_PORT |= USCIA0_STE_U2_BIT

#define HOLD_U3_BIT						BIT2
#define HOLD_U3_PORT					P2OUT
#define HOLD_U3_DIR						P2DIR // For setting the direction of HOLD
#define SET_HOLD_U3_AS_AN_OUTPUT		HOLD_U3_DIR |= HOLD_U3_BIT
#define ENABLE_HOLD_U3					HOLD_U3_PORT &= ~HOLD_U3_BIT // active low
#define DISABLE_HOLD_U3					HOLD_U3_PORT |= HOLD_U3_BIT

#define HOLD_U2_BIT						BIT3
#define HOLD_U2_PORT					P2OUT
#define HOLD_U2_DIR						P2DIR // For setting the direction of HOLD
#define SET_HOLD_U2_AS_AN_OUTPUT		HOLD_U2_DIR |= HOLD_U2_BIT
#define ENABLE_HOLD_U2					HOLD_U2_PORT &= ~HOLD_U2_BIT // active low
#define DISABLE_HOLD_U2					HOLD_U2_PORT |= HOLD_U2_BIT

#define WRITE_PROTECT_BIT				BIT1
#define WRITE_PROTECT_PORT				P2OUT
#define WRITE_PROTECT_DIR				P2DIR // For setting the direction of write-protect
#define SET_WRITE_PROTECT_AS_AN_OUTPUT	WRITE_PROTECT_DIR |= WRITE_PROTECT_BIT
#define ENABLE_WRITE_PROTECT			WRITE_PROTECT_PORT &= ~WRITE_PROTECT_BIT // active low
#define DISABLE_WRITE_PROTECT			WRITE_PROTECT_PORT |= WRITE_PROTECT_BIT

#define FLASH_MEMORY_SIZE 0x10000	// 512 Kbits = 64K bytes
#define FLASH_SECTOR_SIZE 0x1000	// 4 Kbytes
#define FLASH_BLOCK_SIZE 0x8000		// 32 Kbytes
#define TSCE 100 					// chip-erase time limit (milliseconds)
#define TBP 20						// byte-program time (microseconds)

// Device Operation Instructions (see Table 6 in data sheet)
#define READ                0x03
#define HIGH_SPEED_READ     0x0b
#define SECTOR_ERASE        0x20
#define BLOCK_ERASE         0x52
#define CHIP_ERASE          0x60
#define BYTE_PROGRAM        0x02
#define AAI_PROGRAM         0xAF    // Auto Address Increment Program
#define RDSR                0x05    // Read Status Register
#define EWSR                0x50    // Enable-Write-Status-Register
#define WRSR                0x01    // Write-Status-Register
#define WREN                0x06    // Write-Enable
#define WRDI                0x04    // Write-Disable
#define READ_ID             0x90

//Bits of the SPI status  register
#define SR_BUSY                BIT0        // active high, is system busy writing
#define SR_WRITE_ENABLED       BIT1        // active high, is writing allowed
#define SR_BP0                 BIT2        // Block protection first bit
#define SR_BP1                 BIT3        // Block protection second bit
#define SR_AAI                 BIT6        // active high - use AAI programming mode
#define SR_BPL                 BIT7        // controls whether or not BP0 and BP1 bits are read/write (low) or not (high)

// Block protection levels
#define NONE 					0
#define QUARTER_0xC000_0xFFFF 	1
#define HALF_0x8000_0xFFFF 		2
#define FULL 					3

// Function Protoypes

/*
 * This function initializes the port pins associated with the serial flash components.
 */
void InitializeSerialFlash();

/*
 * This function reads the ID of the flash memory component, ComponentNumber.
 *
 * Return Value: The ID read from the flash memory component.
 */
unsigned int ReadFlashMemoryID(unsigned char ComponentNumber);

/*
 * This function reads from the status register of the flash memory component,
 * ComponentNumber.
 *
 * Return Value: The content of the status register.
 */
unsigned char ReadFlashMemoryStatusRegister(unsigned char ComponentNumber);

/*
 * This function writes the value of WriteValue to the status register of the flash memory
 * component, ComponentNumber.
 */
void WriteFlashMemoryStatusRegister(unsigned char WriteValue,unsigned char ComponentNumber);

/*
 * This function reads from flash memory component ComponentNumber, beginning at StartAddress,
 * and reads NumberOfDataValues, storing them in DataValuesArray.
 *
 * The input parameter, ReadMode, is used to select between READ mode and HIGH_SPEED_READ mode.
 */
void ReadFlashMemory(unsigned long StartAddress, unsigned char* DataValuesArray,
 unsigned int NumberOfDataValues, unsigned char ComponentNumber, unsigned char ReadMode);

/*
 * This function sets the block protection of flash memory component, ComponnetNumber, to
 * the value of ProtectionLevel, which can have 1 of 4 values: NONE, QUARTER_0xC000_0xFFFF,
 * HALF_0x8000_0xFFFF, and FULL.
 */
void SetBlockProtection(unsigned char ProtectionLevel, unsigned char ComponentNumber);

/*
 * This function writes the byte, WriteValue, to the flash memory component, ComponentNumber,
 * at memory location MemoryAddress.
 */
void ByteProgramFlashMemory(unsigned long MemoryAddress, unsigned char WriteValue, unsigned char ComponentNumber);

/*
 * This function writes to flash memory component ComponentNumber, beginning at StartAddress,
 * and writes NumberOfDataValues, stored in DataValuesArray.
 *
 * The input parameter, ReadMode, is used to select between READ mode and HIGH_SPEED_READ mode.
 */
void AAIProgramFlashMemory(unsigned long StartAddress, unsigned char* DataValuesArray,
 unsigned int NumberOfDataValues, unsigned char ComponentNumber);

/*
 * This function will erase flash memory component ComponentNumber.
 */
void ChipEraseFlashMemory(unsigned char ComponentNumber);


void SectorBlockEraseFlashMemory(unsigned long StartAddress, unsigned char ComponentNumber, unsigned char EraseMode);

/*
 * This function is used to determine if flash memory component ComponentNumber is busy (that is,
 * whether or not the flash memory has completed the current command.
 *
 * Return Value: The return value is the SR_BUSY bit contained in the status register within the flash
 * memory component.
 */
unsigned char FlashMemoryBusy(unsigned char ComponentNumber);

#endif
