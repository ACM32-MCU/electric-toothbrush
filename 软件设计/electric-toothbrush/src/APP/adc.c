/**
*************************************************************************
**@brief	This file program for adc peripheral. ADC is used to monitor
**			the voltage of device, and it will treasure the voltage per
**			second. ADC cooperate with DMA which will handle with the adc
**			data.
**@adcPort	PC5 - ADC_Channel_6
**@dmaCH	DMA_Channel2
*************************************************************************
**/

#include	"includes.h"

static uint32_t VREF;			// this is 

ADC_HandleTypeDef	ADC_Handle;
DMA_HandleTypeDef	adcDMA_Handle;
ADC_ChannelConfTypeDef	ADC_Channel_Handle;
ADC_AnalogWDGConfTypeDef	ADC_WDT_Handle;
ADC_ResultDef adcTest;

void DMA_errorHandle(void)
{
	printfS("DMA encounter error\r\n");
}

void getADCResult(void)
{
	uint8_t i;
	uint32_t tmp = 0;
	for(i=0;i<10;i++)
	{
		tmp += (adcTest.adcTemp[i] & 0x0FFF);
	}
	adcTest.adcResult = (tmp / 10);
	ADC->CR2 |= (1u << 2);						// stop adc
}

/************************************************************************
 * function   : ADC_GetVrefP
 * Description: ADC Get The VrefP Value. 
 ************************************************************************/ 
uint32_t ADC_GetVrefP(void)
{
    ADC_ChannelConfTypeDef ADC_ChannelConf;
    uint32_t TrimValue_3v, AdcValue_VrefP, VrefP,temp;
    
    ADC_Handle.Init.ClockDiv = ADC_CLOCK_DIV8;
    ADC_Handle.Init.ConConvMode = ADC_CONCONVMODE_DISABLE;
    ADC_Handle.Init.JChannelMode = ADC_JCHANNELMODE_DISABLE;
    ADC_Handle.Init.DiffMode = ADC_DIFFMODE_DISABLE;
    ADC_Handle.Init.DMAMode = ADC_DMAMODE_DISABLE;
    ADC_Handle.Init.OverMode = ADC_OVERMODE_DISABLE;
    ADC_Handle.Init.OverSampMode = ADC_OVERSAMPMODE_DISABLE;
    ADC_Handle.Init.AnalogWDGEn = ADC_ANALOGWDGEN_DISABLE;
    ADC_Handle.Init.ExTrigMode.ExTrigSel = ADC_SOFTWARE_START;
    ADC_Handle.Init.ChannelEn = ADC_CHANNEL_VBGR_EN;

    ADC_Handle.Instance = ADC;
        
    HAL_ADC_Init(&ADC_Handle);

    /* The total adc regular channels number */
    ADC_Handle.ChannelNum = 1;
    
    /* Add adc channels */
    ADC_ChannelConf.Channel = ADC_CHANNEL_VBGR;
    ADC_ChannelConf.RjMode = 0;
    ADC_ChannelConf.Sq = ADC_SEQUENCE_SQ1;
    ADC_ChannelConf.Smp = ADC_SMP_CLOCK_320;    
    HAL_ADC_ConfigChannel(&ADC_Handle,&ADC_ChannelConf);
    
    HAL_ADC_Polling(&ADC_Handle, &AdcValue_VrefP, ADC_Handle.ChannelNum, 0);
    
    TrimValue_3v = *(volatile uint32_t*)(0x00080240); //Read the 1.2v trim value in 3.0v vrefp.

    if(((~TrimValue_3v&0xFFFF0000)>>16) == (TrimValue_3v&0x0000FFFF))
    {
        temp = TrimValue_3v & 0xFFF;
        
        VrefP = (uint32_t)(temp * 3000 / (AdcValue_VrefP & 0xFFF));
                
        return VrefP;
    }
    else
        return 0;
}

/**
**@name void adcDMA_Init(void)
**@func	Configure adc->DR 2 mem by dma
**/
void adcDMA_Init(void)
{
	adcDMA_Handle.Instance 				= DMA_Channel0;
	adcDMA_Handle.Init.Data_Flow		= DMA_DATA_FLOW_P2M;
	adcDMA_Handle.Init.Desination_Inc	= DMA_DST_ADDR_INCREASE_ENABLE;
	adcDMA_Handle.Init.Desination_Width	= DMA_DST_WIDTH_WORD;
	adcDMA_Handle.Init.Mode				= DMA_CIRCULAR;
	adcDMA_Handle.Init.Source_Inc		= DMA_SOURCE_ADDR_INCREASE_DISABLE;
	adcDMA_Handle.Init.Source_Width		= DMA_SRC_WIDTH_WORD;
	adcDMA_Handle.Init.Request_ID		= REQ0_ADC;
	
	adcDMA_Handle.DMA_ITC_Callback		= getADCResult;
	adcDMA_Handle.DMA_IE_Callback		= DMA_errorHandle;
	
	HAL_DMA_Init(&adcDMA_Handle);
}

/**
**@name void ADC_Init(void)
**@func	Initialize the ADC module, use 
**/
void ADC_Init(uint8_t channel)
{
//	uint8_t i;
	
	VREF = ADC_GetVrefP();
	
	ADC_Handle.Instance							= ADC;
	
	ADC_Handle.Init.AnalogWDGEn					= ADC_ANALOGWDGEN_ENABLE;
	ADC_Handle.Init.ChannelEn					= (1u << channel);
	ADC_Handle.Init.ClockDiv					= ADC_CLOCK_DIV9;
	ADC_Handle.Init.ConConvMode					= ADC_CONCONVMODE_DISABLE;
	ADC_Handle.Init.DiffMode					= ADC_DIFFMODE_DISABLE;
	ADC_Handle.Init.DMAMode						= ADC_DMAMODE_DISABLE;
	ADC_Handle.Init.ExTrigMode.ExTrigSel		= ADC_SOFTWARE_START;
	ADC_Handle.Init.ExTrigMode.JExTrigSel		= ADC_SOFTWARE_START;
	ADC_Handle.Init.JChannelMode				= ADC_JCHANNELMODE_DISABLE;
	ADC_Handle.Init.OverMode					= ADC_OVERMODE_DISABLE;
	ADC_Handle.Init.OverSampMode				= ADC_OVERSAMPMODE_DISABLE;
	ADC_Handle.Init.Oversampling.Ratio			= ADC_CR2_OVSR_16X;
	ADC_Handle.Init.Oversampling.RightBitShift	= ADC_CR2_OVSS_1;
	ADC_Handle.Init.Oversampling.TriggeredMode	= 0;
	
	ADC_Handle.ChannelNum 						= 1;
	
//	HAL_ADC_Init(&ADC_Handle);
	
	// Channel Config
	ADC_Channel_Handle.Channel					= channel;
	ADC_Channel_Handle.RjMode					= 0;
	ADC_Channel_Handle.Smp						= ADC_SMP_CLOCK_320;
	
	// ADC Watchdog config
	ADC_WDT_Handle.ITMode						= ENABLE;
	ADC_WDT_Handle.WatchdogMode					= ADC_ANALOGWATCHDOG_RCH_ALL;
	ADC_WDT_Handle.Channel						= channel;
	ADC_WDT_Handle.HighThreshold				= (HIGH_POWER_THS * 0x0FFF) / VREF ;
	ADC_WDT_Handle.LowThreshold					= (LOW_POWER_THS * 0x0FFF) / VREF ;	
	
	ADC_Handle.DMA_Handle = &adcDMA_Handle;
		
	HAL_ADC_Init(&ADC_Handle);
	HAL_ADC_AnalogWDGConfig(&ADC_Handle, &ADC_WDT_Handle);
	
//	for(i=0;i<10;i++)
//	{
//		ADC_Channel_Handle.Sq					= (i + 1);
//		HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Channel_Handle);
//	}

	ADC_Channel_Handle.Sq					= (1);
	HAL_ADC_ConfigChannel(&ADC_Handle, &ADC_Channel_Handle);

	HAL_ADC_Start(&ADC_Handle);

}

void DMA_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&adcDMA_Handle);
}

void ADC_IRQHandler(void)
{
	uint32_t tmp;
	if(ADC->SR & (1u << 5))
	{
		tmp = ADC->DR & 0xFFFF;
		if(tmp <= (ADC->LTR & 0x0FFF))
		{
			sys.adcWatchDogLow = true;
		}
		else if(tmp >= (ADC->HTR & 0x0FFF))
		{
			sys.adcWatchDogHigh = true;
		}
//		printfS("ADC WatchDog\r\n");
	}
	ADC->SR |= (0x3F);
}






