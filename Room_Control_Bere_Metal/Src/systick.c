#include "systick.h"

// --- Definiciones de registros para SysTick (Ver PM0214)  --------------------
#define SYST_CSR    (*(volatile uint32_t *)0xE000E010) // Control y estado
#define SYST_RVR    (*(volatile uint32_t *)0xE000E014) // Valor de recarga
#define SYST_CVR    (*(volatile uint32_t *)0xE000E018) // Valor de registro actual
#define HSI_FREQ    4000000U           // Reloj interno 4 MHz

static volatile uint32_t systick_ms_counter = 0;

void init_systick(void)
{
    SYST_RVR = HSI_FREQ/ 1000 - 1;                      // Recarga = 4000000 - 1
    SYST_CSR = (1 << 0) | (1 << 1) | (1 << 2);    // ENABLE|TICKINT|CLKSOURCE
}

uint32_t systick_get_ms(void){
    return systick_ms_counter;
}  