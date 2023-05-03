#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#include "MCP23017.h"

void buttonHandler(uint gpio, uint32_t events);
void irq_en(bool en);

MCP23017 gpio_expander;

uint8_t regB = 0;

int main(void){

    stdio_init_all();

    mcp23017_init(&gpio_expander, 0b00000111);

    gpio_init(9);
    gpio_set_dir(9, GPIO_IN);
    gpio_init(10);
    gpio_set_dir(10, GPIO_IN);

    gpio_init(15);
    gpio_set_dir(15, GPIO_IN);
    // gpio_pull_up(15);
    gpio_init(17);
    gpio_set_dir(17, GPIO_IN);
    // gpio_pull_up(17);

    irq_en(true);

    uint8_t regBprev = 0;


    while (true)
    {       
        // regB = mcp23017_read_register( &gpio_expander, MCP23017_GPIOB);
        int r1 = gpio_get(15);
        int r2 = gpio_get(17);

        printf("%d, %d\n", r1, r2);


        if (regB != regBprev)
        {
            printf("%d\n", regB);
            regBprev = regB;
        }

        sleep_ms(50);
    }
}

// ISR for buttons. 
void buttonHandler(uint gpio, uint32_t events)
{
    regB = mcp23017_read_register( &gpio_expander, MCP23017_GPIOB);
}
void irq_en(bool en)
{   

    gpio_set_irq_enabled_with_callback(9,GPIO_IRQ_EDGE_FALL,en,&buttonHandler);
    gpio_set_irq_enabled_with_callback(10,GPIO_IRQ_EDGE_FALL,en,&buttonHandler);


}
