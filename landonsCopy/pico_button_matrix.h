#ifndef PICO_BUTTON_MATRIX_H
#define PICO_BUTTON_MATRIX_H

#include "pico/stdlib.h"
#include "hardware/timer.h"

void pico_keypad_init(uint columns[5], uint rows[6], char matrix_values[30]);

char pico_keypad_get_key(void);

void pico_keypad_irq_enable(bool enable, gpio_irq_callback_t callback);

#endif

