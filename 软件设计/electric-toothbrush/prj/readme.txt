/****************************************************************/
*****************************************************************
				electric toothbrush Introduce
*****************************************************************
/****************************************************************/


/**
							Port Map
**/
	PB8		--	LED_MODE1	(Normal port)
	PD3		--	LED_MODE2	(Normal port)
	PB7		--	LED_MODE3	(Normal port)
	
	PA0		--	LED_PWD_R	(Normal port)
	PB3		--	LED_PWD_G	(Normal port)
	PB4		--	LED_PWD_B	(Normal port)
	
	PA15	--	KEY (Normal port, should wake up device)
	
	PA1		-- 	MOTOR_A (INA of MX612E, PWM output)
	PA2		--	MOTOR_B (INB of MX612E, PWM output)
	
	PB5		--	Charge Monitor port	(Normal port)
	
	PA7		--	Battery Monitor port (ADC)
	
	PA5		--	BLE Tx Pin (UART3)
	PB0		--	BLE Rx Pin (UART3)
	PB1		--	BLE Start Pin
	PB2		-- 	BLE CTRL Pin
	
	PB6		--	PWR_LOCK pin (Normal port)
/**
							Port Map
**/






