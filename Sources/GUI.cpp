#include "GUI.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "GlobalInclude.h"

namespace Hogra {

	GUI* GUI::instance = nullptr;

	void GUI::InitGUI(GLFWwindow* window) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		io = ImGui::GetIO();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, false);
		ImGui_ImplOpenGL3_Init("#version 420");
	}

	void GUI::DestroyGUI() {
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

	void GUI::Draw()
	{
		if (!visible)
			return;
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}