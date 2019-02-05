#include "console.h"

void test(void)
{
	DMA_ini(); 							// Инициализация DMA
	UART1_DMA_TX_PB5_ini(); // Инициализация UART1+DMA TX (XS9 18 pin)
	
	DMA_TX_start(buf1, 20);
	DMA_TX_start(buf2, 20);
	DMA_RX_start(buf0, 3); // Ввод 3 значений (не меньше не больше), можно меньше но добить пробелами
	DMA_TX_start(buf0, 20);
	DMA_TX_start(buf3, 20);
		
	UART_Cmd(MDR_UART1, DISABLE); // для нормальной работы ЦАП и ДМА
	flagConsole = 1;
}

