#pragma once
#include "SceneEvent.h"
#include <queue>
#include <iostream>
#include "MemoryManager.h"

namespace Hogra {

	class SceneEventManager
	{
		friend class Allocator;
	public:

		~SceneEventManager() {
			while (!events.empty()) {
				auto* e = popNextEvent();
				Allocator::Delete(e);
			}
		}

		static SceneEventManager* getInstance() {
			if (nullptr == instance) {
				instance = Allocator::New<SceneEventManager>();
			}
			return instance;
		}

		static void DestroyInstance() {
			Allocator::Delete(instance);
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
				Allocator::Delete(events.front());
				events.pop();
			}
		}

		void ExecuteQueue(Scene& scene) {
			while (!empty()) {
				SceneEvent* event = popNextEvent();
				event->Execute(scene);
				Allocator::Delete(event);
			}
		}

	private:
		static SceneEventManager* instance;
		std::queue<SceneEvent*> events;

		SceneEventManager() = default;

	};

}