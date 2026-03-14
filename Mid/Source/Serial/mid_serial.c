//#include "Serial/mid_serial.h"	// mid层的通用抽象，将其他UART设备与UART外设联系
//#include <Serial/bsp_serial.h>
//#include <string.h>
//#include <stdio.h>
//#include <stdarg.h>
//
//static void _spliceByteIntoData(MID_Serial_Handle_t *h_serial);
//static void _parseAndDispatch(MID_Serial_Handle_t *h_serial);
//
//
//void MID_Serial_registerCallback(MID_Serial_Handle_t *h_serial, uint8_t cmd,Callback_Func callback)
//{
//	if(h_serial->callback_cur_cnt >= MID_SERIAL_MAX_CB_COUNT) return;
//
//	for(uint8_t i = 0; i < h_serial->callback_cur_cnt; i++) {
//		if(h_serial->callback_table[i].cmd == cmd) {
//			h_serial->callback_table[i].callback = callback;
//			return;
//		}
//	}
//
//	h_serial->callback_table[h_serial->callback_cur_cnt].cmd = cmd;
//	h_serial->callback_table[h_serial->callback_cur_cnt].callback = callback;
//	h_serial->callback_cur_cnt++;
//}
//
//// 挂起第一次
//void MID_Serial_init(MID_Serial_Handle_t *h_serial)
//{
//	memset(h_serial->data_buf, 0, MID_SERIAL_MAX_DATA_LEN_RX);
//	h_serial->data_cur_len = 0;
//	h_serial->rx_byte = 0;
//	h_serial->callback_cur_cnt = 0;
//	h_serial->is_splicing = 0;
//
//	// 挂起第一次
//	BSP_Serial_receiveByte_IT(h_serial->huartx, &h_serial->rx_byte);
//	h_serial->rx_flag = 0;
//}
//
//// 识别并且拼接一帧数据，并且回调对应函数
//void MID_Serial_processRx(MID_Serial_Handle_t *h_serial)
//{
//	if(!h_serial->rx_flag) return;
//
//	// 帧拼接
//	_spliceByteIntoData(h_serial);
//
//	// 取消置位
//	h_serial->rx_flag = 0;
//}
//
//// 格式化数据发送，自动添加帧头帧尾（用于可变参数）
//void MID_Serial_printfTx(MID_Serial_Handle_t *h_serial, const char *format, ...)
//{
//	uint16_t len;
//	uint8_t data_buf[MID_SERIAL_MAX_DATA_LEN_TX];
//
//	// 可变参数转换
//	va_list ags;
//	va_start(ags, format);
//	len = vsnprintf((char *)data_buf, MID_SERIAL_MAX_DATA_LEN_RX, format, ags);
//	va_end(ags);
//
//	// 帧格式包装
//	len = h_serial->_packData(data_buf, len);
//
//	// 发送
//	BSP_Serial_transmitByte(h_serial->huartx, data_buf, len);
//}
//
//// 普通的数据发送，自动添加帧头帧尾
//void MID_Serial_transmitTx(MID_Serial_Handle_t *h_serial, uint8_t *p_data, uint8_t len)
//{
//	uint8_t data_buf[MID_SERIAL_MAX_DATA_LEN_TX];
//
//	// 数据转移
//	memcpy(data_buf, p_data, len);
//
//	// 帧格式包装
//	len = h_serial->_packData(data_buf, len);
//
//	// 发送
//	BSP_Serial_transmitByte(h_serial->huartx, data_buf, len);
//}
//
//// 挂起下一次
//void MID_Serial_remindAndEnableIT(MID_Serial_Handle_t *h_serial)
//{
//	h_serial->rx_flag = 1;
//	BSP_Serial_receiveByte_IT(h_serial->huartx, &h_serial->rx_byte);
//}
//
//
//static void _spliceByteIntoData(MID_Serial_Handle_t *h_serial)
//{
//	uint8_t byte = h_serial->rx_byte;
//
//	// 等待帧头处理
//	if(h_serial->is_splicing == 0) {
//		// 是帧头就转为处理状态
//		if(h_serial->_isHead(byte)) {
//			h_serial->is_splicing = 1;	// 状态切换
//			h_serial->data_cur_len = 0;
//		}
//
//	} else if(h_serial->is_splicing == 1) {
//		// 超出范围默认为错误数据，重新开始
//		if(h_serial->data_cur_len >= MID_SERIAL_MAX_DATA_LEN_RX) {
//			h_serial->is_splicing = 0;
//			return;
//		}
//
//		// 拼接数据
//		h_serial->data_buf[h_serial->data_cur_len++] = byte;
//
//		// 到此结束则转回完成状态，并且调用对应函数
//		uint8_t is_finished = h_serial->_isFinished(h_serial->data_buf, h_serial->data_cur_len);
//
//		// 只要不是0就下一轮
//		if(is_finished) {
//			h_serial->is_splicing = 0;	// 状态切换
//			if(is_finished == 2)	// 没达到2是异常结束
//				_parseAndDispatch(h_serial);
//		}
//	}
//}
//
//static void _parseAndDispatch(MID_Serial_Handle_t *h_serial)
//{
//	// 始终取某一位作为cmd，负责与回调函数对应
//	uint8_t cmd = h_serial->data_buf[h_serial->pos_cmd];
//
//	for(uint8_t i = 0; i < h_serial->callback_cur_cnt; i++) {
//		if(h_serial->callback_table[i].cmd == cmd && h_serial->callback_table[i].callback != NULL) {
//
//			// 偏移到有效数据位置上，长度相应也减少
//			h_serial->callback_table[i].callback(h_serial->data_buf + h_serial->pos_usefule_data,
//												 h_serial->data_cur_len - h_serial->unuseful_count);
//		}
//	}
//}
