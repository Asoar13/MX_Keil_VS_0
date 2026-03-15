#include "Moter/bsp_moter.h"
#include "Moter/mid_tb6612.h"

/**
  * @brief  初始化(随机一个方向，频率5kHz，精度100，实际输出10)，并立即切断电机供电
  * @param  h_tb6612x: 电机驱动句柄，绑定需要的GPIO
  * @retval 无
  */
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

// 
/**
  * @brief  设置pwm频率
  * @param  h_tb6612x: 电机驱动句柄，绑定需要的GPIO
  * @param  freq: 提供的PWM波的频率
  * @retval 无
  */
void MID_TB6612_setMoterFreq(MID_TB6612_Hanle_t *h_tb6612x, uint32_t freq)
{
	BSP_Moter_setFreq(h_tb6612x->h_timx, h_tb6612x->TIM_CHANNEL_x, freq);
}

// 
/**
  * @brief  设置速度占比
  * @param  h_tb6612x: 电机驱动句柄，绑定需要的GPIO
  * @param  duty: 比例分子(实际输出大小)
  * @retval 无
  */
void MID_TB6612_setSpeedDuty(MID_TB6612_Hanle_t *h_tb6612x, uint32_t duty)
{
	BSP_Moter_setDuty(h_tb6612x->h_timx, h_tb6612x->TIM_CHANNEL_x, duty);
}

// 
/**
  * @brief  设置速度比最大值
  * @param  h_tb6612x: 电机驱动句柄，绑定需要的GPIO
  * @param  period: 比例分母(输出精度)
  * @retval 无
  */
void MID_TB6612_setSpeedPeriod(MID_TB6612_Hanle_t *h_tb6612x, uint32_t period)
{
	BSP_Moter_setPeriod(h_tb6612x->h_timx, h_tb6612x->TIM_CHANNEL_x, period);
}

// 
/**
  * @brief  反向旋转
  * @param  h_tb6612x: 电机驱动句柄，绑定需要的GPIO
  * @retval 无
  */
void MID_TB6612_rotateInReverse(MID_TB6612_Hanle_t *h_tb6612x)
{
	BSP_GPIO_togglePin(&h_tb6612x->ctrl1_gpio_conf);
	BSP_GPIO_togglePin(&h_tb6612x->ctrl2_gpio_conf);
}

// 
/**
  * @brief  急停
  * @param  h_tb6612x: 电机驱动句柄，绑定需要的GPIO
  * @retval 无
  */
void MID_TB6612_stopImmediately(MID_TB6612_Hanle_t *h_tb6612x)
{
	BSP_GPIO_setPin(&h_tb6612x->ctrl1_gpio_conf);
	BSP_GPIO_setPin(&h_tb6612x->ctrl2_gpio_conf);
}

// 
/**
  * @brief  慢停
  * @param  h_tb6612x: 电机驱动句柄，绑定需要的GPIO
  * @retval 无
  */
void MID_TB6612_stopSlowly(MID_TB6612_Hanle_t *h_tb6612x)
{
	BSP_GPIO_resetPin(&h_tb6612x->ctrl1_gpio_conf);
	BSP_GPIO_resetPin(&h_tb6612x->ctrl2_gpio_conf);
}

/**
  * @brief  关闭电机电源
  * @param  h_tb6612x: 电机驱动句柄，绑定需要的GPIO
  * @retval 无
  */
void MID_TB6612_standby(MID_TB6612_Hanle_t *h_tb6612x)
{
	BSP_GPIO_resetPin(&h_tb6612x->stby_gpio_conf);
}

/**
  * @brief  开启或关闭电机
  * @param  h_tb6612x: 电机驱动句柄，绑定需要的GPIO
  * @retval 无
  */
void MID_TB6612_turnOffAndOn(MID_TB6612_Hanle_t *h_tb6612x)
{
	BSP_GPIO_togglePin(&h_tb6612x->stby_gpio_conf);
}
