#include <Servo/bsp_servo.h>
#include <Servo/mid_servo.h>


void MID_Servo_config(MID_ServoHandle_t *p_servo_handle, TIM_HandleTypeDef *htimx, uint32_t TIM_CHANNEL_x)
{
	p_servo_handle->htimx = htimx;
	p_servo_handle->TIM_CHANNEL_x = TIM_CHANNEL_x;
}


void MID_Servo_init(MID_ServoHandle_t *p_servo_handle)
{
	BSP_Servo_init(p_servo_handle->htimx, p_servo_handle->TIM_CHANNEL_x);
}


//int32_t current_pos = 500; // 当前位置 (实际发给 PWM 的，用 float 为了丝滑)
//int32_t start_pos = 500;
//int32_t half_pos = 1500;
//int32_t end_pos = 2500;  // 目标位置 (用户设定的，比如编码器改这个值)
//uint32_t servo_last_tick = 0; // 上次更新的时间
//uint32_t gap = 10;
//int32_t step = 1;
//uint32_t common_edge = 2000 * 0.99f;
//uint32_t diff;
//
//void MID_Servo_oscillateProcess(MID_ServoHandle_t *p_servo_handle)
//{
//	uint32_t servo_now_tick = HAL_GetTick();
//	// 拦截在gap ms之内的进入（非阻塞delay）
//	if(servo_now_tick - servo_last_tick < gap) return;
//	servo_last_tick = servo_now_tick;	// 立刻更新
//
//	// 边界检测（是否需要掉向）
//	if(end_pos == current_pos) {
//		 start_pos = current_pos;
//			 if(end_pos == 2500) {
//			 end_pos = 500;
//		 }
//		 else if(end_pos == 500) {
//			 end_pos = 2500;
//		 }
//		 half_pos = (start_pos + end_pos) / 2;
//		 common_edge = abs(half_pos * 0.99f);
//		 step = (end_pos > current_pos) ? 1 : -1;
//	}
//
//	// 转动方式
//	diff = abs(half_pos - current_pos);
//	if(diff < common_edge) {	/// 近则变速跟上
//		current_pos += step * ((diff <= 100) ? 100 : diff);
//	}
//	else {	/// 边缘则对准归位
//		current_pos = end_pos;
//	}
//	BSP_Servo_setAngle_2500(p_servo_handle->htimx, p_servo_handle->TIM_CHANNEL_x, current_pos);
//}

