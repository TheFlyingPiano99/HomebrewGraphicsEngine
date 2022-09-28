#pragma once

#include "imgui_impl_glfw.h"
#include "Scene.h"
namespace Hogra {

	/*
	* Singleton object
	*/
	class GUI
	{
		friend class Allocator<GUI>;
		static GUI* instance;
		ImGuiIO io;

		GUI() {
		}

		bool visible = false;

	public:

		static GUI* getInstance() {
			if (instance == nullptr) {
				instance = Allocator<GUI>::New();
			}
			return instance;
		}

		static void DestroyInstance() {
			Allocator<GUI>::Delete(instance);
		}

		void initGUI(GLFWwindow* window);

		void destroyGUI();

		void preDrawInit();

		void UpdateGUI(VolumeObject& volumeObject);

		void Draw();

		void setVisible(bool b) {
			visible = b;
		}

		bool isVisible() {
			return visible;
		}

	};

}