#ifndef __MID_KEY_H
#define __MID_KEY_H
#include "Key/bsp_key.h"

typedef enum {
	MID_KEY_FREE = 0,
	MID_KEY_FILTER,
	MID_KEY_DOWN,
	MID_KEY_WAITING,
} MID_Key_State;

typedef struct {
	// 配置
	GPIO_TypeDef *gpiox;	// (手动配置) - 初始化结构体时需要手动配置，作为句柄的控制对象
	uint32_t gpio_pin_key;	// (手动配置) - 初始化结构体时需要手动配置，作为句柄的控制对象

	// 状态、记录
	MID_Key_State key_state; // (软件自动) - 函数自动控制状态机
	uint8_t key_cnt;

	// 时间(反应长按时长)
	uint32_t last_tick;  // (软件自动) - 处理函数自动处理，用于处理按下时长
	uint32_t start_tick; // (软件自动) - 初始化函数自动配置，用于处理按下时长
	uint32_t end_tick;   // (软件自动) - 初始化函数自动配置，用于处理按下时长
}MID_Key_Handle_t;

void MID_Key_pinConfig(MID_Key_Handle_t *key_gpio_conf,
		GPIO_TypeDef *gpiox, uint32_t gpio_pin_key);
uint8_t MID_Key_onceCheckPinState_unblock(MID_Key_Handle_t *key_gpio_conf, uint16_t detect_time, GPIO_PinState standard_state);
uint8_t MID_Key_onceCheckPinState(MID_Key_Handle_t *key_gpio_conf, uint16_t detect_time, GPIO_PinState standard_state);
uint8_t MID_Key_mutipleCheckPinState(MID_Key_Handle_t *key_gpio_conf, uint16_t per_detect_time, GPIO_PinState standard_state);
uint32_t MID_Key_checkContinue(MID_Key_Handle_t *key_gpio_conf, GPIO_PinState standard_state);
uint32_t MID_Key_CheckPressTime(MID_Key_Handle_t *key_config, uint8_t *p_cnt, GPIO_PinState standard_state, uint32_t interval);

/*================================================新版==========================================================*/

void MID_Key_init(MID_Key_Handle_t *hkeyx);
uint32_t MID_Key_Scan(MID_Key_Handle_t *hkeyx, uint8_t *p_cnt, GPIO_PinState dwon_state,
		uint32_t press_interval, uint32_t tip_interval);

#endif
