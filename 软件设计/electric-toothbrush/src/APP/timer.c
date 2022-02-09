#include "includes.h"


/**
**@name void Tiemr_Init(TIM_TypeDef* TIMx, uint16_t arr, uint16_t pcs)
**@brief configure a timer which can be TIM1, TIM3, TIM6, TIM14, TIM15, TIM16, TIM17
**@TIMx the timer should be configured
**@arr	auto reload value
**@pcs	prescaler value
**@note time = System_CLK / (pcs + 1) / (arr + 1)
**/
void Timer_Init(TIM_TypeDef* TIMx, uint16_t arr, uint16_t pcs)
{
	TIM_HandleTypeDef TIM_Handler;
	
	TIM_Handler.Instance = TIMx;
	TIM_Handler.Init.ARRPreLoadEn = TIM_ARR_PRELOAD_ENABLE;
	TIM_Handler.Init.CounterMode = TIM_COUNTERMODE_UP;			// up counter
	TIM_Handler.Init.Prescaler = pcs;
	TIM_Handler.Init.Period = arr;
	TIM_Handler.Init.RepetitionCounter = 0;
	TIM_Handler.Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
	
	HAL_TIMER_MSP_Init(&TIM_Handler);
	HAL_TIMER_Base_Init(&TIM_Handler);
	
	TIMx->SR = 0;
	TIMx->DIER |= 0x01;
	TIMx->EGR |= 0x01;
	
	HAL_TIMER_Base_Start(TIMx);
}

/**
**@name		
**/
void pwrLED_PWM_Init(void)
{
//	Timer_Init(TIM3, (PWM_DUTY_MAX - 1), 63);		// 0.1 ms
	
	TIM1->CCMR1 &= ~(0xFFul << 8);
	TIM1->CCMR1 |= ((6u << 12) | (1u << 11) | (1u << 10));
	
	TIM1->CCER &= ~(0x03 << 4);
	TIM1->CCER |= ((PWR_LED_OFF << 5) | (1 << 4));
	
	TIM1->BDTR |= (1u << 15);			// MOE Enable
	
	TIM1->CCR2 = 0;	
}

/**
**@name	void PWM_Init(void)
**@func	Init PWM function of timer 15, use CH2 and CH1N which control motor and led respectively
**/
void PWM_Init(void)
{	
	// TIM15 CH1N and TIM15 CH1 Init which use to control motor
	// PWM mode 1, CC preload enable, Output Compare 1 fast enable
	TIM15->CCMR1 &= ~(0x7Fu);
	TIM15->CCMR1 |= ((6u << 4) | (1u << 3) | (1u << 2));
	
	// CC1N valid level is low, and output enable
	TIM15->CCER &= ~(0x0Fu);
	TIM15->CCER |= (0xFu << 0);
	
	TIM15->BDTR |= (1u << 15);			// MOE Enable
	TIM15->BDTR &= ~(0xFFul);
	TIM15->BDTR |= (0xFFul);
	
	TIM15->CCR1 = 0;
}

/**
**@name
**@func
**/
void PWM_DeInit(void)
{
	TIM15->CCER &= ~(0x0Fu);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
}

/**
**@name	void PWM_dutySet(uint8_t PWMx, uint16_t duty)
**@func Set PWM duty
**@PWMx	chose which PWM controller should be set, 0 - LED, 1 - Motor
**@duty PWM duty value, max value equal with PWM_MAX_VALUE
**/
void PWM_dutySet(uint8_t PWMx, uint16_t duty)
{
	if(IS_PWM_INSTANCE(PWMx) == 0)	return;
	if(duty > PWM_DUTY_MAX)	duty = PWM_DUTY_MAX;
	if(PWMx == PWM_LED)
		TIM1->CCR2 = duty;
//	else
//		TIM15->CCR2 = duty;	
}

/**
**@name	void PWM_freqSet(uint8_t PWMx, uint16_t freq)
**@func Set PWM frequency
**@PWMx	chose which PWM controller should be set, 0 - LED, 1 - Motor
**@duty PWM freq value, which range from PWM_FREQ_MAX to PWM_FREQ_MIN
**/
void PWM_freqSet(uint8_t PWMx, uint16_t freq)
{
	uint32_t arr;
	if(IS_PWM_INSTANCE(PWMx) == 0)	return;
	if(freq == 0)
	{
		TIM15->ARR = 0;
		return;
	}
	if(freq > PWM_FREQ_MAX)	freq = PWM_FREQ_MAX;
	if(freq < PWM_FREQ_MIN) freq = PWM_FREQ_MIN;
	arr = (PWM_TIMER_FRE / freq);
	if(PWMx == PWM_MOTOR)
	{
		TIM15->ARR = arr-1;
		TIM15->CCR1 = arr / 2;
	}
//	else
//		TIM15->CCR2 = duty;	
}

/**
**@name
**/
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
	if(TIM1->SR & 1u)			// timer update events
	{
		TIM1->SR &= ~(1u);
		
		/*	System LED controller. */
		appSysLedController();
		
		/*	Key handler.	*/
		keyPressHandler();
		
		/*	get power data, full or low. */
		appPowerMeasure();
		
		/*	Charge Check	*/
		ChargeMonitor();
		
		/* In running mode, BLE will send brushing data to upper. */
		if(sys.status == SYSTEM_RUNMODE)
			appBRecordBrushingData();
	}
}

/**
**@name
**/
void TIM15_IRQHandler(void)
{
	if(TIM15->SR & 0x01)
	{
		TIM15->SR &= ~(0x01u);
	}
}

///**
//**@name
//**/
//void TIM3_IRQHandler(void)
//{
//	if(TIM3->SR & 0x01)
//	{
//		TIM3->SR &= ~(0x01ul);
//		appLedController();

//	}
//}



