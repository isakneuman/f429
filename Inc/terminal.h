#ifndef TERMINAL_H_
#define TERMINAL_H_

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_rcc.h"
#include "stdio.h"

#define		USART1_RX_PIN	(1<<10)
#define		USART1_TX_PIN	(1<<9)

void	init_uart1(void);
void	printf_new(uint8_t* data);
void	write_byte(uint8_t data);
void	read_byte(uint8_t* data);

#endif /* TERMINAL_H_ */
