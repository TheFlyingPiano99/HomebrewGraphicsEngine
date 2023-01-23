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
