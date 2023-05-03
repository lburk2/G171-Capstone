#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include <math.h>

#define SAMPLE_RATE 44100
#define TWO_PI 6.28318530718

#define SIN_PIN 16

int main() {
    stdio_init_all();
    float phase = 0.0;
    float freq = 1000.0; // in Hz
    uint slice_num = pwm_gpio_to_slice_num(SIN_PIN);

    gpio_set_function(SIN_PIN, GPIO_FUNC_PWM);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_wrap(&config, 0xFFF);
    pwm_init(slice_num, &config, true);
    pwm_set_enabled(slice_num, true);

    while (1) {
        float amplitude = 0.5 * (1.0 + sin(phase));
        uint16_t duty_cycle = (uint16_t)(amplitude * 0xFFF);
        pwm_set_chan_level(slice_num, PWM_CHAN_A, duty_cycle);

        phase += TWO_PI * freq / SAMPLE_RATE;
        if (phase >= TWO_PI) {
            phase -= TWO_PI;
        }

        // sleep_us(1000000 / SAMPLE_RATE);
    }
    return 0;
}
