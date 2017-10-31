/**
*****************************************************************************
**
**  File        : main.c
**
**  Abstract    : main function.
**
**  Functions   : main
**
**  Environment : Atollic TrueSTUDIO(R)
**                STMicroelectronics STM32F4xx Standard Peripherals Library
**
**  Distribution: The file is distributed “as is,” without any warranty
**                of any kind.
**
**  (c)Copyright Atollic AB.
**  You may use this file as-is or modify it according to the needs of your
**  project. Distribution of this file (unmodified or modified) is not
**  permitted. Atollic AB permit registered Atollic TrueSTUDIO(R) users the
**  rights to distribute the assembled, compiled & linked contents of this
**  file as part of an application binary file, provided that it is built
**  using the Atollic TrueSTUDIO(R) toolchain.
**
**
*****************************************************************************
*/

/* Includes */
#include "stm32f4xx.h"
#include "stm32f4_discovery.h"
#include "math.h"
#include "main.h"


/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* 				   This timer represents the tempo of the instrument and
* 				   thus the interrupt causes the beat.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

    //change the LED to show the beat
    STM_EVAL_LEDToggle(LED4);
    //set the beat flag so a beat can be computed and played in the main loop
    beatFlag = 1;

  }
}

/*******************************************************************************
* Function Name  : DMA1_Stream5_IRQHandler
* Description    : This function handles the Transfer Complete Interrupt of the
* 				   DMA1 Stream 5. This checks will cause the change from audio
* 				   playing back to silence.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DMA1_Stream5_IRQHandler(void){
	if(DMA_GetITStatus(DMA1_Stream5, DMA_IT_TCIF5) == SET) {
		DMA_ClearITPendingBit(DMA1_Stream4, DMA_IT_TCIF5);
		//if audio is playing now, then we want to change back to silence
		if (audioPlayingFlag == 1){
			DMA_ChangeBuffer(silenceBuffer);
			audioPlayingFlag = 0;
		}
	}
}


/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : Handles the interrupt from PD5-9
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void) {
    /* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
        /* Do your stuff when PB12 is changed */
    	STM_EVAL_LEDToggle(LED3);
    	STM_EVAL_LEDToggle(LED5);
    	STM_EVAL_LEDToggle(LED6);

        /* Clear interrupt flag */
        EXTI_ClearITPendingBit(EXTI_Line8);
    }
    /* Make sure that interrupt flag is set */
    else if (EXTI_GetITStatus(EXTI_Line9) != RESET) {
        /* Do your stuff when PB12 is changed */
    	STM_EVAL_LEDToggle(LED3);
    	STM_EVAL_LEDToggle(LED5);
    	STM_EVAL_LEDToggle(LED6);

        /* Clear interrupt flag */
        EXTI_ClearITPendingBit(EXTI_Line9);
    }
}

/*******************************************************************************
* Function Name  : EXTI15_10_IRQHandler
* Description    : Handles the interrupt from PD10-15
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI15_10_IRQHandler(void) {
    /* Make sure that interrupt flag is set */
    if (EXTI_GetITStatus(EXTI_Line10) == SET) {
        /* Do your stuff when PD10 is changed */
    	STM_EVAL_LEDToggle(LED3);
    	STM_EVAL_LEDToggle(LED5);
    	STM_EVAL_LEDToggle(LED6);

        /* Clear interrupt flag */
        EXTI_ClearITPendingBit(EXTI_Line10);
    }
    else if (EXTI_GetITStatus(EXTI_Line11) == SET) {
        /* Do your stuff when PD10 is changed */
    	STM_EVAL_LEDToggle(LED3);
    	STM_EVAL_LEDToggle(LED5);
    	STM_EVAL_LEDToggle(LED6);

        /* Clear interrupt flag */
        EXTI_ClearITPendingBit(EXTI_Line11);
    }
    else if (EXTI_GetITStatus(EXTI_Line12) == SET) {
        /* Do your stuff when PD10 is changed */
    	STM_EVAL_LEDToggle(LED3);
    	STM_EVAL_LEDToggle(LED5);
    	STM_EVAL_LEDToggle(LED6);

        /* Clear interrupt flag */
        EXTI_ClearITPendingBit(EXTI_Line12);
    }
    else if (EXTI_GetITStatus(EXTI_Line13) == SET) {
        /* Do your stuff when PD10 is changed */
    	STM_EVAL_LEDToggle(LED3);
    	STM_EVAL_LEDToggle(LED5);
    	STM_EVAL_LEDToggle(LED6);

        /* Clear interrupt flag */
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
    else if (EXTI_GetITStatus(EXTI_Line14) == SET) {
        /* Do your stuff when PD10 is changed */
    	STM_EVAL_LEDToggle(LED3);
    	STM_EVAL_LEDToggle(LED5);
    	STM_EVAL_LEDToggle(LED6);

        /* Clear interrupt flag */
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
    else if (EXTI_GetITStatus(EXTI_Line15) == SET) {
        /* Do your stuff when PD10 is changed */
    	STM_EVAL_LEDToggle(LED3);
    	STM_EVAL_LEDToggle(LED5);
    	STM_EVAL_LEDToggle(LED6);

        /* Clear interrupt flag */
        EXTI_ClearITPendingBit(EXTI_Line15);
    }
}


/**
**===========================================================================
**
**  Abstract: main program
**
**===========================================================================
*/
int main(void)
{
    uint32_t fTimer;
    uint32_t timerFreq;
    uint16_t DMA_timerPeriod;


    /* Create wave table for sin() wave */
    fillBuffer(frequency[beatCounter]); // fill buffer with first note

    /* Calculate frequency of timer */
    fTimer = 10000;
    /* Calculate Tick Rate */
    timerFreq = TIMER_CLOCK / TIMER6_PRESCALER; /* Timer tick is in Hz */ //42MHz
    /* Calculate period of Timer */
    DMA_timerPeriod = (uint16_t)( timerFreq / fTimer );


    /* Set up the micro */
    Controller_Setup(DMA_timerPeriod);

    //check the tempo on the hardware and set it
    //TODO: CHECK TEMPO
    setTempo(60);

    while(1);

    while (1) {

    	//THIS WONT BE HERE IN THE FINAL THING
		//===========================================================================================================
		// check for button pressed
		if (STM_EVAL_PBGetState(BUTTON_USER) == Bit_SET) {
			/* Debounce */
			while(STM_EVAL_PBGetState(BUTTON_USER) == Bit_SET);

			if(audioPlayingFlag != 1){
				DMA_ChangeBuffer(AUDIOBuffer);
				audioPlayingFlag = 1;
			}
		}
		/* Debounce */
		delay_ms(1);
		//===========================================================================================================



		//handles the playing of a beat
		if(beatFlag == 1){

			// get the note array from hardware

			// compute array for the audio to be played
			/* fillBuffer(frequency[0]); // fill buffer with first note
			   for (int i = 1; i < 8; i++) {
			 	 if char at i is 1
			 	 addToBuffer(frequency[i]);
			   } */

			//play the audio
			DMA_ChangeBuffer(AUDIOBuffer);
			audioPlayingFlag = 1;

			//change the tempo if it needs to change

			//update the beat counter and beat flag
			beatCounter++;
			if(beatCounter>=8) beatCounter = 0;
			beatFlag = 0;
		}
    }
}



/**
 * Sets up all the peripherals and background shit on the micro.
 * Sets up the clocks, NVIC, GPIO pins (pins for audio out, LEDS and Push Button),
 * Timer for the DMA, the DAC, and the DMA.
 * This initializes the DMA with the silence buffer so
 * on startup the micro just plays silence.
 * Sets up the tempo of the device to 60BPM so change it immediately.
 */
void Controller_Setup(uint16_t DMA_timerPeriod){

	/* LED Configuration */
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_LEDInit(LED4);
    STM_EVAL_LEDInit(LED5);
    STM_EVAL_LEDInit(LED6);

	/* System Clocks Configuration */
	RCC_Configuration();

	/* Configure the GPIO ports */
	GPIO_Configuration();

	//set up the external interrupts
	EXTI_Configuration();

	/* NVIC configuration */
	NVIC_Configuration();

	/* Timer Configuration */
	Timer_Configuration( DMA_timerPeriod, TIMER6_PRESCALER);

	/* DAC Configuration */
	DAC_Configuration();

	/* DMA Config */
	DMA_Configuration(silenceBuffer);

	//init the push button
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);


}

/**
  * @brief  Sets up the pins PD8-15 as external interrupts
  * @param  None
  * @retval : None
  */
void EXTI_Configuration(void){
	EXTI_InitTypeDef EXTI_InitStruct;

	/* Tell system that you will use PD0 for EXTI_Line0 */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource8);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource9);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource10);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource11);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource12);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource13);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource14);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource15);


	/* PD0 is connected to EXTI_Line0 */
	EXTI_InitStruct.EXTI_Line = EXTI_Line8 | EXTI_Line9 | EXTI_Line10 | EXTI_Line11 |
								EXTI_Line12 | EXTI_Line13 | EXTI_Line14 | EXTI_Line15;
	/* Enable interrupt */
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	/* Interrupt mode */
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	/* Triggers on rising and falling edge */
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	/* Add to EXTI */
	EXTI_Init(&EXTI_InitStruct);

}


/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval : None
  */
void RCC_Configuration(void)
{
    /* Enable DMA and GPIOA Clocks */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1 | RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE, ENABLE);

    /* Enable DAC1 and TIM6 & TIM2 clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC | RCC_APB1Periph_TIM6 | RCC_APB1Periph_TIM2, ENABLE);

    /* Enable clock for SYSCFG */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval : None
  */
void NVIC_Configuration(void)
{
	//Sets up the IRQ Handler for TIMER 2 - BEAT TIMER
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //Sets up the IRQ Handler for the DMA transfer - SENDS AUDIO TO THE DAC
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Add IRQ vector to NVIC */  // - SCANS FOR BUTTON PRESSES
	/* PB 5 to 9 is connected to EXTI_Line9_5, which has EXTI9_5_IRQn vector */
    //note: everything else is set up as above so doesnt need to be changed
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	/* Add to NVIC */
	NVIC_Init(&NVIC_InitStructure);

	/* PB 10 to 15 is connected to EXTI_Line15_10, which has EXTI19_10_IRQn vector */
    //note: everything else is set up as above so doesnt need to be changed
    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	/* Add to NVIC */
	NVIC_Init(&NVIC_InitStructure);

}


/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval : None
  */
void GPIO_Configuration(void)
{
	//NOTE (from Luke): I believe this is setting up pin A4  (PA4) as the audio output

    GPIO_InitTypeDef GPIO_InitStruct;

    /* Pack the struct */
    GPIO_InitStruct.GPIO_Speed = GPIO_Mode_AN;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

    /* Call Init function */
    GPIO_Init(GPIOA, &GPIO_InitStruct);


    /* Set pin PE8-15 as input for interrupts */
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
    						   GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOE, &GPIO_InitStruct);

;

}

/**
  * @brief  fills buffer (overwrite)
  * @param  uint16_t frequency
  * @retval : None
  */
void fillBuffer(uint16_t frequency) {
	for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
	{
		float fourthousand = n/(double)4000;
		uint16_t val = (uint16_t)((0xFFF+1)/2)*sin((2*M_PI*n*frequency*frequencyScaler)+1)*pow(M_E, -pow(fourthousand, 2));
		AUDIOBuffer[n] = val;
	}
}

/**
  * @brief  adds wave to buffer (no overwrite)
  * @param  uint16_t frequency
  * @retval : None
  */
void addToBuffer(uint16_t frequency) {
	for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
	{
//		float fourthousand = n/(double)4000;
//		uint16_t val = (uint16_t)((0xFFF+1)/2)*sin((2*M_PI*n*frequency*frequencyScaler)+1)*pow(M_E, -pow(foruthousand, 2));
//
//		// if both sounds are quiet
//		if (val < 1023 && AUDIOBuffer[n] < 1023) {
//			uint16_t newVal = (uint16_t)((AUDIOBuffer[n]*val)/(double)(1023.5));
//			AUDIOBuffer[n] = newVal;
//		}
//		// if both either sound is fairly loud
//		else {
//			uint16_t newVal = (uint16_t)(2*(AUDIOBuffer[n] + val) - (AUDIOBuffer[n] * val)/(double)(1023.5) - 2047);
//			AUDIOBuffer[n] = newVal;
//		}
	}
}
/**
  * @brief  Sets the period of TIM2, which is the tempo of the device
  * @param  uint16_t bpm
  * @retval : None
  */
void setTempo(uint16_t bpm){
	uint32_t tempoPeriod = -700 * bpm + 126000;
	TIM2->ARR = tempoPeriod - 1;
}

/**
  * @brief  Changes the buffer the DMA is sending to the DAC
  * @param  uint16_t *waveBuffer
  * @retval : None
  */
void DMA_ChangeBuffer(uint16_t *waveBuffer){
	DMA_Cmd(DMA1_Stream5, DISABLE);
	DMA_Configuration(waveBuffer);
}


/**
  * @brief  Configures the DMA.
  * @param  None
  * @retval : None
  */
void DMA_Configuration(uint16_t * waveBuffer)
{
    DMA_InitTypeDef DMA_InitStructure;

    //Initialize the structure to default values
    DMA_StructInit(&DMA_InitStructure);

    /* PACK YOUR INIT STRUCT HERE */
    DMA_InitStructure.DMA_Channel = DMA_Channel_7;								//the channel we are using
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(DAC_BASE + 0x08);		//what peripheral we are linking the DMA to
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(waveBuffer);			//what data we are linking to the DMA
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;						//the direction things must flow: memory to peripheral
    DMA_InitStructure.DMA_BufferSize = (uint32_t)AUDIOBUFFERSIZE;				//the size, or length of the data
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//We dont want to change the location of the peripheral we send to, so we disable the peripheral address increment
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;						//We want to increment the address of the data we are sening (else we send the same segment of data over and o
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; //The size of the peripherals data size (so how much the peripheral can accept at one time)
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;			//The size of the memory. 16 bits. This should be the same as above, i guess?
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;								//TODO: This probably needs to change to Normal so the data is only played once
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;							//High prio?
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;						//? - dont think it is important
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;			//? - dont think it is important
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;					//? - dont think it is important
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;			//? - dont think it is important

    /* Call Init function */
    DMA_Init(DMA1_Stream5, &DMA_InitStructure);

    /* Enable the Transfer Complete Interrupt */
  	DMA_ITConfig(DMA1_Stream5, DMA_IT_TC, ENABLE);

    /* Enable DMA */
    DMA_Cmd(DMA1_Stream5, ENABLE);

}

/**
  * @brief  Configures the Timers.
  * @param  wavePeriod (period of timer), preScaler (prescaler for timer)
  * @retval : None
  */
void Timer_Configuration(uint16_t wavPeriod, uint16_t preScaler)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStruct;

    /* pack Timer struct */
    TIM_TimeBaseStruct.TIM_Period = wavPeriod-1;
    TIM_TimeBaseStruct.TIM_Prescaler = preScaler-1;
    TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStruct.TIM_RepetitionCounter = 0x0000;

    /* Call init function */
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStruct);

    /* Select Timer to trigger DAC */
    TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);

    /* TIM6 enable counter */
    TIM_Cmd(TIM6, ENABLE);

    /* TIM2 Set Up */
    TIM_TimeBaseStruct.TIM_Period = 84000-1;
    TIM_TimeBaseStruct.TIM_Prescaler = 1000-1;

    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStruct);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
}

/**
  * @brief  Configures the DAC
  * @param  None
  * @retval : None
  */
void DAC_Configuration(void)
{
    DAC_InitTypeDef DAC_InitStruct;

    /* Initialize the DAC_Trigger member */
    DAC_InitStruct.DAC_Trigger = DAC_Trigger_T6_TRGO;
    /* Initialize the DAC_WaveGeneration member */
    DAC_InitStruct.DAC_WaveGeneration = DAC_WaveGeneration_None;
    /* Initialize the DAC_LFSRUnmask_TriangleAmplitude member */
    DAC_InitStruct.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
    /* Initialize the DAC_OutputBuffer member */
    DAC_InitStruct.DAC_OutputBuffer = DAC_OutputBuffer_Enable;

    /* Init DAC */
    DAC_Init(DAC_Channel_1, &DAC_InitStruct);

    /* Enable DMA request */
    DAC_DMACmd(DAC_Channel_1, ENABLE);

    /* Enable DAC Channel1: Once the DAC channel1 is enabled, PA.04 is automatically connected to the DAC converter. */
    DAC_Cmd(DAC_Channel_1, ENABLE);

}

/**
  * @brief  Sets up the SPI for the LED control
  * @param  None
  * @retval : None
  */
void SPI_Configuration(void){

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);

	SPI_InitTypeDef SPI_InitStruct;
	SPI_InitStruct.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStruct.SPI_CRCPolynomial = 0;
	SPI_Init(SPI2, &SPI_InitStruct);
	SPI_Cmd(SPI2,ENABLE);
}
/**
  * @brief  Sends data through SPI to the LEDs to control them
  * @param  char red, char green, char blue, char column
  * @retval : None
  */
void SPI_SendLEDData(char red, char green, char blue, char column){
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET);
	GPIO_WriteBit(GPIOB, GPIO_Pin_14, RESET);
	SPI_SendData(SPI2,red);
	SPI_SendData(SPI2,green);
	SPI_SendData(SPI2,blue);
	SPI_SendData(SPI2,column);
	GPIO_WriteBit(GPIOB, GPIO_Pin_14, SET);
}

/**
  * @brief  Delays by some ammount. Should only be used for debouncing
  * @param  uint32_t milli
  * @retval : None
  */
void delay_ms(uint32_t milli)
{
	uint32_t delay = milli * 17612;              // approximate loops per ms at 168 MHz, Debug config
	for(; delay != 0; delay--);
}


/*
 * Callback used by stm32f4_discovery_audio_codec.c.
 * Refer to stm32f4_discovery_audio_codec.h for more info.
 */
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size){
  /* TODO, implement your code here */
  return;
}

/*
 * Callback used by stm324xg_eval_audio_codec.c.
 * Refer to stm324xg_eval_audio_codec.h for more info.
 */
uint16_t EVAL_AUDIO_GetSampleCallBack(void){
  /* TODO, implement your code here */
  return -1;
}
