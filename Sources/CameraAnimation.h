#pragma once
#include "Animation.h"
#include "Camera.h"


namespace Hogra {
	class CameraAnimation : public Animation
	{
		ALLOCATOR_CONSTRUCTIBLE
	public:

		Camera* GetCamera() const {
			return camera;
		}

		void setCamera(Camera* _camera) {
			camera = _camera;
		}

	private:
		Camera* camera = nullptr;
	};


	class HeadBob : public CameraAnimation
	{
		ALLOCATOR_CONSTRUCTIBLE
	public:



		Camera* GetCamera() const {
			return camera;
		}

		void setCamera(Camera* _camera) {
			camera = _camera;
		}

		// Inherited via CameraAnimation
		void perform(void* obj, float dt_sec) override;

	private:
		Camera* camera = nullptr;

	};

}

