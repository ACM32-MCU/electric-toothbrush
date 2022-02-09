#ifndef	__APP_H__
#define	__APP_H__

#include "ACM32F0x0.h"

enum{
	SYSTEM_SHUTDOWN = 0,
	SYSTEM_SLEEPMODE,
	SYSTEM_RUNMODE
};											

typedef __packed struct{
	bool	adcWatchDogLow;
	bool	adcWatchDogHigh;
	bool	lowPowerFlag;
	bool	fullPowerFlag;
	bool	keyShorPress;
	uint8_t	chargeFlag;
	uint8_t status;
	uint8_t lowPowerCnt;
	uint8_t fullPowerCnt;
	uint8_t motorStatus;
	uint32_t times;
}System_ParaDef;

extern System_ParaDef sys;

void appInit(void);
void appSystemStatusManagement(void);
void appPowerMeasure(void);
void appGotoSleep(void);
void appMonitorSleepOrNot(void);

#endif






