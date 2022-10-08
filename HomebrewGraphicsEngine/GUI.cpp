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


	void GUI::UpdateGUI(Volumetric::VolumeObject& volumeObject)
	{
		if (!visible) {
			return;
		}

		static bool configFeature = false;
		static bool configSTF = false;
		static bool addFeatureToGroup = false;
		static Volumetric::Feature* featureToAdd = nullptr;

		char value_buf[64] = {};
		ImGui::Begin("Settings");
		ImGui::SliderFloat((const char*)"Light power", &volumeObject.GetLightPower(), 1.0f, 10000.0f);
		ImGui::SliderFloat("Opacity scale", &volumeObject.GetOpacityScale(), 1.0, 100.0);

		const char* current_item = volumeObject.GetCurrentTransferRegionSelectMode();
		const char** items = volumeObject.GetTransferRegionSelectModes();
		if (ImGui::BeginCombo("Transfer region select mode", current_item)) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < TRANSFER_MODE_COUNT; n++)
			{
				bool is_selected = (current_item == items[n]); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(items[n], is_selected)) {
					current_item = items[n];
					volumeObject.SetCurrentTransferRegionSelectModes(current_item);
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
				}
			}
			ImGui::EndCombo();
		}

		current_item = (volumeObject.GetSelectedFeatureGroup() != nullptr) ?
			volumeObject.GetSelectedFeatureGroup()->name.c_str() : "Select group";
		std::vector<Volumetric::FeatureGroup>& groups = volumeObject.GetFeatureGroups();
		if (ImGui::BeginCombo("Feature group", current_item)) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < groups.size(); n++)
			{
				bool is_selected = (current_item == groups[n].name.c_str()); // You can store your selection however you want, outside or inside your objects
				if (ImGui::Selectable(groups[n].name.c_str(), is_selected)) {
					current_item = groups[n].name.c_str();
					volumeObject.SetSelectedFeatureGroup(current_item);
				}
				if (is_selected) {
					ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
				}
			}
			ImGui::EndCombo();
		}

		if (volumeObject.GetSelectedFeatureGroup() != nullptr) {
			current_item = (volumeObject.GetSelectedFeature() != nullptr) ?
				volumeObject.GetSelectedFeature()->name.c_str() : "Select feature";
			std::vector<Volumetric::Feature*>& features = volumeObject.GetSelectedFeatureGroup()->features;

			if (ImGui::BeginCombo("Feature", current_item)) // The second parameter is the label previewed before opening the combo.
			{
				for (int n = 0; n < features.size(); n++)
				{
					bool is_selected = (current_item == features[n]->name.c_str()); // You can store your selection however you want, outside or inside your objects
					if (ImGui::Selectable(features[n]->name.c_str(), is_selected)) {
						current_item = features[n]->name.c_str();
						volumeObject.SetSelectedFeature(current_item);
					}
					if (is_selected) {
						ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
					}
				}
				ImGui::EndCombo();
			}
		}

		ImGui::BeginGroup();
		if (ImGui::Button("Reset to STF", ImVec2(120, 50))) {
			configSTF = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Reset to Default", ImVec2(120, 50))) {
			volumeObject.ResetToDefault();
		}
		ImGui::SameLine();
		if (ImGui::Button("Config feature", ImVec2(120, 50))) {
			configFeature = true;
		}
		if (ImGui::Button("Show all features", ImVec2(120, 50))) {
			volumeObject.ShowAll();
		}
		if (ImGui::Button("Load features", ImVec2(120, 50))) {
			volumeObject.LoadFeatures();
		}

		if (ImGui::Button("Create group", ImVec2(120, 50))) {
			volumeObject.CreateFeatureGroup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Show group", ImVec2(120, 50))) {
			volumeObject.ShowSelectedFeatureGroup();
		}
		ImGui::SameLine();
		if (ImGui::Button("Add feature", ImVec2(120, 50))) {
			addFeatureToGroup = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Remove feature", ImVec2(120, 50))) {
			volumeObject.RemoveSelectedFeatureFromFeatureGroup();
		}

		ImGui::EndGroup();
		ImGui::BeginGroup();
		ImGui::Text("Rotate");
		if (ImGui::Button("X", ImVec2(25, 25))) {
			volumeObject.RotateModelAroundX(M_PI / 2.0f);
		}
		ImGui::SameLine();
		if (ImGui::Button("Y", ImVec2(25, 25))) {
			volumeObject.RotateModelAroundY(M_PI / 2.0f);
		}
		ImGui::SameLine();
		if (ImGui::Button("Z", ImVec2(25, 25))) {
			volumeObject.RotateModelAroundZ(M_PI / 2.0f);
		}
		ImGui::EndGroup();
		ImGui::End();

		if (configFeature && volumeObject.GetSelectedFeature() != nullptr) {
			ImGui::Begin("Config feature");
			ImGui::ColorEdit3("Color", &volumeObject.GetSelectedFeature()->color.x);
			ImGui::SliderFloat("Opacity", &volumeObject.GetSelectedFeature()->opacity, 0.0f, 1.0f);
			ImGui::SliderFloat("Emission", &volumeObject.GetSelectedFeature()->emission, 0.0f, 2.0f);
			if (ImGui::Button("Finish", ImVec2(120, 50))) {
				volumeObject.RedrawSelected();
				configFeature = false;
			}

			ImGui::End();
		}

		if (configSTF) {
			ImGui::Begin("Configure STF");

			ImGui::SliderFloat("Class radius", &volumeObject.GetSTFradius(), 0.0f, 50.0f);
			ImGui::SliderFloat("Global opacity", &volumeObject.GetSTFOpacity(), 0.0f, 50.0f);
			ImGui::SliderFloat("Global emission", &volumeObject.GetSTFEmission(), 0.0f, 2.0f);

			if (ImGui::Button("Generate", ImVec2())) {
				volumeObject.ResetToSTF();
				configSTF = false;
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2())) {
				configSTF = false;
			}
		}

		if (addFeatureToGroup) {
			Volumetric::FeatureGroup* group = volumeObject.GetSelectedFeatureGroup();
			if (group != nullptr && group->name.compare("All features") != 0) {
				ImGui::Begin(std::string("Add feature to ").append(group->name).c_str());
				current_item = (featureToAdd != nullptr) ?
					featureToAdd->name.c_str() : "Select feature";
				std::vector<Volumetric::Feature>& features = volumeObject.GetFeatures();
				if (ImGui::BeginCombo("Feature to add", current_item)) // The second parameter is the label previewed before opening the combo.
				{
					for (int n = 0; n < features.size(); n++)
					{
						bool is_selected = (current_item == features[n].name.c_str()); // You can store your selection however you want, outside or inside your objects
						if (ImGui::Selectable(features[n].name.c_str(), is_selected)) {
							current_item = features[n].name.c_str();
							featureToAdd = &features[n];
						}
						if (is_selected) {
							ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
						}
					}
					ImGui::EndCombo();
				}
				if (ImGui::Button("Add", ImVec2())) {
					if (featureToAdd != nullptr) {
						volumeObject.AddFeatureToFeatureGroup(featureToAdd);
						featureToAdd = nullptr;
						addFeatureToGroup = false;
					}
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2())) {
					featureToAdd = nullptr;
					addFeatureToGroup = false;
				}
				ImGui::End();
			}
			else {
				featureToAdd = nullptr;
				addFeatureToGroup = false;
			}
		}
	}

	void GUI::Draw()
	{
		if (!visible)
			return;
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
}