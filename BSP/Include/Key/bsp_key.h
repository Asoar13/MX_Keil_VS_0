#ifndef __BSP_KEY_H
#define __BSP_KEY_H
#include "main.h"
//#include "bsp_common.h"

/****************************************宏*******************************************/
#define BSP_KEY_ON  			1
#define BSP_KEY_OFF 			0
#define BSP_KEY_DEFAULT_DELAY 	300

typedef enum {
	BSP_KEY_NO_STANDARD = 0,
	BSP_KEY_STANDARD,
} BSP_Key_StdState;

typedef struct {
	GPIO_TypeDef *gpiox;
	uint32_t gpio_pin_key;

} BSP_Key_GpioConf_t;

void BSP_Key_checkPinState_unblock(GPIO_TypeDef *gpiox, uint32_t gpio_pin_key, uint8_t *ptr_cnt, uint16_t detect_time, GPIO_PinState standard_state);
BSP_Key_StdState BSP_Key_checkPinState(GPIO_TypeDef *gpiox, uint32_t gpio_pin_key, uint8_t *ptr_cnt, uint16_t per_detect_time, GPIO_PinState standard_state);
void BSP_Key_checkContinueState(GPIO_TypeDef *gpiox, uint32_t gpio_pin_key, uint16_t *ptr_time, GPIO_PinState standard_state);

#endif
