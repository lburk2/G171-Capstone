#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "LCD.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"
#include "hardware/sync.h"
#include "hardware/adc.h"
#include "imageN.h"
// #include "font24.h"
#include "OLED.h"
#include "sd_card.h"
#include "ff.h"
#include "MCP23017.h"

#define MainMenu 5
#define SD 0
#define SPECTRUM 1
#define BEEPS 2
#define BOOPS 3
#define BOINKS 4
#define ROWS 6
#define COLS 5
#define GPIO_INPUT false
#define GPIO_OUTPUT true
#define SIZE_OF_BUFFER 16
#define SW1 0
#define SW2 5
#define SW3 10
#define SW4 15
#define SW5 1
#define SW6 6
#define SW7 11
#define SW8 2
#define SW9 7
#define SW10 12
#define SW11 3
#define SW12 8
#define SW13 13
#define UP 20
#define DOWN 24
#define LEFT 22
#define RIGHT 23
#define CENTER 21
#define RE1 27
#define RE2 29
#define RE3 26
#define RE4 28
#define USER_INTERFACE 25
#define AUDIO_PIN 2
#define ADC_CHANNEL 2
int g_buttonPress = 0; 
uint columns[5] = { 25, 24, 23, 22, 12 };
uint rows[6] = { 29, 28, 27, 26, 11, 13 };
uint button_press=0;
int rowState=0;
int pinState=0;
uint8_t buttonValues[30]={0};
uint8_t prevButtonValues[30]={0};
//ring buffer
int flag=1;
uint8_t ring_buffer[SIZE_OF_BUFFER]={0};
int bufferLength = 0;
int readIndex = 0;
int writeIndex = 0;
int changedKey=-1;
int index1=0;
int index2=0;
int mod0;
int row_index=0;
int col_index=0;
int row_pin=-1;
int col_pin=-1;
int row_pressed;
int cur_sample = 0;
uint16_t * audioTable;
uint16_t AUDIO_SAMPLES = 0;
uint16_t outputVal[10000]={0}; //check sizes for if this will output, check also the highest possible combo between the sound lengths


MCP23017 gpio_expander;

void printMenuOptions();
void menuButtons_init(void);
void irq_en(bool en);
//bool repeating_timer_callback(struct repeating_timer *t); 
void *memset(void *str, int c, size_t n);
void pwm_irh();

// ISR for buttons. 
bool repeating_timer_callback(struct repeating_timer *t) 
{ 
    //printf("line 664   ");
    //check button states
    for (col_index = 0; col_index < COLS; col_index++) //loop through rows
    {
       
        gpio_put(columns[col_index], 1);
        
        //printf("\n    225\n\n    ");
        for(row_index=0; row_index<ROWS; row_index++)//loop through rows
        {
            rowState=gpio_get(rows[row_index]);
            //printf("\n    rowstate: %d\n\n    ",rowState);
            // if (bufferLength == SIZE_OF_BUFFER) {
			// 	printf("\n    Buffer is full!\n\n    ");
			// 	continue;
			// }
            if(rowState ==1) //button pressed
            {
                buttonValues[row_index*5+col_index]=1;
                //printf("\n    237\n\n    ");
                if(prevButtonValues[row_index*5+col_index]==0)
                {
                    ring_buffer[writeIndex] =row_index*5+col_index; //store this value into ring buffer sequentially
                    printf("\n    row state 1 %d\n\n    ", ring_buffer[writeIndex]);
                   
                     //bufferLength++;
                }
            }
            if(rowState ==0) //button released
            {
                buttonValues[row_index*5+col_index]=0;
                //printf("\n    249\n\n    ");
                if(prevButtonValues[row_index*5+col_index]==1)
                {
                    ring_buffer[writeIndex]=row_index*5+col_index;
                     printf("\n    row state 0 %d\n\n    ", ring_buffer[writeIndex]);
                    
                     //bufferLength++;
                }
            }
            prevButtonValues[row_index*5+col_index]=buttonValues[row_index*5+col_index];
           
            writeIndex++;
            // If at last index in buffer, set writeIndex back to 0
            if (writeIndex == SIZE_OF_BUFFER) 
            {
                writeIndex = 0;
            }
        }
        gpio_put(columns[col_index], 0);
        //printf("\n    267\n\n    ");
    }
    //return false;
}

int main(void){

    FRESULT fr;
    FATFS fs;
    FIL fil;
    //int ret;
    char buf2[10000];
    uint16_t samples1[84];
    uint16_t samples2[80];
    uint16_t samples3[76];
    uint16_t samples4[73];
    uint16_t samples5[70];
    uint16_t samples6[68];
    uint16_t samples7[65];
    uint16_t samples8[63];
    uint16_t samples9[60];
    uint16_t samples10[59];
    uint16_t samples11[57];
    uint16_t samples12[55];
    uint16_t samples13[54];

    char* sample;
    char filename[] = "sineWaveOctave.txt";
    spi_inst_t *spi = spi0;

    stdio_init_all();

        //ISR for checking buttons
    struct repeating_timer timer;
    add_repeating_timer_ms(100, repeating_timer_callback, NULL, &timer);
    memset(buttonValues,0,30);
    memset(prevButtonValues,0,30);
    memset(ring_buffer,0,16*sizeof(ring_buffer[0]));

    //PWM Set up
    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);

    int audio_pin_slice = pwm_gpio_to_slice_num(AUDIO_PIN);

    // Setup PWM interrupt to fire when PWM cycle is complete
    pwm_clear_irq(audio_pin_slice);
    pwm_set_irq_enabled(audio_pin_slice, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_irh);
    irq_set_enabled(PWM_IRQ_WRAP, true);

    // Setup PWM for audio output
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 5.5f);
    pwm_config_set_wrap(&config, 254);
    pwm_init(audio_pin_slice, &config, true);

    pwm_set_gpio_level(AUDIO_PIN, 0);
    
    // end PWM setup

    //init button matrix
    menuButtons_init();

    LCD_init();
    Paint_DrawImage(gImage_imageN, 0,0,LCD_WIDTH,LCD_HEIGHT);
    LCD_Write_Command(0x10);
    Paint_DrawRectangle(0,180,320,240,WHITE,2,1);
    LCD_Write_Command(0x11);
    Paint_DrawRectangle(30,180,290,200,BLACK,2,0);
    Paint_DrawRectangle(32,182,96,198,RED,2,1);  
    
    sleep_ms(500);
    //menuButtons_init(); 
    mcp23017_init(&gpio_expander, 0b00000111);
    Paint_DrawRectangle(32,182,160,198,RED,2,1);
    
    sleep_ms(500);
    oled_init();
    Paint_DrawRectangle(32,182,224,198,RED,2,1);

    sleep_ms(500);
    Paint_DrawRectangle(32,182,288,198,RED,2,1);

    // Initialize SD card
    if (!sd_init_driver()) {
        printf("ERROR: Could not initialize SD card\r\n");
        //while (true);
    }

    // Mount drive
    fr = f_mount(&fs, "0:", 1);
    if (fr != FR_OK) {
        printf("ERROR: Could not mount filesystem (%d)\r\n", fr);
        //while (true);
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
        g_buttonPress=ring_buffer[readIndex];
        //printf("line 228 %d\n", g_buttonPress);
        if(buttonValues[g_buttonPress])
        {
            
            if(g_buttonPress!=-1)
            {
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
                g_buttonPress=-1;
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
            g_buttonPress=-1;
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
            g_buttonPress=-1;
            break;
            case DOWN:/* code */
            printf("369\n");
                switch (menuLocation)
                {
                case MainMenu:
                    if(squareY < 4)//lower the box
                    {
                        prevY = squareY;
                        squareY++;
                        printf("in down %d\n", squareY);
                        irq_en(false);
                        Paint_DrawRectangle(39,prevY*20+51,200,prevY*20+51+19,MAGENTA,2,0);
                        Paint_DrawRectangle(39,squareY*20+51,200,squareY*20+51+19,BLACK,2,0);
                        irq_en(true);
                    }
                    break;
                
                default:
                    break;
                }
            g_buttonPress=-1;
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
            g_buttonPress=-1;
            break;


            case SW1:  //c4
                audioTable=samples1;
                AUDIO_SAMPLES=sizeof(samples1/sizeof(samples1[1]));
                break;
            case SW2: //c#
                /* code */
                break;
            case SW3: //d
                /* code */
                break;
            case SW4: //d#
                /* code */
                break;
            case SW5: //e
                /* code */
                break;
            case SW6: //f
                /* code */
                break;
            case SW7: //f#
                /* code */
                break;
            case SW8: //g
                /* code */
                break;
            case SW9: //g#
                /* code */
                break;
            case SW10: //a
                /* code */
                break; //a#
            case SW11: //b
                /* code */
                break;
            case SW12: //b#
                /* code */
                break;
            case SW13: //c5
                /* code */
                break;
            case RE1:
                /* code */
                break;
            case RE2:
                /* code */
                break;
            case RE3:
                /* code */
                break;
            case RE4:
                /* code */
                break;
            case USER_INTERFACE:
                /* code */
                break;
            default:
            //printf("in default, key is %d\n", g_buttonPress);
                break;
            }
            g_buttonPress=-1;
            }
        }
        bufferLength--;	 //	Decrease buffer size after reading
        readIndex++;	 //	Increase readIndex position to prepare for next read

        // If at last index in buffer, set readIndex back to 0
        if (readIndex == SIZE_OF_BUFFER) {
            readIndex = 0;
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
        gpio_put(columns[i], 0);
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



void pwm_irh() {
    pwm_clear_irq(pwm_gpio_to_slice_num(AUDIO_PIN));
    pwm_set_gpio_level(AUDIO_PIN, audioTable[cur_sample >> 2]);


    if (cur_sample < (AUDIO_SAMPLES * 4) - 1) {
        ++cur_sample;
       
    } else {
        cur_sample = 0;
    }
}

void irq_en(bool en)
{   
    // irq_set_enabled (PWM_IRQ_WRAP, en);
    // irq_set_enabled (TIMER_IRQ_0, en);
    // irq_set_enabled (TIMER_IRQ_1, en);
    // irq_set_enabled (TIMER_IRQ_2, en);
    // irq_set_enabled (TIMER_IRQ_3, en);
}
