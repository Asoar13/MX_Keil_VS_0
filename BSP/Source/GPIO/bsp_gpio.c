#include "GPIO/bsp_gpio.h"

/**
  * @brief  按照指定参数配置gpio-pin的模式（中断则自动进入）
  * @param  p_gpio_conf：gpio的结构体，绑定被修改的gpio-pin信息
  * @param  pin_mode：类型很多，参考BSP_PinMode_t定义
  * @note   模式比较多，暂时不全 
  * @retval 无
  */
void BSP_GPIO_setMode(BSP_GpioConf_t *p_gpio_conf, BSP_PinMode_t pin_mode)
{ 
    GPIO_InitTypeDef gpio_init_struct;
    gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
    gpio_init_struct.Pin  = p_gpio_conf->gpio_pin_x;
    
    // 模式匹配
    switch(pin_mode) {
        case BSP_PIN_8_RAISING_EXIT_GROUP2_2_2:
        gpio_init_struct.Mode = GPIO_MODE_IT_RISING;        // 上升触发
        gpio_init_struct.Pull = GPIO_PULLDOWN;              // 下拉默认
        HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_2); // 优先组2
        HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 2);           // 抢占等级2 次等级等级2 
	    HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);                   // 中断线8属于9—5
        break;

        case BSP_PIN_IN_PULLDOWN:
        gpio_init_struct.Mode = GPIO_MODE_INPUT;    // 输入模式
        gpio_init_struct.Pull = GPIO_PULLDOWN;      // 默认下拉
        break;
    }

    // 启用配置
    GPIO_TypeDef *GPIOx= (GPIO_TypeDef *)p_gpio_conf->gpiox;
    HAL_GPIO_Init(GPIOx, &gpio_init_struct);
}

/**
  * @brief  专门负责退出中断的函数
  * @param  p_gpio_conf：gpio的结构体，绑定被修改的gpio-pin信息
  * @param  pin_mode：类型很多，参考BSP_PinMode_t定义
  * @retval 无
  */
void BSP_GPIO_disableEXIT(BSP_GpioConf_t *p_gpio_conf, BSP_PinMode_t pin_mode)
{
    switch(pin_mode) {
        case BSP_PIN_8_RAISING_EXIT_GROUP2_2_2:
        HAL_NVIC_DisableIRQ(EXTI9_5_IRQn);
    }
}

void BSP_GPIO_togglePin(BSP_GpioConf_t *p_gpio_conf)
{
    GPIO_TypeDef *GPIOx = (GPIO_TypeDef*)p_gpio_conf->gpiox;
    HAL_GPIO_TogglePin(GPIOx, p_gpio_conf->gpio_pin_x);
}

void BSP_GPIO_resetPin(BSP_GpioConf_t *p_gpio_conf)
{
    GPIO_TypeDef *GPIOx = (GPIO_TypeDef*)p_gpio_conf->gpiox;
    GPIOx->BSRR = p_gpio_conf->gpio_pin_x << 16;
}

void BSP_GPIO_setPin(BSP_GpioConf_t *p_gpio_conf)
{
    GPIO_TypeDef *GPIOx = (GPIO_TypeDef*)p_gpio_conf->gpiox;
    GPIOx->BSRR = p_gpio_conf->gpio_pin_x;
}

uint8_t BSP_GPIO_readPin(BSP_GpioConf_t *p_gpio_conf)
{
    GPIO_TypeDef *GPIOx = (GPIO_TypeDef*)p_gpio_conf->gpiox;
    return (GPIOx->IDR & p_gpio_conf->gpio_pin_x) ? 1 : 0;
}

/* -------------------------------------------GPIO中断函数----------------------------------------------- */

#include "Key/mid_key.h"
extern MID_Key_Handle_t h_key_1;
void EXTI9_5_IRQHandler()
{
    // APP_AdjustableFans的Key配置的GPIO中断
	HAL_GPIO_EXTI_IRQHandler(h_key_1.key_gpio_conf.gpio_pin_x);
}

