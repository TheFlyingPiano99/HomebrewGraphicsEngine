#pragma once

/*
#include "fftw/fftw++.h"
#include "fftw/Complex.h"
#include "fftw/Array.h"
*/
#define DJ_FFT_IMPLEMENTATION
#include "dj_fft.h"
#include <algorithm>
#include <complex>
#include "FFT.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*
* Returns value from [0..1] interval
*/
float rng()
{
    return (double)rand() / (double)RAND_MAX;
}