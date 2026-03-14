#include <OLED/mid_oled.h>
#include <OLED/mid_oledfont.h>
#include <OLED/bsp_oled.h>
#include <string.h>

#define __MID_OLED_ABS(num) (num = ((num) < 0) ? -(num) : (num))

// 修改后的初始化数组 (适配全屏一次性刷新)
static uint8_t CMD_Data[] = {
    0xAE,           // 关显示
    0x20, 0x00,     // [新增] 设置内存寻址模式：0x00 = 水平模式 (Horizontal)
    0x21, 0, 127,   // [新增] 设置列地址范围：0~127 (全屏)
    0x22, 0, 7,     // [新增] 设置页地址范围：0~7   (全屏)
    0x40,           // 设置起始行：0
    0x81, 0xFF,     // 对比度设置：最大
    0xA1,           // 段重映射 (SEG Remap)
    0xA6,           // 正常显示 (不反白)
    0xA8, 0x3F,     // 多路复用率 (Multiplex Ratio)
    0xC8,           // COM扫描方向
    0xD3, 0x00,     // 显示偏移 (Display Offset)
    0xD5, 0x80,     // 显示时钟分频 (Display Clock Divide)
    0xD8, 0x05,     // 区域颜色模式 (Area Color Mode) - 部分SSD1306无需此指令，保留无害
    0xD9, 0xF1,     // 预充电周期 (Pre-Charge Period)
    0xDA, 0x12,     // COM引脚硬件配置
    0xDB, 0x30,     // VCOMH 电压倍率
    0x8D, 0x14,     // 电荷泵设置 (Charge Pump) -> 开
    0xAF            // 开显示
};


/***************************************************************************内置层*******************************************************************************/
/*=====================================基础打印==============================================*/
/**
 * @brief 计算OLED显示的掩码（用于保留原有像素，只更新目标区域）
 * @param y_offset 像素在页内的偏移(0-7)
 * @param mask_low 输出低段位掩码
 * @param mask_high 输出高段位掩码
 */
static void __MID_OLED_calcMask(uint8_t y_offset, uint8_t *mask_low, uint8_t *mask_high)
{
    *mask_low = 0xFF << y_offset;   // 低段位掩码（当前页保留位）
    *mask_high = 0xFF >> (8 - y_offset); // 高段位掩码（下一页保留位）
}

/**
 * @brief 计算图像边缘的掩码（用于删除超出边缘的像素，实现规定范围显示）
 * @param buf_y_end 图像在数组里时最下端的y坐标（小值：0~127）
 * @param mask_limit 输出高段位掩码
 */
static void __MID_OLED_calcMaskLimit(uint8_t imag_y_end,uint8_t *mask_limit)
{
	uint8_t y_limit_setoff = (imag_y_end-1) % 8 + 1;	// 图象末尾偏移
	*mask_limit = 0xFF >> (8 - y_limit_setoff);	// 高位掩码（保留未超出图像的bit）
}

/**
 * @brief 更新OLED缓冲区数据（核心：跨行像素拆分与原有数据保护）
 * @param x_buf 缓冲区列索引
 * @param y_buf 缓冲区页索引
 * @param pixel_data 待显示的像素数据(1字节)
 * @param mask_low 低段位掩码
 * @param mask_high 高段位掩码
 */
static void __MID_OLED_updateBuf(MID_OLED_Handle_t *h_oled, uint8_t x_buf, uint8_t y_buf, uint8_t pixel_data,
                           uint8_t mask_low, uint8_t mask_high, uint8_t y_offset)
{
    // 处理当前页的像素数据
	h_oled->show_buf[y_buf][x_buf] = (h_oled->show_buf[y_buf][x_buf] & mask_high) | (pixel_data << y_offset);

    // 若当前页不是最后一页，且偏移不是0（mask_high不是全掩），处理跨页的像素数据
    if (y_buf < 7 && mask_high > 0) {
    	h_oled->show_buf[y_buf + 1][x_buf] = (h_oled->show_buf[y_buf + 1][x_buf] & mask_low) | (pixel_data >> (8 - y_offset));
    }
}

/**
 * @brief 精准显示指定大小的图像到OLED指定位置（支持跨行，不覆盖原有内容）
 * @param x_pos 图像左上角列坐标(0-127)
 * @param y_pos 图像左上角行坐标(0-63)
 * @param img_data 图像数据数组(1字节=8个像素点)
 * @param img_width 图像宽度(列数)
 * @param img_height 图像高度(行数)
 */
static void __MID_OLED_showImagePrecise(MID_OLED_Handle_t *h_oled, uint8_t x_pos, uint8_t y_pos, const uint8_t *img_data, uint8_t img_width, uint8_t img_height)
{
    // 1. 基础参数校验（避免越界）
    if (x_pos > 127 || y_pos > 63 || img_width < 1 || img_height < 1 || img_data == NULL) {
        return;
    }

    // 2. 计算核心参数（坐标转缓冲区索引）
    uint8_t y_offset = y_pos % 8;          // 像素在页内的偏移(0-7)
    uint8_t buf_y_start = y_pos / 8;       // 缓冲区起始页
    uint8_t buf_x_start = x_pos;           // 缓冲区起始列
    // 计算缓冲区结束坐标（避免超出OLED范围）
    uint8_t buf_x_end = x_pos + img_width - 1;
    uint8_t buf_y_end = (y_pos + img_height - 1) / 8;
    buf_x_end = (buf_x_end > 127) ? 127 : buf_x_end;
    buf_y_end = (buf_y_end > 7) ? 7 : buf_y_end;

    // 3. 计算掩码（用于保护原有像素）
    uint8_t mask_low, mask_high, mask_limit;
    __MID_OLED_calcMask(y_offset, &mask_low, &mask_high);	//保护原有像素，加入新字节的像素
    __MID_OLED_calcMaskLimit(img_height, &mask_limit);	//保留范围内像素，掩盖超出范围的字节像素

    // 4. 遍历更新缓冲区（双层循环简化）
    for (uint8_t img_y = 0; img_y <= (img_height - 1) / 8; img_y++) {
        uint8_t buf_y = buf_y_start + img_y;
        if (buf_y > buf_y_end) break; // 超出缓冲区范围则退出

        for (uint8_t img_x = 0; img_x < img_width; img_x++) {
            uint8_t buf_x = buf_x_start + img_x;
            if (buf_x > buf_x_end) break; // 超出列范围则退出

            // 取当前像素数据
            uint8_t pixel_data = img_data[img_y * img_width + img_x];
            // 最下页进行边缘处理
            if(img_y == (img_height - 1) / 8)
            {pixel_data &= mask_limit;}
            // 更新缓冲区（自动处理跨页逻辑）
            __MID_OLED_updateBuf(h_oled, buf_x, buf_y, pixel_data, mask_low, mask_high, y_offset);
        }
    }
}

static void __MID_OLED_showImageFormSource(MID_OLED_Handle_t *h_oled, const uint8_t *source, uint8_t x, uint8_t y, uint8_t No_x, uint8_t width, uint8_t height)
{
	__MID_OLED_showImagePrecise(h_oled, x, y, source + (height*width / 8)*No_x, width, height);
}

/*=====================================基础清除==============================================*/
/**
 * @brief 清除Buffer中指定区域的像素（置为0），不立即刷新屏幕
 * @param x_pos 起始列 (0-127)
 * @param y_pos 起始行像素 (0-63)
 * @param width 宽度
 * @param height 高度
 */
static void __MID_OLED_ClearArea_Buf(MID_OLED_Handle_t *h_oled, uint8_t x_pos, uint8_t y_pos, uint8_t width, uint8_t height)
{
    // 1. 边界校验
    if (x_pos > 127 || y_pos > 63) return;
    if (x_pos + width > 128) width = 128 - x_pos;
    if (y_pos + height > 64) height = 64 - y_pos;

    // 2. 遍历每一列
    for (uint8_t x = x_pos; x < x_pos + width; x++) {
        uint8_t y_start = y_pos, y_end = y_pos + height;
        uint8_t page_start = y_start / 8, page_end = (y_end - 1) / 8;

        for (uint8_t p = page_start; p <= page_end; p++) {
            uint8_t data_mask = 0x00, page_y_start = p * 8, page_y_end = p * 8 + 7; // 0代表不清除，1代表清除
            uint8_t clear_start = (y_start > page_y_start) ? (y_start % 8) : 0;
            uint8_t clear_end = (y_end - 1 < page_y_end) ? ((y_end - 1) % 8) : 7;

            // 生成清除掩码：例如 clear_start=2, clear_end=4 -> 00011100
            for(uint8_t k = clear_start; k <= clear_end; k++) {
                data_mask |= (1 << k);
            }

            // 执行清除：使用 & (~mask)
            h_oled->show_buf[p][x] &= (~data_mask);
        }
    }
}

/*=====================================数字基础==============================================*/
/**
 * @brief 判断数字的符号并显示在最前面
 * @param source 数字来自具体一个数据源
 * @param x 数字左上角列坐标(0-127)
 * @param y 数字左上角行坐标(0-63)
 * @param num 数字本身
 * @param width 数字宽度(列数)
 * @param height 数字高度(行数)
 */
static void __MID_OLED_showSign(MID_OLED_Handle_t *h_oled, const uint8_t *source, uint8_t x, uint8_t y, int32_t num, uint8_t width, uint8_t height)
{
	uint8_t sign_No_x = 0;
	if(num >= 0){
		sign_No_x = '+' - ' ';
	}else{
		sign_No_x = '-' - ' ';
	}
	__MID_OLED_showImageFormSource(h_oled, source, x, y, sign_No_x, width, height);
}

/**
 * @brief 把一个数字对象按照10^len算出
 * @param object 对象本身(的指针)
 * @param len 要获得的长度
 */
static void __MID_OLED_get10_n(uint32_t *object, uint8_t len)
{
	*object = 1;
	for(uint8_t i = 0; i < len; i++){
		*object *= 10;
	}
}

/*=====================================绘画基础==============================================*/
/**
 * @brief 点亮或者熄灭特定坐标
 * @param x x坐标(0-127)
 * @param y y坐标(0-63)
 * @param color 点亮还是熄灭
 */
static void __MID_OLED_drawPoint(MID_OLED_Handle_t *h_oled, uint8_t x, uint8_t y, uint8_t color)
{
	uint8_t arr_page = y / 8;	// 第arr_page页 [0, 7]
	uint8_t arr_byte = x;		// 第arr_byte字节 [0, 127]
	uint8_t arr_bit = y % 8;	// 第arr_bit比特 [0, 7]

	h_oled->show_buf[arr_page][arr_byte] |= color << arr_bit;
}

/*=====================================中文基础==============================================*/

// 辅助函数：判断 UTF-8 字符的字节长度
static uint8_t __MID_OLED_GetUTF8Len(char first_byte)
{
    if ((first_byte & 0x80) == 0) return 1;       // 0xxxxxxx -> 1字节 (ASCII)
    if ((first_byte & 0xE0) == 0xC0) return 2;    // 110xxxxx -> 2字节
    if ((first_byte & 0xF0) == 0xE0) return 3;    // 1110xxxx -> 3字节 (常用汉字)
    if ((first_byte & 0xF8) == 0xF0) return 4;    // 11110xxx -> 4字节
    return 3; // 无法识别，默认按3字节跳过，防止死循环
}

static void __MID_OLED_getSplitedStr(const char *str, uint32_t index_cur, uint8_t step, char* str_cur, uint8_t len)
{
	// 填充'\0'，提前为str_cur添加结束标志
	memset(str_cur, '\0', len);

	// 从指定index_cur下标开始取step个char放到str_cur
	for(uint8_t i = 0; i < step; i++){
		str_cur[i] = str[index_cur + i];
	}
}

static const uint8_t* __MID_OLED_FindCnPtrBySize(uint8_t size_code, const char *str)
{
	if(size_code == MID_OLED_SMA_CODE) {
		for(uint32_t i = 0; i < SMACnFontMatrixLib_size; i++){
			if(strcmp(str, SMACnFontMatrixLib[i].cn_index) == 0){
				return SMACnFontMatrixLib[i].sma_cn_matrix;
			}
		}
		return SMACnFontMatrixLib[0].sma_cn_matrix;
	} else if(size_code == MID_OLED_MID_CODE) {
		for(uint32_t i = 0; i < MidCnFontMatrixLib_size; i++){
			if(strcmp(str, MidCnFontMatrixLib[i].cn_index) == 0){
				return MidCnFontMatrixLib[i].mid_cn_matrix;
			}
		}
		return MidCnFontMatrixLib[0].mid_cn_matrix;
	} else if(size_code == MID_OLED_BIG_CODE) {
		for(uint32_t i = 0; i < BIGCnFontMatrixLib_size; i++){
			if(strcmp(str, BIGCnFontMatrixLib[i].cn_index) == 0){
				return BIGCnFontMatrixLib[i].big_cn_matrix;
			}
		}
		return BIGCnFontMatrixLib[0].big_cn_matrix;
	}
	return NULL;
}
/*=====================================UTF-8基础==============================================*/

static unsigned char * __MID_OLED_confirmSize_ASCII(uint8_t confirm_code, uint8_t *width, uint8_t *height)
{
	if(confirm_code == MID_OLED_SMA_CODE) {
		*width = 6;	 *height = 8;
		return (unsigned char *)F6x8;
	} else if(confirm_code == MID_OLED_MID_CODE) {
		*width = 8;	 *height = 16;
		return (unsigned char *)F8X16;
	} else if(confirm_code == MID_OLED_BIG_CODE) {
		*width = 12;  *height = 24;
		return (unsigned char *)F12X24;
	}
	return NULL;
}

static void __MID_OLED_confirmSize_UTF8CN(uint8_t confirm_code, uint8_t *width, uint8_t *height)
{
	if(confirm_code == MID_OLED_SMA_CODE) {
		*width = 8;  *height = 8;
	} else if(confirm_code == MID_OLED_MID_CODE) {
		*width = 16;  *height = 16;
	} else if(confirm_code == MID_OLED_BIG_CODE) {
		*width = 24;  *height = 24;
	}
}

/***********************************************************************完整显示功能函数**************************************************************************/
/**
 * @brief 根据输入的参数确定任意图像的位置与大小并显示
 * @param x 图像左上角列坐标(0-127)
 * @param y 图像左上角行坐标(0-63)
 * @param img 图像本身(的指针)
 * @param img_width 图像宽度(列数)
 * @param img_height 图像高度(行数)
 */
static void MID_OLED_showAnyImage(MID_OLED_Handle_t *h_oled, uint8_t x, uint8_t y, const uint8_t *img, uint8_t img_width, uint8_t img_height)
{
	__MID_OLED_showImagePrecise(h_oled, x, y, img, img_width, img_height);
}

/**
 * @brief 根据参数在指定位置显示字符串
 * @param x 字符串左上角列坐标(0-127)
 * @param y 字符串左上角行坐标(0-63)
 * @param str 字符串本身(的指针)
 */
static void MID_OLED_showAnyString(MID_OLED_Handle_t *h_oled, const uint8_t *source, uint8_t width, uint8_t height,
		uint8_t x, uint8_t y,  const char *str)
{
	for(uint8_t i = 0; str[i] != '\0'; i++){
		uint8_t No_x = str[i] - ' ';
		__MID_OLED_showImageFormSource(h_oled, source, x + width * i, y, No_x, width, height);
	}
}
/**
 * @brief 根据参数在指定位置显示整型
 * @param x 数字左上角列坐标(0-127)
 * @param y 数字左上角行坐标(0-63)
 * @param num 数字本身
 * @param len 数字长度
 * @param sign_confirme 是否显示符号
 */
static void MID_OLED_showAnyInt(MID_OLED_Handle_t *h_oled, const uint8_t *source, uint8_t width, uint8_t height,
		uint8_t x, uint8_t y, int32_t num, uint8_t len, uint8_t sign_confirme)
{
	uint32_t length = 1, No_x;
	int32_t single_num;

	// 添加符号
	if(sign_confirme == MID_OLED_SIGN){
		__MID_OLED_showSign(h_oled, source, x, y, num, width, height);
		x += width;
	}
	__MID_OLED_ABS(num);

	// 计算长度，生成10^n
	__MID_OLED_get10_n(&length, len);
	length /= 10;

	// 取得每位数字，按顺序显示
	for(uint8_t i = 0; i < len; i++){
		single_num = (num / length) % 10;
		No_x = single_num + '0' - ' ';
		__MID_OLED_showImageFormSource(h_oled, source, x + width * i, y, No_x, width, height);
		length /= 10;
	}
}

/**
 * @brief 根据参数在指定位置显示浮点数
 * @param x 浮点数左上角列坐标(0-127)
 * @param y 浮点数左上角行坐标(0-63)
 * @param num 浮点数本身
 * @param int_len 浮点数整数部分长度
 * @param frac_len 浮点数小数部分长度
 * @param sign_confirme 是否显示符号
 */
static void MID_OLED_showAnyFloat(MID_OLED_Handle_t *h_oled, const uint8_t *source, uint8_t width, uint8_t height,
		uint8_t x, uint8_t y, float num, uint8_t int_len, uint8_t frac_len, uint8_t sign_confirme)
{
	// 添加符号
	if(sign_confirme == MID_OLED_SIGN){
		__MID_OLED_showSign(h_oled, source, x, y, num, width, height);
		x += width;
	}
	__MID_OLED_ABS(num);

	// 显示整数部分
	uint16_t int_part = num;
	MID_OLED_showAnyInt(h_oled, source, width, height,
			x, y, int_part, int_len, MID_OLED_UNSIGN);

	// 显示小数点
	MID_OLED_showAnyString(h_oled, source, width, height,
			x + int_len * width, y, ".");

	// 显示小数部分
	uint32_t frac_part, length;
	__MID_OLED_get10_n(&length, frac_len);
	frac_part = (uint32_t)(num * length) % length;
	MID_OLED_showAnyInt(h_oled, source, width, height,
			x + (int_len + 1) * width, y, frac_part, frac_len, MID_OLED_UNSIGN);
}

/**
 * @brief 以某个颜色连接两点
 * @param x_start x起始坐标 (0-127)
 * @param y_start x起始坐标 (0-63)
 * @param x_end x结束坐标 (0-127)
 * @param y_end x结束坐标 (0-63)
 * @param color 直线的颜色 (light / extinguish)
 */
#define OLED_WIDTH          128  // OLED宽度
#define OLED_HEIGHT         128  // OLED高度
static void MID_OLED_drawAnyLine(MID_OLED_Handle_t *h_oled, int8_t x_start, int8_t y_start, int8_t x_end, int8_t y_end, uint8_t color)
{
    int8_t x_cur = x_start, y_cur = y_start;

    // 1. 完整的参数合法性检查
    if(x_start < 0 || x_start >= OLED_WIDTH || y_start < 0 || y_start >= OLED_HEIGHT) return;
    if(x_end < 0 || x_end >= OLED_WIDTH || y_end < 0 || y_end >= OLED_HEIGHT) return;
    // 检查颜色参数，非法则默认点亮
    if(color != MID_OLED_LIGHT && color != MID_OLED_EXTINGUISH) color = MID_OLED_LIGHT;

    // 2. 使用Bresenham算法画直线（无浮点数，避免精度和除零问题）
    int8_t dx = x_end - x_start;
    int8_t dy = y_end - y_start;

    // 计算方向和步长
    int8_t step_x = (dx > 0) ? 1 : (dx < 0) ? -1 : 0;
    int8_t step_y = (dy > 0) ? 1 : (dy < 0) ? -1 : 0;

    // 取绝对值
    uint8_t abs_dx = (dx > 0) ? dx : -dx;
    uint8_t abs_dy = (dy > 0) ? dy : -dy;

    // 误差项
    int16_t err = (abs_dx > abs_dy) ? abs_dx : -abs_dy;
    err /= 2;

    while(1) {
        // 绘制当前点（确保坐标合法）
        if(x_cur >= 0 && x_cur < OLED_WIDTH && y_cur >= 0 && y_cur < OLED_HEIGHT) {
            __MID_OLED_drawPoint(h_oled, x_cur, y_cur, color);
        }

        // 到达终点则退出
        if(x_cur == x_end && y_cur == y_end) break;

        // 计算下一个点
        int16_t e2 = err;
        if(e2 > -abs_dx) { err -= abs_dy;x_cur += step_x; }
        if(e2 < abs_dy) { err += abs_dx; y_cur += step_y; }
    }
}

/**
 * @brief 根据参数在指定位置显示一个汉字
 * @param x 浮点数左上角列坐标(0-127)
 * @param y 浮点数左上角行坐标(0-63)
 * @param str 汉字本身 (数组)
 */
static void MID_OLED_showAnyCnChar(MID_OLED_Handle_t *h_oled, uint8_t size_code, uint8_t width, uint8_t height,
		uint8_t x, uint8_t y, const char *str)
{
	// 寻找索引绑定的指针（数组）
	 const uint8_t *ptr = __MID_OLED_FindCnPtrBySize(size_code, str);

	// 指定位置显示
	MID_OLED_showAnyImage(h_oled, x, y, ptr, width, height);
}

/**
 * @brief 根据参数在指定位置显示一句话 (UTF-8都行)
 * @param x 浮点数左上角列坐标(0-127)
 * @param y 浮点数左上角行坐标(0-63)
 * @param str 句子本身 (数组)
 */
static void MID_OLED_showAnyString_UTF8(MID_OLED_Handle_t *h_oled, const uint8_t *source_ascii, uint8_t width_ascii, uint8_t height_ascii,
								 uint8_t size_code, uint8_t width_utf8, uint8_t height_utf8,
								 uint8_t x, uint8_t y, const char *str)
{
	uint8_t step = 0, x_cur = x;	// x_cur是当前x所在坐标[0, 127]
	char str_cur[CHINESE_FONT_SIZE + 1] = "spe";

	// 分类显示
	for(uint8_t index_cur = 0; str[index_cur] != '\0'; index_cur += step){	// 读取到'\0'结束，每次偏移一定char数

		// 更新step，由第一个字节决定step
		step = __MID_OLED_GetUTF8Len(str[index_cur]);

		// 取出step长度的char作为当前目标str
		__MID_OLED_getSplitedStr(str, index_cur, step, str_cur, CHINESE_FONT_SIZE + 1);

		if(step == 1){	// ASCII码处理英文(被UTF-8兼容)

			// 指定位置显示英文
			MID_OLED_showAnyString(h_oled, source_ascii, width_ascii, height_ascii, x_cur, y, str_cur);
			x_cur += width_ascii;
		} else {		// UTF-8码处理中文

			// 指定位置显示中文
			MID_OLED_showAnyCnChar(h_oled, size_code, width_utf8, height_utf8, x_cur, y, str_cur);
			x_cur += width_utf8;	// 中文宽高相等
		}
	}
}

/***************************************************************************对外层*******************************************************************************/

/**
 * @brief 初始化OLED
 */
void MID_OLED_init(MID_OLED_Handle_t *h_oled)
{
    HAL_Delay(200); // 供电延迟

    // 复位
    h_oled->_RESET(h_oled->hw_handle);

    // 写初始化指令
    for(uint8_t i = 0; i < sizeof(CMD_Data); i++)
    {
    	h_oled->_writeCmd(h_oled->hw_handle, CMD_Data+i, 1);
    }

    MID_OLED_refreshScreen(h_oled);	// 刷新
}

/**
 * @brief 将show_buf数组按照128x64对应在OLED的数据寄存器
 */
void MID_OLED_refreshScreen(MID_OLED_Handle_t *h_oled)
{
    // 显存总大小 = 8页 * 128列 = 1024字节
    // 0x40 是数据(Data)的寄存器地址
    // 0x78 是设备地址(写)

    // 强制类型转换，将二维数组视为一维大数组
	h_oled->_writeData(h_oled->hw_handle, (uint8_t *)h_oled->show_buf, 128 * 8);
}

/**
 * @brief 规划一个待清除区域，然后清除show_buf的此区域
 * @param x 待清除区域左上角列坐标(0-127)
 * @param y 待清除区域左上角行坐标(0-63)
 * @param width 待清除区域宽度(列数)
 * @param height 待清除区域高度(行数)
 */
void MID_OLED_clearSome(MID_OLED_Handle_t *h_oled, uint8_t x_start, uint8_t y_start, uint8_t width, uint8_t height)
{
	__MID_OLED_ClearArea_Buf(h_oled, x_start,  y_start, width, height);
}

/**
 * @brief show_buf的数据全部归零
 */
void MID_OLED_clearAll(MID_OLED_Handle_t *h_oled)
{
	for(uint8_t y = 0; y < 8; y++){
		memset(h_oled->show_buf[y], 0, 128);
	}
}

void MID_OLED_showInt(MID_OLED_Handle_t *h_oled, uint8_t x, uint8_t y, int32_t num, uint8_t len,
		uint8_t sign_confirme, uint8_t size_code)
{
	// 确定大小
	unsigned char *matrix_source = NULL;
	uint8_t width, height;
	matrix_source = __MID_OLED_confirmSize_ASCII(size_code, &width, &height);

	// 显示
	MID_OLED_showAnyInt(h_oled, matrix_source, width, height, x, y, num, len, sign_confirme);
}

void MID_OLED_showFloat(MID_OLED_Handle_t *h_oled, uint8_t x, uint8_t y, float num, uint8_t int_len, uint8_t frac_len,
		uint8_t sign_confirme, uint8_t size_code)
{
	// 确定大小
	unsigned char *matrix_source = NULL;
	uint8_t width, height;
	matrix_source = __MID_OLED_confirmSize_ASCII(size_code, &width, &height);

	// 显示
	MID_OLED_showAnyFloat(h_oled, matrix_source, width, height, x, y, num, int_len, frac_len, sign_confirme);
}

void MID_OLED_showString_ASCII(MID_OLED_Handle_t *h_oled, uint8_t x, uint8_t y, const char *str,
		uint8_t size_code)
{
	// 确定大小
	unsigned char *matrix_source = NULL;
	uint8_t width, height;
	matrix_source = __MID_OLED_confirmSize_ASCII(size_code, &width, &height);

	// 显示
	MID_OLED_showAnyString(h_oled, matrix_source, width, height, x, y, str);
}

void MID_OLED_showString_UTF8(MID_OLED_Handle_t *h_oled, uint8_t x, uint8_t y, const char *str,
		uint8_t size_code)
{
	// 确定大小
	unsigned char *matrix_source_ASCII = NULL;
	uint8_t width_ASCII, height_ASCII, width_UTF8, height_UTF8;
	matrix_source_ASCII = __MID_OLED_confirmSize_ASCII(size_code, &width_ASCII, &height_ASCII);
	__MID_OLED_confirmSize_UTF8CN(size_code, &width_UTF8, &height_UTF8);

	// 显示
	MID_OLED_showAnyString_UTF8(h_oled, matrix_source_ASCII, width_ASCII, height_ASCII,
								size_code, width_UTF8, height_UTF8,
								x, y, str);
}

void MID_OLED_showImage(MID_OLED_Handle_t *h_oled, uint8_t x, uint8_t y, const uint8_t *img, uint8_t img_width, uint8_t img_height)
{
	MID_OLED_showAnyImage(h_oled, x, y, img, img_width, img_height);
}

void MID_OLED_drawLine(MID_OLED_Handle_t *h_oled, int8_t X1, int8_t Y1, int8_t X2, int8_t Y2, uint8_t color)
{
	MID_OLED_drawAnyLine(h_oled, X1, Y1, X2, Y2, color);
}

// 输入左上角坐标
void MID_OLED_drawSquareFrame(MID_OLED_Handle_t *h_oled, int8_t x_start, int8_t y_start, uint8_t width, uint8_t height)
{
	// 不合理检测
	if(width < 1 || height < 1) return;

	// 宽度为1 则+0，显示像素本身宽度
	width--; height--;

	MID_OLED_drawAnyLine(h_oled, x_start, y_start, x_start, y_start + height, MID_OLED_LIGHT);
	MID_OLED_drawAnyLine(h_oled, x_start, y_start, x_start + width, y_start, MID_OLED_LIGHT);
	MID_OLED_drawAnyLine(h_oled, x_start + width, y_start + height, x_start, y_start + height, MID_OLED_LIGHT);
	MID_OLED_drawAnyLine(h_oled, x_start + width, y_start + height, x_start + width, y_start, MID_OLED_LIGHT);
}

/**
 * @brief 指定坐标画一个圆圈（当圆边突破OLED限制时只显示范围内的图像）
 * @param x_cen 坐标(0-127)
 * @param y_cen 坐标(0-63)
 * @param radius 圆的半径
 * @param line_thick 线条厚度（当 line_thick >= radius 时为实心圆）
 */
void MID_OLED_drawCircle(MID_OLED_Handle_t *h_oled, int8_t x_cen, int8_t y_cen, uint8_t radius, uint8_t line_thick)
{
	// 确定圆的范围(方形)
	if(!line_thick || !radius) return;
	int16_t x_left = x_cen - (radius -1), x_right = x_cen + (radius -1);
	int16_t y_up   = y_cen - (radius -1), y_down  = y_cen + (radius -1);

	// 范围检查(矩形)
	x_left   = (x_left > 127) ? 127  : (x_left < 0) ? 0  : x_left;
	x_right  = (x_right > 127) ? 127 : (x_right < 0) ? 0 : x_right;
	y_up     = (y_up > 63) ?    63   : (y_up < 0) ? 0    : y_up;
	y_down   = (y_down > 63) ?  63   : (y_down < 0) ? 0  : y_down;
	line_thick = (line_thick > radius) ? radius : line_thick;

	// 斜边平方
	int16_t extern_line_gap_pow = radius * radius;
	int16_t intern_line_gap_pow = (radius - line_thick) * (radius - line_thick);

	// 画圆
	for(int16_t y = y_up; y <= y_down; y++) {
		for(int16_t x = x_left; x <= x_right; x++) {

			// 在范围内则有效
			int16_t pow_sum = (x - x_cen)*(x - x_cen) + (y - y_cen)*(y - y_cen);
			if(intern_line_gap_pow <= pow_sum && pow_sum < extern_line_gap_pow)
				__MID_OLED_drawPoint(h_oled, x, y, 1);
		}
	}
}
