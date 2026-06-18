/* tone.c */
#include "tone.h"
#include "delay.h"
#include <math.h>

/* Shared state definitions */
volatile bool g_isr_consumed = false;
volatile uint32_t g_sample_counter = 0u;
Queue_t g_sample_queue;
volatile bool g_underflow_error = false;

volatile uint8_t live_sw1_pin = 0;
volatile uint8_t live_sample_pin = 0;
volatile uint8_t live_dac_out = 0;

static volatile int16_t g_current_sample = 0;
static int16_t g_sine_table[SAMPLES_PER_PERIOD];

/* Sine lookup table: mapped to digital 0 or 1 for square wave */
static void build_sine_table(void) {
    for (uint32_t i = 0u; i < SAMPLES_PER_PERIOD; i++) {
        float angle = 2.0f * (float)M_PI * (float)i / (float)SAMPLES_PER_PERIOD;
        g_sine_table[i] = (sinf(angle) >= 0.0f) ? 1 : 0;
    }
}

static inline void output_sample(int16_t sample) {
    if (sample) {
        PIN_SET(DAC_OUT_PORT, DAC_OUT_PIN);
    } else {
        PIN_CLR(DAC_OUT_PORT, DAC_OUT_PIN);
    }

    live_dac_out = PIN_READ(DAC_OUT_PORT, DAC_OUT_PIN);
}

void Tone_Init(void) {
    build_sine_table();
    Queue_Init(&g_sample_queue);

    // Explicitly shut them all down at startup using the updated macro
    ALL_LEDS_OFF();
}

void tone_play_with_busy_waiting(void) {
    uint32_t sample_idx = 0;

    while (1) {
        /* Read the pin state continuously */
        live_sw1_pin = PIN_READ(SW1_PORT, SW1_PIN);

        /* CHANGE TO RESET: Only run when the button is PRESSED */
        if (live_sw1_pin == GPIO_PIN_RESET) {
        	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);

            output_sample(g_sine_table[sample_idx]);
            sample_idx = (sample_idx + 1u) % SAMPLES_PER_PERIOD;

            PIN_TOG(SAMPLE_PORT, SAMPLE_PIN);
            live_sample_pin = PIN_READ(SAMPLE_PORT, SAMPLE_PIN);

            Delay_us(20);
        } else {
        	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);
        }
    }
}

/* Interrupt-driven playback using Queue */
void tone_play_with_interrupt(void) {
    uint32_t sample_idx = 0;

    while (1) {
        /* Update live tracking variable for SW1 */
        live_sw1_pin = PIN_READ(SW1_PORT, SW1_PIN);

        if (live_sw1_pin == GPIO_PIN_SET) {
            /* Switch NOT pressed (Released): Force mirror pin LOW */
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_RESET);

            while (!Queue_IsFull(&g_sample_queue)) {
                Queue_Enqueue(&g_sample_queue, g_sine_table[sample_idx]);
                sample_idx = (sample_idx + 1u) % SAMPLES_PER_PERIOD;
            }

            /* When completely full and switch is released: Solid Green */
            LED_GREEN_ON();
            LED_BLUE_OFF();
            LED_RED_OFF();
        } else {
            /* Switch IS pressed: Force mirror pin HIGH */
            HAL_GPIO_WritePin(GPIOE, GPIO_PIN_5, GPIO_PIN_SET);

            /* Stop filling the queue immediately. Turn off Green. */
            LED_GREEN_OFF();
        }
    }
}

void timer_callback_isr(TIM_HandleTypeDef *htim) {
    if (htim->Instance != TIM2) return;

    HAL_GPIO_TogglePin(SAMPLE_PORT, SAMPLE_PIN);

    /* Update live tracking variable for SAMPLE pin state right after toggling */
    live_sample_pin = PIN_READ(SAMPLE_PORT, SAMPLE_PIN);

    uint32_t pos = g_sample_counter % SAMPLES_PER_PERIOD;
    if (pos == 0 || pos == HALF_PERIOD_SAMPLES) {
        HAL_GPIO_TogglePin(PERIOD_PORT, PERIOD_PIN);
    }

    int16_t sample;
    if (Queue_Dequeue(&g_sample_queue, &sample)) {
        output_sample(sample);

        /* Only allow the ISR to switch to Blue/Red if the button is pressed
           and actively draining. This stops the 50kHz ghosting effect. */
        if (PIN_READ(SW1_PORT, SW1_PIN) == GPIO_PIN_RESET) {
            if (Queue_IsEmpty(&g_sample_queue)) {
                LED_BLUE_OFF();
                LED_RED_ON();   /* Red: Empty */
            } else {
                LED_BLUE_ON();  /* Blue: Between empty and full */
                LED_RED_OFF();
            }
        }
    } else {
        g_underflow_error = true;

        if (PIN_READ(SW1_PORT, SW1_PIN) == GPIO_PIN_RESET) {
            LED_BLUE_OFF();
            LED_RED_ON();       /* Red: Empty underflow */
        }
    }

    g_sample_counter++;
}
