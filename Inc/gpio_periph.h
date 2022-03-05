#ifndef GPIO_PERIPH_H_
#define GPIO_PERIPH_H_

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"

#define TRIGG_PIN	8
#define ECHO_PIN	6
#define ULTRA_SOUND_Port	GPIOA

#define I2C1_Port	GPIOB
#define I2C1_SDA	GPIO_PinSource7
#define I2C1_SCL	GPIO_PinSource6

void	init_i2c1_gpio(void);
void 	init_hc_sr04_gpio(void);

#endif /* GPIO_PERIPH_H_ */
