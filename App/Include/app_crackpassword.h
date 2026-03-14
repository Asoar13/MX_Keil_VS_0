//#ifndef __APP_CRACKPASSWORD_H
//#define __APP_CRACKPASSWORD_H
//#include "main.h"
//#include "OLED/mid_oled.h"
//#include "Key/mid_key.h"
//#include "Encoder/mid_encoder.h"
//#include "tim.h"
//
//#define PW_LENGTH   7
//#define POSITION_Y  3
//#define START_GAP   8
//
//#define COUNT_S_X   10
//#define COUNT_S_Y   22
//#define COUNT_MS_X  66
//#define COUNT_MS_Y  22
//
//typedef enum {
//	APP_CrackPW_WAITING,
//	APP_CrackPW_VERIFYING,
//	APP_CrackPW_ERROR,
//	APP_CrackPW_PASS,
//} APP_CrackPW_VerifyResult_t;
//
//typedef struct {
//	// 密码
//	int32_t current_pw[PW_LENGTH];
//	int32_t target_pw[PW_LENGTH];
//	uint8_t password_len;
//	MID_EncoderAdjustItem_t adjusted_item_info;
//
//	// 密码验证结果状态
//	volatile APP_CrackPW_VerifyResult_t verify_result;
//	uint32_t result_start_tick;
//
//	// 光标
//	uint8_t cursor_index;
//	uint8_t cursor_last_index;
//
//	// 系统状态
//	uint32_t app_total_tick;
//	uint32_t app_last_tick;
//	uint32_t app_rest_tick;
//
//	// 硬件绑定
//	MID_Key_Handle_t hkey;
//	MID_EncoderHandle_Hard_t hencoder;
//} APP_CrackPW_Cxt_t;
//
//
//void APP_CrackPW_config(APP_CrackPW_Cxt_t *p_app_cpw_cxt,
//		GPIO_TypeDef *gpiox, uint32_t gpio_pin_key, TIM_HandleTypeDef *htimx);
//void APP_CrackPW_init(APP_CrackPW_Cxt_t *p_app_cpw_cxt);
//void APP_CrackPW_process(APP_CrackPW_Cxt_t *p_app_cpw_cxt);
//
//#endif
