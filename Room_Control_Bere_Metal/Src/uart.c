#include "uart.h"

//Configura PA2/PA3 como AF7 para USART2.
//Configura los pines PA2 y PA3 para funcionar como pines de USART2 TX y RX.

void init_gpio_uart(void) {  
    // PA2->TX AF7, PA3->RX AF7
    RCC->AHB2ENR |= (1 << 0);            // habilita el reloj para GPIOA
    GPIOA->MODER &= ~((3U<<4)|(3U<<6));  // Limpia bits 
    GPIOA->MODER |=  ((2U<<4)|(2U<<6));  // Modo Alternate Function
    GPIOA->AFRL &= ~((0xFU<<8)|(0xFU<<12)); // Limpia bits
    GPIOA->AFRL |=  ((7U<<8)|(7U<<12));  // AF7 = USART2
}

//Habilita RCC APB1 USART2, configura BRR, enciende UE/TE/RE.
void init_uart(void) {
    RCC->APB1ENR1 |= (1 << 17);          // habilita USART2
    USART2->BRR = (HSI_FREQ + (BAUD_RATE/2)) / BAUD_RATE;
    USART2->CR1 = (1 << 3) | (1 << 2);  // TE | RE
    USART2->CR1 |= (1 << 0);            // UE
}

//Espera TXE y escribe en TDR. Envía un solo carácter.
void uart_send(char c) {
    while (!(USART2->ISR & (1 << 7)));   // TXE
    USART2->TDR = (uint8_t)c;
}

//Espera RXNE y lee RDR. Recibe un solo carácter.
char uart_receive(void) {
    while (!(USART2->ISR & (1 << 5)));   // RXNE
    return (char)(USART2->RDR & 0xFF);
}

//Envía una cadena completa carácter por carácter.
void uart_send_string(const char *str) {
    while (*str) {
        uart_send(*str++);
    }
}