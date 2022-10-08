#pragma once
#include "UserControl.h"
#include "Camera.h"
#include "volumetric/VolumeObject.h"

namespace Hogra {

    class ObservObjectControl :
        public UserControl
    {
    public:
        ObservObjectControl();

        ~ObservObjectControl() override = default;

        void Rotate(const glm::vec2& delta) override;

        void SetCamera(Camera& _camera) {
            camera = &_camera;
        }

        void Zoom(float delta) override;

        void primaryAction() override;

        void grab() override;

        void release() override;

        void SetVolumeObject(Volumetric::VolumeObject& _volumeObject) {
            volumeObject = &_volumeObject;
        }

    private:
        Camera* camera = nullptr;
        Volumetric::VolumeObject* volumeObject = nullptr;
        float rotationSpeed = 0.1f;
        float zoomSpeed = 0.1f;
    };

}

