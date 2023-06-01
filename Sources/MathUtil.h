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

inline constexpr double m_pi = 3.141592653589793238462643383279502884;

/*
* Returns value from [0..1] interval
*/
float rng()
{
    return (double)rand() / (double)RAND_MAX;
}