//#include <Serial/mid_snr9816.h>
//#include <string.h>
//#include "usart.h"
//
///*----------------------------对外句柄--------------------------------*/
//
//MID_Serial_Handle_t h_snr9816 = {
//		// 硬件配置
//		.huartx = &huart1,
//
//		// 帧逻辑配置
//		._packData = MID_SNR9816_packData,
//
//		// 解析cmd配置
//};
//
///*=========================发送封装==========================*/
//
//// 帧头(1) + 长度(2) + 指令(2)+ 数据(len)
//uint16_t MID_SNR9816_packData(uint8_t *p_data, uint16_t len)
//{
//	// 后移5位
//	memmove(p_data +5, p_data, len);
//	len += 2; // 规定指令(2)也加入计算
//
//	// 加头
//	p_data[0] = 0xFD;
//	p_data[1] = (len >> 8) & 0xFF;	p_data[2] = len & 0xFF;
//	p_data[3] = 0x01;				p_data[4] = 0x01;
//	len += 3;
//
//	return len;
//}
