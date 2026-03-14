#ifndef __MID_ENCODER_H
#define __MID_ENCODER_H
#include "Encoder/bsp_encoder.h"

typedef struct {
	TIM_HandleTypeDef *htimx;
	volatile int16_t last_count;
	volatile int16_t count;
	volatile int16_t step;
} MID_EncoderHandle_Hard_t;

typedef struct {
	int32_t *p_variable;
	int32_t max;
	int32_t min;
	uint16_t step;
} MID_EncoderAdjustItem_t;

//typedef struct {
//	GPIO_TypeDef *gpiox;
//	uint32_t gpio_pin_a;
//	uint32_t gpio_pin_b;
//	TIM_HandleTypeDef *htimx;
//	volatile int16_t last_count;
//	volatile int16_t count;
//	volatile MID_EncoderDirect direct;
//} MID_EncoderHandle_Hard_t;

void MID_Encoder_init(MID_EncoderHandle_Hard_t *encoder_gpio_conf);
void MID_Encoder_process(MID_EncoderHandle_Hard_t *encoder_handle, MID_EncoderAdjustItem_t *item_info);

#endif
