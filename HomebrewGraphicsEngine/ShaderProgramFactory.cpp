#include "ShaderProgramFactory.h"

namespace hograengine {

	ShaderProgramFactory* ShaderProgramFactory::instance = nullptr;

	ShaderProgramFactory* ShaderProgramFactory::getInstance() {
		if (nullptr == instance) {
			instance = new ShaderProgramFactory();
		}
		return instance;
	}
	
	void ShaderProgramFactory::destroyInstance() {
		if (nullptr != instance) {
			delete instance;
		}
		instance = nullptr;
	}
	
	ShaderProgram* ShaderProgramFactory::getDefaultPBRProgramWithMapping() {
		if (nullptr == defaultBPRProgramWithMapping) {
			defaultBPRProgramWithMapping
				= new ShaderProgram(
					AssetFolderPathManager::getInstance()->getShaderFolderPath().append("default.vert"),
					"",
					AssetFolderPathManager::getInstance()->getShaderFolderPath().append("default.frag")
				);
		}
		return defaultBPRProgramWithMapping;
	}

	ShaderProgram* ShaderProgramFactory::getGlyphProgram()
	{
		if (nullptr == glyphProgram) {
			glyphProgram
				= new ShaderProgram(
					AssetFolderPathManager::getInstance()->getShaderFolderPath().append("simple2D.vert"),
					"",
					AssetFolderPathManager::getInstance()->getShaderFolderPath().append("glyph.frag")
				);
		}
		return glyphProgram;
	}

	ShaderProgram* ShaderProgramFactory::getCaptionProgram()
	{
		if (nullptr == captionProgram) {
			captionProgram = new ShaderProgram(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("simple2D.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("caption.frag")
			);
		}
		return captionProgram;
	}

	ShaderProgram* ShaderProgramFactory::getEmissiveMaterialProgram()
	{
		if (nullptr == emissiveMaterial) {
			emissiveMaterial = new ShaderProgram(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("default.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("emissive.frag")
			);

		}
		return emissiveMaterial;
	}

}
