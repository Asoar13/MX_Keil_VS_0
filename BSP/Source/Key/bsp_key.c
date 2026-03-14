#include <Key/bsp_key.h>

#define BSP_KEY_CONFIRM 1
#define BSP_KEY_CANCLE  0

// 无效条件为 非standard_state保持一个per_detect_time (确保为整10)
void BSP_Key_checkPinState_unblock(GPIO_TypeDef *gpiox, uint32_t gpio_pin_key, uint8_t *ptr_cnt, uint16_t detect_time, GPIO_PinState standard_state)
{
	// 循环计时
	for(uint16_t i = 0; i < detect_time / 10; i++) {

		// 检测按下
		if(HAL_GPIO_ReadPin(gpiox, gpio_pin_key) == standard_state) {
			(*ptr_cnt)++;
			break;
		}
		HAL_Delay(10);	// 计时累积
	}
}

// 无效条件为 非standard_state保持一个per_detect_time (确保为整10)
BSP_Key_StdState BSP_Key_checkPinState(GPIO_TypeDef *gpiox, uint32_t gpio_pin_key, uint8_t *ptr_cnt, uint16_t per_detect_time, GPIO_PinState standard_state)
{
	// 循环计时
	for(uint16_t i = 0; i < per_detect_time / 10; i++) {

		// 检测按下
		if(HAL_GPIO_ReadPin(gpiox, gpio_pin_key) == standard_state) {
			HAL_Delay(10);	// 按下消抖

			// 等待松开
			while(HAL_GPIO_ReadPin(gpiox, gpio_pin_key) == standard_state);
			HAL_Delay(10);	// 松开消抖

			(*ptr_cnt)++;
			return BSP_KEY_STANDARD;
		}

		HAL_Delay(10);	// 计时累积
	}

	return BSP_KEY_NO_STANDARD;
}

// 每隔一段时间检测是否松开，是则结束统计，否则统计次数加一 （达到上限也结束）
void BSP_Key_checkContinueState(GPIO_TypeDef *gpiox, uint32_t gpio_pin_key, uint16_t *ptr_time, GPIO_PinState standard_state)
{
	if(HAL_GPIO_ReadPin(gpiox, gpio_pin_key) == standard_state) {
		HAL_Delay(10);	// 按下消抖
		uint8_t end = 0;

		while(end == 0){

			// 松开 检查
			HAL_Delay(10);	// 延迟等待
			if(HAL_GPIO_ReadPin(gpiox, gpio_pin_key) != standard_state) {
				end = 1;
				break;
			}

			// 继续统计
			(*ptr_time) += 10;
		}
	}
}

// 返回最后一次按的时间
// 修改按下次数
// 有效返回值在下判断是否按下时更新

////
// 按下更新start_tick， 打断、关闭计时flag并重置计时， 返回无效;
// 松开更新end_tick，计算更新diff， 允许计时flag， 返回无效0;

// 计时， 若超时， 更新有效标志， 返回diff
////

