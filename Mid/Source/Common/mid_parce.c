#include "Common/mid_parce.h"

// cmd与某个特定函数映射
// 返回 0 成功； 1 覆盖； 2 溢出； 3 空指针
uint8_t MID_Parce_registerCallback(MID_Parce_CallbackTable *callback_table, uint8_t cmd, Callback_Func callback)
{
	// 空指针检查
	if(callback_table == NULL || callback == NULL) return 3;

	// 溢出检查
	if(callback_table->callback_cnt >= MID_PARCE_MAX_CB_COUNT) return 2;

	// 重复检查
	uint8_t cnt = callback_table->callback_cnt;
	for(uint8_t i = 0; i < cnt; i++) {
		if(callback_table->callback_table[i].cmd == cmd) {
			callback_table->callback_table[i].callback = callback;
			return 1;
		}
	}

	// 创建新映射
	callback_table->callback_table[cnt].cmd = cmd;
	callback_table->callback_table[cnt].callback = callback;
	callback_table->callback_cnt++;
	return 0;
}

// 识别指令，触发回调函数
// 返回 0 成功； 1 没有此cmd的函数； 2 空指针
uint8_t MID_Parce_parseAndDispatch(MID_Parce_CallbackTable *callback_table, uint8_t cmd, uint8_t *p_data, uint8_t len)
{
	// 空指针检查
	if(callback_table == NULL || p_data == NULL) return 2;

	uint8_t cnt = callback_table->callback_cnt;
	for(uint8_t i = 0; i < cnt; i++) {
		if(callback_table->callback_table[i].cmd == cmd && callback_table->callback_table[i].callback != NULL) {
			callback_table->callback_table[i].callback(p_data, len);
			return 0;
		}
	}
	return 1;
}
