#pragma once
#include "ShaderProgram.h"
#include "AssetFolderPathManager.h"

namespace hograengine {
	class ShaderProgramFactory
	{
	public:
		static ShaderProgramFactory* getInstance();

		static void destroyInstance();

		ShaderProgram* getDefaultPBRProgramWithMapping();

		ShaderProgram* getGlyphProgram();

		ShaderProgram* getCaptionProgram();

	private:
		ShaderProgramFactory() = default;
		~ShaderProgramFactory() = default;

		static ShaderProgramFactory* instance;

		ShaderProgram* defaultBPRProgramWithMapping = nullptr;
		ShaderProgram* glyphProgram = nullptr;
		ShaderProgram* captionProgram = nullptr;
	};
}
