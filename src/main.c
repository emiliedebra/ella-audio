/* Includes */
#include "main.h"

/* -------- Variables -------- */
uint16_t silenceBuffer[AUDIOBUFFERSIZE] = {0};
uint16_t AUDIOBuffer[AUDIOBUFFERSIZE];     /* Array for the waveform */
uint16_t DRUMBuffer[AUDIOBUFFERSIZE];     /* Array for the waveform */
uint16_t PIANOTWOBuffer[AUDIOBUFFERSIZE];     /* Array for the waveform */
extern unsigned char violin_g3[];
uint8_t beatCounter = 0;
uint8_t beatFlag = 0;
uint8_t audioPlayingFlag = 0;

enum pauseResume { PAUSE = 0, RESUME = 1 };
uint8_t pauseResumeStatus = PAUSE;

float frequencyScaler = 9.99E-5;
uint16_t frequency[16] = {220, 246, 261, 293, 329, 349, 392, 440, 110, 123, 65, 73, 82, 87, 98, 55};

/* -------- Handlers --------- */
/*******************************************************************************
* Function Name  : TIM2_IRQHandler
* Description    : This function handles TIM2 global interrupt request.
* 				   This timer represents the tempo of the instrument and
* 				   thus the interrupt causes the beat.
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


/**
**===========================================================================
**
**  Main Program
**
**===========================================================================
*/
int main(void)
{
    uint32_t fTimer;
    uint32_t timerFreq;
    uint16_t DMA_timerPeriod;

    programFlash(); /* Uncomment if you want to program flash - ONLY RUN ONCE */

    /* Calculate frequency of timer */
    fTimer = 10000;
    /* Calculate Tick Rate */
    timerFreq = TIMER_CLOCK / TIMER6_PRESCALER; /* Timer tick is in Hz */ //42MHz
    /* Calculate period of Timer */
    DMA_timerPeriod = (uint16_t)( timerFreq / fTimer );

    /* Set up the micro */
    Controller_Setup(DMA_timerPeriod);

    //check the tempo on the hardware and set it to initial value
    setTempo(60);

    while (1) {
		// check for play or pause pressed
    	if (STM_EVAL_PBGetState(BUTTON_USER) == Bit_SET) {

			/* Debounce */
			while(STM_EVAL_PBGetState(BUTTON_USER) == Bit_SET);

			if (pauseResumeStatus == PAUSE) {
				resume();
			}
			else {
				pause();
			}
			/* Debounce */
			delay_ms(1);
    	}

    	if(beatFlag == 1){

			//play the audio
			addDrumBeat(getDrumBeat(beatCounter));
			//addDrumBeat(0x0);
//			addPianoBeat(0x0);
//			addPianoBeat(getPianoBeat(beatCounter));
    		addPianoTwoBeat(getPianoBeat(beatCounter));
			// TODO: change the tempo or volume if it needs to change
			play();
//    		fillPianoBuffer((uint32_t)violin_g3);
//    		DMA_ChangeBuffer(AUDIOBuffer);
//    		audioPlayingFlag = 1;
			//update the beat counter and beat flag
			beatCounter++;
			if(beatCounter>=8) beatCounter = 0;
			beatFlag = 0;
		}
    }
}

/* ---------- Controller Methods ---------- */

// DEBUG PURPOSES ONLY
uint8_t getDrumBeat(uint8_t beat){
	uint8_t result = 0;

	if(beat == 0){
		result = 0b00010101;
	}
	else if (beat == 1){
		result = 0b00101011;
	}
	else if (beat == 2){
		result = 0b00001011;
	}
	else if (beat == 3){
		result = 0b00010001;
	}
	else if (beat == 4){
		result = 0b00010101;
	}
	else if (beat == 5){
		result = 0b00001011;
	}
	else if (beat == 6){
		result = 0b00000001;
	}
	else if (beat == 7){
		result = 0b00010101;
	}
	return result;
}

// DEBUG PURPOSES ONLY
uint8_t getPianoBeat(uint8_t beat){
	uint8_t result = 0;

	if(beat == 0){
		result = 0b00010101;
	}
	else if (beat == 1){
		result = 0b00101010;
	}
	else if (beat == 2){
		result = 0b00001010;
	}
	else if (beat == 3){
		result = 0b00010000;
	}
	else if (beat == 4){
		result = 0b00010101;
	}
	else if (beat == 5){
		result = 0b00001010;
	}
	else if (beat == 6){
		result = 0b00000000;
	}
	else if (beat == 7){
		result = 0b00010101;
	}
	return result;
}

/**
  * @brief  : works out which notes need to be played for the piano
  * @param  : uint8_t beat
  * @retval : None
  */
void addPianoBeat(uint8_t beat){

	fillPianoBuffer((uint32_t)silenceBuffer);
	for(int i = 0; i < 8; i++){
		if ((beat & 0b00000001) == 0b00000001){
			//then add this beat to the track
			switch(i){
				case 0: addToPianoBuffer((uint32_t)A4_START);	// A4
						break;
				case 1: addToPianoBuffer((uint32_t)G3_START);	// G3
						break;
				case 2: addToPianoBuffer((uint32_t)F3_START);	// F3
						break;
				case 3: addToPianoBuffer((uint32_t)E3_START);	// E3
						break;
				case 4: addToPianoBuffer((uint32_t)D3_START);	// D3
						break;
				case 5: addToPianoBuffer((uint32_t)C3_START);	// C3
						break;
				case 6: addToPianoBuffer((uint32_t)B3_START);	// B3
						break;
				case 7: addToPianoBuffer((uint32_t)A3_START);	// A3
						break;
				default: break;
			}
		}
		beat = beat >> 1;
	}
}

/**
  * @brief  : works out which notes need to be played for the violin
  * @param  : uint8_t beat
  * @retval : None
  */
void addPianoTwoBeat(uint8_t beat){

	fillPianoTwoBuffer((uint32_t)silenceBuffer);
	for(int i = 0; i < 8; i++){
		if ((beat & 0b00000001) == 0b00000001){
			//then add this beat to the track
			switch(i){
				case 0: addToPianoTwoBuffer((uint32_t)A1_START);	// A1
						break;
				case 1: addToPianoTwoBuffer((uint32_t)B2_START);	// B2
						break;
				case 2: addToPianoTwoBuffer((uint32_t)C2_START);	// C2
						break;
				case 3: addToPianoTwoBuffer((uint32_t)D2_START);	// D2
						break;
				case 4: addToPianoTwoBuffer((uint32_t)E2_START);	// E2
						break;
				case 5: addToPianoTwoBuffer((uint32_t)F2_START);	// F2
						break;
				case 6: addToPianoTwoBuffer((uint32_t)G2_START);	// G2
						break;
				case 7: addToPianoTwoBuffer((uint32_t)A2_START);	// A2
						break;
				default: break;
			}
		}
		beat = beat >> 1;
	}
}

/**
  * @brief  : works out which notes need to be played for the drum
  * @param  : uint8_t beat
  * @retval : None
  */
void addDrumBeat(uint8_t beat){
	fillDrumBuffer((uint32_t)silenceBuffer);
	for(int i = 0; i < 8; i++){
		if ((beat & 0b00000001) == 0b00000001){
			//then add this beat to the track
			switch(i) {
			case 0: addToDrumBuffer((uint32_t)KICK_START);		// KICK
					break;
			case 1: addToDrumBuffer((uint32_t)SNARE_START);		// SNARE
					break;
			case 2: addToDrumBuffer((uint32_t)FLOORTOM_START);	// FLOOR TOM
					break;
			case 3: addToDrumBuffer((uint32_t)HITOM_START);		// HI TOM
					break;
			case 4: addToDrumBuffer((uint32_t)CHIHAT1_START);	// CLOSED HI HAT 1
					break;
			case 5: addToDrumBuffer((uint32_t)CHIHAT2_START);	// CLOSED HI HAT 2
					break;
			case 6: addToDrumBuffer((uint32_t)OHIHAT_START);	// OPEN HI HAT
					break;
			case 7: addToDrumBuffer((uint32_t)CRASH_START);		// CRASH
					break;
			default: break;
			}
		}
		beat = beat >> 1;
	}
}

/**
  * @brief Combines all instruments and plays
  * @param  : None
  * @retval : None
  */
void play() {
	addToDrumBuffer((uint32_t)AUDIOBuffer);
	addToDrumBuffer((uint32_t)PIANOTWOBuffer);
	DMA_ChangeBuffer(DRUMBuffer);
	audioPlayingFlag = 1;
}

// disable the timer
void pause() {
	TIM_Cmd(TIM2, DISABLE);
	pauseResumeStatus = PAUSE;
}

// re-enable the timer
void resume() {
	TIM_Cmd(TIM2, ENABLE);
	pauseResumeStatus = RESUME;
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

/* ------------ Configuration Methods ----------- */
/**
 * Sets up all the peripherals and background operations on the micro.
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

	/* NVIC configuration */
	NVIC_Configuration();

	/* Configure the GPIO ports */
	GPIO_Configuration();

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
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval : None
  */
void RCC_Configuration(void)
{
    /* Enable DMA and GPIOA Clocks */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1 | RCC_AHB1Periph_GPIOA, ENABLE);

    /* Enable DAC1 and TIM6 & TIM2 clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC | RCC_APB1Periph_TIM6 | RCC_APB1Periph_TIM2, ENABLE);
}

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval : None
  */
void NVIC_Configuration(void)
{
	//Sets up the IRQ Handler for TIMER 2
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    //Sets up the IRQ Handler for the DMA transfer
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
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
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;

    /* Call Init function */
    GPIO_Init(GPIOA, &GPIO_InitStruct);

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
    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(waveBuffer);				//what data we are linking to the DMA
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


/* ---------- Helper Functions ---------- */
void delay_ms(uint32_t milli)
{
	uint32_t delay = milli * 17612;              // approximate loops per ms at 168 MHz, Debug config
	for(; delay != 0; delay--);
}


// Here to prevent Atollic from complaining
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
