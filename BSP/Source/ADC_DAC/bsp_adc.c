#include "ADC_DAC/bsp_adc.h"

#ifdef HAL_ADC_MODULE_ENABLED

	/**
  	* @brief  初始化校准ADC
 	* @param  hadcx: ADC的指针
 	* @retval 无
 	*/
	void BSP_ADC_init(void* hadcx)
	{
		HAL_ADCEx_Calibration_Start((ADC_HandleTypeDef*)hadcx);
	}

	/**
  	* @brief  读取ADC
 	* @param  hadcx: ADC的指针
 	* @param  timeout: 读取时间限制
 	* @retval 正常读取: 正常的值(0 - 4095)
	* @note   仅适合一个通道时
 	* @retval 正常时: 0 - 4095 的值
 	* @retval 错误时: 0xFFFFFFFFFFFFFFF
 	*/
	uint32_t BSP_ADC_getADCValue(void* hadcx, uint32_t timeout)
	{
		HAL_ADC_Start((ADC_HandleTypeDef*)hadcx);
		if(HAL_ADC_PollForConversion((ADC_HandleTypeDef*)hadcx, timeout) != HAL_OK) return UINT32_MAX;
		return HAL_ADC_GetValue((ADC_HandleTypeDef*)hadcx);
	}

	/**
  	* @brief  初始化校准ADC，启动DMA(DMA联动)
 	* @param  hadcx: ADC的指针
 	* @param  pData: 接收ADC值的数组
 	* @param  Length: 数组长度
 	* @retval 无
 	*/
	void BSP_ADC_initAutoConver_DMA(void* hadcx, uint32_t* pData, uint32_t Length)
	{
		HAL_ADCEx_Calibration_Start((ADC_HandleTypeDef*)hadcx);
		HAL_ADC_Start_DMA((ADC_HandleTypeDef*)hadcx, pData, Length);
	}

#endif /* HAL_ADC_MODULE_ENABLED */
