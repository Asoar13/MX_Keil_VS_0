#ifndef __MID_ADC_H
#define __MID_ADC_H
#include "main.h"
#include "ADC_DAC/bsp_adc.h"

#define MID_ADC_WIN_SIZE     	16
#define MID_ADC_CHANNEL_CNT  	4
#define MID_ADC_DEADBAND_SIZE	128

typedef struct {
	// 过滤
	uint16_t win_buf[MID_ADC_WIN_SIZE];	// 单个通道配置一个窗口
	uint8_t  win_idx;					// 记录位置
	uint32_t win_sum;					// 总和，减小计算量
	uint16_t deadband_last_value;		// 上次死区记录的合理值
	uint16_t valid_value;				// 此通道的最终值

} MID_ADCChannel_Handle_t;

typedef struct {

	// 硬件绑定
	void *h_adc; // (手动配置)-需要在初始化结构体时配置

	// ADC缓冲
	uint16_t adc_buf_raw[MID_ADC_CHANNEL_CNT];	// (软件自动)-函数自动处理数值
	uint8_t  adc_duf_len; 						// (参数配置)-作为函数参数接收

	// 通道组
	MID_ADCChannel_Handle_t channel_table[MID_ADC_CHANNEL_CNT]; // (软件自动)-函数自动处理数值

	// 更新转换值标志
	uint8_t update_flag; // (软件自动)-转换完成由函数自动更新

} MID_ADC_Handle_t;	// 一个ADC句柄将拥有多个通道


void MID_ADC_init(MID_ADC_Handle_t* hadcx);
uint32_t MID_ADC_getValue(MID_ADC_Handle_t* hadcx, uint16_t range, uint32_t timeout);

void MID_ADC_initAutoConver_DMA(MID_ADC_Handle_t *h_adc, uint8_t buf_len);
void MID_ADC_remindUpdate_IRQ(MID_ADC_Handle_t *h_adc);
void MID_ADC_process(MID_ADC_Handle_t *h_adc, uint8_t select_filter, uint8_t select_deadband);
uint16_t MID_ADC_getADCValue(MID_ADC_Handle_t *h_adc, uint8_t rank_x, uint16_t range);

#endif
