#include "SPI/bsp_spi.h"

#define SPI_SCK_H(spi_config)		spi_config->gpio_sck->BSRR = spi_config->gpio_pin_sck
#define SPI_CSN_H(spi_config)		spi_config->gpio_csn->BSRR = spi_config->gpio_pin_csn
#define SPI_MOSI_H(spi_config)		spi_config->gpio_mosi->BSRR = spi_config->gpio_pin_mosi
#define SPI_SCK_L(spi_config)		spi_config->gpio_sck->BSRR = spi_config->gpio_pin_sck << 16
#define SPI_CSN_L(spi_config)		spi_config->gpio_csn->BSRR = spi_config->gpio_pin_csn << 16
#define SPI_MOSI_L(spi_config)		spi_config->gpio_mosi->BSRR = spi_config->gpio_pin_mosi	<< 16
#define SPI_MISO_READ(spi_config)	((spi_config->gpio_miso->IDR & spi_config->gpio_pin_miso) ? 1 : 0)

// 写出之后会交换数据（Byte会改变）
static void _swapByte(BSP_SPI_GpioConf_t *spi_config, uint8_t *Byte)
{
	for(uint8_t j = 0; j < 8; j++) {

		// Mode 0 空闲时为低电平，可以发送
		if((*Byte) & 0x80) SPI_MOSI_H(spi_config);
		else 			   SPI_MOSI_L(spi_config);
		(*Byte) <<= 1;

		SPI_SCK_H(spi_config);
		(*Byte) |= SPI_MISO_READ(spi_config);
		SPI_SCK_L(spi_config);	// 低电平结束（Mode 0）
	}
}

// 仅写数据（Byte不会改变）
static void _writeByte(BSP_SPI_GpioConf_t *spi_config, uint8_t *Byte)
{
	for(uint8_t j = 0; j < 8; j++) {

		// Mode 0 空闲时为低电平，可以发送
		if((*Byte) & 0x80) SPI_MOSI_H(spi_config);
		else 			   SPI_MOSI_L(spi_config);
		(*Byte) <<= 1;

		SPI_SCK_H(spi_config);
		SPI_SCK_L(spi_config);	// 低电平结束（Mode 0）
	}
}

void BSP_SPI_Soft_swapByte(BSP_SPI_GpioConf_t *spi_config, uint8_t reg_addr, uint8_t *swap_buf, uint16_t len)
{
	// 开始
	SPI_SCK_L(spi_config);	// 低电平进入（Mode 0）
	SPI_CSN_L(spi_config);

	// 发送cmd或者address
	_swapByte(spi_config, &reg_addr);

	// 交换len个数据
	for(uint16_t i = 0; i < len; i++) {

		// 交换一个数据
		_swapByte(spi_config, &swap_buf[i]);
	}

	// 结束
	SPI_CSN_H(spi_config);
}

void BSP_SPI_Soft_WriteReg(BSP_SPI_GpioConf_t *spi_config, uint8_t reg_addr, uint8_t data)
{
	// 开始
	SPI_SCK_L(spi_config);	// 低电平进入（Mode 0）
	SPI_CSN_L(spi_config);

	// 发送cmd或者address
	_writeByte(spi_config, &reg_addr);

	// 交换一个数据
	_writeByte(spi_config, &data);

	// 结束
	SPI_CSN_H(spi_config);
}

void BSP_SPI_Soft_WriteBuffer(BSP_SPI_GpioConf_t *spi_config, uint8_t reg_addr, uint8_t *swap_buf, uint16_t len)
{
	// 开始
	SPI_SCK_L(spi_config);	// 低电平进入（Mode 0）
	SPI_CSN_L(spi_config);

	// 发送cmd或者address
	_writeByte(spi_config, &reg_addr);

	// 写len个数据
	for(uint16_t i = 0; i < len; i++) {

		// 写数据
		uint8_t tmp = swap_buf[i];
		_writeByte(spi_config, &tmp);
	}

	// 结束
	SPI_CSN_H(spi_config);
}

uint8_t BSP_SPI_Soft_ReadReg(BSP_SPI_GpioConf_t *spi_config, uint8_t reg_addr)
{
	// 开始
	SPI_SCK_L(spi_config);	// 低电平进入（Mode 0）
	SPI_CSN_L(spi_config);

	// 发送cmd或者address
	_swapByte(spi_config, &reg_addr);

	// 交换一个数据
	uint8_t tep = 0xFF;
	_swapByte(spi_config, &tep);

	// 结束
	SPI_CSN_H(spi_config);

	return tep;
}

void BSP_SPI_Soft_ReadBuffer(BSP_SPI_GpioConf_t *spi_config, uint8_t reg_addr, uint8_t *pBuf, uint16_t len)
{
    // 开始
    SPI_SCK_L(spi_config);
    SPI_CSN_L(spi_config);

    // 1. 发送命令 (例如 0x61 R_RX_PAYLOAD)
    _swapByte(spi_config, &reg_addr);

    // 2. 循环读取数据
    for (uint16_t i = 0; i < len; i++) {
        pBuf[i] = 0xFF; // 准备 NOP
        _swapByte(spi_config, &pBuf[i]); // 发送 0xFF，接收到的数据存回 pBuf[i]
    }

    // 结束
    SPI_CSN_H(spi_config);
}

void BSP_SPI_Soft_WriteBytes(BSP_SPI_GpioConf_t *spi_config, uint8_t *write_buf, uint16_t len)
{
	// 开始
	SPI_SCK_L(spi_config);	// 低电平进入（Mode 0）
	SPI_CSN_L(spi_config);

	// 写len个数据
	for(uint16_t i = 0; i < len; i++) {

		// 写一个数据
		_writeByte(spi_config, &write_buf[i]);
	}

	// 结束
	SPI_CSN_H(spi_config);
}

BSP_SPI_GpioConf_t nrf_spi = {
		.gpio_csn = GPIOA,
		.gpio_pin_csn = GPIO_PIN_1,

		.gpio_sck = GPIOA,
		.gpio_pin_sck = GPIO_PIN_2,

		.gpio_mosi = GPIOA,
		.gpio_pin_mosi = GPIO_PIN_3,

		.gpio_miso = GPIOA,
		.gpio_pin_miso = GPIO_PIN_4,
};


