#ifndef __APP_ADJUSTABLEFANS_H
#define __APP_ADJUSTABLEFANS_H
#include "main.h"

void APP_AdjustableFans_init();
void APP_AdjustableFans_process(uint8_t *p_cnt, uint8_t *p_times);
void APP_AdjustaleFans_AAPP_AdjustableFans_processDC_DMA_IRQ();
void APP_AdjustaleFans_ADC_DMA_IRQ();

#endif
