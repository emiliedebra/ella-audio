
#include "main.h"

/* -------- Variables -------- */
uint16_t silenceBuffer[AUDIOBUFFERSIZE] = {0};
uint16_t PIANOONEBuffer[AUDIOBUFFERSIZE];     /* Array for the waveform */
uint16_t DRUMBuffer[AUDIOBUFFERSIZE];     /* Array for the waveform */
uint16_t PIANOTWOBuffer[AUDIOBUFFERSIZE];     /* Array for the waveform */
uint32_t ADC1ConvertedValue[2] = {0};
// hardcoded here for now
//uint8_t pianoOneArray[8] = {0b10000000, 0b01000000, 0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010, 0b00000001};
//uint8_t pianoTwoArray[8] = {0b10000000, 0b01000000, 0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010, 0b00000001};
//uint8_t drumArray[8] = {0b10000000, 0b01000000, 0b00100000, 0b00010000, 0b00001000, 0b00000100, 0b00000010, 0b00000001};

uint8_t pianoOneArray[8] = {0};
uint8_t pianoTwoArray[8] = {0};
uint8_t drumArray[8] = {0};

uint8_t beatCounter = 0;
uint8_t beatFlag = 0;
uint8_t updateLEDs = 0;
uint8_t audioPlayingFlag = 0;
uint8_t ledOnCol = 0;
uint8_t instrument = 0;

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
* Function Name  : TIM3_IRQHandler
* Description    : This function handles TIM3 global interrupt request.
* 				   This timer represents the tempo of the instrument and
* 				   thus the interrupt causes the beat.
*******************************************************************************/
void TIM3_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    STM_EVAL_LEDToggle(LED6);
    //change the LED to show the beat
    ledOnCol = (ledOnCol+1)&7;
    updateLEDs = 1;
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

    // programFlash(); /* Uncomment if you want to program flash - ONLY RUN ONCE */

    /* Calculate frequency of timer */
    fTimer = 10000;
    /* Calculate Tick Rate */
    timerFreq = TIMER_CLOCK / TIMER6_PRESCALER; /* Timer tick is in Hz */ //42MHz
    /* Calculate period of Timer */
    DMA_timerPeriod = (uint16_t)( timerFreq / fTimer );

    /* Set up the micro */
    Controller_Setup(DMA_timerPeriod);

    //check the tempo on the hardware and set it to initial value
    setTempo(110);

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

    	// check for instrument press
    	if (1) {
    		instrument = 0;
    	}

    	// update the LEDs
    	if (updateLEDs == 1) {
			if (ledOnCol != beatCounter) {
				if (instrument == 0) {
					SPI_SendLEDData(0x0, 0x0, drumArray[ledOnCol], ledOnCol);
				}
				else if (instrument == 1) {
					SPI_SendLEDData(0x0, 0x0, pianoOneArray[ledOnCol], ledOnCol);
				}
				else if (instrument == 2) {
					SPI_SendLEDData(0x0, 0x0, pianoOneArray[ledOnCol], ledOnCol);
				}
			}
			else {
				uint8_t LEDbeat = 0;
				if (beatCounter == 0) {
					LEDbeat = 7;
				}
				else {
					LEDbeat = beatCounter - 1;
				}


				if (instrument == 0) {
					SPI_SendLEDData(getDrumBeat(LEDbeat), ~getDrumBeat(LEDbeat), 0x0, LEDbeat);
				}
				else if (instrument == 1) {
					SPI_SendLEDData(getPianoOneBeat(LEDbeat), ~getPianoOneBeat(LEDbeat), 0x0, LEDbeat);
				}
				else if (instrument == 2) {
					SPI_SendLEDData(getPianoTwoBeat(LEDbeat), ~getPianoTwoBeat(LEDbeat), 0x0, LEDbeat);
				}
				STM_EVAL_LEDToggle(LED5);
			}
			updateLEDs = 0;
			checkButtons();
			// setTempo(getTempo());
			setInstrument();
    	}

    	// play the audio
		if(beatFlag == 1) {
			// play the audio
			addDrumBeat(getDrumBeat(beatCounter));
			addPianoOneBeat(getPianoOneBeat(beatCounter));
			addPianoTwoBeat(getPianoTwoBeat(beatCounter));
			// TODO: change the tempo or volume if it needs to change
			play();
			//update the beat counter and beat flag
			beatCounter++;
			if(beatCounter>=8) beatCounter = 0;
			beatFlag = 0;
		}
    }
}

void checkButtons(){
	for(uint8_t row = 0; row<8; row++){
		//check row
		GPIO_ResetBits(GPIOE, 0b11111111); // reset all bits
		GPIO_SetBits(GPIOE, (1<<row)); // set current row enabled
		for(uint8_t col = 8; col<16; col++){
			if(GPIO_ReadInputDataBit(GPIOE, (1<<col)) == Bit_SET){
				while(GPIO_ReadInputDataBit(GPIOE, (1<<col)) == Bit_SET);
				// if drum selected
				if (instrument == 0) {
					drumArray[col-8] = drumArray[col-8] ^ (0b10000000>>row);
				}
				// else if piano one selected
				else if (instrument == 1) {
					pianoOneArray[col-8] = pianoOneArray[col-8] ^ (0b10000000>>row);
				}
				// else if piano two selected
				else if (instrument == 2) {
					pianoTwoArray[col-8] = pianoTwoArray[col-8] ^ (0b10000000>>row);
				}
				delay_ms(1);
			}
		}
	}
}

/* ---------- Controller Methods ---------- */

uint8_t getDrumBeat(uint8_t beat){
	return drumArray[beat];
}

uint8_t getPianoOneBeat(uint8_t beat){
	return pianoOneArray[beat];
}

uint8_t getPianoTwoBeat(uint8_t beat) {
	return pianoTwoArray[beat];
}

/**
  * @brief  : works out which notes need to be played for the piano
  * @param  : uint8_t beat
  * @retval : None
  */
void addPianoOneBeat(uint8_t beat){

	fillPianoOneBuffer((uint32_t)silenceBuffer);
	for(int i = 0; i < 8; i++){
		if ((beat & 0b00000001) == 0b00000001){
			//then add this beat to the track
			switch(i){
				case 0: addToPianoOneBuffer((uint32_t)A4_START);	// A4
						break;
				case 1: addToPianoOneBuffer((uint32_t)G3_START);	// G3
						break;
				case 2: addToPianoOneBuffer((uint32_t)F3_START);	// F3
						break;
				case 3: addToPianoOneBuffer((uint32_t)E3_START);	// E3
						break;
				case 4: addToPianoOneBuffer((uint32_t)D3_START);	// D3
						break;
				case 5: addToPianoOneBuffer((uint32_t)C3_START);	// C3
						break;
				case 6: addToPianoOneBuffer((uint32_t)B3_START);	// B3
						break;
				case 7: addToPianoOneBuffer((uint32_t)A3_START);	// A3
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
	addToDrumBuffer((uint32_t)PIANOONEBuffer);
	addToDrumBuffer((uint32_t)PIANOTWOBuffer);
	// get Volume
//	float volume = getVolume();
//	for (int i = 0; i < AUDIOBUFFERSIZE; i++) {
//		DRUMBuffer[i] = volume*DRUMBuffer[i];
//	}
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

float getVolume() {
	return ADC1ConvertedValue[1];
}

uint8_t getTempo() {
	return ADC1ConvertedValue[0];
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

void setInstrument() {
	for(uint8_t button = 12; button<16; button++){
		if(GPIO_ReadInputDataBit(GPIOD, (1<<button)) == Bit_SET){
			while(GPIO_ReadInputDataBit(GPIOD, (1<<button)) == Bit_SET);
			instrument = button-12;
		}
	}
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

	/* Configure the GPIO ports */
	GPIO_Configuration();


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

	//SPI Config
	SPI_Configuration();
}

/**
  * @brief  Configures the different system clocks.
  * @param  None
  * @retval : None
  */
void RCC_Configuration(void)
{
    /* Enable DMA and GPIOA Clocks */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1 | RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE , ENABLE);

    /* Enable DAC1 and TIM6 & TIM2 clocks */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC | RCC_APB1Periph_TIM6 | RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3, ENABLE);

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
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // Set up IRQ Handler for TIMER 3
//    NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

    //Sets up the IRQ Handler for the DMA transfer
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Stream5_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
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
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_SPI2);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_SPI2);

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStructure);

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
	GPIO_ResetBits(GPIOB, GPIO_Pin_12);
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET);
	SPI_SendData(SPI2,~red);
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET);
	SPI_SendData(SPI2,~green);
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET);
	SPI_SendData(SPI2,~blue);
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET);
	SPI_SendData(SPI2,pow(2,7-column));
	while(SPI_I2S_GetFlagStatus(SPI2,SPI_I2S_FLAG_TXE)==RESET);
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_BSY) == SET);
	GPIO_SetBits(GPIOB, GPIO_Pin_12);
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

    /* Set pin PE8-15 as input for interrupts */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 |
							   GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE, &GPIO_InitStruct);


	/* Set up the outputs for the button rows */
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
							   GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOE, &GPIO_InitStruct);

	//set these pins high
	GPIO_SetBits(GPIOE, GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 |
			   	   	    GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);

	// Pins for Volume and Tempo
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;// PC0, PC1
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN; //The pins are configured in analog mode
//	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL ; //We don't need any pull up or pull down
//	GPIO_Init(GPIOC, &GPIO_InitStruct); //Initialize GPIOC pins with the configuration
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_1; //PA1
//	GPIO_Init(GPIOA, &GPIO_InitStruct); //Initialize GPIOA pins with the configuration
//
//	// output LEDs for instrument
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_15 ;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);

	//set these pins high
	GPIO_SetBits(GPIOA,  GPIO_Pin_10 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_15);

	// Pins for input buttons play/pause
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruct);

	// Pins for input buttons instruments
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOD, &GPIO_InitStruct);
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

//    // Channel 0 Stream 0 init (for ADC)
//    DMA_InitStructure.DMA_Channel = DMA_Channel_2;
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;//ADC3's data register
//	DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)&ADC1ConvertedValue;
//	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralToMemory;
//	DMA_InitStructure.DMA_BufferSize = 2;
//	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//Reads 16 bit values
//	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//Stores 16 bit values
//	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
//	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
//	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
//	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//	DMA_Init(DMA2_Stream0, &DMA_InitStructure);
//	DMA_Cmd(DMA2_Stream0, ENABLE);
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

    /* TIM3 Set Up */
	TIM_TimeBaseStruct.TIM_Period = 50-1;
	TIM_TimeBaseStruct.TIM_Prescaler = 1000-1;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStruct);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
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

int ADC_Convert(){
//	ADC_SoftwareStartConv(ADC1); //Start the conversion
//	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)); //Processing the conversion
//	return ADC_GetConversionValue(ADC1); //Return the converted data
}

void ADC_Configuration(){

//	ADC_InitTypeDef ADC_init_structure; //Structure for ADC configuration
//	ADC_CommonInitTypeDef ADC_CommonInitStruct;
//	// Clock configuration
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); //The ADC1 is connected the APB2 peripheral bus thus we will use its clock source
//	RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOCEN, ENABLE); //Clock for the ADC port!! Do not forget about this one ;)
//
//	ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
//	ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div2;
//	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
//	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
//	ADC_CommonInit(&ADC_CommonInitStruct);
//	// ADC structure configuration
//	ADC_DeInit();
//	ADC_init_structure.ADC_DataAlign = ADC_DataAlign_Right; //data converted will be shifted to right
//	ADC_init_structure.ADC_Resolution = ADC_Resolution_12b; //Input voltage is converted into a 12bit number giving a maximum value of 4096
//	ADC_init_structure.ADC_ContinuousConvMode = ENABLE; //the conversion is continuous, the input data is converted more than once
//	ADC_init_structure.ADC_ExternalTrigConv = 0;
//	ADC_init_structure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; //no trigger for conversion
//	ADC_init_structure.ADC_NbrOfConversion = 2; //I think this one is clear :p
//	ADC_init_structure.ADC_ScanConvMode = DISABLE; //The scan is configured in one channel
//	ADC_Init(ADC1,&ADC_init_structure); //Initialize ADC with the previous configuration
//	// Select the channel to be read from
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_144Cycles); // PC0
//	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 2, ADC_SampleTime_144Cycles); // PC1
//	ADC_DMARequestAfterLastTransferCmd(ADC1, ENABLE);
//	ADC_DMACmd(ADC1, ENABLE);
//	// Enable ADC conversion
//	ADC_Cmd(ADC1,ENABLE);

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
