#include <stdint.h>
#include "stm32f10x.h"

// Простая задержка
void delay(uint32_t ticks) {
    for (volatile uint32_t i=0; i<ticks; i++) {
        __NOP();
    }
}

int __attribute((noreturn)) main(void) {
    // Включаем тактирование AFIO и портов C и A
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN; // для LED
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN; // для кнопок

    // Настройка PC13 как выход push-pull 10MHz
    GPIOC->CRH &= ~GPIO_CRH_CNF13;        // очистка CNF
    GPIOC->CRH |= GPIO_CRH_MODE13_0;      // MODE = 01 (10MHz)

    // Настройка PA0 и PA1 как вход floating
    GPIOA->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0); // PA0
    GPIOA->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_MODE1); // PA1
    GPIOA->CRL |= (0x01 << 2); // CNF0 = 01 -> floating input
    GPIOA->CRL |= (0x01 << 6); // CNF1 = 01 -> floating input

    uint32_t delay_ticks = 1000000; // начальная задержка (~1 Гц)

    while (1) {
        // Включаем LED
        GPIOC->ODR |= (1U<<13U);
        delay(delay_ticks);

        // Выключаем LED
        GPIOC->ODR &= ~(1U<<13U);
        delay(delay_ticks);

        // Кнопка A (PA0) - увеличить частоту
        if ((GPIOA->IDR & (1U<<0)) && delay_ticks > 15625) { // ограничение 64 Гц
            delay_ticks /= 2;
            delay(100000); // антидребезг
        }

        // Кнопка C (PA1) - уменьшить частоту
        if ((GPIOA->IDR & (1U<<1)) && delay_ticks < 64000000) { // ограничение 1/64 Гц
            delay_ticks *= 2;
            delay(100000); // антидребезг
        }
    }
}
