#include <Encoder/bsp_encoder.h>
#include <Encoder/mid_encoder.h>
#include <tim.h>

/*--------------------------------------对外句柄--------------------------------------------------*/
//MID_EncoderHandle_Hard_t h_encoder = {
//		.htimx = &htim3,
//};

/*===============================回应上层需求====================================*/
/**
  * @brief 回应上层，开始检查编码器的动作
  * @param htimx: 绑定了硬件的句柄，是本文件里对外的数据
  * @retval 状态
  */
uint8_t BSP_Encoder_startCount(TIM_HandleTypeDef *htimx, uint32_t Channel)
{
	return HAL_TIM_Encoder_Start(htimx, Channel);
}

/**
  * @brief 为上层汇报当前编码器已走距离
  * @param htimx: 绑定了硬件的句柄，是本文件里对外的数据
  * @retval None
  */
uint32_t BSP_Encoder_getCurCount(TIM_HandleTypeDef *htimx)
{
	return __HAL_TIM_GetCounter(htimx);
}
