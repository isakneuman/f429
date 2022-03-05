#include "dac.h"

DAC_InitTypeDef	dac;

void	init_speaker(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	GPIO_InitTypeDef dac2_pin;

	dac2_pin.GPIO_Mode	=	GPIO_Mode_AN;
	dac2_pin.GPIO_Pin	=	GPIO_Pin_5;
	dac2_pin.GPIO_PuPd	=	GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &dac2_pin);
	GPIO_SetBits(GPIOA, GPIO_Pin_5);
	DAC_StructInit(&dac);
	DAC_Init(DAC_Channel_2, &dac);
	DAC_Cmd(DAC_Channel_2, ENABLE);
}

void	sinewave_sound()
{
	for (int i=0; i<100; i++)
	{
		DAC_SetChannel2Data(DAC_Align_12b_R,(sin(i*2*PI/100) + 1)*(4096/2));
	}
}

