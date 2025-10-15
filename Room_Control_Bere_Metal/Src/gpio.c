#include "gpio.h"                                     // Pin PC13 (Button)

//void gpio_init_pin(GPIO_Typedef_t * GPIO, uint8_t pin, uint8_t mode, uint8_t type, uint8_t speed, uint8_t pupd, uint8_t initial_value)
void init_gpio(GPIO_Typedef_t * GPIO, uint8_t pin, uint8_t mode, uint8_t type, uint8_t speed, uint8_t pupd, uint8_t initial_value)

// GPIO mode: 00=input, 01=output, 10=alt func, 11=analog
// GPIO type: 00=push-pull, 01=open-drain

{
    GPIO->MODER &= ~(3 << (pin*2));           // Limpia bits
    GPIO->MODER |=  (mode << (pin*2));           // Configura modo

    GPIO->TYPER &= ~(1 << pin);           // Limpia bits
    GPIO->TYPER |=  (type << pin);

    GPIO->SPEEDR &= ~(3 << (pin*2));
    GPIO->SPEEDR |= (speed << (pin*2));
    
    GPIO->PUPDR &= ~(3 << (pin*2));
    GPIO->PUPDR |= (pupd << (pin*2));

    GPIO->ODR &= ~(1 << pin);
    GPIO->ODR |= (initial_value << pin);
}

void set_gpio(GPIO_Typedef_t * GPIO, uint8_t pin)
{
    GPIO->ODR |= (1 << pin);
}

void clear_gpio(GPIO_Typedef_t * GPIO, uint8_t pin)
{
    GPIO->ODR &= ~(1 << pin);
}


uint8_t read_gpio(GPIO_Typedef_t * GPIO, uint8_t pin)
{
    // Leer estado del botón PC13 (botón presionado = 0, no presionado = 1)
    if ((GPIO->IDR & (1 << pin)) == 0) { // pressed
        return 1;
    }
    return 0;
}

void gpio_setup_pin(GPIO_Typedef_t *GPIOx, uint8_t pin, uint8_t mode, uint8_t af)
{
    // Configurar modo del pin (00 entrada, 01 salida, 10 alterno, 11 analógico)
    GPIOx->MODER &= ~(0x3U << (pin * 2));
    GPIOx->MODER |=  (mode & 0x3U) << (pin * 2);

    // Si el modo es alterno (10 = 2), configurar AFRL o AFRH
    if (mode == 0x2) {
        if (pin < 8) {
            GPIOx->AFRL &= ~(0xFU << (pin * 4));
            GPIOx->AFRL |=  (af & 0xFU) << (pin * 4);
        } else {
            GPIOx->AFRH &= ~(0xFU << ((pin - 8) * 4));
            GPIOx->AFRH |=  (af & 0xFU) << ((pin - 8) * 4);
        }
    }
}