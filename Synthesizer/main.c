#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "LCD.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "imageN.h"
// #include "font24.h"
#include "menuButtons.h"
#include "OLED.h"
#include "sd_card.h"
#include "ff.h"
#include "MCP23017.h"
#include "pwm_audio_setup.h"
#include "pico_button_matrix.h"

uint8_t g_buttonPress = -1; 

void printMenuOptions();
void menuButtons_init(void);
void gpio_callback(uint gpio, uint32_t events);
void irq_en(bool en);


int main(void){

    FRESULT fr;
    FATFS fs;
    FIL fil;
    //int ret;
    char buf2[10000];
    uint16_t samples[1000];
    char* sample;
    char filename[] = "test02.txt";
    spi_inst_t *spi = spi0;

    stdio_init_all();

    LCD_init();
    Paint_DrawImage(gImage_imageN, 0,0,LCD_WIDTH,LCD_HEIGHT);
    LCD_Write_Command(0x10);
    Paint_DrawRectangle(0,180,320,240,WHITE,2,1);
    LCD_Write_Command(0x11);
    Paint_DrawRectangle(30,180,290,200,BLACK,2,0);
    Paint_DrawRectangle(32,182,96,198,RED,2,1);  
    
    sleep_ms(500);
    menuButtons_init();
    Paint_DrawRectangle(32,182,160,198,RED,2,1);
    
    sleep_ms(500);
    oled_init();
    Paint_DrawRectangle(32,182,224,198,RED,2,1);

    sleep_ms(500);
    Paint_DrawRectangle(32,182,288,198,RED,2,1);

    // Initialize SD card
    if (!sd_init_driver()) {
        printf("ERROR: Could not initialize SD card\r\n");
        while (true);
    }

    // Mount drive
    fr = f_mount(&fs, "0:", 1);
    if (fr != FR_OK) {
        printf("ERROR: Could not mount filesystem (%d)\r\n", fr);
        while (true);
    }
    
    spi_init(spi, 125000000);
    
    sleep_ms(500);
    LCD_clear(MAGENTA);
    sleep_ms(500);
    printMenuOptions();
    
    oled_setCursor(0,0);
    // oled_WriteString("Nebraska! \n");
    // oled_WriteCharacter16('N');
    oled_WriteString16("***Nebraska!***\n");

    uint16_t squareY = 0;
    // uint16_t color[] = {WHITE, GREEN, BLUE, MAGENTA};
    // uint8_t colorIndex = 0;

    uint8_t menuLocation = 5;
    uint8_t prevMenuLocation = 5;
    uint8_t prevY = 1;

    Paint_DrawRectangle(39,prevY*20+51,200,prevY*20+51+19,MAGENTA,2,0);
    Paint_DrawRectangle(39,squareY*20+51,200,squareY*20+51+19,BLACK,2,0);

    struct render_area frame_area = {start_col: 0, end_col : OLED_WIDTH - 1, start_page : 0, end_page : OLED_NUM_PAGES -
                                                                                                        1};
    calc_render_area_buflen(&frame_area);
    uint8_t buf[OLED_BUF_LEN];
    oled_fill(buf,0x00);

    // //button init
    // uint columns[5] = { 22, 23, 24, 25, 12 };
    // uint rows[6] = { 26, 27, 28, 29, 11, 13 };
    // char matrix[30] = 
    // {
    //     '1', '2', '3', '4','0', 
    //     '5', '6', '7','8', '0', 
    //     '9', '10','11', '12' ,'0', 
    //     '13', '0', '0', '0', '0',
    //     'w', 's', 'a', 'd', 'x',
    //     'g', 'b', 'r', '!', '$',
    // };
    // pico_keypad_init(columns, rows, matrix);
    // int key = pico_keypad_get_key();
    // spi_init(spi, 25 * 1000 * 1000);
    // // Open file for writing ()
    // fr = f_open(&fil, filename, FA_WRITE | FA_CREATE_ALWAYS);
    // if (fr != FR_OK) {
    //     printf("ERROR: Could not open file (%d)\r\n", fr);
    // }

    // // Write something to file
    // f_printf(&fil, "0,    804,   1608,   2410,   3212,   4011,   4808,   5602,   6393,   7179,   7962,   8739,   9512, 10278,  11039,  11793,  12539,  13279,  14010,  14732,  15446,  16151,  16846,  17530,  18204,  18868,  19519,  20159,  20787,  21403,  22005,  22594,  23170,  23731,  24279,  24811,  25329,  25832,  26319,  26790,  27245,  27683,  28105,  28510,  28898,  29268,  29621,  29956,  30273,  30571,  30852,  31113,  31356,  31580,  31785,  31971,  32137,  32285,  32412,  32521,  32609,  32678,  32728,  32757,  32767,  32757,  32728,  32678,  32609,  32521,  32412,  32285,  32137,  31971,  31785,  31580,  31356,  31113,  30852,  30571,  30273,  29956,  29621,  29268,  28898,  28510,  28105,  27683,  27245,  26790,  26319,  25832,  25329,  24811,  24279,  23731,  3170,  22594,  22005,  21403,  20787,  20159,  19519,  18868,  18204,  17530,  16846,  16151,  15446,  14732,  14010,  13279,   12539,  11793,  11039,  10278,   9512,   8739,   7962,   7179,   6393,   5602,   4808,   4011,   3212,   2410,   1608,    804, 0,   -804,  -1608,  -2410,  -3212,  -4011,  -4808,  -5602,  -6393,  -7179,  -7962,  -8739,  -9512, -10278, -11039, -11793,   -12539, -13279, -14010, -14732, -15446, -16151, -16846, -17530, -18204, -18868, -19519, -20159, -20787, -21403, -22005, -22594, -23170, -23731, -24279, -24811, -25329, -25832, -26319, -26790, -27245, -27683, -28105, -28510, -28898, -29268, -29621, -29956, -30273, -30571, -30852, -31113, -31356, -31580, -31785, -31971, -32137, -32285, -32412, -32521, -32609, -32678, -32728, -32757, -32767, -32757, -32728, -32678, -32609, -32521, -32412, -32285, -32137, -31971, -31785, -31580, -31356, -31113, -30852, -30571, -30273, -29956, -29621, -29268, -28898, -28510, -28105, -27683, -27245, -26790, -26319, -25832, -25329, -24811, -24279, -23731, -23170, -22594, -22005, -21403, -20787, -20159, -19519, -18868, -18204, -17530, -16846, -16151, -15446, -14732, -14010, -13279, -12539, -11793, -11039, -10278,  -9512,  -8739,  -7962,  -7179,  -6393,  -5602,  -4808,  -4011,  -3212,  -2410,  -1608,   -804");

    // // Close file
    // fr = f_close(&fil);
    // spi_init(spi, 125000000);


    while (true)
    {
        prevMenuLocation = menuLocation;

        switch (g_buttonPress)
        {
            case RIGHT:/* code */
                switch (menuLocation)
                {
                    case MainMenu:
                        
                        break;
                    case SD:
                        
                        break;
                    case SPECTRUM:
                        
                        break;
                    case BEEPS:
                        
                        break;
                    case BOOPS:
                        
                        break;
                    case BOINKS:
                        
                        break;
                    
                    default:
                        break;
                }
                g_buttonPress = -1;
            break;
            case LEFT:/* code */
                switch (menuLocation)
                {
                    case MainMenu:
                        
                        break;
                    case SD:
                        menuLocation = 5;
                        break;
                    case SPECTRUM:
                        menuLocation = 5;
                        break;
                    case BEEPS:
                        menuLocation = 5;
                        break;
                    case BOOPS:
                        menuLocation = 5;
                        break;
                    case BOINKS:
                        menuLocation = 5;
                        break;
                    
                    default:
                        break;
                }
            g_buttonPress = -1;
            break;
            case UP:/* code */
                switch (menuLocation)
                {
                case MainMenu:
                    if(squareY > 0 )
                    {
                        prevY = squareY;
                        squareY--;
                        irq_en(false);
                        Paint_DrawRectangle(39,prevY*20+51,200,prevY*20+51+19,MAGENTA,2,0);
                        Paint_DrawRectangle(39,squareY*20+51,200,squareY*20+51+19,BLACK,2,0);
                        irq_en(true);
                    }
                break;
                
                default:
                    break;
                }
            g_buttonPress = -1;
            break;
            case DOWN:/* code */
                switch (menuLocation)
                {
                case MainMenu:
                    if(squareY < 4)
                    {
                        prevY = squareY;
                        squareY++;
                        irq_en(false);
                        Paint_DrawRectangle(39,prevY*20+51,200,prevY*20+51+19,MAGENTA,2,0);
                        Paint_DrawRectangle(39,squareY*20+51,200,squareY*20+51+19,BLACK,2,0);
                        irq_en(true);
                    }
                    break;
                
                default:
                    break;
                }
            g_buttonPress = -1;
            break;
            case CENTER:/* code */
                switch (menuLocation)
                {
                case MainMenu:
                    menuLocation = squareY;
                    break;
                
                default:
                    break;
                }
            g_buttonPress = -1;
            break;
            case RE:/* code */
                oled_render(buf, &frame_area);
                oled_setCursor(0,2);
                oled_WriteString16("***Nebraska?***\n");
                g_buttonPress = -1;
            break;
            case RERIGHT:/* code */
                oled_render(buf, &frame_area);
                oled_setCursor(0,2);
                oled_WriteString16("ADSR RIGHT\n");
                g_buttonPress = -1;
            break;
            case RELEFT:/* code */
                oled_render(buf, &frame_area);
                oled_setCursor(0,2);
                oled_WriteString16("ADSR LEFT\n");
                g_buttonPress = -1;
            break;
            default:

            break;
        }

        if(prevMenuLocation != menuLocation){
            switch (menuLocation)
            {
            case MainMenu:
                printMenuOptions();
                irq_en(false);
                Paint_DrawRectangle(39,prevY*20+51,200,prevY*20+51+19,MAGENTA,2,0);
                Paint_DrawRectangle(39,squareY*20+51,200,squareY*20+51+19,BLACK,2,0);
                irq_en(true);
                break;
            case SD:
                LCD_clear(RED);
                Paint_DrawString_EN(20, 30, "SD Card Stuff Here", RED, WHITE);

                spi_init(spi, 25 * 1000 * 1000);
                
                // Open file for reading
                fr = f_open(&fil, filename, FA_READ);
                if (fr != FR_OK) {
                    printf("ERROR: Could not open file (%d)\r\n", fr);
                    while (true);
                }
                // Print every line in file over serial
                printf("Reading from file '%s':\r\n", filename);
                printf("---\r\n");
                int count = 0;
                while (f_gets(buf2, sizeof(buf2), &fil)) {
                    //printf(buf2);
                    sample = strtok(buf2, ",");
                        while (sample != NULL) {
                            samples[count++] = atoi(sample);
                            sample = strtok(NULL, ",");
                        }
                }
                
                for (uint16_t i = 0; i < count; i++){
                    printf("%d\n", samples[i]);
                }

                printf("\r\n---\r\n");
                f_close(&fil);
                //f_unmount("0:");
                spi_init(spi, 125000000);
                break;
            case SPECTRUM:
                LCD_clear(BLUE);
                Paint_DrawString_EN(20, 30, "Spectrum Stuff Here", BLUE, WHITE);
                break;
            case BEEPS:
                LCD_clear(GREEN);
                Paint_DrawString_EN(20, 30, "BEEPS Stuff Here", GREEN, WHITE);
                break;
            case BOOPS:
                LCD_clear(BLACK);
                Paint_DrawString_EN(20, 30, "BOOPS Stuff Here", BLACK, WHITE);
                break;
            case BOINKS:
                LCD_clear(GRAY);
                Paint_DrawString_EN(20, 30, "SD Card Stuff Here", GRAY, WHITE);
                break;
            
            default:
                break;
            }
        }
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

void menuButtons_init(void)
{
    // Initialize the buttons for the user controlls
    gpio_init(RE);
    gpio_set_dir(RE, GPIO_IN);
    gpio_pull_up(RE);

    gpio_init(UP);
    gpio_set_dir(UP, GPIO_IN);
    gpio_pull_up(UP);

    gpio_init(LEFT);
    gpio_set_dir(LEFT, GPIO_IN);
    gpio_pull_up(LEFT);

    gpio_init(CENTER);
    gpio_set_dir(CENTER, GPIO_IN);
    gpio_pull_up(CENTER);

    gpio_init(DOWN);
    gpio_set_dir(DOWN, GPIO_IN);
    gpio_pull_up(DOWN);

    gpio_init(RIGHT);
    gpio_set_dir(RIGHT, GPIO_IN);
    gpio_pull_up(RIGHT);

    gpio_init(RERIGHT);
    gpio_set_dir(RERIGHT, GPIO_IN);
    gpio_pull_down(RERIGHT);

    gpio_init(RELEFT);
    gpio_set_dir(RELEFT, GPIO_IN);
    gpio_pull_down(RELEFT);

    //enable interrupts
    irq_en(true);
}

// ISR for buttons. 
void gpio_callback(uint gpio, uint32_t events)
{
    uint8_t pin_state_1 = gpio_get(gpio);
    for(volatile uint32_t i = 0;i<10000;i++){}
    uint8_t pin_state_2 = gpio_get(gpio);

    if (pin_state_1 == pin_state_2)
    {
        if (gpio == RE)
        {
            g_buttonPress = RE;  
        }
        if (gpio == UP)
        {
            g_buttonPress = UP;
        }
        if (gpio == DOWN)
        {
            g_buttonPress = DOWN;
        }
        if (gpio == LEFT)
        {
             g_buttonPress = LEFT;        
        }
        if (gpio == RIGHT)
        {
            g_buttonPress = RIGHT;
        }
        if (gpio == CENTER)
        {
            g_buttonPress = CENTER;
        }
        if (gpio == RERIGHT)
        {
            g_buttonPress = RERIGHT;
        }
        if (gpio == RELEFT)
        {
            g_buttonPress = RELEFT;
        }
    }
}

void irq_en(bool en)
{   
    if(en)
    {
        // enable interrupts
        gpio_set_irq_enabled_with_callback(RE, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
        gpio_set_irq_enabled(UP, GPIO_IRQ_EDGE_FALL, true);
        gpio_set_irq_enabled(LEFT, GPIO_IRQ_EDGE_FALL, true);
        gpio_set_irq_enabled(CENTER, GPIO_IRQ_EDGE_FALL, true);
        gpio_set_irq_enabled(DOWN, GPIO_IRQ_EDGE_FALL, true);
        gpio_set_irq_enabled(RIGHT, GPIO_IRQ_EDGE_FALL, true);
        gpio_set_irq_enabled(RELEFT, GPIO_IRQ_EDGE_FALL, true);
        gpio_set_irq_enabled(RERIGHT, GPIO_IRQ_EDGE_FALL, true);
    }else{
        // disable interrupts
        gpio_set_irq_enabled_with_callback(RE, GPIO_IRQ_EDGE_FALL, false, &gpio_callback);
        gpio_set_irq_enabled(UP, GPIO_IRQ_EDGE_FALL, false);
        gpio_set_irq_enabled(LEFT, GPIO_IRQ_EDGE_FALL, false);
        gpio_set_irq_enabled(CENTER, GPIO_IRQ_EDGE_FALL, false);
        gpio_set_irq_enabled(DOWN, GPIO_IRQ_EDGE_FALL, false);
        gpio_set_irq_enabled(RIGHT, GPIO_IRQ_EDGE_FALL, false); 
        gpio_set_irq_enabled(RELEFT, GPIO_IRQ_EDGE_FALL, false);
        gpio_set_irq_enabled(RERIGHT, GPIO_IRQ_EDGE_FALL, false);
    }
 
}