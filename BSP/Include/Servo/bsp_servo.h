#ifndef __BSP_SERVO_H
#define __BSP_SERVO_H
#include "main.h"

void BSP_Servo_init(TIM_HandleTypeDef *htimx, uint32_t TIM_CHANNEL_x);
void BSP_Servo_setAngle_2500(TIM_HandleTypeDef *htimx, uint32_t TIM_CHANNEL_x, uint32_t compare);
void BSP_Servo_setAngle_180(TIM_HandleTypeDef *htimx, uint32_t TIM_CHANNEL_x, uint32_t angle);

#endif
