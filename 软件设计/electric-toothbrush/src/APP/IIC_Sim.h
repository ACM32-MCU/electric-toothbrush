#ifndef		__IIC_SIM_H__
#define		__IIC_SIM_H__

#include	"ACM32F0x0.h"
#include	"port.h"

enum{
	IIC_WRITE = 0,
	IIC_READ
};

//#define		IIC_SIM_TIMER


#ifdef		IIC_SIM_TIMER
//#define		IIC_ACK

#define		IIC_SIM_BPS_MAX			100000

typedef enum {
	IIC_SIM_IDLE	= 0,
	IIC_SIM_START,
	IIC_SIM_ADDR,
	IIC_SIM_READ,
	IIC_SIM_WRITE,
	IIC_SIM_ACK_S,
	IIC_SIM_ACK_R,
	IIC_SIM_STOP
}IIC_SimStateTypeDef;

typedef struct{
	TIM_TypeDef		*TIMx;
	uint32_t period;			// Timer Loader
	uint8_t status;				/*!> A state machine of IIC controller, reference @IIC_SimStateTypeDef*/
	uint8_t busy;				/*!> Use to indicater that IIC is used or not. */
	
	uint8_t *tx_buf;			// TX data register
	uint32_t tx_len;			//
	uint8_t tx_done;			// TX done flag
	
	uint8_t *rx_buf;			// RX data register
	uint8_t rx_done;			// RX done flag
	uint32_t rx_len;			//
	
	uint8_t addr;
}IIC_SIM_TypeDef;


void IIC_Sim_MspInit(void);

void IIC_SIM_WriteOperation(uint8_t addr, uint8_t *tx, uint32_t len);
void IIC_SIM_ReadOperation(uint8_t addr, uint8_t *tx, uint32_t len);

#else

#define		IIC_SIM_CLK_FREQ			200		// this value is a parameter which is used when called System_Delay.

HAL_StatusTypeDef IIC_Sim_Init(uint32_t freq);

void IIC_Sim_Stop_Generator(void);
void IIC_Sim_Start_Generator(void);
HAL_StatusTypeDef IIC_Sim_WriteAddr(uint8_t addr, uint8_t wr);
HAL_StatusTypeDef IIC_Sim_WriteDat(uint8_t dat);
uint8_t IIC_Sim_ReadDat(uint8_t ack);

#endif		/*	IIC_SIM_TIMER	*/

#endif		/*	__IIC_SIM_H__	*/






