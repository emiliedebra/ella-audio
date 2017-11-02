#include "wavecreator.h"

extern unsigned char snare[];
extern unsigned char floorTom[];
extern unsigned char bass[];
extern unsigned char closedHiHat1[];
extern unsigned char closedHiHat2[];
extern unsigned char crash[];
extern unsigned char hiTom[];
extern unsigned char openHiHat[];
/**
  * @brief  fills buffer (overwrite)
  * @param  uint16_t frequency
  * @retval : None
  */
void fillBuffer(uint32_t frequencyADDR) {
	int offset = 0;
	uint32_t Address = frequencyADDR;
	for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
	{
//		float fourthousand = n/(double)4000;
	//	uint16_t val = (uint16_t)((0xFFF+1)/2)*sin((2*M_PI*n*frequency*frequencyScaler)+1)*pow(M_E, -pow(fourthousand, 2));
		AUDIOBuffer[n] = *(uint16_t*)(Address + offset);
		offset += 2;
	}
}

/**
  * @brief  fills buffer for flash (overwrite)
  * @param  uint16_t frequency
  * @retval : None
  */
void fillFlashBuffer(uint16_t frequency) {
	for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
	{
		float fourthousand = n/(double)4000;
		uint16_t val = (uint16_t)((0xFFF+1)/2)*sin((2*M_PI*n*frequency*frequencyScaler)+1)*pow(M_E, -pow(fourthousand, 2));
		AUDIOBuffer[n] = 0.2*val;
	}
}

void createDrumSynth(uint16_t drumSound) {
	switch (drumSound) {
	// Kick Drum: frequency = 43Hz
	case 0:
		for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
		{
			float threethousand = n/(double)1500;
			uint16_t val = (uint16_t)((0xFFF+1)/2)*sin((2*M_PI*n*43*frequencyScaler)+1)*pow(M_E, -pow(threethousand, 2));
			DRUMBuffer[n] = 0.2*val;
//			threethousand = n/(double)500;
//			DRUMBuffer[n] += 0.03*bass[n]*pow(M_E, -pow(threethousand, 2));
//			DRUMBuffer[n] *= pow(M_E, -pow(threethousand, 2));
		}
		break;
	// Snare Drum
	case 1:
		for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
		{
			float threethousand = n/(double)500;
			DRUMBuffer[n] = snare[n];
			DRUMBuffer[n] *= pow(M_E, -pow(threethousand, 2));
		}
		break;
	// Floor Tom
	case 2:
		for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
		{
			float threethousand = n/(double)1500;
			uint16_t val = (uint16_t)((0xFFF+1)/2)*sin((2*M_PI*n*100*frequencyScaler)+1)*pow(M_E, -pow(threethousand, 2));
			DRUMBuffer[n] = 0.2*val;
			threethousand = n/(double)500;
			DRUMBuffer[n] += floorTom[n];
			DRUMBuffer[n] *= pow(M_E, -pow(threethousand, 2));
		}
		break;
	// Hi Tom
	case 3:
		for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
		{
			float threethousand = n/(double)500;
			DRUMBuffer[n] = hiTom[n];
			DRUMBuffer[n] *= pow(M_E, -pow(threethousand, 2));
		}
		break;
	// Hi Hat 1	(Closed)
	case 4:
		for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
		{
			float threethousand = n/(double)500;
			DRUMBuffer[n] = closedHiHat1[n];
			DRUMBuffer[n] *= pow(M_E, -pow(threethousand, 2));
		}
		break;
	// Hi Hat 1 (Open)
	case 5:
		for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
		{
			float threethousand = n/(double)2000;
			DRUMBuffer[n] = 0.9*openHiHat[n];
			DRUMBuffer[n] *= pow(M_E, -pow(threethousand, 2));
		}
		break;
	// Hi Hat 2 (Closed)
	case 6:
		for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
		{
			float threethousand = n/(double)500;
			DRUMBuffer[n] = closedHiHat2[n];
			DRUMBuffer[n] *= pow(M_E, -pow(threethousand, 2));
		}
		break;
	// Crash Cymbal
	case 7:
		for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
		{
			float threethousand = n/(double)5000;
			DRUMBuffer[n] = 0.8*crash[n];
			DRUMBuffer[n] *= pow(M_E, -pow(threethousand, 2));
		}
		break;
	}
}

/**
  * @brief  adds wave to buffer (no overwrite)
  * @param  uint16_t frequency
  * @retval : None
  */
void addToBuffer(uint32_t frequencyADDR) {
//	int offset = 0;
	uint32_t Address = frequencyADDR;
	for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
	{
		// float fourthousand = n/(double)4000;
		// uint16_t val = (uint16_t)((0xFFF+1)/2)*sin((2*M_PI*n*frequency*frequencyScaler)+1)*pow(M_E, -pow(fourthousand, 2));

		AUDIOBuffer[n] = (uint16_t)(AUDIOBuffer[n] + *(uint16_t*)(Address) - (AUDIOBuffer[n]*(*(uint16_t*)(Address)))/(double)2048);
		Address += 2;
		// if both sounds are quiet
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
