#include "OLED/bsp_oled.h"
#include <OLED/mid_oled.h>

/*============================================硬件IIC===============================================*/

#ifdef HAL_I2C_MODULE_ENABLED

	// 硬件IIC
	void BSP_OLED_writeCmd_Hard_IIC(void *hw_handle, uint8_t *p_cmd, uint16_t Size)
	{
		HAL_I2C_Mem_Write((I2C_HandleTypeDef*)hw_handle, 0x78, 0x00, I2C_MEMADD_SIZE_8BIT, p_cmd, Size, 0x100);
	}

	void BSP_OLED_writeData_Hard_IIC(void *hw_handle, uint8_t *p_data, uint16_t Size)
	{
		HAL_I2C_Mem_Write((I2C_HandleTypeDef*)hw_handle, 0x78, 0x40, I2C_MEMADD_SIZE_8BIT, p_data, Size, 0x100);
	}

	void BSP_OLED_RESET_Hard_IIC(void *hw_handle)
	{}	// 空，IIC不需要

	MID_OLED_Handle_t h_oled_i2c = {
			.hw_handle = &hi2c2,

			._writeCmd = BSP_OLED_writeCmd_Hard_IIC,
			._writeData = BSP_OLED_writeData_Hard_IIC,
			._RESET = BSP_OLED_RESET_Hard_IIC,

			.show_buf = {{0}},
	};

#endif /* HAL_I2C_MODULE_ENABLED */

/*============================================软件IIC===============================================*/

#ifdef __BSP_IIC_H

	// 软件IIC
	void BSP_OLED_writeCmd_Soft_IIC(void *hw_handle, uint8_t *p_cmd, uint16_t Size)
	{
		BSP_IIC_Soft_writeByte((BSP_IIC_GpioConf_t*)hw_handle, 0x3C, 0x00, Size, p_cmd);
	}

	void BSP_OLED_writeData_Soft_IIC(void *hw_handle, uint8_t *p_data, uint16_t Size)
	{
		BSP_IIC_Soft_writeByte((BSP_IIC_GpioConf_t*)hw_handle, 0x3C, 0x40, Size, p_data);
	}

	void BSP_OLED_RESET_Soft_IIC(void *hw_handle)
	{}	// 空，IIC不需要


	// 先试试软件IIC
	BSP_IIC_GpioConf_t oled_iic_gpio_conf = {
			.sck_gpiox = GPIOB,
			.sck_pin = GPIO_PIN_10,
			.sda_gpiox = GPIOB,
			.sda_pin = GPIO_PIN_11,
	};

	MID_OLED_Handle_t h_oled_iic = {
			.hw_handle  = &oled_iic_gpio_conf,

			._writeCmd  = BSP_OLED_writeCmd_Soft_IIC,
			._writeData = BSP_OLED_writeData_Soft_IIC,
			._RESET = BSP_OLED_RESET_Soft_IIC,

			.show_buf = {{0}},
	};

#endif

/*============================================硬件SPI===============================================*/

#ifdef HAL_SPI_MODULE_ENABLED

	// 硬件IIC
	typedef struct {
		// SPI通用引脚配置
		SPI_HandleTypeDef *hspix;

		// 单独软件csn
		GPIO_TypeDef *gpio_csn;
		uint16_t gpio_pin_csn;

		// OLED-SPI专用引脚配置
		GPIO_TypeDef *gpio_dc;
		uint16_t gpio_pin_dc;
		GPIO_TypeDef *gpio_res;
		uint16_t gpio_pin_res;

	} BSP_SPI_Hard_Expand_GpioConf_t;

	void BSP_OLED_writeCmd_Hard_SPI(void *hw_handle, uint8_t *p_cmd, uint16_t Size)
	{
		BSP_SPI_Hard_Expand_GpioConf_t *conf = (BSP_SPI_Hard_Expand_GpioConf_t*)hw_handle;

		conf->gpio_csn->BSRR = conf->gpio_pin_csn << 16;
		conf->gpio_dc->BSRR = conf->gpio_pin_dc << 16;
		HAL_SPI_Transmit(conf->hspix, p_cmd, Size, 100);
		conf->gpio_csn->BSRR = conf->gpio_pin_csn;
	}

	void BSP_OLED_writeData_Hard_SPI(void *hw_handle, uint8_t *p_data, uint16_t Size)
	{
		BSP_SPI_Hard_Expand_GpioConf_t *conf = (BSP_SPI_Hard_Expand_GpioConf_t*)hw_handle;

		conf->gpio_csn->BSRR = conf->gpio_pin_csn << 16;
		conf->gpio_dc->BSRR = conf->gpio_pin_dc;
		HAL_SPI_Transmit(conf->hspix, p_data, Size, 100);
		conf->gpio_csn->BSRR = conf->gpio_pin_csn;
	}

	void BSP_OLED_RESET_Hard_SPI(void *hw_handle)
	{
		BSP_SPI_Hard_Expand_GpioConf_t *conf = (BSP_SPI_Hard_Expand_GpioConf_t*)hw_handle;

		conf->gpio_res->BSRR = conf->gpio_pin_res << 16;
		HAL_Delay(100);
		conf->gpio_res->BSRR = conf->gpio_pin_res;
		HAL_Delay(100);
	}


	// 试试硬件SPI
//	BSP_SPI_Hard_Expand_GpioConf_t oled_spi_gpio_conf_h = {
//
//			// SPI通用引脚配置
//			.hspix = &hspi1,
//
//			// 单独软件csn
//			.gpio_csn = GPIOA,
//			.gpio_pin_csn = GPIO_PIN_3,
//
//			// OLED-SPI专用引脚配置
//			.gpio_dc = GPIOA,
//			.gpio_pin_dc = GPIO_PIN_4,
//			.gpio_res = GPIOA,
//			.gpio_pin_res = GPIO_PIN_6,
//	};
//
//	MID_OLED_Handle_t h_oled_spi_h = {
//			.hw_handle = &oled_spi_gpio_conf_h,
//
//			._writeCmd = BSP_OLED_writeCmd_Hard_SPI,
//			._writeData = BSP_OLED_writeData_Hard_SPI,
//			._RESET = BSP_OLED_RESET_Hard_SPI,
//
//			.show_buf = {{0}},
//	};

#endif /* HAL_SPI_MODULE_ENABLED */

/*============================================软件SPI===============================================*/

#ifdef __BSP_SPI_H

	// 软件SPI
	typedef struct {
		// SPI通用引脚配置
		BSP_SPI_GpioConf_t spi_gpio_conf;

		// OLED-SPI专用引脚配置
		GPIO_TypeDef *gpio_dc;
		uint16_t gpio_pin_dc;
		GPIO_TypeDef *gpio_res;
		uint16_t gpio_pin_res;

	} BSP_SPI_Soft_Expand_GpioConf_t;

	void BSP_OLED_writeCmd_Soft_SPI(void *hw_handle, uint8_t *p_cmd, uint16_t Size)
	{
		BSP_SPI_Soft_Expand_GpioConf_t *oled_spi_conf = (BSP_SPI_Soft_Expand_GpioConf_t *)hw_handle;

		oled_spi_conf->gpio_dc->BSRR = oled_spi_conf->gpio_pin_dc << 16;
		BSP_SPI_Soft_WriteBytes(&oled_spi_conf->spi_gpio_conf, p_cmd, Size);
	}

	void BSP_OLED_writeData_Soft_SPI(void *hw_handle, uint8_t *p_data, uint16_t Size)
	{
		BSP_SPI_Soft_Expand_GpioConf_t *oled_spi_conf = (BSP_SPI_Soft_Expand_GpioConf_t *)hw_handle;

		oled_spi_conf->gpio_dc->BSRR = oled_spi_conf->gpio_pin_dc;
		BSP_SPI_Soft_WriteBytes(&oled_spi_conf->spi_gpio_conf, p_data, Size);
	}

	void BSP_OLED_RESET_Soft_SPI(void *hw_handle)
	{
		BSP_SPI_Soft_Expand_GpioConf_t *oled_spi_conf = (BSP_SPI_Soft_Expand_GpioConf_t *)hw_handle;

		oled_spi_conf->gpio_res->BSRR = oled_spi_conf->gpio_pin_res << 16;
		HAL_Delay(100);
		oled_spi_conf->gpio_res->BSRR = oled_spi_conf->gpio_pin_res;
		HAL_Delay(100);
	}


	// 试试软件SPI
	BSP_SPI_Soft_Expand_GpioConf_t oled_spi_gpio_conf_s = {

			// SPI通用引脚配置
			.spi_gpio_conf = {
					.gpio_csn = GPIOA,
					.gpio_pin_csn = GPIO_PIN_3,

					.gpio_sck = GPIOA,
					.gpio_pin_sck = GPIO_PIN_5,

					.gpio_mosi = GPIOA,
					.gpio_pin_mosi = GPIO_PIN_7,
			},

			// OLED-SPI专用引脚配置
			.gpio_dc = GPIOA,
			.gpio_pin_dc = GPIO_PIN_4,
			.gpio_res = GPIOA,
			.gpio_pin_res = GPIO_PIN_6,
	};

	MID_OLED_Handle_t h_oled_spi_s = {
			.hw_handle = &oled_spi_gpio_conf_s,

			._writeCmd = BSP_OLED_writeCmd_Soft_SPI,
			._writeData = BSP_OLED_writeData_Soft_SPI,
			._RESET = BSP_OLED_RESET_Soft_SPI,

			.show_buf = {{0}},
	};

#endif










