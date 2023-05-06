#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/sync.h"
#include "hardware/adc.h"

#define AUDIO_PIN 2
#define ADC_CHANNEL 2

#include "rock.h"
#include "c4.h"
//#include "pico_keypad4x4.h"

#define GPIO_INPUT false
#define GPIO_OUTPUT true

#define ROWS 6
#define COLS 5
#define SIZE_OF_BUFFER 16

int cur_sample = 0;
uint16_t * audioTable;
uint16_t AUDIO_SAMPLES = 0;
uint16_t outputVal[10000]={0}; //check sizes for if this will output, check also the highest possible combo between the sound lengths
char key;
char prevkey=-1;

int index1=0;
int index2=0;
int mod0;
int row_index=0;
int col_index=0;
int row_pin=-1;
int col_pin=-1;
int row_pressed;

uint _columns[5] = { 25, 24, 23, 22, 12 };
uint _rows[6] = { 29, 28, 27, 26, 11, 13 };
uint button_press=0;
int rowState=0;
int pinState=0;
uint8_t buttonValues[30]={0};
uint8_t prevButtonValues[30]={0};
//ring buffer
uint8_t ring_buffer[SIZE_OF_BUFFER]={0};
int bufferLength = 0;
int readIndex = 0;
int writeIndex = 0;

/*void buttonHandler(uint gpio, uint32_t events)
{
    if(GPIO_IRQ_EDGE_RISE & events)
    {
        for (int i = 0; i < ROWS; i++) 
        {
            if (_rows[i] == gpio) 
            {
                row_pin = i;
                break;
            }
        }
        gpio_put(_columns[0],1);
        gpio_put(_columns[1],0);
        gpio_put(_columns[2],0);
        gpio_put(_columns[3],0);
        gpio_put(_columns[4],0);
        for(volatile uint32_t i = 0;i<1000;i++){}
        if(gpio_get(gpio)==1)
        {
            button_press=row_pin*5;
            
        }
        gpio_put(_columns[0],0);
        gpio_put(_columns[1],1);
        gpio_put(_columns[2],0);
        gpio_put(_columns[3],0);
        gpio_put(_columns[4],0);
        for(volatile uint32_t i = 0;i<1000;i++){}
        if(gpio_get(gpio)==1)
        {
            button_press=row_pin*5+1;
            
        }
        gpio_put(_columns[0],0);
        gpio_put(_columns[1],0);
        gpio_put(_columns[2],1);
        gpio_put(_columns[3],0);
        gpio_put(_columns[4],0);
        for(volatile uint32_t i = 0;i<1000;i++){}
        if(gpio_get(gpio)==1)
        {
            button_press=row_pin*5+2;
            
        }
        gpio_put(_columns[0],0);
        gpio_put(_columns[1],0);
        gpio_put(_columns[2],0);
        gpio_put(_columns[3],1);
        gpio_put(_columns[4],0);
        for(volatile uint32_t i = 0;i<1000;i++){}
        if(gpio_get(gpio)==1)
        {
            button_press=row_pin*5+3;

        }
        gpio_put(_columns[0],0);
        gpio_put(_columns[1],0);
        gpio_put(_columns[2],0);
        gpio_put(_columns[3],0);
        gpio_put(_columns[4],1);
        for(volatile uint32_t i = 0;i<1000;i++){}
        if(gpio_get(gpio)==1)
        {
            button_press=row_pin*5+4;
        }
        gpio_put(_columns[0],1);
        gpio_put(_columns[1],1);
        gpio_put(_columns[2],1);
        gpio_put(_columns[3],1);
        gpio_put(_columns[4],1);
        for(volatile uint32_t i = 0;i<10000;i++){}
    }
    
    if(GPIO_IRQ_EDGE_FALL & events)
    {
        for (int i = 0; i < ROWS; i++) 
        {
            if (_rows[i] == gpio) 
            {
                row_pin = i;
                break;
            }
        }
        

    }
}
*/

//void buttonHandler(uint gpio, uint32_t events)
// {
//         for (uint col = 0; col < COLS; col++) {
//         gpio_put(_columns[col], 1); // Set the column to HIGH
//         for (uint row = 0; row < COLS; row++) {
//             if (gpio_get(_rows[row]) == 0) { // Check if button is pressed
//                 button_state[row][col] = 1;
//             }
//             else {
//                 button_state[row][col] = 0;
//             }
//         }
//         gpio_put(_columns[col], 0); // Set the row to LOW
//     }
// }

void *memset(void *str, int c, size_t n);

void pico_keypad_init(uint columns[5], uint rows[6]) 
{
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
        //gpio_set_irq_enabled_with_callback(rows[i],GPIO_IRQ_EDGE_RISE|GPIO_IRQ_EDGE_FALL, 1, &buttonHandler);
    }
}


int gcd(uint16_t a, uint16_t b)
{
    if (a == 0) 
    {
        return b;
    }
    return gcd(b % a, a);
}
int lcm(uint16_t a, uint16_t b) 
{
    return (a * b) / gcd(a, b);
}

uint16_t * arrayAdd(uint16_t tone1[], uint16_t tone2[],uint16_t len1,uint16_t len2 )
{
    mod0=lcm(len1,len2);
       
    for(int count=0;count<mod0;count++)
    {
        index1=count % len1;
        index2=count % len2;
        outputVal[count]=tone1[index1]+tone2[index2];
    }
    
    return outputVal;
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

 bool repeating_timer_callback(struct repeating_timer *t) 
{ 
    //check button states
    for (col_index = 0; col_index < COLS; col_index++) //loop through rows
    {
        gpio_put(_columns[col_index], 1);
        //printf("\n    225\n\n    ");
        for(row_index=0; row_index<ROWS; row_index++)//loop through rows
        {
            rowState=gpio_get(_rows[row_index]);
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
        gpio_put(_columns[col_index], 0);
        //printf("\n    267\n\n    ");
    }
}

int main(void) {
    stdio_init_all();

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
    pico_keypad_init(_columns, _rows);

    struct repeating_timer timer;
    add_repeating_timer_ms(1000, repeating_timer_callback, NULL, &timer);

    memset(buttonValues,0,30);
    memset(prevButtonValues,0,30);
    memset(ring_buffer,0,16*sizeof(ring_buffer[0]));
    
 int prev_state=0;
    while(1) 
    {
        
        // if (bufferLength == 0) {
		// 		printf("\n    Buffer is empty!\n\n    ");
		// 		continue;
		// 	}
        
        //printf("\n    The output value is %d\n\n  read index is%d\n\n  ", ring_buffer[readIndex], readIndex);
    
        //  if(button_press != prev_state)
        //  {
            
        //     prev_state=button_press;
        //    // button_press=-1;
        //  }
         
        // if(button_press!=prevkey)
        // {
        //      printf("230: %u\n", button_press);
        //     switch(button_press)
        //     {
        //         case 1: 
                //  AUDIO_SAMPLES=sizeof(audio_buffer)/sizeof(audio_buffer[0]);
                //  audioTable=audio_buffer;
        //         printf("Key pressed: %u\n", button_press);
        //         prevkey=button_press;
        //         break;

        //         case 5: 
        //         AUDIO_SAMPLES=sizeof(audio_buffer2); 
        //         audioTable=audio_buffer2;
        //         printf("Key pressed: %u\n", button_press);
        //         prevkey=button_press;
        //         break;
                //for(volatile uint32_t i = 0;i<1000000000;i++){}
                // case 9:
                // {
                // printf("Key pressed: %u\n", button_press);
                
                // //add size check between the two buffs
                // audioTable=arrayAdd(audio_buffer, audio_buffer2,(sizeof(audio_buffer)/sizeof(audio_buffer[0])),(sizeof(audio_buffer2)/sizeof(audio_buffer2[0])));
                // AUDIO_SAMPLES=mod0;
                // int loop;
                // printf("start of loop");
                // for(loop = 0; loop < mod0; loop++)
                // {
                // printf("%d ", audioTable[loop]);}
                // for(volatile uint32_t i = 0;i<1000000;i++){}
                // prevkey=button_press;
                // break;
                // }

                // default: 
                // AUDIO_SAMPLES=0;
                // printf("Key pressed: %u\n", button_press);
                // prevkey=button_press;
                // break;
        //     }
        // }
    }
}

