#ifndef __MID_DFPLAYERMINI_H
#define __MID_DFPLAYERMINI_H
#include "main.h"
#include "Serial/mid_serial.h"

uint8_t MID_DFPlayer_mini_isHead(uint8_t byte);
uint8_t MID_DFPlayer_mini_isFinished(uint8_t *p_data, uint16_t len);
uint16_t MID_DFPlayer_mini_packData(uint8_t *p_data, uint16_t len);

#endif
