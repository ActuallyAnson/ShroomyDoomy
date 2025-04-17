/******************************************************************************/
/*!
\file    ImguiPanelDocking.cpp
\project FruitLoops
\author  Gareth Ching
\date    Dec 03, 2024
\brief   This file implements the setup of the ImGui docking interface for the
         FruitLoops editor or debug UI.

         The `SetupDocking` function configures a full-screen docking layout
         using ImGui's Docking API, enabling flexible UI arrangement. It
         initialises a borderless, immovable window as a docking container and
         embeds a menu bar with basic controls (e.g., Exit).

         This system is essential for in-editor tooling, allowing various
         panels (e.g., Inspector, Hierarchy, Console) to be dynamically
         docked and managed by the user.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/

#include "pch.h"
#include "ImguiPanelDocking.h"

namespace FruitLoops {

/**
* @brief Configures and initializes the ImGui docking interface.
*
* This function sets up the docking layout for ImGui, creating a full-screen,
* borderless window that allows for docking and managing panels. It also adds
* a menu bar for basic functionalities like exiting the application.
*
* Key Features:
* - Creates a central dock space with customizable layout options.
* - Adds a menu bar with a "File" menu for application control.
*
* @note This function relies on ImGui's docking API and assumes that docking
*       is enabled in the ImGui configuration.
*/
	void SetupDocking() {
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        ImGui::Begin("DockSpace", nullptr, windowFlags);
        ImGui::PopStyleVar(2);

        ImGuiID dockspaceID = ImGui::GetID("FruitLoopsDockspace");
        ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

        if (ImGui::BeginMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("Exit")) {
                    // Set a flag to exit the game, or call gameActive = false if accessible
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImGui::End(); // End the DockSpace window
	}
}