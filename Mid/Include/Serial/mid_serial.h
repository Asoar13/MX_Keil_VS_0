//
///*=================================在指定引脚 接收/发送 指定格式数据===========================================*/
//
//#ifndef __MID_SERIAL_H
//#define __MID_SERIAL_H
//#include "main.h"
//
//#define MID_SERIAL_MAX_DATA_LEN_RX 20
//#define MID_SERIAL_MAX_DATA_LEN_TX 40
//#define MID_SERIAL_MAX_CB_COUNT    10
//
//
//typedef void (*Callback_Func) (uint8_t *p_data, uint8_t data_len);
//typedef struct {
//	uint8_t cmd;
//	Callback_Func callback;
//} MID_Serial_CallbakConf_t;
//
//typedef struct {
//	// 硬件配置（手动初始化）
//	UART_HandleTypeDef *huartx;
//
//	// 通信协议逻辑配置（手动初始化）
//	uint8_t  (*_isHead) (uint8_t cur_byte);
//	uint8_t  (*_isFinished) (uint8_t *p_cur_data, uint16_t cur_len);
//	uint16_t (*_packData) (uint8_t *p_data, uint16_t len);
//
//	// 数据解析配置（手动初始化）
//	uint8_t pos_cmd;
//	uint8_t pos_usefule_data;
//	uint8_t unuseful_count;
//
//	// 处理状态（软件初始化）
//	uint8_t is_splicing;
//
//	// 数据状态（软件初始化）
//	uint8_t  rx_flag;
//	uint8_t  rx_byte;
//	uint8_t  data_buf[MID_SERIAL_MAX_DATA_LEN_RX];		// 除了帧头其他都存
//	uint16_t data_cur_len;
//
//	// 回调函数表（软件初始化）
//	MID_Serial_CallbakConf_t callback_table[MID_SERIAL_MAX_CB_COUNT];
//	uint8_t callback_cur_cnt;
//
//} MID_Serial_Handle_t;
//
//
//void MID_Serial_registerCallback(MID_Serial_Handle_t *h_serial, uint8_t cmd,Callback_Func callback);
//void MID_Serial_init(MID_Serial_Handle_t *h_serial);
//void MID_Serial_processRx(MID_Serial_Handle_t *h_serial);
//void MID_Serial_printfTx(MID_Serial_Handle_t *h_serial, const char *format, ...);
//void MID_Serial_transmitTx(MID_Serial_Handle_t *h_serial, uint8_t *p_data, uint8_t len);
//void MID_Serial_remindAndEnableIT(MID_Serial_Handle_t *h_serial);
//
//#endif
