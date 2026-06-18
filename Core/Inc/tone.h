/* tone.h */
#ifndef TONE_H
#define TONE_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "queue.h"

extern volatile uint8_t live_SAMPLE;
extern volatile uint8_t live_PERIOD;
extern volatile uint8_t live_DAC_OUT;
extern volatile uint8_t live_SW1;

/* ------------------------------------------------------------------
 * Pin Definitions (Migrated from pin_config.h)
 * ------------------------------------------------------------------ */
#define DAC_OUT_PORT     GPIOG
#define DAC_OUT_PIN      GPIO_PIN_2
#define SAMPLE_PORT      GPIOG
#define SAMPLE_PIN       GPIO_PIN_3
#define PERIOD_PORT      GPIOG
#define PERIOD_PIN       GPIO_PIN_4

/* Switch Input */
#define SW1_PORT         GPIOC
#define SW1_PIN          GPIO_PIN_0

/* RGB LED (common-anode active LOW) */
#define LED_RED_PORT     GPIOE
#define LED_RED_PIN      GPIO_PIN_11
#define LED_GREEN_PORT   GPIOE
#define LED_GREEN_PIN    GPIO_PIN_13
#define LED_BLUE_PORT    GPIOE
#define LED_BLUE_PIN     GPIO_PIN_15

/* Helper Macros */
#define PIN_SET(port, pin)  HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET)
#define PIN_CLR(port, pin)  HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET)
#define PIN_TOG(port, pin)  HAL_GPIO_TogglePin(port, pin)
#define PIN_READ(port, pin) HAL_GPIO_ReadPin(port, pin)

/* Helper Macros restored to Active-HIGH */
#define LED_ON(port, pin)   PIN_SET(port, pin)  /* Set pin to 3.3V to turn ON */
#define LED_OFF(port, pin)  PIN_CLR(port, pin)  /* Clear pin to 0V to turn OFF */

#define LED_RED_ON()        LED_ON(LED_RED_PORT, LED_RED_PIN)
#define LED_RED_OFF()       LED_OFF(LED_RED_PORT, LED_RED_PIN)
#define LED_GREEN_ON()      LED_ON(LED_GREEN_PORT, LED_GREEN_PIN)
#define LED_GREEN_OFF()     LED_OFF(LED_GREEN_PORT, LED_GREEN_PIN)
#define LED_BLUE_ON()       LED_ON(LED_BLUE_PORT, LED_BLUE_PIN)
#define LED_BLUE_OFF()      LED_OFF(LED_BLUE_PORT, LED_BLUE_PIN)

#define ALL_LEDS_OFF()      do { LED_RED_OFF(); LED_GREEN_OFF(); LED_BLUE_OFF(); } while(0)

/* ------------------------------------------------------------------
 * Tone Generator Configuration
 * ------------------------------------------------------------------ */
#define SAMPLE_RATE_HZ         50000u
#define WAVEFORM_FREQ_HZ       25u
#define SAMPLES_PER_PERIOD     (SAMPLE_RATE_HZ / WAVEFORM_FREQ_HZ)
#define HALF_PERIOD_SAMPLES    (SAMPLES_PER_PERIOD / 2u)

/* Shared State Variables */
extern volatile bool g_isr_consumed;
extern volatile uint32_t g_sample_counter;
extern volatile bool g_underflow_error;
extern Queue_t g_sample_queue;

/* API Functions */
void Tone_Init(void);
void tone_play_with_busy_waiting(void);
void tone_play_with_interrupt(void);
void timer_callback_isr(TIM_HandleTypeDef *htim);

#endif /* TONE_H */
