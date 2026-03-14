#ifndef __MID_PARCE_H
#define __MID_PARCE_H
#include "main.h"

#define MID_PARCE_MAX_CB_COUNT 10

typedef void (*Callback_Func) (uint8_t *p_data, uint8_t len);

typedef struct {
	uint8_t cmd;
	Callback_Func callback;
} MID_Parce_CallbackConf_t;

typedef struct {
	uint8_t callback_cnt;
	MID_Parce_CallbackConf_t callback_table[MID_PARCE_MAX_CB_COUNT];
} MID_Parce_CallbackTable;


uint8_t MID_Parce_registerCallback(MID_Parce_CallbackTable *callback_table, uint8_t cmd, Callback_Func callback);
uint8_t MID_Parce_parseAndDispatch(MID_Parce_CallbackTable *callback_table, uint8_t cmd, uint8_t *p_data, uint8_t len);

#endif
