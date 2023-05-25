#include "FFT.h"
#include "AssetFolderPathManager.h"


namespace Hogra {

	FFT_3D::FFT_3D()
	{

	}

    constexpr auto Pi = 3.141592653589793238462643383279502884;

    /*
     * Returns offset to most significant bit
     * NOTE: only works for positive power of 2s
     * examples:
     * 1b      -> 0d
     * 100b    -> 2d
     * 100000b -> 5d
     */
    int FFT_3D::FindMSB(int x)
    {
        if (0 > x) {
            throw "Negative input!";
        }
        int p = 0;

        while (x > 1) {
            x >>= 1;
            ++p;
        }

        return p;
    }

    /*
    *  Bit reverse an integer given a word of nb bits
    *  NOTE: Only works for 32-bit words max
    *  examples:
    *  10b      -> 01b
    *  101b     -> 101b
    *  1011b    -> 1101b
    *  0111001b -> 1001110b
    */
    int FFT_3D::Bitr(uint32_t x, int nb)
    {
        x = (x << 16) | (x >> 16);
        x = ((x & 0x00FF00FF) << 8) | ((x & 0xFF00FF00) >> 8);
        x = ((x & 0x0F0F0F0F) << 4) | ((x & 0xF0F0F0F0) >> 4);
        x = ((x & 0x33333333) << 2) | ((x & 0xCCCCCCCC) >> 2);
        x = ((x & 0x55555555) << 1) | ((x & 0xAAAAAAAA) >> 1);

        return ((x >> (32 - nb)) & (0xFFFFFFFF >> (32 - nb)));
    }

	void FFT_3D::Init(unsigned int _cnt)
	{
        this->cnt = _cnt;
		computeProgram.Init(AssetFolderPathManager::getInstance()->getComputeShaderFolderPath().append("gradient3D.comp"));
		texture.InitForCompute(glm::uvec3(cnt), 0, GL_RG32F, GL_RG, GL_FLOAT);
	}

    void FFT_3D::Transform(std::complex<float> input[], std::complex<float> output[], Direction dir)
    {
        float nrm = float(1) / (float(cnt) * std::sqrt(float(cnt)));

        // pre-process the input data
        auto cnt3 = cnt * cnt * cnt;
        for (int i = 0; i < cnt3; i++)
        {
            output[i] = nrm * input[i];     // Normalize
            //std::cout << output[i] << std::endl;
        }

        // upload data to GPU
        texture.WriteData(output);
        computeProgram.Activate();
        texture.BindToImageUnit();
        auto passID = glGetUniformLocation(computeProgram.GetGlID(), "u_PassID");
        computeProgram.SetUniform("u_Dir", float(dir));
        computeProgram.SetUniform("u_ArgSize", cnt);
        computeProgram.SetUniform("u_Arg", 0);

        // run
        int msb = FindMSB(cnt3) / 3; // lg2(N) = lg2(cbrt(NxNxN))
        auto cnt_copy = cnt >> 1;   // division by 2
        int groupCnt = (int)std::ceilf(cnt / 8.0f);
        //computeProgram.SetNumberOfWorkGroups({ groupCnt, groupCnt , groupCnt });
        computeProgram.SetNumberOfWorkGroups({ cnt, cnt, cnt });
        /*
            for (int i = 0; i < msb; ++i) {
                glUniform1i(passID, i);
                computeProgram.Dispatch();
            }
        */
        computeProgram.Dispatch();

        // retrieve data
        texture.ReadData(output);
        std::cout << "Complex byte size: " << sizeof(std::complex<float>) << "; cnt: " << cnt << std::endl;

    }
}
