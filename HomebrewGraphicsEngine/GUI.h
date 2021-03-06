#pragma once

#include "imgui_impl_glfw.h"
#include "Scene.h"
namespace Hogra {

	/*
	* Singleton object
	*/
	class GUI
	{
		static GUI* instance;
		ImGuiIO io;

		GUI() {
		}

		bool visible = false;

	public:

		static GUI* getInstance() {
			if (instance == nullptr) {
				instance = new GUI();
			}
			return instance;
		}

		static void DestroyInstance() {
			if (instance != nullptr) {
				delete instance;
				instance = nullptr;
			}
		}

		void initGUI(GLFWwindow* window);

		void destroyGUI();

		void preDrawInit();

		void configToScene(Scene& scene);

		void Draw();

		void setVisible(bool b) {
			visible = b;
		}

		bool isVisible() {
			return visible;
		}

	};

}