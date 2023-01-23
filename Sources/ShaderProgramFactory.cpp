#include "ShaderProgramFactory.h"

namespace Hogra {

	ShaderProgramFactory* ShaderProgramFactory::instance = nullptr;

	ShaderProgramFactory* ShaderProgramFactory::GetInstance() {
		if (nullptr == instance) {
			instance = Allocator::New<ShaderProgramFactory>();
		}
		return instance;
	}
	
	void ShaderProgramFactory::DestroyInstance() {
		Allocator::Delete(instance);
	}
	
	ShaderProgram* ShaderProgramFactory::GetDefaultPBRProgramWithMapping() {
		if (nullptr == defaultBPRProgramWithMapping) {
			defaultBPRProgramWithMapping = Allocator::New<ShaderProgram>();
			defaultBPRProgramWithMapping->Init(
					AssetFolderPathManager::getInstance()->getShaderFolderPath().append("defaultDeferred.vert"),
					"",
					AssetFolderPathManager::getInstance()->getShaderFolderPath().append("defaultDeferred.frag")
				);
		}
		return defaultBPRProgramWithMapping;
	}

	ShaderProgram* ShaderProgramFactory::GetGlyphProgram()
	{
		if (nullptr == glyphProgram) {
			glyphProgram
				= Allocator::New<ShaderProgram>();
			glyphProgram->Init(
					AssetFolderPathManager::getInstance()->getShaderFolderPath().append("simple2D.vert"),
					"",
					AssetFolderPathManager::getInstance()->getShaderFolderPath().append("glyph.frag")
				);
		}
		return glyphProgram;
	}

	ShaderProgram* ShaderProgramFactory::GetForwardCaptionProgram()
	{
		if (nullptr == captionProgram) {
			captionProgram = Allocator::New<ShaderProgram>();
			captionProgram->Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("simple2D.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("caption.frag")
			);
		}
		return captionProgram;
	}

	ShaderProgram* ShaderProgramFactory::GetDeferredEmissiveMaterialProgram()
	{
		if (nullptr == emissiveMaterial) {
			emissiveMaterial = Allocator::New<ShaderProgram>();
			emissiveMaterial->Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("defaultDeferred.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("emissive.frag")
			);
		}
		return emissiveMaterial;
	}

	ShaderProgram* ShaderProgramFactory::GetForwardSpriteProgram() {
		if (nullptr == spriteProgram) {
			spriteProgram = Allocator::New<ShaderProgram>();
			spriteProgram->Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("defaultForward.vert"),
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
