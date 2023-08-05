#pragma once
#include "ShaderProgram.h"
#include "AssetFolderPathManager.h"
#include "MemoryManager.h"

namespace Hogra {
    class ShaderProgramFactory
    {
        ALLOCATOR_CONSTRUCTIBLE
    public:
        static ShaderProgramFactory* GetInstance();

        static void DestroyInstance();

        ShaderProgram* GetDeferredPBRProgramWithMapping();

        ShaderProgram* GetDeferredPBRProgramForHomogenousMaterial();

        ShaderProgram* GetGlyphProgram();

        ShaderProgram* GetForwardCaptionProgram();

        ShaderProgram* GetDeferredEmissiveMaterialProgram();

        ShaderProgram* GetForwardTwoAndHalfDimensionalSpriteProgram();

        ShaderProgram* GetForward2DSpriteProgram();

        ShaderProgram* GetForwardCellShadingProgramWithMapping();

        ShaderProgram* GetForwardCellShadingProgramForHomogenousMaterial();

        ShaderProgram* GetFlatContainerProgram();

        ShaderProgram* GetTexturedContainerProgram();

        void ForgetPointers();

    private:
        static ShaderProgramFactory* instance;

        ShaderProgram* defaultBPRProgramWithMapping = nullptr;
        ShaderProgram* emissiveMaterial = nullptr;
        ShaderProgram* glyphProgram = nullptr;
        ShaderProgram* captionProgram = nullptr;
        ShaderProgram* twoAndHaldDimSpriteProgram = nullptr;
        ShaderProgram* twoDimSpriteProgram = nullptr;
        ShaderProgram* forwardCellShadingProgram = nullptr;
        ShaderProgram* forwardHomogenousCellShadingProgram = nullptr;
        ShaderProgram* flatContainerProgram = nullptr;
        ShaderProgram* texturedContainerProgram = nullptr;
    };
}
