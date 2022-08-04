#pragma once
#include "SceneEvent.h"
#include <queue>
#include <iostream>

namespace Hogra {

	class SceneEventManager
	{
		static SceneEventManager* instance;
		std::queue<SceneEvent*> events;

		SceneEventManager() = default;

	public:

		~SceneEventManager() {
			while (!events.empty()) {
				auto* e = popNextEvent();
				delete e;
			}
		}

		static SceneEventManager* getInstance() {
			if (nullptr == instance) {
				instance = new SceneEventManager();
			}
			return instance;
		}

		static void DestroyInstance() {
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
			while (!events.empty()) {
				delete events.front();
				events.pop();
			}
		}

		void executeQueue(float dt) {
			while (!empty()) {
				SceneEvent* event = popNextEvent();
				event->execute(dt);
				delete event;
			}
		}
	};

}