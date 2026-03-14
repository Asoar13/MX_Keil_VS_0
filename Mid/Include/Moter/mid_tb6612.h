#ifndef __MID_TB6612_H
#define __MID_TB6612_H
#include "main.h"
#include "Moter/bsp_moter.h"

typedef struct {
	// PWM控制
	void *h_timx;			// (手动配置) - 初始化结构体时手动配置，指定tim
	uint32_t TIM_CHANNEL_x; // (手动配置) - 初始化结构体时手动配置，指定PWM产生口

	// GPIO控制
	GPIO_TypeDef *gpio_stby;	// (手配置动) - 初始化结构体时手动配置
	uint32_t gpio_pin_stby;		// (手动配置) - 初始化结构体时手动配置
	GPIO_TypeDef *gpio_ctrl1;	// (手动配置) - 初始化结构体时手动配置
	uint32_t gpio_pin_ctrl1;	// (手动配置) - 初始化结构体时手动配置
	GPIO_TypeDef *gpio_ctrl2;	// (手动配置) - 初始化结构体时手动配置
	uint32_t gpio_pin_ctrl2; 	// (手动配置) - 初始化结构体时手动配置

} MID_TB6612_Hanle_t;

void MID_TB6612_init(MID_TB6612_Hanle_t *h_tb6612x);
void MID_TB6612_setMoterFreq(MID_TB6612_Hanle_t *h_tb6612x, uint32_t freq);
void MID_TB6612_setSpeedDuty(MID_TB6612_Hanle_t *h_tb6612x, uint32_t duty);
void MID_TB6612_setSpeedPeriod(MID_TB6612_Hanle_t *h_tb6612x, uint32_t period);
void MID_TB6612_rotateInReverse(MID_TB6612_Hanle_t *h_tb6612x);
void MID_TB6612_stopImmediately(MID_TB6612_Hanle_t *h_tb6612x);
void MID_TB6612_stopSlowly(MID_TB6612_Hanle_t *h_tb6612x);
void MID_TB6612_standby(MID_TB6612_Hanle_t *h_tb6612x);
void MID_TB6612_turnOffAndOn(MID_TB6612_Hanle_t *h_tb6612x);

#endif
