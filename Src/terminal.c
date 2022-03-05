#include "terminal.h"

int __io_putchar(int ch)
{
	write_byte(ch);
	return ch;
}

void	init_uart1(void)
{
	GPIO_InitTypeDef	USART1_RX, USART1_TX;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_PinAFConfig(GPIOA, 9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, 10, GPIO_AF_USART1);

	USART1_RX.GPIO_Mode	=	GPIO_Mode_AF;
	USART1_RX.GPIO_Pin	=	USART1_RX_PIN;
	USART1_TX.GPIO_Mode	=	GPIO_Mode_AF;
	USART1_TX.GPIO_Pin	=	USART1_TX_PIN;

	GPIO_Init(GPIOA,&USART1_RX);
	GPIO_Init(GPIOA,&USART1_TX);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	USART_InitTypeDef	Usart1_init;
	USART_StructInit(&Usart1_init);
	USART_Init(USART1, &Usart1_init);
	USART_Cmd(USART1, ENABLE);
}

void	read_byte(uint8_t* data)
{
	while( !(USART1->SR & USART_SR_RXNE));
	*data = USART1->DR;
}

void	write_byte(uint8_t data)
{
	while( !(USART1->SR & USART_SR_TXE) );
	USART1->DR	= data;
}

void printf_new(uint8_t* data)
{
	while(*data != '\0')
	{
		while( !(USART1->SR & USART_SR_TXE) );
		USART1->DR	= *data++;
	}
}
