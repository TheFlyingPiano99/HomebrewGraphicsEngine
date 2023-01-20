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
		auto buttonSize = ImVec2(300, 25);
		auto squareButtonSize = ImVec2(25, 25);
		static bool configFeature = false;
		static bool configSTF = false;
		static bool addFeatureToGroup = false;
		static bool removeFeatureFromGroup = false;
		static Volumetric::Feature* featureToAdd = nullptr;
		static Volumetric::Feature* featureToRemove = nullptr;
		const char* current_item;
		const char** items;
		char value_buf[64] = {};
		{	// Settings window
			ImGui::Begin("Volume Object Settings");
			{
				ImGui::BeginGroup();
				ImGui::Text("General");
				ImGui::SliderFloat((const char*)"Light power", &volumeObject.GetLightPower(), 1.0f, 1000.0f);
				ImGui::ColorEdit3("Light color", &(volumeObject.GetLightColor().x));
				ImGui::SliderFloat("Density", &volumeObject.GetDensity(), 1.0, 100.0);
				ImGui::EndGroup();
			}

			current_item = volumeObject.GetCurrentTransferRegionSelectMode();
			items = volumeObject.GetTransferRegionSelectModes();

			{
				ImGui::BeginGroup();
				ImGui::Text("Transfer function");

				if (ImGui::Button("Reset to Default", buttonSize)) {
					volumeObject.ResetToDefault();
				}
				if (ImGui::Button("Generate STF...", buttonSize)) {
					configSTF = true;
				}
				if (ImGui::Button("Load STF", buttonSize)) {
					volumeObject.LoadFeatures();
				}
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
				ImGui::ColorEdit3("Draw color", &(volumeObject.GetTransferDrawColor().x));
				if (ImGui::Button("Fixate", buttonSize)) {
					volumeObject.FixateTransferFunction();
				}
				ImGui::EndGroup();
			}

			//---------------------------------------------------------------------------
			// Group:

			{
				ImGui::BeginGroup();
				ImGui::Text("Feature group");

				current_item = (volumeObject.GetSelectedFeatureGroup() != nullptr) ?
					volumeObject.GetSelectedFeatureGroup()->name.c_str() : "Select group";
				std::vector<Volumetric::FeatureGroup*>& instanceGroups = volumeObject.GetFeatureGroups();
				if (ImGui::BeginCombo("Group", current_item)) // The second parameter is the label previewed before opening the combo.
				{
					for (int n = 0; n < instanceGroups.size(); n++)
					{
						bool is_selected = (current_item == instanceGroups[n]->name.c_str()); // You can store your selection however you want, outside or inside your objects
						if (ImGui::Selectable(instanceGroups[n]->name.c_str(), is_selected)) {
							current_item = instanceGroups[n]->name.c_str();
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

				if (ImGui::Button("New group", buttonSize)) {
					volumeObject.CreateFeatureGroup();
				}
				if (ImGui::Button("Show selected group", buttonSize)) {
					volumeObject.ShowSelectedFeatureGroup();
				}
				if (ImGui::Button("Delete selected group", buttonSize)) {
					volumeObject.DeleteSelectedGroup();
				}
				if (ImGui::Button("Add feature to group...", buttonSize)) {
					addFeatureToGroup = true;
				}
				if (ImGui::Button("Remove feature from group...", buttonSize)) {
					removeFeatureFromGroup = true;
				}
				if (ImGui::Button("Config selected feature...", buttonSize)) {
					configFeature = true;
				}
				if (ImGui::Button("Show all features", buttonSize)) {
					volumeObject.ShowAll();
				}
				ImGui::EndGroup();
			}

			// Transformation:
			{
				ImGui::BeginGroup();
				ImGui::Text("Rotate");
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				if (ImGui::Button("+X", squareButtonSize)) {
					volumeObject.RotateModelAroundX(M_PI / 2.0f);
				}
				ImGui::PopStyleColor(1);
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				if (ImGui::Button("+Y", squareButtonSize)) {
					volumeObject.RotateModelAroundY(M_PI / 2.0f);
				}
				ImGui::PopStyleColor(1);
				ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
				if (ImGui::Button("+Z", squareButtonSize)) {
					volumeObject.RotateModelAroundZ(M_PI / 2.0f);
				}
				ImGui::PopStyleColor(1);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
				if (ImGui::Button("-X", squareButtonSize)) {
					volumeObject.RotateModelAroundX(-M_PI / 2.0f);
				}
				ImGui::SameLine();
				ImGui::PopStyleColor(1);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
				if (ImGui::Button("-Y", squareButtonSize)) {
					volumeObject.RotateModelAroundY(-M_PI / 2.0f);
				}
				ImGui::SameLine();
				ImGui::PopStyleColor(1);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
				if (ImGui::Button("-Z", squareButtonSize)) {
					volumeObject.RotateModelAroundZ(-M_PI / 2.0f);
				}
				ImGui::PopStyleColor(1);
				ImGui::EndGroup();
			}
			if (ImGui::Button("Redraw", buttonSize)) {
				volumeObject.ForceRedraw();
			}
			if (volumeObject.GetShowNormals()) {
				if (ImGui::Button("Shading", buttonSize)) {
					volumeObject.SetShowNormals(false);
				}
			}
			else if (ImGui::Button("Normals", buttonSize)) {
				volumeObject.SetShowNormals(true);
			}
			if (ImGui::Button("Phong / PBR", buttonSize)) {
				volumeObject.ToggleUsePBR();
			}
			ImGui::SliderFloat((const char*)"Gradient based local illum", &volumeObject.GetGradientBasedIllumination(), 0.0f, 5.0f);
			ImGui::SliderFloat((const char*)"Local shadows", &volumeObject.GetLocalShadows(), 0.0f, 1.0f);
			ImGui::End();
		}
		{
			if (configFeature && volumeObject.GetSelectedFeature() != nullptr) {
				ImGui::Begin("Config feature");
				ImGui::ColorEdit3("Color", &volumeObject.GetSelectedFeature()->color.x);
				ImGui::SliderFloat("Opacity", &volumeObject.GetSelectedFeature()->opacity, 0.0f, 1.0f);
				ImGui::SliderFloat("Emission", &volumeObject.GetSelectedFeature()->emission, 0.0f, 2.0f);
				if (ImGui::Button("Finish", buttonSize)) {
					volumeObject.RedrawSelected();
					configFeature = false;
				}
				if (ImGui::Button("Cancel", buttonSize)) {
					configFeature = false;
				}

				ImGui::End();
			}
			else {
				configFeature = false;
			}
		}

		{
			if (configSTF) {
				ImGui::Begin("Configure STF");

				ImGui::SliderFloat("Class radius", &volumeObject.GetSTFradius(), 0.0f, 1.0f);
				ImGui::SliderInt("Class min. contributors", &volumeObject.GetSTFMinContributions(), 1, 10000);
				ImGui::SliderFloat("Global opacity", &volumeObject.GetSTFOpacity(), 0.0f, 1.0f);
				ImGui::SliderFloat("Global emission", &volumeObject.GetSTFEmission(), 0.0f, 2.0f);

				if (ImGui::Button("Generate", ImVec2())) {
					volumeObject.GenerateSTF();
					configSTF = false;
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2())) {
					configSTF = false;
				}
				ImGui::End();
			}
		}

		{
			if (addFeatureToGroup) {
				Volumetric::FeatureGroup* group = volumeObject.GetSelectedFeatureGroup();
				std::vector<Volumetric::Feature>& features = volumeObject.GetFeatures();
				if (group != nullptr && features.size() > 0 && group->name.compare("All features") != 0) {
					ImGui::Begin(std::string("Add feature to ").append(group->name).c_str());
					current_item = (featureToAdd != nullptr) ?
						featureToAdd->name.c_str() : "Select feature";
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
		{
			if (removeFeatureFromGroup) {
				Volumetric::FeatureGroup* group = volumeObject.GetSelectedFeatureGroup();
				if (group != nullptr && group->features.size() > 0 && group->name.compare("All features") != 0) {
					ImGui::Begin(std::string("Remove feature from ").append(group->name).c_str());
					current_item = (featureToRemove != nullptr) ?
						featureToRemove->name.c_str() : "Select feature";
					auto& features = group->features;
					if (ImGui::BeginCombo("Feature to remove", current_item)) // The second parameter is the label previewed before opening the combo.
					{
						for (int n = 0; n < features.size(); n++)
						{
							bool is_selected = (current_item == features[n]->name.c_str()); // You can store your selection however you want, outside or inside your objects
							if (ImGui::Selectable(features[n]->name.c_str(), is_selected)) {
								current_item = features[n]->name.c_str();
								featureToRemove = features[n];
							}
							if (is_selected) {
								ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
							}
						}
						ImGui::EndCombo();
					}
					if (ImGui::Button("Remove", ImVec2())) {
						if (featureToRemove != nullptr) {
							volumeObject.RemoveFeatureFromGroup(featureToRemove);
							featureToRemove = nullptr;
							removeFeatureFromGroup = false;
						}
					}
					ImGui::SameLine();
					if (ImGui::Button("Cancel", ImVec2())) {
						featureToRemove = nullptr;
						removeFeatureFromGroup = false;
					}
					ImGui::End();
				}
				else {
					featureToRemove = nullptr;
					removeFeatureFromGroup = false;
				}
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