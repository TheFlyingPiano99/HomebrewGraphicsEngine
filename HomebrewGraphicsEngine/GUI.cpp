#include "GUI.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GlobalInclude.h"

namespace hograengine {

	GUI* GUI::instance = nullptr;

	void GUI::initGUI(GLFWwindow* window) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		io = ImGui::GetIO();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, false);
		ImGui_ImplOpenGL3_Init("#version 420");
	}

	void GUI::destroyGUI() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void GUI::preDrawInit()
	{
		if (!visible) {
			return;
		}
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

	}

	void GUI::configToScene(Scene& scene)
	{
		if (!visible)
			return;
		char value_buf[64] = {};
		ImGui::Begin("Settings");
		//TODO
		ImGui::End();
	}

	void GUI::draw()
	{
		if (!visible)
			return;
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}