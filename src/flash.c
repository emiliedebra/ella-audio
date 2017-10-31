#include "flash.h"

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
void programFlash(uint32_t start, uint16_t * buffer, uint16_t length) {
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
  FLASH_EraseSector(FLASH_Sector_9, VoltageRange_3);
  FLASH_EraseSector(FLASH_Sector_10, VoltageRange_3);
  FLASH_EraseSector(FLASH_Sector_11, VoltageRange_3);
//  /* Get the number of the start and end sectors */
//  uint32_t StartSector = GetSector(start);
//  uint32_t EndSector = GetSector(start+length*2);
//
//  for (int i = StartSector; i < EndSector; i += 8)
//  {
//    /* Device voltage range supposed to be [2.7V to 3.6V], the operation will
//       be done by word */
//    if (FLASH_EraseSector(i, VoltageRange_3) != FLASH_COMPLETE)
//    {
//      /* Error occurred while sector erase.
//         User can add here some code to deal with this error  */
//      while (1)
//      {
//      }
//    }
//  }

  /* Program the user Flash area half word by half word
    (area defined by start and length) ***********/

  uint32_t Address = start;
//  uint16_t offset = 0;
  for (int i = 0; i < AUDIOBUFFERSIZE; i++)
  {
    FLASH_ProgramHalfWord(Address, buffer[i]);
    Address = Address + 2;
  }

  /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
  FLASH_Lock();
}
