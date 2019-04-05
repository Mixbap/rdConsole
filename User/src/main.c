//--------------------------------------------------------------------------------------------------------
// Микроконтроллер: K1986BE92QI
// Устройство: Блок обработки радиолокационного датчика (дальномера)
// Файл: main.c 
// Назначение: Анализ сигнала биений радиолокационного датчика и формирование модулирующего сигнала  
// Компилятор:  Armcc 5.06 update 3 из комплекта Keil uVision 5.21.1.0 
//--------------------------------------------------------------------------------------------------------
#include "ini.h"
#include "it.h"
#include "lcd.h"
#include "console.h"
#include "main.h"
#include "config.h"
#define DISTANCE_VALUE 3

uint32_t countStartLCD = 0;
uint16_t distanceArr[DISTANCE_VALUE];
uint8_t indDistance = 0;
int distance = 0;

int main(void)
{
	uint32_t n = 0;

	rdParamDefIni(&param);		// Инициализация параметров блока по умолчанию
	CLK_80_ini(); 			// Инициализация CLK = 80 MHz от HSE
	
	#ifdef IK_PORT
		IK_port_ini();			// ������������� ����� ������ ��
	#endif
	
	#ifdef LED
		LED_PortB_ini();		// Инициализация LED (PB0, PB1, PB2, PB3)
	#endif
	
	PortB_EXT_ini();		// Инициализация внешнего прерывания по PB10 (XS9 23 pin)
	//LCD_ini();					// Инициализация ЖКИ
	//LCD_mode_print(2);	// Вывод режима работы на ЖКИ
	//LCD_distance_print(52); // Вывод дальности на ЖКИ
	
	Modulator_ini(&param);	// Инициализация модулятора
	TIMER_CAPTURE_ini();// Инициализация таймера в режиме захвата (PC2 XS9 26 pin)
	
	NVIC_EnableIRQ(EXT_INT2_IRQn); // Разрешение внешних прерываний по PB10 (XS9 23 pin)  
	
	while(1)
	{
		while (!flagConsole && !Flag_IRQ)
		{
			; // Бесконечный цикл ожидания изменения одного из флагов
		}
		// Проверка выхода из терминала
		if (flagConsole)
		{
			runConsole();
			Modulator_ini(&param);								// Инициализация модулятора
			TIMER_CAPTURE_ini(); 						// Инициализация таймера в режиме захвата (PC2 XS9 26 pin)
			NVIC_EnableIRQ(EXT_INT2_IRQn); 	// Разрешение внешних прерываний по PB10 (XS9 23 pin) 
			flagConsole = 0;							// Сброс флага выхода из терминала 
		}
		if (Flag_IRQ)	
		{
			Flag_IRQ = 0;
			if 	(param.mode == 1)
				checkLimits(RD, &n);// Проверка попадания в пороги (выдача ИК)
			else if (param.mode == 2)
				distanceMode(); // Режим измерения дальности
		}		
	}
}

//------------------------------------------------------------
// Проверка попадания в пороги (выдача ИК)
//------------------------------------------------------------
void checkLimits(int localRD, uint32_t *n_ptr)
{
	// Проверка попадания в пороги
	if (localRD > param.freqBw0 && localRD < param.freqBw1)
	{		
		(*n_ptr)++;
	}
	else if (*n_ptr > 0)
	{			
		(*n_ptr)--;
	}
	// Индикация попадания в пороги
	
	#ifdef LED
		// больше нижнего порога
		if (localRD > param.freqBw0)
			LED0_ON();
		else
			LED0_OFF();

		// меньше верхнего порога
		if (localRD < param.freqBw1)
			LED1_ON();
		else
			LED1_OFF();
	#endif
	
	// Проверка и индикация порога накопления
	if (*n_ptr == param.limitAcc)
	{
		#ifdef LED
			LED3_ON();
			LED2_OFF();
		#endif
		#ifdef IK_PORT
			IK_ON();
			delay(100000);
			IK_OFF();
		#endif
		*n_ptr = 0;
	}
}

//------------------------------------------------------------
// Режим измерения дальности
//------------------------------------------------------------
void distanceMode(void)
{
	uint32_t i;
	uint32_t result = 0;
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
	/*
	// Вывод на ЖКИ
	if (countStartLCD > 5000)
	{
		flagConsole = 1;
		countStartLCD = 0;
		LCD_mode_print(2);	// Вывод режима работы на ЖКИ
		LCD_distance_print(distance); // Вывод дальности на ЖКИ
	}
	else
	{
		countStartLCD++;
	}
	*/	
}



















