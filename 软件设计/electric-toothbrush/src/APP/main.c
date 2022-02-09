#include "includes.h"

uint8_t	PowerOn_Flag = 0;

#ifdef		DEBUG_MODE

void USART_Init(UART_TypeDef *UARTx, uint32_t baud)
{
	UART_HandleTypeDef	UART_Handle;
	
	UART_Handle.Instance		= UARTx;
	UART_Handle.Init.BaudRate	= baud;
	UART_Handle.Init.HwFlowCtl	= UART_HWCONTROL_NONE;
	UART_Handle.Init.Mode		= UART_MODE_TX_RX_DEBUG;
	UART_Handle.Init.Parity		= UART_PARITY_NONE;
	UART_Handle.Init.StopBits	= UART_STOPBITS_1;
	UART_Handle.Init.WordLength	= UART_WORDLENGTH_8B;
	
	HAL_UART_Init(&UART_Handle);
	
	UARTx->IE |= UART_IE_RXI;
	
	DEBUG_PRINT("MCU is running, HCLK=%dHz, PCLK=%dHz\r\n", System_Get_SystemClock(), System_Get_APBClock());  
}

void UART1_IRQHandler(void)
{
	if(UART1->RIS & UART_RIS_RXI)
	{
		while(!(UART2->FR & UART_FR_TXFE)) ;
		UART2->DR = (UART1->DR & 0xFF);
	}
}

#endif




int main(void)
{
	uint32_t cnt = 0;
	System_Init();
#ifdef	DEBUG_MODE
	USART_Init(UART1, 115200);
#endif
	PWR_LockPortInit();
	
	BLE_Init(115200);
	HAL_EFlash_Init(64000000);
	memManageInit();
	ADC_Init(ADC_CHANNEL_4);
	
	Charge_Port_Init();	
	LED_Init();
	Key_Init();
	
	appInit();
	appBInit();		// BLE App Init
	
	Timer_Init(TIM1, (PWM_DUTY_MAX - 1), 63);				// 1ms
	Timer_Init(TIM15, 1999 ,127);			// 0.1ms
	pwrLED_PWM_Init();
	Motor_Init();
	PWM_Init();
	
	IIC_Sim_Init(0);
	
	DEBUG_PRINT("motor mode: %d\r\n", sys.motorStatus);
	DEBUG_PRINT("key status: %d\r\n", Key_GetPressValue());
	DEBUG_PRINT("sys status: %d\r\n", sys.status);
	DEBUG_PRINT("====== System Initalize Completely ======\r\n");
	
	sys.times ++ ;
	
	while(1)
	{
		/*	System status manage	*/
		appSystemStatusManagement();
		
		/*	Motor Control	*/
		Motor_ModeSelector();
		
		/*	LED State Control	*/
		appSysLedManagement();
		
		/*	Motor Mode indicator LED control	*/
		appMotorModeLedControl();
		
		/*	Sleep Mode Enter	*/
		appMonitorSleepOrNot();
		
		/* BLE Rec data handler */
		BLE_receDataHandler();
		
		if(++cnt > 640000)
		{
			cnt = 0;
			QMI8658_Test();
//			EEPROM_24LC256_Test();
		}
	}
}




