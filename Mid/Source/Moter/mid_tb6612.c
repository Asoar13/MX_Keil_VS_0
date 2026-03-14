#include "Moter/bsp_moter.h"
#include "Moter/mid_tb6612.h"

// 初始化
void MID_TB6612_init(MID_TB6612_Hanle_t *h_tb6612x)
{
	BSP_Moter_init(h_tb6612x->h_timx, h_tb6612x->TIM_CHANNEL_x);

	// 选择一个方向
	BSP_Moter_setPin_H(h_tb6612x->gpio_ctrl1, h_tb6612x->gpio_pin_ctrl1);
	BSP_Moter_setPin_L(h_tb6612x->gpio_ctrl2, h_tb6612x->gpio_pin_ctrl2);

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
	BSP_Moter_togglePin(h_tb6612x->gpio_ctrl1, h_tb6612x->gpio_pin_ctrl1);
	BSP_Moter_togglePin(h_tb6612x->gpio_ctrl2, h_tb6612x->gpio_pin_ctrl2);
}

// 急停
void MID_TB6612_stopImmediately(MID_TB6612_Hanle_t *h_tb6612x)
{
	BSP_Moter_setPin_H(h_tb6612x->gpio_ctrl1, h_tb6612x->gpio_pin_ctrl1);
	BSP_Moter_setPin_H(h_tb6612x->gpio_ctrl2, h_tb6612x->gpio_pin_ctrl2);
}

// 慢停
void MID_TB6612_stopSlowly(MID_TB6612_Hanle_t *h_tb6612x)
{
	BSP_Moter_setPin_L(h_tb6612x->gpio_ctrl1, h_tb6612x->gpio_pin_ctrl1);
	BSP_Moter_setPin_L(h_tb6612x->gpio_ctrl2, h_tb6612x->gpio_pin_ctrl2);
}

void MID_TB6612_standby(MID_TB6612_Hanle_t *h_tb6612x)
{
	BSP_Moter_setPin_L(h_tb6612x->gpio_stby, h_tb6612x->gpio_pin_stby);
}

void MID_TB6612_turnOffAndOn(MID_TB6612_Hanle_t *h_tb6612x)
{
	BSP_Moter_togglePin(h_tb6612x->gpio_stby, h_tb6612x->gpio_pin_stby);
}
