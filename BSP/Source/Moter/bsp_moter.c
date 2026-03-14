#include <Moter/bsp_moter.h>
#include "Moter/mid_tb6612.h"

/*===================默认对外句柄=========================*/

// MID_TB6612_Hanle_t h_tb6612 = {
// 		.h_timx = &htim2,
// 		.TIM_CHANNEL_x = TIM_CHANNEL_1,

// 		.gpio_ctrl1 = GPIOB,
// 		.gpio_pin_ctrl1 = GPIO_PIN_9,
// 		.gpio_ctrl2 = GPIOB,
// 		.gpio_pin_ctrl2 = GPIO_PIN_8,
// };

/*===================默认对外句柄=========================*/

void BSP_Moter_init(void *htim_x, uint32_t TIM_CHANNEL_x)
{
	TIM_HandleTypeDef *htimx = (TIM_HandleTypeDef*)htim_x;

	HAL_TIM_PWM_Start(htimx, TIM_CHANNEL_x);
}

// 由PSC和ARR共同决定
// 只改变PSC保证ARR不变，不影响Period来修改频率
void BSP_Moter_setFreq(void *htim_x, uint32_t TIM_CHANNEL_x, uint32_t freq)
{
	TIM_HandleTypeDef *htimx = (TIM_HandleTypeDef*)htim_x;

	// 读取当前ARR，计算PSC值
	uint32_t psc = 72000000 / freq / (htimx->Instance->ARR + 1);

	// 设置
	htimx->Instance->PSC = psc - 1;
}

// 仅由ARR决定
// ARR与PSC相反倍增，ARR * PSC不变，保证Freq不变
void BSP_Moter_setPeriod(void *htim_x, uint32_t TIM_CHANNEL_x, uint32_t period)
{
	TIM_HandleTypeDef *htimx = (TIM_HandleTypeDef*)htim_x;

	// 先乘再除，保证被除数大于除数
	htimx->Instance->PSC = (htimx->Instance->PSC + 1) * (htimx->Instance->ARR + 1) / period - 1;
	htimx->Instance->ARR = period - 1;
}

// 仅由Compare决定
// 设置compare即可
void BSP_Moter_setDuty(void *htim_x, uint32_t TIM_CHANNEL_x, uint32_t duty)
{
	TIM_HandleTypeDef *htimx = (TIM_HandleTypeDef*)htim_x;

	__HAL_TIM_SET_COMPARE(htimx, TIM_CHANNEL_x, duty);
}

void BSP_Moter_togglePin(void *gpiox, uint32_t gpio_pin_x)
{
	HAL_GPIO_TogglePin((GPIO_TypeDef *)gpiox, gpio_pin_x);
}

void BSP_Moter_setPin_L(void *gpiox, uint32_t gpio_pin_x)
{
	GPIO_TypeDef *gpio = (GPIO_TypeDef *)gpiox;
	gpio->BSRR = gpio_pin_x << 16;
}

void BSP_Moter_setPin_H(void *gpiox, uint32_t gpio_pin_x)
{
	GPIO_TypeDef *gpio = (GPIO_TypeDef *)gpiox;
	gpio->BSRR = gpio_pin_x;
}
