#ifndef	__ADC_H__
#define	__ADC_H__

#include	"ACM32F0x0.h"

typedef struct{
	uint32_t adcTemp[10];
	uint32_t adcResult;
	
}ADC_ResultDef;

void ADC_Init(uint8_t channel);
void adcMeasureStart(void);


#endif


