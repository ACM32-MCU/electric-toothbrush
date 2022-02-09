#include "includes.h"

LED_Ctrl_Def led;
System_ParaDef sys;

void appInit(void)
{
	memset(&led, 0, sizeof(led));
	memset(&sys, 0, sizeof(sys));
	ReadWordFromFlash((uint32_t *)STORE_ADDR, (uint32_t *)(&sys), (sizeof(sys)));
	
	/* Is it the first time to power on ? */
	if((sys.motorStatus) == 0xFF)
	{
		memset(&sys, 0, sizeof(sys));
		WriteWordToFlash((uint32_t *)(&sys), (uint32_t *)STORE_ADDR, (sizeof(sys)));
	}
	
	sys.status = SYSTEM_SHUTDOWN;
	sys.chargeFlag = 0;
}

/**
**@name
**@input
**@return
**@brief
**/
void appSystemStatusManagement(void)
{
	switch(sys.status)
	{
		case SYSTEM_SHUTDOWN:
			/* Is it waken up by charging ? */
			if(sys.chargeFlag)		// wake up by charging
			{
				if(sys.keyShorPress)
				{
					sys.keyShorPress = false;
					sys.status = SYSTEM_RUNMODE;
				}
				else
					sys.status = SYSTEM_SLEEPMODE;
			}
			else{
				if(sys.keyShorPress)
				{
					sys.keyShorPress = false;
					sys.status = SYSTEM_RUNMODE;
				}
			}
			break;
		case SYSTEM_SLEEPMODE:
			if(sys.chargeFlag)
			{
				if(sys.keyShorPress)
				{
					sys.keyShorPress = false;
					sys.status = SYSTEM_RUNMODE;
				}
			}
			else
				sys.status = SYSTEM_SHUTDOWN;
			break;
		case SYSTEM_RUNMODE:
			if(sys.keyShorPress)		// key is pressed
			{
				sys.keyShorPress = false;
				if(sys.chargeFlag)		// in charging
					sys.status = SYSTEM_SLEEPMODE;
				else
					sys.status = SYSTEM_SHUTDOWN;
			}
			break;
		default:
			sys.status = SYSTEM_SHUTDOWN;
			break;
	}
}

/**
**@name	void appPowerMeasure(void)
**@func	this function is use to start adc periodly which is called by timer 1
**/
void appPowerMeasure(void)
{
	static uint32_t timeScale = 0;					// 
	if(++timeScale > ADC_START_PERIOD)
	{
		timeScale = 0;								// Clear cnt
		ADC->CR1 |= (1u << 20);						// Start an ADC
		if(sys.adcWatchDogLow)						
		{
			sys.adcWatchDogLow = false;
			if(sys.lowPowerCnt < POWER_STATE_CONFIRM_TIME+1)
				sys.lowPowerCnt ++;
			if(sys.fullPowerCnt)
				sys.fullPowerCnt --;
		}
		else if(sys.adcWatchDogHigh)
		{
			sys.adcWatchDogHigh = false;
			if(sys.fullPowerCnt < POWER_STATE_CONFIRM_TIME+1)
				sys.fullPowerCnt ++;
			if(sys.lowPowerCnt)
				sys.lowPowerCnt --;
		}
		else
		{
			if(sys.lowPowerCnt)
				sys.lowPowerCnt --;
			if(sys.fullPowerCnt)
				sys.fullPowerCnt --;
		}
		
		// confirm power state
		if(sys.lowPowerFlag && !sys.lowPowerCnt)
			sys.lowPowerFlag = false;
		else if(sys.fullPowerFlag && !sys.fullPowerCnt)
			sys.fullPowerFlag = false;
		else if(!sys.lowPowerFlag && (sys.lowPowerCnt > POWER_STATE_CONFIRM_TIME))
			sys.lowPowerFlag = true;
		else if(!sys.fullPowerFlag && (sys.fullPowerCnt > POWER_STATE_CONFIRM_TIME))
			sys.fullPowerFlag = true;
	}
}

/**
**@name
**/
void CalledBeforeShutDown(void)
{
	PowerLed_Select(PWR_LED_UNKNOWN, PWR_LED_OFF);
	ModeLed_Select(MODE_LED_UNKNOWN, MODE_LED_OFF);
}

/**
**@name	void appMonitorSleepOrNot(void)
**/
void appMonitorSleepOrNot(void)
{
	static uint8_t status = 0xFF;
	if(status != sys.status)
	{
		status = sys.status;
		switch(status)
		{
			case SYSTEM_SHUTDOWN:
				SystemPowerOff();
				break;
			case SYSTEM_SLEEPMODE:
				break;
			case SYSTEM_RUNMODE:
				SystemPowerOn();
				break;
		}
	}
}
	
/**
**@name	void appGotoSleep(void)
**/
void appGotoSleep(void)
{
//	WriteWordToFlash((uint32_t *)(&sys), (uint32_t *)STORE_ADDR, (sizeof(sys) / 4));		// store data
	
	TIM1->CR1 &= ~(1u << 0);
	TIM15->CR1 &= ~(1u << 0);
	
	System_Enable_Disable_RTC_Domain_Access(FUNC_ENABLE);
	/*时钟切换到RC*/
	SCU->CCR1 =SYS_CLK_SRC_RCH;
    /* 关闭XTH、XTL、RC4M、32K 时钟  */
    SCU->RCHCR&= ~(1 << 16);    // RC4M Disable
    SCU->XTHCR &= ~(1 <<  0);    // XTH  Disable

    SCU->STOPCFG   |=  (1 << 0);    // RTC域写使能
    PMU->ANACR &= ~(1 << 0);    // XTL   Disable
    PMU->ANACR &= ~(1 << 8);    // RC32K Disable

    /* RTCCLK Disable */
    PMU->CR1 &= ~(1 << 5);

    /* 进入Standby模式 */
    SCU->STOPCFG |= (1 << 11);//PDDS  DeepSleep 下进入 STANDBY 模式，主区断电
    /* 清标志，允许进入Standby */
    PMU->CR1  |= (4 << 9) | (1 << 8) | (1 << 0);
	
	/*	PC13 wake up configuration	*/
	PMU->CR2 |= (1u << 1);				// falling edge to wake up
	PMU->CR1 |= (1u << 17);				// Enable wake up port 
	PMU->CR1 |= (1u << 1);				// Clear wake up flag
	PMU->CR1 &= ~(1u << 25);			// Wake up 2 filter Disable
	
	/*	PB5 wake up configuration	*/
	PMU->CR2 &= ~(1u << 4);				// rising edge to wake up
	PMU->CR1 |= (1u << 20);				// Enable wake up port 
	PMU->CR1 |= (1u << 1);				// Clear wake up flag
	PMU->CR1 &= ~(1u << 28);			// Wake up 2 filter Disable
	
	System_Enter_Standby_Mode();
	System_Delay_MS(20);
}








