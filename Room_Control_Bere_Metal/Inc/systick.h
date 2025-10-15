#ifndef SYSTICK_H
#define SYSTICK_H
#include <stdint.h>

// SysTick
typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t LOAD;
    volatile uint32_t VAL;
    volatile uint32_t CALIB;
} SysTick_TypeDef;

#define SysTick ((SysTick_TypeDef *)0xE000E010U)

/**
 * @brief Inicializa el SysTick para generar interrupciones cada 1 ms.
 *        Debe llamarse una sola vez al iniciar el programa.
 */
void init_systick(void);

/**
 * @brief Devuelve el número de milisegundos desde que se inicializó el sistema.
 */
uint32_t systick_get_ms(void);

void Systick_Handler(void);

#endif