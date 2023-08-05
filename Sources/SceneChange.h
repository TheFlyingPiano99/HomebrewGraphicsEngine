/*
* ~~~~~~~~~~~~~~~~~~~~~~~~~
* Homebrew Graphics Engine
* Zoltan Simon 2023
* Repository: https://github.com/TheFlyingPiano99/HomebrewGraphicsEngine
* ~~~~~~~~~~~~~~~~~~~~~~~~~
*/
#pragma once

namespace Hogra {

    struct SceneChange
    {
    public:

        enum class ChangeType {
            noChange,
            restartScene,
            nextScene,
            quit
        };

        ChangeType changeType = ChangeType::noChange;

        int nextSceneId = -1;

    };
}
