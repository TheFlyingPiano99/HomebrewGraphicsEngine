#pragma once
#include "SceneEvent.h"
#include <queue>
#include <iostream>
#include "MemoryManager.h"

namespace Hogra {

	class SceneEventManager
	{
		friend class Allocator<SceneEventManager>;
	public:

		~SceneEventManager() {
			while (!events.empty()) {
				auto* e = popNextEvent();
				delete e;
			}
		}

		static SceneEventManager* getInstance() {
			if (nullptr == instance) {
				instance = Allocator<SceneEventManager>::New();
			}
			return instance;
		}

		static void DestroyInstance() {
			Allocator<SceneEventManager>::Delete(instance);
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

		void ClearQueue() {
			while (!events.empty()) {
				delete events.front();
				events.pop();
			}
		}

		void ExecuteQueue(Scene& scene) {
			while (!empty()) {
				SceneEvent* event = popNextEvent();
				event->Execute(scene);
				delete event;
			}
		}
	private:
		static SceneEventManager* instance;
		std::queue<SceneEvent*> events;

		SceneEventManager() = default;

	};

}