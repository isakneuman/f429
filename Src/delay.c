#include "delay.h"

__IO	uint32_t	ms	=	0;


void  Delay (uint32_t nCount)
{
	uint32_t curr = ms;
	while((ms-curr)<nCount);
}



void init_delay(void)
{
	RCC_ClocksTypeDef  rcc_clocks;
	RCC_GetClocksFreq(&rcc_clocks);
	uint32_t pclk1 = rcc_clocks.PCLK1_Frequency;
	SysTick_Config(pclk1/1000);
	NVIC_EnableIRQ(SysTick_IRQn);
}

void SysTick_Handler(void)
{
	ms++;
}
