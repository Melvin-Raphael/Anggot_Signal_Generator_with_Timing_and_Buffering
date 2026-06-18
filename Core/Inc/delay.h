/* delay.h */
#ifndef DELAY_H
#define DELAY_H

#include <stdint.h>

/* Initialises the DWT cycle counter for precision delays */
void Delay_Init(void);

/* Microsecond and millisecond blocking delays */
void Delay_us(uint32_t us);
void Delay_ms(uint32_t ms);

#endif /* DELAY_H */
