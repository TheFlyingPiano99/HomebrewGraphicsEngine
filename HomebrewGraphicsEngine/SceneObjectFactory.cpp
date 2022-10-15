#include "SceneObjectFactory.h"

namespace Hogra {
	SceneObjectFactory* Hogra::SceneObjectFactory::GetInstance() {
		if (nullptr == instance) {
			instance = Allocator::New<SceneObjectFactory>();
		}
		return instance;
	}

	SceneObject* SceneObjectFactory::Create2DSpriteObject(const std::string& texturePath, OrientationProvider* orientationProvider) {
		auto* geometry = GeometryFactory::GetInstance()->getFullScreenQuad();
		auto* program = ShaderProgramFactory::GetInstance()->GetSpriteProgram();
		auto* material = Allocator::New<Material>();
		material->Init(program);
		auto* texture = Allocator::New<Texture2D>();
		texture->Init(texturePath, 0, GL_RGBA, GL_UNSIGNED_BYTE);
		material->addTexture(texture);
		auto* mesh = Allocator::New<Mesh>();
		mesh->Init(material, geometry);
		auto* obj = Allocator::New<SceneObject>();
		obj->Init(mesh);

		obj->SetIsCastingShadow(false);
		auto* connector = Allocator::New<OrientationConnector>();
		connector->Init(orientationProvider);
		obj->SetOrientationConnector(connector);
		return obj;
	}


	SceneObjectFactory* SceneObjectFactory::instance = nullptr;

}
