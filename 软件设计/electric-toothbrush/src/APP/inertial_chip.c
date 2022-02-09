#include	"includes.h"

void QMI8658_Init(void)
{
	IIC_Sim_Init(0);
}


void QMI8658_Test(void)
{
	static uint8_t i = 0;
	uint8_t dat = 0xFF;
//	for(i=0;i<0x61;i++)
	{
		IIC_Sim_Start_Generator();	

		IIC_Sim_WriteAddr(0x6A, IIC_WRITE);
		
		IIC_Sim_WriteDat(i);
		
		IIC_Sim_Start_Generator();
		
		IIC_Sim_WriteAddr(0x6A, IIC_READ);

		dat = IIC_Sim_ReadDat(1);
		
		IIC_Sim_Stop_Generator();
		
		printfS("IIC read addr: %02x, dat: %02x\r\n", i, dat);
		if(++i > 0x61)	i = 0;
	}
}


void EEPROM_24LC256_Test(void)
{
	uint8_t dat = 0xFF;
//	for(i=0;i<0x61;i++)
	{
		IIC_Sim_Start_Generator();	

		IIC_Sim_WriteAddr(0x50, IIC_WRITE);
		
		IIC_Sim_WriteDat(0x00);
		
		IIC_Sim_WriteDat(0x00);
		
		IIC_Sim_WriteDat(0x55);
		
		IIC_Sim_Stop_Generator();
		
//		IIC_Sim_WriteDat(i);
		System_Delay_MS(10);
		
		IIC_Sim_Start_Generator();	

		IIC_Sim_WriteAddr(0x50, IIC_WRITE);
		
		IIC_Sim_WriteDat(0x00);
		
		IIC_Sim_WriteDat(0x00);
		
		IIC_Sim_Start_Generator();
		
		IIC_Sim_WriteAddr(0x50, IIC_READ);

		dat = IIC_Sim_ReadDat(1);
		
		IIC_Sim_Stop_Generator();
		
		printfS("IIC read dat: %02x\r\n", dat);
//		if(++i > 0x61)	i = 0;
	}
}













