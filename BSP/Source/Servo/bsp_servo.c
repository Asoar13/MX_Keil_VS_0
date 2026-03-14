#include <Servo/bsp_servo.h>

void BSP_Servo_init(TIM_HandleTypeDef *htimx, uint32_t TIM_CHANNEL_x)
{
	HAL_TIM_PWM_Start(htimx, TIM_CHANNEL_x);
}

void BSP_Servo_setAngle_2500(TIM_HandleTypeDef *htimx, uint32_t TIM_CHANNEL_x, uint32_t compare)
{
	__HAL_TIM_SET_COMPARE(htimx, TIM_CHANNEL_x, compare);
}

void BSP_Servo_setAngle_180(TIM_HandleTypeDef *htimx, uint32_t TIM_CHANNEL_x, uint32_t angle)
{
	// [0, 180] => [500, 2500]
	uint32_t compare = 500 + (uint32_t)(angle / 180 * 2000) % 2001;
	__HAL_TIM_SET_COMPARE(htimx, TIM_CHANNEL_x, compare);
}
