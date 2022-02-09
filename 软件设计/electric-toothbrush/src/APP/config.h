#ifndef	__CONFIG_H__
#define	__CONFIG_H__


#define		DEBUG_MODE

#define		LONG_PRESS_TIME				2000		// 2s

#define		PWM_DUTY_MAX				1000
#define		PWM_FREQ_MAX				350
#define		PWM_FREQ_MIN				170
#define		PWM_TIMER_FRE				500000

#define		MOTOR_SPEED_1				300
#define		MOTOR_SPEED_2				325
#define		MOTOR_SPEED_3				350

#define		TWINKLE_INTERVAL			2000UL		// ms
#define		BREATHE_INTERVAL			1UL			// 0.1 ms
#define		BREATHE_HOLD_TIME			1000		// ACTUAL = BREATHE_HOLD_TIME * BREATHE_INTERVAL

#define		LOW_POWER_THS				1750U 		/* This value should be corresponded to 1.75V. When supply power of MUC is 3.3V, it
													   would be 2172 normally */		
#define		HIGH_POWER_THS				2100U 		/* This value should be corresponded to 2.1. When supply power of MUC is 3.3V, it
													   would be 2606 normally */
#define		POWER_STATE_CONFIRM_TIME	4			/* It means that the power state confirmed need the times which is used to check it more than POWER_STATE_CONFIRM_TIME*/
#define		NO_CHARGE_CHECK_CNT			50
#define		ADC_START_PERIOD			100			// ms

//#define		CRC_SUPPORT

#define		BLE_RECORD_POINT_NUM		2000		/* This value indicate the maximum number of brushing point will be recorded and transmit. */
	
#endif





