#include "it.h"

void DMA_IRQHandler (void)
{
	//RD = RD_TIMER->CNT;
	Flag_IRQ = 1;
	
	//uint16_t i;
	//for (i = 0;i < 200; i++) {}
  DMA_InitStructure.DMA_CycleSize = MODULATOR_BUFF;
  DMA_Init (DMA_Channel_TIM1, &DMA_Channel_InitStructure);
}

