
#include "main.h"

/* -------- Variables -------- */
uint16_t silenceBuffer[AUDIOBUFFERSIZE] = {0};
uint16_t PIANOONEBuffer[AUDIOBUFFERSIZE];     /* Array for the waveform */
uint16_t DRUMBuffer[AUDIOBUFFERSIZE];     /* Array for the waveform */
uint16_t PIANOTWOBuffer[AUDIOBUFFERSIZE];     /* Array for the waveform */
uint32_t ADC1ConvertedValue[2] = {0};

uint8_t pianoOneArray[8] = {0};
uint8_t pianoTwoArray[8] = {0};
uint8_t drumArray[8] = {0};
uint16_t DMA_timerPeriod;
uint8_t beatCounter = 0;
uint8_t beatFlag = 0;
uint8_t updateLEDs = 0;
uint8_t audioPlayingFlag = 0;
uint8_t ledOnCol = 0;
uint8_t instrument = 0;
float volume = 0;
uint8_t row = 0;
uint8_t alternate = 0;
uint8_t pauseResumeStatus = PAUSE;

float frequencyScaler = 9.99E-5;
uint16_t frequency[16] = {220, 246, 261, 293, 329, 349, 392, 440, 110, 61, 65, 73, 82, 87, 98, 55};

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


    // programFlash(); /* Uncomment if you want to program flash - ONLY RUN ONCE */

    // Calculate frequency of timer
    fTimer = 10000;
    // Calculate Tick Rate
    timerFreq = TIMER_CLOCK / TIMER6_PRESCALER;
    // Calculate period of Timer
    DMA_timerPeriod = (uint16_t)( timerFreq / fTimer );
    // Set up the micro
    Controller_Setup(DMA_timerPeriod);
    // check the tempo on the hardware and set it to initial value
    setTempo(getTempo());
    setVolume();

    while (1) {
		// check for play or pause pressed (PC2)
    	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2) == Bit_SET){
			while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2) == Bit_SET);
			if (pauseResumeStatus == PAUSE) {
				resume();
			}
			else {
				pause();
			}
		}
    	// check for clear (PC3)
    	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3) == Bit_SET){
			while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3) == Bit_SET);
			stop();
			continue;
		}

    	// update the LEDs
    	if (updateLEDs == 1) {
			if (ledOnCol != beatCounter) {
				if (instrument == 0) {
					SPI_SendLEDData(0x0, 0x0, drumArray[ledOnCol], ledOnCol);
					GPIO_SetBits(GPIOA,  GPIO_Pin_8);
					GPIO_ResetBits(GPIOA, GPIO_Pin_10 | GPIO_Pin_15);
					GPIO_ResetBits(GPIOC, GPIO_Pin_6);
				}
				else if (instrument == 1) {
					SPI_SendLEDData(0x0, 0x0, pianoOneArray[ledOnCol], ledOnCol);
					GPIO_SetBits(GPIOA,  GPIO_Pin_15);
					GPIO_ResetBits(GPIOA, GPIO_Pin_10 | GPIO_Pin_8);
					GPIO_ResetBits(GPIOC, GPIO_Pin_6);
				}
				else if (instrument == 2) {
					SPI_SendLEDData(0x0, 0x0, pianoTwoArray[ledOnCol], ledOnCol);
					GPIO_SetBits(GPIOC,  GPIO_Pin_6);
					GPIO_ResetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_15);
				}
				else {

					GPIO_SetBits(GPIOA,  GPIO_Pin_10);
					GPIO_ResetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_15);
					GPIO_ResetBits(GPIOC, GPIO_Pin_6);
				}
			}
			else {
				if (instrument == 0) {
					SPI_SendLEDData(getDrumBeat(beatCounter), ~getDrumBeat(beatCounter), 0x0, beatCounter);
				}
				else if (instrument == 1) {
					SPI_SendLEDData(getPianoOneBeat(beatCounter), ~getPianoOneBeat(beatCounter), 0x0, beatCounter);
				}
				else if (instrument == 2) {
					SPI_SendLEDData(getPianoTwoBeat(beatCounter), ~getPianoTwoBeat(beatCounter), 0x0, beatCounter);
				}
				STM_EVAL_LEDToggle(LED5);
			}
			updateLEDs = 0;
			// update from all controllers

			if (alternate == 1){
				row++;
				if (row > 7) row = 0;
				checkButtons(row);
				alternate = 0;
			}
			else {
				alternate++;
			}


			setTempo(getTempo());
			setVolume();
			setInstrument();
    	}

    	// play the audio
		if(beatFlag == 1) {
			// play the audio
			uint8_t playBeat = beatCounter + 1;
			if (beatCounter == 7) {
				playBeat = 0;
			}
			addDrumBeat(getDrumBeat(playBeat));
			addPianoOneBeat(getPianoOneBeat(playBeat));
			addPianoTwoBeat(getPianoTwoBeat(playBeat));
			play();

			//update the beat counter and beat flag
			beatCounter++;
			if(beatCounter>=8) beatCounter = 0;
			beatFlag = 0;
		}
    }
}

/**
  * @brief  : checks for button presses on all grid elements and adds to respective instrument arrays
  * @param  : None
  * @retval : None
  */
void checkButtons(uint8_t row){
	//uint8_t buttonSetFlag = 0;
	//for(uint8_t row = 0; row<8; row++){
		//check row
		GPIO_ResetBits(GPIOE, 0b11111111); // reset all bits
		GPIO_SetBits(GPIOE, (1<<row)); // set current row enabled
		for(uint8_t col = 8; col<16; col++){
			//if(buttonSetFlag == 1) return;
			if(GPIO_ReadInputDataBit(GPIOE, (1<<col)) == Bit_SET){
				while(GPIO_ReadInputDataBit(GPIOE, (1<<col)) == Bit_SET);
				// if drum selected

				if (instrument == 0) {
					drumArray[col-8] = drumArray[col-8] ^ (0b10000000>>row);
					//buttonSetFlag = 1;
				}
				// else if piano one selected
				else if (instrument == 1) {
					pianoOneArray[col-8] = pianoOneArray[col-8] ^ (0b10000000>>row);
					//buttonSetFlag = 1;
				}
				// else if piano two selected
				else if (instrument == 2) {
					pianoTwoArray[col-8] = pianoTwoArray[col-8] ^ (0b10000000>>row);
					//buttonSetFlag = 1;
				}
				// delay_ms(1);
			}

		}
		GPIO_ResetBits(GPIOE, 0b11111111);
	//}
}

/* ---------- Controller Methods ---------- */
/**
  * @brief  : Gets volume
  * @param  : None
  * @retval : None
  */
float getVolume() {
	volume = ADC_Convert_Volume();
	return (volume*1.1/(double)3860)+0.33161;
}

/**
  * @brief  : Gets tempo
  * @param  : None
  * @retval : None
  */
uint8_t getTempo() {
	uint16_t tempo = ADC_Convert_Tempo();
	return (0.0208*tempo + 24.824);
}

/**
  * @brief  : Sets the period of TIM2, which is the tempo of the device
  * @param  : uint16_t bpm
  * @retval : None
  */
void setTempo(uint16_t bpm){
	uint32_t tempoPeriod = -700 * bpm + 126000;
	TIM2->ARR = tempoPeriod - 1;
}

/**
  * @brief  : Sets the volume of output buffer
  * @param  : None
  * @retval : None
  */
void setVolume() {
	volume = getVolume();
}


void stop() {
	if (instrument == 0) {
		memset(drumArray, 0b0, 8);
	}
	else if (instrument == 1) {
		memset(pianoOneArray, 0b0, 8);
	}
	else {
		memset(pianoTwoArray, 0b0, 8);
	}
}


/**
  * @brief  : Changes the buffer the DMA is sending to the DAC
  * @param  : uint16_t *waveBuffer
  * @retval : None
  */
void DMA_ChangeBuffer(uint16_t *waveBuffer){
	DMA_Cmd(DMA1_Stream5, DISABLE);
	DMA_Configuration(waveBuffer);
}

/**
  * @brief  : Sets the instrument currently being edited
  * @param  : None
  * @retval : None
  */
void setInstrument() {
	for(uint8_t button = 12; button<16; button++){
		if(GPIO_ReadInputDataBit(GPIOD, (1<<button)) == Bit_SET){
			while(GPIO_ReadInputDataBit(GPIOD, (1<<button)) == Bit_SET);
			instrument = button-12;
		}
	}
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

int ADC_Convert_Volume(){
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_144Cycles); // PC1
	ADC_SoftwareStartConv(ADC1); //Start the conversion
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)); //Processing the conversion
	return ADC_GetConversionValue(ADC1); //Return the converted data
}

int ADC_Convert_Tempo(){
	ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 1, ADC_SampleTime_144Cycles); // PC1
	ADC_SoftwareStartConv(ADC1); //Start the conversion
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC)); //Processing the conversion
	return ADC_GetConversionValue(ADC1); //Return the converted data
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

	// LED Configuration (DEBUG)
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_LEDInit(LED4);
    STM_EVAL_LEDInit(LED5);
    STM_EVAL_LEDInit(LED6);
	// Push Button Init (DEBUG)
	STM_EVAL_PBInit(BUTTON_USER, BUTTON_MODE_GPIO);

	// System Clocks Configuration
	RCC_Configuration();
	// Configure the GPIO ports
	GPIO_Configuration();
	// NVIC configuration
	NVIC_Configuration();
	// Timer Configuration
	Timer_Configuration( DMA_timerPeriod, TIMER6_PRESCALER);
	// DAC Configuration
	DAC_Configuration();
	// DMA Config
	DMA_Configuration(silenceBuffer);
	DMA0_Configuration();
	ADC_Configuration();
	// SPI Config
	SPI_Configuration();
}

/* ---------- Helper Functions ---------- */
void delay_ms(uint32_t milli)
{
	uint32_t delay = milli * 17612;              // approximate loops per ms at 168 MHz, Debug config
	for(; delay != 0; delay--);
}


// Here to prevent Atollic from complaining
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size){
  /* TODO, implement your code here */
  return;
}

uint16_t EVAL_AUDIO_GetSampleCallBack(void){
  /* TODO, implement your code here */
  return -1;
}
