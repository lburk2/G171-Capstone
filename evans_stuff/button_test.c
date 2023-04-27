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

/*
    UPDATE: 4/4/23: I am now adding rotary encoder functionality 
    Using 9 LEDs on the breadboard, I am testing if I can cycle through 10 
    different 'attack' states.
*/


#include <stdio.h>
#include "pico/stdlib.h"   // stdlib 
#include "hardware/irq.h"  // interrupts
#include "hardware/gpio.h" // gpio
#include "hardware/pwm.h"  // pwm 
#include "hardware/sync.h"
#include "ADSR_c.h"
// #include "hardware/uart.h"

#include "sample.h"

#define BUTTON_PIN 2
#define LED_PIN 3
#define AUDIO_PIN 16

// LED defines for 4/4/23
#define LED0 17
#define LED1 18
#define LED2 19
#define LED3 20
#define LED4 21
#define LED5 22
#define LED6 26
#define LED7 27
#define LED8 28

uint8_t g_attackRate = 4; // will define units later
uint8_t g_decayRate = 5; // will define units later
uint8_t g_sustainLvl = 5; // will define units later
uint8_t g_releaseRate = 5; // will define units later

// FUNCTIONS TO MOVE 4/4/23:
void irq_en(bool en);
void ADSR_callback(uint gpio, uint32_t events);
void button_callback(uint gpio, uint32_t events);
void encoder_callback(uint gpio, uint32_t events);
void pins_init(void);

int main() {

    // uart_init(uart0, 115200);
    stdio_init_all();
    pins_init();

    gpio_init(BUTTON_PIN);
    gpio_init(LED_PIN);

    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    gpio_pull_down(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_LEVEL_HIGH | GPIO_IRQ_EDGE_FALL,
                                       true, button_callback);


    // testing encoder on 4/4/23
    

    irq_en(true);

    while(1) {

        switch (g_attackRate) 
        {
            case 0:
                gpio_put(LED0, 1);
                break;
            case 1:
                gpio_put(LED1, 1);
                break;
            case 2:
                gpio_put(LED2, 1);
                break;
            case 3:
                gpio_put(LED3, 1);
                break;
            case 4:
                gpio_put(LED4, 1);
                break;
            case 5:
                gpio_put(LED5, 1);
                break;
            case 6:
                gpio_put(LED6, 1);
                break;
            case 7:
                gpio_put(LED7, 1);
                break;
            case 8:
                gpio_put(LED8, 1);
                break;
            case 9: 
                gpio_put(LED4, 0);
                
                break;
            case 10:
                gpio_put(LED0, 0);
                gpio_put(LED8, 0);
                break;
            default:
                break;
        }
    }

    return 0;
}




    // Lets see if we can plug in the ADSR to this code 3/30/23

    // encoder_ISR when knob is turned. Increment or decimate a glob var based on 
    // which direction you turn it

    // then, run a switch and affect ADSR based on that glob var


    void encoder_callback(uint gpio, uint32_t events) {

        volatile uint8_t pin_state_1 = gpio_get(gpio);
            for(volatile uint32_t i = 0;i<10000;i++){}
        volatile uint8_t pin_state_2 = gpio_get(gpio);

        printf("Encoder Interrupt occurred at pin %d with event %d\n", gpio, events);

        if (pin_state_1 == pin_state_2) {
        // logic for LEDs 4/4/23
            if(gpio == ARER)   //attack rotary encoder right
                if(g_attackRate >= 10) {
                    g_attackRate = 10;
                    printf("attack is maxed\n");
                }
                else {
                    g_attackRate++;
                    printf("attackRate increased by 1!\n");
                }
            if(gpio == AREL)
                if(g_attackRate <= 0) {
                    g_attackRate = 0;
                    printf("attack is too low\n");
                }
                else {
                    g_attackRate--;
                    printf("attackRate decreased by 1!\n");
                }
        }
        pin_state_1 = pin_state_2;
    }



    // if(currentDecayRate != g_decayRate) {
    //     // update current decay rate
    //     currentDecayRate = g_decayRate;
    // }


    // if(buttonPress_to_activateADSR) -> then initialize ADSR.
    // ADSR testADSR;
    //initADSR(ADSR* testADSR);

    // testADSR->output = process(testADSR); // should I use arrow or dot here?
    // Dot is for struct, arrow is for a pointer to da struct

    // adsrAudio = (testADSR->output)*audioSignalArray[];

    // 10:27 PM testing with level parameters!
    void irq_en(bool en) {
        if(en)
        {
            gpio_set_irq_enabled_with_callback(ARER, GPIO_IRQ_EDGE_RISE, true, &encoder_callback);
            gpio_set_irq_enabled(AREL, GPIO_IRQ_EDGE_RISE, true);
        }
        else
        {
            gpio_set_irq_enabled_with_callback(ARER, GPIO_IRQ_EDGE_RISE, false, &encoder_callback);
            gpio_set_irq_enabled(AREL, GPIO_IRQ_EDGE_RISE, false);
        }
    }

    // a function to test button interrupts
    // plug this into the irq with callback function
    void button_callback(uint gpio, uint32_t events) {
       //  printf("Interrupt occurred at pin %d with event %d\n", gpio, events);
        //gpio_put(LED_PIN, 1);
        if(events == GPIO_IRQ_LEVEL_HIGH) {
            gpio_put(AUDIO_PIN, 1);
        }
        if(events == GPIO_IRQ_EDGE_FALL) {
            gpio_put(AUDIO_PIN, 0);
        }
    }

    void pins_init(void) 
    {
            
        gpio_init(ARER);
        gpio_init(AREL);

        gpio_set_dir(ARER, GPIO_IN);
        gpio_set_dir(AREL, GPIO_IN);

        gpio_pull_up(ARER);
        gpio_pull_up(AREL);

        gpio_init(LED1);
        gpio_init(LED2);
        gpio_init(LED3);
        gpio_init(LED4);
        gpio_init(LED5);
        gpio_init(LED6);
        gpio_init(LED7);
        gpio_init(LED8);
        gpio_init(LED0);

        gpio_set_dir(LED1, GPIO_OUT);
        gpio_set_dir(LED2, GPIO_OUT);
        gpio_set_dir(LED3, GPIO_OUT);
        gpio_set_dir(LED4, GPIO_OUT);
        gpio_set_dir(LED5, GPIO_OUT);
        gpio_set_dir(LED6, GPIO_OUT);
        gpio_set_dir(LED7, GPIO_OUT);
        gpio_set_dir(LED8, GPIO_OUT);
        gpio_set_dir(LED0, GPIO_OUT);
        

    }
    
