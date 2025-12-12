#include <stdint.h>
#include <stm32f10x.h>
#include <stdbool.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

void delay_us(uint32_t us) {
    us *= 72;
    for(volatile uint32_t i = 0; i < us; i++) {
        __asm volatile ("nop");
    }
}

void TIM2_IRQHandler(void) {
	if (TIM2->SR & TIM_SR_UIF) {
		GPIOC->ODR ^= (1 << 13);
		TIM2->SR &= ~TIM_SR_UIF;
	}
}

void toggleSpeed(void) {
    static uint8_t direction = 0;
	TIM2->CR1 &= ~TIM_CR1_CEN;
    
    if (direction == 0) {
        if (TIM2->PSC < 0x8000) {
            TIM2->PSC = TIM2->PSC << 1;
        } else {
            direction = 1;
            TIM2->PSC = TIM2->PSC >> 1;
        }
    } else {
        if (TIM2->PSC > 1023) {
            TIM2->PSC = TIM2->PSC >> 1;
        } else {
            direction = 0;
            TIM2->PSC = TIM2->PSC << 1;
        }
    }
    
    TIM2->EGR |= TIM_EGR_UG;
	TIM2->CR1 |= TIM_CR1_CEN;
}

int __attribute((noreturn)) main(void) {
	

	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;

	GPIOA->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_MODE1);
	GPIOA->CRL |= GPIO_CRL_CNF1_1;
	GPIOA->ODR |= (1U << 1U);

	GPIOC->CRH &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
	GPIOC->CRH |= GPIO_CRH_MODE13_0;
	GPIOC->ODR |= (1U << 13U);
	
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
 	RCC->APB1RSTR |= RCC_APB1RSTR_TIM2RST;
 	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM2RST;
 	TIM2->PSC = 1023;
	TIM2->ARR = 4095;
	TIM2->DIER |= TIM_DIER_UIE;
	NVIC_ClearPendingIRQ(TIM2_IRQn);
	NVIC_EnableIRQ(TIM2_IRQn);
	TIM2->CR1 |= TIM_CR1_CEN;
	
	bool ledEnable = true;
	bool buttonPrevStage = GPIOA->IDR & GPIO_IDR_IDR1;
	bool buttonPressed = false;

	while (1) {
		bool buttonNewStage = GPIOA->IDR & GPIO_IDR_IDR1;
		
		if(buttonPrevStage == true && buttonNewStage == false) {
			toggleSpeed();
			delay_us(100000);
		}

		buttonPrevStage = buttonNewStage;
	}
}