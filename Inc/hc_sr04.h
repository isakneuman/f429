#ifndef HC_SR04_H_
#define HC_SR04_H_

	#include "stm32f4xx.h"
	#include "gpio_periph.h"

	/* ---------------------------------------------
	 * TIM1 	- 	Генерирует импульс для TRIG_PIN
	 * TIM1_CH1	-	PA8
	 * ---------------------------------------------
	 * TIM3		-	Измеряет длительность импульса ECHO_PIN
	 * TIM3_CH1	-	PA6
	 * ---------------------------------------------
	 * TIM2		-	Вывод ШИМ для индикации
	 * TIM2_CH1	-	PA5
	 * ---------------------------------------------
	 */
	/*
	 * Пример:
	 * if(get_measured_state())
	 * {
	 *	 printf("distance: %d\r\n", get_distance_sm());
	 * }
	 *
	 */


	#define TOO_FAR_TIMEOUT		30000
	#define TRIGG_DURATION		12

	#define	uS_CALC(x)		(x/1000000)-1
	#define	mS_CALC(x)		(x/1000)-1

	#define NEED_TRIG			1<<0
	#define WAIT_START_ECHO		1<<1
	#define START_ECHO			1<<2
	#define END_ECHO			1<<3
	#define WTF1				1<<4
	#define WTF2				1<<5
	#define WTF3				1<<6

	void init_hc_sr04(void);

	uint8_t 	get_measured_state(void);
	uint32_t	get_distance_sm(void);


#endif /* HC_SR04_H_ */
