#include <Key/mid_key.h>

/*==========================对外开放==============================*/

// 初始化示例
MID_Key_Handle_t h_key_1 = {
		.key_gpio_conf = {
			.gpiox = GPIOB,
			.gpio_pin_x = GPIO_PIN_8,
		}
};

/*================================================新版==========================================================*/

/**
  * @brief  清除句柄中的记录数据，初始化句柄
  * @param  hkeyx: key的结构体，绑定被修改的key信息
  * @retval 无
  */
void MID_Key_init(MID_Key_Handle_t *hkeyx)
{
	hkeyx->key_state = MID_KEY_FREE;
	hkeyx->key_cnt = 0;
	hkeyx->start_tick = 0;
	hkeyx->end_tick = 0;
}

/* @brief 按下计次统计 + 按下时长统计 + 按下中状态提示（非阻塞）
 * @param hkeyx key的配置和状态句柄
 * @param p_cnt 计数变量的地址
 * @param dwon_state key什么状态表示按下
 * @param press_interval 前后两次按下的间隔(ms)（越小，连击就越需要快，不然会断开）
 * @param tip_interval 在按下状态时，给外界提示的间隔(ms)
 * @retval [0, 1] "0" 代表此次无效, "1" 为按下状态提示
 * 		   (1, max] 除0/1之外的数字就是按下时长
 */
uint32_t MID_Key_scan(MID_Key_Handle_t *hkeyx, uint8_t *p_cnt, uint8_t dwon_state,
		uint32_t press_interval, uint32_t tip_interval)
{
	uint8_t phys_state = BSP_GPIO_readPin(&hkeyx->key_gpio_conf);

	switch(hkeyx->key_state) {

	// ------ 1.空闲状态（只负责检测是否按下） --------------------------------------
	case MID_KEY_FREE:
		// 按下检测
		if(phys_state == dwon_state) {
			MID_Key_init(hkeyx);							// 清除上次次动作记录，初始化
			hkeyx->last_tick = BSP_Tick_getCurTick_32(); 	// 过滤之前更新计时值
			hkeyx->key_state = MID_KEY_FILTER;				// 切换到消抖过滤
		}
		break;

	// ------- 2.过滤消抖状态（检测到稳定的信号并判断归于哪个状态）-------------------
	case MID_KEY_FILTER:
		// 定时过滤
		if(BSP_Tick_getCurTick_32() - hkeyx->last_tick > 10)
		{
			// 过滤结果，确定稳定状态
			if(phys_state == dwon_state) {
				hkeyx->key_state = MID_KEY_DOWN;				// 切换到按下状态
				hkeyx->key_cnt++;								// 计数
				hkeyx->last_tick  = BSP_Tick_getCurTick_32(); 	// 更新提示计时
				hkeyx->start_tick = BSP_Tick_getCurTick_32();	// 记下切换时间
			} else {
				hkeyx->key_state = MID_KEY_WAITING;			// 切换到松开状态
				hkeyx->end_tick = BSP_Tick_getCurTick_32();	// 记下切换时间
			}
		}
		break;

	// ------- 3.按下状态（定时返回按下提示，检测是否有松开迹象）---------------------
	case MID_KEY_DOWN:
		// 间断返回按下提示
		if(BSP_Tick_getCurTick_32() - hkeyx->last_tick > tip_interval) {
			hkeyx->last_tick = BSP_Tick_getCurTick_32(); 	// 更新计时
			return 1;
		}

		// 松开检测
		if(phys_state != dwon_state) {
			hkeyx->last_tick = BSP_Tick_getCurTick_32(); 	// 过滤之前更新计时值
			hkeyx->key_state = MID_KEY_FILTER;				// 切换到消抖
		}
		break;

	// ------- 4.松开等待状态（超时检测，还检测是否有按下状态）------------------------
	case MID_KEY_WAITING:
		// 超时检测
		if(BSP_Tick_getCurTick_32() - hkeyx->end_tick > press_interval) {
			uint32_t diff = hkeyx->end_tick - hkeyx->start_tick;
			hkeyx->key_state = MID_KEY_FREE;	// 更新下一次
			(*p_cnt) += hkeyx->key_cnt;			// 最后才更新计数值
			return diff;						// 返回上次按下时长
		}

		// 按下检测
		if(phys_state == dwon_state) {
			hkeyx->last_tick = BSP_Tick_getCurTick_32(); 	// 过滤之前更新计时值
			hkeyx->key_state = MID_KEY_FILTER;				// 切换到消抖过滤
		}
	}

	return 0;
}



