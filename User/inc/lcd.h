// ***********************************************************************************
// ���������������: K1986BE92QI
// ����������: ���� ��������� ����������������� ������� (����������)
// ����: lcd.h 
// ����������: ������� ��� ������ � ���
// ����������:  Armcc 5.06 update 3 �� ��������� Keil uVision 5.21.1.0 
// ***********************************************************************************

#ifndef LCD_H
#define LCD_H

#include "mlt_lcd.h"
#include "math.h"

void LCD_ini(void);
void LCD_mode_print(uint8_t mode);
void LCD_distance_print(uint8_t distanceValue);

#endif
