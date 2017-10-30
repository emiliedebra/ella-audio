#include "main.h"

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
		float fourthousand = n/(double)4000;
		uint16_t val = (uint16_t)((0xFFF+1)/2)*sin((2*M_PI*n*frequency*frequencyScaler)+1)*pow(M_E, -pow(fourthousand, 2));

		// if both sounds are quiet
		if (val < 1023 && AUDIOBuffer[n] < 1023) {
			uint16_t newVal = (uint16_t)((AUDIOBuffer[n]*val)/(double)(1023.5));
			AUDIOBuffer[n] = newVal;
		}
		// if both either sound is fairly loud
		else {
			uint16_t newVal = (uint16_t)(2*(AUDIOBuffer[n] + val) - (AUDIOBuffer[n] * val)/(double)(1023.5) - 2047);
			AUDIOBuffer[n] = newVal;
		}
	}
}
