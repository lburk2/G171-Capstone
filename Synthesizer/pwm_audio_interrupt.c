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
//#include "c4.h"
#include "pico_keypad4x4.h"

int cur_sample = 0;
uint8_t * audioTable;
uint16_t AUDIO_SAMPLES = 0;
char key;
char prevkey=-1;

int index1=0;
int index2=0;
int mod0;
//uint8_t output[600];//FOR TESTING



uint columns[5] = { 10, 11, 12, 13, 14 };
uint rows[6] = { 15, 16, 17, 18, 19, 20 };
uint matrix[30] = 
{
    1, 2, 3, 4, 0, 
    5, 6, 7, 8, 0, 
    9, 10, 11, 12 , 0, 
    13, 0, 0, 0, 0,
    14, 15, 16, 17, 18,
    19, 20, 21, 22, 23,
};

int gcd(int a, int b)
{
    if (a == 0) 
    {
        return b;
    }
    return gcd(b % a, a);
}
int lcm(int a, int b) 
{
    return (a * b) / gcd(a, b);
}

uint8_t * arrayAdd(uint8_t tone1[], uint8_t tone2[],int len1,int len2 )
{
    mod0=lcm(sizeof(audio_buffer),sizeof(audio_buffer2));
    //printf("lcm: %d\n", mod0);
    uint8_t *outputVal=(uint8_t *) malloc(mod0 * sizeof(uint8_t));
       
    for(int count=0;count<mod0;count++)
    {
        index1=count % len1;
        index2=count % len2;
        outputVal[count]=tone1[index1]+tone2[index2];
        //printf("1:%d , 2:%d ", index1,index2);
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
    pico_keypad_init(columns, rows, matrix);

    while(1) 
    {

        key = pico_keypad_get_key();
        if(key!=prevkey)
        {
            switch(key)
            {
                case 1: 
                AUDIO_SAMPLES=sizeof(audio_buffer);
                printf("buf size %u\n", AUDIO_SAMPLES);
                audioTable=audio_buffer;
                printf("Key pressed: %u\n", key);
                prevkey=key;
                break;

                case 5: 
                AUDIO_SAMPLES=sizeof(audio_buffer2); 
                audioTable=audio_buffer2;
                printf("Key pressed: %u\n", key);
                prevkey=key;
                break;

                case 9:
                {
                printf("Key pressed: %u\n", key);
                
                //add size check
                AUDIO_SAMPLES=sizeof(arrayAdd(audio_buffer, audio_buffer2,sizeof(audio_buffer),sizeof(audio_buffer2)));
        
                audioTable=arrayAdd(audio_buffer, audio_buffer2,sizeof(audio_buffer),sizeof(audio_buffer2));
                // int loop;
                // for(loop = 0; loop < mod0; loop++)
                // printf("%d ", audioTable[loop]);
                // free(audioTable);
                prevkey=key;
                break;
                }

                default: 
                AUDIO_SAMPLES=0;
                printf("Key pressed: %u\n", key);
                prevkey=key;
                break;
            }
        }
    }
}
//right now have to think of a way to sum more than one button press waveforms
//1. they are all different lengths????
//2. maybe setting a flag in each case and if more than one flag is high, sum the waveforms in question??
//3. but there are a ton of a combinations where that can happen, maybe some kind of bit masking?
