#include "main.h"


int main(void)
{
	init_delay();
	init_uart1();

	init_lcd1602();
	init_hc_sr04();
	uint32_t data = 0;

	while(1)
	{
		if(get_measured_state())
		{
			data = get_distance_sm();
			hc_sr04_print(PCF8574_ADDRESS, data);
			printf("%d\r\n",data);
		}
		Delay(100);
//		lcd_print(PCF8574_ADDRESS, &data);
	}
}


