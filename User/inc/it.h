#ifndef IT_H
#define IT_H

#include "ini.h"
//#include "console.h"

//extern uint16_t RD;
extern uint8_t Flag_IRQ;
extern uint8_t firstIrq;

void DMA_IRQHandler(void);
void EXT_INT2_IRQHandler(void);

#endif
