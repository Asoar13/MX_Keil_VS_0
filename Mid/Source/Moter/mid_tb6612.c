#include "Moter/bsp_moter.h"
#include "Moter/mid_tb6612.h"

// 初始化
void MID_TB6612_init(MID_TB6612_Hanle_t *h_tb6612x)
{
	BSP_Moter_init(h_tb6612x->h_timx, h_tb6612x->TIM_CHANNEL_x);

	// 选择一个方向
	BSP_GPIO_setPin(&h_tb6612x->ctrl1_gpio_conf);
	BSP_GPIO_resetPin(&h_tb6612x->ctrl2_gpio_conf);

	// 默认数值
	MID_TB6612_setMoterFreq(h_tb6612x, 5000);
	MID_TB6612_setSpeedPeriod(h_tb6612x, 100);
	MID_TB6612_setSpeedDuty(h_tb6612x, 10);
	MID_TB6612_standby(h_tb6612x);
}

// 设置pwm频率
void MID_TB6612_setMoterFreq(MID_TB6612_Hanle_t *h_tb6612x, uint32_t freq)
{
	BSP_Moter_setFreq(h_tb6612x->h_timx, h_tb6612x->TIM_CHANNEL_x, freq);
}

// 设置速度占比
void MID_TB6612_setSpeedDuty(MID_TB6612_Hanle_t *h_tb6612x, uint32_t duty)
{
	BSP_Moter_setDuty(h_tb6612x->h_timx, h_tb6612x->TIM_CHANNEL_x, duty);
}

// 设置速度比最大值
void MID_TB6612_setSpeedPeriod(MID_TB6612_Hanle_t *h_tb6612x, uint32_t period)
{
	BSP_Moter_setPeriod(h_tb6612x->h_timx, h_tb6612x->TIM_CHANNEL_x, period);
}

// 反向旋转
void MID_TB6612_rotateInReverse(MID_TB6612_Hanle_t *h_tb6612x)
{
	BSP_GPIO_togglePin(&h_tb6612x->ctrl1_gpio_conf);
	BSP_GPIO_togglePin(&h_tb6612x->ctrl2_gpio_conf);
}

// 急停
void MID_TB6612_stopImmediately(MID_TB6612_Hanle_t *h_tb6612x)
{
	BSP_GPIO_setPin(&h_tb6612x->ctrl1_gpio_conf);
	BSP_GPIO_setPin(&h_tb6612x->ctrl2_gpio_conf);
}

// 慢停
void MID_TB6612_stopSlowly(MID_TB6612_Hanle_t *h_tb6612x)
{
	BSP_GPIO_resetPin(&h_tb6612x->ctrl1_gpio_conf);
	BSP_GPIO_resetPin(&h_tb6612x->ctrl2_gpio_conf);
}

void MID_TB6612_standby(MID_TB6612_Hanle_t *h_tb6612x)
{
	BSP_GPIO_resetPin(&h_tb6612x->stby_gpio_conf);
}

void MID_TB6612_turnOffAndOn(MID_TB6612_Hanle_t *h_tb6612x)
{
	BSP_GPIO_togglePin(&h_tb6612x->stby_gpio_conf);
}
