#include "Time/bsp_tick.h"

/**
* @brief  读取当前的32位tick
* @param  无
* @retval 32位tick值
*/
uint32_t BSP_Tick_getCurTick_32(void)
{
   return HAL_GetTick();
}

/**
* @brief  暂停tick计时
* @param  无
* @retval 无
*/
void BSP_Tick_turnOff()
{
   HAL_SuspendTick();
}

/**
* @brief  继续tick计时
* @param  无
* @retval 无
*/
void BSP_Tick_turnOn()
{
   HAL_ResumeTick();
}

