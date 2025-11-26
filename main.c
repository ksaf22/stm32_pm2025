#include <stdint.h>
#include <stm32f10x.h>

void delay(uint32_t ticks)
{
	for (int i = 0; i < ticks; i++)
	{
		__NOP();
	}
}

int __attribute((noreturn)) main(void)
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;

	GPIOA->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_MODE1);
	GPIOA->CRL |= GPIO_CRL_CNF1_1;
	GPIOA->ODR |= (1U << 1U);

	GPIOC->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
	GPIOC->CRH |= GPIO_CRH_MODE13_0;
	GPIOC->ODR |= (1U << 13U);

	uint32_t delays[] = {
		500000000,
		250000000,
		100000000,
		50000000,
		25000000};
	uint8_t mode = 0;

	while (1)
	{
		if (!(GPIOA->IDR & GPIO_IDR_IDR1))
		{
			if (mode < sizeof(delays) / sizeof(delays[0]) - 1)
			{
				mode++;
			}
			else
			{
				mode = 0;
			}
			for (int i = 0; i < 3; i++)
			{
				GPIOC->ODR &= ~(1U << 13U);
				delay(5000000);
				GPIOC->ODR |= (1U << 13U);
				delay(5000000);
			}
			delay(80000000);
		}
		GPIOC->ODR ^= (1U << 13U);
		delay(delays[mode]);
	}
}
