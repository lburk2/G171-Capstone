#include <stdio.h>
#include "LCD.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "menuButtons.h"


// void menuButtons_init(void)
// {
//     // Initialize the buttons for the user controlls
//     gpio_init(RE);
//     gpio_set_dir(RE, GPIO_IN);
//     gpio_pull_up(RE);

//     gpio_init(UP);
//     gpio_set_dir(UP, GPIO_IN);
//     gpio_pull_up(UP);

//     gpio_init(LEFT);
//     gpio_set_dir(LEFT, GPIO_IN);
//     gpio_pull_up(LEFT);

//     gpio_init(CENTER);
//     gpio_set_dir(CENTER, GPIO_IN);
//     gpio_pull_up(CENTER);

//     gpio_init(DOWN);
//     gpio_set_dir(DOWN, GPIO_IN);
//     gpio_pull_up(DOWN);

//     gpio_init(RIGHT);
//     gpio_set_dir(RIGHT, GPIO_IN);
//     gpio_pull_up(RIGHT);

//     // enable interrupts

//     gpio_set_irq_enabled_with_callback(RE, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
//     gpio_set_irq_enabled(UP, GPIO_IRQ_EDGE_FALL, true);
//     gpio_set_irq_enabled(LEFT, GPIO_IRQ_EDGE_FALL, true);
//     gpio_set_irq_enabled(CENTER, GPIO_IRQ_EDGE_FALL, true);
//     gpio_set_irq_enabled(DOWN, GPIO_IRQ_EDGE_FALL, true);
//     gpio_set_irq_enabled(RIGHT, GPIO_IRQ_EDGE_FALL, true);
// }
// // ISR for buttons. 
// void gpio_callback(uint gpio, uint32_t events)
// {
//     uint8_t pin_state_1 = gpio_get(gpio);
//     sleep_ms(10);
//     uint8_t pin_state_2 = gpio_get(gpio);

//     if (pin_state_1 == pin_state_2)
//     {
//         if (gpio == RE)
//         {
//             g_buttonPress = RE;  
//         }
//         if (gpio == UP)
//         {
//             g_buttonPress = UP;
//         }
//         if (gpio == DOWN)
//         {
//             g_buttonPress = DOWN;
//         }
//         if (gpio == LEFT)
//         {
//              g_buttonPress = LEFT;        
//         }
//         if (gpio == RIGHT)
//         {
//             g_buttonPress = RIGHT;
//         }
//         if (gpio == CENTER)
//         {
//             g_buttonPress = CENTER;
//         }
//     }
// }