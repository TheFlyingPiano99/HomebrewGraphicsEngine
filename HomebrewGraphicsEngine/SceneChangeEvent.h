#pragma once

namespace Hogra {

	struct SceneChangeEvent
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
