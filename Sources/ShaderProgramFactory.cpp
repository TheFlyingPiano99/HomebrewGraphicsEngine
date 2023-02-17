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
	
	ShaderProgram* ShaderProgramFactory::GetDeferredPBRProgramWithMapping() {
		if (nullptr == defaultBPRProgramWithMapping) {
			defaultBPRProgramWithMapping = Allocator::New<ShaderProgram>();
			defaultBPRProgramWithMapping->Init(
					AssetFolderPathManager::getInstance()->getShaderFolderPath().append("instanced3DWithTBN.vert"),
					"",
					AssetFolderPathManager::getInstance()->getShaderFolderPath().append("deferredMappedGeometryPass.frag")
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
					AssetFolderPathManager::getInstance()->getShaderFolderPath().append("single2D.vert"),
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
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("single2D.vert"),
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
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("instanced3DWithTBN.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("deferredEmissiveGeometryPass.frag")
			);
		}
		return emissiveMaterial;
	}

	ShaderProgram* ShaderProgramFactory::GetForwardSpriteProgram() {
		if (nullptr == spriteProgram) {
			spriteProgram = Allocator::New<ShaderProgram>();
			spriteProgram->Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("single3DWithTBN.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("forwardSprite.frag")
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
