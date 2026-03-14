#ifndef __MID_SEG_H
#define __MID_SEG_H
#include "Seg/bsp_seg.h"

#define MID_SEG_NUM_BUF_SIZE 8

typedef struct {
	BSP_Seg_GpioConf_t p_gpio_conf; // (手动配置) - 绑定句柄控制的对象
	uint8_t num_cnt; // (参数配置) - 初始化函数参数配置
} MID_Seg_Handele_t;

void MID_Seg_init(MID_Seg_Handele_t *h_seg, uint8_t num_cnout);
void MID_Seg_transmitCmd(MID_Seg_Handele_t *h_seg, uint8_t cmd);
void MID_Seg_setBright(MID_Seg_Handele_t *h_seg, uint8_t bright);
void MID_Seg_setPosAndNum(MID_Seg_Handele_t *h_seg, uint8_t pos, uint16_t num, uint8_t len, uint8_t colon_select);
void MID_Seg_setSeg(MID_Seg_Handele_t *h_seg, uint8_t pos, uint8_t *seg, uint8_t len);
void MID_Seg_turnOff(MID_Seg_Handele_t *h_seg);
void MID_Seg_turnOn(MID_Seg_Handele_t *h_seg);

#endif
