#include "ShaderProgramFactory.h"

namespace Hogra {

	ShaderProgramFactory* ShaderProgramFactory::instance = nullptr;

	ShaderProgramFactory* ShaderProgramFactory::GetInstance() {
		if (nullptr == instance) {
			instance = Allocator<ShaderProgramFactory>::New();
		}
		return instance;
	}
	
	void ShaderProgramFactory::DestroyInstance() {
		Allocator<ShaderProgramFactory>::Delete(instance);
	}
	
	ShaderProgram* ShaderProgramFactory::GetDefaultPBRProgramWithMapping() {
		if (nullptr == defaultBPRProgramWithMapping) {
			defaultBPRProgramWithMapping = Allocator<ShaderProgram>::New();
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
				= Allocator<ShaderProgram>::New();
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
			captionProgram = Allocator<ShaderProgram>::New();
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
			emissiveMaterial = Allocator<ShaderProgram>::New();
			emissiveMaterial->Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("default.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("emissive.frag")
			);
		}
		return emissiveMaterial;
	}

	ShaderProgram* ShaderProgramFactory::GetSpriteProgram() {
		if (nullptr == spriteProgram) {
			spriteProgram = Allocator<ShaderProgram>::New();
			spriteProgram->Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("default.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("sprite.frag")
			);
		}
		return spriteProgram;
	}

	void ShaderProgramFactory::ForgetPointers()
	{
		defaultBPRProgramWithMapping = nullptr;
		emissiveMaterial = nullptr;
		glyphProgram = nullptr;
		captionProgram = nullptr;
	}

}
