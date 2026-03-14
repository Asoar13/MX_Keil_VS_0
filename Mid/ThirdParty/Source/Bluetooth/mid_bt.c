//#include "Bluetooth/mid_bt.h"
//#include "Bluetooth/bsp_bt.h"
//#include <string.h>
//#include "usart.h"
//
//static void _spliceFrame(MID_Bt_Handle_t *hbtx);
//static void _parseAndDispatch(MID_Bt_Handle_t *hbtx);
//
//static uint32_t mid_bt_handle_size = sizeof(MID_Bt_Handle_t);
//
//// 配置蓝牙（对应的USART）
//void MID_Bt_config(MID_Bt_Handle_t *hbtx, UART_HandleTypeDef *huartx)
//{
//	hbtx->huartx = huartx;
//}
//
//// 初始化数据
//void MID_Bt_init(MID_Bt_Handle_t *hbtx)
//{
//	// 初始化
//	UART_HandleTypeDef *huartx = hbtx->huartx;
//	memset(hbtx, 0, mid_bt_handle_size);
//	hbtx->huartx = huartx;
//	BSP_Bt_UART_RxCallback_IT(hbtx->huartx, &hbtx->rx_byte, 1);
//
//}
//
//// 接入硬件UART的回调函数
//void MID_Bt_UART_Callback(MID_Bt_Handle_t *hbtx)
//{
//	// 示意接收到
//	hbtx->rx_flag = 1;
//
//	// 开启下一次接收
//	BSP_Bt_UART_RxCallback_IT(hbtx->huartx, &hbtx->rx_byte, 1);
//}
//
//// 注册回调函数（0：失败，1；成功）
//uint8_t MID_Bt_registerCallback(MID_Bt_Handle_t *hbtx, uint8_t cmd, MID_Bt_CallBack_t callback)
//{
//	uint8_t i, count = hbtx->callback_count;
//
//	// 达到限制，不允许注册
//	if(count >= MID_BT_MAX_CB_COUNT) return 0;
//
//	// 遍历是否有重复cmd
//	for(i = 0; i < count; i++) {
//
//		// 相同的cmd就替换掉
//		if(hbtx->callback_table[i].cmd == cmd)
//			break;
//	}
//
//	// 找到相同的就替换，没有就加入新的
//	i = (i == count - 1) ? i : count;
//
//	// 替换i对应的cmd或者count对应的(空)的cmd 和callback
//	hbtx->callback_count++;
//	hbtx->callback_table[i].cmd = cmd;
//	hbtx->callback_table[i].callback = callback;
//
//	return 1;
//}
//
//// 处理蓝牙信号
//void MID_Bt_Process(MID_Bt_Handle_t *hbtx)
//{
//	if(hbtx->rx_flag) {
//
//		// 拼接此帧
//		_spliceFrame(hbtx);
//		hbtx->rx_flag = 0;	// 取消标志位
//	}
//}
//
//// 拼帧，得到指令
//static void _spliceFrame(MID_Bt_Handle_t *hbtx)
//{
//	uint8_t byte = hbtx->rx_byte;
//
//	switch(hbtx->splice_state) {
//
//	case MID_BT_WAIT_HEAD:
//		// 等到帧头，更换到等待长度状态
//		if(byte == MID_BT_HEAD)
//			hbtx->splice_state = MID_BT_WAIT_LENGTH;
//		break;
//
//	case MID_BT_WAIT_LENGTH:
//		// 等一个帧，记录此帧数字为预期的数据大小，更换到等待数据状态
//		hbtx->expected_datalen = byte;
//		hbtx->cur_datalen = 0;			// 重置cur_datalen
//		hbtx->splice_state = MID_BT_WAIT_DATA;
//		break;
//
//	case MID_BT_WAIT_DATA:
//		// 等几帧作为数据，如果已经存入的Byte到了预期大小，更换到等待结束尾状态
//		hbtx->data_buffer[hbtx->cur_datalen++] = byte;
//		if(hbtx->cur_datalen == hbtx->expected_datalen)
//			hbtx->splice_state = MID_BT_WAIT_TAIL;
//		break;
//
//	case MID_BT_WAIT_TAIL:
//		// 这帧如果是帧尾就开始解析数据，
//		// 不是则表明数据长度和内容不匹配，更换到等待开始头状态，丢弃此数据
//		if(byte == MID_BT_TAIL)
//			_parseAndDispatch(hbtx);
//		hbtx->splice_state = MID_BT_WAIT_HEAD;
//		break;
//	}
//}
//
//// 解析指令，调用对应回调函数
//static void _parseAndDispatch(MID_Bt_Handle_t *hbtx)
//{
//	// 第一个是指令对象，后面都是参数
//	uint8_t cmd = hbtx->data_buffer[0];
//	uint8_t *p_params = &(hbtx->data_buffer[1]);
//	uint8_t len = hbtx->cur_datalen - 1;
//
//	// 调用对应的回调函数
//	for(uint8_t i = 0; i < MID_BT_MAX_CB_COUNT; i++) {
//		if(hbtx->callback_table[i].cmd == cmd && hbtx->callback_table[i].callback != NULL) {
//			hbtx->callback_table[i].callback(p_params, len);
//			return;
//		}
//	}
//}
//
//
//
