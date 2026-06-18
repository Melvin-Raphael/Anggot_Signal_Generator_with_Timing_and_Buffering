/* delay.c */
#include "delay.h"
#include "stm32f4xx_hal.h"

void Delay_Init(void) {
    /* Enable TRC */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    /* Reset cycle counter */
    DWT->CYCCNT = 0;
    /* Enable cycle counter */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

void Delay_us(uint32_t us) {
    uint32_t startTicks = DWT->CYCCNT;
    uint32_t targetTicks = us * (SystemCoreClock / 1000000);

    while ((DWT->CYCCNT - startTicks) < targetTicks) {
        /* Busy wait */
    }
}

void Delay_ms(uint32_t ms) {
    HAL_Delay(ms);
}
