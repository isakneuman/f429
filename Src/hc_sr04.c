#include "hc_sr04.h"

__IO	uint8_t		state	=	0;
__IO	uint32_t	result	=	0;
__IO	uint32_t	measured_time	=	0;

void init_timers_hc_sr04(void);

void TIM3_start(void);
void TIM3_stop(void);
void TIM1_start(void);
void TIM1_stop(void);

void init_trig(void);
void init_echo(void);

inline void TIM3_start(void)
{
	TIM3->CR1  |= TIM_CR1_CEN;
}

inline void TIM3_stop(void)
{
	TIM3->CR1  &= ~TIM_CR1_CEN;
}

inline void TIM1_start(void)
{
	TIM1->CR1  |= TIM_CR1_CEN;
}

inline void TIM1_stop(void)
{
	TIM1->CR1  &= ~TIM_CR1_CEN;
}


void init_hc_sr04(void)
{
	init_hc_sr04_gpio();
	init_timers_hc_sr04();
	state |= NEED_TRIG;
}

void init_trig(void)
{
	RCC->APB2ENR	|=	RCC_APB2ENR_TIM1EN;

	TIM1->PSC 	= 	uS_CALC(HSI_VALUE);
	TIM1->ARR 	= 	TRIGG_DURATION-1;
	TIM1->CCR1 	= 	1;
	TIM1->CR1  	|= 	TIM_CR1_OPM;
	TIM1->BDTR 	|= 	TIM_BDTR_MOE;
	TIM1->CCMR1	|=	0b111<<TIM_CCMR1_OC1M_Pos;
	TIM1->CCER 	|= 	(TIM_CCER_CC1E);
}

void init_echo(void)
{
	RCC->APB1ENR	|=	RCC_APB1ENR_TIM3EN;

	TIM3->PSC	=	uS_CALC(HSI_VALUE);
	TIM3->ARR	=	TOO_FAR_TIMEOUT-1;

	TIM3->CCMR1	|=	TIM_CCMR1_IC1F|
	 	 	 	 	TIM_CCMR1_IC2F|
					0b01<<TIM_CCMR1_CC1S_Pos|
					0b10<<TIM_CCMR1_CC2S_Pos;

	TIM3->SMCR 	|=	0b101<<TIM_SMCR_TS_Pos|
			 	 	0b100<<TIM_SMCR_SMS_Pos;

	TIM3->DIER	|=	TIM_DIER_CC1IE|
			 	 	TIM_DIER_CC2IE|
					TIM_DIER_UIE;

	TIM3->CCER	|= 	TIM_CCER_CC2P|
			 	 	TIM_CCER_CC1E|
			 	 	TIM_CCER_CC2E;
	NVIC_EnableIRQ(TIM3_IRQn);

	TIM3->CR1 	|= 	TIM_CR1_CEN;
}

void TIM3_IRQHandler(void)
{
	if( TIM3->SR & TIM_SR_CC1IF )
	{
		TIM3->SR &= ~TIM_SR_CC1IF;
		TIM3->SR &= ~TIM_SR_UIF;

		if(state & WAIT_START_ECHO)
		{
			state &= ~WAIT_START_ECHO;
			state |= START_ECHO;
		}
		else
		{
			measured_time = 0;
			state = WTF1|END_ECHO;
		}
	}
	if( TIM3->SR & TIM_SR_CC2IF )
	{
		if(state & START_ECHO)
		{
			measured_time = TIM3->CCR2;
			state &= ~START_ECHO;
			state |= END_ECHO;
		}
		else
		{
			TIM3->SR &= ~TIM_SR_CC2IF;
			measured_time = TOO_FAR_TIMEOUT;
			state = WTF2|END_ECHO;
		}

	}
	if( TIM3->SR & TIM_SR_UIF )
	{
		TIM3->SR &= ~TIM_SR_UIF;
		if(state & START_ECHO)
		{
			state &= ~START_ECHO;
		}
	}

}

uint8_t 	get_measured_state(void)
{
	if(state & NEED_TRIG)
	{
		state |= WAIT_START_ECHO;
		state &= ~NEED_TRIG;
		TIM1_start();
		return 0;
	}
	if(state & END_ECHO)
	{
		state 	&= ~END_ECHO;
		measured_time	=	measured_time/58;
		return 1;
	}
	return 1;
}

uint32_t	get_distance_sm(void)
{
	state |= NEED_TRIG;
	return measured_time;
}

void init_timers_hc_sr04(void)
{
	init_trig();
	init_echo();
}


