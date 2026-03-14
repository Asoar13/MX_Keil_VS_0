#include "ADC_DAC/bsp_adc.h"

#ifdef HAL_ADC_MODULE_ENABLED

	void BSP_ADC_init(void* hadcx)
	{
		HAL_ADCEx_Calibration_Start((ADC_HandleTypeDef*)hadcx);
	}

	// 错误时返回0xFFFFFFFFFFFFFFF
	uint32_t BSP_ADC_getADCValue(void* hadcx, uint32_t timeout)
	{
		HAL_ADC_Start((ADC_HandleTypeDef*)hadcx);
		if(HAL_ADC_PollForConversion((ADC_HandleTypeDef*)hadcx, timeout) != HAL_OK) return UINT32_MAX;
		return HAL_ADC_GetValue((ADC_HandleTypeDef*)hadcx);
	}


	void BSP_ADC_initAutoConver_DMA(void* hadcx, uint32_t* pData, uint32_t Length)
	{
		HAL_ADCEx_Calibration_Start((ADC_HandleTypeDef*)hadcx);
		HAL_ADC_Start_DMA((ADC_HandleTypeDef*)hadcx, pData, Length);
	}

#endif /* HAL_ADC_MODULE_ENABLED */
