#include "SceneObjectFactory.h"
#include "ComputeProgram.h"

namespace Hogra {
	SceneObjectFactory* Hogra::SceneObjectFactory::GetInstance() {
		if (nullptr == instance) {
			instance = Allocator::New<SceneObjectFactory>();
		}
		return instance;
	}

	SceneObject* SceneObjectFactory::CreateTwoAndHalfDimensionalSpriteObject(const std::filesystem::path& texturePath, OrientationProvider* orientationProvider) {
		auto* geometry = GeometryFactory::GetInstance()->GetQuad();
		auto* program = ShaderProgramFactory::GetInstance()->GetForwardTwoAndHalfDimensionalSpriteProgram();
		auto* material = Allocator::New<Material>();
		material->Init(program);
		material->SetAlphaBlend(false);	// Blending is mot working properly with Deferred layer when sprite is highly transparent.
		auto* texture = Allocator::New<Texture2D>();
		texture->Init(texturePath, 0, GL_RGBA, GL_UNSIGNED_BYTE, false, true);

		material->AddTexture(texture);
				
		auto* mesh = Allocator::New<Mesh>();
		mesh->Init(material, geometry);
		auto* obj = Allocator::New<SceneObject>();
		obj->Init(mesh);

		obj->SetIsCastingShadow(false);
		if (nullptr != orientationProvider) {
			auto* connector = Allocator::New<OrientationConnector>();
			connector->Init(orientationProvider);
			obj->SetOrientationConnector(connector);
		}
		return obj;
	}

	SceneObject* SceneObjectFactory::Create2DSpriteObject(const std::filesystem::path& texturePath, const glm::vec3& pos, float scale)
	{
		auto geometry = GeometryFactory::GetInstance()->GetSimpleQuad();
		auto program = ShaderProgramFactory::GetInstance()->GetForward2DSpriteProgram();
		Material* material = Allocator::New<Material>();
		material->Init(program);
		material->SetAlphaBlend(true);
		Texture2D* texture = Allocator::New<Texture2D>();
		texture->Init(texturePath, 0, GL_RGBA, GL_UNSIGNED_BYTE, false, true);
		material->AddTexture(texture);
		Mesh* mesh = Allocator::New<Mesh>();
		mesh->Init(geometry, material);
		mesh->SetDepthTest(false);
		mesh->setStencilTest(false);
		SceneObject* obj = Allocator::New<SceneObject>();
		obj->Init(mesh);
		obj->SetIsCastingShadow(false);
		obj->SetPosition(pos);
		float screenAspectRatio = (float)GlobalVariables::windowWidth / (float)GlobalVariables::windowHeight;
		float spriteAspectRatio = (float)texture->GetDimensions().x / (float)texture->GetDimensions().y;
		obj->SetScale({ spriteAspectRatio / screenAspectRatio * scale, 1.0f * scale, 1.0f });
		return obj;
	}


	SceneObjectFactory* SceneObjectFactory::instance = nullptr;

}
