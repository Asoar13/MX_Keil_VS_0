#ifndef __BSP_ADC_H
#define __BSP_ADC_H
#include "main.h"

#ifdef HAL_ADC_MODULE_ENABLED
#include "adc.h"
#endif /* HAL_ADC_MODULE_ENABLED */

#define BSP_ADC_MAX	4095

void BSP_ADC_init(void* hadcx);
uint32_t BSP_ADC_getADCValue(void* hadcx, uint32_t timeout);
void BSP_ADC_initAutoConver_DMA(void* hadcx, uint32_t* pData, uint32_t Length);

#endif
