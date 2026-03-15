#ifndef __BSP_MOTER_H
#define __BSP_MOTER_H
#include "main.h"

#ifdef HAL_TIM_MODULE_ENABLED
	#include "tim.h"

	void BSP_Moter_init(void *htim_x, uint32_t TIM_CHANNEL_x);
	void BSP_Moter_setFreq(void *htim_x, uint32_t TIM_CHANNEL_x, uint32_t freq);
	void BSP_Moter_setPeriod(void *htim_x, uint32_t TIM_CHANNEL_x, uint32_t period);
	void BSP_Moter_setDuty(void *htim_x, uint32_t TIM_CHANNEL_x, uint32_t duty);

#endif /* HAL_TIM_MODULE_ENABLED */

#endif
