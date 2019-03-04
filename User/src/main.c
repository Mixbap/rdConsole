// ***********************************************************************************
// ���������������: K1986BE92QI
// ����������: ���� ��������� ����������������� ������� (����������)
// ����: main.c 
// ����������: ������ ������� ������ ����������������� ������� � ������������ ������������� �������  
// ����������:  Armcc 5.06 update 3 �� ��������� Keil uVision 5.21.1.0 
// ***********************************************************************************
 
#include "main.h"


int main(void)
{
	rdParamDefIni();		// ������������� ���������� ����� �� ���������
	CLK_80_ini(); 			// ������������� CLK = 80 MHz �� HSE
	
	#ifdef IK_PORT
		IK_port_ini();			// ������������� ����� ������ ��
	#endif
	
	#ifdef LED
		LED_PortB_ini();		// ������������� LED (PB0, PB1, PB2, PB3)
	#endif
	
	PortB_EXT_ini();		// ������������� �������� ���������� �� PB10 (XS9 23 pin)
	//LCD_ini();					// ������������� ���
	//LCD_mode_print(2);	// ����� ������ ������ �� ���
	//LCD_distance_print(52); // ����� ��������� �� ���
	
	Modulator_ini();		// ������������� ����������
	TIMER_CAPTURE_ini();// ������������� ������� � ������ ������� (PC2 XS9 26 pin)
	
	NVIC_EnableIRQ(EXT_INT2_IRQn); // ���������� ������� ���������� �� PB10 (XS9 23 pin)  
	
	while(1)
	{
		// �������� ������ �� ���������
		if (flagConsole)
		{
			Modulator_ini();								// ������������� ����������
			TIMER_CAPTURE_ini(); 						// ������������� ������� � ������ ������� (PC2 XS9 26 pin)
			NVIC_EnableIRQ(EXT_INT2_IRQn); 	// ���������� ������� ���������� �� PB10 (XS9 23 pin) 
			flagConsole = 0;								// ����� ����� ������ �� ��������� 
		}
			
		// �������� ���������� ������ ������
		if (param.mode == 1)
		{
			checkLimits(); // �������� ��������� � ������ (������ ��)
		}
		else if (param.mode == 2)
		{
			//checkLimits(); // �������� ��������� � ������ (������ ��)
			distanceMode();  // ����� ��������� ���������
		}

	}
}

//------------------------------------------------------------
// �������� ��������� � ������ (������ ��)
//------------------------------------------------------------
void checkLimits(void)
{
	// �������� ������ �� ����������
	if (Flag_IRQ == 1)
	{
		Flag_IRQ = 0;
		
		#ifdef LED
			LED0_OFF();
			LED1_OFF();
		#endif
	}
		// �������� ��������� � ������
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

		// ��������� ��������� � ������
		#ifdef LED
			if (RD > param.freqBw0)
				LED0_ON();
			
			if (RD < param.freqBw1)
				LED1_ON();
		#endif
			
		// �������� � ��������� ������ ����������
		if (n == param.limitAcc)
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
			n = 0;
		}
}

//------------------------------------------------------------
// ����� ��������� ���������
//------------------------------------------------------------
void distanceMode(void)
{
	uint32_t i;
	uint32_t result = 0;
	// �������� ������ �� ����������
	if (Flag_IRQ == 1)
	{
		Flag_IRQ = 0;
		
		// ���������� ����
		distanceArr[indDistance] = RD;
		indDistance++;
		if (indDistance == DISTANCE_VALUE)
			indDistance = 0;
		
		// ���������� ���������
		for (i = 0; i < DISTANCE_VALUE; i++)
		{
			result = result + distanceArr[i]; 
		}
		distance = (int)(result/DISTANCE_VALUE);
		/*
		// ����� �� ���
		if (countStartLCD > 5000)
		{
			flagConsole = 1;
			countStartLCD = 0;
			LCD_mode_print(2);	// ����� ������ ������ �� ���
			LCD_distance_print(distance); // ����� ��������� �� ���
		}
		else
		{
			countStartLCD++;
		}
*/
	}	
}



















