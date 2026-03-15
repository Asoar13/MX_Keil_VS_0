#include "Seg/mid_seg.h"

// 共阳数码管段码表: 0~9 [无冒号 / 有冒号]
// bit7=冒号(0亮1灭), bit6=g, bit5=f, bit4=e, bit3=d, bit2=c, bit1=b, bit0=a
static const uint8_t TM1637_SEG[2][10] = {
			// [0] 无冒号/小数点 (bit7 = 0)
		    {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F},

		    //[1] 有冒号/小数点 (bit7 = 1，相当于把上面的值全都 | 0x80)
		    {0xBF, 0x86, 0xDB, 0xCF, 0xE6, 0xED, 0xFD, 0x87, 0xFF, 0xEF}
};

static void _numIntoArr(uint16_t num, uint8_t len, uint8_t *num_arr);

/**
  * @brief  发送指令给数码管
  * @param  h_seg: 绑定的数码管的GPIO配置句柄
  * @param  cmd: 指令字节
  * @retval 无
  */
static void MID_TM1637_transmitCmd(MID_Seg_Handele_t *h_seg, uint8_t cmd)
{
	BSP_Seg_start(&h_seg->p_gpio_conf);
	BSP_Seg_transmitByte(&h_seg->p_gpio_conf, cmd);
	BSP_Seg_stop(&h_seg->p_gpio_conf);
}

/**
  * @brief  初始化数码管(最大亮度，显示4个0)
  * @param  h_seg: 绑定的数码管的GPIO配置句柄
  * @param  num_count: 指定此数码管的大小
  * @retval 无
  */
void MID_TM1637_init(MID_Seg_Handele_t *h_seg, uint8_t num_count)
{
	// 配置
	h_seg->num_cnt = num_count;

	// 初始化
	MID_TM1637_transmitCmd(h_seg, 0x40);
	uint8_t seg[] = {0x3F, 0x3F, 0x3F, 0x3F};
	MID_TM1637_setSeg(h_seg, 0, seg, 4);
	MID_TM1637_setBright(h_seg, 7);
}

/**
  * @brief  控制数码管的亮度
  * @param  h_seg: 绑定的数码管的GPIO配置句柄
  * @param  bright: (0 ~ 7)
  * @retval 无
  */
void MID_TM1637_setBright(MID_Seg_Handele_t *h_seg, uint8_t bright)
{
	// 检验
	bright = (bright > 7) ? 7 : bright;

	BSP_Seg_start(&h_seg->p_gpio_conf);
	BSP_Seg_transmitByte(&h_seg->p_gpio_conf, 0x88 | bright);
	BSP_Seg_stop(&h_seg->p_gpio_conf);
}

/**
  * @brief  在指定位置显示指定数字
  * @param  h_seg: 绑定的数码管的GPIO配置句柄
  * @param  pos: 显示的位置
  * @param  num: 被显示的数字
  * @param  len: 预计的数字长度
  * @param  colon_select: 决定是否点亮右下角的符号
  * @note   显示长度不超过初始化时的设置长度，否则截断超出部分
  * @retval 无
  */
void MID_TM1637_setPosAndNum(MID_Seg_Handele_t *h_seg, uint8_t pos, uint16_t num, uint8_t len, uint8_t colon_select)
{
	// 检验
	len = (len > h_seg->num_cnt) ? h_seg->num_cnt :  len;

	// 转换数组
	uint8_t num_arr[MID_SEG_NUM_BUF_SIZE];
	_numIntoArr(num, len, num_arr);

	// 发送
	BSP_Seg_start(&h_seg->p_gpio_conf);
	BSP_Seg_transmitByte(&h_seg->p_gpio_conf, 0xC0 | pos);
	for(uint8_t i = 0; i < len; i++) {
		BSP_Seg_transmitByte(&h_seg->p_gpio_conf, TM1637_SEG[colon_select ? 1 :  0][num_arr[i]]);
	}
	BSP_Seg_stop(&h_seg->p_gpio_conf);
}

/**
  * @brief  时数码管显示指定段码
  * @param  h_seg: 绑定的数码管的GPIO配置句柄
  * @param  pos: 显示的位置(0为第一位)
  * @param  seg: 段码数组
  * @param  len: 段码数组长度
  * @note   段码以共阴为准
  * @note   显示长度不超过初始化时的设置长度，否则截断超出部分
  * @retval 无
  */
void MID_TM1637_setSeg(MID_Seg_Handele_t *h_seg, uint8_t pos, uint8_t *seg, uint8_t len)
{
	// 检验
	if(!seg) return;
	len = (len > h_seg->num_cnt) ? h_seg->num_cnt :  len;

	BSP_Seg_start(&h_seg->p_gpio_conf);
	BSP_Seg_transmitByte(&h_seg->p_gpio_conf, 0xC0 | pos);
	for(uint8_t i = 0; i < len; i++) {
		BSP_Seg_transmitByte(&h_seg->p_gpio_conf, seg[i]);
	}
	BSP_Seg_stop(&h_seg->p_gpio_conf);
}

/**
  * @brief  关闭数码管(0x80)
  * @param  h_seg: 绑定的数码管的GPIO配置句柄
  * @retval 无
  */
void MID_TM1637_turnOff(MID_Seg_Handele_t *h_seg)
{
	BSP_Seg_start(&h_seg->p_gpio_conf);
	BSP_Seg_transmitByte(&h_seg->p_gpio_conf, 0x80);
	BSP_Seg_stop(&h_seg->p_gpio_conf);
}

/**
  * @brief  开启数码管(0x88)，并设置亮度为最大(7)
  * @param  h_seg: 绑定的数码管的GPIO配置句柄
  * @retval 无
  */
void MID_TM1637_turnOn(MID_Seg_Handele_t *h_seg)
{
	BSP_Seg_start(&h_seg->p_gpio_conf);
	BSP_Seg_transmitByte(&h_seg->p_gpio_conf, 0x88 | 7);
	BSP_Seg_stop(&h_seg->p_gpio_conf);
}

/**
  * @brief  将一个数字写到数组里
  * @param  num: 待转换的数值
  * @param  len: 预设数字长度
  * @param  num_arr: 给定数组
  * @note   位数不足会高位补零，超过会截断并丢弃高位
  * @retval 无
  */
static void _numIntoArr(uint16_t num, uint8_t len, uint8_t *num_arr)
{
	uint16_t pow_10 = 1;
	while(len--)
		pow_10 *= 10;

	for(uint8_t i = 0; pow_10 /= 10; i++) {
		num_arr[i] = num / pow_10 % 10;
	}
}

