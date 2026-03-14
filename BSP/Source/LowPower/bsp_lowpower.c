#include "LowPower/bsp_lowpower.h"

void SystemClock_Config(void);	// main.c的函数

void BSP_LowPower_sleep()
{
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
}

void BSP_LowPower_stop()
{
	HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
	SystemClock_Config();
}
