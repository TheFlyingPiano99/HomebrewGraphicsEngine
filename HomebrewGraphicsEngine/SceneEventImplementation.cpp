#include "SceneEventImplementation.h"

namespace Hogra {
    void RestartSceneEvent::execute(Scene* scene, float dt)
    {
        SceneChange change;
        change.changeType = SceneChange::ChangeType::restartScene;
        scene->SetSceneChange(change);
    }
}