#ifndef __I2C_H_
#define __I2C_H_

#include "main.h"
#include "stdint.h"
#include <math.h>

// --- 1. 核心暗号：绝对不能删，DMP库靠它们识别身份 ---
#define STM32_HAL
#define MPU6050

// 会警告重定义，给它判断一下
#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

// --- 2. 适配你的单片机延时和获取时间 ---
#define delay_ms  HAL_Delay
#define get_ms(timestamp) (*timestamp=HAL_GetTick())

#define fabs        fabsf
#define min(a,b) ((a<b)?a:b)

// --- 3. 恢复原作者的空回调函数，防止找不到函数报错 ---

// 声明一下这个结构体，不然需要包含inv_mpu.h
struct int_param_s;
static inline int reg_int_cb(struct int_param_s *int_param)
{
  UNUSED(int_param);
    return 0;
}

// 我不用，改为标准空白
#define __no_operation() do {} while(0)

// --- 4. 屏蔽打印：解决没配Trace.h和printf的报错（你需要的修改在这里） ---
#define log_i(...)     do {} while (0)
#define log_e(...)     do {} while (0)

// --- 底层读写声明（保持不变） ---
HAL_StatusTypeDef i2c_write(uint8_t slave_addr, uint8_t reg_addr, uint8_t length, uint8_t const *data);
HAL_StatusTypeDef i2c_read(uint8_t slave_addr, uint8_t reg_addr, uint8_t length, uint8_t *data);
HAL_StatusTypeDef IICwriteBit(uint8_t slave_addr, uint8_t reg_addr, uint8_t bitNum, uint8_t data);
HAL_StatusTypeDef IICwriteBits(uint8_t slave_addr, uint8_t reg_addr, uint8_t bitStart, uint8_t length, uint8_t data);

#endif
