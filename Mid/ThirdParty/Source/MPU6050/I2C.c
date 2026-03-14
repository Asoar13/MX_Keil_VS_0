#include "MPU6050/I2C.h"
#include "main.h"
#include "IIC/bsp_iic.h"

// 函数式对接
// 封装IIC软件，与官方库对接

BSP_IIC_GpioConf_t mpu6050_iic_config = {
		.sck_gpiox = GPIOB,
		.sck_pin = GPIO_PIN_8,
		.sda_gpiox = GPIOB,
		.sda_pin = GPIO_PIN_9,
};

HAL_StatusTypeDef i2c_write(uint8_t slave_addr, uint8_t reg_addr, uint8_t length, uint8_t const *data) {
   return BSP_IIC_Soft_writeByte(&mpu6050_iic_config, slave_addr, reg_addr, length, data);
}

HAL_StatusTypeDef i2c_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t length, uint8_t *data) {

    return BSP_IIC_Soft_readByte(&mpu6050_iic_config, slave_addr, reg_addr, length, data);
}

HAL_StatusTypeDef IICwriteBit(uint8_t slave_addr, uint8_t reg_addr, uint8_t bitNum, uint8_t data) {

    return BSP_IIC_Soft_writeBit(&mpu6050_iic_config, slave_addr, reg_addr, bitNum, data);
}

HAL_StatusTypeDef IICwriteBits(uint8_t slave_addr, uint8_t reg_addr, uint8_t bitStart, uint8_t length, uint8_t data) {
    return BSP_IIC_Soft_writeBits(&mpu6050_iic_config, slave_addr, reg_addr, bitStart, length, data);
}

//// 宏式定向（不适用，参数不同）
//// 宏替换官方库的函数
//
//#define i2c_write 	BSP_IIC_Soft_writeByte
//#define i2c_read 		BSP_IIC_Soft_readByte
//#define IICwriteBit 	BSP_IIC_Soft_writeBit
//#define IICwriteBits 	BSP_IIC_Soft_writeBits
