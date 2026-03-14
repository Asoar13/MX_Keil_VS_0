#include "Time/bsp_tick.h"

uint32_t BSP_Tick_getCurTick_32(void)
{
   return HAL_GetTick();
}

void BSP_Tick_turnOff()
{
   HAL_SuspendTick();
}

void BSP_Tick_turnOn()
{
   HAL_ResumeTick();
}

