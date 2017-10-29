//    /* Create wave table for sine wave with ADSR Envelope */
//#include "stm32f4xx.h"
//#include "math.h"
//#define AUDIOBUFFERSIZE         9000
//void sinADSR(void);
//uint16_t freq = 440;
//float frequencyScaler = 9.999E-5;
//uint16_t AUDIOBuffer[AUDIOBUFFERSIZE];
//
//void sinADSR() {
//	for (uint16_t n = 0; n < AUDIOBUFFERSIZE; n++)
//	{
//		// AUDIOBuffer[n] += 0.1*(uint16_t)((0xFFF+1)/2)*sin((2*M_PI*n*freq*frequencyScaler)+1);
//		// attack
//		if (n <= 1000) {
//			AUDIOBuffer[n] = (uint16_t)((sin((2*M_PI*n*freq*frequencyScaler)+1)*n*2047)/1000);
//		}
//		// decay
//		else if (n <= 5000) {
//			int16_t num = (n*-41)/400 + 2150;
//			AUDIOBuffer[n] = (uint16_t)(sin((2*M_PI*n*freq*frequencyScaler)+1)*num);
//		}
//		// sustain
//		else if (n <= 8000) {
//			AUDIOBuffer[n] = (uint16_t)(1637*sin((2*M_PI*n*freq*frequencyScaler)+1));
//		}
//		// release
//		else {
//			uint16_t number = -((n*1637)/1000) + 14733;
//			AUDIOBuffer[n] = (uint16_t)(sin((2*M_PI*n*freq*frequencyScaler)+1)*number);
//		}
//	//		AUDIOBuffer[n] += 0.1*(uint16_t)((0xFFF+1)/2)*sin((2*M_PI*n*freq*frequencyScaler)+1);
//	}
//}
//
