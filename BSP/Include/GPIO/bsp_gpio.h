#ifndef __BSP_GPIO_H
#define __BSP_GPIO_H
#include "main.h"

typedef struct {
    void    *gpiox;
    uint32_t gpio_pin_x;
} BSP_GpioConf_t;

typedef enum {
    BSP_PIN_8_RAISING_EXIT_GROUP2_2_2,
    BSP_PIN_OUT_PP_PULLDOWN,
    BSP_PIN_IN_PULLDOWN,
} BSP_PinMode_t;

void BSP_GPIO_setMode(BSP_GpioConf_t *p_gpio_conf, BSP_PinMode_t pin_mode);
void BSP_GPIO_disableEXIT(BSP_GpioConf_t *p_gpio_conf, BSP_PinMode_t pin_mode);
void BSP_GPIO_togglePin(BSP_GpioConf_t *p_gpio_conf);
void BSP_GPIO_resetPin(BSP_GpioConf_t *p_gpio_conf);
void BSP_GPIO_setPin(BSP_GpioConf_t *p_gpio_conf);
uint8_t BSP_GPIO_readPin(BSP_GpioConf_t *p_gpio_conf);


#endif 

