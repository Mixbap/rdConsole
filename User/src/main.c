#include "main.h"


int main(void)
{
	rdParamDefIni();		// Инициализация параметров блока по умолчанию
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
		
		if (param.mode == 1)
		{
			checkLimits(); // Проверка попадания в пороги (выдача ИК)
		}
		else if (param.mode == 2)
		{
			//checkLimits(); // Проверка попадания в пороги (выдача ИК)
			distanceMode();  // Режим измерения дальности
		}

	}
}

//------------------------------------------------------------
// Проверка попадания в пороги (выдача ИК)
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
	if (RD > param.freqBw0)
	{
		if (RD < param.freqBw1)
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
	if (RD > param.freqBw0)
		LED0_ON;
			
	if (RD < param.freqBw1)
		LED1_ON;
	
	// Проверка и индикация порога накопления
	if (n == param.limitAcc)
	{
		LED3_ON;
		LED2_OFF;
		n = 0;
	}

}

//------------------------------------------------------------
// Режим измерения дальности
//------------------------------------------------------------
void distanceMode(void)
{
	uint32_t i;
	uint32_t result = 0;
	// Проверка выхода из прерывания
	if (Flag_IRQ == 1)
	{
		RD = RD_TIMER->CNT;
		RD_TIMER->CNT = 0;
		Flag_IRQ = 0;

		// Заполнение окна
		distanceArr[indDistance] = RD;
		indDistance++;
		if (indDistance == DISTANCE_VALUE)
			indDistance = 0;
		
		// Вычисление дальности
		for (i = 0; i < DISTANCE_VALUE; i++)
		{
			result = result + distanceArr[i]; 
		}
		distance = (int)(result/DISTANCE_VALUE);
	}	
}



















