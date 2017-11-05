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
// Private Includes
#include "flash.h"
#include "wavecreator.h"

/* ----- Macros ------*/
#define AUDIOBUFFERSIZE         4500
#define WAVEFREQ            	880 	/* 880Hz --> A5 */
#define TIMER6_PRESCALER    	2     	/* produces a 42MHz tick */
#define TIMER_CLOCK           	84E6 	/* TIM6 runs at 84MHz */

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* ----- Variables ------ */
extern uint16_t silenceBuffer[AUDIOBUFFERSIZE];
extern uint16_t AUDIOBuffer[AUDIOBUFFERSIZE];     /* Array for the waveform */
extern uint16_t DRUMBuffer[AUDIOBUFFERSIZE];     /* Array for the waveform */
extern uint16_t PIANOTWOBuffer[AUDIOBUFFERSIZE];     /* Array for the waveform */
extern uint8_t beatCounter;
extern uint8_t beatFlag;
extern uint8_t audioPlayingFlag;
extern uint8_t changeToSilenceFlag;

extern float frequencyScaler;
extern uint16_t frequency[16];

/*----- Function Prototypes ------*/
void RCC_Configuration(void);
void DMA_Configuration(uint16_t * waveBuffer);
void DMA_ChangeBuffer(uint16_t *waveBuffer);
void NVIC_Configuration(void);
void GPIO_Configuration(void);
void UART_Configuration(void);
void Timer_Configuration(uint16_t wavPeriod, uint16_t preScaler);
void DAC_Configuration(void);
void delay_ms(uint32_t milli);
void Controller_Setup(uint16_t DMA_timerPeriod);
void setTempo(uint16_t bpm);
uint8_t getDrumBeat(uint8_t beat);
uint8_t getPianoBeat(uint8_t beat);
void addDrumBeat(uint8_t beat);
void addPianoBeat(uint8_t beat);
void addPianoTwoBeat(uint8_t beat);
void play(void);
void pause(void);
void resume(void);

void EXTI_Configuration(void);
void SPI_Configuration(void);
void SPI_SendLEDData(char red, char green, char blue, char column);



#endif /* MAIN_H_ */
