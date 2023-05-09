// A simple delay line filter guy for our FIRs
// poop poop butt fart
// 5/9/23 implementation
// Evan Cornwell & chatGPT 

// input is input array
// output is output array
// nx is number of input samples (we choose this)
// nh is number of filt coeffs, either HIPASS_LENGTH or LOPASS_LENGTH
// filter is the array itself. i.e. "HIPASS_CUT_1000" or, "LOPASS_CUT_0_750"

#include <stdint.h>
#include "lopass_coeffs.h"
#include "hipass_coeffs.h"

uint16_t delay_line[];

// try this code out. Should let you pick how many samples you put in (to see how much el pico can handel)

void fir_delay_line(uint8_t *input, uint16_t *output, int nx, int nh, uint8_t *filter) {
    
    int i, j;
    uint16_t sum;

    // should this bee global?
    delay_line[nx - 1] = {0};

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




