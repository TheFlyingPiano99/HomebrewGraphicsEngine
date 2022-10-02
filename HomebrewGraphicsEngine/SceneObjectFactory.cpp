#include "SceneObjectFactory.h"

namespace Hogra {
	SceneObjectFactory* Hogra::SceneObjectFactory::GetInstance() {
		if (nullptr == instance) {
			instance = Allocator<SceneObjectFactory>::New();
		}
		return instance;
	}

	SceneObject* SceneObjectFactory::Create2DSpriteObject(const std::string& texturePath, OrientationProvider* orientationProvider) {
		auto* geometry = GeometryFactory::GetInstance()->getFullScreenQuad();
		auto* program = ShaderProgramFactory::GetInstance()->GetSpriteProgram();
		auto* material = Allocator<Material>::New();
		material->Init(program);
		auto* texture = Allocator<Texture2D>::New();
		texture->Init(texturePath, 0, GL_RGBA, GL_UNSIGNED_BYTE);
		material->addTexture(texture);
		auto* mesh = Allocator<Mesh>::New();
		mesh->Init(material, geometry);
		auto* obj = Allocator<SceneObject>::New();
		obj->Init(mesh);

		obj->SetIsCastingShadow(false);
		auto* connector = Allocator<OrientationConnector>::New();
		connector->Init(orientationProvider);
		obj->SetOrientationConnector(connector);
		return obj;
	}


	SceneObjectFactory* SceneObjectFactory::instance = nullptr;

}
