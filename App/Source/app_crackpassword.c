//#include "app_crackpassword.h"
//#include <string.h>
//#include <Tool/bsp_tool.h>
//
//static uint8_t cursor_position_x[] = {0, 16, 32, 48, 64, 80, 96};
//static void _drawIU(APP_CrackPW_Cxt_t *p_app_cpw_cxt);
//static void _detectKey(APP_CrackPW_Cxt_t *p_app_cpw_cxt);
//static void _cursorUpdateEvent(APP_CrackPW_Cxt_t *p_app_cpw_cxt);
//static void _detectEncoder(APP_CrackPW_Cxt_t *p_app_cpw_cxt);
//static void _verifyProcess(APP_CrackPW_Cxt_t *p_app_cpw_cxt);
//static void _verifyEvent(APP_CrackPW_Cxt_t *p_app_cpw_cxt);
//static void _countdownProcess(APP_CrackPW_Cxt_t *p_app_cpw_cxt);
//static void _deadlineEvent(APP_CrackPW_Cxt_t *p_app_cpw_cxt);
//
//
//void APP_CrackPW_config(APP_CrackPW_Cxt_t *p_app_cpw_cxt,
//		GPIO_TypeDef *gpiox, uint32_t gpio_pin_key, TIM_HandleTypeDef *htimx)
//{
//	// 清除所有数据
//	memset(p_app_cpw_cxt, 0, sizeof(APP_CrackPW_Cxt_t));
//
//	MID_Key_pinConfig(&p_app_cpw_cxt->hkey, gpiox, gpio_pin_key);
//	MID_Encoder_config(&p_app_cpw_cxt->hencoder, htimx);
//}
//
//void APP_CrackPW_init(APP_CrackPW_Cxt_t *p_app_cpw_cxt)
//{
//	// 密码设置
//	p_app_cpw_cxt->password_len = PW_LENGTH;
//	int32_t default_pw[PW_LENGTH] =  {5, 1, 1, 0, 2, 8, 0};
//	memcpy(p_app_cpw_cxt->target_pw, default_pw, sizeof(default_pw));
//	// 初始输入密码current_pw默认 00000
//
//	// 被修改对象的设置
//	p_app_cpw_cxt->adjusted_item_info.p_variable = p_app_cpw_cxt->current_pw; // 绑定到第一位
//	p_app_cpw_cxt->adjusted_item_info.min = 0;
//	p_app_cpw_cxt->adjusted_item_info.step = 1;
//	p_app_cpw_cxt->adjusted_item_info.max = 9;
//
//	// 光标默认都在 0
//
//	// 系统状态设置
//	p_app_cpw_cxt->app_last_tick = HAL_GetTick();
//	p_app_cpw_cxt->app_total_tick = 120 * 1000;
//	p_app_cpw_cxt->app_rest_tick = p_app_cpw_cxt->app_total_tick;
//
//	MID_Encoder_init(&p_app_cpw_cxt->hencoder);
//
//	// 触发更新事件
//	_cursorUpdateEvent(p_app_cpw_cxt);
//}
//
//void APP_CrackPW_process(APP_CrackPW_Cxt_t *p_app_cpw_cxt)
//{
//	// 处理特定状态的信息
//	_detectEncoder(p_app_cpw_cxt); // 完成
//	_detectKey(p_app_cpw_cxt);	 // 完成
//	_verifyProcess(p_app_cpw_cxt); // 完成
//	_countdownProcess(p_app_cpw_cxt);
//
//	// 显示全局信息
//	_drawIU(p_app_cpw_cxt); // 半
//}
//
//// 显示该有信息
//static void _drawIU(APP_CrackPW_Cxt_t *p_app_cpw_cxt)
//{
//	// 显示current_pw
//	uint8_t len = p_app_cpw_cxt->password_len;
//	for(uint8_t i = 0; i < len; i++) {
//		MID_OLED_showInt(START_GAP + i * 16, POSITION_Y, p_app_cpw_cxt->current_pw[i], 1, 0, 16);
//	}
//
//	// 显示倒计时
//	uint32_t h, m, s, ms = p_app_cpw_cxt->app_rest_tick;
//	BSP_Tool_Time_msIntoTime(&h, &m, &s, ms);
//	MID_OLED_showInt(COUNT_S_X,      COUNT_S_Y,  m,  2, 0, 8);
//	MID_OLED_showInt(COUNT_S_X + 17, COUNT_S_Y,  s,  2, 0, 8);
//	MID_OLED_showInt(COUNT_MS_X,     COUNT_MS_Y, ms, 7, 0, 8);
//
//	// 显示最终结果 (_countdownProcess() -> _deadlineEvent()已经处理)
//	// 显示光标标记（_detectKey() -> _cursorUpdateEvent()已经处理）
//	// 显示密码验证结果 (_verifyProcess() -> _verifyEvent()已经处理）
//
//	// 刷新OLED
//	MID_OLED_refreshScreen();
//}
//
//// 实时检测按键状态
//static void _detectKey(APP_CrackPW_Cxt_t *p_app_cpw_cxt)
//{
//	uint32_t time = MID_Key_checkContinue(&p_app_cpw_cxt->hkey, GPIO_PIN_SET);
//
//	// 无效拦截
//	if(time == 0) return;
//
//	if(time < 700) {	// 短按更新光标
//		// 光标下标边界检测
//		uint8_t cur_cursor = p_app_cpw_cxt->cursor_index + 1;
//		if(cur_cursor >= p_app_cpw_cxt->password_len) cur_cursor = 0;
//
//		// 更新，并触发更新事件
//		p_app_cpw_cxt->cursor_index = cur_cursor;
//		_cursorUpdateEvent(p_app_cpw_cxt);
//		p_app_cpw_cxt->cursor_last_index = cur_cursor;
//
//	} else {			// 长按转换验证状态
//		p_app_cpw_cxt->verify_result = APP_CrackPW_VERIFYING; // 此状态会自动修改
//	}
//}
//
//static void _cursorUpdateEvent(APP_CrackPW_Cxt_t *p_app_cpw_cxt)
//{
//	uint8_t last_cursor = p_app_cpw_cxt->cursor_last_index;
//	uint8_t cur_cursor = p_app_cpw_cxt->cursor_index;
//
//	// 光标符号擦除与更新
//	MID_OLED_clearSome(START_GAP + cursor_position_x[last_cursor]- 3, POSITION_Y - 1, 8 + 6, 16 + 2);
//	MID_OLED_drawSquareFrame(START_GAP + cursor_position_x[cur_cursor]- 3, POSITION_Y - 1, 8 + 6, 16 + 2);
//
//	// 编码器绑定修改
//	p_app_cpw_cxt->adjusted_item_info.p_variable = p_app_cpw_cxt->current_pw + cur_cursor;
//
//	// 提示的擦除和更新
//    BSP_Tool_Rand_initSeed();
//  	MID_OLED_clearSome(0, 32, 128, 32);
//    char expression[128 / 6 + 1];
//	BSP_Tool_Rand_getCombineExpre(p_app_cpw_cxt->target_pw[cur_cursor], 4, expression); // 获取算式
//	uint8_t show_x = 128 / 2 - strlen(expression) * 8 / 2;	// 计算中心显示对应的x坐标
//	MID_OLED_showString_ASCII(show_x, 40, expression, 16);
//}
//
//// 实时检测编码器修改
//static void _detectEncoder(APP_CrackPW_Cxt_t *p_app_cpw_cxt)
//{
//	MID_Encoder_process(&p_app_cpw_cxt->hencoder, &p_app_cpw_cxt->adjusted_item_info);
//}
//
//// 间断倒计时
//static void _verifyProcess(APP_CrackPW_Cxt_t *p_app_cpw_cxt)
//{
//	// 等待拦截
//	if(p_app_cpw_cxt->verify_result == APP_CrackPW_WAITING) return;
//
//	// 中间时段拦截（第一次result_start_tick为0，通过）
//	uint32_t now_tick = HAL_GetTick();
//	if(now_tick - p_app_cpw_cxt->result_start_tick < 1500) return;
//	p_app_cpw_cxt->result_start_tick = now_tick;
//
//	// 按照result决定处理
//	_verifyEvent(p_app_cpw_cxt);
//}
//
//static void _verifyEvent(APP_CrackPW_Cxt_t *p_app_cpw_cxt)
//{
//	// 先清除显示区域
//	MID_OLED_clearSome(0, 32, 128, 32);
//
//	// 是否已经验证
//	if(p_app_cpw_cxt->verify_result == APP_CrackPW_VERIFYING) {
//
//		// 未验证则验证
//		if(memcmp(p_app_cpw_cxt->current_pw, p_app_cpw_cxt->target_pw,
//				  p_app_cpw_cxt->password_len * sizeof(int32_t)) == 0) {
//			MID_OLED_showString_UTF8(12, 32, "恭喜全对!", 24);
//			p_app_cpw_cxt->verify_result = APP_CrackPW_PASS;
//		} else {
//			MID_OLED_showString_UTF8(25, 32, "错了!!!", 24);
//			p_app_cpw_cxt->verify_result = APP_CrackPW_ERROR;
//		}
//	} else {
//
//		// 验证完成的就求换状态
//		p_app_cpw_cxt->verify_result = APP_CrackPW_WAITING;
//		p_app_cpw_cxt->result_start_tick = 0;
//	}
//}
//
//static void _countdownProcess(APP_CrackPW_Cxt_t *p_app_cpw_cxt)
//{
//	// 没有设置就不进行
//	uint32_t rest = p_app_cpw_cxt->app_rest_tick;
//	if(rest == 0) return;
//
//	uint32_t now_tick = HAL_GetTick();
//	rest -= (now_tick - p_app_cpw_cxt->app_last_tick); // 时间间隔
//	p_app_cpw_cxt->app_last_tick = now_tick;
//	p_app_cpw_cxt->app_rest_tick = rest;
//
//	// rest为0.05s时触发截止事件
//	if(rest <= 50 || rest > p_app_cpw_cxt->app_total_tick) {
//		p_app_cpw_cxt->app_rest_tick = 0;
//		_deadlineEvent(p_app_cpw_cxt);
//	}
//}
//
//static void _deadlineEvent(APP_CrackPW_Cxt_t *p_app_cpw_cxt)
//{
//	// 验证结果
//	p_app_cpw_cxt->verify_result = APP_CrackPW_VERIFYING; // 请求验证
//	_verifyEvent(p_app_cpw_cxt); // 验证状态
//
//	// 显示最终结果
//	if(p_app_cpw_cxt->verify_result == APP_CrackPW_PASS) {
//		MID_OLED_showString_UTF8(12, 32, "恭喜全对!", 24);
//	} else {
//		MID_OLED_showString_UTF8(10, 32, "时间到!!", 24);
//	}
//	_drawIU(p_app_cpw_cxt);
//	while(1);
//}
//
//
//
//
//
//
