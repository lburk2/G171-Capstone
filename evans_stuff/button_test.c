// 3/17/23
// Crispy Kev's test on if he can get a button to make some simple beeps 
// and boops on the rasp pi pico
// This is for Sr. Design.
// 
// Check out this cool link here: https://www.youtube.com/watch?v=BjNzfTrE6Go

/*
    UPDATE: 5 PM. Got it working with LED!!
    Can we apply it with sound??
    Need to:
        include sound header
        replace the LED code with sound code

*/


#include <stdio.h>
#include "pico/stdlib.h"   // stdlib 
#include "hardware/irq.h"  // interrupts
#include "hardware/gpio.h" // gpio
#include "hardware/pwm.h"  // pwm 
#include "hardware/sync.h"
#include "ADSR_c.h" // wait for interrupt

#include "sample.h"

#define BUTTON_PIN 2
#define LED_PIN 3
#define AUDIO_PIN 16

// a function to test button interrupts
// plug this into the irq with callback function
void button_callback(uint gpio, uint32_t events) {
    printf("Interrupt occurred at pin %d with event %d\n", gpio, events);
    //gpio_put(LED_PIN, 1);
   if(events == GPIO_IRQ_LEVEL_HIGH) {
       gpio_put(AUDIO_PIN, 1);
   }
   if(events == GPIO_IRQ_EDGE_FALL) {
       gpio_put(AUDIO_PIN, 0);
   }

}

int main() {
    stdio_init_all();

    gpio_init(BUTTON_PIN);
    gpio_init(LED_PIN);

    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_pull_down(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_LEVEL_HIGH | GPIO_IRQ_EDGE_FALL,
                                       true, button_callback);

    while(1) {}

    return 0;


    // Lets see if we can plug in the ADSR to this code
}