#include "stdint.h"

// Funciton Declarations
extern void nco_set_frequency(double freq); // sets NCO frequency
extern void nco_set_attenuation(uint16_t att); // sets attenuation factor
extern int16_t nco_run_sinusoid(void);	// returns next sample of a sinusoid
extern int16_t nco_run_triangle(void);	// returns next sameple of a triangle wave

