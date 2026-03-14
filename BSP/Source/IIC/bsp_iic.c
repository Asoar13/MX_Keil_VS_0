#include "IIC/bsp_iic.h"

// 基于开漏输出(Open-Drain)模式的宏定义
#define IIC_SCL_H(iic_config)  HAL_GPIO_WritePin(iic_config->sck_gpiox, iic_config->sck_pin, GPIO_PIN_SET)
#define IIC_SCL_L(iic_config)  HAL_GPIO_WritePin(iic_config->sck_gpiox, iic_config->sck_pin, GPIO_PIN_RESET)
#define IIC_SDA_H(iic_config)  HAL_GPIO_WritePin(iic_config->sda_gpiox, iic_config->sda_pin, GPIO_PIN_SET)
#define IIC_SDA_L(iic_config)  HAL_GPIO_WritePin(iic_config->sda_gpiox, iic_config->sda_pin, GPIO_PIN_RESET)
#define IIC_SDA_READ(iic_config) HAL_GPIO_ReadPin(iic_config->sda_gpiox, iic_config->sda_pin)

// 实际只需要几微秒
static void IIC_Delay()
{
    uint32_t i = 35; // 延时，适配72MHz主频，这个速度最稳
    while(i--);
}

// SCK高期间，SDA高电平下拉
static void IIC_Start(BSP_IIC_GpioConf_t *iic_config)
{
    IIC_SDA_H(iic_config); IIC_SCL_H(iic_config); IIC_Delay();
    IIC_SDA_L(iic_config); IIC_Delay();
    IIC_SCL_L(iic_config);
}

// SCK高期间，SDA低电平上拉
static void IIC_Stop(BSP_IIC_GpioConf_t *iic_config)
{
    IIC_SCL_L(iic_config); IIC_SDA_L(iic_config); IIC_Delay();
    IIC_SCL_H(iic_config); IIC_SDA_H(iic_config); IIC_Delay();
}

// SDA松开，SCK高期间，读取SDA的Ack
static uint8_t IIC_Wait_Ack(BSP_IIC_GpioConf_t *iic_config)
{
    uint8_t errTime = 0;
    IIC_SDA_H(iic_config); IIC_Delay();
    IIC_SCL_H(iic_config); IIC_Delay();
    while(IIC_SDA_READ(iic_config)) {
        errTime++;
        if(errTime > 250) {
            IIC_Stop(iic_config);
            return 1; // 失败
        }
    }
    IIC_SCL_L(iic_config);
    return 0; // 成功
}

// SCK低期间，SDA高电平下拉
static void IIC_Ack(BSP_IIC_GpioConf_t *iic_config)
{
    IIC_SCL_L(iic_config); IIC_SDA_L(iic_config); IIC_Delay();
    IIC_SCL_H(iic_config); IIC_Delay();
    IIC_SCL_L(iic_config);
}

// SCK低期间，SDA低电平上拉
static void IIC_NAck(BSP_IIC_GpioConf_t *iic_config)
{
    IIC_SCL_L(iic_config); IIC_SDA_H(iic_config); IIC_Delay();
    IIC_SCL_H(iic_config); IIC_Delay();
    IIC_SCL_L(iic_config);
}


// SCK低期间，SDA变化
static void IIC_Send_Byte(BSP_IIC_GpioConf_t *iic_config, uint8_t txd)
{
    uint8_t t;
    IIC_SCL_L(iic_config);
    for(t = 0; t < 8; t++) {
        if((txd & 0x80) >> 7) IIC_SDA_H(iic_config);
        else IIC_SDA_L(iic_config);
        txd <<= 1;
        IIC_Delay();
        IIC_SCL_H(iic_config); IIC_Delay();
        IIC_SCL_L(iic_config); IIC_Delay();
    }
}

// SDA松开，SCK高期间，读取SDA发送NAck/Ack
static uint8_t IIC_Read_Byte(BSP_IIC_GpioConf_t *iic_config, unsigned char ack)
{
    uint8_t i, receive = 0;
    IIC_SDA_H(iic_config);
    for(i = 0; i < 8; i++) {
        IIC_SCL_L(iic_config); IIC_Delay();
        IIC_SCL_H(iic_config);
        receive <<= 1;
        if(IIC_SDA_READ(iic_config)) receive++;
        IIC_Delay();
    }
    if (!ack) IIC_NAck(iic_config);
    else IIC_Ack(iic_config);
    return receive;
}

/****************************************************对外******************************************************/

// slave_add需要原地址(7位)
// 按照iic_config配置，发送 slave_addr(写) => reg_addr => (data x length)
HAL_StatusTypeDef BSP_IIC_Soft_writeByte(BSP_IIC_GpioConf_t *iic_config, uint8_t slave_addr, uint8_t reg_addr,
		uint16_t length, uint8_t const *data)
{
    IIC_Start(iic_config);

    IIC_Send_Byte(iic_config, slave_addr << 1);
    if(IIC_Wait_Ack(iic_config)) { IIC_Stop(iic_config); return HAL_ERROR; }

    IIC_Send_Byte(iic_config, reg_addr);
    if(IIC_Wait_Ack(iic_config)) { IIC_Stop(iic_config); return HAL_ERROR; }

    for(uint16_t i = 0; i < length; i++) {
        IIC_Send_Byte(iic_config, data[i]);
        if(IIC_Wait_Ack(iic_config)) { IIC_Stop(iic_config); return HAL_ERROR; }
    }

    IIC_Stop(iic_config);
    return HAL_OK;
}

// slave_add需要原地址(7位)
// 按照iic_config配置，发送 slave_addr(写) => reg_addr => slave_addr(读), 读取(data x length)
HAL_StatusTypeDef BSP_IIC_Soft_readByte(BSP_IIC_GpioConf_t *iic_config, uint8_t slave_addr, uint8_t reg_addr,
		uint16_t length, uint8_t *data)
{
    IIC_Start(iic_config);

    IIC_Send_Byte(iic_config, slave_addr << 1);
    if(IIC_Wait_Ack(iic_config)) { IIC_Stop(iic_config); return HAL_ERROR; }

    IIC_Send_Byte(iic_config, reg_addr);
    if(IIC_Wait_Ack(iic_config)) { IIC_Stop(iic_config); return HAL_ERROR; }

    IIC_Start(iic_config);

    IIC_Send_Byte(iic_config, (slave_addr << 1) | 0x01); // 读方向
    if(IIC_Wait_Ack(iic_config)) { IIC_Stop(iic_config); return HAL_ERROR; }

    for(uint16_t i = 0; i < length; i++) {
        if(i == length - 1) data[i] = IIC_Read_Byte(iic_config, 0);
        else data[i] = IIC_Read_Byte(iic_config, 1);
    }

    IIC_Stop(iic_config);
    return HAL_OK;
}

// slave_add需要原地址(7位)
// 按照iic_config配置，先读取byte，再选择修改某一位，又写回去
HAL_StatusTypeDef BSP_IIC_Soft_writeBit(BSP_IIC_GpioConf_t *iic_config, uint8_t slave_addr, uint8_t reg_addr,
		uint8_t bitNum, uint8_t data)
{
    uint8_t tmp;
    BSP_IIC_Soft_readByte(iic_config, slave_addr, reg_addr, 1, &tmp);
    tmp = (data != 0) ? (tmp | (1 << bitNum)) : (tmp & ~(1 << bitNum));
    return BSP_IIC_Soft_writeByte(iic_config, slave_addr, reg_addr, 1, &tmp);
}

// slave_add需要原地址(7位)
// 按照iic_config配置，先读取byte，再选择修改从某处开始的某几位，又写回去
HAL_StatusTypeDef BSP_IIC_Soft_writeBits(BSP_IIC_GpioConf_t *iic_config, uint8_t slave_addr, uint8_t reg_addr,
		uint8_t bitStart, uint8_t length, uint8_t data)
{
    uint8_t tmp, dataShift;
    if (BSP_IIC_Soft_readByte(iic_config, slave_addr, reg_addr, 1, &tmp) == HAL_OK) {
        uint8_t mask = (((1 << length) - 1) << (bitStart - length + 1));
        dataShift = data << (bitStart - length + 1);
        tmp &= ~mask;
        tmp |= dataShift;
        return BSP_IIC_Soft_writeByte(iic_config, slave_addr, reg_addr, 1, &tmp);
    }
    return HAL_ERROR;
}
