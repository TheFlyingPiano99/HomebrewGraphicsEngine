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

		ShaderProgram* GetForwardSpriteProgram();

		void ForgetPointers();

	private:
		static ShaderProgramFactory* instance;

		ShaderProgram* defaultBPRProgramWithMapping = nullptr;
		ShaderProgram* emissiveMaterial = nullptr;
		ShaderProgram* glyphProgram = nullptr;
		ShaderProgram* captionProgram = nullptr;
		ShaderProgram* spriteProgram = nullptr;
	};
}
