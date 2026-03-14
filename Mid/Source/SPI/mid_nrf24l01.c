#include "SPI/mid_nrf24l01.h"
#include "SPI/mid_nrf24l01_macro.h"
#include "SPI/bsp_spi.h"

#define NRF_CE_H(h_nrf24l01)  h_nrf24l01->gpio_ce->BSRR = h_nrf24l01->gpio_pin_ce
#define NRF_CE_L(h_nrf24l01)  h_nrf24l01->gpio_ce->BSRR = h_nrf24l01->gpio_pin_ce << 16

static uint8_t NRF_ADDRESS[NRF_ADDR_WIDTH] = {0x34, 0x43, 0x10, 0x10, 0x01};

static uint8_t _processTransmit(MID_NRF24L01_Handle_t *h_nrf24l01);
static uint8_t _processReceive(MID_NRF24L01_Handle_t *h_nrf24l01);
static void NRF_Delay_us(uint32_t us);
static void _startRxMode(MID_NRF24L01_Handle_t *h_nrf24l01);
static void _startTxMode(MID_NRF24L01_Handle_t *h_nrf24l01);


/*==========================对外开放==============================*/
//   (在此处初始化配置，在app层extern来使用，作为这些函数的第一个参数)
//MID_NRF24L01_Handle_t h_nrf = {
//	.gpio_spi_conf = {
//			.gpio_csn = GPIOA,
//			.gpio_pin_csn = GPIO_PIN_1,
//
//			.gpio_sck = GPIOA,
//			.gpio_pin_sck = GPIO_PIN_2,
//
//			.gpio_mosi = GPIOA,
//			.gpio_pin_mosi = GPIO_PIN_3,
//
//			.gpio_miso = GPIOA,
//			.gpio_pin_miso = GPIO_PIN_4,
//	},
//
//	.gpio_ce = GPIOA,
//	.gpio_pin_ce = GPIO_PIN_0,
//
//	.gpio_irq = GPIOA,
//	.gpio_pin_irq = GPIO_PIN_5,
//};

/**
 * @brief  初始化 NRF24L01模块
 * (在此模块的函数中应该第一个被调用)
 */
void MID_NRF24L01_init(MID_NRF24L01_Handle_t *h_nrf24l01)
{
	 /*-------------------配置状态---------------------------------------------*/
	h_nrf24l01->irq_flag = 0;
	h_nrf24l01->tx_flag = 0;
	h_nrf24l01->tick_last = 0;

    NRF_CE_L(h_nrf24l01); // 待机模式可配置

    /*-------------------配置地址（宽度、数值）---------------------------------*/

    // 设置地址宽度
    BSP_SPI_Soft_WriteReg(&h_nrf24l01->gpio_spi_conf, NRF_WRITE_REG + SETUP_AW, 0x03);  // 0x03代表5字节地址
    // 设置本机地址 (TX和RX地址设为一样，方便收到ACK)
    BSP_SPI_Soft_WriteBuffer(&h_nrf24l01->gpio_spi_conf, NRF_WRITE_REG + TX_ADDR, NRF_ADDRESS, NRF_ADDR_WIDTH);
    BSP_SPI_Soft_WriteBuffer(&h_nrf24l01->gpio_spi_conf, NRF_WRITE_REG + RX_ADDR_P0, NRF_ADDRESS, NRF_ADDR_WIDTH);

    /*-------------------配置发/收（物理通道、空中速率、功耗）-------------------*/

    // 配置通道0
    BSP_SPI_Soft_WriteReg(&h_nrf24l01->gpio_spi_conf, NRF_WRITE_REG + EN_RXADDR, 0x01);    			// 允许通道0接收
    BSP_SPI_Soft_WriteReg(&h_nrf24l01->gpio_spi_conf, NRF_WRITE_REG + RX_PW_P0, NRF_PLOAD_WIDTH); 	// 通道0有效数据宽度
    // 射频频率与速率配置
    BSP_SPI_Soft_WriteReg(&h_nrf24l01->gpio_spi_conf, NRF_WRITE_REG + RF_CH, NRF_CHANNEL); // 设置频道
    BSP_SPI_Soft_WriteReg(&h_nrf24l01->gpio_spi_conf, NRF_WRITE_REG + RF_SETUP, 0x0F);     // 2Mbps, 0dBm最大功率

    /*-------------------优化配置-----------------------------------------------*/

    // 设置自动重发与应答
    BSP_SPI_Soft_WriteReg(&h_nrf24l01->gpio_spi_conf, NRF_WRITE_REG + EN_AA, 0x01);       // 开启通道0自动应答
    BSP_SPI_Soft_WriteReg(&h_nrf24l01->gpio_spi_conf, NRF_WRITE_REG + SETUP_RETR, 0x1A);  // 0x1A: 等待500us(0001), 自动重发10次(1010)

    /*-------------------默认进入接收模式----------------------------------------*/
    _startRxMode(h_nrf24l01);
}

/**
 * @brief 映射cmd与回调函数，可以实现传输“cmd在第一位的数据”就调用“cmd对应函数”来处理此数据，会自动去除cmd部分
 *  (在此模块的函数中需要在Init之后调用)
 */
uint8_t MID_NRF24L01_registerCallback(MID_NRF24L01_Handle_t *h_nrf24l01, uint8_t cmd, Callback_Func callback)
{
	return MID_Parce_registerCallback(&h_nrf24l01->callback_table, cmd, callback);
}

/**
 * @brief  在有状态位时执行相应的处理函数
 *  (在此模块的函数中需要放在while中，以便轮询执行非阻塞处理)
 */
void MID_NRF24L01_process(MID_NRF24L01_Handle_t *h_nrf24l01)
{
    if(h_nrf24l01->tx_flag)
    	_processTransmit(h_nrf24l01);

    if(h_nrf24l01->irq_flag) {
    	_processReceive(h_nrf24l01);
    	h_nrf24l01->irq_flag = 0;
    }
}

/**
 * @brief 触发模块的中断标记位
 *  (在此模块的函数中需要在stm32中断回调函数中调用，配合实现模块产生中断时才触发处理的效果)
 * @param h_nrf24l01 句柄，含配置信息
 * @param rx_buf 接收数据的数组
 */
void MID_NRF24L01_remindGetedData_IT(MID_NRF24L01_Handle_t *h_nrf24l01)
{
	h_nrf24l01->irq_flag = 1;
}

/**
 * @brief 指定被发送的数组，并触发发送标记位
 *   (在此模块的函数中需要在init之后调用，之后会由while的处理函数来非阻塞发送)
 * @param h_nrf24l01 句柄，含配置信息
 * @param rx_buf 被发送数组
 */
void MID_NRF24L01_transmitData(MID_NRF24L01_Handle_t *h_nrf24l01, uint8_t *tx_buf)
{
	h_nrf24l01->tx_data_ptr = tx_buf;
	h_nrf24l01->tx_flag = 1;	// 切换到处理发送状态
}

/**
 * @brief  待机模式I (功耗22uA，寄存器值保留，恢复快)
 *   (在此模块的函数中需要init之后调用)
 * @param h_nrf24l01 句柄，含配置信息
 */
void MID_NRF24L01_standbyI(MID_NRF24L01_Handle_t *h_nrf24l01)
{
    NRF_CE_L(h_nrf24l01);
    // CONFIG 寄存器保持 PWR_UP=1，不工作时即为待机I
    BSP_SPI_Soft_WriteReg(&h_nrf24l01->gpio_spi_conf, NRF_WRITE_REG + NRF_CONFIG, 0x0E);
}

/**
 * @brief  掉电模式 (功耗900nA，寄存器值保留，恢复最慢)
 *   (在此模块的函数中需要init之后调用)
 * @param h_nrf24l01 句柄，含配置信息
 */
void MID_NRF24L01_turnOff(MID_NRF24L01_Handle_t *h_nrf24l01)
{
    NRF_CE_L(h_nrf24l01);
    // PWR_UP = 0
    BSP_SPI_Soft_WriteReg(&h_nrf24l01->gpio_spi_conf, NRF_WRITE_REG + NRF_CONFIG, 0x00);
}

/**
 * @brief  微秒级粗略延时 (用于给 CE 产生 10us 脉冲)
 */
static void NRF_Delay_us(uint32_t us)
{
    uint32_t count = us * 10; // 适配 72MHz/168MHz 的粗略延时，可按需修改
    while(count--);
}

/**
 * @brief  切换到接收模式 (RX Mode)
 * @param h_nrf24l01 句柄，含配置信息
 */
static void _startRxMode(MID_NRF24L01_Handle_t *h_nrf24l01)
{
    NRF_CE_L(h_nrf24l01); // 进入待机配置

    // 配置为接收模式 (PRIM_RX=1, PWR_UP=1, 开启2字节CRC, 开启中断)
    BSP_SPI_Soft_WriteReg(&h_nrf24l01->gpio_spi_conf, NRF_WRITE_REG + NRF_CONFIG, 0x0F);

    NRF_CE_H(h_nrf24l01); 	// 进入接收模式，开始监听空中信号
    NRF_Delay_us(130); 		// 从待机进入接收需要 130us 的建立时间
}

/**
 * @brief  切换到接发送模式 (TX Mode)
 * @param h_nrf24l01 句柄，含配置信息
 */
static void _startTxMode(MID_NRF24L01_Handle_t *h_nrf24l01)
{
    NRF_CE_L(h_nrf24l01); // 进入待机配置

    // 配置为发送模式 (PRIM_RX=0, PWR_UP=1)
    BSP_SPI_Soft_WriteReg(&h_nrf24l01->gpio_spi_conf, NRF_WRITE_REG + NRF_CONFIG, 0x0E);

    // 产生 >10us 的高电平脉冲，激发发送
    NRF_CE_H(h_nrf24l01);
    NRF_Delay_us(15);
    NRF_CE_L(h_nrf24l01);
}

/**
 * @brief 处理发送
 * @param h_nrf24l01 句柄，含配置信息
 * @retval 正常处理中 0； 超时无应答 1； 最大重发应答 2
 */
static uint8_t _processTransmit(MID_NRF24L01_Handle_t *h_nrf24l01)
{
	uint8_t status = BSP_SPI_Soft_ReadReg(&h_nrf24l01->gpio_spi_conf, NRF_STATUS); // 读取状态

    switch(h_nrf24l01->tx_flag) {

    /*-------------------------------发送数据 (第一次进入一次)------------------------------------*/
    case 1:
    	// 发送数据到 NRF24L01
		NRF_CE_L(h_nrf24l01); // 拉低CE，准备配置发送
		BSP_SPI_Soft_WriteBuffer(&h_nrf24l01->gpio_spi_conf, WR_TX_PLOAD, h_nrf24l01->tx_data_ptr, NRF_PLOAD_WIDTH); // 把数据写进 TX FIFO

		// 配置为发送模式
		_startTxMode(h_nrf24l01);

		// 更新时刻
		h_nrf24l01->tick_last = HAL_GetTick();
		h_nrf24l01->tx_flag = 2; // 切换为等待Ack状态
		return 0;

    /*-------------------------------等待应答 (中间多次进入)--------------------------===----------*/
    case 2:
		// 无反应 并且 未达到超时设定 直接返回0
		while((status & (TX_DS | MAX_RT)) == 0 && HAL_GetTick() - h_nrf24l01->tick_last < NRF_TIMEOUT) return 0;
		h_nrf24l01->tx_flag = 3;	// 切换为解析应答状态
		return 0;

    /*-------------------------------处理应答 (最后一次进入一次)------------------------------------*/
	// 解析应答(正常解析中 0； 超时无应答 1； 最大重发应答 2)
    case 3:
    	uint8_t state = 0;

		// 无反应 但是超时 回复 1
		if((status & (TX_DS | MAX_RT)) == 0) state = 1;
		else {
			// 有反应 但是最大重发 回复 2
			if(status & MAX_RT)
				state = 2;

			// 有反应 正常Ack 回复 0
			if(status & TX_DS)
				state = 0;

			BSP_SPI_Soft_WriteReg(&h_nrf24l01->gpio_spi_conf, FLUSH_TX, 0xFF); // 清除发不出去的数据，防止卡死
		}

		// 清除状态标志位 (写1清除)
		BSP_SPI_Soft_WriteReg(&h_nrf24l01->gpio_spi_conf, NRF_WRITE_REG + NRF_STATUS, TX_DS | MAX_RT);

		// 回到接收状态
		_startRxMode(h_nrf24l01);
		h_nrf24l01->tx_flag = 0; // 切换到发送完成状态
		return  state;
    }

    return 0;
}

/**
 * @brief  轮询检查是否收到数据
 * @param h_nrf24l01 句柄，含配置信息
 * @retval 1:收到数据  0:没收到数据
 */
static uint8_t _processReceive(MID_NRF24L01_Handle_t *h_nrf24l01)
{
	uint8_t status = BSP_SPI_Soft_ReadReg(&h_nrf24l01->gpio_spi_conf, NRF_STATUS); // 读取状态

	// 检查接收标志位否有效
    if (status & RX_DR)
    {
        // 从 RX FIFO 读取数据
        BSP_SPI_Soft_ReadBuffer(&h_nrf24l01->gpio_spi_conf, RD_RX_PLOAD, h_nrf24l01->rx_data, NRF_PLOAD_WIDTH);

        // 写1清除中断标志
        BSP_SPI_Soft_WriteReg(&h_nrf24l01->gpio_spi_conf, NRF_WRITE_REG + NRF_STATUS, RX_DR);

        // 调用回调
        MID_Parce_parseAndDispatch(&h_nrf24l01->callback_table, h_nrf24l01->rx_data[0],
        							h_nrf24l01->rx_data + 1, NRF_PLOAD_WIDTH - 1);
        return 1; // 成功收到
    }

    return 0; // 没收到
}
