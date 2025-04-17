/******************************************************************************/
/*!
\file    ImGuiPanels.h
\project FruitLoops
\author  Zhang TongYan
\date    Dec 03, 2024
\brief   This header declares the ImGuiPanels class for managing all in-editor
		 UI panels using ImGui, including rendering, entity inspection,
		 animation, audio, and asset management.

		 The ImGuiPanels class provides core developer tooling such as
		 real-time object transformation, custom inspector editing,
		 hierarchical reordering, and resource manipulation. It supports
		 both visual and behavioural debugging via a component-based ECS.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#pragma once
#include "FrameBuffer.h"
#include "LayerStack.h"
namespace FruitLoops {
	class ImGuiPanels : public Subject, public EngineSystems {

	public:
/*!
* \brief Constructor for the ImGuiPanels class. Initializes the ImGuiPanels system
* and assigns the global `imguiSystem` pointer to this instance.
*/
		ImGuiPanels();

/*!
* \brief Destructor for the ImGuiPanels class. Cleans up the system and resets
* the global `imguiSystem` pointer to nullptr.
*/
		~ImGuiPanels();

/*!
* \brief Initializes the ImGuiPanels system, setting up variables, loading assets,
* and preparing the UI for use.
*/
		void Initialize() override;

/*!
* \brief Updates the ImGuiPanels system. Handles rendering of all panels,
* user input, and interactions with game objects.
* \param deltatime The time elapsed since the last frame.
*/
		void Update(float deltatime) override;

		static void OnIsPaused();
		void UpdateAllObjects(const std::map<Entity, std::string>& newAllObjects); // Add this method

	private:

/*!
* \brief Resumes the game by disabling the `isPaused` flag and triggering
* the manual resume mechanism in the PauseManager.
*/
		static void OffIsPaused();

		static bool isPaused;


	private:
/*!
* \brief Initializes default variables and settings for the ImGuiPanels system.
* Sets up initial values for object properties, colors, and UI states.
*/
		void InitializeVars();

/*!
* \brief Displays a control panel with options to toggle the grid, pause the game,
* save changes, undo actions, and switch between levels.
*/
		void ShowCheckboxPanel();

/*!
* \brief Renders the framebuffer in an ImGui window. Provides controls for object
* manipulation, grid visibility, and pause functionality.
* \param frameBuffer The framebuffer object to display.
* \param layerStack The stack of layers to render.
* \param dt The time elapsed since the last frame update.
* \param gameActive A flag indicating whether the game is currently active.
*/
		void ShowFrameBuffer(const std::unique_ptr<FrameBuffer>& frameBuffer, LayerStack& layerStack, double dt,
			bool& gameActive);

/*!
* \brief Displays performance metrics such as FPS and frame time, along with
* controls for V-Sync and target frame rate.
*/
		void ShowPerformancePanel();

/*!
* \brief Displays a hierarchy of all game objects in the scene, allowing users
* to select, drag, and organize objects into layers.
*/
		void ShowObjectInspector();

/*!
* \brief Displays a detailed inspector for the selected game object. Allows
* modification of properties like position, scale, and attached components.
*/
		void ShowPropertyEditor();

/*!
* \brief Displays a content browser for managing assets such as textures,
* sounds, and scripts. Supports drag-and-drop functionality for asset assignment.
*/
		void ShowContentBrowser();

/*!
* \brief Handles sound-related operations for individual sounds or background music.
* Allows the user to play, pause, stop, and adjust volume levels.
* \param soundKey The key identifying the sound.
* \param isBGM A flag indicating whether the sound is background music.
*/
		void SoundHandler(const std::string& soundKey, bool isBGM);

/*!
* \brief Allows the addition or removal of components to/from the selected entity.
* Displays a list of available components and updates the entity accordingly.
* \param selectedEntity The entity to modify.
*/
		void ModifyComponents(Entity selectedEntity);

/*!
* \brief Displays a sound manager panel for controlling sound effects and background music.
* Includes drag-and-drop functionality for setting a new background music track.
*/
		void ShowSoundHandler();

		void ShowAnimationEditor();

		std::vector<MinMax2D> gameObjectBoundingBoxes;
		std::map<Entity, std::string>& allObjects;

		ImVec4 clear_color;
		ImVec2 dragOffset;
		ImU32 gridColor;
		ImVec2 iconSize;
		ImDrawList* drawList;

		int selectedObjectIndex;
		int previousObjectIndex;
		int cellSize;
		int columns;

		float objectScale[2];      
		float objectPosition[2];       
		float objectRotation[2];   
		float materialColor[3];
		float lineThickness;
		float padding;

		std::string objectName;                
		std::string objectTag;                     
                 
		bool previewSpritesheet;
		bool isDragging;
		bool showGrid;
		bool moveMode;
		bool scaleMode;
		bool rotateMode;
		
		std::filesystem::path currentDirectory;

		GLuint folderIconTexture;
		GLuint audioIconTexture;
		GLuint textIconTexture;
		GLuint textfileIconTexture;
	};
	
	extern ImGuiPanels* imguiSystem;
}