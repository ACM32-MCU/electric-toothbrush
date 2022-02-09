#include "includes.h"

#define		BLE_RECE_BUFFER_SIZE			512

UART_HandleTypeDef	BLE_UART_Handle;
struct{
	uint8_t buffer[BLE_RECE_BUFFER_SIZE];
	uint16_t pointerRead;
	uint16_t pointerWrite;
}BLE_bufferRec;

#define	BLE_USART		UART3

/**
**@name	void BLE_Init(uint32_t baud)
**@func	Initialize the port and uart connected with BLE
**@brief	In this project, we choose uart2 to connected with BLE, which Rx pin is PD5 and Tx pin is PD4
**/
void BLE_Init(uint32_t baud)
{
	GPIO_InitTypeDef	GPIO_Handle;
	
	/* Enable Clock */
    System_Module_Enable(EN_UART3);
	
	/**	Communication port of BLE Initialized	**/
	GPIO_Handle.Pin       = GPIO_PIN_5;
	GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
	GPIO_Handle.Pull      = GPIO_PULLUP;
	GPIO_Handle.Alternate = GPIO_FUNCTION_4;
	
	HAL_GPIO_Init(GPIOA, &GPIO_Handle);
	
	GPIO_Handle.Pin       = GPIO_PIN_0;
	GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
	GPIO_Handle.Pull      = GPIO_PULLUP;
	GPIO_Handle.Alternate = GPIO_FUNCTION_4;
	
	HAL_GPIO_Init(GPIOB, &GPIO_Handle);
	
	/**	Start Port of BLE Initialized	**/
	GPIO_Handle.Pin		  = GPIO_PIN_1;
	GPIO_Handle.Mode      = GPIO_MODE_INPUT;
	GPIO_Handle.Pull      = GPIO_PULLUP;
	GPIO_Handle.Alternate = GPIO_FUNCTION_0;
	
	HAL_GPIO_Init(GPIOB, &GPIO_Handle);
	
	/**	CTRL Port of BLE Initialized	**/
	GPIO_Handle.Pin		  = GPIO_PIN_2;
	GPIO_Handle.Mode      = GPIO_MODE_AF_PP;
	GPIO_Handle.Pull      = GPIO_PULLUP;
	GPIO_Handle.Alternate = GPIO_FUNCTION_0;
	
	HAL_GPIO_Init(GPIOB, &GPIO_Handle);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_CLEAR);		// Set BLE Exit Sleep Mode and Enter to AT Command Mode
	
	BLE_UART_Handle.Instance		= UART3;
	BLE_UART_Handle.Init.BaudRate	= baud;
	BLE_UART_Handle.Init.HwFlowCtl	= UART_HWCONTROL_NONE;
	BLE_UART_Handle.Init.Mode		= UART_MODE_TX_RX;
	BLE_UART_Handle.Init.Parity		= UART_PARITY_NONE;
	BLE_UART_Handle.Init.StopBits	= UART_STOPBITS_1;
	BLE_UART_Handle.Init.WordLength	= UART_WORDLENGTH_8B;
	BLE_UART_Handle.Init.FIFOEn		= DISABLE;
	
	/*	Configure the Interrupt Enable	*/
	UART3->IE |= ((1ul << 4) | (1ul << 6));							// Enable interrupt: RXI, RTI, OEI, BEI, PEI, FEI
			
	/* NVIC Config */
	NVIC_ClearPendingIRQ(UART3_IRQn);
	NVIC_SetPriority(UART3_IRQn, 5);
	NVIC_EnableIRQ(UART3_IRQn);
	
//	UART2->DR = 0;
	/*	Initialize the BLE UART and Start it	*/
	HAL_UART_Init(&BLE_UART_Handle);
	
	/*	Rec buffer initialize	*/
	BLE_bufferRec.pointerRead = BLE_bufferRec.pointerWrite = 0;

}

/**
**@name	BLE_StatusDef	BLE_isConnected(void)
**@func Confirm that the BLE is connected or not
**@return 
		BLE_CONNECTED(1) : BLE is connected
		BLE_UNCONNECTED(0)	: BLE is unconnected
**/
BLE_StatusDef	BLE_isConnected(void)
{
	return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_4) ? BLE_CONNECTED : BLE_UNCONNECTED;
}

/**
**@name	int8_t	BLE_atCommand_Send(char *str)
**@func	This function is used to send AT Command to BLE.
**@input	
		str: The pointer of command which is prepared to send to BLE
**@return -1: transmit failed, 0: transmit success
**@note	THE LETTERS OF COMMAND SHOULD BE CAPITAL.
**/
int8_t	BLE_atCommand_Send(char *str)
{
	char buffer[50];
	sprintf(buffer, "AT+%s\r\n", str);
	if(HAL_UART_Transmit(&BLE_UART_Handle, (uint8_t *)buffer, strlen(buffer), 100) == HAL_ERROR)
		return BLE_FAULT;
	else
		return BLE_SUCCESS;
}

/**
**@name	uint8_t BLE_GetRecByte(void)
**@func	Get BLE data from UART2
**@return rece data
**/
uint8_t BLE_GetRecByte(void)
{
	uint8_t ch;
	ch = BLE_bufferRec.buffer[BLE_bufferRec.pointerRead++];
	BLE_bufferRec.pointerRead &= (BLE_RECE_BUFFER_SIZE - 1);
	return ch;
}

/**
**@name	uint8_t BLE_GetRecBytes(void)
**@func	Get BLE data from UART2
**@input
		dst: the pointer to rece data
		len: rece data length to be rece
		timeout: indicate that how long we can wait for rece data
**@return result 0-Success, != 0 fault
**/
int8_t BLE_GetRecBytes(uint8_t *dst, uint32_t len, uint32_t timeout)
{
	uint32_t lo_timeout;
	
	lo_timeout = timeout;
	while(len)
	{
		if(BLE_bufferRec.pointerRead != BLE_bufferRec.pointerWrite)
		{
			*(dst++) = BLE_GetRecByte();
			len --;
			lo_timeout = timeout;
		}
		else{
			if(--lo_timeout == 0)	return BLE_TIMEOUT;
		}
	}
	return BLE_SUCCESS;
}

/**
**@name	int8_t BLE_GetRespond(uint8_t *dst, uint32_t timeout)
**@func	Get BLE Respond Data after send AT Command to it.
**@input
		dst: pointer to buffer which should store respond data, respond data start at second pointer
		timeout: set timeout of rece respond
**@return	0: Get respond success, -1: Get respond failed
**/
int8_t BLE_GetRespond(uint8_t *dst, uint32_t timeout)
{
	uint32_t lo_timeout;
	*(dst++) = '\0';				// Initialize the first data of buffer for check string "\r\n"
	*(dst++) = '\0';
	lo_timeout = timeout;
	while(((*(dst-2)) != '\r') || ((*(dst-1)) != '\n'))
	{
		if(BLE_bufferRec.pointerRead != BLE_bufferRec.pointerWrite)
		{
			*(dst++) = BLE_GetRecByte();			// get data to ch
			lo_timeout = timeout;
		}
		else
		{
			if(--lo_timeout == 0)	return BLE_TIMEOUT;		// rece data timeout
		}
	}
	*dst = '\0';
	return BLE_SUCCESS;
}

/**
**@name	void BLE_receDataHandler(void)
**@func	This function is use to handle with received data from BLE except AT Command Respond
**/
void BLE_receDataHandler(void)
{
	uint8_t ch;
	if(BLE_bufferRec.pointerRead != BLE_bufferRec.pointerWrite)
	{
		ch = BLE_GetRecByte();
		if(ch == STX)				// it indicate that command data be received
		{
			uint8_t *buf;
			uint8_t header_tmp[3];
			uint16_t len;
			ch = BLE_GetRecBytes(header_tmp, 3, 0xFFFFFFFF);
			len = header_tmp[1] + (((uint16_t)header_tmp[2]) << 8);
			buf = mymalloc(len+5);
			memcpy(buf, header_tmp, 3);
#ifdef	CRC_SUPPORT
			ch = BLE_GetRecBytes(&buf[3], len+2, 0xFFFFFFFF);			// data and CRC
#else
			ch = BLE_GetRecBytes(&buf[3], len, 0xFFFFFFFF);
#endif
			
			appBReceComLookUp(buf);
			myfree(buf);
		}
			
	}
}

/**
			BLE AT Command Hanlde Part
**/

/**
**@name	int8_t BLE_RESET(void)
**@func	reset BLE by AT Command
**@return 0: success, -1: fault
**/
int8_t BLE_Reset(void)
{
	uint8_t buf[50];
	
	/* Ensure that recevie buffer have no data */
	while(BLE_bufferRec.pointerRead != BLE_bufferRec.pointerWrite)
	{
		BLE_receDataHandler();
	}
	BLE_atCommand_Send("RESET");
	if(BLE_GetRespond(buf, 0xFFF) == -1)	return BLE_FAULT;
	if(strcmp((char *)buf, "+RESET=OK"))	return BLE_FAULT;
	if(BLE_GetRespond(buf, 0xFFF) == -1)	return BLE_FAULT;
	if(strcmp((char *)buf, "AT+OK"))
		return BLE_SUCCESS;
	else
		return BLE_FAULT;
}

/**	
**@name	int8_t BLE_Factory(void)
**@func	Factory BLE
**@return 0: success, -1: fault
**/
int8_t BLE_Factory(void)
{
	uint8_t buf[50];
	
	/* Ensure that recevie buffer have no data */
	while(BLE_bufferRec.pointerRead != BLE_bufferRec.pointerWrite)
	{
		BLE_receDataHandler();
	}
	BLE_atCommand_Send("DEFAULT");
	if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
	if(strcmp((char *)buf, "+DEFAULT=OK"))	return BLE_FAULT;
	if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
	if(strcmp((char *)buf, "AT+OK"))
		return BLE_SUCCESS;
	else
		return BLE_FAULT;
}

/**
**@name		int8_t BLE_GetVersion(uint8_t *version)
**@func		This function is used to get BLE version
**@input	dst: the pointer to register which is used to store version number
**@return 	0: success, -1 fault
**/
int8_t BLE_GetVersion(uint8_t *version)
{
	uint8_t buf[50];
	
	/* Ensure that recevie buffer have no data */
	while(BLE_bufferRec.pointerRead != BLE_bufferRec.pointerWrite)
	{
		BLE_receDataHandler();
	}
	BLE_atCommand_Send("VERSION");
	if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
	if(memcmp(buf, "+VERSION=", 9))			return BLE_FAULT;
	else{
		memcpy(version, &buf[9], (strlen((char *)buf)-9));
		return BLE_SUCCESS;
	}
}

/**
**@name		int8_t BLE_BaudSet(uint8_t baud)
**@func		This function is used to query baud rate or set baud rate of BLE
**@input	baud: the baud rate you want to set
**@return	The result: =0 SUCCESS, <0 FAULT, >0 The result of query
**@note		
			baud code | 0    | 1    | 2    | 3     | 4     | 5     | 6      | 7      | 8      | 9      |
			baud rate | 2400 | 4800 | 9600 | 19200 | 38400 | 57600 | 115200 | 230400 | 460800 | 921600 |
**/
int8_t BLE_BaudSet(uint8_t baud)
{
	uint8_t buf[50];
	if(baud > 9 && baud != 0xFF)	return BLE_PARA_ERR;
	
	/* Ensure that recevie buffer have no data */
	while(BLE_bufferRec.pointerRead != BLE_bufferRec.pointerWrite)
	{
		BLE_receDataHandler();
	}
	
	if(baud == 0xFF)				// It means to query the baud rate of BLE
	{
		BLE_atCommand_Send("BAUD");
		if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
		if(memcmp(buf, "+BAUD=", 6))	return BLE_FAULT;
		else
			return buf[6] & 0x0F;	// return the baud rate code which is represented ASCII code
	}
	else
	{
		sprintf((char *)buf, "BAUD=%d", baud);
		BLE_atCommand_Send((char *)buf);
		if(BLE_GetRespond(buf, 0xFFF))	return BLE_TIMEOUT;
		if(strcmp((char *)buf, "+BAUD=OK"))	return BLE_FAULT;
		else	return BLE_SUCCESS;
	}
}

/**
**@name		int8_t BLE_SetBroadcastName(uint8_t *name)
**@func		This function is used to query name or set name which is used when BLE broadcast
**@input	name: the pointer to name string
**@return	=0 SUCCESS, <0 FAULT
**/
int8_t BLE_SetBroadcastName(uint8_t *name)
{
	uint8_t buf[50];
	if(strlen((char *)name) > 18)	return BLE_PARA_ERR;
	
	/* Ensure that recevie buffer have no data */
	while(BLE_bufferRec.pointerRead != BLE_bufferRec.pointerWrite)
	{
		BLE_receDataHandler();
	}
	
	if((*name == 0) && strlen((char *)name) == 1)			// query name
	{
		BLE_atCommand_Send("NAME");
		if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
		if(memcmp(buf, "+NAME=", 6))	return BLE_FAULT;
		else{
			memcpy(name, &buf[6], (strlen((char *)buf) - 6));
		}
	}
	else
	{
		sprintf((char *)buf, "NAME=%s", name);
		if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
		if(strcmp((char *)buf, "+NAME=OK"))	return BLE_FAULT;
		if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
		if(strcmp((char *)buf, "AT+OK"))	return BLE_FAULT;
	}
	return BLE_SUCCESS;
}

/**
**@name		int8_t BLE_SetAddress(uint8_t *addr)
**@func		This function is used to query addr or set addr of BLE
**@input	name: the pointer to addr string
**@return	=0 SUCCESS, <0 FAULT
**/
int8_t BLE_SetAddress(uint8_t *addr)
{
	uint8_t buf[50];
	if(strlen((char *)addr) != 12)	return BLE_PARA_ERR;	// BLE Address should be 12 bytes data
	if(strcmp((char *)addr, "000000000000") == 0)	return BLE_PARA_ERR;		// BLE Address should not be 12 zeros
	
	/* Ensure that recevie buffer have no data */
	while(BLE_bufferRec.pointerRead != BLE_bufferRec.pointerWrite)
	{
		BLE_receDataHandler();
	}
	
	if((*addr == 0) && strlen((char *)addr) == 1)			// query addr
	{
		BLE_atCommand_Send("ADDR");
		if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
		if(memcmp(buf, "+ADDR=", 6))	return BLE_FAULT;
		else{
			memcpy(addr, &buf[6], (strlen((char *)buf) - 6));
		}
	}
	else
	{
		sprintf((char *)buf, "NAME=%s", addr);
		if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
		if(strcmp((char *)buf, "+ADDR=OK"))	return BLE_FAULT;
		if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
		if(strcmp((char *)buf, "AT+OK"))	return BLE_FAULT;
	}
	return BLE_SUCCESS;
}

/**
**@name		int8_t BLE_SetBroadcastInterval(uint8_t inter)
**@func		This function is used to query broadcast interval time or set it of BLE
**@input	inter: the code should be set
**@return	=0 SUCCESS, <0 FAULT, >0 the code of interval time
**@note		interval code(ASCII)| 0  | 1  | 2  | 3   | 4   | 5   | 6    | 7	   |
			interval time(ms) 	| 20 | 50 | 50 | 100 | 200 | 500 | 1000 | 2000 |
**/
int8_t BLE_SetBroadcastInterval(uint8_t inter)
{
	uint8_t buf[50];
	if((inter > 7) && (inter != 0xFF))	return BLE_PARA_ERR;		// BLE have 7 kind of interval time setting
	
	/* Ensure that recevie buffer have no data */
	while(BLE_bufferRec.pointerRead != BLE_bufferRec.pointerWrite)
	{
		BLE_receDataHandler();
	}
	
	if(inter == 0xFF)			// query interval
	{
		BLE_atCommand_Send("ADVINT");
		if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
		if(memcmp(buf, "+ADVINT=", 8))	return BLE_FAULT;
		else{
			return (buf[8]&0x0F);
		}
	}
	else
	{
		sprintf((char *)buf, "ADVINT=%d", inter);
		if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
		if(strcmp((char *)buf, "+ADVINT=OK"))	return BLE_FAULT;
		if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
		if(strcmp((char *)buf, "AT+OK"))	return BLE_FAULT;
	}
	return BLE_SUCCESS;
}

/**
**@name		int8_t BLE_SetConnctionInterval(uint8_t inter)
**@func		This function is used to query connection interval time or set it of BLE
**@input	inter: the code should be set
**@return	=0 SUCCESS, <0 FAULT, >0 the code of interval time
**@note		connection code(ASCII)| 0  | 1  | 2  | 3   | 4   | 5   | 6    | 7	 |
			connection time(ms)   | 20 | 50 | 50 | 100 | 200 | 500 | 1000 | 2000 |
**/
int8_t BLE_SetConnctionInterval(uint8_t inter)
{
	uint8_t buf[50];
	if((inter > 7) && (inter != 0xFF))	return BLE_PARA_ERR;		// BLE have 7 kind of connection time setting
	
	/* Ensure that recevie buffer have no data */
	while(BLE_bufferRec.pointerRead != BLE_bufferRec.pointerWrite)
	{
		BLE_receDataHandler();
	}
	
	if(inter == 0xFF)			// query interval
	{
		BLE_atCommand_Send("CONINT");
		if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
		if(memcmp(buf, "+CONINT=", 8))	return BLE_FAULT;
		else{
			return (buf[8]&0x0F);
		}
	}
	else
	{
		sprintf((char *)buf, "CONINT=%d", inter);
		if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
		if(strcmp((char *)buf, "+CONINT=OK"))	return BLE_FAULT;
	}
	return BLE_SUCCESS;
}

/**
**@name		int8_t BLE_SetTransmitPower(uint8_t power)
**@func		This function is used to query transmission power or set it of BLE
**@input	power: the code should be set
**@return	=0 SUCCESS, <0 FAULT, >0 the code of interval time
**@note		interval code(ASCII)| 0   | 1   | 2   | 3   | 4  | 5  | 6 | 7 | 8 |
			interval time(db) 	| -30 | -25 | -19 | -13 | -8 | -3 | 0 | 2 | 4 |
**/
int8_t BLE_SetTransmitPower(uint8_t power)
{
	uint8_t buf[50];
	if((power > 8) && (power != 0xFF))	return BLE_PARA_ERR;		// BLE have 8 kind of power setting
	
	/* Ensure that recevie buffer have no data */
	while(BLE_bufferRec.pointerRead != BLE_bufferRec.pointerWrite)
	{
		BLE_receDataHandler();
	}
	
	if(power == 0xFF)							// query current power setting
	{
		BLE_atCommand_Send("POWER");
		if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
		if(memcmp(buf, "+POWER=", 7))	return BLE_FAULT;
		else{
			return (buf[7]&0x0F);
		}
	}
	else
	{
		sprintf((char *)buf, "POWER=%d", power);
		if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
		if(strcmp((char *)buf, "+POWER=OK"))	return BLE_FAULT;
	}
	return BLE_SUCCESS;
}

/**
**@name		int8_t BLE_SetStandbyMode(uint8_t mode)
**@func		This function is used to query standby mode or set it of BLE
**@input	mode: the code should be set
**@return	=0 SUCCESS, <0 FAULT, >0 the code of interval time
**@note		There are two standby modes. In mode 0, BLE will open broadcast and open uart after power on .
			In mode 1, BLE open broadcast in light sleep and close uart after power on. BLE will be waken
			up after connected and enter light sleep after disconnected.
**/
int8_t BLE_SetStandbyMode(uint8_t mode)
{
	uint8_t buf[50];
	if((mode > 1) && (mode != 0xFF))	return BLE_PARA_ERR;		// BLE have 8 kind of power setting
	
	/* Ensure that recevie buffer have no data */
	while(BLE_bufferRec.pointerRead != BLE_bufferRec.pointerWrite)
	{
		BLE_receDataHandler();
	}
	
	if(mode == 0xFF)
	{
		BLE_atCommand_Send("WORKMODE");
		if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
		if(memcmp(buf, "+WORKMODE=", 10))	return BLE_FAULT;
		else{
			return (buf[10]&0x0F);
		}
	}
	else{
		sprintf((char *)buf, "WORKMODE=%d", mode);
		if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
		if(strcmp((char *)buf, "+WORKMODE=OK"))	return BLE_FAULT;
		if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
		if(strcmp((char *)buf, "AT+OK"))	return BLE_FAULT;
	}
	return BLE_SUCCESS;
}

/**
**@name		int8_t BLE_EnterSleep(void)
**@func		This function use to enter Sleep mode
**@return	=0: success, <0 fault
**/
int8_t BLE_EnterSleep(void)
{
	uint8_t buf[50];
	
	/* Ensure that recevie buffer have no data */
	while(BLE_bufferRec.pointerRead != BLE_bufferRec.pointerWrite)
	{
		BLE_receDataHandler();
	}
	
	BLE_atCommand_Send("SLEEP");
	if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
	if(strcmp((char *)buf, "+SLEEP=OK"))	return BLE_FAULT;
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	return BLE_SUCCESS;
}

/**
**@name		int8_t BLE_Disconnected(void)
**@func		This function use to make BLE disconnected
**@return	=0: success, <0 fault
**/
int8_t BLE_Disconnected(void)
{
	uint8_t buf[50];
	
	/* Ensure that recevie buffer have no data */
	while(BLE_bufferRec.pointerRead != BLE_bufferRec.pointerWrite)
	{
		BLE_receDataHandler();
	}
	
	BLE_atCommand_Send("DISC");
	if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
	if(strcmp((char *)buf, "+DISC=OK"))	return BLE_FAULT;
	return BLE_SUCCESS;
}

/**
**@name		int8_t BLE_QueryConnection(void)
**@func		This function use to query the BLE connected or not
**@return	=0: success, <0 fault, 1: disconnected, 2: connected
**/
int8_t BLE_QueryConnection(void)
{
	uint8_t buf[50];
	
	/* Ensure that recevie buffer have no data */
	while(BLE_bufferRec.pointerRead != BLE_bufferRec.pointerWrite)
	{
		BLE_receDataHandler();
	}
	
	BLE_atCommand_Send("STAT");
	if(BLE_GetRespond(buf, 0xFFF))	return BLE_FAULT;
	if(memcmp(buf, "+STAT=", 6))	return BLE_FAULT;
	else{
		return ((buf[6]+1) & 0x0F);
	}
}

/**
			BLE AT Command Hanlde Part
**/

/**
**@name	
**/
void BLE_SendByte(uint8_t ch)
{
	while(BLE_UART_Handle.Instance->FR & UART_FR_BUSY);
	BLE_UART_Handle.Instance->DR = ch;
}


/**
**@name	void UART2_IRQHandler(void)
**@func	This is a Interrupt Handler of USART2
**/
void UART3_IRQHandler(void)
{
	uint8_t ch;
	if(BLE_USART->RIS & UART_RIS_RXI)				// there are some data waiting for read
	{
		BLE_USART->ICR |= UART_ICR_RXI;
		ch = (BLE_USART->DR & 0xFF);
		BLE_bufferRec.buffer[BLE_bufferRec.pointerWrite++] = ch;
		BLE_bufferRec.pointerWrite &= (BLE_RECE_BUFFER_SIZE - 1);
	}else if(BLE_USART->RIS & UART_RIS_RTI)
	{
		BLE_USART->ICR |= UART_ICR_RTI;
		ch = (BLE_USART->DR & 0xFF);
		BLE_bufferRec.buffer[BLE_bufferRec.pointerWrite++] = ch;
		BLE_bufferRec.pointerWrite &= (BLE_RECE_BUFFER_SIZE - 1);
	}
}






