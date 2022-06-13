#pragma once
#include "SceneEvent.h"
#include <queue>
class SceneEventManager
{
	static SceneEventManager* instance;
	std::queue<SceneEvent*> events;

	SceneEventManager() = default;

public:

	static SceneEventManager* getInstance() {
		if (nullptr == instance) {
			instance = new SceneEventManager();
		}
		return instance;
	}

	static void destroyInstance() {
		if (nullptr != instance) {
			delete instance;
			instance = nullptr;
		}
	}


	void pushEvent(SceneEvent* event) {
		events.push(event);
	}

	bool empty() const {
		return events.empty();
	}
	
	SceneEvent* popNextEvent() {
		auto* e = events.front();
		events.pop();
		return e;
	}

	void clearQueue() {
		while(!events.empty()) {
			delete events.front();
			events.pop();
		}
	}

	void executeQueue(float dt) {
		while (!empty()) {
			SceneEvent* event = popNextEvent();
			event->execute(dt);
		}
	}
};

