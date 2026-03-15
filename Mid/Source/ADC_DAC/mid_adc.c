#include "ADC_DAC/mid_adc.h"
#include <string.h>

#define MID_ADC_MAX	4095

static void MID_ADC_filterADC_DMA(MID_ADC_Handle_t *h_adc, uint8_t select_filter, uint8_t select_deadband);

/**
  * @brief 	初始化ADC（不用DMA）
  * @param  h_adc: adc的句柄，绑定被修改的adc信息
  * @retval 无
  */
void MID_ADC_init(MID_ADC_Handle_t* h_adc)
{
	ADC_HandleTypeDef *hadcx = (ADC_HandleTypeDef*)h_adc->h_adc;
	BSP_ADC_init(hadcx);
}

/**
  * @brief 	读取ADC通道的值，并映射到range范围之内（不用DMA）
  * @param  h_adc: adc的句柄，绑定被修改的adc信息
  * @param  range: 将4095映射到range之内
  * @param  timeout: 允许的读取时间(ms)(阻塞式)
  * @retval 无
  */
uint32_t MID_ADC_getValue(MID_ADC_Handle_t* h_adc, uint16_t range, uint32_t timeout)
{
	ADC_HandleTypeDef *hadcx = (ADC_HandleTypeDef*)h_adc->h_adc;
	return range * BSP_ADC_getADCValue(hadcx, timeout) / MID_ADC_MAX;
}

/**
  * @brief  处于main函数初始区域的函数，用于注明ADC通道的数量，初始化ADC句柄（与DMA联动时使用）
  * @param  h_adc: adc的句柄，绑定被修改的adc信息
  * @param  buf_len: ADC开启的（或者被需要的）通道的数量
  * @retval 无
  */
void MID_ADC_initAutoConver_DMA(MID_ADC_Handle_t *h_adc, uint8_t buf_len)
{
	if(!h_adc) return;	// 空检查

	void *hard_ptr = h_adc->h_adc;
	memset(h_adc->adc_buf_raw, 0, sizeof(h_adc->adc_buf_raw));		// 详细清除
	memset(h_adc->channel_table, 0, sizeof(h_adc->channel_table));	// 详细清除
	h_adc->h_adc = hard_ptr;

	// 句柄初始化
	buf_len = (buf_len > MID_ADC_CHANNEL_CNT) ? MID_ADC_CHANNEL_CNT: buf_len;	// 范围检查
	h_adc->adc_duf_len = buf_len;

	// 硬件初始化
	BSP_ADC_initAutoConver_DMA(h_adc->h_adc, (uint32_t*)h_adc->adc_buf_raw, buf_len); // 自动的硬件需要最后才初始化触发
}

/**
  * @brief  处于中断函数里的函数，用于提示ADC转换完成，示意MID_ADC_process处理（与DMA联动时使用）
  * @param  h_adc: adc的句柄，绑定被修改的adc信息
  * @retval 无
  */
void MID_ADC_remindUpdate_IRQ(MID_ADC_Handle_t *h_adc)
{
	h_adc->update_flag = 1;
}

/**
  * @brief  处于while循环中的函数，用于刷新ADC句柄的状态和处理ADC值（与DMA联动时使用）
  * @param  h_adc: adc的句柄，绑定被修改的adc信息
  * @param  select_filter: 决定是否使用平均滤波
  * @param  select_deadband: 决定是否使用死区（回滞滤波）
  * @retval 无
  */
void MID_ADC_process(MID_ADC_Handle_t *h_adc, uint8_t select_filter, uint8_t select_deadband)
{
	if(!h_adc->update_flag) return;
	h_adc->update_flag = 0;

	MID_ADC_filterADC_DMA(h_adc, select_filter, select_deadband);
}

/**
  * @brief  读取并将普通的ADC映射到一个范围
  * @param  h_adc: adc的句柄，绑定被修改的adc信息
  * @param  rank_x: ADC的通道序号
  * @param  range: 返回值的映射（4095 -> range）
  * @retval 16位数据
  */
uint16_t MID_ADC_getADCValue(MID_ADC_Handle_t *h_adc, uint8_t rank_x, uint16_t range)
{
	// 检查
	if(rank_x <= 0 || !h_adc) return 0;

	return h_adc->channel_table[rank_x - 1].valid_value * range / MID_ADC_MAX;
}

/**
  * @brief  被MID_ADC_process函数调用，进行滤波
  * @param  h_adc: adc的句柄，绑定被修改的adc信息
  * @param  select_filter: 决定是否使用平均滤波
  * @param  select_deadband: 决定是否使用死区（回滞滤波）
  * @retval 无
  */
static void MID_ADC_filterADC_DMA(MID_ADC_Handle_t *h_adc, uint8_t select_filter, uint8_t select_deadband)
{
	if(!h_adc) return;	// 空检查

	for(uint8_t i = 0; i < h_adc->adc_duf_len; i++) {
		MID_ADCChannel_Handle_t* channelx = &h_adc->channel_table[i];	// 取地址简化操作
		uint16_t new_value = h_adc->adc_buf_raw[i];

		//	滑动过滤，减干扰
		if(select_filter) {
			// 先更新总和
			channelx->win_sum += new_value;
			channelx->win_sum -= channelx->win_buf[channelx->win_idx];

			// 溢出检查( +100扩大缓冲)
			channelx->win_sum = (channelx->win_sum > MID_ADC_MAX * MID_ADC_WIN_SIZE + 100) ? 0: channelx->win_sum;

			// 再更新窗口记录
			channelx->win_buf[channelx->win_idx] = new_value;

			// 平均滤波
			channelx->valid_value = channelx->win_sum / MID_ADC_WIN_SIZE;

			// 滑动
			channelx->win_idx = (channelx->win_idx +1) % MID_ADC_WIN_SIZE;	// 取模滑动
		} else {

			channelx->valid_value = new_value;	// 为下面计算差值做准备
		}

		// 死区过滤，加稳定
		if(select_deadband) {
			// 死区步长限制
			uint16_t last = channelx->deadband_last_value, cur = channelx->valid_value;
			uint16_t dx = (last > cur) ? last - cur: cur - last;

			if(dx >= MID_ADC_DEADBAND_SIZE)
				channelx->deadband_last_value = cur;
			else
				channelx->valid_value = channelx->deadband_last_value;
		}
	}
}


