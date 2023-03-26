#include "stdint.h"
#include "LUT.h"
#include "myNCO.h"
#include "math.h"

/* Global Variables */
static double freqDelta;
uint16_t attFactor;
uint32_t index;
uint32_t pa = 0;

/*
 * nco_set_frequency(double freq)
 *
 * This function sets the frequency of the NCO
 * by calculating the frequency delta
 *
 * freqDelta = (f*2^M)/fs
 *
 */

void nco_set_frequency(double freq)
{
	double fs = 48000.0;
	freqDelta = freq * 4294967295.0 / fs;

}

/*
 * nco_set_attenuation(uint16_t att)
 *
 * This function sets the attenuation factor
 * for the NCO. Valid inputs are 0 (no attenuation)
 * to 16 (full attenuation)
 *
 */
void nco_set_attenuation(uint16_t att)
{
	attFactor = att;
}

/*
 *  nco_run_sinusoid
 *
 *  This function returns the next sample of the
 *  sinusoid stored in the LUT.h file. This function
 *  uses a Phase accumulator (PA) to make the frequency
 *  variable.
 *
 */
int16_t nco_run_sinusoid(void)
{
	//increment PA by freqDelta
	pa = pa + freqDelta;
	//get index from highest 8 bits
	index = pa >> 24;
	//check to see if index has overflowed
	if(index >= 255)
		pa = 0; //reset PA to zero
	// return a 16-bit signed integer
	return sinusoid[index] >> attFactor;

}

/*
 *  nco_run_triangle
 *
 *  This function returns the next sample of the
 *  triangle wave stored in the LUT.h file. This function
 *  uses a Phase accumulator (PA) to make the frequency
 *  variable.
 *
 */
int16_t nco_run_triangle(void)
{

	//increment PA by freqDelta
	pa = pa + freqDelta;
	//get index from highest 8 bits
	index = pa >> 24;
	//check to see if index has overflowed
	if(index >= 255)
		pa = 0; //reset PA to zero
	// return a 16-bit signed integer
	return triangle[index] >> attFactor;

}

