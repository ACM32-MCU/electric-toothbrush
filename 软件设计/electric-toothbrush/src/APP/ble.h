#ifndef	__BLE_H__
#define	__BLE_H__

#include "ACM32F0x0.h"

/**
		Definition of Status when Send AT Command
**/
typedef enum{
	BLE_SUCCESS 		= 0,
	BLE_CRCERR			= -4,
	BLE_TIMEOUT			= -3,
	BLE_PARA_ERR		= -2,
	BLE_FAULT			= -1
}BLE_ReturnValueDef;

/**
		Definition of BLE Status
**/
typedef enum{
	BLE_UNCONNECTED 	= 0,
	BLE_CONNECTED		= 1					/* Under this state, BLE can enter AT Command mode by setting CTRL low. */
}BLE_StatusDef;


/**
		Definition of BLE Mode
**/
typedef enum{
	BLE_MODE_DEEPSLEEP 		= 0,				/* Entered by transmit "AT+SLEEP", keep CTRL port on high. Exited by Set CTRL port	*/
	BLE_MODE_LIGHTSLEEP		= 1,				
	BLE_MODE_NORMAL			= 2,
	BLE_MODE_STANDBY_1		= 3,				/* Entered by transmit "AT+WORKMODE=0". In this mode, Module is in AT Command Mode whatever
											CTRL port is low or not when BLE is unconnected. */
	BLE_STANDBY_2			= 4					/* Entered by transmit "AT+WORKMODE=1". In this mode, if you want BLE work on AT Command 
											Mode, you should set CTRL port low*/
}BLE_ModeDef;

/**
		Definition UUID of BLE Service
**/
#define		BLE_UUID_PRIMARY_SERVICE		0x0001
#define		BLE_UUID_CHARACTERISTIC_NOTIFY	0x0002
#define		BLE_UUID_CHARACTERISTIC_WRITE	0x0003
#define		BLE_UUID_OTA_SERVICE			0xFF00
#define		BLE_UUID_OTA_READ_WRITE			0xFF01


void BLE_Init(uint32_t baud);					/* BLE Initialize function */
BLE_StatusDef	BLE_isConnected(void);			/* This function is used to check BLE connected or not */
int8_t	BLE_atCommand_Send(char *str);			/* This function is used to send AT Command */
void BLE_receDataHandler(void);					/* This function is used to handle with RECE Data from BLE except RESPOND of AT Command */
void BLE_SendByte(uint8_t ch);
uint8_t BLE_GetRecByte(void);
int8_t BLE_GetRecBytes(uint8_t *dst, uint32_t len, uint32_t timeout);


int8_t BLE_Reset(void);
int8_t BLE_Factory(void);
int8_t BLE_GetVersion(uint8_t *version);
int8_t BLE_BaudSet(uint8_t baud);
int8_t BLE_SetBroadcastName(uint8_t *name);
int8_t BLE_SetAddress(uint8_t *addr);
int8_t BLE_SetBroadcastInterval(uint8_t inter);
int8_t BLE_SetConnctionInterval(uint8_t inter);
int8_t BLE_SetTransmitPower(uint8_t power);
int8_t BLE_SetStandbyMode(uint8_t mode);
int8_t BLE_EnterSleep(void);
int8_t BLE_Disconnected(void);
int8_t BLE_QueryConnection(void);

#endif



