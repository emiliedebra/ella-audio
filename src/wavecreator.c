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
  * @brief  fills respective buffers from flash (overwrite)
  * @param  uint16_t frequency
  * @retval : None
  */
void fillPianoOneBuffer(uint32_t frequencyADDR) {
	int offset = 0;
	uint32_t Address = frequencyADDR;
	for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
	{
		PIANOONEBuffer[n] = *(uint16_t*)(Address + offset);
		offset += 2;
	}
}

void fillDrumBuffer(uint32_t frequencyADDR) {
	int offset = 0;
	uint32_t Address = frequencyADDR;
	for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
	{
		DRUMBuffer[n] = *(uint16_t*)(Address + offset);
		offset += 2;
	}
}

void fillPianoTwoBuffer(uint32_t frequencyADDR) {
	int offset = 0;
	uint32_t Address = frequencyADDR;
	for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
	{
		PIANOTWOBuffer[n] = *(uint16_t*)(Address + offset);
		offset += 2;
	}
}

/**
  * @brief  : preps buffer for flash (overwrite)
  * @param  : uint16_t frequency
  * @retval : None
  */
void fillFlashBuffer(uint16_t frequency) {
	for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
	{
		float fourthousand = n/(double)4000;
		uint16_t val = (uint16_t)((0xFFF+1)/2)*sin((2*M_PI*n*frequency*frequencyScaler)+1)*pow(M_E, -pow(fourthousand, 2));
		PIANOONEBuffer[n] = 0.1*val;
	}
}

/**
  * @brief  : writes drum arrays from extern variables (overwrite)
  * @param  : uint16_t frequency
  * @retval : None
  */
void createDrumSynth(uint16_t drumSound) {
	switch (drumSound) {
	// Kick Drum: frequency = 43Hz
	case 0:
		for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
		{
			float threethousand = n/(double)1500;
			uint16_t val = (uint16_t)((0xFFF+1)/2)*sin((2*M_PI*n*43*frequencyScaler)+1)*pow(M_E, -pow(threethousand, 2));
			DRUMBuffer[n] = 0.2*val;
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
	default: break;
	}
}

/**
  * @brief  : writes drum arrays from extern variables (overwrite)
  * @param  : uint16_t frequency
  * @retval : None
  */
void createViolinSynth(uint16_t violinSound) {
//	switch (violinSound) {
//	// A2
//	case 0:
//		for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
//		{
//			float threethousand = n/(double)500;
//			VIOLINBuffer[n] = 0.2*violin_a2[n];
//			VIOLINBuffer[n] *= pow(M_E, -pow(threethousand, 2));
//		}
//		break;
//	// B2
//	case 1:
//		for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
//		{
//			float threethousand = n/(double)500;
//			VIOLINBuffer[n] = violin_b2[n];
//			VIOLINBuffer[n] *= pow(M_E, -pow(threethousand, 2));
//		}
//		break;
//	// C3
//	case 2:
//		for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
//		{
//			float threethousand = n/(double)500;
//			VIOLINBuffer[n] = violin_c3[n];
//			VIOLINBuffer[n] *= pow(M_E, -pow(threethousand, 2));
//		}
//		break;
//	// D3
//	case 3:
//		for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
//		{
//			float threethousand = n/(double)500;
//			VIOLINBuffer[n] = violin_d3[n];
//			VIOLINBuffer[n] *= pow(M_E, -pow(threethousand, 2));
//		}
//		break;
//	// E3
//	case 4:
//		for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
//		{
//			float threethousand = n/(double)500;
//			VIOLINBuffer[n] = violin_e3[n];
//			VIOLINBuffer[n] *= pow(M_E, -pow(threethousand, 2));
//		}
//		break;
//	// F3
//	case 5:
//		for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
//		{
//			float threethousand = n/(double)2000;
//			VIOLINBuffer[n] = violin_f3[n];
//			VIOLINBuffer[n] *= pow(M_E, -pow(threethousand, 2));
//		}
//		break;
//	// G3
//	case 6:
//		for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
//		{
//			float threethousand = n/(double)500;
//			VIOLINBuffer[n] = violin_g3[n];
//			VIOLINBuffer[n] *= pow(M_E, -pow(threethousand, 2));
//		}
//		break;
//	// A3
//	case 7:
//		for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
//		{
//			float threethousand = n/(double)5000;
//			VIOLINBuffer[n] = 0.2*violin_a3[n];
//			VIOLINBuffer[n] *= pow(M_E, -pow(threethousand, 2));
//		}
//		break;
//	default: break;
//	}
}

/**
  * @brief  adds wave from flash to respective buffer as mixer function (no overwrite)
  * @param  uint16_t frequency
  * @retval : None
  */
void addToPianoOneBuffer(uint32_t frequencyADDR) {
	uint32_t Address = frequencyADDR;
	for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
	{
		PIANOONEBuffer[n] = volume*(uint16_t)(PIANOONEBuffer[n] + *(uint16_t*)(Address) - (PIANOONEBuffer[n]*(*(uint16_t*)(Address)))/(double)2048);
		Address += 2;
	}
}

void addToDrumBuffer(uint32_t frequencyADDR) {
	uint32_t Address = frequencyADDR;
	for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
	{
		DRUMBuffer[n] = volume*(uint16_t)(DRUMBuffer[n] + *(uint16_t*)(Address) - (DRUMBuffer[n]*(*(uint16_t*)(Address)))/(double)2048);
		Address += 2;
	}
}

void addToPianoTwoBuffer(uint32_t frequencyADDR) {
	uint32_t Address = frequencyADDR;
	for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
	{
		PIANOTWOBuffer[n] = volume*(uint16_t)(PIANOTWOBuffer[n] + *(uint16_t*)(Address) - (PIANOTWOBuffer[n]*(*(uint16_t*)(Address)))/(double)2048);
		Address += 2;
	}
}
