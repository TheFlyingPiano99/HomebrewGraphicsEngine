#pragma once
#include <vector>
#include "MemoryManager.h"
#include "Camera.h"
#include "FBO.h"
#include "Texture2D.h"
#include "Identifiable.h"

namespace Hogra {

	class Component : public Identifiable
	{
		friend class Allocator;
	public:

		virtual ~Component() = default;

		virtual void BeforePhysicsLoopUpdate() {}

		virtual void EarlyPhysicsUpdate(float dt) {};

		virtual void LatePhysicsUpdate(float dt) {};

		virtual void AfterPhysicsLoopUpdate() {}

		virtual void Draw(FBO& outFBO, const Texture2D& depthTexture, const Camera& camera) {}

		virtual void Serialize() {}

		virtual void UpdateGui() {}

	protected:
	};

}