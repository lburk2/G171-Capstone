#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"
#include "hardware/sync.h"
#include "hardware/adc.h"
#include "hardware/timer.h"
#include "pwm_audio_setup.h"
#include "pico_keypad4x4.h"
#include "rock.h"
#include "LUT.h"
#include "myNCO.h"

int main(void)
{
    dma_irh(audio_buffer);
    pwm_audio_setup(AUDIO_SAMPLES, 2, 2, 4, audio_buffer);

    // Kick things off with the trigger DMA channel
    dma_channel_start(trigger_dma_chan);

    while(1) {
        __wfi(); //need to make this more specific?
    }
}
    

// uint columns[5] = { 22, 23, 24, 25 };//change to correct pins
// uint rows[6] = { 26, 27, 28, 29 };
// uint8_t matrix[30] = 
// {
//     '1', '2', '3', '4','5', 
//     '6', '7', '8','9', '10', 
//     '11', '12','13', '0' , '0', 
//     '0', '0', '0', '0', '0',
//     '0', '0', '0', '0', '0',
//     '0', '0', '0', '0', '0',
// };

// int main() 
// {
//     stdio_init_all();
//     pico_keypad_init(columns, rows, matrix);

//     int key = pico_keypad_get_key();
//     switch(key)
//     {
//         case 1:
//             //output c
//             break;
//         case 2:
//         //output c#
//             break;
//         case 3:
//         //output d
//             break;
//         case 4:
//         //output d#
//             break;
//         case 5:
//         //output e
//             break;
//         case 6:
//         //output f
//             break;
//         case 7:
//         //output f#
//             break;
//         case 8:
//         //output g
//             break;
//         case 9:
//         //output g#
//             break;
//         case 10:
//         //output a
//             break;
//         case 11:
//         //output Bb
//             break;
//         case 12:
//         //output b
//             break;
//         case 13:
//         //output c
//             break;
//         default:
//             //speaker not outputting
            
//     }
    
// }
