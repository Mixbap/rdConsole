#include "it.h"

void DMA_IRQHandler (void)
{
	Flag_IRQ = 1;
	
  DMA_InitStructure.DMA_CycleSize = MODULATOR_BUFF;
  DMA_Init (DMA_Channel_TIM1, &DMA_Channel_InitStructure);
}

