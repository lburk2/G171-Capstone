/*
    4/8/23: I am testing ADSR with audio here
    Easier to do it here than in the messy button_test file
    A copy of main.c, but now with ADSR input
*/

#include <stdio.h>
#include "pico/stdlib.h" 
#include "hardware/irq.h"  
#include "hardware/pwm.h"   
#include "hardware/sync.h" 

#include "sample.h"
#include "LUT.h"
#include "ADSR_c.h"

#define AUDIO_PIN  16  
#define BUTTON_PIN 2
#define LED_PIN 3



int wav_position = 0;
float Fs = 11000;
int outputBuffer[500];
int buf_size = sizeof(outputBuffer);
int buf_position = 0;
ADSR testADSR_1;
float outputTest = 0;

void pins_init(void);

/*
 * PWM Interrupt Handler which outputs PWM level and advances the 
 * current sample. 
 * 
 * We repeat the same value for 8 cycles this means sample rate etc
 * adjust by factor of 8   (this is what bitshifting <<3 is doing)
 * 
 */

void pwm_interrupt_handler() {
    // pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN));    
    
        // if(buf_position > buf_size) {
        //     printf("buffer reset to 0!\n");
        //     buf_position = 0;
// 
        // }

       //  float envelope_output = getOutput(testADSR_1);
       //  outputBuffer[buf_position] = envelope_output * sinusoid[wav_position];

        // modulo by 85 (length of sine array), to wrap at each period
        // #define C4_LENGTH sizeof(array);
        // we are gonna multiply sample at a time
        gate(&testADSR_1);
        // testADSR_1.gate = 1;

        while(testADSR_1.gate == 1) {

            printf("IN processing loop\n");
        
            outputTest = process(&testADSR_1);
            testADSR_1.output = outputTest;
            printf("process output: %f\n", testADSR_1.output);
            
            float out = testADSR_1.output * sinusoid[wav_position];
            pwm_set_gpio_level(AUDIO_PIN, out);  
            // printf("One sample of audio has been processed, sin value current: %d\n" , sinusoid[wav_position]);
    
            // wrapping if needed
            wav_position++;
            if (wav_position % SIN_LENGTH == 0) {
                // printf("wav set back to 0\n");
                wav_position = 0;
            }
        }

        return;
}

void button_callback(uint gpio, uint32_t events) {
   // printf("Interrupt occurred at pin %d with event %d\n", gpio, events);

    // debounce 
    for(volatile int i = 0; i < 10000; i++) {}


   if(events == GPIO_IRQ_EDGE_RISE) {
       gpio_put(LED_PIN, 1);
        pwm_interrupt_handler();
   }
   if(events == GPIO_IRQ_EDGE_FALL) {
       gpio_put(LED_PIN, 0);
       // need to use this to reset the gate voltage level and set ADSR state to idle
       pwm_interrupt_handler();
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


    // stuff from crispy's code **************** //

    pwm_set_gpio_level(AUDIO_PIN, 0);

    gpio_init(BUTTON_PIN);
    gpio_init(LED_PIN);

    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_pull_down(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_RISE,
                                       true, button_callback);

    // 4/8/23 additions

    initADSR(&testADSR_1);
    setAttackRate(&testADSR_1, 1.0 * Fs);
    setDecayRate(&testADSR_1, 0.3 * Fs);
    setReleaseRate(&testADSR_1, 5.0 * Fs);
    setSustainLevel(&testADSR_1, 0.8 * Fs);

    // testADSR_1.state = env_attack;
    // need to do a long big loop where the process runs a BUNCHA times.
    // testADSR_1->output = process(testADSR_1);
    
    printf("ADSR has been set up (main)\n");

    while(1) {
        
    }

    // ADSR will be automatically on by default.
    // IF there is a parameter change, run the process again and multiply your audio signal
    // Basically, this will change the length of the ADSR envelope, and this is why I am confusion


    // ***************************************** //

    while(1) {
        __wfi(); // Wait for Interrupt
    }
}

void pins_init(void) 
    {

    }


// CODE FROM TESS 4/26/23
// a is length of the sine LUT
// b is the length of the ADSR envelope

    // int gcd(int a, int b)
    // {
    //     if (a == 0)
    //     {
    //         return b;
    //     }
    //     return gcd(b % a, a);
    // }
// 
// 
    // int lcm(int a, int b)
    // {
    //     return (a * b) / gcd(a, b);
    // }
// 
    // uint8_t *arrayAdd(uint8_t tone1[], uint8_t tone2[], int len1, int len2)
    // {
    //     mod0 = lcm(sizeof(audio_buffer), sizeof(audio_buffer2));
    //     printf("lcm: %d\n", mod0);
    //     uint8_t outputVal = (uint8_t)malloc(mod0 * sizeof(uint8_t));
// 
    // for (int count = 0; count < mod0; count++)
    // {
    //     index1 = count % len1;
    //     index2 = count % len2;
    //     outputVal[count] = tone1[index1] + tone2[index2];
    //     printf("1:%d , 2:%d ", index1, index2);
    // }
// 
    // return outputVal;
    // }