#ifndef INC_I2C_PERIPH_H_
#define INC_I2C_PERIPH_H_

#include "stm32f4xx.h"
#include "stm32f4xx_i2c.h"
#include "stm32f4xx_rcc.h"

void	init_i2c1(void);
void	i2c1_start(uint8_t address);
void	i2c1_send(uint8_t* data,uint8_t size, uint8_t address);

#endif /* INC_I2C_PERIPH_H_ */
