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

}
