#include "includes.h"






void Motor_ModeSelector(void)
{
	static uint8_t status = 0xFF;
	if(status != sys.status)			// the status of system is updated
	{
		status = sys.status;
		if(status != SYSTEM_RUNMODE)
			PWM_DeInit();
		else
			PWM_Init();
			
	}
	if(status == SYSTEM_RUNMODE)		// system in run mode
	{
		switch(sys.motorStatus)
		{
			case 0:
				PWM_freqSet(PWM_MOTOR, MOTOR_SPEED_1);
				break;
			case 1:
				PWM_freqSet(PWM_MOTOR, MOTOR_SPEED_2);
				break;
			case 2:
				PWM_freqSet(PWM_MOTOR, MOTOR_SPEED_3);
				break;
			default:
				sys.motorStatus = 0;
				break;
		}
	}
}




