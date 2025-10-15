#include "rcc.h"

void rcc_init(void)
{
    RCC->AHB2ENR |= (1 << 0);                      // Habilita reloj GPIOA
    RCC->AHB2ENR |= (1 << 2);                      // Habilita reloj GPIOC
}
// Habilitar reloj para SYSCFG
void rcc_syscfg_clock_enable(void)
{
    RCC->APB2ENR |= (1 << 0); // SYSCFGEN bit
}

void rcc_tim3_clock_enable(void)
{
    // TIM3 está en el bus APB1
    RCC->APB1ENR1 |= (1 << 1); // Bit 1 habilita TIM3
}