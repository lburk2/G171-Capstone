#include <stdio.h>
#include "pico/stdlib.h"   // stdlib 
#include "hardware/irq.h"  // interrupts
#include "hardware/pwm.h"  // pwm 
#include "hardware/sync.h" // wait for interrupt 
#include <math.h>
#include "sample.h"
#include "LUT.h"

// Audio PIN is to match some of the design guide shields. 
#define AUDIO_PIN  16  
#define BUTTON_PIN 2
#define FILT_CONTROL 0
#define LED_PIN 3

// Sampling frequency (Hz)
#define SAMPLE_RATE 11000

// Initial cutoff frequency (Hz)
#define INITIAL_FC 1000

// some global variables
int wav_position = 0;
float fc = INITIAL_FC;

// Biquad filter coefficients
float b0 = 0, b1 = 0, a0 = 0, a1 = 0;
float x0 = 0, x1 = 0, x2 = 0, z1 = 0, z2 = 0;

void update_filter(float fc);


void pwm_interrupt_handler() {
    pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN));    
    if (wav_position < (SIN_LENGTH) - 1) { 
        
        // LET'S test out an IIR with this bad boy: 4/23/23
        x0 = sinusoid[wav_position];

        float y0 = (b0/a0)*x0 + (b1/a0)*x1 + (b1/a0)*x2 - (a1/a0)*z1 - (a1/a0)*z2;

        pwm_set_gpio_level(AUDIO_PIN, y0);  
        wav_position++;

        x2 = x1;
        x1 = x0;
        z2 = z1;
        z1 = y0;

        // end of test. Is this too inefficient?

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

void filter_callback(uint gpio, uint32_t events) {

    if(events == GPIO_IRQ_EDGE_RISE) {
        printf("fc decrement by 10, fc: %f\n", fc);
        fc -= 10;
        update_filter(fc);
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
    pwm_config config = pwm_get_default_config();

    pwm_config_set_clkdiv(&config, 8.0f); 
    pwm_config_set_wrap(&config, 10); 
    pwm_init(audio_pin_slice, &config, true);


    // stuff from crispy's code **************** //

    pwm_set_gpio_level(AUDIO_PIN, 0);

    gpio_init(BUTTON_PIN);
    gpio_init(LED_PIN);
    gpio_init(FILT_CONTROL); // added filter control 4/23/23
    
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_set_dir(FILT_CONTROL, GPIO_IN); // added filter control 4/23/23

    gpio_pull_down(BUTTON_PIN);
    gpio_pull_down(FILT_CONTROL);

    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_LEVEL_HIGH | GPIO_IRQ_EDGE_FALL,
                                       true, button_callback);
    gpio_set_irq_enabled_with_callback(FILT_CONTROL, GPIO_IRQ_EDGE_RISE,
                                       true, filter_callback);

    update_filter(fc);



    while(1) {
        __wfi(); // Wait for Interrupt
    }
}




void update_filter(float fc) {
    // Calculate filter coefficients
    b0 = 1 - exp(-2*M_PI*fc/SAMPLE_RATE);
    b1 = exp(-2*M_PI*fc/SAMPLE_RATE);
    a0 = 1 + b1;
    a1 = -b1;
}

