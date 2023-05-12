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
#define ROWS 5
#define COLS 5
#define GPIO_INPUT false
#define GPIO_OUTPUT true
#define SIZE_OF_BUFFER 32
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
// #define RE1 27
// #define RE2 29
// #define RE3 26
// #define RE4 28
// #define USER_INTERFACE 25
#define AUDIO_PIN 16
#define ADC_CHANNEL 2
#define ENCODER1_A  0
#define ENCODER1_B  1
#define ENCODER2_A  2
#define ENCODER2_B  3
#define ENCODER3_A  4
#define ENCODER3_B  5
#define ENCODER4_A  6
#define ENCODER4_B  7

int encoder1_count = 0;
int encoder2_count = 0;
int encoder3_count = 0;
int encoder4_count = 0;
uint8_t g_buttonPress = 0; 
uint columns[5] = { 25, 24, 23, 22, 12 };
uint rows[6] = { 29, 28, 27, 26, 11}; //can add 13 back in for rotary encoders
uint button_press=0;
int rowState=0;
int pinState=0;
uint8_t buttonValues[30]={0};
uint8_t prevButtonValues[30]={0};
//ring buffer
int flag=1;
uint8_t ring_buffer[SIZE_OF_BUFFER]={0};
bool bufferLength = 0;
int prevBufferLength=-1;
int readIndex = 0;
int writeIndex = 0;
int changedKey=-1;
int index1=0;
int index2=0;
int mod0;
uint8_t row_index=0;
uint8_t col_index=0;
int row_pin=-1;
int col_pin=-1;
int row_pressed;
int cur_sample = 0;
uint16_t * audioTable;
uint16_t AUDIO_SAMPLES = 0;
uint16_t outputVal[10000]={0}; //check sizes for if this will output, check also the highest possible combo between the sound lengths

uint16_t delay_line[84]={0};
uint8_t regB = 0;
uint8_t regBprev= 0;
MCP23017 gpio_expander;

// Define the size of the ring buffer
#define BUFFER_SIZE 32

// Define the ring buffer structure
typedef struct {
    uint8_t buffer[BUFFER_SIZE]; // Buffer memory
    uint32_t head; // Index of the next element to be added
    uint32_t tail; // Index of the next element to be removed
    uint32_t size; // Size of the buffer in bytes
} ring_buffer_t;
ring_buffer_t ringBuf;


void printMenuOptions();
void menuButtons_init(void);
void irq_en(bool en);
void *memset(void *str, int c, size_t n);
void fir_delay_line(uint8_t *input, uint16_t *output, int nx, int nh, uint8_t *filter);
void pwm_irh();
void ring_buffer_write(ring_buffer_t * buffer, uint8_t data);
uint8_t ring_buffer_read(ring_buffer_t * buffer);
void ring_buffer_init(ring_buffer_t *buffer, uint32_t size);

// ISR for buttons. 
bool repeating_timer_callback(struct repeating_timer *t) 
{ 
    uint8_t index=0;
    //check button states
    for (col_index = 0; col_index < COLS; col_index++) //loop through columns
    {
        gpio_put(columns[col_index], 1);
        for(row_index=0; row_index<ROWS; row_index++)//loop through rows
        {
            rowState=gpio_get(rows[row_index]);
            // if (bufferLength == SIZE_OF_BUFFER) {
			// 	printf("\n    Buffer is full!\n\n    ");
			// 	continue;
			// }
            if(rowState==0) //button pressed
            {
                buttonValues[row_index*5+col_index]=0;
                if(prevButtonValues[row_index*5+col_index]==1)
                {
                    // ring_buffer[writeIndex]=row_index*5+col_index;
                    printf("162\n\n    ");
                    // writeIndex++;
                    index = (row_index*5+col_index);
                    ring_buffer_write(&ringBuf, index); 
                    bufferLength=1;
                    prevButtonValues[row_index*5+col_index]=buttonValues[row_index*5+col_index];
                    break;
                }
               // prevButtonValues[row_index*5+col_index]=buttonValues[row_index*5+col_index];
                
            }
            else //(rowState ==0) //button released
            {
                buttonValues[row_index*5+col_index]=1;
                if(prevButtonValues[row_index*5+col_index]==0)
                {

                    // ring_buffer[writeIndex] =row_index*5+col_index; //store this value into ring buffer sequentially
                    // printf("\n    row state high %d\n\n    ", ring_buffer[writeIndex]);
                    // writeIndex++;
                    index = (row_index*5+col_index);
                    ring_buffer_write(&ringBuf, index); 
                    bufferLength=1;
                    prevButtonValues[row_index*5+col_index]=buttonValues[row_index*5+col_index];
                    break;
                }
                //prevButtonValues[row_index*5+col_index]=buttonValues[row_index*5+col_index];
                
            }
            
            // If at last index in buffer, set writeIndex back to 0
            // if (writeIndex == SIZE_OF_BUFFER) 
            // {
            //     writeIndex = 0;
            // }
        }
        gpio_put(columns[col_index], 0);
        for(int i=0;i<100;i++){}

        // //read rotary encoder pins
        // regB = mcp23017_read_register( &gpio_expander, MCP23017_GPIOB);
        // if (regB != regBprev)
        // {
        //     printf("%d\n", regB);
        //     regBprev = regB;
        // }
        //  // Check the status of each encoder and update the counts accordingly
        // if (!(regBprev & (1 << ENCODER1_A)) && (regBprev & (1 << ENCODER1_B))) {
        //     encoder1_count++;
        // } else if ((regBprev & (1 << ENCODER1_A)) && !(regBprev & (1 << ENCODER1_B))) {
        //     encoder1_count--;
        // }
        
        // if (!(regBprev & (1 << ENCODER2_A)) && (regBprev & (1 << ENCODER2_B))) {
        //     encoder2_count++;
        // } else if ((regBprev & (1 << ENCODER2_A)) && !(regBprev & (1 << ENCODER2_B))) {
        //     encoder2_count--;
        // }

        // if (!(regBprev & (1 << ENCODER3_A)) && (regBprev & (1 << ENCODER3_B))) {
        //     encoder3_count++;
        // } else if ((regBprev & (1 << ENCODER3_A)) && !(regBprev & (1 << ENCODER3_B))) {
        //     encoder3_count--;
        // }

        // if (!(regBprev & (1 << ENCODER4_A)) && (regBprev & (1 << ENCODER4_B))) {
        //     encoder4_count++;
        // } else if ((regBprev & (1 << ENCODER4_A)) && !(regBprev & (1 << ENCODER4_B))) {
        //     encoder4_count--;
        // }

    }
    return true;
}

int main(void){

    FRESULT fr;
    FATFS fs;
    FIL fil;
    //int ret;
    char buf2[10000];
    uint16_t samples1[85]={0};
    uint16_t samples2[80]={0};
    uint16_t samples3[75]={0};
    uint16_t samples4[71]={0};
    uint16_t samples5[67]={0};
    uint16_t samples6[64]={0};
    uint16_t samples7[60]={0};
    uint16_t samples8[57]={0};
    uint16_t samples9[53]={0};
    uint16_t samples10[51]={0};
    uint16_t samples11[48]={0};
    uint16_t samples12[45]={0};
    uint16_t samples13[43]={0};
    uint16_t zeros[85]={0};
    int count = 0,  semicolon_count = 0;

    uint16_t samples[13][100] ={0};
    char* sample;
    char* subsample;
    char filename1[] = "sineWaveOctave.txt";
    char filename2[] = "squareOctave.txt";
    char filename3[] = "sawtoothOctave.txt";
    spi_inst_t *spi = spi0;

    int lowPassEnable=0;

    stdio_init_all();

    struct repeating_timer timer;
    add_repeating_timer_ms(50, repeating_timer_callback, NULL, &timer);
    memset(buttonValues,0,30);
    memset(prevButtonValues,0,30);
    memset(ring_buffer,0,16*sizeof(ring_buffer[0]));
    
    ring_buffer_init(&ringBuf, BUFFER_SIZE);

    //PWM Set up
    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);

    int audio_pin_slice = pwm_gpio_to_slice_num(AUDIO_PIN);

    // Setup PWM interrupt to fire when PWM cycle is complete
    pwm_clear_irq(audio_pin_slice);
    pwm_set_irq_enabled(audio_pin_slice, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_irh);
    irq_set_enabled(PWM_IRQ_WRAP, false);

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
        g_buttonPress= ring_buffer_read(&ringBuf);
        if (g_buttonPress != 69) //ring_buffer_read(&ringBuf) !=0
        { // Check if there is data available to read from the ring buffer
            printf("line 367 %d\n",g_buttonPress);
            
        // if(bufferLength)
        // {
            //bufferLength=0;

            //g_buttonPress=ring_buffer[readIndex];
            // readIndex++;
            // if (readIndex == SIZE_OF_BUFFER) {
            //     readIndex = 0;
            // }
            if(buttonValues[g_buttonPress])
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
                    ring_buffer_init(&ringBuf, 32);
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
                ring_buffer_init(&ringBuf, 32);
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
                ring_buffer_init(&ringBuf, 32);
                g_buttonPress=-1;
                break;
                case DOWN:/* code */
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
                ring_buffer_init(&ringBuf, 32);
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
                ring_buffer_init(&ringBuf, 32);
                g_buttonPress=-1;
                break;
                case SW1:  //c4
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    audioTable=samples1;
                    AUDIO_SAMPLES=85;
                    irq_set_enabled(PWM_IRQ_WRAP, true);
                    break;
                case SW2: //c#
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    AUDIO_SAMPLES=80;
                    audioTable=samples2;
                    irq_set_enabled(PWM_IRQ_WRAP, true);
                    break;
                case SW3: //d
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    AUDIO_SAMPLES=75;
                    audioTable=samples3;
                    irq_set_enabled(PWM_IRQ_WRAP, true);
                    break;
                case SW4: //d#
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    AUDIO_SAMPLES=71;
                    audioTable=samples4;
                    irq_set_enabled(PWM_IRQ_WRAP, true);
                    break;
                case SW5: //e
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    AUDIO_SAMPLES=67;
                    audioTable=samples5;
                    irq_set_enabled(PWM_IRQ_WRAP, true);
                    break;
                case SW6: //f
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    AUDIO_SAMPLES=64;
                    audioTable=samples6;
                    irq_set_enabled(PWM_IRQ_WRAP, true);
                    break;
                case SW7: //f#
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    AUDIO_SAMPLES=60;
                    audioTable=samples7;
                    irq_set_enabled(PWM_IRQ_WRAP, true);
                    break;
                case SW8: //g
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    AUDIO_SAMPLES=57;
                    audioTable=samples8;
                    irq_set_enabled(PWM_IRQ_WRAP, true);
                    break;
                case SW9: //g#
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    AUDIO_SAMPLES=53;
                    audioTable=samples9;
                    irq_set_enabled(PWM_IRQ_WRAP, true);
                    break;
                case SW10: //a
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    AUDIO_SAMPLES=51;
                    audioTable=samples10;
                    irq_set_enabled(PWM_IRQ_WRAP, true);
                    break; //a#
                case SW11: //b
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    AUDIO_SAMPLES=48;
                    audioTable=samples11;
                    irq_set_enabled(PWM_IRQ_WRAP, true);
                    break;
                case SW12: //b#
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    AUDIO_SAMPLES=45;
                    audioTable=samples12;
                    irq_set_enabled(PWM_IRQ_WRAP, true);
                    break;
                case SW13: //c5
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    AUDIO_SAMPLES=43;
                    audioTable=samples13;
                    irq_set_enabled(PWM_IRQ_WRAP, true);
                    break;
                // case RE1:
                //     /* code */
                //     break;
                // case RE2:
                //     /* code */
                //     break;
                // case RE3:
                //     /* code */
                //     break;
                // case RE4:
                //     /* code */
                //     break;
                // case USER_INTERFACE:
                //     /* code */
                //     break;
                default:
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    audioTable = zeros; 
                    AUDIO_SAMPLES=85;
                    break;
                }
                g_buttonPress=-1;
            
            }
            else if(!buttonValues[g_buttonPress])
            {
                switch (g_buttonPress)
                {
                case SW1:  //c4
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    audioTable = zeros; 
                    break;
                case SW2: //c#
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    audioTable = zeros; 
                    break;
                case SW3: //d
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    audioTable = zeros;
                    break;
                case SW4: //d#
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    audioTable = zeros;
                    break;
                case SW5: //e
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    audioTable = zeros;
                    break;
                case SW6: //f
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    audioTable = zeros;
                    break;
                case SW7: //f#
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    audioTable = zeros;
                    break;
                case SW8: //g
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    audioTable = zeros;
                    break;
                case SW9: //g#
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    audioTable = zeros;
                    break;
                case SW10: //a
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    audioTable = zeros;
                    break; //a#
                case SW11: //b
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    audioTable = zeros;
                    break;
                case SW12: //b#
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    audioTable = zeros;
                    break;
                case SW13: //c5
                    irq_set_enabled(PWM_IRQ_WRAP, false);
                    audioTable = zeros; 
                    break;
                default:
                    
                    break;
                }
            }

            bufferLength=0;       
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
                Paint_DrawString_EN(20, 30, "Loading Sinusoids...", RED, WHITE);

                spi_init(spi, 25 * 1000 * 1000);
                
                // Open file for reading
                fr = f_open(&fil, filename1, FA_READ);
                if (fr != FR_OK) {
                    printf("ERROR: Could not open file (%d)\r\n", fr);
                    while (true);
                }
                // Print every line in file over serial
                printf("Reading from file '%s':\r\n", filename1);
                printf("---\r\n");
                
                count = 0,  semicolon_count = 0;
                while (f_gets(buf2, sizeof(buf2), &fil) != NULL) {
                    //printf(buf2);
                    sample = strtok(buf2, ";");
                     while (sample != NULL) {
                        subsample = strtok(sample, ",");
                        while (subsample != NULL) {
                            samples[semicolon_count][count++] = atoi(subsample);
                            subsample = strtok(NULL, ",");
                        }
                        sample = strtok(NULL, ";");
                        semicolon_count++;
                        count = 0;
                    }
                }

                // for(int16_t i = 0; i < 13; i++)
                // {
                //     for (uint j = 0; j < (sizeof(samples[i]) / sizeof(samples[i][0])) && samples[i][j] != 0; j++)
                //     {
                //         printf("%d, ", samples[i][j]);
                //     }
                //     printf("\n\n");
                // }

                f_close(&fil);
                for(int i=0; i<85; i++)
                {
                    samples1[i]=samples[0][i];
                }
                for(int i=0; i<80; i++)
                {
                    samples2[i]=samples[1][i];
                }
                for(int i=0; i<75; i++)
                {
                    samples3[i]=samples[2][i];
                }
                for(int i=0; i<71; i++)
                {
                    samples4[i]=samples[3][i];
                }
                for(int i=0; i<67; i++)
                {
                    samples5[i]=samples[4][i];
                }
                for(int i=0; i<64; i++)
                {
                    samples6[i]=samples[5][i];
                }
                for(int i=0; i<60; i++)
                {
                    samples7[i]=samples[6][i];
                }
                for(int i=0; i<57; i++)
                {
                    samples8[i]=samples[7][i];
                }
                for(int i=0; i<53; i++)
                {
                    samples9[i]=samples[8][i];
                }
                for(int i=0; i<51; i++)
                {
                    samples10[i]=samples[9][i];
                }
                for(int i=0; i<48; i++)
                {
                    samples11[i]=samples[10][i];
                }
                for(int i=0; i<45; i++)
                {
                    samples12[i]=samples[11][i];
                }
                for(int i=0; i<43; i++)
                {
                    samples13[i]=samples[12][i];
                }

                printf("\r\n---\r\n");
                //for(int i=0;i<1000000;i++){}

                f_close(&fil);
                //f_unmount("0:");
                ring_buffer_init(&ringBuf, 32);
                spi_init(spi, 125000000);

                sleep_ms(100);
                menuLocation = 5;

                printMenuOptions();
                irq_en(false);
                Paint_DrawRectangle(39,prevY*20+51,200,prevY*20+51+19,MAGENTA,2,0);
                Paint_DrawRectangle(39,squareY*20+51,200,squareY*20+51+19,BLACK,2,0);
                irq_en(true);

                break;
            case SPECTRUM: 

                LCD_clear(BLUE);
                Paint_DrawString_EN(20, 30, "Loading Square ...", BLUE, WHITE);

                spi_init(spi, 25 * 1000 * 1000);
                
                // Open file for reading
                fr = f_open(&fil, filename2, FA_READ);
                if (fr != FR_OK) {
                    printf("ERROR: Could not open file (%d)\r\n", fr);
                    while (true);
                }
                // Print every line in file over serial
                printf("Reading from file '%s':\r\n", filename2);
                printf("---\r\n");
                
                count = 0,  semicolon_count = 0;
                while (f_gets(buf2, sizeof(buf2), &fil) != NULL) {
                    //printf(buf2);
                    sample = strtok(buf2, ";");
                     while (sample != NULL) {
                        subsample = strtok(sample, ",");
                        while (subsample != NULL) {
                            samples[semicolon_count][count++] = atoi(subsample);
                            subsample = strtok(NULL, ",");
                        }
                        sample = strtok(NULL, ";");
                        semicolon_count++;
                        count = 0;
                    }
                }

                // for(int16_t i = 0; i < 13; i++)
                // {
                //     for (uint j = 0; j < (sizeof(samples[i]) / sizeof(samples[i][0])) && samples[i][j] != 0; j++)
                //     {
                //         printf("%d, ", samples[i][j]);
                //     }
                //     printf("\n\n");
                // }

                f_close(&fil);
                for(int i=0; i<85; i++)
                {
                    samples1[i]=samples[0][i];
                }
                for(int i=0; i<80; i++)
                {
                    samples2[i]=samples[1][i];
                }
                for(int i=0; i<75; i++)
                {
                    samples3[i]=samples[2][i];
                }
                for(int i=0; i<71; i++)
                {
                    samples4[i]=samples[3][i];
                }
                for(int i=0; i<67; i++)
                {
                    samples5[i]=samples[4][i];
                }
                for(int i=0; i<64; i++)
                {
                    samples6[i]=samples[5][i];
                }
                for(int i=0; i<60; i++)
                {
                    samples7[i]=samples[6][i];
                }
                for(int i=0; i<57; i++)
                {
                    samples8[i]=samples[7][i];
                }
                for(int i=0; i<53; i++)
                {
                    samples9[i]=samples[8][i];
                }
                for(int i=0; i<51; i++)
                {
                    samples10[i]=samples[9][i];
                }
                for(int i=0; i<48; i++)
                {
                    samples11[i]=samples[10][i];
                }
                for(int i=0; i<45; i++)
                {
                    samples12[i]=samples[11][i];
                }
                for(int i=0; i<43; i++)
                {
                    samples13[i]=samples[12][i];
                }

                printf("\r\n---\r\n");

                f_close(&fil);
                ring_buffer_init(&ringBuf, 32);
                //f_unmount("0:");
                spi_init(spi, 125000000);

                sleep_ms(100);

                menuLocation = 5;

                printMenuOptions();
                irq_en(false);
                Paint_DrawRectangle(39,prevY*20+51,200,prevY*20+51+19,MAGENTA,2,0);
                Paint_DrawRectangle(39,squareY*20+51,200,squareY*20+51+19,BLACK,2,0);
                irq_en(true);

                break;
            case BEEPS:
                LCD_clear(BLUE);
                Paint_DrawString_EN(20, 30, "Loading Square ...", BLUE, WHITE);

                spi_init(spi, 25 * 1000 * 1000);
                
                // Open file for reading
                fr = f_open(&fil, filename3, FA_READ);
                if (fr != FR_OK) {
                    printf("ERROR: Could not open file (%d)\r\n", fr);
                    while (true);
                }
                // Print every line in file over serial
                printf("Reading from file '%s':\r\n", filename3);
                printf("---\r\n");
                
                count = 0,  semicolon_count = 0;
                while (f_gets(buf2, sizeof(buf2), &fil) != NULL) {
                    //printf(buf2);
                    sample = strtok(buf2, ";");
                     while (sample != NULL) {
                        subsample = strtok(sample, ",");
                        while (subsample != NULL) {
                            samples[semicolon_count][count++] = atoi(subsample);
                            subsample = strtok(NULL, ",");
                        }
                        sample = strtok(NULL, ";");
                        semicolon_count++;
                        count = 0;
                    }
                }

                // for(int16_t i = 0; i < 13; i++)
                // {
                //     for (uint j = 0; j < (sizeof(samples[i]) / sizeof(samples[i][0])) && samples[i][j] != 0; j++)
                //     {
                //         printf("%d, ", samples[i][j]);
                //     }
                //     printf("\n\n");
                // }

                f_close(&fil);
                for(int i=0; i<85; i++)
                {
                    samples1[i]=samples[0][i];
                }
                for(int i=0; i<80; i++)
                {
                    samples2[i]=samples[1][i];
                }
                for(int i=0; i<75; i++)
                {
                    samples3[i]=samples[2][i];
                }
                for(int i=0; i<71; i++)
                {
                    samples4[i]=samples[3][i];
                }
                for(int i=0; i<67; i++)
                {
                    samples5[i]=samples[4][i];
                }
                for(int i=0; i<64; i++)
                {
                    samples6[i]=samples[5][i];
                }
                for(int i=0; i<60; i++)
                {
                    samples7[i]=samples[6][i];
                }
                for(int i=0; i<57; i++)
                {
                    samples8[i]=samples[7][i];
                }
                for(int i=0; i<53; i++)
                {
                    samples9[i]=samples[8][i];
                }
                for(int i=0; i<51; i++)
                {
                    samples10[i]=samples[9][i];
                }
                for(int i=0; i<48; i++)
                {
                    samples11[i]=samples[10][i];
                }
                for(int i=0; i<45; i++)
                {
                    samples12[i]=samples[11][i];
                }
                for(int i=0; i<43; i++)
                {
                    samples13[i]=samples[12][i];
                }

                printf("\r\n---\r\n");

                f_close(&fil);
                ring_buffer_init(&ringBuf, 32);
                //f_unmount("0:");
                spi_init(spi, 125000000);

                //sleep_ms(1000);

                menuLocation = 5;

                printMenuOptions();
                irq_en(false);
                Paint_DrawRectangle(39,prevY*20+51,200,prevY*20+51+19,MAGENTA,2,0);
                Paint_DrawRectangle(39,squareY*20+51,200,squareY*20+51+19,BLACK,2,0);
                irq_en(true);
                break;
            case BOOPS:
                LCD_clear(GREEN);
                lowPassEnable=1;
                if(lowPassEnable)
                {
                    lowPassEnable=0;
                    Paint_DrawString_EN(20, 60, "Low Pass Disabled", GREEN, WHITE);

                }else
                {
                    lowPassEnable=1;
                    Paint_DrawString_EN(20, 60, "Low Pass Enabled", GREEN, WHITE);
                }
                sleep_ms(1000);
                menuLocation = 5;
                printMenuOptions();
                irq_en(false);
                Paint_DrawRectangle(39,prevY*20+51,200,prevY*20+51+19,MAGENTA,2,0);
                Paint_DrawRectangle(39,squareY*20+51,200,squareY*20+51+19,BLACK,2,0);
                irq_en(true);
                break;
            case BOINKS:
                LCD_clear(GRAY);
                lowPassEnable=1;
                if(lowPassEnable)
                {
                    lowPassEnable=0;
                    Paint_DrawString_EN(20, 60, "High Pass Disabled", GRAY, WHITE);
                    
                }else
                {
                    lowPassEnable=1;
                    Paint_DrawString_EN(20, 60, "High Pass Enabled", GRAY, WHITE);
                }
                
                sleep_ms(1000);
                menuLocation = 5;
                printMenuOptions();
                irq_en(false);
                Paint_DrawRectangle(39,prevY*20+51,200,prevY*20+51+19,MAGENTA,2,0);
                Paint_DrawRectangle(39,squareY*20+51,200,squareY*20+51+19,BLACK,2,0);
                irq_en(true);
                break;
            default:
                break;
            }
        }
        if(lowPassEnable)
        {
           // fir_delay_line();
            
        }
    }
}

void ring_buffer_init(ring_buffer_t *buffer, uint32_t size) {
    buffer->head = 0; // Initialize the head index to 0
    buffer->tail = 0; // Initialize the tail index to 0
    buffer->size = size; // Set the size of the buffer
}

// Define the ring buffer write function
void ring_buffer_write(ring_buffer_t * buffer, uint8_t data) {
    uint32_t next_head = (buffer->head + 1) % buffer->size; // Calculate the index of the next head element
    if (next_head != buffer->tail) { // Check if the buffer is full
        printf("in write %d and head %ld\n",data,buffer->head);
        buffer->buffer[buffer->head] = data; // Write the data to the buffer
        buffer->head = next_head; // Update the head index
    }
}

// Define the ring buffer read function
uint8_t ring_buffer_read(ring_buffer_t *buffer) {
    if (buffer->head == buffer->tail) { // Check if the buffer is empty
    
        return 69; // Return 0 if the buffer is empty
    }
    uint8_t data = buffer->buffer[buffer->tail]; // Read the data from the buffer
    buffer->tail = (buffer->tail + 1) % buffer->size; // Update the tail index
    printf("in read %d and tail %ld\n",data,buffer->head);
    return data; // Return the data
}

void printMenuOptions() // prints menu options
{
LCD_clear(MAGENTA);
Paint_DrawString_EN(5, 10, "Welcome to Dynth", MAGENTA, WHITE);
Paint_DrawString_EN(5, 30, "------------------", MAGENTA, WHITE);
Paint_DrawString_EN(5, 50, "- Sinusoid", MAGENTA, WHITE);
Paint_DrawString_EN(5, 70, "- Square Wave", MAGENTA, WHITE);
Paint_DrawString_EN(5, 90, "- Triangle Wave", MAGENTA, WHITE);
Paint_DrawString_EN(5, 110, "- Low-Pass Filter ", MAGENTA, WHITE);
Paint_DrawString_EN(5, 130,"- High Pass Filter ", MAGENTA, WHITE);
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
void fir_delay_line(uint8_t *input, uint16_t *output, int nx, int nh, uint8_t *filter) {
    
    int i, j;
    uint16_t sum;

    for(i = 0; i < nx; i++) {

        // shifting delay line 
        for(j = nx - 1; j > 0; j--) {
            delay_line[j] = delay_line[j - 1];
        }
        delay_line[0] = input[i];

        // doing filtering
        sum = 0;
        for (j = 0; j < nh; j++) {
            sum += filter[j] * delay_line[j];
        }

        output[i] = sum;
    }
}


void irq_en(bool en)
{   
    irq_set_enabled (PWM_IRQ_WRAP, en);
    irq_set_enabled (TIMER_IRQ_0, en);
    irq_set_enabled (TIMER_IRQ_1, en);
    irq_set_enabled (TIMER_IRQ_2, en);
    irq_set_enabled (TIMER_IRQ_3, en);
}
