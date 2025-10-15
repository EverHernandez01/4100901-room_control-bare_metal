#include "nvic.h"
#include "uart.h"
#include "rcc.h" 

static void nvic_enable_irq(uint32_t IRQn)
{
    NVIC->ISER[IRQn / 32U] |= (1UL << (IRQn % 32U));
}

// --- Configurar prioridad de interrupción ---------------------------
void nvic_set_priority(uint32_t IRQn, uint8_t priority)
{
    // Cada campo NVIC -> IP tiene 8 bits por IRQ
    NVIC -> IP[IRQn] = (priority << 4) & 0xF0; 
}

void nvic_exti_pc13_button_enable(void) {
    // 1. Habilitar el reloj para SYSCFG
    rcc_syscfg_clock_enable();

    // 2. Configurar la línea EXTI13 (SYSCFG_EXTICR) pin (PC13)
    SYSCFG->EXTICR[3] &= ~(0x000FU << 4);  // Limpiar campo EXTI13
    SYSCFG->EXTICR[3] |=  (0x0002U << 4);  // Conectar EXTI13 a PC13

    // 3. Configurar la línea EXTI13 para interrupción
    EXTI->IMR1 |= (1U << 13);

    // 4. Configurar el trigger de flanco de bajada
    EXTI->FTSR1 |= (1U << 13);
    EXTI->RTSR1 &= ~(1U << 13);

    // 5. Habilitar la interrupción EXTI15_10 en el NVIC
    nvic_enable_irq(EXTI15_10_IRQn);

    // 6. Establecer prioridad ALTA (valor bajo)
    nvic_set_priority(EXTI15_10_IRQn, 1);
}

void nvic_usart2_irq_enable(void) {

    // Habilitar interrupción de recepción en USART2
    USART2->CR1 |= (1U << 5);  // RXNEIE

    // Establecer prioridad BAJA (valor más alto)
    nvic_set_priority(USART2_IRQn, 3);

    // Habilitar IRQ
    nvic_enable_irq(USART2_IRQn);
}