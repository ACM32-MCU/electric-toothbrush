#include "includes.h"



/**
**@name		void appMotorModeLedControl(void)
**@input	None
**@return	None
**@brief	The function is used to control motor mode indicator led which is on PB8, PD3, PB7. 
**/
void appMotorModeLedControl(void)
{
	static uint8_t state = 0xFF;
	
	if(sys.status == SYSTEM_RUNMODE)
	{
		if(state != sys.motorStatus)
		{
			state = sys.motorStatus;
			if(sys.motorStatus == 0)
			{
				ModeLed_Select(MODE_LED_1, MODE_LED_ON);
			}
			else if(sys.motorStatus == 1)
			{
				ModeLed_Select(MODE_LED_2, MODE_LED_ON);
			}
			else if(sys.motorStatus == 2)
			{
				ModeLed_Select(MODE_LED_3, MODE_LED_ON);
			}
		}
	}
	else
	{
		state = 0xFF;
		ModeLed_Select(MODE_LED_UNKNOWN, MODE_LED_OFF);
	}
}

/**
**@name	void appLedManagement(void)
**@func manage the led state
**@breif	In charge mode, when power is not full, led in breathe mode, otherwise, led in on mode
			Not in charge mode, led is off in sleep, on in normal, and twinkle in low power
**/
void appSysLedManagement(void)
{
	if(sys.status == SYSTEM_RUNMODE)
	{
		if(sys.chargeFlag)				// in charge mode
		{
			if(sys.fullPowerFlag)
				led.state = LED_ON;
			else
				led.state = LED_BREATHE;
		}
		else
		{
			if(sys.lowPowerFlag)
				led.state = LED_TWINKLE;
			else
				led.state = LED_ON;
		}
	}
	else{
		if(sys.chargeFlag)				// in charge mode
		{
			if(sys.fullPowerFlag)
				led.state = LED_ON;
			else
				led.state = LED_BREATHE;
		}
		else{
			led.state = LED_OFF;
		}
	}
	
}

/**
**@name		void appLedController(void)
**@input	None
**@return	None
**@brief	This function is used to control led port, which is called by timer per millisecond
**/
void appSysLedController(void)
{
	static uint8_t led_state = 0xFF;
	
	if(led_state != led.state)
	{
		led_state = led.state;
		if(led.state == LED_OFF)
		{
			led.duty = 0;
			PowerLed_Select(PWR_LED_UNKNOWN, PWR_LED_OFF);
			PWM_dutySet(PWM_LED, led.duty);
		}
		else if(led.state == LED_TWINKLE)		// low power warning
		{
			led.duty = 0;
			PowerLed_Select(PWR_LED_R, PWR_LED_ON);
			PWM_dutySet(PWM_LED, led.duty);
		}
		else if(led.state == LED_ON)
		{
			led.duty = 0;
			PowerLed_Select(PWR_LED_R, PWR_LED_OFF);
			PWM_dutySet(PWM_LED, led.duty);
		}
		else if(led.state == LED_BREATHE)
		{
			if(led.duty == PWM_DUTY_MAX)
				led.dir = LED_FADE;
			else
				led.dir = LED_BRIGHTER;
		}
		else
			led.state = LED_OFF;
	}
	else{
		if(led.state == LED_BREATHE)
		{
			PowerLed_Select(PWR_LED_UNKNOWN, PWR_LED_OFF);
			if(led.dir == LED_BRIGHTER)
			{
				if(led.duty < PWM_DUTY_MAX)
					led.duty += BREATHE_INTERVAL;
				else
				{
					if(++led.cnt > BREATHE_HOLD_TIME)
					{
						led.dir = LED_FADE;
						led.cnt = 0;
					}
				}
			}
			else
			{
				if(led.duty > BREATHE_INTERVAL)
					led.duty -= BREATHE_INTERVAL;
				else
				{
					led.duty = 0;
					if(++led.cnt > BREATHE_HOLD_TIME)
					{
						led.dir = LED_BRIGHTER;
						led.cnt = 0;
					}
				}
			}
			PWM_dutySet(PWM_LED, led.duty);
		}
	}
}







