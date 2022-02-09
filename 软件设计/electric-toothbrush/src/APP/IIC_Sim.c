/**
		This file is a IIC interface simulator which is use timer to implete.
	SCL port is PA4 and SDA port is PA3. Use EXTI4 to be an soft interrupt to
	handle data. This IIC simulator can support standard IIC simunication, which
	Bit rate is 100kbps.
**/

#include "includes.h"

#define		SDA_OUT		 GPIOAB->DIR |= (1ul << 3)
#define		SDA_IN		 GPIOAB->DIR &= ~(1ul << 3)

Port_HandleTypeDef	SCL = {GPIOA, GPIO_PIN_4, GPIO_MODE_OUTPUT_PP, GPIO_PULLUP, GPIO_FUNCTION_0};
Port_HandleTypeDef	SDA = {GPIOA, GPIO_PIN_3, GPIO_MODE_INPUT, GPIO_PULLUP, GPIO_FUNCTION_0};		// SDA is input in default

#define		SCL_HIGH	HAL_GPIO_WritePin(SCL.Instance, SCL.Init.Pin, GPIO_PIN_SET)
#define		SCL_LOW		HAL_GPIO_WritePin(SCL.Instance, SCL.Init.Pin, GPIO_PIN_CLEAR)

#define		SDA_HIGH	HAL_GPIO_WritePin(SDA.Instance, SDA.Init.Pin, GPIO_PIN_SET)
#define		SDA_LOW		HAL_GPIO_WritePin(SDA.Instance, SDA.Init.Pin, GPIO_PIN_CLEAR)

#define		SDA_READ	HAL_GPIO_ReadPin(SDA.Instance, SDA.Init.Pin)

#ifdef 	IIC_SIM_TIMER

uint8_t scl_state = 1;

IIC_SIM_TypeDef		IIC_Sim;
EXTI_HandleTypeDef	EXTI4_Handle;

/**
**@name
**/
HAL_StatusTypeDef EXTI_SoftSetINT(EXTI_HandleTypeDef	*hexti)
{
	if (!IS_EXTI_ALL_LINE(hexti->u32_Line))      return HAL_ERROR;
	EXTI->SWIER |= (0x01UL << hexti->u32_Line);
	
	return HAL_OK;
}

/**
**@name
**/
HAL_StatusTypeDef EXTI_InitWithoutEdgeTrig(EXTI_HandleTypeDef	*hexti)
{
	if (!IS_EXTI_ALL_LINE(hexti->u32_Line))      return HAL_ERROR;
	
	EXTI->IENR |= (0x01UL << hexti->u32_Line);
	EXTI->EENR &= ~(0x01UL << hexti->u32_Line);
	
	NVIC_ClearPendingIRQ(EXTI_IRQn);
	NVIC_SetPriority(EXTI_IRQn, 2);
	NVIC_EnableIRQ(EXTI_IRQn);
	
	return HAL_OK;
}

/**
**@name		void IIC_Sim_MspInit(void)
**@input	None
**@return	None
**@brief	Init the IIC port
**/
__weak void IIC_Sim_MspInit(void)
{
	HAL_GPIO_Init(SCL.Instance, &SCL.Init);
	HAL_GPIO_WritePin(SCL.Instance, SCL.Init.Pin, GPIO_PIN_SET);
	HAL_GPIO_Init(SDA.Instance, &SDA.Init);
	HAL_GPIO_WritePin(SDA.Instance, SDA.Init.Pin, GPIO_PIN_SET);
}

/**
**@name		HAL_StatusTypeDef IIC_Sim_Init(uint32_t freq)
**@input	freq: the bit rate of IIC
**@return	HAL_OK / HAL_ERROR
**/
HAL_StatusTypeDef IIC_Sim_Init(uint32_t freq)
{
	uint32_t l32u_SysClk;
	
	if(freq > IIC_SIM_BPS_MAX) return	HAL_ERROR;
	
	IIC_Sim.TIMx		= TIM3;
	
	l32u_SysClk = System_Get_SystemClock();
	IIC_Sim.period		= l32u_SysClk / freq / 2;
	
	IIC_Sim.rx_done = IIC_Sim.tx_done = 0;
	
	EXTI4_Handle.u32_Line = EXTI_LINE_4;
	
	IIC_Sim_MspInit();
	
	Timer_Init(IIC_Sim.TIMx, (IIC_Sim.period - 1), 0);
	EXTI_InitWithoutEdgeTrig(&EXTI4_Handle);
	
	return HAL_OK;
}

/**
**@name
**/
void IIC_SIM_WriteOperation(uint8_t addr, uint8_t *tx, uint32_t len)
{
	IIC_Sim.addr = addr;
	IIC_Sim.tx_buf = tx;
	IIC_Sim.tx_len = len;
}

/**
**@name
**/
void IIC_SIM_ReadOperation(uint8_t addr, uint8_t *rx, uint32_t len)
{
	IIC_Sim.addr = addr;
	IIC_Sim.rx_buf = rx;
	IIC_Sim.rx_len = len;
}

/**
**@name
**/
static uint8_t IIC_SIM_StartGenerator(void)
{
	SDA_OUT;
	HAL_GPIO_WritePin(SDA.Instance, SDA.Init.Pin, GPIO_PIN_CLEAR);
	IIC_Sim.status = IIC_SIM_ADDR;			// send addr after start signal generated
	return HAL_OK;
}

static uint8_t IIC_SIM_StopGenerator(void)
{
	SDA_OUT;
	HAL_GPIO_WritePin(SDA.Instance, SDA.Init.Pin, GPIO_PIN_SET);
	IIC_Sim.status = IIC_SIM_IDLE;			// transfer to idle after stop signal generated
	return HAL_OK;
}

static uint8_t IIC_SIM_AckGenerator(void)
{
	SDA_OUT;
	HAL_GPIO_WritePin(SDA.Instance, SDA.Init.Pin, GPIO_PIN_CLEAR);
	if(IIC_Sim.rx_len == 0)					// check rx operation is over or not
	{
		IIC_Sim.status = IIC_SIM_STOP;
		IIC_Sim.rx_done = 1;				
		EXTI_SoftSetINT(&EXTI4_Handle);
	}
	return HAL_OK;
}

static uint8_t IIC_Sim_AckCheck(void)
{
	SDA_IN;
	if(HAL_GPIO_ReadPin(SDA.Instance, SDA.Init.Pin) == GPIO_PIN_CLEAR)
	{
		if(IIC_Sim.rx_len)
			IIC_Sim.status = IIC_SIM_READ;
		else if(IIC_Sim.tx_len)
			IIC_Sim.status = IIC_SIM_WRITE;
		else{
			IIC_Sim.status = IIC_SIM_STOP;
			IIC_Sim.tx_done = 1;
			EXTI_SoftSetINT(&EXTI4_Handle);
		}
		return HAL_OK;
	}
	else
		return HAL_ERROR;
}

/**
**@name
**/
static void IIC_SIM_WriteAddr(void)
{
	static uint8_t pos = 0;
	static uint8_t dat = 0;
	
	SDA_OUT;			// SDA set output mode
	
	if(pos == 0)
		dat = IIC_Sim.addr;
	
	HAL_GPIO_WritePin(SDA.Instance, SDA.Init.Pin, ((dat & 0x80) ? GPIO_PIN_SET : GPIO_PIN_CLEAR));
	if(++pos == 8)
	{
		IIC_Sim.status = IIC_SIM_ACK_R;
		pos = 0;
	}
	
	dat <<= 1;
	
}

/**
**@name
**/
static void IIC_SIM_WriteDat(void)
{
	static uint8_t pos = 0;
	static uint8_t dat = 0;
	static uint8_t tx_pointer = 0;
	SDA_OUT;		// SDA set output mode
	
	if(pos == 0)
		dat = IIC_Sim.tx_buf[tx_pointer];
	
	HAL_GPIO_WritePin(SDA.Instance, SDA.Init.Pin, ((dat & 0x80) ? GPIO_PIN_SET : GPIO_PIN_CLEAR));
	
	if(++pos == 8)
	{
		IIC_Sim.tx_len = 0;
		IIC_Sim.status = IIC_SIM_ACK_R;
		pos = 0;
	}
	
	dat <<= 1;
}

/**
**@name
**/
static void IIC_SIM_ReadDat(void)
{
	static uint8_t pos = 0;
	static uint8_t dat = 0;
	static uint8_t rx_pointer = 0;
	
	SDA_IN;
	
	if(HAL_GPIO_ReadPin(SDA.Instance, SDA.Init.Pin) == GPIO_PIN_SET)
		dat |= 1;
	
	if(++pos == 8)
	{
		pos = 0;
		IIC_Sim.rx_buf[rx_pointer] = dat;
		IIC_Sim.rx_len--;
		IIC_Sim.status = IIC_SIM_ACK_S;
		dat = 0;
	}	
}


/**
**@name
**@input
**@return
**/
void IIC_Sim_Handler(void)
{
	switch(IIC_Sim.status)
	{
		case IIC_SIM_IDLE:
			if(IIC_Sim.rx_len || IIC_Sim.tx_len)
			{
				IIC_Sim.status = IIC_SIM_START;
			}
			break;
		case IIC_SIM_START:
			if(scl_state)
				IIC_SIM_StartGenerator();
			break;
		case IIC_SIM_ADDR:
			if(scl_state == 0)
				IIC_SIM_WriteAddr();
			break;
		case IIC_SIM_READ:
			if(scl_state == 0)
				IIC_SIM_ReadDat();
			break;
		case IIC_SIM_WRITE:
			if(scl_state == 0)
				IIC_SIM_WriteDat();
			break;
		case IIC_SIM_ACK_R:
			if(IIC_Sim_AckCheck() == HAL_ERROR)
			{
				IIC_Sim.status = IIC_SIM_IDLE;
				IIC_Sim.rx_len = 0;
				IIC_Sim.tx_len = 0;
			}
			break;
		case IIC_SIM_ACK_S:
			if(scl_state == 0)
				IIC_SIM_AckGenerator();
			break;
		case IIC_SIM_STOP:
			if(scl_state)
				IIC_SIM_StopGenerator();
			break;
		default:	
			IIC_Sim.status = IIC_SIM_IDLE;
			break;
		
	}
}


/**
**@name
**/
void TIM3_IRQHandler(void)
{
	static uint8_t cnt = 0;
	if(TIM3->SR & 0x01)
	{
		TIM3->SR &= ~(0x01u);
		if(cnt)
		{
			cnt = 0;
			scl_state  ^= 0x01;
			HAL_GPIO_WritePin(SCL.Instance, SCL.Init.Pin, (scl_state ? GPIO_PIN_SET : GPIO_PIN_CLEAR));
		}
		else{			// in middle of SCL state change, we should check the SDA state or change it.
			IIC_Sim_Handler();
		}
	}
}

/**
**@name
**/
void EXTI_IRQHandler(void)
{
	if(EXTI->PDR & EXTI4_Handle.u32_Line)
	{
		EXTI->PDR |= EXTI4_Handle.u32_Line;		// Clear Interrupt
		EXTI->SWIER &= ~EXTI4_Handle.u32_Line;	// Clear Soft trigger
		if(IIC_Sim.rx_done)
		{
			IIC_Sim.rx_done = 0;
		}
		if(IIC_Sim.tx_done)
		{
			IIC_Sim.tx_done = 0;
		}
	}
}

#else

/**
**@name		HAL_StatusTypeDef IIC_Sim_Init(uint32_t freq)
**@input	freq: 
**@return	HAL_OK / HAL_ERROR
**/
HAL_StatusTypeDef IIC_Sim_Init(uint32_t freq)
{
	HAL_GPIO_Init(SDA.Instance, &SDA.Init);
	HAL_GPIO_WritePin(SDA.Instance, SDA.Init.Pin, GPIO_PIN_SET);
	
	HAL_GPIO_Init(SCL.Instance, &SCL.Init);
	HAL_GPIO_WritePin(SCL.Instance, SCL.Init.Pin, GPIO_PIN_CLEAR);
	
	return HAL_OK;
}

/**
**@name		void IIC_Sim_Start_Generator(void)
**@input	None
**@return	None
**@brief	Generate a start signal of IIC
**/
void IIC_Sim_Start_Generator(void)
{
	SDA_OUT;
	
	SDA_HIGH;
	System_Delay(IIC_SIM_CLK_FREQ / 2);
	SCL_HIGH;
	System_Delay(IIC_SIM_CLK_FREQ / 2);			// according with system clock
	SDA_LOW;
	System_Delay(IIC_SIM_CLK_FREQ / 2);
	SCL_LOW;
}

/**
**@name		void IIC_Sim_Stop_Generator(void)
**@input	None
**@return	None
**@brief	Generate a stop signal of IIC
**/
void IIC_Sim_Stop_Generator(void)
{
	SDA_OUT;
	
	SDA_LOW;
	System_Delay(IIC_SIM_CLK_FREQ / 2);
	SCL_HIGH;
	System_Delay(IIC_SIM_CLK_FREQ / 2);			// according with system clock
	SDA_HIGH;
	System_Delay(IIC_SIM_CLK_FREQ / 2);
	SCL_LOW;
}

/**
**@name		HAL_StatusTypeDef IIC_Sim_WriteAddr(uint8_t dat)
**@input	dat: use to send by IIC
**@return	HAL_OK/HAL_ERROR
**@brief	use to send data
**/
HAL_StatusTypeDef IIC_Sim_WriteAddr(uint8_t addr, uint8_t wr)
{
	uint8_t dat;
	dat = (addr << 1) + wr;
	
	return IIC_Sim_WriteDat(dat);
}


/**
**@name		HAL_StatusTypeDef IIC_Sim_WriteDat(uint8_t dat)
**@input	dat: use to send by IIC
**@return	HAL_OK/HAL_ERROR
**@brief	use to send data
**/
HAL_StatusTypeDef IIC_Sim_WriteDat(uint8_t dat)
{
	uint8_t i, state;
//	SCL_LOW;
//	System_Delay(IIC_SIM_CLK_FREQ/2);
	SDA_OUT;
	
	// send data
	for(i=0;i<8;i++)
	{
		if(dat & 0x80)	SDA_HIGH;
		else 	SDA_LOW;
		System_Delay(IIC_SIM_CLK_FREQ/2);
		SCL_HIGH;
		System_Delay(IIC_SIM_CLK_FREQ);
		dat <<= 1;
		SCL_LOW;
		System_Delay(IIC_SIM_CLK_FREQ);
	}
	
	// check ACK
	SDA_IN;
	System_Delay(IIC_SIM_CLK_FREQ / 2);
	SCL_HIGH;
	System_Delay(IIC_SIM_CLK_FREQ / 2);
	state = SDA_READ;
	SCL_LOW;
	System_Delay(IIC_SIM_CLK_FREQ / 2);
	
	if(state == GPIO_PIN_SET)
		return HAL_ERROR;
	else
		return HAL_OK;
}


/**
**@name		HAL_StatusTypeDef IIC_Sim_ReadDat(uint8_t dat)
**@input	ack: select ACK or NACK. 1: NACK; 0: ACK
**@return	received data
**@brief	use to receive data
**/
uint8_t IIC_Sim_ReadDat(uint8_t ack)
{
	uint8_t res = 0;
	uint8_t i;
//	SCL_LOW;
//	System_Delay(IIC_SIM_CLK_FREQ / 2);
	SDA_IN;
	
	// Receive data
	for(i=0;i<8;i++)
	{
		res <<= 1;
		SCL_HIGH;
		System_Delay(IIC_SIM_CLK_FREQ / 2);
		res |= (SDA_READ ? 1 : 0);
		System_Delay(IIC_SIM_CLK_FREQ / 2);
		SCL_LOW;
		System_Delay(IIC_SIM_CLK_FREQ / 2);
	}
	
	// Send ACK
	
	SDA_OUT;
//	System_Delay(IIC_SIM_CLK_FREQ / 2);
	ack ? SDA_HIGH : SDA_LOW;
	System_Delay(IIC_SIM_CLK_FREQ / 2);
	SCL_HIGH;
	System_Delay(IIC_SIM_CLK_FREQ / 2);
	SCL_LOW;
	System_Delay(IIC_SIM_CLK_FREQ / 2);
	return res;
}


#endif		/*	IIC_SIM_TIMER	*/



