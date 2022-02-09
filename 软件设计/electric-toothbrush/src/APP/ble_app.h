#ifndef		__BLE_APP_H__
#define		__BLE_APP_H__

#include "ACM32F0x0.h"
#include "config.h"

#define		STX		0x02

typedef __packed struct{
	uint32_t time;					// This shows the relative time of each activity
	uint16_t location;				// This shows the brush loaction when using
	uint16_t pressure;				// This is the force between tooth and brush
	uint16_t angle;					// This shows the angle between brush and horizon
	
}BLE_RealTimeDataDef;				// This define the data structure about brushing tooth in real time

typedef struct ble_Brush* ble_brush;

typedef struct ble_Brush{
	uint8_t nodeNum;					// the node label number
	uint8_t brushMode;					// the mode which the brush work in
	BLE_RealTimeDataDef	brushData_RL;	// the brushing data of this node
}BLE_BrushingParaDef;


/**
		The structure following is used to transmit brushing data to Phone. Crc is chosen to be check digital.
		All of the data in following structure should participate in CRC calculation EXCEPT HEADER. For half-
		word data, the low byte will be sent first.
**/
typedef struct{
	uint8_t Header;					// This is the start flag of a transmission
	uint8_t	Command;				// This is a byte which indicate what should do.
	uint8_t len;					// The transmission data length
	uint8_t	*dat;					// Pointer to data which is waited for transmitting
	uint16_t crc;					// Check digit
}BLE_TransmitStructureDef;

typedef	struct{
	uint8_t isConnected;
	
}BLE_HandleDef;


extern BLE_RealTimeDataDef	ble_rtLocation;
extern BLE_BrushingParaDef	ble_brushingPara;



int8_t appBReceComLookUp(uint8_t *ch);
void appBInit(void);
void appBRecordBrushingData(void);
void appBSendBrushingData(void);

extern int8_t (*appBFuncHandler[])(uint8_t *buf, uint16_t len);

#endif	











