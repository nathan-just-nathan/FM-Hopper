#include "serial_flash.h"

void InitializeSerialFlash()
{
	// U2
	SET_ENABLE_U2_AS_AN_OUTPUT;
	SET_HOLD_U2_AS_AN_OUTPUT;
	DISABLE_HOLD_U2;

	// U3
	SET_ENABLE_U3_AS_AN_OUTPUT;
	SET_HOLD_U3_AS_AN_OUTPUT;
	DISABLE_HOLD_U3;

	SET_WRITE_PROTECT_AS_AN_OUTPUT;
	DISABLE_WRITE_PROTECT;
}

unsigned int ReadFlashMemoryID(unsigned char ComponentNumber) {

	unsigned char localComponentNumber = ComponentNumber;

	unsigned int output = 0;

	switch(localComponentNumber){
	case FLASH_MEMORY_U2:
		ENABLE_FLASH_MEMORY_U2;

		// Call SPISend 4 times
		SPISendByte(READ_ID);	// 0x90
		SPISendByte(0x00);
		SPISendByte(0x00);
		SPISendByte(0x00);

		// Call SPIReceive twice, combine both bytes into unsigned int
		output |= (unsigned int)SPIReceiveByte();
		output <<=8;
		output |= (unsigned int)SPIReceiveByte();

		DISABLE_FLASH_MEMORY_U2;
		break;

	case FLASH_MEMORY_U3:
		ENABLE_FLASH_MEMORY_U3;

		// Call SPISend 4 times
		SPISendByte(READ_ID);	// 0x90
		SPISendByte(0x00);
		SPISendByte(0x00);
		SPISendByte(0x00);

		// Call SPIReceive twice, combine both bytes into unsigned int
		output |= (unsigned int)SPIReceiveByte();
		output <<=8;
		output |= (unsigned int)SPIReceiveByte();

		DISABLE_FLASH_MEMORY_U3;
		break;

	default: ;
	}

	return output;
}

unsigned char ReadFlashMemoryStatusRegister(unsigned char ComponentNumber) {

	unsigned char localComponentNumber = ComponentNumber;

	unsigned char output = 0;

	switch (localComponentNumber) {
	case FLASH_MEMORY_U2:
		ENABLE_FLASH_MEMORY_U2;
		SPISendByte(RDSR);	// Read status register (0x05)
		output |= SPIReceiveByte();
		DISABLE_FLASH_MEMORY_U2;
		break;

	case FLASH_MEMORY_U3:
		ENABLE_FLASH_MEMORY_U3;
		SPISendByte(RDSR);	// Read status register (0x05)
		output |= SPIReceiveByte();
		DISABLE_FLASH_MEMORY_U3;
		break;
	default: ;
	}
	return output;
}

void WriteFlashMemoryStatusRegister(unsigned char WriteValue,
									unsigned char ComponentNumber) {

	unsigned char localWriteValue = WriteValue;
	unsigned char localComponentNumber = ComponentNumber;

	switch 	(localComponentNumber) {

	case FLASH_MEMORY_U2:
		ENABLE_FLASH_MEMORY_U2;			// Accept instruction entry
		SPISendByte(EWSR);				// Enable Write Status Register (50)
		DISABLE_FLASH_MEMORY_U2;		// Execute instruction
		ENABLE_FLASH_MEMORY_U2;			// Accept instruction entry
		SPISendByte(WRSR);				// Write Status Register (01)
		SPISendByte(localWriteValue);	// Write new value to status register
		DISABLE_FLASH_MEMORY_U2;		// Execute instruction
		break;

	case FLASH_MEMORY_U3:
		ENABLE_FLASH_MEMORY_U3;			// Accept instruction entry
		SPISendByte(EWSR); 				// Enable Write Status Register (50)
		DISABLE_FLASH_MEMORY_U3;		// Execute instruction
		ENABLE_FLASH_MEMORY_U3;			// Accept instruction entry
		SPISendByte(WRSR);				// Write Status Register (01)
		SPISendByte(localWriteValue);	// Write new value to status register
		DISABLE_FLASH_MEMORY_U3;		// Execute instruction
		break;

	default: ;
	}
}

void ReadFlashMemory(unsigned long StartAddress,
					 unsigned char* DataValuesArray,
					 unsigned int NumberOfDataValues,
					 unsigned char ComponentNumber,
					 unsigned char ReadMode) {

	unsigned long localStartAddress = StartAddress;
	unsigned char* localDataValuesArray = DataValuesArray;
	unsigned int localNumberOfDataValues = NumberOfDataValues;
	unsigned char localComponentNumber = ComponentNumber;
	unsigned char localReadMode = ReadMode;

	unsigned char i;

	switch (localComponentNumber) {
		case FLASH_MEMORY_U2:
			ENABLE_FLASH_MEMORY_U2;
			SPISendByte(localReadMode);
			SPISendAddress(localStartAddress);
			if (localReadMode == HIGH_SPEED_READ) SPISendByte(0x00); // Dummy byte
			for(i=0; i<localNumberOfDataValues; i++){
				localDataValuesArray[i] = SPIReceiveByte();
			}
			DISABLE_FLASH_MEMORY_U2;
			break;

		case FLASH_MEMORY_U3:
			ENABLE_FLASH_MEMORY_U3;
			SPISendByte(localReadMode);
			SPISendAddress(localStartAddress);
			if (localReadMode == HIGH_SPEED_READ) SPISendByte(0x00); // Dummy byte
			for(i=0; i<localNumberOfDataValues; i++){
				localDataValuesArray[i] = SPIReceiveByte();
			}
			DISABLE_FLASH_MEMORY_U3;
			break;

		default: ;
		}
}

void ByteProgramFlashMemory(unsigned long MemoryAddress,
							unsigned char WriteValue,
							unsigned char ComponentNumber) {

	unsigned long localMemoryAddress = MemoryAddress;
	unsigned char localWriteValue = WriteValue;
	unsigned char localComponentNumber = ComponentNumber;

	switch (localComponentNumber) {
	case FLASH_MEMORY_U2:
		ENABLE_FLASH_MEMORY_U2;
		SPISendByte(WREN);			// Write-enable
		DISABLE_FLASH_MEMORY_U2;

		ENABLE_FLASH_MEMORY_U2;
		SPISendByte(BYTE_PROGRAM);
		SPISendAddress(localMemoryAddress);
		SPISendByte(localWriteValue);
		DISABLE_FLASH_MEMORY_U2;
		break;

	case FLASH_MEMORY_U3:
		ENABLE_FLASH_MEMORY_U3;
		SPISendByte(WREN);			// Write-enable
		DISABLE_FLASH_MEMORY_U3;

		ENABLE_FLASH_MEMORY_U3;
		SPISendByte(BYTE_PROGRAM);
		SPISendAddress(localMemoryAddress);
		SPISendByte(localWriteValue);
		DISABLE_FLASH_MEMORY_U3;
		break;

	default: ;
	}

	while (FlashMemoryBusy(localComponentNumber)); // Wait for completion, then exit
}

void AAIProgramFlashMemory(unsigned long StartAddress,
						   unsigned char* DataValuesArray,
						   unsigned int NumberOfDataValues,
						   unsigned char ComponentNumber) {

	unsigned long localStartAddress = StartAddress;
	unsigned char* localDataValuesArray = DataValuesArray;
	unsigned int localNumberOfDataValues = NumberOfDataValues;
	unsigned char localComponentNumber = ComponentNumber;

	unsigned char i;

	switch (localComponentNumber) {
	case FLASH_MEMORY_U2:
		ENABLE_FLASH_MEMORY_U2;
		SPISendByte(WREN);			// Write-enable
		DISABLE_FLASH_MEMORY_U2;

		ENABLE_FLASH_MEMORY_U2;
		SPISendByte(AAI_PROGRAM);
		SPISendAddress(localStartAddress);

		// Send first byte
		SPISendByte(DataValuesArray[0]);
		DISABLE_FLASH_MEMORY_U2;
		while (FlashMemoryBusy(localComponentNumber));

		// Send remaining bytes
		for(i=1; i<localNumberOfDataValues; i++){
			ENABLE_FLASH_MEMORY_U2;
			SPISendByte(AAI_PROGRAM);
			SPISendByte(localDataValuesArray[i]);
			DISABLE_FLASH_MEMORY_U2;
			while (FlashMemoryBusy(localComponentNumber));
		}

		ENABLE_FLASH_MEMORY_U2;
		SPISendByte(WRDI);			// Write-Disable
		DISABLE_FLASH_MEMORY_U2;
		break;

	case FLASH_MEMORY_U3:
		ENABLE_FLASH_MEMORY_U3;
		SPISendByte(WREN);			// Write-enable
		DISABLE_FLASH_MEMORY_U3;

		ENABLE_FLASH_MEMORY_U3;
		SPISendByte(BYTE_PROGRAM);
		SPISendAddress(localStartAddress);

		// Send first byte
		SPISendByte(DataValuesArray[0]);
		DISABLE_FLASH_MEMORY_U3;
		while (FlashMemoryBusy(localComponentNumber));

		// Send remaining bytes
		for(i=1; i<localNumberOfDataValues; i++){
			ENABLE_FLASH_MEMORY_U3;
			SPISendByte(AAI_PROGRAM);
			SPISendByte(localDataValuesArray[i]);
			DISABLE_FLASH_MEMORY_U3;
			while (FlashMemoryBusy(localComponentNumber));
		}
		ENABLE_FLASH_MEMORY_U3;
		SPISendByte(WRDI);			// Write-Disable
		DISABLE_FLASH_MEMORY_U3;
		break;

	default: ;
	}

	while (FlashMemoryBusy(localComponentNumber));
}

void ChipEraseFlashMemory(unsigned char ComponentNumber) {

	unsigned char localComponentNumber = ComponentNumber;

	switch (localComponentNumber) {
	case FLASH_MEMORY_U2:
		ENABLE_FLASH_MEMORY_U2;
		SPISendByte(WREN);			// Write-enable
		DISABLE_FLASH_MEMORY_U2;

		ENABLE_FLASH_MEMORY_U2;
		SPISendByte(CHIP_ERASE);
		DISABLE_FLASH_MEMORY_U2;
		break;

	case FLASH_MEMORY_U3:
		ENABLE_FLASH_MEMORY_U3;
		SPISendByte(WREN);			// Write-enable
		DISABLE_FLASH_MEMORY_U3;

		ENABLE_FLASH_MEMORY_U3;
		SPISendByte(CHIP_ERASE);
		DISABLE_FLASH_MEMORY_U3;
		break;

	default: ;
	}

	while (FlashMemoryBusy(localComponentNumber)); // Wait for completion, then exit

}

void SectorBlockEraseFlashMemory(unsigned long StartAddress,
								 unsigned char ComponentNumber,
								 unsigned char EraseMode) {

	unsigned long localStartAddress = StartAddress;
	unsigned char localComponentNumber = ComponentNumber;
	unsigned char localEraseMode = EraseMode;

	switch (localComponentNumber) {
	case FLASH_MEMORY_U2:
		ENABLE_FLASH_MEMORY_U2;
		SPISendByte(WREN);			// Write-enable
		DISABLE_FLASH_MEMORY_U2;

		ENABLE_FLASH_MEMORY_U2;
		SPISendByte(localEraseMode);
		SPISendAddress(localStartAddress);
		DISABLE_FLASH_MEMORY_U2;
		break;

	case FLASH_MEMORY_U3:
		ENABLE_FLASH_MEMORY_U3;
		SPISendByte(WREN);			// Write-enable
		DISABLE_FLASH_MEMORY_U3;

		ENABLE_FLASH_MEMORY_U3;
		SPISendByte(localEraseMode);
		SPISendAddress(localStartAddress);
		DISABLE_FLASH_MEMORY_U3;
		break;

	default: ;
	}

	while (FlashMemoryBusy(localComponentNumber)); // Wait for completion, then exit

}

void SetBlockProtection(unsigned char ProtectionLevel,
						unsigned char ComponentNumber) {

	unsigned char localProtectionLevel = ProtectionLevel;
	unsigned char localComponentNumber = ComponentNumber;

	unsigned char status = ReadFlashMemoryStatusRegister(localComponentNumber);

	switch  (localProtectionLevel){
	case NONE:
		// set bit3 and bit2 to 0
		status &= ~(SR_BP1|SR_BP0);
		break;
	case QUARTER_0xC000_0xFFFF:
		// set bit3 to 0 and bit2 to 1
		status &= ~SR_BP1;
		status |= SR_BP0;
		break;
	case HALF_0x8000_0xFFFF:
		// set bit3 to 1 and bit2 to 0
		status |= SR_BP1;
		status &= ~SR_BP0;
		break;
	case FULL:
		// set bit3 and bit2 to 1
		status |= (SR_BP1|SR_BP0);
		break;
	default: ;
	}

	WriteFlashMemoryStatusRegister(status,localComponentNumber);

}

unsigned char FlashMemoryBusy(unsigned char ComponentNumber) {

	return (char)ReadFlashMemoryStatusRegister(ComponentNumber)&SR_BUSY;

}


