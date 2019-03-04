#include "lcd.h"

//------------------------------------------------------------
// ������������� LCD
//------------------------------------------------------------
void LCD_ini(void)
{
	const char group[] = "    \xCD\xC8\xC3 \x33\x33\x37\x32"; // "��� 3372" 
	const char name[] = "  \xD0\xC0\xC4\xC8\xCE\xC4\xC0\xD2\xD7\xC8\xCA"; //"�����������"
	
	U_MLT_Init();
	
	U_MLT_Put_String(group, 4);
	
	U_MLT_Put_String (name, 6);
}

//------------------------------------------------------------
// ����� ������ ������
//------------------------------------------------------------
void LCD_mode_print(uint8_t mode)
{
	const char printModeSenser[] = "\xD0\xE5\xE6\xE8\xEC\x3A \xC4\xC0\xD2\xD7\xC8\xCA"; // "�����: ������"
	const char printModeDist[] = "\xD0\xE5\xE6\xE8\xEC\x3A \xC4\xC0\xCB\xDC\xCD\xCE\xCC\xC5\xD0"; // "�����: ���������"
	
	// �������� ������� 1 
	U_MLT_Clear_Chip (1);
	// �������� ������� 2
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
// ����� ���������
//------------------------------------------------------------
void LCD_distance_print(uint8_t distanceValue)
{
	uint8_t i = 0;
	uint8_t idx = 0;
	int ceil;
	int active = 1;
	
	// ������ ������
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
	
	// ����� ���������
	for (i = 0; i < idx; i++)
	{
		ceil = (int)distanceValue/(active/10);
		active = active/10;
		distanceValue = distanceValue - (ceil * active);
		U_MLT_Put_Char(ceil + 0x30, i + 5, 3);
	}
}






