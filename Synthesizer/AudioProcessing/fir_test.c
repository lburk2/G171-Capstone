// From chatgpt on 4.18.23 at 4PM!!
// It works!! 

// TODO: put matlab coeffs in here and make this modular



#include "pico/stdlib.h"
#include "hardware/irq.h"  // interrupts
#include "hardware/pwm.h"  // pwm 
#include "hardware/sync.h" // wait for interrupt 
// #include "hardware/adc.h"
#include "LUT.h"
#include <stdio.h>

// 4/24/24 update: testing matlab lowpass filters with 110 taps
#include "FIR_lo_400_uint8_t.h"

#define FILTER_TAP_NUM 110
#define AUDIO_PIN 16
#define BUTTON_PIN 2
#define LED_PIN 3

int wav_position = 0;

// static float filter_taps[FILTER_TAP_NUM] = {
//     10, 10, 5, 1, 1
// };

static float filter_buf[FILTER_TAP_NUM] = {0};

// h*x = y; its our output
int y = 0;



int fir_filter(int input)
{
    int output = 0;
    int i;

    // Shift the buffer
    for (i = FILTER_TAP_NUM - 1; i >= 1; i--) {
        filter_buf[i] = filter_buf[i - 1];
    }

    // Add new sample to buffer
    filter_buf[0] = input;

    // Compute output
    for (i = 0; i < FILTER_TAP_NUM; i++) {
        output += FIR_stop_400[i] * filter_buf[i];
    }

    return output;
}

void pwm_interrupt_handler() {
    pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN));    
    if (wav_position < (SIN_LENGTH) - 1) { 
        // filtering 
        y = fir_filter(triangle[wav_position]);
        pwm_set_gpio_level(AUDIO_PIN, y);  
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

int main()
{
    // Initialize ADC
    // adc_init();
    // adc_gpio_init(26); // Use GPIO26 as ADC input
    // adc_select_input(0);

    stdio_init_all();
    set_sys_clock_khz(176000, true); 
    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);
    int audio_pin_slice = pwm_gpio_to_slice_num(AUDIO_PIN);

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
    pwm_config_set_clkdiv(&config, 4.0f); 
    pwm_config_set_wrap(&config, 10); 
    pwm_init(audio_pin_slice, &config, true);

    pwm_set_gpio_level(AUDIO_PIN, 0);

    gpio_init(BUTTON_PIN);
    gpio_init(LED_PIN);

    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_pull_down(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_LEVEL_HIGH | GPIO_IRQ_EDGE_FALL,
                                       true, button_callback);

    // Initialize filter
    // ...

    // Read input samples and apply filter
    while (1) {
        // float input = adc_read() / 4095.0; // Convert ADC reading to range [0, 1]
        
        // Do something with the filtered output
        

    }
}