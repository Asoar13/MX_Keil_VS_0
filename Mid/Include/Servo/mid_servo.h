#ifndef __MID_SERVO_H
#define __MID_SERVO_H
#include "main.h"

typedef struct {
	TIM_HandleTypeDef *htimx;
	uint32_t TIM_CHANNEL_x;

} MID_ServoHandle_t;

#endif
