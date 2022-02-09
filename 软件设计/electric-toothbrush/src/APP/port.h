#ifndef	__PORT_H__
#define	__PORT_H__

#include	"ACM32F0x0.h"
#include	"HAL_GPIO.h"

enum{
	PWR_LED_R = 0,
	PWR_LED_B,
	PWR_LED_G,
	PWR_LED_UNKNOWN
};

enum{
	MODE_LED_1 = 0,
	MODE_LED_2,
	MODE_LED_3,
	MODE_LED_UNKNOWN
};

#define		MODE_LED_ON				GPIO_PIN_SET
#define		MODE_LED_OFF			GPIO_PIN_CLEAR

#define		PWR_LED_ON				GPIO_PIN_CLEAR
#define		PWR_LED_OFF				GPIO_PIN_SET

#define		KEY_PRESS				1

typedef struct{
	bool isPressed;
	uint8_t pressState;
	uint16_t pressTime;
	void (*shortPressHandler)(void);
	void (*longPressHandler)(void);
}Key_HandleDef;

typedef struct{
	enum_GPIOx_t		Instance;
	GPIO_InitTypeDef	Init;
}Port_HandleTypeDef;

extern Port_HandleTypeDef	MODE_LED[3];
extern Port_HandleTypeDef	PWR_LED[3];

void LED_Init(void);
void ModeLed_Select(uint8_t led, enum_PinState_t state);
void PowerLed_Select(uint8_t led, enum_PinState_t state);

void Key_Init(void);
uint8_t Key_GetPressValue(void);
void keyPressHandler(void);

void PWR_LockPortInit(void);
void SystemPowerOff(void);
void SystemPowerOn(void);

void Charge_Port_Init(void);
bool IsCharging(void);
void ChargeMonitor(void);

void Motor_Init(void);

#endif



