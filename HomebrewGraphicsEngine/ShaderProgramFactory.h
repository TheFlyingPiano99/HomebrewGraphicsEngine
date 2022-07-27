#pragma once
#include "ShaderProgram.h"
#include "AssetFolderPathManager.h"

namespace Hogra {
	class ShaderProgramFactory
	{
	public:
		static ShaderProgramFactory* getInstance();

		static void DestroyInstance();

		ShaderProgram* GetDefaultPBRProgramWithMapping();

		ShaderProgram* GetGlyphProgram();

		ShaderProgram* GetCaptionProgram();

		ShaderProgram* GetEmissiveMaterialProgram();


	private:
		ShaderProgramFactory() = default;
		~ShaderProgramFactory() = default;

		static ShaderProgramFactory* instance;

		ShaderProgram* defaultBPRProgramWithMapping = nullptr;
		ShaderProgram* emissiveMaterial = nullptr;
		ShaderProgram* glyphProgram = nullptr;
		ShaderProgram* captionProgram = nullptr;
	};
}
