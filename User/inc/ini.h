#ifndef INI_H
#define INI_H

#include "MDR32Fx.h"                    // Device header
#include "MDR32F9Qx_config.h"           // Keil::Device:Startup_MDR1986BE9x
#include "MDR32F9Qx_rst_clk.h"          // Keil::Drivers:RST_CLK
#include "MDR32F9Qx_port.h"             // Keil::Drivers:PORT
#include "MDR32F9Qx_dac.h"              // Keil::Drivers:DAC
#include "MDR32F9Qx_timer.h"            // Keil::Drivers:TIMER
#include "MDR32F9Qx_dma.h"              // Keil::Drivers:DMA

//------------------------------------------------------------
#define LED0_ON		PORT_SetBits(MDR_PORTB, PORT_Pin_0)
#define LED1_ON		PORT_SetBits(MDR_PORTB, PORT_Pin_1)
#define LED2_ON		PORT_SetBits(MDR_PORTB, PORT_Pin_2)
#define LED3_ON		PORT_SetBits(MDR_PORTB, PORT_Pin_3)

#define LED0_OFF	PORT_ResetBits(MDR_PORTB, PORT_Pin_0)
#define LED1_OFF	PORT_ResetBits(MDR_PORTB, PORT_Pin_1)
#define LED2_OFF	PORT_ResetBits(MDR_PORTB, PORT_Pin_2)
#define LED3_OFF	PORT_ResetBits(MDR_PORTB, PORT_Pin_3)
//------------------------------------------------------------

#define MODULATOR_BUFF 32
#define MODULATOR_FREQ 100000
#define MODULATOR_PERIOD(F)  ((uint16_t)((uint32_t)(80000000) / (uint32_t)(F * MODULATOR_BUFF)) - 1)
#define MODULATOR_AMPLITUDE 2047
#define MODULATOR_CONST 2047

#define RD_PORT	    MDR_PORTC					
#define RD_Pin		  PORT_Pin_2        
#define RD_TIMER	MDR_TIMER3 				
#define RD_CHN	  TIMER_CHANNEL1

#define POROG_NAKOPLENIE  10
#define POROG_MAX 105
#define POROG_MIN 95


void DAC2_PortE_ini(void);
void LED_PortB_ini(void);
void CLK_80_ini(void);
void DAC2_ini(void);
void Sin_massiv(void);
void DMA_DAC2_ini(void);
void TIMER1_ini(void);
void Modulator_ini(void);
void TIMER_CAPTURE_ini(void);


extern DMA_CtrlDataInitTypeDef DMA_InitStructure;
extern DMA_ChannelInitTypeDef DMA_Channel_InitStructure;


#endif