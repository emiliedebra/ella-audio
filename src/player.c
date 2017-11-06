#include "main.h"

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
				case 0: addToPianoOneBuffer((uint32_t)A3_START);	// A3
						break;
				case 1: addToPianoOneBuffer((uint32_t)B3_START);	// C3
						break;
				case 2: addToPianoOneBuffer((uint32_t)C3_START);	// B3
						break;
				case 3: addToPianoOneBuffer((uint32_t)D3_START);	// D3
						break;
				case 4: addToPianoOneBuffer((uint32_t)E3_START);	// E3
						break;
				case 5: addToPianoOneBuffer((uint32_t)F3_START);	// F3
						break;
				case 6: addToPianoOneBuffer((uint32_t)G3_START);	// G3
						break;
				case 7: addToPianoOneBuffer((uint32_t)A4_START);	// A4
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
				case 1: addToPianoTwoBuffer((uint32_t)B1_START);	// B1
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
			case 2: addToDrumBuffer((uint32_t)CHIHAT1_START);	// CLOSED HI HAT 1
					break;
			case 3: addToDrumBuffer((uint32_t)FLOORTOM_START);	// FLOOR TOM
					break;
			case 4: addToDrumBuffer((uint32_t)OHIHAT_START);	// OPEN HI HAT
					break;
			case 5: addToDrumBuffer((uint32_t)HITOM_START);		// HI TOM
					break;
			case 6: addToDrumBuffer((uint32_t)CHIHAT2_START);	// CLOSED HI HAT 2
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
