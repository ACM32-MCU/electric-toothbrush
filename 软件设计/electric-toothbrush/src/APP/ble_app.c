#include "includes.h"


BLE_HandleDef ble_user;
BLE_RealTimeDataDef	ble_rtLocation = {0, 0, 0, 0};
BLE_BrushingParaDef	ble_brushingPara;

/**
**@name	void appBInit(void)
**@func	Init the BLE APP Fucntion 
**/
void appBInit(void)
{
	System_Module_Enable(EN_CRC);
	// CRC Init
	CRC->INIT = 0xFFFF;						// Set 0xFFFF to be the initial data of CRC
	CRC->CTRL |= (1ul << 3);				// PLOY LEN set to be 16 bits
//	CRC->CTRL |= ((1ul << 7) | (1ul << 5));	// REVIN & REVOUT
	CRC->POLY = 0x8005ul;					// Set POLY
//	CRC->OUTXOR = 0xFFFF;
	
	CRC->CTRL |= (1u << 0);					// RESET CRC
	
	memset((uint8_t *)(&ble_rtLocation), 0, sizeof(ble_rtLocation));
}

/**
**@name	void appBSendData(BLE_TransmitStructureDef *ble_dat)
**@func Transmit data to BLE
**@input ble_dat: the data to be sent
**/
void appBSendData(BLE_TransmitStructureDef *ble_dat)
{
	BLE_SendByte(ble_dat->Header);	
	CRC->CTRL |= 1u;					// reset CRC module
	
	// Send Command
	CRC->DATA = ble_dat->Command;
	BLE_SendByte(ble_dat->Command);
	
	// Send Data Len
	CRC->DATA = (ble_dat->len & 0xFF);
	BLE_SendByte(ble_dat->len & 0xFF);
	
	if(ble_dat->len)					// there are data which should be sent. if not, the function will send CRC directly
	{
		uint16_t i;
		
		// Send data
		for(i=0;i<ble_dat->len;i++)
		{
			CRC->DATA = ble_dat->dat[i];
			BLE_SendByte(ble_dat->dat[i]);
		}
	}

#ifdef	CRC_SUPPORT	
	// Send CRC
	ble_dat->crc = CRC->DATA & 0xFFFF;
	BLE_SendByte(ble_dat->crc & 0xFF);
	BLE_SendByte((ble_dat->crc >> 8) & 0xFF);
#endif
}

/**
**@name		int8_t appBGetCRCResult(uint8_t *src, uint32_t len)
**@func		Get result of CRC
**@input	
			src:	pointer to first address of data
			len:	the length of data which need to get CRC
**@return	
**/
int8_t appBGetCRCResult(uint8_t *src, uint32_t len)
{
	uint32_t i=0;
	uint32_t crc;
	
	CRC->CTRL |= 1u;					// reset CRC module
	
	for(i=0;i<len;i++)
	{
		CRC->DATA = src[i];
	}
	crc = src[i] + (((uint16_t)src[i+1]) << 8);
	if(crc == CRC->DATA)
		return BLE_SUCCESS;
	else
		return BLE_FAULT;
}

/**
**@name		int8_t appBReceComLookUp(uint8_t ch)
**@func		acquire pointer to handle fucntion
**@input	ch: pointer to all of received data
**@return	=0 success, !=0 fault
**/
int8_t appBReceComLookUp(uint8_t *ch)
{
	uint32_t len=0;
	DEBUG_BLE("%s,%s", __FUNCTION__,ch);
	
	len = ch[1] + (((uint16_t)ch[2]) << 8);

#ifdef	CRC_SUPPORT
	if(appBGetCRCResult(ch, len+3))				// 
	{
		DEBUG_BLE("CRC check error\r\n");
		return BLE_CRCERR;
	}
#endif
	
	appBFuncHandler[ch[0]](&ch[3], len);
	
	return BLE_SUCCESS;
}

/**
**@name		void BLE_RecordBrushingData(void)
**@func		This function will be called by timer1, which is means the function called per milisecond.
**/
void appBRecordBrushingData(void)
{
	static uint16_t cnt = 0;
	if(cnt == 0)
	{
		if(ble_brushingPara.nodeNum != sys.times)	ble_brushingPara.nodeNum = sys.times;
		if(ble_brushingPara.brushMode != sys.motorStatus) ble_brushingPara.brushMode = sys.motorStatus;
		ble_brushingPara.brushData_RL = ble_rtLocation;
		appBSendBrushingData();
	}
	if(++cnt > 499)	cnt = 0;
	
	// record the brushing data
	{
		ble_rtLocation.time ++;
	}
}


/**
**@name		void appBSendBrushingData(void)
**@func		This function will be called before sleep.
**/
void appBSendBrushingData(void)
{
	uint8_t *buf;
	
	BLE_TransmitStructureDef	ble_sender;
	buf = mymalloc(6);
	
	buf[0] = ble_brushingPara.nodeNum;
	buf[1] = ble_brushingPara.brushMode;
	memcpy(&buf[2], (uint8_t *)(&ble_brushingPara.brushData_RL), sizeof(BLE_RealTimeDataDef));
	
	ble_sender.Header = STX;
	ble_sender.Command = 0x03;
	ble_sender.len = 2 + sizeof(BLE_RealTimeDataDef);
	ble_sender.dat = buf;
	
	appBSendData(&ble_sender);
	myfree(buf);
}

/**
**@name		int8_t appBModeSwitch(uint8_t *buf, uint16_t len)
**@func		Control Motor work mode by BLE
**@input	
			buf: Pointer to data
			len: the length of valid data
**@return	=0 success, !=0 fault
**/
int8_t appBModeSwitch(uint8_t *buf, uint16_t len)
{
	if(buf[0] > 2)
	{	
		DEBUG_BLE("%s, para erro \r\n", __FUNCTION__);
		return BLE_PARA_ERR;
	}
	else
	{	
		sys.motorStatus = buf[0];
		WriteWordToFlash((uint32_t *)(&sys), (uint32_t *)STORE_ADDR, (sizeof(sys)));
		DEBUG_BLE("%s, motor mode: %d \r\n", __FUNCTION__, sys.motorStatus);
	}
	return BLE_SUCCESS;
}

/**
**@Brief	The following is a function array which is used to handle 
			with received data from BLE. 
			
**/
int8_t (*appBFuncHandler[])(uint8_t *buf, uint16_t len) = {
	appBModeSwitch,
	NULL
};




