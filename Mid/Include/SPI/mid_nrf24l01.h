#ifndef __MID_NRF24L01_H
#define __MID_NRF24L01_H
#include "main.h"
#include "SPI/bsp_spi.h"
#include "Common/mid_parce.h"

#define NRF_PLOAD_WIDTH   32    // 有效数据宽度：1~32 字节 (默认32)
#define NRF_ADDR_WIDTH    5     // 地址宽度：3~5 字节 (默认5)
#define NRF_CHANNEL       40    // RF频道：0~125，实际频率 2400 + RF_CH MHz (默认2440MHz)

#define NRF_TIMEOUT       50	// 超时配置50ms

typedef struct {
	// 硬件绑定
	BSP_SPI_GpioConf_t gpio_spi_conf;
	GPIO_TypeDef *gpio_ce;
	uint16_t gpio_pin_ce;
	GPIO_TypeDef *gpio_irq;
	uint16_t gpio_pin_irq;

	// 数据
	uint8_t  tx_flag;
	uint8_t *tx_data_ptr;
	uint8_t  irq_flag;
	uint8_t  rx_data[NRF_PLOAD_WIDTH];

	// 非阻塞时间记录
	uint32_t tick_last;

	// 回调函数表
	MID_Parce_CallbackTable callback_table;

} MID_NRF24L01_Handle_t;

/* ================= 对外接口函数 ================= */

void MID_NRF24L01_init(MID_NRF24L01_Handle_t *h_nrf24l01);
uint8_t MID_NRF24L01_registerCallback(MID_NRF24L01_Handle_t *h_nrf24l01, uint8_t cmd, Callback_Func callback);
void MID_NRF24L01_process(MID_NRF24L01_Handle_t *h_nrf24l01);
void MID_NRF24L01_transmitData(MID_NRF24L01_Handle_t *h_nrf24l01, uint8_t *tx_buf);
void MID_NRF24L01_remindGetedData_IT(MID_NRF24L01_Handle_t *h_nrf24l01);
void MID_NRF24L01_standbyI(MID_NRF24L01_Handle_t *h_nrf24l01);   // 待机模式I（省电，恢复快）
void MID_NRF24L01_turnOff(MID_NRF24L01_Handle_t *h_nrf24l01);    // 掉电模式（最省电，恢复慢）

#endif
