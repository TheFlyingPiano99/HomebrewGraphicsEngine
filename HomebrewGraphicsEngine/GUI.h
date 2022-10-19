#pragma once

#include "imgui_impl_glfw.h"
#include "Scene.h"
namespace Hogra {

	/*
	* Singleton object
	*/
	class GUI
	{
		friend class Allocator;
		static GUI* instance;
		ImGuiIO io;

		GUI() {
		}

		~GUI() {

		}

		bool visible = false;

	public:

		static GUI* getInstance() {
			if (instance == nullptr) {
				instance = Allocator::New<GUI>();
			}
			return instance;
		}

		static void DestroyInstance() {
			Allocator::Delete(instance);
		}

		void InitGUI(GLFWwindow* window);

		void DestroyGUI();

		void preDrawInit();

		void UpdateGUI(Volumetric::VolumeObject& volumeObject);

		void Draw();

		void setVisible(bool b) {
			visible = b;
		}

		bool IsVisible() {
			return visible;
		}

	};

}