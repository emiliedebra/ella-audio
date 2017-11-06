/*
 * config.h
 *
 *  Created on: 07 Nov 2017
 *      Author: luke
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "stm32f4xx.h"
#include "stm32f4_discovery.h"


void RCC_Configuration(void);
void NVIC_Configuration(void);
void SPI_Configuration(void);
void GPIO_Configuration(void);
void DMA_Configuration(uint16_t * waveBuffer, uint32_t bufferSize);
void Timer_Configuration(uint16_t wavPeriod, uint16_t preScaler);
void DAC_Configuration(void);
void ADC_Configuration(void);



#endif /* CONFIG_H_ */
