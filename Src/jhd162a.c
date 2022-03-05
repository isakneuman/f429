/*
 * jhd162a.c
 *
 *  Created on: 21 Feb 2022
 *      Author: isakn
 */
#include "jhd16a.h"

#define PIN_RS    (1 << 0)
#define PIN_EN    (1 << 2)
#define BACKLIGHT (1 << 3)

uint8_t ascii_nums[] =
{
		52,//0
		53,//1
};

void	hc_sr04_print(uint8_t address, uint16_t data)
{
	lcd_cmd(0x01,address);
	Delay(LED_DELAY);
	lcd_print_simple(address,"Distance: ");
	lcd_data((data/100)+48,address);
	lcd_data(((data%100)/10)+48,address);
	lcd_data((data%10)+48,address);
}

void lcd_print_simple(uint8_t address, uint8_t* data)
{
	while(*data)
	{
		lcd_data(*data,address);
		data++;
	}
}

void lcd_print(uint8_t address, uint8_t* data)
{
	static uint8_t count = 0, second_floor = 0;
	while(*data)
	{
		if(count>31)
		{
			if(second_floor)
			{
				lcd_cmd(0x01,address);
			    Delay(LED_DELAY);
				lcd_cmd(0x80,address);
				Delay(LED_DELAY);
				second_floor = 0;
				count = 0;
			}
		}
		else if(count>15)
		{
			if(!second_floor)
			{
				lcd_cmd(0xC0,address);
				Delay(LED_DELAY);
				second_floor = 1;
			}
		}
		lcd_data(*data,address);
		data++;
		count++;
	}
}

void	lcd_send(uint8_t data, uint8_t address, uint8_t flags)
{
	uint8_t arr_data[4];
	uint8_t up = (data & 0xF0);
	uint8_t lo = (data << 4) & 0xF0;;

	arr_data[0] = up|flags|BACKLIGHT|PIN_EN;
	arr_data[1] = up|flags|BACKLIGHT;
	arr_data[2] = lo|flags|BACKLIGHT|PIN_EN;
	arr_data[3] = lo|flags|BACKLIGHT;

	i2c1_send(arr_data, sizeof(arr_data), address);
}

void	lcd_cmd(uint8_t cmd, uint8_t address)
{
	lcd_send(cmd, address, 0);
}

void	lcd_data(uint8_t data, uint8_t address)
{
	lcd_send(data, address, PIN_RS);
}

void	init_lcd1602(void)
{
	init_i2c1_gpio();
	init_i2c1();
	Delay(50);
	lcd_cmd(0x30,PCF8574_ADDRESS);
	Delay(50);
	lcd_cmd(0x02,PCF8574_ADDRESS);
	Delay(50);
	lcd_cmd(0x0F,PCF8574_ADDRESS);
	Delay(50);
	lcd_cmd(0x01,PCF8574_ADDRESS);
	Delay(50);
}

