/*
 * main.h
 *
 *  Created on: 28 Oct 2017
 *      Author: luke and Em
 */

#ifndef MAIN_H_
#define MAIN_H_

/* ----- Includes ------- */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "string.h"
#include "math.h"
#include "flash.h"
#include "wavecreator.h"

/* ----- Macros ------*/
#define AUDIOBUFFERSIZE         4500
#define TIMER6_PRESCALER    	2     	/* produces a 42MHz tick */
#define TIMER_CLOCK           	84E6 	/* TIM6 runs at 84MHz */

/* ----- Enums ----- */
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
enum pauseResume { PAUSE = 0, RESUME = 1 };

/* ----- Variables ------ */
uint16_t silenceBuffer[AUDIOBUFFERSIZE] = {0};
uint16_t PIANOONEBuffer[AUDIOBUFFERSIZE];     /* Array for the waveform */
uint16_t DRUMBuffer[AUDIOBUFFERSIZE];     /* Array for the waveform */
uint16_t PIANOTWOBuffer[AUDIOBUFFERSIZE];     /* Array for the waveform */

uint32_t ADC1ConvertedValue[2] = {0};

uint8_t pianoOneArray[8] = {0};
uint8_t pianoTwoArray[8] = {0};
uint8_t drumArray[8] = {0};

uint8_t EArray[8] = {0b00111100, 0b001111110, 0b10010011, 0b10010001, 0b10010001, 0b10010001, 0b01110010, 0b00110100};
uint8_t LArray[8] = {0b00000000, 0b11111111, 0b11111111, 0b00000011, 0b00000011, 0b00000011, 0b00000011, 0b00000000};
uint8_t AArray[8] = {0b00011100, 0b00111110, 0b001100011, 0b01000001, 0b01000001, 0b00100010, 0b01111111, 0b00000010};

uint8_t prevGridBtnStateArray[8] = {0};
uint8_t prevInstBtnState = 0;
uint8_t prevControlBtnState = 0;

uint16_t DMA_timerPeriod;
uint8_t beatCounter 		= 0;
uint8_t beatFlag 			= 0;
uint8_t updateInterfaceFlag = 0;
uint8_t audioPlayingFlag 	= 0;
uint8_t ledOnCol 			= 0;
uint8_t currentInstrument 	= 0;
float volume 				= 0;
uint8_t row 				= 0;
uint8_t pauseResumeStatus 	= PAUSE;

float frequencyScaler 		= 9.99E-5;
uint16_t frequency[16] 		= {220, 246, 261, 293, 329, 349, 392, 440, 110, 61, 65, 73, 82, 87, 98, 55};


/*----- Function Prototypes ------*/

//function prototypes for main.c
void DMA_ChangeBuffer(uint16_t *waveBuffer);
void delay_ms(uint32_t milli);
void Controller_Setup(uint16_t DMA_timerPeriod);
int ADC_Convert_Volume(void);
int ADC_Convert_Tempo(void);
void setInstrument(void);
void setVolume(void);
void setTempo(void);
void checkButtons(void);
void setLEDs(void);
void updateInterface(void);
void clearInstrument(void);
void SPI_SendLEDData(char red, char green, char blue, char column);


//function prototypes for player.c
uint8_t getDrumBeat(uint8_t beat);
uint8_t getPianoOneBeat(uint8_t beat);
uint8_t getPianoTwoBeat(uint8_t beat);
void addDrumBeat(uint8_t beat);
void addPianoOneBeat(uint8_t beat);
void addPianoTwoBeat(uint8_t beat);
void play(void);
void pause(void);
void resume(void);




#endif /* MAIN_H_ */
