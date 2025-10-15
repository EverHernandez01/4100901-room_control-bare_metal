#include "room_control.h"
#include "gpio.h"
#include "systick.h"
#include "uart.h"
#include "tim.h"



// ======================== VARIABLES GLOBALES ==========================
// Variable de estado global
room_state_t current_state = ROOM_IDLE;
static uint32_t led_on_time = 0;
static uint32_t last_button_time = 0;
static uint32_t last_heartbeat_time = 0;
static uint8_t current_duty = PWM_INITIAL_DUTY;


extern volatile uint32_t system_ms_counter;  // Variable global del SysTick (definida en main.c)

// ======================== FUNCIONES ===============================

// Inicialización de la aplicación Apaga el LED (PWM=0) y muestra mensaje
void room_control_app_init(void)
{
    // Inicializar PWM en TIM3_CH1 (LED controlado por PWM)
    tim3_ch1_pwm_init(1000); // 1 kHz
    tim3_ch1_pwm_set_duty_cycle(current_duty);

    // Estado inicial: Idle, LED apagado
    current_state = ROOM_IDLE;
    clear_gpio(GPIOA,5); // LED principal apagado
    uart_send_string("Sistema Room Control Iniciado.\r\n");
}

// ----------------------------------------------------
// Lógica al presionar botón físico (PC13)
// ----------------------------------------------------
void room_control_on_button_press(void)
{
    uint32_t now = systick_get_ms();
    if ((now - last_button_time) < DEBOUNCE_MS){
        return; // Ignorar rebotes
    }
    last_button_time = now;

    // Alternar entre estados
    if (current_state == ROOM_IDLE){
        current_state = ROOM_OCCUPIED;
        set_gpio(GPIOA,5); // Encender LED principal
        tim3_ch1_pwm_set_duty_cycle(100);
        uart_send_string("Sala Ocupada.\r\n");
        led_on_time = now;
    }else{
        current_state = ROOM_IDLE;
        clear_gpio(GPIOA,5); // Apagar LED principal
        tim3_ch1_pwm_set_duty_cycle(0);
        uart_send_string("Sala Vacía.\r\n");
    }
}

// ----------------------------------------------------
// Lógica al recibir carácter por UART
// ----------------------------------------------------
void room_control_on_uart_receive(char received_char)
{
     switch (received_char) {
        case 'h':
        case 'H':
            tim3_ch1_pwm_set_duty_cycle(100);
            uart_send_string("PWM 100%\r\n");
            break;
        case 'l':
        case 'L':
            tim3_ch1_pwm_set_duty_cycle(0);
            uart_send_string("PWM 0%\r\n");
            break;
        case 'O':
        case 'o':
            current_state = ROOM_OCCUPIED;
            set_gpio(GPIOA,5);
            tim3_ch1_pwm_set_duty_cycle(100);
            uart_send_string("Comando: Sala Ocupada\r\n");
            led_on_time = systick_get_ms();
            break;
        case 'I':
        case 'i':
            current_state = ROOM_IDLE;
            clear_gpio(GPIOA,5);
            tim3_ch1_pwm_set_duty_cycle(0);
            uart_send_string("Comando: Sala Vacía\r\n");
            break;
        case 'B':
            uart_send_string("Esperando valor de brillo...\r\n");
            break;

        default:
            if (received_char >= '0' && received_char <= '9'){
                // Escalar el número 0-9 a 0-90% duty
                uint8_t new_duty = (received_char - '0')*10;
                tim3_ch1_pwm_set_duty_cycle(new_duty);
                current_duty = new_duty;
                uart_send_string("Nuevo duty PWM: ");
                uart_send(received_char);
                uart_send_string("0%\r\n");
            }else{
                // Echo de cualquier otro cáracter
                uart_send(received_char);
            }
            break;
    }
}

// ----------------------------------------------------
// Actualización periódica (llamar en el bucle principal)
// ----------------------------------------------------
void room_control_update(void)
{
     uint32_t now = systick_get_ms();

    // --- Timeout LED ---
    if(current_state == ROOM_OCCUPIED && (now-led_on_time)>= LED_TIMEOUT_MS){
        current_state = ROOM_IDLE;
        clear_gpio(GPIOA,5);
        tim3_ch1_pwm_set_duty_cycle(0);
        uart_send_string("Timeout: LED apagado (sala vacía)\r\n");
    }

    // --- Hearbeat ---
    if ((now - last_heartbeat_time) >= HEARTBEAT_MS){
        static uint8_t hb_state = 0;
        if(hb_state){
            clear_gpio(GPIOA,5);
        }else  {
            set_gpio(GPIOA,5);
        }       
        hb_state = !hb_state;
        last_heartbeat_time = now;
    }
}
