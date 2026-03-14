#include "Time/bsp_tick.h"

uint32_t BSP_Tick_getCurTick_32(void)
{
   return HAL_GetTick();
}

