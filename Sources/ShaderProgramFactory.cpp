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
					AssetFolderPathManager::getInstance()->getShaderFolderPath().append("DefaultPipeline/instanced3DWithTBN.vert"),
					"",
					AssetFolderPathManager::getInstance()->getShaderFolderPath().append("DefaultPipeline/deferredMappedGeometryPass.frag")
				);
		}
		return defaultBPRProgramWithMapping;
	}

	ShaderProgram* ShaderProgramFactory::GetDeferredPBRProgramForHomogenousMaterial() {
		auto shader = Allocator::New<ShaderProgram>();
		shader->Init(
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("DefaultPipeline/instanced3DWithTBN.vert"),
			"",
			AssetFolderPathManager::getInstance()->getShaderFolderPath().append("DefaultPipeline/deferredHomogeneGeometryPass.frag")
		);
		return shader;
	}

	ShaderProgram* ShaderProgramFactory::GetGlyphProgram()
	{
		if (nullptr == glyphProgram) {
			glyphProgram
				= Allocator::New<ShaderProgram>();
			glyphProgram->Init(
					AssetFolderPathManager::getInstance()->getShaderFolderPath().append("DefaultPipeline/single2DNoViewProj.vert"),
					"",
					AssetFolderPathManager::getInstance()->getShaderFolderPath().append("UI/glyph.frag")
				);
		}
		return glyphProgram;
	}

	ShaderProgram* ShaderProgramFactory::GetForwardCaptionProgram()
	{
		if (nullptr == captionProgram) {
			captionProgram = Allocator::New<ShaderProgram>();
			captionProgram->Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("DefaultPipeline/single2DNoViewProj.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("UI/caption.frag")
			);
		}
		return captionProgram;
	}

	ShaderProgram* ShaderProgramFactory::GetDeferredEmissiveMaterialProgram()
	{
		if (nullptr == emissiveMaterial) {
			emissiveMaterial = Allocator::New<ShaderProgram>();
			emissiveMaterial->Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("DefaultPipeline/instanced3DWithTBN.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("DefaultPipeline/deferredEmissiveGeometryPass.frag")
			);
		}
		return emissiveMaterial;
	}

	ShaderProgram* ShaderProgramFactory::GetForwardTwoAndHalfDimensionalSpriteProgram() {
		if (nullptr == twoAndHaldDimSpriteProgram) {
			twoAndHaldDimSpriteProgram = Allocator::New<ShaderProgram>();
			twoAndHaldDimSpriteProgram->Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("DefaultPipeline/single3DWithTBN.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("DefaultPipeline/forward25DSprite.frag")
			);
		}
		return twoAndHaldDimSpriteProgram;
	}

	ShaderProgram* ShaderProgramFactory::GetForward2DSpriteProgram()
	{
		if (nullptr == twoDimSpriteProgram) {
			twoDimSpriteProgram = Allocator::New<ShaderProgram>();
			twoDimSpriteProgram->Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("DefaultPipeline/single2DNoViewProj.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("DefaultPipeline/forward2DSprite.frag")
			);
		}
		return twoDimSpriteProgram;
	}

	ShaderProgram* ShaderProgramFactory::GetForwardCellShadingProgramWithMapping()
	{
		if (nullptr == forwardCellShadingProgram) {
			forwardCellShadingProgram = Allocator::New<ShaderProgram>();
			forwardCellShadingProgram->Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("DefaultPipeline/single3DWithTBN.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("DefaultPipeline/forwardCellShading.frag")
			);
		}
		return forwardCellShadingProgram;
	}

	ShaderProgram* ShaderProgramFactory::GetForwardCellShadingProgramForHomogenousMaterial()
	{
		if (nullptr == forwardHomogenousCellShadingProgram) {
			forwardHomogenousCellShadingProgram = Allocator::New<ShaderProgram>();
			forwardHomogenousCellShadingProgram->Init(
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("DefaultPipeline/single3DWithTBN.vert"),
				"",
				AssetFolderPathManager::getInstance()->getShaderFolderPath().append("DefaultPipeline/forwardHomogenousCellShading.frag")
			);
		}
		return forwardHomogenousCellShadingProgram;
	}

	void ShaderProgramFactory::ForgetPointers()
	{
		defaultBPRProgramWithMapping = nullptr;
		emissiveMaterial = nullptr;
		glyphProgram = nullptr;
		captionProgram = nullptr;
		twoAndHaldDimSpriteProgram = nullptr;
		twoDimSpriteProgram = nullptr;
		forwardCellShadingProgram = nullptr;
		forwardHomogenousCellShadingProgram = nullptr;
	}

}
