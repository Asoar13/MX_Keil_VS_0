#include <Encoder/mid_encoder.h>
#include <Encoder/bsp_encoder.h>
#include <Key/bsp_key.h>

/*==============================初始化数值===============================*/

/**
  * @brief 初始化硬件编码器
  * @param encoder_handle: 绑定了硬件的句柄，是本文件里对外的数据
  * @retval None
  */
void MID_Encoder_init(MID_EncoderHandle_Hard_t *encoder_handle)
{
	BSP_Encoder_startCount(encoder_handle->htimx, TIM_CHANNEL_ALL);

	encoder_handle->last_count = BSP_Encoder_getCurCount(encoder_handle->htimx);
	encoder_handle->count = 0;
	encoder_handle->step = 0;
}

/*==============================刷新信息============================*/

/**
  * @brief 获取当前编码器的状态，并和上一次状态对比，以此更新旋转步距和方向
  * @param encoder_handle: 绑定了硬件的句柄，是本文件里对外的数据
  * @retval None
  */
static void MID_Encoder_refresh(MID_EncoderHandle_Hard_t *encoder_handle)
{
	// 更新count 和 gap
	encoder_handle->count = (int16_t)__HAL_TIM_GetCounter(encoder_handle->htimx);
	int16_t gap = encoder_handle->count - encoder_handle->last_count;

	// 计次拦截（count 与 last_count相距不足 4会被拦截）
	if(gap > -4 && gap < 4) {
		encoder_handle->step = 0;
		return;
	}

	// 记录合理值
	int16_t step = gap / 4; // 步数取整（忽略余数干扰——未记满一转）
	encoder_handle->step = step;
	encoder_handle->last_count += step * 4; // 增量取整（不计余数，始终把未到位的一次忽略，确保下一次到位时会step +1）
}

/**
  * @brief 根据编码器句柄修改已知数据信息
  * @param encoder_handle: 绑定了硬件的句柄，是本文件里对外的数据
  * @param item_info: 绑定了变量的结构体，需要在app层手动初始化
  * @retval None
  */
void MID_Encoder_process(MID_EncoderHandle_Hard_t *encoder_handle, MID_EncoderAdjustItem_t *item_info)
{
	// 更新
	MID_Encoder_refresh(encoder_handle);

	// 静止判断，拦截进入下面代码
	int16_t step = encoder_handle->step;
	if(step == 0) return;

	int32_t *p_varible = item_info->p_variable;
	*(p_varible) +=  item_info->step * step;

	// 范围判断
	int32_t max = item_info->max, min = item_info->min;
	if(*(p_varible) > max) *(p_varible) = max;
	if(*(p_varible) < min) *(p_varible) = min;
}
