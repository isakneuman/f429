#include "i2c_periph.h"

I2C_InitTypeDef	i2c1;

void	init_i2c1(void)
{
	I2C1->CR1	|=	I2C_CR1_SWRST;
	I2C1->CR1	&=	~I2C_CR1_SWRST;

	i2c1.I2C_Mode	=	I2C_Mode_I2C;
	i2c1.I2C_OwnAddress1	=	0;
	i2c1.I2C_DutyCycle	=	I2C_DutyCycle_2;
	i2c1.I2C_ClockSpeed	=	100000;
	i2c1.I2C_AcknowledgedAddress	=	I2C_AcknowledgedAddress_7bit;

	I2C_Init(I2C1, &i2c1);
	I2C_AcknowledgeConfig(I2C1, ENABLE);
	I2C_Cmd(I2C1, ENABLE);
}

void	i2c1_start(uint8_t address)
{
	 /*!< While the bus is busy */
	while(I2C1->SR2 & I2C_SR2_BUSY);
	/* Start the config sequence */
	I2C1->CR1 |= I2C_CR1_START;
	while(!(I2C1->SR1 & I2C_SR1_SB));

	I2C1->DR	=	(address<<1);
	while(!(I2C1->SR1 & I2C_SR1_ADDR));
	(void)(I2C1->SR1|I2C1->SR2);
}

void	i2c1_send(uint8_t* data, uint8_t size, uint8_t address)
{
	i2c1_start(address);
	while(size--)
	{
		while(!(I2C1->SR1 & I2C_SR1_TXE));
		I2C1->DR	=	*data;
		data++;
		while(!(I2C1->SR1 & I2C_SR1_BTF));
	}
	I2C1->CR1	|=	I2C_CR1_STOP;
}
