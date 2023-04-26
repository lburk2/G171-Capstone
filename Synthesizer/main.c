#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

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
//#include "pwm_audio_setup.h"
// #include "pico_button_matrix.h"

#define ROWS 6
#define COLS 5
#define GPIO_INPUT false
#define GPIO_OUTPUT true
uint8_t g_buttonPress = 0; 
uint row_pin;
uint columns[5] = { 22, 23, 24, 25, 12 };
uint rows[6] = { 26, 27, 28, 29, 11, 13 };


void printMenuOptions();
void menuButtons_init(void);
void buttonHandler(uint gpio, uint32_t events);
void irq_en(bool en);


int main(void){

    FRESULT fr;
    FATFS fs;
    FIL fil;
    //int ret;
    char buf2[10000];
    uint16_t samples1[1000];
    uint16_t samples2[1000];
    uint16_t samples3[1000];
    uint16_t samples4[1000];
    uint16_t samples5[1000];
    uint16_t samples6[1000];
    uint16_t samples7[1000];
    uint16_t samples8[1000];
    uint16_t samples9[1000];
    uint16_t samples10[1000];
    uint16_t samples11[1000];
    uint16_t samples12[1000];
    uint16_t samples13[1000];

    char* sample;
    char filename[] = "sineWaveOctave.txt";
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
    //menuButtons_init();
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

    struct render_area frame_area = {start_col: 0, end_col : OLED_WIDTH - 1, start_page : 0, end_page : OLED_NUM_PAGES -1};
    calc_render_area_buflen(&frame_area);
    uint8_t buf[OLED_BUF_LEN];
    oled_fill(buf,0x00);

    while (true)
    {
        prevMenuLocation = menuLocation;

        /*
        *   Button input switch statement 
        *   
        *   uses the global variable for the buttonpress
        *   to determine what menu action to perfrom based on the 
        *   button press from the user. There is a case for each UI
        *   input, and nested switch statements to determine the 
        *   action based on the input. 
        */
        switch (g_buttonPress)
        {
            case RIGHT:/* code */  //Right UI button
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
            case LEFT:/* code */    //left AKA back button
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
            case UP:/* code */ // UP 
                switch (menuLocation)
                {
                case MainMenu: //raise the box 
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
                    if(squareY < 4)//lower the box
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
            // case RE:/* code */
            //     oled_render(buf, &frame_area);
            //     oled_setCursor(0,2);
            //     oled_WriteString16("***Nebraska?***\n");
            //     g_buttonPress = -1;
            // break;
            // case RERIGHT:/* code */
            //     oled_render(buf, &frame_area);
            //     oled_setCursor(0,2);
            //     oled_WriteString16("ADSR RIGHT\n");
            //     g_buttonPress = -1;
            // break;
            // case RELEFT:/* code */
            //     oled_render(buf, &frame_area);
            //     oled_setCursor(0,2);
            //     oled_WriteString16("ADSR LEFT\n");
            //     g_buttonPress = -1;
            // break;
            default:

            break;
        }
        /*
        *   Menu action switch
        *   
        *   based on the menu locaiton action from the previous statement,
        *   this switch statement will actually perform the action. 
        * 
        */
        if(prevMenuLocation != menuLocation){ //check that we're in a new menu location
            switch (menuLocation)
            {
            case MainMenu: //print menu options with the box at prev location.
                printMenuOptions();
                irq_en(false);
                Paint_DrawRectangle(39,prevY*20+51,200,prevY*20+51+19,MAGENTA,2,0);
                Paint_DrawRectangle(39,squareY*20+51,200,squareY*20+51+19,BLACK,2,0);
                irq_en(true);
                break;
            case SD: //open the file on the SD card and store samples in arrays
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
                            samples1[count++] = atoi(sample);
                            sample = strtok(NULL, ",");
                        }
                }
                while (f_gets(buf2, sizeof(buf2), &fil)) {
                    //printf(buf2);
                    sample = strtok(buf2, ",");
                        while (sample != NULL) {
                            samples2[count++] = atoi(sample);
                            sample = strtok(NULL, ",");
                        }
                }
                while (f_gets(buf2, sizeof(buf2), &fil)) {
                    //printf(buf2);
                    sample = strtok(buf2, ",");
                        while (sample != NULL) {
                            samples3[count++] = atoi(sample);
                            sample = strtok(NULL, ",");
                        }
                }
                while (f_gets(buf2, sizeof(buf2), &fil)) {
                    //printf(buf2);
                    sample = strtok(buf2, ",");
                        while (sample != NULL) {
                            samples4[count++] = atoi(sample);
                            sample = strtok(NULL, ",");
                        }
                }
                while (f_gets(buf2, sizeof(buf2), &fil)) {
                    //printf(buf2);
                    sample = strtok(buf2, ",");
                        while (sample != NULL) {
                            samples5[count++] = atoi(sample);
                            sample = strtok(NULL, ",");
                        }
                }
                while (f_gets(buf2, sizeof(buf2), &fil)) {
                    //printf(buf2);
                    sample = strtok(buf2, ",");
                        while (sample != NULL) {
                            samples6[count++] = atoi(sample);
                            sample = strtok(NULL, ",");
                        }
                }
                while (f_gets(buf2, sizeof(buf2), &fil)) {
                    //printf(buf2);
                    sample = strtok(buf2, ",");
                        while (sample != NULL) {
                            samples7[count++] = atoi(sample);
                            sample = strtok(NULL, ",");
                        }
                }
                while (f_gets(buf2, sizeof(buf2), &fil)) {
                    //printf(buf2);
                    sample = strtok(buf2, ",");
                        while (sample != NULL) {
                            samples8[count++] = atoi(sample);
                            sample = strtok(NULL, ",");
                        }
                }
                while (f_gets(buf2, sizeof(buf2), &fil)) {
                    //printf(buf2);
                    sample = strtok(buf2, ",");
                        while (sample != NULL) {
                            samples9[count++] = atoi(sample);
                            sample = strtok(NULL, ",");
                        }
                }
                while (f_gets(buf2, sizeof(buf2), &fil)) {
                    //printf(buf2);
                    sample = strtok(buf2, ",");
                        while (sample != NULL) {
                            samples10[count++] = atoi(sample);
                            sample = strtok(NULL, ",");
                        }
                }
                while (f_gets(buf2, sizeof(buf2), &fil)) {
                    //printf(buf2);
                    sample = strtok(buf2, ",");
                        while (sample != NULL) {
                            samples11[count++] = atoi(sample);
                            sample = strtok(NULL, ",");
                        }
                }
                while (f_gets(buf2, sizeof(buf2), &fil)) {
                    //printf(buf2);
                    sample = strtok(buf2, ",");
                        while (sample != NULL) {
                            samples12[count++] = atoi(sample);
                            sample = strtok(NULL, ",");
                        }
                }
                while (f_gets(buf2, sizeof(buf2), &fil)) {
                    //printf(buf2);
                    sample = strtok(buf2, ",");
                        while (sample != NULL) {
                            samples13[count++] = atoi(sample);
                            sample = strtok(NULL, ",");
                        }
                }
                for (uint16_t i = 0; i < count; i++){
                    printf("%d, ", samples1[i]);
                }
                printf("\n");
                for (uint16_t i = 0; i < count; i++){
                    printf("%d, ", samples2[i]);
                }
                printf("\n");
                for (uint16_t i = 0; i < count; i++){
                    printf("%d, ", samples3[i]);
                }
                printf("\n");
                for (uint16_t i = 0; i < count; i++){
                    printf("%d, ", samples4[i]);
                }
                printf("\n");
                for (uint16_t i = 0; i < count; i++){
                    printf("%d, ", samples5[i]);
                }
                printf("\n");
                for (uint16_t i = 0; i < count; i++){
                    printf("%d, ", samples6[i]);
                }
                printf("\n");
                for (uint16_t i = 0; i < count; i++){
                    printf("%d, ", samples7[i]);
                }
                printf("\n");
                for (uint16_t i = 0; i < count; i++){
                    printf("%d, ", samples8[i]);
                }
                printf("\n");
                for (uint16_t i = 0; i < count; i++){
                    printf("%d, ", samples9[i]);
                }
                printf("\n");
                for (uint16_t i = 0; i < count; i++){
                    printf("%d, ", samples10[i]);
                }
                printf("\n");
                for (uint16_t i = 0; i < count; i++){
                    printf("%d, ", samples11[i]);
                }
                printf("\n");
                for (uint16_t i = 0; i < count; i++){
                    printf("%d, ", samples12[i]);
                }
                printf("\n");
                for (uint16_t i = 0; i < count; i++){
                    printf("%d, ", samples13[i]);
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

void printMenuOptions() // prints menu options
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
    // Initialize rows of button matrix
    for (int i = 0; i < COLS; i++) {
        gpio_init(columns[i]);

        gpio_set_dir(columns[i], GPIO_OUTPUT);
        gpio_put(columns[i], 1);
    }
    
    for (int i = 0; i < ROWS; i++) 
    {
        gpio_init(rows[i]);

        gpio_set_dir(rows[i], GPIO_INPUT);
        gpio_pull_down(rows[i]);
    }
    //enable interrupts
    irq_en(true);
}

// ISR for buttons. 
void buttonHandler(uint gpio, uint32_t events)
{
    for (int i = 0; i < ROWS; i++) 
    {
        if (rows[i] == gpio) 
        {
            row_pin = i;
            break;
        }
    }
    gpio_put(columns[0],1);
    gpio_put(columns[1],0);
    gpio_put(columns[2],0);
    gpio_put(columns[3],0);
    gpio_put(columns[4],0);
    for(volatile uint32_t i = 0;i<1000;i++){}
    if(gpio_get(gpio)==1)
    {
        g_buttonPress=row_pin*5;
        
    }
    gpio_put(columns[0],0);
    gpio_put(columns[1],1);
    gpio_put(columns[2],0);
    gpio_put(columns[3],0);
    gpio_put(columns[4],0);
    for(volatile uint32_t i = 0;i<1000;i++){}
    if(gpio_get(gpio)==1)
    {
        g_buttonPress=row_pin*5+1;
        
    }
    gpio_put(columns[0],0);
    gpio_put(columns[1],0);
    gpio_put(columns[2],1);
    gpio_put(columns[3],0);
    gpio_put(columns[4],0);
    for(volatile uint32_t i = 0;i<1000;i++){}
    if(gpio_get(gpio)==1)
    {
        g_buttonPress=row_pin*5+2;
        
    }
    gpio_put(columns[0],0);
    gpio_put(columns[1],0);
    gpio_put(columns[2],0);
    gpio_put(columns[3],1);
    gpio_put(columns[4],0);
    for(volatile uint32_t i = 0;i<1000;i++){}
    if(gpio_get(gpio)==1)
    {
        g_buttonPress=row_pin*5+3;

    }
    gpio_put(columns[0],0);
    gpio_put(columns[1],0);
    gpio_put(columns[2],0);
    gpio_put(columns[3],0);
    gpio_put(columns[4],1);
    for(volatile uint32_t i = 0;i<1000;i++){}
    if(gpio_get(gpio)==1)
    {
        g_buttonPress=row_pin*5+4;
    }
    gpio_put(columns[0],1);
    gpio_put(columns[1],1);
    gpio_put(columns[2],1);
    gpio_put(columns[3],1);
    gpio_put(columns[4],1);
    for(volatile uint32_t i = 0;i<10000;i++){}
}
void irq_en(bool en)
{   
    for (int i = 0; i < 6; i++)
    {
        gpio_set_irq_enabled_with_callback(rows[i],GPIO_IRQ_EDGE_RISE,en,&buttonHandler);
    }
}
