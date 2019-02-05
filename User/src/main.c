#include "main.h"


int main(void)
{
	CLK_80_ini(); 			// Инициализация CLK = 80 MHz от HSE
	LED_PortB_ini();		// Инициализация LED (PB0, PB1, PB2, PB3)
	PortB_EXT_ini();		// Инициализация внешнего прерывания по PB10 (XS9 23 pin)
	
	Modulator_ini();		// Инициализация модулятора
	TIMER_CAPTURE_ini();// Инициализация таймера в режиме захвата (PC2 XS9 26 pin)
	
	NVIC_EnableIRQ(EXT_INT2_IRQn); // Разрешение внешних прерываний по PB10 (XS9 23 pin)  
	
	while(1)
	{
		if (flagConsole)
		{
			Modulator_ini(); 								// Инициализация модулятора
			TIMER_CAPTURE_ini(); 						// Инициализация таймера в режиме захвата (PC2 XS9 26 pin)
			NVIC_EnableIRQ(EXT_INT2_IRQn); 	// Разрешение внешних прерываний по PB10 (XS9 23 pin) 
		}
		
		checkLimits(); // Проверка попадания в пороги
	}
}

//------------------------------------------------------------
// Проверка попадания в пороги
//------------------------------------------------------------
void checkLimits(void)
{
	// Проверка выхода из прерывания
	if (Flag_IRQ == 1)
	{
		RD = RD_TIMER->CNT;
		RD_TIMER->CNT = 0;
		Flag_IRQ = 0;
		LED0_OFF;
		LED1_OFF;
	}
	
	// Проверка попадания в пороги
	if (RD > POROG_MIN)
	{
		if (RD < POROG_MAX)
			n++;
		else
		{
			if (n>0)
				n--;
		}
	}
	else
	{
		if (n>0)
			n--;
	}
		
	// Индикация попадания в пороги
	if (RD > POROG_MIN)
		LED0_ON;
			
	if (RD < POROG_MAX)
		LED1_ON;
	
	// Проверка и индикация порога накопления
	if (n == POROG_NAKOPLENIE)
	{
		LED3_ON;
		LED2_OFF;
		n = 0;
	}
}


