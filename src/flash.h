#ifndef FLASH_H_
#define FLASH_H_

/* ----- Includes ------- */
#include "string.h"
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "main.h"

/* ----- Macros ------ */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

#define A5_START				((uint32_t)0x080A0000)
#define B5_START				((uint32_t)0x080A2710)
#define C5_START				((uint32_t)0x080A4E20)
#define D5_START				((uint32_t)0x080A7530)
#define E5_START				((uint32_t)0x080A9C40)
#define F5_START				((uint32_t)0x080AC350)
#define G5_START				((uint32_t)0x080AEA60)
#define A4_START				((uint32_t)0x080C0000)

/* ------ Function Prototypes ------ */
uint32_t GetSector(uint32_t Address);
void programFlash();
#endif
