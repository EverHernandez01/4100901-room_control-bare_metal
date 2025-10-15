#ifndef TIM_H
#define TIM_H

#include <stdint.h>
#include "gpio.h"  // Librería GPIO existente
#include "rcc.h"   // Librería RCC existente

// TIM3
typedef struct {
    volatile uint32_t CR1; // Controla la habilitación y modo del timer
    volatile uint32_t CR2; // Controla el modo de operación del timer
    volatile uint32_t SMCR; // Configuración del modo esclavo
    volatile uint32_t DIER;
    volatile uint32_t SR;
    volatile uint32_t EGR;    // Genera eventos de actualización y captura/compare (Contador actual)
    volatile uint32_t CCMR1;  // Configura los modos de salida de los canales
    volatile uint32_t CCMR2;
    volatile uint32_t CCER;   //Habilita la salida del canal 1
    volatile uint32_t CNT;    // Contador del timer
    volatile uint32_t PSC;  // Prescaler: divide la frecuencia del reloj
    volatile uint32_t ARR;  // Auto-Reload Register: define el período del timer
    volatile uint32_t RESERVED1;
    volatile uint32_t CCR1;  // Captura/Comparación del canal 1 (duty cycle PWM)
    volatile uint32_t CCR2;
    volatile uint32_t CCR3;
    volatile uint32_t CCR4;
    volatile uint32_t RESERVED2;
    volatile uint32_t DCR;
    volatile uint32_t DMAR;
} TIM_TypeDef;

// Direcciones base periféricas
#define TIM3_BASE           (0x40000400UL)
#define TIM3                ((TIM_TypeDef *) TIM3_BASE)

// Constantes
#define TIM_PCLK_FREQ_HZ    4000000U   // Frecuencia del reloj del timer 4 MHz APB1 timer clock
#define PWM_FREQUENCY       1000U      // salida 1 kHz PWM
#define PWM_PERIOD          (TIM_PCLK_FREQ_HZ / PWM_FREQUENCY)
#define PWM_DUTY_CYCLE      50         // 50% duty cycle
#define PWM_PIN             6U         // PA6 = TIM3_CH1

#define PWM_DC_TO_CCR(DC) ((PWM_PERIOD * (DC)) / 100) // Macro para calcular CCR

// Prototipos de funciones
void tim3_ch1_pwm_init(uint32_t pwm_freq_hz); 
void tim3_ch1_pwm_set_duty_cycle(uint8_t duty_cycle_percent); // duty_cycle en % (0-100)

#endif // TIM_H