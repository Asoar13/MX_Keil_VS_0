#ifndef __BSP_SEG_H
#define __BSP_SEG_H
#include "main.h"

typedef struct {
	void *gpio_sda;
	void *gpio_sck;
	uint32_t gpio_pin_sda;
	uint32_t gpio_pin_sck;
} BSP_Seg_GpioConf_t;

void BSP_Seg_start(BSP_Seg_GpioConf_t *p_seg_conf);
void BSP_Seg_stop(BSP_Seg_GpioConf_t *p_seg_conf);
void BSP_Seg_transmitByte(BSP_Seg_GpioConf_t *p_seg_conf, uint8_t byte);

#endif
