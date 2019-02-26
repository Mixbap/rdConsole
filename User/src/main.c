#include "main.h"


int main(void)
{
	rdParamDefIni();		// ������������� ���������� ����� �� ���������
	CLK_80_ini(); 			// ������������� CLK = 80 MHz �� HSE
	LED_PortB_ini();		// ������������� LED (PB0, PB1, PB2, PB3)
	PortB_EXT_ini();		// ������������� �������� ���������� �� PB10 (XS9 23 pin)
	
	Modulator_ini();		// ������������� ����������
	TIMER_CAPTURE_ini();// ������������� ������� � ������ ������� (PC2 XS9 26 pin)
	
	NVIC_EnableIRQ(EXT_INT2_IRQn); // ���������� ������� ���������� �� PB10 (XS9 23 pin)  
	
	while(1)
	{
		if (flagConsole)
		{
			Modulator_ini(); 								// ������������� ����������
			TIMER_CAPTURE_ini(); 						// ������������� ������� � ������ ������� (PC2 XS9 26 pin)
			NVIC_EnableIRQ(EXT_INT2_IRQn); 	// ���������� ������� ���������� �� PB10 (XS9 23 pin) 
		}
		
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
		RD = RD_TIMER->CNT;
		RD_TIMER->CNT = 0;
		Flag_IRQ = 0;
		LED0_OFF;
		LED1_OFF;
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
	if (RD > param.freqBw0)
		LED0_ON;
			
	if (RD < param.freqBw1)
		LED1_ON;
	
	// �������� � ��������� ������ ����������
	if (n == param.limitAcc)
	{
		LED3_ON;
		LED2_OFF;
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
		RD = RD_TIMER->CNT;
		RD_TIMER->CNT = 0;
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
	}	
}



















