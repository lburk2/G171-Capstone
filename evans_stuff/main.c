#include <stdio.h>
#include "pico/stdlib.h"   // stdlib 
#include "hardware/irq.h"  // interrupts
#include "hardware/pwm.h"  // pwm 
#include "hardware/sync.h" // wait for interrupt 
 
// Audio PIN is to match some of the design guide shields. 
#define AUDIO_PIN  16  
#define BUTTON_PIN 2
#define LED_PIN 3

/* 
 * This include brings in static arrays which contain audio samples. 
 * if you want to know how to make these please see the python code
 * for converting audio samples into static arrays. 
 */
#include "sample.h"
#include "LUT.h"
int wav_position = 0;

/*
 * PWM Interrupt Handler which outputs PWM level and advances the 
 * current sample. 
 * 
 * We repeat the same value for 8 cycles this means sample rate etc
 * adjust by factor of 8   (this is what bitshifting <<3 is doing)
 * 
 */
void pwm_interrupt_handler() {
    pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN));    
    if (wav_position < (WAV_DATA_LENGTH) - 1) { 
        // set pwm level 
        // allow the pwm value to repeat for 8 cycles this is >>3 
        pwm_set_gpio_level(AUDIO_PIN, WAV_DATA[wav_position]);  
        wav_position++;
    } else {
        // reset to start
        wav_position = 0;
    }
}

void button_callback(uint gpio, uint32_t events) {
    printf("Interrupt occurred at pin %d with event %d\n", gpio, events);
   if(events == GPIO_IRQ_LEVEL_HIGH) {
       gpio_put(LED_PIN, 1);
       pwm_interrupt_handler();
   }
   if(events == GPIO_IRQ_EDGE_FALL) {
       gpio_put(LED_PIN, 0);
   }

}

int main(void) {
    /* Overclocking for fun but then also so the system clock is a 
     * multiple of typical audio sampling rates.
     */
    stdio_init_all();
    set_sys_clock_khz(176000, true); 
    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);

    int audio_pin_slice = pwm_gpio_to_slice_num(AUDIO_PIN);


    /*
        THIS CODE MAKES THE SOUND RUN AUTOMATICALLY
    */
    // Setup PWM interrupt to fire when PWM cycle is complete
    // pwm_clear_irq(audio_pin_slice);
    // pwm_set_irq_enabled(audio_pin_slice, true);
    // set the handle function above
    // irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_interrupt_handler); 
    // irq_set_enabled(PWM_IRQ_WRAP, true);

    // Setup PWM for audio output
    pwm_config config = pwm_get_default_config();
    /* Base clock 176,000,000 Hz divide by wrap 250 then the clock divider further divides
     * to set the interrupt rate. 
     * 
     * 11 KHz is fine for speech. Phone lines generally sample at 8 KHz
     * 
     * 
     * So clkdiv should be as follows for given sample rate
     *  8.0f for 11 KHz
     *  4.0f for 22 KHz
     *  2.0f for 44 KHz etc
     */
    pwm_config_set_clkdiv(&config, 8.0f); 
    pwm_config_set_wrap(&config, 10); 
    pwm_init(audio_pin_slice, &config, true);


    // stuff from crispy's code **************** //

    pwm_set_gpio_level(AUDIO_PIN, 0);

    gpio_init(BUTTON_PIN);
    gpio_init(LED_PIN);

    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_pull_down(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_LEVEL_HIGH | GPIO_IRQ_EDGE_FALL,
                                       true, button_callback);

    // ***************************************** //

    while(1) {
        __wfi(); // Wait for Interrupt
    }
}