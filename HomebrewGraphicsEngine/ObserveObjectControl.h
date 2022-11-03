#pragma once
#include "UserControl.h"
#include "Camera.h"
#include "volumetric/VolumeObject.h"
#include "Scene.h"
#include "Collider.h"

namespace Hogra {

    class ObserveObjectControl :
        public UserControl
    {
    public:
        ObserveObjectControl();

        ~ObserveObjectControl() override = default;

        void Rotate(const glm::vec2& delta) override;

        void SetCamera(Camera& _camera) {
            camera = &_camera;
        }

        void Zoom(float delta) override;

        void primaryAction() override;

        void grab() override;

        void release() override;

        void grabPlane(float x, float y);

        void releasePlane(float x, float y);


        void SetVolumeObject(Volumetric::VolumeObject& _volumeObject) {
            volumeObject = &_volumeObject;
        }

        void SetScene(Scene* scene) {
            this->scene = scene;
        }

        void AddCollider(AABBCollider* _collider) {
            this->collider = _collider;
        }

    private:

        void DragPlane(float delta);

        Scene* scene = nullptr;
        Camera* camera = nullptr;
        Volumetric::VolumeObject* volumeObject = nullptr;
        float rotationSpeed = 0.1f;
        float zoomSpeed = 4.0f;
        bool isPlaneGrabbed = false;
        glm::vec3 planeNormal;
        glm::vec3 planePosition;
        AABBCollider* collider = nullptr;
    };

}

