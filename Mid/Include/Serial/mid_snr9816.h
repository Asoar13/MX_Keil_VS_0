#ifndef __MID_SNR9816_H
#define __MID_SNR9816_H
#include "main.h"
#include "Serial/mid_serial.h"

#define NOW_TEMP_IS "\xCF\xD6\xD4\xDA\xCE\xC2\xB6\xC8\xCA\xC7"
#define DEGREES		"\xB6\xC8"

//extern MID_Serial_Handle_t h_snr9816;

uint16_t MID_SNR9816_packData(uint8_t *p_data, uint16_t len);


#endif /* __MID_SNR9816_H */
