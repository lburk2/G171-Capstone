#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "LCD.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"

#include "imageN.h"
// #include "font24.h"
#include "OLED.h"

void printMenuOptions();

int main(void){

    stdio_init_all();

    // LCD_init();
    // // Paint_DrawImage(gImage_imageN, 0,0,LCD_WIDTH,LCD_HEIGHT);
    // // LCD_Write_Command(0x10);
    // // Paint_DrawRectangle(0,180,320,240,WHITE,2,1);
    // // LCD_Write_Command(0x11);
    // // Paint_DrawRectangle(30,180,290,200,BLACK,2,0);
    // // Paint_DrawRectangle(32,182,96,198,RED,2,1); 
    // LCD_clear(MAGENTA); 

    oled_init();
    oled_setCursor(0,0);

    oled_WriteString16("***Nebraska!***\n");
    
    gpio_init(0);
    gpio_set_dir(0,GPIO_OUT);


    while (true)
    {
        gpio_put(0,false);
        sleep_ms(100);
        gpio_put(0,true);
        sleep_ms(100);
    }
}

void printMenuOptions()
{
LCD_clear(MAGENTA);
Paint_DrawString_EN(5, 10, "Welcome to Synth", MAGENTA, WHITE);
Paint_DrawString_EN(5, 30, "------------------", MAGENTA, WHITE);
Paint_DrawString_EN(5, 50, "- SD Card", MAGENTA, WHITE);
Paint_DrawString_EN(5, 70, "- Spectrum", MAGENTA, WHITE);
Paint_DrawString_EN(5, 90, "- Beeps ", MAGENTA, WHITE);
Paint_DrawString_EN(5, 110, "- Boops ", MAGENTA, WHITE);
Paint_DrawString_EN(5, 130,"- Boinks ", MAGENTA, WHITE);
}

