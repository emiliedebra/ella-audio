#include "flash.h"

uint32_t getDrumAddress(int i) {
	if (i == 0) {
		return KICK_START;
	}
	else if (i == 1) {
		return FLOORTOM_START;
	}
	else if (i == 2) {
		return HITOM_START;
	}
	else if (i == 3) {
		return CHIHAT1_START;
	}
	else if (i == 4) {
		return CHIHAT2_START;
	}
	else if (i == 5) {
		return OHIHAT_START;
	}
	else if (i == 6) {
		return SNARE_START;
	}
	return CRASH_START;
}
/*
 * Returns sector of given address in flash
 */
uint32_t GetSector(uint32_t Address)

{
	uint32_t sector = 0;
	if((Address <ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
	{
		sector = FLASH_Sector_0;
	}
	else if((Address <ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
	{
		sector = FLASH_Sector_1;
	}
	else if((Address <ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
	{
		sector = FLASH_Sector_2;
	}
	else if((Address <ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
	{
		sector = FLASH_Sector_3;
	}
	else if((Address <ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
	{
		sector = FLASH_Sector_4;
	}
	else if((Address <ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
	{
		sector = FLASH_Sector_5;
	}
	else if((Address <ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
	{
		sector = FLASH_Sector_6;
	}
	else if((Address <ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
	{
		sector = FLASH_Sector_7;
	}
	else if((Address <ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
	{
		sector = FLASH_Sector_8;
	}
	else if((Address <ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
	{
		sector = FLASH_Sector_9;
	}
	else if((Address <ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
	{
		sector = FLASH_Sector_10;
	}
	else
	{
		sector = FLASH_Sector_11;
	}


	return sector;
}

/*
 * Attempts to save a certain length buffer to the flash address given
 */
void programFlash() {
  /*!< At this stage the microcontroller clock setting is already configured,
       this is done through SystemInit() function which is called from startup
       file (startup_stm32f4xx.s) before to branch to application main.
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */

  /* Unlock the Flash to enable the flash control register access *************/
  FLASH_Unlock();

  /* Erase the user Flash area
    (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

  /* Clear pending flags (if any) */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);

//  FLASH_EraseSector(FLASH_Sector_5, VoltageRange_3);
//  FLASH_EraseSector(FLASH_Sector_6, VoltageRange_3);
//  FLASH_EraseSector(FLASH_Sector_7, VoltageRange_3);
//  FLASH_EraseSector(FLASH_Sector_8, VoltageRange_3);
  FLASH_EraseSector(FLASH_Sector_9, VoltageRange_3);
  FLASH_EraseSector(FLASH_Sector_10, VoltageRange_3);
  FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3);

  /* Program the user Flash area half word by half word
    (area defined by start and length) ***********/

  // Program A3
  fillFlashBuffer(frequency[0]);
  uint32_t Address = A3_START;
  for (int i = 0; i < AUDIOBUFFERSIZE; i++)
  {
    FLASH_ProgramHalfWord(Address, AUDIOBuffer[i]);
    Address = Address + 2;
  }

  // Program B3
  fillFlashBuffer(frequency[1]);
  Address = B3_START;
  for (int i = 0; i < AUDIOBUFFERSIZE; i++)
  {
	FLASH_ProgramHalfWord(Address, AUDIOBuffer[i]);
	Address = Address + 2;
  }

  // Program C3
  fillFlashBuffer(frequency[2]);
  Address = C3_START;
  for (int i = 0; i < AUDIOBUFFERSIZE; i++)
  {
	FLASH_ProgramHalfWord(Address, AUDIOBuffer[i]);
	Address = Address + 2;
  }

  // Program D3
  fillFlashBuffer(frequency[3]);
  Address = D3_START;
  for (int i = 0; i < AUDIOBUFFERSIZE; i++)
  {
	FLASH_ProgramHalfWord(Address, AUDIOBuffer[i]);
	Address = Address + 2;
  }

  // Program E3
  fillFlashBuffer(frequency[4]);
  Address = E3_START;
  for (int i = 0; i < AUDIOBUFFERSIZE; i++)
  {
	FLASH_ProgramHalfWord(Address, AUDIOBuffer[i]);
	Address = Address + 2;
  }

  // Program F3
  fillFlashBuffer(frequency[5]);
  Address = F3_START;
  for (int i = 0; i < AUDIOBUFFERSIZE; i++)
  {
	FLASH_ProgramHalfWord(Address, AUDIOBuffer[i]);
	Address = Address + 2;
  }

  // Program G3
  fillFlashBuffer(frequency[6]);
  Address = G3_START;
  for (int i = 0; i < AUDIOBUFFERSIZE; i++)
  {
	FLASH_ProgramHalfWord(Address, AUDIOBuffer[i]);
	Address = Address + 2;
  }

  //Program A4
  fillFlashBuffer(frequency[7]);
  Address = A4_START;
  for (int i = 0; i < AUDIOBUFFERSIZE; i++)
  {
	FLASH_ProgramHalfWord(Address, AUDIOBuffer[i]);
	Address = Address + 2;
  }

  // Save Drum Sounds to Flash
  for (int i = 0; i < 8; i++) {
	  createDrumSynth(i);
	  Address = (uint32_t)getDrumAddress(i);
	  for (int j = 0; j < AUDIOBUFFERSIZE; j++) {
		  FLASH_ProgramHalfWord(Address, DRUMBuffer[j]);
		  Address += 2;
	  }
  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  FLASH_Lock();
}
