#include "Seg/mid_seg.h"

// 共阳数码管段码表：0~9 [无冒号 / 有冒号]
// bit7=冒号(0亮1灭), bit6=g, bit5=f, bit4=e, bit3=d, bit2=c, bit1=b, bit0=a
static const uint8_t TM1637_SEG[2][10] = {
			// [0] 无冒号/小数点 (bit7 = 0)
		    {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F},

		    //[1] 有冒号/小数点 (bit7 = 1，相当于把上面的值全都 | 0x80)
		    {0xBF, 0x86, 0xDB, 0xCF, 0xE6, 0xED, 0xFD, 0x87, 0xFF, 0xEF}
};

static void _numIntoArr(uint16_t num, uint8_t len, uint8_t *num_arr);

void MID_Seg_init(MID_Seg_Handele_t *h_seg, uint8_t num_count)
{
	// 配置
	h_seg->num_cnt = num_count;

	// 初始化
	MID_Seg_transmitCmd(h_seg, 0x40);
	uint8_t seg[] = {0x3F, 0x3F, 0x3F, 0x3F};
	MID_Seg_setSeg(h_seg, 0, seg, 4);
	MID_Seg_setBright(h_seg, 7);
}

void MID_Seg_transmitCmd(MID_Seg_Handele_t *h_seg, uint8_t cmd)
{
	BSP_Seg_start(&h_seg->p_gpio_conf);
	BSP_Seg_transmitByte(&h_seg->p_gpio_conf, cmd);
	BSP_Seg_stop(&h_seg->p_gpio_conf);
}

void MID_Seg_setBright(MID_Seg_Handele_t *h_seg, uint8_t bright)
{
	// 检验
	bright = (bright > 7) ? 7 : bright;

	BSP_Seg_start(&h_seg->p_gpio_conf);
	BSP_Seg_transmitByte(&h_seg->p_gpio_conf, 0x88 | bright);
	BSP_Seg_stop(&h_seg->p_gpio_conf);
}

void MID_Seg_setPosAndNum(MID_Seg_Handele_t *h_seg, uint8_t pos, uint16_t num, uint8_t len, uint8_t colon_select)
{
	// 检验
	len = (len > h_seg->num_cnt) ? h_seg->num_cnt : len;

	// 转换数组
	uint8_t num_arr[MID_SEG_NUM_BUF_SIZE];
	_numIntoArr(num, len, num_arr);

	// 发送
	BSP_Seg_start(&h_seg->p_gpio_conf);
	BSP_Seg_transmitByte(&h_seg->p_gpio_conf, 0xC0 | pos);
	for(uint8_t i = 0; i < len; i++) {
		BSP_Seg_transmitByte(&h_seg->p_gpio_conf, TM1637_SEG[colon_select ? 1 : 0][num_arr[i]]);
	}
	BSP_Seg_stop(&h_seg->p_gpio_conf);
}

void MID_Seg_setSeg(MID_Seg_Handele_t *h_seg, uint8_t pos, uint8_t *seg, uint8_t len)
{
	// 检验
	len = (len > h_seg->num_cnt) ? h_seg->num_cnt : len;

	BSP_Seg_start(&h_seg->p_gpio_conf);
	BSP_Seg_transmitByte(&h_seg->p_gpio_conf, 0xC0 | pos);
	for(uint8_t i = 0; i < len; i++) {
		BSP_Seg_transmitByte(&h_seg->p_gpio_conf, seg[i]);
	}
	BSP_Seg_stop(&h_seg->p_gpio_conf);
}

void MID_Seg_turnOff(MID_Seg_Handele_t *h_seg)
{
	BSP_Seg_start(&h_seg->p_gpio_conf);
	BSP_Seg_transmitByte(&h_seg->p_gpio_conf, 0x80);
	BSP_Seg_stop(&h_seg->p_gpio_conf);
}

void MID_Seg_turnOn(MID_Seg_Handele_t *h_seg)
{
	BSP_Seg_start(&h_seg->p_gpio_conf);
	BSP_Seg_transmitByte(&h_seg->p_gpio_conf, 0x88 | 7);
	BSP_Seg_stop(&h_seg->p_gpio_conf);
}

static void _numIntoArr(uint16_t num, uint8_t len, uint8_t *num_arr)
{
	uint16_t pow_10 = 1;
	while(len--)
		pow_10 *= 10;

	for(uint8_t i = 0; pow_10 /= 10; i++) {
		num_arr[i] = num / pow_10 % 10;
	}
}
