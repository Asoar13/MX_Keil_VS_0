#include "Seg/bsp_seg.h"

// 指针转换
static void BSP_Seg_setL(void *gpiox, uint32_t gpio_pin_x)
{
	GPIO_TypeDef *gpio = (GPIO_TypeDef *)gpiox;
	gpio->BSRR = gpio_pin_x << 16;
}

static void BSP_Seg_setH(void *gpiox, uint32_t gpio_pin_x)
{
	GPIO_TypeDef *gpio = (GPIO_TypeDef *)gpiox;
	gpio->BSRR = gpio_pin_x;
}

// 简化写法（减少参数）
#define BSP_SEG_SET_SCK_H(p_seg_conf) BSP_Seg_setH(p_seg_conf->gpio_sck, p_seg_conf->gpio_pin_sck)
#define BSP_SEG_SET_SCK_L(p_seg_conf) BSP_Seg_setL(p_seg_conf->gpio_sck, p_seg_conf->gpio_pin_sck)

#define BSP_SEG_SET_SDA_H(p_seg_conf) BSP_Seg_setH(p_seg_conf->gpio_sda, p_seg_conf->gpio_pin_sda)
#define BSP_SEG_SET_SDA_L(p_seg_conf) BSP_Seg_setL(p_seg_conf->gpio_sda, p_seg_conf->gpio_pin_sda)


static void BSP_SEG_DELAY_US()
{
	uint32_t i = 135;
	while(i--){}
}

/*-------------------------------------------------对外函数接口----------------------------------------------------*/

void BSP_Seg_start(BSP_Seg_GpioConf_t *p_seg_conf)
{
	// 归位
	BSP_SEG_SET_SCK_H(p_seg_conf);	// 时钟上拉
	BSP_SEG_SET_SDA_H(p_seg_conf);	// 数据上拉
	BSP_SEG_DELAY_US();

	// 开始信号
	BSP_SEG_SET_SDA_L(p_seg_conf);	// 数据下拉
	BSP_SEG_DELAY_US();

	BSP_SEG_SET_SCK_L(p_seg_conf);	// 数据下拉
	BSP_SEG_DELAY_US();
}

void BSP_Seg_stop(BSP_Seg_GpioConf_t *p_seg_conf)
{
	// 归位
	BSP_SEG_SET_SDA_L(p_seg_conf);	// 数据下拉
	BSP_SEG_SET_SCK_L(p_seg_conf);	// 时钟下拉
	BSP_SEG_DELAY_US();

	// 结束信号
	BSP_SEG_SET_SCK_H(p_seg_conf);  // 时钟上拉
	BSP_SEG_DELAY_US();

	BSP_SEG_SET_SDA_H(p_seg_conf);	// 数据上拉
	BSP_SEG_DELAY_US();
}

void BSP_Seg_transmitByte(BSP_Seg_GpioConf_t *p_seg_conf, uint8_t byte)
{
	// 前8个SCK发送
	for(uint8_t i = 0; i < 8; i++) {
		// 下降SCK写数据
		BSP_SEG_SET_SCK_L(p_seg_conf);
		BSP_SEG_DELAY_US();
		if(byte & 0x01) BSP_SEG_SET_SDA_H(p_seg_conf);
		else 			BSP_SEG_SET_SDA_L(p_seg_conf);
		BSP_SEG_DELAY_US();

		// 上升待从机读取数据
		BSP_SEG_SET_SCK_H(p_seg_conf);
		BSP_SEG_DELAY_US();
		BSP_SEG_SET_SCK_L(p_seg_conf);	// 结束

		// 位移到下一位
		byte >>= 1;
	}

	// 第9个SCK接收Ack
	BSP_SEG_SET_SCK_L(p_seg_conf);
	BSP_SEG_SET_SDA_H(p_seg_conf);	// 释放主线
	BSP_SEG_DELAY_US();

	BSP_SEG_SET_SCK_H(p_seg_conf);	// 主机在这里读取Ack
	BSP_SEG_DELAY_US();

	BSP_SEG_SET_SCK_L(p_seg_conf);	// 结束
	BSP_SEG_DELAY_US();
}
