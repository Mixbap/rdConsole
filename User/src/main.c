#include "main.h"
//------------------------------------------------------------
//------------------------------------------------------------	 
int main(void)
{
	CLK_80_ini();
	LED_PortB_ini();
	PortB_EXT_ini();
	
	Modulator_ini();
	TIMER_CAPTURE_ini();
	
	NVIC_EnableIRQ(EXT_INT2_IRQn);
	
	while(1)
	{
		if (flagConsole)
		{
			Modulator_ini();
			TIMER_CAPTURE_ini();
			NVIC_EnableIRQ(EXT_INT2_IRQn);
		}
			
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



