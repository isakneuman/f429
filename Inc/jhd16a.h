#ifndef JHD16A_H_
#define JHD16A_H_

#include "stm32f4xx.h"
#include "i2c_periph.h"
#include "gpio_periph.h"
#include "delay.h"

#define PCF8574_ADDRESS		0x27
#define LED_DELAY	0x05

void	init_lcd1602(void);

void	lcd_send(uint8_t data, uint8_t address, uint8_t flags);
void	lcd_cmd(uint8_t cmd, uint8_t address);
void	lcd_data(uint8_t data, uint8_t address);
void 	lcd_print(uint8_t lcd_addr, uint8_t* data);
void 	lcd_print_simple(uint8_t lcd_addr, uint8_t* data);
void	hc_sr04_print(uint8_t address, uint16_t data);

#endif /* JHD16A_H_ */
