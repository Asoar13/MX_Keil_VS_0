#ifndef __BSP_COMMON_H
#define __BSP_COMMON_H
#include "main.h"

typedef struct {
	GPIO_TypeDef  *gpiox;
	uint32_t gpio_pin_x;
}BSP_GpioTypeDef;

#define SET   1
#define RESET 0

#endif
