#ifndef __BSP_IIC_H
#define __BSP_IIC_H
#include "main.h"

typedef struct {
	GPIO_TypeDef *sda_gpiox;
	uint16_t sda_pin;
	GPIO_TypeDef *sck_gpiox;
	uint16_t sck_pin;
} BSP_IIC_GpioConf_t;

HAL_StatusTypeDef BSP_IIC_Soft_readByte(BSP_IIC_GpioConf_t *iic_config, uint8_t slave_addr, uint8_t reg_addr,
		uint16_t length, uint8_t *data);
HAL_StatusTypeDef BSP_IIC_Soft_writeByte(BSP_IIC_GpioConf_t *iic_config, uint8_t slave_addr, uint8_t reg_addr,
		uint16_t length, uint8_t const *data);
HAL_StatusTypeDef BSP_IIC_Soft_writeBit(BSP_IIC_GpioConf_t *iic_config, uint8_t slave_addr, uint8_t reg_addr,
		uint8_t bitNum, uint8_t data);
HAL_StatusTypeDef BSP_IIC_Soft_writeBits(BSP_IIC_GpioConf_t *iic_config, uint8_t slave_addr, uint8_t reg_addr,
		uint8_t bitStart, uint8_t length, uint8_t data);

#endif
