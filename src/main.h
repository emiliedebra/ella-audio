/*
 * main.h
 *
 *  Created on: 28 Oct 2017
 *      Author: luke (and Em :( )
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

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* ----- Variables ------ */
extern uint16_t silenceBuffer[AUDIOBUFFERSIZE];
extern uint16_t PIANOONEBuffer[AUDIOBUFFERSIZE];     /* Array for the waveform */
extern uint16_t DRUMBuffer[AUDIOBUFFERSIZE];     /* Array for the waveform */
extern uint16_t PIANOTWOBuffer[AUDIOBUFFERSIZE];     /* Array for the waveform */
extern uint32_t ADC1ConvertedValue[2];
extern uint8_t pianoOneArray[8];
extern uint8_t pianoTwoArray[8];
extern uint8_t drumArray[8];
extern uint8_t beatCounter;
extern uint8_t beatFlag;
extern uint8_t audioPlayingFlag;
extern uint8_t changeToSilenceFlag;
extern uint8_t updateInterfaceFlag;

extern float volume;

enum pauseResume { PAUSE = 0, RESUME = 1 };
extern uint8_t pauseResumeStatus;

extern float frequencyScaler;
extern uint16_t frequency[16];

/*----- Function Prototypes ------*/

//function prototypes for main.c
void DMA_ChangeBuffer(uint16_t *waveBuffer);
void delay_ms(uint32_t milli);
void Controller_Setup(uint16_t DMA_timerPeriod);
int ADC_Convert_Volume(void);
int ADC_Convert_Tempo(void);
void setInstrument(void);
float getVolume(void);
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
