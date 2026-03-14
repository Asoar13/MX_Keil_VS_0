#ifndef __MID_OLED_H__
#define __MID_OLED_H__

#include <main.h>

#define   MID_OLED_SIGN			1
#define   MID_OLED_UNSIGN		0
#define   MID_OLED_LIGHT		1
#define   MID_OLED_EXTINGUISH	0
#define   MID_OLED_SMA_CODE	    8
#define   MID_OLED_MID_CODE	    16
#define   MID_OLED_BIG_CODE	    24

// 最终效果： 一个定义好的句柄，放在函数前就能控制相应模块

typedef struct {

	// 通信硬件绑定
	void *hw_handle;
	void (*_writeCmd)  (void *hw_handle, uint8_t *p_cmd, uint16_t Size);
	void (*_writeData) (void *hw_handle, uint8_t *p_data, uint16_t Size);
	void (*_RESET) (void *hw_handle);

	// 缓冲区绑定
	uint8_t show_buf[8][128];
} MID_OLED_Handle_t;

/*******************************************************对外层********************************************************/

void MID_OLED_init(MID_OLED_Handle_t *h_oled);
void MID_OLED_refreshScreen(MID_OLED_Handle_t *h_oled);
void MID_OLED_clearSome(MID_OLED_Handle_t *h_oled, uint8_t x, uint8_t y, uint8_t width, uint8_t height);
void MID_OLED_clearAll(MID_OLED_Handle_t *h_oled);
void MID_OLED_showImage(MID_OLED_Handle_t *h_oled, uint8_t x, uint8_t y, const uint8_t *img, uint8_t img_width, uint8_t img_height);
void MID_OLED_drawLine(MID_OLED_Handle_t *h_oled, int8_t X1, int8_t Y1, int8_t X2, int8_t Y2, uint8_t color);
void MID_OLED_drawSquareFrame(MID_OLED_Handle_t *h_oled, int8_t x_start, int8_t y_start, uint8_t width, uint8_t img_height);
void MID_OLED_drawCircle(MID_OLED_Handle_t *h_oled, int8_t x_cen, int8_t y_cen, uint8_t radius, uint8_t line_thick);

void MID_OLED_showInt(MID_OLED_Handle_t *h_oled, uint8_t x, uint8_t y, int32_t num, uint8_t len,
		uint8_t sign_confirme, uint8_t size_code);
void MID_OLED_showFloat(MID_OLED_Handle_t *h_oled, uint8_t x, uint8_t y, float num, uint8_t int_len, uint8_t frac_len,
		uint8_t sign_confirme, uint8_t size_code);
void MID_OLED_showString_ASCII(MID_OLED_Handle_t *h_oled, uint8_t x, uint8_t y, const char *str,
		uint8_t size_code);
void MID_OLED_showString_UTF8(MID_OLED_Handle_t *h_oled, uint8_t x, uint8_t y, const char *str,
		uint8_t size_code);

#endif
