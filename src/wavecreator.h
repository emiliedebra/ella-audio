#ifndef WAVECREATOR_H_
#define WAVECREATOR_H_

/* ----- Includes ------- */
#include "string.h"
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "main.h"
/* ------ Function Prototypes ------ */
void fillBuffer(uint16_t frequency);
void addToBuffer(uint16_t frequency);
#endif
