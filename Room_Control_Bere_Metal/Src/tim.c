#include "tim.h"
#include "rcc.h"
#include "gpio.h"


static uint8_t s_last_duty_percent = 0;

void tim3_ch1_pwm_init(uint32_t pwm_freq_hz)
{
    
    gpio_setup_pin(GPIOA, 6, GPIO_MODE_AF, 2);

    
    rcc_tim3_clock_enable();

    
    TIM3->PSC = 100-1; // Prescaler: divide por 100

    // ARR = (f_clk / f_pwm) - 1
    TIM3->ARR = (TIM_PCLK_FREQ_HZ / pwm_freq_hz) - 1;

    // PWM modo 1 en CH1, habilitar salida
    TIM3->CCMR1 = (6U << 4);
    TIM3->CCER |= (1U << 0);

    // Arrancamos con duty = 0% por seguridad
    TIM3->CCR1 = 0;
    s_last_duty_percent = 0;

    // Habilitar contador
    TIM3->CR1 |= (1U << 0);
}

void tim3_ch1_pwm_set_duty_cycle(uint8_t duty_cycle_percent)
{
    if (duty_cycle_percent > 100)
        duty_cycle_percent = 100;

    uint32_t arr = TIM3->ARR;
    uint32_t ccr = ((arr + 1U) * duty_cycle_percent) / 100U;
    TIM3->CCR1 = ccr;

    s_last_duty_percent = duty_cycle_percent;
}

void tim3_ch1_pwm_set_frequency(uint32_t pwm_freq_hz)
{
    TIM3->PSC = 100-1;
    if (pwm_freq_hz == 0)
        pwm_freq_hz = PWM_FREQUENCY;

    TIM3->ARR = (TIM_PCLK_FREQ_HZ / pwm_freq_hz) - 1;


    tim3_ch1_pwm_set_duty_cycle(s_last_duty_percent);
}