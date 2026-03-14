//#include "Serial/mid_dfplayermini.h"
//#include <string.h>
//#include "usart.h"
//
///*------------------------------对外的句柄-------------------------------------*/
//MID_Serial_Handle_t h_dfplayer_mini = {
//		// 硬件配置
//		.huartx = &huart1,
//
//		// 帧逻辑配置
//		._packData = MID_DFPlayer_mini_packData,
//
//		// 解析cmd配置
//};
//
//
//static uint16_t _getCheckTowsCmop(uint8_t *star_p, uint8_t len)
//{
//	uint16_t orig_chk = 0;
//	for(uint8_t i = 0; i < len; i++) {
//		orig_chk += star_p[i];
//	}
//
//	return 1 + ~orig_chk;
//}
//
///*============接收完整帧=============*/
//
////uint8_t MID_DFPlayer_mini_isHead(uint8_t byte)
////{
////
////}
////
////uint8_t MID_DFPlayer_mini_isFinished(uint8_t *p_data, uint16_t len)
////{
////
////}
//
///*============发送完整帧=============*/
//
//// 帧头(1) + 固定数据(2) + 数据(len) + 校验字节(2) + 帧尾(1)
//uint16_t MID_DFPlayer_mini_packData(uint8_t *p_data, uint16_t len)
//{
//	// 后移3位
//	memmove(p_data +3, p_data, len);
//
//	// 添加帧头和固定数据（帧头 + 版本号 + 长度）
//	p_data[0] = 0x7E; p_data[1] = 0xFF; p_data[2] = 0x06;
//	len += 3;
//
//	// 计算并添加校验字节
//	uint16_t tows_chk = _getCheckTowsCmop(p_data +1, 6);
//	p_data[len++] = (tows_chk >> 8) & 0xFF;
//	p_data[len++] = tows_chk & 0xFF;
//
//	// 添加帧尾
//	p_data[len++] = 0xEF;
//
//	return len;
//}
//
//
//
