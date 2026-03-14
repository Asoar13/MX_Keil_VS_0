#ifndef __MID_CI_03T_H
#define __MID_CI_03T_H
#include "main.h"
#include "Serial/mid_serial.h"

//extern MID_Serial_Handle_t h_ci_03t;

uint8_t MID_CI_03T_isHead(uint8_t byte);
uint8_t MID_CI_03T_isFinished(uint8_t *p_data, uint16_t len);
uint16_t MID_CI_03T_packData(uint8_t *p_data, uint16_t len);

#endif /* __MID_CI_03T_H */
