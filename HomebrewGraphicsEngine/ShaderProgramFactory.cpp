#include "ShaderProgramFactory.h"

namespace Hogra {

	ShaderProgramFactory* ShaderProgramFactory::instance = nullptr;

	ShaderProgramFactory* ShaderProgramFactory::getInstance() {
		if (nullptr == instance) {
			instance = new ShaderProgramFactory();
		}
		return instance;
	}
	
	void ShaderProgramFactory::DestroyInstance() {
		if (nullptr != instance) {
			delete instance;
		}
		instance = nullptr;
	}
	
	ShaderProgram* ShaderProgramFactory::GetDefaultPBRProgramWithMapping() {
		if (nullptr == defaultBPRProgramWithMapping) {
			defaultBPRProgramWithMapping
				= new ShaderProgram();
			defaultBPRProgramWithMapping->Init(
					AssetFolderPathManager::getInstance()->getShaderFolderPath().append("default.vert"),
					"",
					AssetFolderPathManager::getInstance()->getShaderFolderPath().append("default.frag")
				);
		}
		return defaultBPRProgramWithMapping;
	}

	ShaderProgram* ShaderProgramFactory::GetGlyphProgram()
	{
		if (nullptr == glyphProgram) {
			glyphProgram
				= new ShaderProgram();
			glyphProgram->Init(
					AssetFolderPathManager::getInstance()->getShaderFolderPath().append("simple2D.vert"),
					"",
					AssetFolderPathManager::getInstance()->getShaderFolderPath().append("glyph.frag")
				);
		}
		return glyphProgram;
	}

	ShaderProgram* ShaderProgramFactory::GetCaptionProgram()
	{
		if (nullptr == captionProgram) {
			captionProgram = new ShaderProgram();
			captionProgram->Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("simple2D.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("caption.frag")
			);
		}
		return captionProgram;
	}

	ShaderProgram* ShaderProgramFactory::GetEmissiveMaterialProgram()
	{
		if (nullptr == emissiveMaterial) {
			emissiveMaterial = new ShaderProgram();
			emissiveMaterial->Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("default.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("emissive.frag")
			);

		}
		return emissiveMaterial;
	}

}
