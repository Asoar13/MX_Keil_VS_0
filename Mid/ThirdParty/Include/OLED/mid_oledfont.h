#ifndef __OLEDFONT_H__
#define __OLEDFONT_H__
#include "stm32f1xx_hal.h"
#define CHINESE_FONT_SIZE 3

typedef struct {
	char cn_index[CHINESE_FONT_SIZE + 1];	// UTF-8下3个字节一个汉字，剩1个为'\0'
	const uint8_t sma_cn_matrix[8];		// 汉字对应的字模
}MID_OLED_SMAChinseFont_t;

typedef struct {
	char cn_index[CHINESE_FONT_SIZE + 1];	// UTF-8下3个字节一个汉字，剩1个为'\0'
	const uint8_t mid_cn_matrix[32];		// 汉字对应的字模
}MID_OLED_MidChinseFont_t;

typedef struct {
	char cn_index[CHINESE_FONT_SIZE + 1];	// UTF-8下3个字节一个汉字，剩1个为'\0'
	const uint8_t big_cn_matrix[72];		// 汉字对应的字模
}MID_OLED_BIGChinseFont_t;


extern const unsigned char F6x8[];						// 小型ASCII 字模库
extern const unsigned char F8X16[];						// 中型ASCII 字模库
extern const unsigned char F12X24[];					// 大型ASCII 字模库

extern const MID_OLED_SMAChinseFont_t SMACnFontMatrixLib[];	// 小型汉字 字模库
extern uint32_t SMACnFontMatrixLib_size;					// 小型汉字 字模库 大小
extern const MID_OLED_MidChinseFont_t MidCnFontMatrixLib[];	// 中型汉字 字模库
extern uint32_t MidCnFontMatrixLib_size;					// 中型汉字 字模库 大小
extern const MID_OLED_BIGChinseFont_t BIGCnFontMatrixLib[];	// 大型汉字 字模库
extern uint32_t BIGCnFontMatrixLib_size;					// 大型汉字 字模库 大小

extern uint8_t oled_upword_arrow[];
extern uint32_t oled_upword_arrow_size;
#endif

