// ***********************************************************************************
// Микроконтроллер: K1986BE92QI
// Устройство: Блок обработки радиолокационного датчика (дальномера)
// Файл: lcd.c 
// Назначение: Обертка для работы с ЖКИ
// Компилятор:  Armcc 5.06 update 3 из комплекта Keil uVision 5.21.1.0 
// ***********************************************************************************
#include "lcd.h"
#include "mlt_lcd.h"
#include "math.h"
//------------------------------------------------------------
// Инициализация LCD
//------------------------------------------------------------
void LCD_ini(void)
{
	const char group[] = "    \xCD\xC8\xC3 \x33\x33\x37\x32"; // "НИГ 3372" 
	const char name[] = "  \xD0\xC0\xC4\xC8\xCE\xC4\xC0\xD2\xD7\xC8\xCA"; //"РАДИОДАТЧИК"
	
	U_MLT_Init();
	
	U_MLT_Put_String(group, 4);
	
	U_MLT_Put_String (name, 6);
}

//------------------------------------------------------------
// Вывод режима работы
//------------------------------------------------------------
void LCD_mode_print(uint8_t mode)
{
	const char printModeSenser[] = "\xD0\xE5\xE6\xE8\xEC\x3A \xC4\xC0\xD2\xD7\xC8\xCA"; // "Режим: ДАТЧИК"
	const char printModeDist[] = "\xD0\xE5\xE6\xE8\xEC\x3A \xC4\xC0\xCB\xDC\xCD\xCE\xCC\xC5\xD0"; // "Режим: ДАЛЬНОМЕР"
	
	// Очистить дисплей 1 
	U_MLT_Clear_Chip (1);
	// Очистить дисплей 2
	U_MLT_Clear_Chip (2);
	
	if (mode == 1)
	{
		U_MLT_Put_String(printModeSenser, 1);
	}
	else if (mode == 2)
	{
		U_MLT_Put_String(printModeDist, 1);
	}

}

//------------------------------------------------------------
// Вывод дальности
//------------------------------------------------------------
void LCD_distance_print(uint8_t distanceValue)
{
	uint8_t i = 0;
	uint8_t idx = 0;
	int ceil;
	int active = 1;
	
	// Размер данных
	while (!idx)
	{
		ceil = (int)(distanceValue/active);
		if (ceil == 0)
			idx = i;
		else
		{
			i++;
			active = active * 10;
		}
	}
	
	// Вывод дальности
	for (i = 0; i < idx; i++)
	{
		ceil = (int)distanceValue/(active/10);
		active = active/10;
		distanceValue = distanceValue - (ceil * active);
		U_MLT_Put_Char(ceil + 0x30, i + 5, 3);
	}
}






