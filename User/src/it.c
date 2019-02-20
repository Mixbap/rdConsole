#include "it.h"

//------------------------------------------------------------
// Обработчик прерываний от DMA
//------------------------------------------------------------
void DMA_IRQHandler (void)
{
	Flag_IRQ = 1;
  DMA_InitStructure.DMA_CycleSize = param.bufMode;
  DMA_Init (DMA_Channel_TIM1, &DMA_Channel_InitStructure);
}

//------------------------------------------------------------
// Обработчик внешних прерываний EXT_INT2
//------------------------------------------------------------
void EXT_INT2_IRQHandler(void)
{
	if (firstIrq)
	{
		LED3_ON;
		runConsole();
	}
	
	firstIrq = 1;
	LED2_ON;
}
