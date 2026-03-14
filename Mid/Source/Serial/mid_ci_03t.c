//#include "Serial/mid_ci_03t.h"
//#include <string.h>
//#include "usart.h"
//
///*------------------------------对外的句柄-------------------------------------*/
//
//MID_Serial_Handle_t h_ci_03t = {
//		// 硬件配置
//		.huartx = &huart1,
//
//		// 帧逻辑配置
//		._isHead = MID_CI_03T_isHead,
//		._isFinished = MID_CI_03T_isFinished,
//		._packData = MID_CI_03T_packData,
//
//		// 解析cmd配置
//		.pos_cmd = 1,
//		.pos_usefule_data = 1,
//		.unuseful_count = 2,
//};
//
///*============接收完整帧=============*/
//
//uint8_t MID_CI_03T_isHead(uint8_t byte)
//{
//	if(byte == 0xAA) return 1;
//	else			 return 0;
//}
//
////
//uint8_t MID_CI_03T_isFinished(uint8_t *p_data, uint16_t len)
//{
//	uint8_t expected_len = p_data[0];
//
//	if(expected_len +2 == len && p_data[len -1] == 0x55) return 2;
//	if(expected_len +2 == len) return 1;;
//	return 0;
//}
//
///*============发送完整帧=============*/
//
//// 帧头(1) + 长度(1) + 数据(len) + 帧尾(1)
//uint16_t MID_CI_03T_packData(uint8_t *p_data, uint16_t len)
//{
//	// 后移2位
//	memmove(p_data +2, p_data, len);
//
//	// 加头
//	p_data[0] = 0xAA;	p_data[1] = len;
//	len += 2;
//
//	// 加尾
//	p_data[len++] = 0x55;
//
//	return len;
//}
//
//
//
//
//
