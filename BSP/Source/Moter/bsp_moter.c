#include <Moter/bsp_moter.h>
#include "Moter/mid_tb6612.h"

/*===================默认对外句柄=========================*/

// 默认格式
// MID_TB6612_Hanle_t h_tb6612_x = {
// 	.h_timx = &htim2,
// 	.TIM_CHANNEL_x = TIM_CHANNEL_1,

// 	.stby_gpio_conf = {
// 		.gpiox = GPIOA,
// 		.gpio_pin_x = GPIO_PIN_12,
// 	},
// 	.ctrl1_gpio_conf = {
// 		.gpiox = GPIOA,
// 		.gpio_pin_x = GPIO_PIN_10,
// 	},
// 	.ctrl2_gpio_conf = {
// 		.gpiox = GPIOA,
// 		.gpio_pin_x = GPIO_PIN_11,
// 	},
// };

/*===================默认对外句柄=========================*/

/**
 * @brief  初始化，启动PWM
 * @param  htim_x: 定时器句柄
 * @param  TIM_CHANNEL_x: 定时器通道
 * @retval 无
 */
void BSP_Moter_init(void *htim_x, uint32_t TIM_CHANNEL_x)
{
	TIM_HandleTypeDef *htimx = (TIM_HandleTypeDef*)htim_x;

	HAL_TIM_PWM_Start(htimx, TIM_CHANNEL_x);
}

/**
 * @brief  初始化，启动PWM
 * @param  htim_x: 定时器句柄
 * @param  TIM_CHANNEL_x: 定时器通道
 * @param  freq: 提供的PWM波的频率
 * @note   freq由PSC和ARR共同决定，只改变PSC保证ARR不变，不影响Period来修改频率
 * @retval 无
 */
void BSP_Moter_setFreq(void *htim_x, uint32_t TIM_CHANNEL_x, uint32_t freq)
{
	TIM_HandleTypeDef *htimx = (TIM_HandleTypeDef*)htim_x;

	// 读取当前ARR，计算PSC值
	uint32_t psc = 72000000 / freq / (htimx->Instance->ARR + 1);

	// 设置
	htimx->Instance->PSC = psc - 1;
}

/**
 * @brief  初始化，启动PWM
 * @param  htim_x: 定时器句柄
 * @param  TIM_CHANNEL_x: 定时器通道
 * @param  period: 提供的PWM波的周期(占空比分母)
 * @note   period仅由ARR决定，ARR与PSC相反倍增，ARR * PSC不变，保证Freq不变
 * @retval 无
 */
void BSP_Moter_setPeriod(void *htim_x, uint32_t TIM_CHANNEL_x, uint32_t period)
{
	TIM_HandleTypeDef *htimx = (TIM_HandleTypeDef*)htim_x;

	// 先乘再除，保证被除数大于除数
	htimx->Instance->PSC = (htimx->Instance->PSC + 1) * (htimx->Instance->ARR + 1) / period - 1;
	htimx->Instance->ARR = period - 1;
}

/**
 * @brief  初始化，启动PWM
 * @param  htim_x: 定时器句柄
 * @param  TIM_CHANNEL_x: 定时器通道
 * @param  duty: 提供的PWM波的占空比(占空比分子)
 * @note   duty仅由Compare决定
 * @retval 无
 */
void BSP_Moter_setDuty(void *htim_x, uint32_t TIM_CHANNEL_x, uint32_t duty)
{
	TIM_HandleTypeDef *htimx = (TIM_HandleTypeDef*)htim_x;

	__HAL_TIM_SET_COMPARE(htimx, TIM_CHANNEL_x, duty);
}

