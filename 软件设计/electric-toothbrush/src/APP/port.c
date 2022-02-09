#include	"includes.h"

Port_HandleTypeDef	MODE_LED[MODE_LED_UNKNOWN] = {
	{GPIOB, {GPIO_PIN_8, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_FUNCTION_0}},
	{GPIOD, {GPIO_PIN_3, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_FUNCTION_0}},
	{GPIOB, {GPIO_PIN_7, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_FUNCTION_0}}
};

Port_HandleTypeDef	PWR_LED[PWR_LED_UNKNOWN] = {
	{GPIOA, {GPIO_PIN_0, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_FUNCTION_0}},		// RED
	{GPIOB, {GPIO_PIN_4, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_FUNCTION_0}},		// BLUE
	{GPIOB, {GPIO_PIN_3, GPIO_MODE_AF_PP, GPIO_PULLUP, GPIO_FUNCTION_2}}			// GREEN, TIM1_CH2 PWM control
};

/**
**@name	void LED_Init(void)
**@func	Initialize the led port.
**/
void LED_Init(void)
{
	uint8_t i = 0;
	for(i=0;i<3;i++)
	{
		HAL_GPIO_Init(MODE_LED[i].Instance, &MODE_LED[i].Init);
		HAL_GPIO_Init(PWR_LED[i].Instance, &PWR_LED[i].Init);
		
		HAL_GPIO_WritePin(MODE_LED[i].Instance, MODE_LED[i].Init.Pin, MODE_LED_OFF);
		HAL_GPIO_WritePin(PWR_LED[i].Instance, PWR_LED[i].Init.Pin, PWR_LED_OFF);
	}
}

/**
**@name		void ModeLed_Select(uint8_t led, enum_PinState_t state)
**@input
	led:	Specifies which MODE_LED should be set accoding @state. If led is not equal to MODE_LED_x(x=1,2,3),
			it meas all of MODE_LED should be set as @state.
	state:	The specific MODE_LED should be set, and other MODE_LED shoule be set as an oppisite value. This
			parameter reference @MODE_LED_ON and @MODE_LED_OFF
**@return	None
**@brief
**/
void ModeLed_Select(uint8_t led, enum_PinState_t state)
{
	enum_PinState_t state_opp;
	uint8_t i=0;
	
	state_opp = (enum_PinState_t)(!state);
	if(led < MODE_LED_UNKNOWN)
	{
		for(i=0;i<MODE_LED_UNKNOWN;i++)
		{
			if(i == led)
				HAL_GPIO_WritePin(MODE_LED[i].Instance, MODE_LED[i].Init.Pin, state);
			else
				HAL_GPIO_WritePin(MODE_LED[i].Instance, MODE_LED[i].Init.Pin, state_opp);
		}
	}
	else {
		HAL_GPIO_WritePin(MODE_LED[MODE_LED_1].Instance, MODE_LED[MODE_LED_1].Init.Pin, state);
		HAL_GPIO_WritePin(MODE_LED[MODE_LED_2].Instance, MODE_LED[MODE_LED_2].Init.Pin, state);
		HAL_GPIO_WritePin(MODE_LED[MODE_LED_3].Instance, MODE_LED[MODE_LED_3].Init.Pin, state);
	}
}

/**
**@name		void PowerLed_Select(uint8_t led, enum_PinState_t state)
**@input
	led:	Specifies which PWR_LED should be set accoding @state. If led is not equal to PWR_LED_x(x=1,2,3),
			it meas all of PWR_LED should be set as @state.
	state:	The specific PWR_LED should be set, and other MODE_LED shoule be set as an oppisite value. This
			parameter reference @PWR_LED_ON and @PWR_LED_OFF
**@return	None
**@brief	It can just control LEDG and LEDR. LEDG is controlled by PWM through TIM3_CH1
**/
void PowerLed_Select(uint8_t led, enum_PinState_t state)
{
	enum_PinState_t state_opp;
	uint8_t i=0;
	
	state_opp = (enum_PinState_t)(!state);
	if(led < PWR_LED_G)
	{
		for(i=0;i<PWR_LED_G;i++)
		{
			if(i == led)
				HAL_GPIO_WritePin(PWR_LED[i].Instance, PWR_LED[i].Init.Pin, state);
			else
				HAL_GPIO_WritePin(PWR_LED[i].Instance, PWR_LED[i].Init.Pin, state_opp);
		}
	}
	else {
		HAL_GPIO_WritePin(PWR_LED[PWR_LED_R].Instance, PWR_LED[PWR_LED_R].Init.Pin, state);
		HAL_GPIO_WritePin(PWR_LED[PWR_LED_B].Instance, PWR_LED[PWR_LED_B].Init.Pin, state);
	}
}

/**
**@name	void Key_Init(void)
**@func	Initialize the key port which is PC13
**/
void Key_Init(void)
{
	GPIO_InitTypeDef	GPIO_Handle;
	
//	System_Enable_Disable_RTC_Domain_Access(FUNC_ENABLE);
	
//	PMU->IOSEL	&= ~(3u);							// PC13 selected to be a normal gpio port
//	PMU->IOCR	&= ~(0x7Fu);
//	PMU->IOCR	|= (1u << 5) | (1u << 0);			// input enable & pull-up
	
	GPIO_Handle.Mode		= GPIO_MODE_INPUT;	// generate an interrupt on falling edge
	GPIO_Handle.Pin			= GPIO_PIN_15;
	GPIO_Handle.Pull		= GPIO_PULLUP;			
	GPIO_Handle.Alternate	= GPIO_FUNCTION_0;		// normal gpio port function
	
	// interrupt configure parameter
//	NVIC_ClearPendingIRQ(GPIOAB_IRQn);
//	NVIC_EnableIRQ(GPIOAB_IRQn);
//	NVIC_SetPriority(GPIOAB_IRQn, 1);
	
	// Init the port
	HAL_GPIO_Init(GPIOA, &GPIO_Handle);
}

uint8_t Key_GetPressValue(void)
{
	return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15);
}

/**
**@name	void Charge_Port_Init(void)
**@func	Initialize the Charge port which is PB5. PB5 is also a wake up port on wakeup5
**/
void Charge_Port_Init(void)
{
	GPIO_InitTypeDef	GPIO_Handle;
	
	GPIO_Handle.Mode		= GPIO_MODE_INPUT;		// 
	GPIO_Handle.Pin			= GPIO_PIN_5;
	GPIO_Handle.Pull		= GPIO_PULLUP;			
	GPIO_Handle.Alternate	= GPIO_FUNCTION_0;		// normal gpio port function
	
	// Init the port
	HAL_GPIO_Init(GPIOB, &GPIO_Handle);
}

/**
**@name	void PWR_LockPortInit(void)
**@func	Initialize the power locked port, which will set '1' to make system keep power on state.
**/
void PWR_LockPortInit(void)
{
	GPIO_InitTypeDef	GPIO_Handle;
	
	GPIO_Handle.Mode		= GPIO_MODE_OUTPUT_PP;		// 
	GPIO_Handle.Pin			= GPIO_PIN_6;
	GPIO_Handle.Pull		= GPIO_PULLUP;			
	GPIO_Handle.Alternate	= GPIO_FUNCTION_0;		// normal gpio port function
	
	// Init the port
	HAL_GPIO_Init(GPIOB, &GPIO_Handle);
	
	SystemPowerOn();		// set Power Off
}

/**
**@name	void SystemPowerOff(void)
**@func	Clear PB6 to shut down the system.
**/
void SystemPowerOff(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_CLEAR);
}

/**
**@name	void SystemPowerOff(void)
**@func	Clear PB6 to shut down the system.
**/
void SystemPowerOn(void)
{
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
}

/**
**@name	void shortPressHandler_PC13(void)
**@func	when press key in a short time, gear of motor will switch to next
**/
void shortPressKeyHandler(void)
{
	DEBUG_KEY("short press handler\r\n");
	sys.keyShorPress = true;
}

/**
**@name	void longPressHandler_PC13(void)
**@func when press key in a long time, device will enter to standby mode
**/
void longPressKeyHandler(void)
{	
	DEBUG_KEY("long press handler\r\n");
	if(sys.status == SYSTEM_RUNMODE)
	{
		if(++sys.motorStatus > 2)
		{
			sys.motorStatus = 0;
			WriteWordToFlash((uint32_t *)(&sys), (uint32_t *)STORE_ADDR, (sizeof(sys)));
		}
	}
}

Key_HandleDef	key = {false, 0, 0, shortPressKeyHandler, longPressKeyHandler};

/**
**@name	void keyPressHandler(void)
**@func when monitor a key pressed, check the press state and chose a handler
**@note	this function should be called per millisecond
**/
void keyPressHandler(void)
{
	key.isPressed = Key_GetPressValue();
	switch(key.pressState)
	{
		case 0:
			if(key.isPressed)
			{
				key.pressTime = 0;
				key.pressState = 1;
			}
			break;
		case 1:		/*	eliminate jitter	*/
			if(key.isPressed)
			{
				if(++key.pressTime > 10)
					key.pressState = 2;
			}
			else
				key.pressState = 0;
			break;
		case 2:		/*	whether long press is existed	*/
			if(key.isPressed)
			{
				if(++key.pressTime > LONG_PRESS_TIME)
					key.pressState = 3;
			}
			else
			{
				if(key.shortPressHandler != NULL)
					key.shortPressHandler();
				else
					DEBUG_KEY("have no short press handler!!\r\n");
				key.pressState = 0;
			}
			break;
		case 3:
			if(key.longPressHandler != NULL)
				key.longPressHandler();
			else
				DEBUG_KEY("have no long press handler!!\r\n");
			key.pressState = 4;
			break;
		case 4:		/*	wait for releasing key	*/
			if(key.isPressed == 0)
				key.pressState = 0;
			break;
	}
}

extern uint8_t PowerOn_Flag;

/**
**@name	void GPIOCD_IRQHandler(void)
**@func this function use to handle key press
**/
void GPIOAB_IRQHandler(void)
{
	HAL_GPIO_IRQHandler(GPIOA, GPIO_PIN_15);
	if(PowerOn_Flag)
	{
		if(Key_GetPressValue() == KEY_PRESS)
		{
			key.isPressed = true;
		}
	}
	else
		PowerOn_Flag = 1;	
}

/**
**@name	void Motor_Init(void)
**@func Initialize the motor port PB13 & PB14 which controlled by PWM
**@note	PB13 is CH1N of timer 15 and PB14 is CH1 of timer 15
**/
void Motor_Init(void)
{
	GPIO_InitTypeDef	GPIO_Handle;
	
	GPIO_Handle.Mode		= GPIO_MODE_AF_PP;
	GPIO_Handle.Pin			= GPIO_PIN_1;
	GPIO_Handle.Pull		= GPIO_PULLUP;			
	GPIO_Handle.Alternate	= GPIO_FUNCTION_5;		// normal gpio port function
	
	HAL_GPIO_Init(GPIOA, &GPIO_Handle);
	
	GPIO_Handle.Pin			= GPIO_PIN_2;
	HAL_GPIO_Init(GPIOA, &GPIO_Handle);
}


/**
**@name		bool IsCharging(void)
**/
bool IsCharging(void)
{
	return (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_5) ? false : true);
}


/**
**@name		void ChargeMonitor(void)
**@input	None
**@return
**@brief	This is to ensure the device is in charging or not. And it will be called by timer per millisecond.
**/
void ChargeMonitor(void)
{
	if(IsCharging())
	{
		sys.chargeFlag = NO_CHARGE_CHECK_CNT;
	}
	else
	{
		if(sys.chargeFlag)
			sys.chargeFlag--;
	}
}







