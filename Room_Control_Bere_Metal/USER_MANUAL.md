# Manual de Usuario: Sistema de Control de Sala  
**Universidad Nacional de Colombia - Sede Manizales**  
**Curso:** Estructuras Computacionales (4100901)  
**Autor:** Ever Daniel Hernández Tulcán  

---

## 1. Introducción

El **Sistema de Control de Sala** es una aplicación embebida desarrollada para la placa **STM32L476RG**, cuyo propósito es simular un **controlador básico de iluminación para una habitación**.  
Utiliza LEDs para representar bombillas, un botón para simular la presencia del usuario, y comunicación UART para permitir el control remoto.  
El brillo de una bombilla secundaria se regula mediante **modulación por ancho de pulso (PWM)**, demostrando el uso de periféricos avanzados y la gestión de eventos por interrupciones.

---

## 2. Hardware Utilizado

- **Placa:** NUCLEO-L476RG (basada en microcontrolador STM32L476RG).  
- **LED de Heartbeat (LD2 - PB3):** Indica que el sistema está en ejecución.  
- **LED Principal (PA5):** Simula la bombilla de la habitación, encendida/apagada por el botón.  
- **LED PWM (canal TIM3_CH1):** Simula una bombilla con brillo regulable.  
- **Botón de Usuario (PC13):** Genera interrupciones externas (EXTI13) para cambiar el estado de la habitación.  
- **Interfaz UART (USART2):** Permite comunicación serial con un PC (TX/RX), para enviar mensajes y recibir comandos.  

---

## 3. Funcionalidades

### 3.1 Heartbeat LED
El **LED de heartbeat** (PB3) parpadea periódicamente, indicando que el sistema se encuentra activo.  
Esta funcionalidad se actualiza mediante interrupciones del **SysTick**, con una base de tiempo de 1 ms.

---

### 3.2 Control de Bombilla por Botón
El **botón de usuario (B1)** permite alternar el estado de la habitación:

- Si la sala está **idle (vacía)**, al presionar el botón se enciende el LED principal (PA5), indicando que la habitación ahora está **ocupada**.  
- Si está **occupied**, una nueva pulsación apaga la luz y vuelve al estado **idle**.  

Adicionalmente, el sistema apaga automáticamente la bombilla después de **3 segundos (3000 ms)** de inactividad, gracias al temporizador **SysTick**.

Cada evento se comunica por UART con mensajes como: Botón presionado → LED encendido.
LED apagado automáticamente.


---

### 3.3 Comunicación UART
La **comunicación UART** permite interactuar con el sistema desde un computador mediante un terminal serial (por ejemplo, PuTTY o TeraTerm).

**Parámetros recomendados:**
- Baudrate: 9600 bps  
- Bits de datos: 8  
- Paridad: Ninguna  
- Bits de parada: 1  
- Control de flujo: Ninguno  

**Comandos disponibles:**

| Comando | Descripción                            | Acción                                                                 |
|----------|----------------------------------------|------------------------------------------------------------------------|
| `h` / `H` | Brillo máximo                         | PWM = 100%                                                             |
| `l` / `L` | Bombilla atenuada                     | PWM = 0% (apagada)                                                     |
| `1`–`9`   | Brillo intermedio                     | Cambia el duty cycle a múltiplos de 10% (`1`=10%, `5`=50%, `9`=90%)   |
| `o` / `O` | Cambiar estado a ocupado              | Enciende bombilla principal                                            |
| `i` / `I` | Cambiar estado a inactivo             | Apaga bombilla principal                                               |
| Cualquier otro | Eco                              | Devuelve el mismo carácter recibido                                    |

Cada comando genera una respuesta por UART para confirmar la acción realizada.

---

### 3.4 Control PWM de Bombilla
El **PWM (Pulse Width Modulation)** controla el brillo de la bombilla secundaria.  
Se genera con el **TIM3_CH1** configurado a una frecuencia de **1 kHz**.  
El ciclo de trabajo (**duty cycle**) se ajusta según los comandos UART, simulando diferentes niveles de iluminación.

---

## 4. Arquitectura del Sistema

### 4.1 Módulos

| Módulo | Descripción |
|--------|--------------|
| **main.c** | Inicializa periféricos y coordina los eventos. |
| **gpio.c/h** | Configura y controla pines de entrada/salida. |
| **systick.c/h** | Genera la base de tiempo de 1 ms. |
| **uart.c/h** | Implementa comunicación serial y manejo de interrupciones. |
| **tim.c/h** | Configura el temporizador TIM3 para generar PWM. |
| **nvic.c/h** | Habilita y configura interrupciones externas (EXTI y USART2). |
| **room_control.c/h** | Contiene la lógica de la aplicación: estados, tiempo de apagado y comandos UART. |

---

### 4.2 Flujo de Ejecución

1. **Inicialización:**  
   `main()` llama a `peripherals_init()` para configurar todos los periféricos.  
   Luego invoca `room_control_app_init()`.

2. **Bucle principal:**  
   Se procesan eventos provenientes de interrupciones:
   - `button_event` → Llama `room_control_on_button_press()`.
   - `uart_event_char` → Llama `room_control_on_uart_receive()`.
   - Llama periódicamente a `room_control_update()` para manejar temporizadores.

3. **Interrupciones:**
   - **SysTick_Handler:** Incrementa el contador global de milisegundos.  
   - **EXTI15_10_IRQHandler:** Detecta pulsación del botón.  
   - **USART2_IRQHandler:** Captura caracteres recibidos por UART.  

---

## 5. Uso del Sistema

### Conexión
1. Conecta la placa **Nucleo STM32L476RG** al PC mediante cable USB.  
2. Abre un terminal serial (PuTTY, TeraTerm) en el puerto COM detectado, a **9600 bps**.  
3. Verifica que aparezca el mensaje:   Sistema de Control de Sala Inicializado!


### Inicio
- El **LED heartbeat** parpadeará, indicando que el sistema está en funcionamiento.  
- El **LED principal (PA5)** comenzará apagado (estado *idle*).

### Interacción
- Presiona el **botón (PC13)** para encender o apagar el LED principal.  
- Envía comandos UART para cambiar brillo o estado.  
- Después de 3 segundos sin actividad, la luz se apaga automáticamente.

---

## 6. Diagramas

### 6.1 Diagrama de Estados

    +-------------+
    |   IDLE      |
    | (LED OFF)   |
    +-------------+
           |
      Botón / 'o'
           v
    +-------------+
    | OCCUPIED    |
    | (LED ON)    |
    +-------------+
           |
 Timeout 3s o 'i'
           v
    +-------------+
    |   IDLE      |
    +-------------+

---

### 6.2 Diagrama de Componentes

+---------------------------------------------------------+

main.c
Inicializa periféricos, gestiona eventos y llama
funciones del módulo room_control.
+---------------------------------------------------------+
       |                     |                    |
       v                     v                    v
+----------------+ +----------------+ +----------------+
| gpio.c/h | | uart.c/h | | tim.c/h |
| Manejo de LEDs | | Comunicación | | Generación PWM |
| y botón | | serial UART | | (TIM3_CH1) |
+----------------+ +----------------+ +----------------+
_____________________|
v
+-------------------+
| room_control.c/h |
| Lógica de estados |
| Control de LED y |
| tiempo de apagado |
+-------------------+

## 7. Conclusión

Este proyecto integra **GPIO**, **UART**, **TIM (PWM)**, **SysTick** y **NVIC** para construir un sistema embebido funcional basado en interrupciones y temporización precisa.  
Sirve como práctica educativa para comprender el flujo completo de una aplicación embebida real, desde la configuración de periféricos hasta la lógica de aplicación basada en eventos.

---
