#include "main.h"
//------------------------------------------------------------
//------------------------------------------------------------	 
int main(void)
{
	CLK_80_ini();
	LED_PortB_ini();
	Modulator_ini();
	TIMER_CAPTURE_ini();
	
	//DMA_ini(); 							// Инициализация DMA
	//UART1_DMA_TX_PB5_ini(); // Инициализация UART1+DMA TX (XS9 18 pin)
	
	//DMA_TX_start(buf1, 20);
	//DMA_TX_start(buf2, 20);
	//DMA_RX_start(buf0, 3); // Ввод 3 значений (не меньше не больше), можно меньше но добить пробелами
	//DMA_TX_start(buf0, 20);
	//DMA_TX_start(buf3, 20);
	

	
	while(1)
	{
		
		if (Flag_IRQ == 1)
		{
			RD = RD_TIMER->CNT;
			RD_TIMER->CNT = 0;
			Flag_IRQ = 0;
			LED0_OFF;
			LED1_OFF;
		}
		
	//--------------------------------------------	
	if (RD > POROG_MIN)
	{
		if (RD < POROG_MAX)
		{
			n++;
		}
		else
		{
			if (n>0)
			{
				n--;
			}
		}
	}
	else
	{
		if (n>0)
		{
			n--;
		}
	}
	//--------------------------------------------	
		if (RD > POROG_MIN)
		{
			LED0_ON;
		}
		
		if (RD < POROG_MAX)
		{
			LED1_ON;
		}
	//------------------------------------------
		if (n == POROG_NAKOPLENIE)
		{
			LED3_ON;
			LED2_OFF;
			n = 0;
		}
	
	}
}
//------------------------------------------------------------
//------------------------------------------------------------
