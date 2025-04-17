#pragma once
/******************************************************************************/
/*!
\file    ImguiPanelDocking.h
\project FruitLoops
\author  Gareth Ching
\date    Dec 03, 2024
\brief   This file declares the interface for setting up the ImGui docking
         layout within the FruitLoops editor or debug environment.

         It exposes the `SetupDocking` function, which configures a
         full-screen docking container window using ImGui's Docking API.
         The layout enables flexible panel arrangement and embeds a
         top-level menu bar for core editor controls.

         This header is intended to be included wherever docking layout
         configuration is required, typically during the main UI loop.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
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
	void SetupDocking();
	
}