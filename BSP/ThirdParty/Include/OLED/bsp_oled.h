
/***************************************OLED的通信选择*******************************************/
#ifndef __BSP_OLED_H
#define __BSP_OLED_H
#include "main.h"

#ifdef HAL_I2C_MODULE_ENABLED
	// 硬件宏定义保护，只有开启硬件IIC才引入 "i2c.h"
	#include "i2c.h"
#endif /* HAL_I2C_MODULE_ENABLED */

#ifdef HAL_SPI_MODULE_ENABLED
	// 硬件宏定义保护，只有开启硬件SPI才引入 "spi.h"
	#include "spi.h"
#endif /* HAL_SPI_MODULE_ENABLED */

// 引入软件IIC "IIC/bsp_iic.h"
#include "SPI/bsp_spi.h"
#include "IIC/bsp_iic.h"

#endif  /* __BSP_OLED_H */
