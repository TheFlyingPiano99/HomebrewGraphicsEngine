#pragma once
#include "ShaderProgram.h"
#include "AssetFolderPathManager.h"
#include "MemoryManager.h"

namespace Hogra {
	class ShaderProgramFactory
	{
		friend class Allocator;
	public:
		static ShaderProgramFactory* GetInstance();

		static void DestroyInstance();

		ShaderProgram* GetDeferredPBRProgramWithMapping();

		ShaderProgram* GetGlyphProgram();

		ShaderProgram* GetForwardCaptionProgram();

		ShaderProgram* GetDeferredEmissiveMaterialProgram();

		ShaderProgram* GetForwardSpriteProgram();

		void ForgetPointers();

	private:
		ShaderProgramFactory() = default;
		~ShaderProgramFactory() = default;

		static ShaderProgramFactory* instance;

		ShaderProgram* defaultBPRProgramWithMapping = nullptr;
		ShaderProgram* emissiveMaterial = nullptr;
		ShaderProgram* glyphProgram = nullptr;
		ShaderProgram* captionProgram = nullptr;
		ShaderProgram* spriteProgram = nullptr;
	};
}
