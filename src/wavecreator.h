#ifndef WAVECREATOR_H_
#define WAVECREATOR_H_

/* ----- Includes ------- */
#include "string.h"
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "main.h"
/* ------ Function Prototypes ------ */

void fillFlashBuffer(uint16_t frequency);
void createDrumSynth(uint16_t frequency);
void fillPianoBuffer(uint32_t frequencyADDR);
void fillDrumBuffer(uint32_t frequencyADDR);
void addToPianoBuffer(uint32_t frequencyADDR);
void addToDrumBuffer(uint32_t frequencyADDR);

#endif
