#ifndef		__LED_APP_H__
#define		__LED_APP_H__

#include "ACM32F0x0.h"

#define			LED_OFF				0
#define			LED_ON				1
#define			LED_TWINKLE			2
#define			LED_BREATHE			3

#define			LED_BRIGHTER		1
#define			LED_FADE			2
#define			LED_NO_BREATHE		0

#define			LED_SET_TIME(x, time)		{x->onTime = (time & 0xFFF); 	\
											 x->offTime = ((time >> 12)&0xFFF);	\
											(x->dir) = ((time >> 29) & 0x03) ;	\
											(x->reverseTime) |= ((time >> 24) & 0x1F);}

__packed typedef struct{
	uint8_t reverseTime	:	5;
	uint8_t dir			: 2;
	uint8_t onOrOff 	: 1;
}LED_BreatheDirDef;

typedef	struct{
	uint8_t state;					// 0 off, 1 always on, 2 twinkle, 3 breathe
//	LED_BreatheDirDef breatheDir;	// led bright or fade, 2 fade, 1 bright, 0 no breathe
	uint16_t onTime;				// 
	uint16_t offTime;
	uint16_t duty;					// current duty in CCR
	uint16_t cnt;					// hold time counter
	uint8_t reverseTime	:	5;
	uint8_t dir			: 2;
	uint8_t onOrOff 	: 1;
}LED_Ctrl_Def;


extern LED_Ctrl_Def led;


void appSysLedManagement(void);
void appSysLedController(void);
void appMotorModeLedControl(void);

#endif






