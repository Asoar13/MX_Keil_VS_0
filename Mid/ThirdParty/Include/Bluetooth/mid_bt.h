//#ifndef __MID_BT_H
//#define __MID_BT_H
//#include "main.h"
//#define MID_BT_MAX_CB_COUNT  5
//#define MID_BT_MAX_DATA_LEN  5
//#define MID_BT_HEAD  0xAA
//#define MID_BT_TAIL  0x55
//
//typedef enum {
//	MID_BT_WAIT_HEAD,
//	MID_BT_WAIT_LENGTH,
//	MID_BT_WAIT_DATA,
//	MID_BT_WAIT_TAIL,
//} MID_Bt_SpliceState_t;
//
//// 回调函数类型
//typedef void (*MID_Bt_CallBack_t)(uint8_t *p_params, uint8_t len);
//
//typedef struct {
//	uint8_t cmd;
//	MID_Bt_CallBack_t callback;
//} MID_Bt_CbConf_t;
//
//typedef struct {
//	// -----硬件配置------
//	UART_HandleTypeDef *huartx;
//
//	// -----回调表--------
//	MID_Bt_CbConf_t callback_table[MID_BT_MAX_CB_COUNT];
//	uint8_t callback_count;
//
//	// -----UART接收------
//	uint8_t rx_byte;
//	uint8_t rx_flag;
//
//	// -----拼帧状态------
//	MID_Bt_SpliceState_t splice_state;
//	uint8_t data_buffer[MID_BT_MAX_DATA_LEN];
//	uint8_t expected_datalen;
//	uint8_t cur_datalen;
//
//} MID_Bt_Handle_t;
//
//// 配置蓝牙（对应的USART）
//void MID_Bt_config(MID_Bt_Handle_t *hbtx, UART_HandleTypeDef *huartx);
//void MID_Bt_init(MID_Bt_Handle_t *hbtx);
//void MID_Bt_UART_Callback(MID_Bt_Handle_t *hbtx);
//uint8_t MID_Bt_registerCallback(MID_Bt_Handle_t *hbtx, uint8_t cmd, MID_Bt_CallBack_t callback);
//void MID_Bt_Process(MID_Bt_Handle_t *hbtx);
//
//#endif
