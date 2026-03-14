#ifndef __BSP_TOOL_H
#define __BSP_TOOL_H
#include "main.h"

#define BSP_Tool_GETRANDNUM(min, max)		   \
	( ((max) > (min)) ? 					   \
	  (min) + (rand()) % ((max) - (min) + 1) : \
	  (min) )

void BSP_Tool_Time_msIntoTime(uint32_t *h, uint32_t *m, uint32_t *s, uint32_t ms);
void BSP_Tool_Time_sIntoTime(uint32_t *d, uint32_t *h, uint32_t *m, uint32_t s);

uint8_t BSP_Tool_Str_searchChar(char *str, uint8_t n, char ch);
void BSP_Tool_Str_insertSrcInDest(char *dest_str, uint8_t insert_pox, char* src_str, uint8_t src_len);
void BSP_Tool_Str_replaceDestBySrc(char *dest_str, uint8_t replaced_pox, uint8_t replaced_len,
								   char* src_str, uint8_t src_len);
void BSP_Tool_Str_transfMathExpre(char *expression, int X1, char *sign_str, int X2, int Y);

void BSP_Tool_Rand_initSeed();
char BSP_Tool_Rand_getAddtion(int *X1, int *X2, int sum, int min, int max);
char BSP_Tool_Rand_getSubtruction(int *X1, int *X2, int diff, int min, int max);
char BSP_Tool_Rand_getMutiplication(int *X1, int *X2, int pord, int min, int max);
char BSP_Tool_Rand_getDivision(int *X1, int *X2, int quot, int min, int max);
void BSP_Tool_Rand_getCombineExpre(int Y, uint8_t n, char *expression);

#endif
