#pragma once
#include <vector>
#include <complex>
#include "ComputeProgram.h"
#include "Texture3D.h"
#include <glm/glm.hpp>

/* 
This FFT implementation is part of Hogra engine
and it is a direct adaptation of the
dj_fft.h - public domain FFT library
by Jonathan Dupuy
*/

namespace Hogra {
	class FFT_3D
	{
		ALLOCATOR_CONSTRUCTIBLE
	public:
		FFT_3D();

		void Init(unsigned int cnt);

		enum class Direction {
			Forward = +1,
			Backward = -1
		};

		void Transform(std::complex<float> input[], std::complex<float> output[], Direction dir);

	private:
		int cnt = 0;
		ComputeProgram computeProgram;
		Texture3D texture;

		int FindMSB(int x);

		int Bitr(uint32_t x, int nb);

	};
}

