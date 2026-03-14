#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <Tool/bsp_tool.h>

/**************************************************************************************************************************/

void BSP_Tool_Time_msIntoTime(uint32_t *h, uint32_t *m, uint32_t *s, uint32_t ms)
{
	*s = (ms) / 1000;

	*m = (*s) / 60;
	*s = (*s) % 60;

	*h = (*m) / 60;
	*m = (*m) % 60;
}

void BSP_Tool_Time_sIntoTime(uint32_t *d, uint32_t *h, uint32_t *m, uint32_t s)
{
	*m = (s) / 60;

	*h = (*m) / 60;
	*m = (*m) % 60;

	*d = (*h) / 24;
	*h = (*h) % 24;
}

/**************************************************************************************************************************/

// 在str里定位第n个ch
uint8_t BSP_Tool_Str_searchChar(char *str, uint8_t n, char ch)
{
	uint8_t i;

	// 边缘检测
	if(n <= 0 || ch == '\0' || str == NULL) return 0;

	for(i = 0; str[i] != '\0'; i++) {
		if(str[i] == ch) {
			n--;

			// 第n次进入时就触发
			if(!n) {
				return i;
			}
		}
	}
	return 0;
}

// 在dest_str字符串的insert_pox位置插入src_len长的src_str字符串
void BSP_Tool_Str_insertSrcInDest(char *dest_str, uint8_t insert_pox, char* src_str, uint8_t src_len)
{
	// dest_str = dest_store1 + dest_store2; src_str
	// dest_store2后移src_len，留出空位
	memmove(dest_str + insert_pox + src_len,	// 后退后的第一位
			dest_str + insert_pox,				// dest_store2的第一位
			strlen(dest_str + insert_pox) +1);	// dest_store2的长度 加 '\0'

	// dest_str = dest_store1 + rubish + dest_store2; src_str
	// src_str覆盖rubish
	memcpy(dest_str + insert_pox,	// rubish第一位
		   src_str,					// src_str的第一位
		   src_len);				// src_str的长度
}

// 将dest_str字符串的replaced_pox位置开始的的replaced_len长度换做src_len长的src_str字符串
void BSP_Tool_Str_replaceDestBySrc(char *dest_str, uint8_t replaced_pox, uint8_t replaced_len,
		char* src_str, uint8_t src_len)
{
	// dest_str = dest_store1 + dest_replaced + dest_store2; src_str
	// dest_store2前移replaced_len覆盖dest_replaced
	memmove(dest_str + replaced_pox,							// dest_replaced第一位
			dest_str + replaced_pox + replaced_len,				// dest_store2第一位
			strlen(dest_str + replaced_pox + replaced_len) +1);	// dest_store2长度 加 '\0'

	// dest_str = dest_store1 + dest_store2; src_str
	// 在dest_store1后插入src_str
	BSP_Tool_Str_insertSrcInDest(dest_str, replaced_pox, src_str, src_len);
}

void BSP_Tool_Str_transfMathExpre(char *expression, int X1, char *sign_str, int X2, int Y)
{
	sprintf(expression, "%d%s%d = __", X1, sign_str, X2);
}

/**************************************************************************************************************************/

static void _Rand_updateNumArr(int *num_arr, uint8_t len, uint8_t No_x, int X1, int X2);
static void _Rand_updateSignArr(char *sign_arr, uint8_t No_x, char sign);
static void _Rand_ExpendExpre(char *sign_arr, int *num_arr, uint8_t n);

// 随机计算工具组
static char (*rand_arithmetic_arr[4]) (int*, int*, int, int, int) = {
		BSP_Tool_Rand_getAddtion,
		BSP_Tool_Rand_getSubtruction,
		BSP_Tool_Rand_getMutiplication,
		BSP_Tool_Rand_getDivision,
};

void BSP_Tool_Rand_initSeed()
{
	  srand(HAL_GetTick());
}

char BSP_Tool_Rand_getAddtion(int *X1, int *X2, int sum, int min, int max)
{
	// X1 + X2 = sum;
	while(1) {
		*X1 = BSP_Tool_GETRANDNUM(min, max);
		*X2 = sum - (*X1);
		if(*X2 >= min && *X2 <= max) break;
	}
	return '+';
}

char BSP_Tool_Rand_getSubtruction(int *X1, int *X2, int diff, int min, int max)
{
	// X1 - X2 = diff;
	while(1) {
		*X2 = BSP_Tool_GETRANDNUM(min, max);
		*X1 = (*X2) + diff;
		if(*X1 >= min && *X1 <= max) break;
	}
	return '-';
}

char BSP_Tool_Rand_getMutiplication(int *X1, int *X2, int pord, int min, int max)
{
	// X1 * X2 = sum;
	while(1) {
		*X1 = BSP_Tool_GETRANDNUM(min, max);
		if(*X1 == 0) continue;
		*X2 = pord / (*X1);
		if(*X2 >= min && *X2 <= max && (*X1) * (*X2) == pord) break;
	}
	return '*';
}

char BSP_Tool_Rand_getDivision(int *X1, int *X2, int quot, int min, int max)
{
	// X1 / X2 = sum;
	while(1) {
		*X2 = BSP_Tool_GETRANDNUM(min, max);
		*X1 = (*X2) * quot;
		if(*X1 >= min && *X1 <= max && *X2 != 0) break;
	}
	return '/';
}

#include <OLED/mid_oled.h>
// 拼凑一个有效数字为n个，计算结果为Y的算式expression
void BSP_Tool_Rand_getCombineExpre(int Y, uint8_t n, char *expression)
{
	// 符号和数字拆分，避免ASCII影响数字计算
	char sign_arr[128 / 6 + 1] = " _";	// 保证不越界
	int num_arr[128 / 6 / 2] = {Y};	// 保证不越界
	uint8_t real_count = 1;

	// 多次延长算式expression
	for(uint8_t i = 1; i < n; i++) {	// 已近有一位，i从1开始

		// 符号数组 => [((_/_)+(_*_))] ("_"数字占位，"?"确定符号)
		// [ _ ] => [ (_+_) ] => [ ((_/_)+_) ] => [ ((_/_)+(_*_)) ]
		// (每次更新都随机把"_"转化为"(_?_)"), (如果拆分后的"?"级别高于拆分前"_"旁更高级的"?"则不需要"()")
		// 数字数组 => [6, 2, 2, 2]
		// [ 7 ] => [ 3, 4 ] => [ 6, 2, 4 ] => [ 6, 2, 2, 2 ]
		// (数字被随机规则拆分，规则确定符号数组的运算符)
		_Rand_ExpendExpre(sign_arr, num_arr, real_count++);
	}

	// 数字数组和符号数组的融合
	// 每个被占位都被对应数字填充
	for(uint8_t i = 1; i <= real_count; i++) {
		char num_str[2 + 1];
		uint8_t mark_pos = BSP_Tool_Str_searchChar(sign_arr, 1, '_');
		sprintf(num_str, "%d", num_arr[i - 1]);				// 生成num_str
		uint8_t num_len = (num_arr[i - 1] < 10) ? 1 : 2;	// 长度不确定
		BSP_Tool_Str_replaceDestBySrc(sign_arr, mark_pos, 1, num_str, num_len);		// 数字替换占位'_'
	}

	// 只copy字符串sign_arr的空格后的部分
	uint8_t i;
	for(i = 0; i < 128 / 6 + 1 && sign_arr[i] == ' '; i++);
	memcpy(expression, sign_arr + i, 128 / 6 + 1 - i);
}

// 随机把已知有效长度为n的数字和符号数组中的一个成员拆分成两个成员
static void _Rand_ExpendExpre(char *sign_arr, int *num_arr, uint8_t n)
{
	// 随机一个数字，作为目标的序号
	uint8_t i = BSP_Tool_GETRANDNUM(1, n);

	// 找到第i个数
	int now_num = num_arr[i - 1];

	// 随机得到一个简单算式
	int X1, X2;
	char sign = rand_arithmetic_arr[BSP_Tool_GETRANDNUM(0, 3)](&X1, &X2, now_num, 0, 99);

	// 更新num_arr 和 sign_arr
	_Rand_updateNumArr(num_arr, n, i, X1, X2);
	_Rand_updateSignArr(sign_arr, i, sign);
}

// 将长度为len的num_arr中的第No_x个数字替换为两数字X1、X2	  [ 6, 2, 4 ] => [ 6, 2, 2, 2 ]
static void _Rand_updateNumArr(int *num_arr, uint8_t len, uint8_t No_x, int X1, int X2)
{
	uint8_t replaced_index = No_x - 1;	//将排位转换为下标

	// 先放X1
	num_arr[replaced_index] = X1;	//第No_x位

	// 处理X2（倒数第一位向后复制扩展有效长度，前面的依次继续复制，直到遇到第No_x位的X1）
	for(uint8_t i = len - 1; i > replaced_index; i--) {
		num_arr[i + 1] = num_arr[i];
	}
	num_arr[replaced_index + 1] = X2;	// 第No_x + 1位
}

// 将sign_arr中第No_x个"-"标记所在位拆分为符号组合	[ ((_/_)+_) ] => [ ((_/_)+(_*_))
static void _Rand_updateSignArr(char *sign_arr, uint8_t No_x, char sign)
{
	// 寻找位置
	uint8_t mark_pox = BSP_Tool_Str_searchChar(sign_arr, No_x, '_');

	// 规则判断
	// 需要括号 <= 自己是+-左右边有/或*  自己是+-左边有-  自己是*/左边有/
	uint8_t lower_mark = 0;		// 低级标志，需要括号
//	if(No_x == 1) {
//		lower_mark = 0;	// 如果在最左边就不需要括号了
//	} else {
		switch(sign) {
		case '+':
		case '-':
			// 左边有"-"
			if(sign_arr[mark_pox - 1] == '-') lower_mark = 1;
			// 左右边有"*"
			if(sign_arr[mark_pox - 1] == '*' || sign_arr[mark_pox + 1] == '*') lower_mark = 1;
			// 右边有"/"
			if(sign_arr[mark_pox + 1] == '/') lower_mark = 1;
		case '/':
		case '*':
			// 左边有"/"
			if(sign_arr[mark_pox - 1] == '/') lower_mark = 1;
		}
//	}

	// 生成有无括号的符号组
	char signs_str[5 + 1]; uint8_t signs_len = 3;
	if(lower_mark) {
		sprintf(signs_str, "(_%c_)", sign);
		signs_len = 5;
	} else {
		sprintf(signs_str, "_%c_", sign);
		signs_len = 3;
	}

	// 替换扩增
	BSP_Tool_Str_replaceDestBySrc(sign_arr, mark_pox, 1, signs_str, signs_len);
}





