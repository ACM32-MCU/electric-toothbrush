#ifndef	__TIMER_H__
#define	__TIMER_H__

#include	"ACM32F0x0.h"

#define		PWM_LED		0
#define		PWM_MOTOR	1

#define		IS_PWM_INSTANCE(x)		((x == PWM_LED) || (x == PWM_MOTOR))


void Timer_Init(TIM_TypeDef* TIMx, uint16_t arr, uint16_t pcs);

void pwrLED_PWM_Init(void);

void PWM_Init(void);
void PWM_DeInit(void);
void PWM_dutySet(uint8_t PWMx, uint16_t duty);
void PWM_freqSet(uint8_t PWMx, uint16_t freq);

#endif

