#include "SceneEventImplementation.h"

namespace Hogra {
    void RestartSceneEvent::Execute(Scene& scene)
    {
        SceneChange change;
        change.changeType = SceneChange::ChangeType::restartScene;
        scene.SetSceneChange(change);
    }
}