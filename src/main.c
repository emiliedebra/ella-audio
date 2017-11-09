/*
 * The main file for the ELLA Audio Device
 */


#include "main.h"
#include "config.h"
#include "stm32f4xx_it.h"


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
    setTempo();
    setVolume();

    while (1) {
		// check for play or pause pressed (PC2)
    	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_2) == Bit_SET){
    		if( !(prevControlBtnState&(1<<2)) ){
    			(pauseResumeStatus == PAUSE) ? resume() : pause();
    		}
    		prevControlBtnState|=(1<<2);
		}
    	else{
			prevControlBtnState&=(~(1<<2));
		}
    	// check for clear board (PC3)
    	if(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_3) == Bit_SET){
			if( !(prevControlBtnState&(1<<3))) {
				clearInstrument();
			}
			prevControlBtnState|=(1<<3);
		}
    	else{
			prevControlBtnState&=(~(1<<3));
		}

    	//if we need to update the interface (LEDs and Buttons)
    	if(updateInterfaceFlag == 1){
    		updateInterface();
    		updateInterfaceFlag = 0;
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
**===========================================================================
**
**  Helper functions.
**  These are called from the main method loop to seperate out code
**
**===========================================================================
*/

/**
  * @brief  : updates the interface hardware. Includes setting LEDs, checking buttons
  * 		  reading volume and tempo potentiometers, and checking which instrument is selected
  * @param  : None
  * @retval : None
  */
void updateInterface(void){
	//display the selected instruments notes and the cursor
	setLEDs();

	//update the LED column that we are on for multiplexing
	ledOnCol = (ledOnCol+1)&7;

	// update from all controllers
	checkButtons();
	setTempo();
	setVolume();
	setInstrument();

}
/**
  * @brief  : updates the LED grid to show the selected instruments notes and the cursor
  * @param  : None
  * @retval : None
  */
void setLEDs(void){
	if (ledOnCol != beatCounter) {
		switch(currentInstrument){
			case 0: SPI_SendLEDData(0x0, 0x0, drumArray[ledOnCol], ledOnCol);
					GPIO_SetBits(GPIOA,  GPIO_Pin_8);
					GPIO_ResetBits(GPIOA, GPIO_Pin_10 | GPIO_Pin_15);
					GPIO_ResetBits(GPIOC, GPIO_Pin_6);
					break;
			case 1: SPI_SendLEDData(0x0, 0x0, pianoOneArray[ledOnCol], ledOnCol);
					GPIO_SetBits(GPIOA,  GPIO_Pin_15);
					GPIO_ResetBits(GPIOA, GPIO_Pin_10 | GPIO_Pin_8);
					GPIO_ResetBits(GPIOC, GPIO_Pin_6);
					break;
			case 2: SPI_SendLEDData(0x0, 0x0, pianoTwoArray[ledOnCol], ledOnCol);
					GPIO_SetBits(GPIOC,  GPIO_Pin_6);
					GPIO_ResetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_10 | GPIO_Pin_15);
					break;
			default:if(beatCounter==0 || beatCounter==4){
						SPI_SendLEDData(EArray[ledOnCol], 0, 0, ledOnCol);
					}
					else if(beatCounter==1 || beatCounter==5){
						SPI_SendLEDData(0, LArray[ledOnCol], 0, ledOnCol);
					}
					else if(beatCounter==2 || beatCounter==6){
						SPI_SendLEDData(0, 0, LArray[ledOnCol], ledOnCol);
					}
					else{
						SPI_SendLEDData(AArray[ledOnCol], AArray[ledOnCol], 0, ledOnCol);
					}
					GPIO_SetBits(GPIOA,  GPIO_Pin_10);
					GPIO_ResetBits(GPIOA, GPIO_Pin_8 | GPIO_Pin_15);
					GPIO_ResetBits(GPIOC, GPIO_Pin_6);
					break;
			}
	}
	//display the cursor
	else {
		switch(currentInstrument){
			case 0: SPI_SendLEDData(getDrumBeat(beatCounter), ~getDrumBeat(beatCounter), 0x0, beatCounter);
					break;
			case 1: SPI_SendLEDData(getPianoOneBeat(beatCounter), ~getPianoOneBeat(beatCounter), 0x0, beatCounter);
					break;
			case 2: SPI_SendLEDData(getPianoTwoBeat(beatCounter), ~getPianoTwoBeat(beatCounter), 0x0, beatCounter);
					break;
			default:if(beatCounter==0 || beatCounter==4){
						SPI_SendLEDData(EArray[ledOnCol], 0, 0, ledOnCol);
					}
					else if(beatCounter==1 || beatCounter==5){
						SPI_SendLEDData(0, LArray[ledOnCol], 0, ledOnCol);
					}
					else if(beatCounter==2 || beatCounter==6){
						SPI_SendLEDData(0, 0, LArray[ledOnCol], ledOnCol);
					}
					else{
						SPI_SendLEDData(AArray[ledOnCol], AArray[ledOnCol], 0, ledOnCol);
					}
					break;
			}
	}
}


/**
  * @brief  : checks for button presses on all grid elements and adds to respective instrument arrays
  * @param  : None
  * @retval : None
  */
void checkButtons(void){
	row++;
	if (row > 7) row = 0;

	GPIO_ResetBits(GPIOE, 0b11111111); // reset all bits
	GPIO_SetBits(GPIOE, (1<<row)); // set current row enabled
	for(uint8_t col = 8; col<16; col++){
		if(GPIO_ReadInputDataBit(GPIOE, (1<<col)) == Bit_SET){
			delay_ms(5);
			if(!(prevGridBtnStateArray[row]&(1<<(col-8)))){
				switch(currentInstrument){
					case 0: drumArray[col-8] = drumArray[col-8] ^ (0b10000000>>row);
							break;
					case 1: pianoOneArray[col-8] = pianoOneArray[col-8] ^ (0b10000000>>row);
							break;
					case 2: pianoTwoArray[col-8] = pianoTwoArray[col-8] ^ (0b10000000>>row);
							break;
					default: break;
				}
			}
			prevGridBtnStateArray[row]|=(1<<(col-8));

		}else{
			prevGridBtnStateArray[row]&=(~(1<<(col-8)));
		}

	}
	GPIO_ResetBits(GPIOE, 0b11111111); // reset all bits again
}


/**
  * @brief  : Gets volume
  * @param  : None
  * @retval : None
  */
void setVolume() {
	int temp = ADC_Convert_Volume();
	volume = (float)(temp*1.1/(float)3860)+0.33161;
}


/**
  * @brief  : Sets the period of TIM2, which is the tempo of the device according to the tempo POT
  * @param  : uint16_t bpm
  * @retval : None
  */
void setTempo(void){
	//get the tempo from the POT, and work out the BPM
	uint16_t tempo = ADC_Convert_Tempo();
	uint16_t bpm = 0.0208*tempo + 24.824;
	//work out the period for the Timer and update it
	uint32_t tempoPeriod = -700 * bpm + 126000;
	TIM2->ARR = tempoPeriod - 1;
}


/**
  * @brief  : Clears the current instruments' selected notes
  * @param  : uint16_t bpm
  * @retval : None
  */
void clearInstrument() {

	switch(currentInstrument){
	case 0: memset(drumArray, 0b0, 8);
			break;
	case 1: memset(pianoOneArray, 0b0, 8);
			break;
	case 2: memset(pianoTwoArray, 0b0, 8);
			break;
	default: break;
	}

}

/**
  * @brief  : Sets the instrument currently being edited
  * @param  : None
  * @retval : None
  */
void setInstrument() {
	for(uint8_t button = 12; button<16; button++){
		if(GPIO_ReadInputDataBit(GPIOD, (1<<button)) == Bit_SET){
			if( !(prevInstBtnState&(1<<(button-12))) ){
				currentInstrument = button-12;
			}
			prevInstBtnState|=(1<<(button-12));
		}else{
			prevInstBtnState&=(~(1<<(button-12)));
		}
	}
}

/**
**===========================================================================
**
**  Peripheral Access functions
**  These interface with peripherals to send/retrieve data
**
**===========================================================================
*/


/**
  * @brief  : Changes the buffer the DMA is sending to the DAC
  * @param  : uint16_t *waveBuffer
  * @retval : None
  */
void DMA_ChangeBuffer(uint16_t *waveBuffer){
	DMA_Cmd(DMA1_Stream5, DISABLE);
	DMA_Configuration(waveBuffer, (uint32_t)AUDIOBUFFERSIZE);
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


/**
**===========================================================================
**
**  Configuration functions.
**  These set up the code
**
**===========================================================================
*/
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
	DMA_Configuration(silenceBuffer, (uint32_t)AUDIOBUFFERSIZE);
	//ADC Config
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
