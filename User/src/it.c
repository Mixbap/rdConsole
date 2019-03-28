// ***********************************************************************************
// Микроконтроллер: K1986BE92QI
// Устройство: Блок обработки радиолокационного датчика (дальномера)
// Файл: it.c 
// Назначение: Обработчик прерываний 
// Компилятор:  Armcc 5.06 update 3 из комплекта Keil uVision 5.21.1.0 
// ***********************************************************************************
#include "it.h"

//------------------------------------------------------------
// Обработчик прерываний от DMA
//------------------------------------------------------------
void DMA_IRQHandler (void)
{
	Flag_IRQ = 1;
  DMA_InitStructure.DMA_CycleSize = param.bufMode;
  DMA_Init (DMA_Channel_TIM1, &DMA_Channel_InitStructure);
	
	RD = RD_TIMER->CNT;
	RD_TIMER->CNT = 0;

}

//------------------------------------------------------------
// Обработчик внешних прерываний EXT_INT2
//------------------------------------------------------------
void EXT_INT2_IRQHandler(void)
{
	if (firstIrq)
	{
		runConsole();
	}
	
	firstIrq = 1;
}
