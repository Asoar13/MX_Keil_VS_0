#ifndef __BSP_SPI_H
#define __BSP_SPI_H
#include "main.h"

typedef struct {
	GPIO_TypeDef *gpio_sck;
	uint16_t gpio_pin_sck;

	GPIO_TypeDef *gpio_mosi;
	uint16_t gpio_pin_mosi;

	GPIO_TypeDef *gpio_miso;
	uint16_t gpio_pin_miso;

	GPIO_TypeDef *gpio_csn;
	uint16_t gpio_pin_csn;

} BSP_SPI_GpioConf_t;


void BSP_SPI_Soft_swapByte(BSP_SPI_GpioConf_t *spi_config, uint8_t reg_addr, uint8_t *swap_buf, uint16_t len);
void BSP_SPI_Soft_WriteReg(BSP_SPI_GpioConf_t *spi_config, uint8_t reg_addr, uint8_t data);
void BSP_SPI_Soft_WriteBuffer(BSP_SPI_GpioConf_t *spi_config, uint8_t reg_addr, uint8_t *swap_buf, uint16_t len);
uint8_t BSP_SPI_Soft_ReadReg(BSP_SPI_GpioConf_t *spi_config, uint8_t reg_addr);
void BSP_SPI_Soft_ReadBuffer(BSP_SPI_GpioConf_t *spi_config, uint8_t reg_addr, uint8_t *pBuf, uint16_t len);
void BSP_SPI_Soft_WriteBytes(BSP_SPI_GpioConf_t *spi_config, uint8_t *write_buf, uint16_t len);

#endif
