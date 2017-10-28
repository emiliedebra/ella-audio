/*
 * main.h
 *
 *  Created on: 28 Oct 2017
 *      Author: luke
 */

#ifndef MAIN_H_
#define MAIN_H_

/* Macro */
#define AUDIOBUFFERSIZE         9000
#define WAVEFREQ            	880 	/* 880Hz --> A5 */
#define TIMER6_PRESCALER    	2     	/* produces a 42MHz tick */
#define TIMER_CLOCK           	84E6 	/* TIM6 runs at 84MHz */

/* Variables */
uint16_t silenceBuffer[AUDIOBUFFERSIZE] = {0};
uint16_t AUDIOBuffer[AUDIOBUFFERSIZE];     /* Array for the waveform */

uint8_t beatCounter = 0;
uint8_t beatFlag = 0;
uint8_t audioPlayingFlag = 0;
uint8_t changeToSilenceFlag = 0;

/* Function Prototypes */

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



#endif /* MAIN_H_ */
