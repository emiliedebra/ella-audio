#ifndef WAVECREATOR_H_
#define WAVECREATOR_H_

/* ----- Includes ------- */
#include "string.h"
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "main.h"
/* ------ Function Prototypes ------ */
void fillBuffer(uint32_t frequencyADDR);
void addToBuffer(uint32_t frequencyADDR);
#endif
