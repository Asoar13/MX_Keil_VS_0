#ifndef __BSP_ENCODER_H
#define __BSP_ENCODER_H
#include "main.h"

uint8_t BSP_Encoder_startCount(TIM_HandleTypeDef *htimx, uint32_t Channel);
uint32_t BSP_Encoder_getCurCount(TIM_HandleTypeDef *htimx);

#endif
