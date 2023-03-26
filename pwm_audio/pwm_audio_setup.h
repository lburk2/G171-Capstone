#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/sync.h"
#include "hardware/adc.h"

extern uint32_t single_sample;
extern uint32_t* single_sample_ptr;

extern int pwm_dma_chan, trigger_dma_chan, sample_dma_chan;

void dma_irh(uint8_t audio_buffer[]);
void pwm_audio_setup(uint16_t AUDIO_SAMPLES, uint8_t AUDIO_PIN, uint8_t ADC_CHANNEL, uint8_t REPETITION_RATE,uint8_t audio_buffer[]);