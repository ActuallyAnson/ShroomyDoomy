/******************************************************************************/
/*!
\file    ImGuiPanels.h
\project FruitLoops
\author  Zhang TongYan
\date    Dec 03, 2024
\brief   This file declares the ImGuiPanels class responsible for managing the
         runtime UI system using ImGui.

         ImGuiPanels encapsulates a wide array of panels including the
         framebuffer view, object inspector, content browser, animation editor,
         sound manager, and component modifiers. It also handles UI-based
         interactions such as drag-and-drop, real-time entity manipulation,
         and custom inspector editing for ECS-based components.

         This system is core to the in-editor user experience and supports
         live debugging, asset management, game control toggles, and developer
         workflow tooling.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#include "ImGuiPanels.h"
#include "Graphics.h"
#include "FrameBuffer.h"
#include "Serialization.h"
#include "ImguiPanelDocking.h"
#include "ScriptSystem.h"
#include "Core.h"
#include "ECSDefinitions.h"
#include "Factory.h"
#include "Graphics.h"
#include "AssetsManager.h"
#include "LogicProxySystem.h"
#include "PrefabManager.h"
#include "LogicProxySystem.h"

namespace FruitLoops {

    ImGuiPanels* imguiSystem = nullptr;

    bool ImGuiPanels::isPaused = false;

/*!
* \brief Constructor for the ImGuiPanels class. Initializes the ImGui system
* and assigns the global `imguiSystem` pointer to this instance.
*/
    ImGuiPanels::ImGuiPanels() : allObjects(factory->GetAllGameObjectsMap())
    {
        imguiSystem = this;
    }

/*!
* \brief Destructor for the ImGuiPanels class. Cleans up and resets the global
* `imguiSystem` pointer.
*/
    ImGuiPanels::~ImGuiPanels()
    {
        imguiSystem = nullptr;
    }

/*!
* \brief Initializes the ImGuiPanels system by setting up default variables,
* loading assets like icons, and configuring UI states.
*/
    void ImGuiPanels::Initialize()
    {
        InitializeVars();
    }

/*!
* \brief Updates the ImGui panels, handling frame rendering, input processing,
* and drawing all active panels such as the framebuffer view, hierarchy, and inspector.
* \param deltatime The time elapsed since the last frame update.
*/
    void ImGuiPanels::Update(float deltatime)
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        SetupDocking();

        ImGuiPanels::ShowCheckboxPanel();

        ImGuiPanels::ShowFrameBuffer(coreEngine->GetFrameBuffer(), coreEngine->GetLayerStack(), deltatime, coreEngine->GetGameState());

        // --- GET DISPLAY SIZE ---
        //ImVec2 displaySize = ImGui::GetIO().DisplaySize;

        ImGuiPanels::ShowPerformancePanel();

        ImGuiPanels::ShowObjectInspector();

        ImGuiPanels::ShowPropertyEditor();

        ImGuiPanels::ShowContentBrowser();

        ImGuiPanels::ShowSoundHandler();

        ImGuiPanels::ShowAnimationEditor();

        ImGui::End();
    }

/*!
* \brief Initializes default variables and loads resources like textures
* for the ImGui system, including icons and grid settings.
*/
    void ImGuiPanels::InitializeVars()
    {
        ImGuiIO& io = ImGui::GetIO();
        io.FontGlobalScale = 1.5f;

        clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        selectedObjectIndex = 0;
        previousObjectIndex = 0;
        

        objectScale[0] = 100.0f;
        objectScale[1] = 100.0f;

        objectPosition[0] = 0.0f;
        objectPosition[1] = 0.0f;

        objectRotation[0] = 0.0f;
        objectRotation[1] = 0.0f;

        objectName = "Unnamed";
        objectTag = "None";
        objectName.resize(64);
        objectTag.resize(64);

        materialColor[0] = { 1.0f };
        materialColor[1] = { 1.0f };
        materialColor[2] = { 1.0f };

        dragOffset = ImVec2(0, 0);
        iconSize = ImVec2(300, 300);
        gridColor = IM_COL32(200, 200, 200, 255);

        cellSize = 50;
        lineThickness = 1.0f;
        padding = 20.f;
        columns = 4;

        currentDirectory = "../Assets";
        bool success;

        // load folder icon
        success = assetsManager->LoadTexture("../Assets/Textures/folder icon.png", "FolderIcon");
        if (!success) {
#ifdef _DEBUG
            spdlog::error("Failed to load folder icon texture.");
#endif
        }
        else {
            folderIconTexture = assetsManager->GetTextures("FolderIcon"); // Retrieve the texture ID
            if (folderIconTexture == 0) {
#ifdef _DEBUG
                spdlog::error("Failed to retrieve folder icon texture ID.");
#endif
            }
            else {
                //spdlog::info("Folder icon texture loaded successfully with ID: {}", folderIconTexture);
            }
        }

        // Load audio icon
        success = assetsManager->LoadTexture("../Assets/Textures/audio.png", "AudioIcon");
        if (!success) {
#ifdef _DEBUG
            spdlog::error("Failed to load audio icon texture.");
#endif
        }
        else {
            audioIconTexture = assetsManager->GetTextures("AudioIcon");
            if (audioIconTexture == 0) {
#ifdef _DEBUG
                spdlog::error("Failed to retrieve audio icon texture ID.");
#endif
            }
        }

        // Load text icon
        success = assetsManager->LoadTexture("../Assets/Textures/text.png", "TextIcon");
        if (!success) {
#ifdef _DEBUG
            spdlog::error("Failed to load text icon texture.");
#endif
        }
        else {
            textIconTexture = assetsManager->GetTextures("TextIcon");
            if (textIconTexture == 0) {
#ifdef _DEBUG
                spdlog::error("Failed to retrieve text icon texture ID.");
#endif
            }
        }

        // Load textfile icon
        success = assetsManager->LoadTexture("../Assets/Textures/textfile.png", "TextFileIcon");
        if (!success) {
#ifdef _DEBUG
            spdlog::error("Failed to load textfile icon texture.");
#endif
        }
        else {
            textfileIconTexture = assetsManager->GetTextures("TextFileIcon");
            if (textfileIconTexture == 0) {
#ifdef _DEBUG
                spdlog::error("Failed to retrieve textfile icon texture ID.");
#endif
            }
        }

        isDragging = false;
        showGrid = false;
        isPaused = false;
        moveMode = true;
        scaleMode = false;
        rotateMode = false;
    }

/*!
* \brief Displays a control panel with options to toggle the grid,
* pause the game, save changes, undo actions, and switch levels.
*/
    void ImGuiPanels::ShowCheckboxPanel()
    {
        ImGui::Begin("Control Panel");

        // Grid state checkbox
        ImGui::Checkbox("Show Grid", &showGrid);

        // Pause state checkbox
        ImGui::Checkbox("Pause Engine", &isPaused);
        if (isPaused) {
            PauseManager::OnManualPause();
        }
        else {
            PauseManager::OffManualPause();
        }
        ImGui::Separator();
        // save changes button
        if (ImGui::Button("Save Changes")) {

            LogicProxySystem::End();
            std::vector<bool> cache;
            for (Layer* layer : coreEngine->GetLayerStack()) {
                
                cache.push_back(layer->IsVisible());
                if (layer->IsVisible()) {
                    layer->UpdateOriginalScale();
                }
                *layer->SetVisibility() = true;
                 layer->ShowObjects();
            }
            SerializeAndSaveAllGameObjects();
            SCRIPTSYSTEM->InitializeBehaviorComponents();
            LogicProxySystem::Initialize();
            if (!factory->SaveConfig("../GameData/config.json")) {
                std::cerr << "Failed to save config data." << std::endl;
            }
            else {
                std::cerr << "Successfully saved game config." << std::endl;
            }
            if (!factory->SaveAnimation("../GameData/AnimationContainer.json", GRAPHICS->animatorContainer)) {
                std::cerr << "Failed to save animation data." << std::endl;
            }
            else {
                std::cerr << "Successfully saved animator objects." << std::endl;
            }

            size_t i = 0;
            for (Layer* layer : coreEngine->GetLayerStack()) {
                *layer->SetVisibility() = cache[i];
                if (cache[i]) {
                    layer->ShowObjects();
                }
                else {
                    layer->HideObjects();
                }
                i++;
            }
        }

        if (ImGui::Button("Run Game")) {
            // C# game logic start
            SCRIPTSYSTEM->InitializeBehaviorComponents();
            LogicProxySystem::Initialize();
        }

        if (ImGui::Button("Pause Game")) {
            // C# game logic end
            LogicProxySystem::End();
        }

        if (ImGui::Button("Swap to Level 2")) {
            
            levelManager->Level1ToLevel2();
        }

        if (ImGui::Button("Swap to Tutorial Level")) {

            levelManager->Level1ToTutorial();
        }

        if (ImGui::Button("Swap from Level 2 to Level 1")) {
            levelManager->Level2ToLevel1();
        }

        if (ImGui::Button("Swap from Tutorial to Level 1")) {
            levelManager->TutorialToLevel1();
        }

        ImGui::Separator();

        ImGui::End();
    }

/*!
* \brief Renders the game's framebuffer in an ImGui window, allowing
* real-time interaction like dragging, scaling, and rotating objects.
* \param frameBuffer The framebuffer object to display.
* \param layerStack The stack of layers to render.
* \param dt The time delta since the last update.
* \param gameActive A reference to a flag indicating if the game is running.
*/
    void ImGuiPanels::ShowFrameBuffer(const std::unique_ptr<FrameBuffer>& frameBuffer, LayerStack& layerStack, double dt, 
        bool& gameActive)
    {
        drawList = ImGui::GetForegroundDrawList();

        ImGui::Begin("Framebuffer View");

        // Get the position and size of the content region (excluding window decorations)
        ImVec2 framebufferWindowPos = ImGui::GetWindowPos();
        ImVec2 framebufferContentSize = ImGui::GetContentRegionAvail();

        // Store these values in variables accessible to your input system
        InputSystem::framebufferWindowPos = framebufferWindowPos;
        InputSystem::framebufferWindowSize = framebufferContentSize;

        // TRACK THE DIFFERENT STATE
        static bool isDraggingObject = false;
        static int draggedEntity = -1;
        static Vector2D dragOffSet;

        static bool isResizingObject = false;
        static int resizingEntity = -1;
        
        static bool isRotatingObject = false;
        static int rotatingEntity = -1;
        static float initialMouseAngle = 0.0f;
        static Vector2D centerOfRotation;

        // Ensure the framebuffer size is valid
        int newFramebufferWidth = static_cast<int>(framebufferContentSize.x);
        int newFramebufferHeight = static_cast<int>(framebufferContentSize.y);

        // Avoid resizing to zero dimensions
        if (newFramebufferWidth <= 0 || newFramebufferHeight <= 0) {
            newFramebufferWidth = static_cast<int>(frameBuffer->GetWidth());
            newFramebufferHeight = static_cast<int>(frameBuffer->GetHeight());
        }

        // Check if the framebuffer needs to be resized
        if (newFramebufferWidth != frameBuffer->GetWidth() || newFramebufferHeight != frameBuffer->GetHeight()) {
            frameBuffer->RescaleFrameBuffer(static_cast<float>(newFramebufferWidth), static_cast<float>(newFramebufferHeight));

            // Update the InputSystem framebuffer dimensions
            InputSystem::framebufferWindowSize.x = static_cast<float>(newFramebufferWidth);
            InputSystem::framebufferWindowSize.y = static_cast<float>(newFramebufferHeight);

            // Update the camera's projection if necessary
            GRAPHICS->camera2d.update(newFramebufferWidth, newFramebufferHeight);
        }

        // Bind the framebuffer
        frameBuffer->Bind();

        // Set the OpenGL viewport to match the framebuffer's dimensions
        glViewport(0, 0, static_cast<GLsizei>(frameBuffer->GetWidth()), static_cast<GLsizei>(frameBuffer->GetHeight()));
        glClearColor(0.1725f, 0.1725f, 0.1725f, 1.0f);
        // Clear the framebuffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render your layers
        layerStack.handleEvents();   // Handle events from top to bottom
        layerStack.updateLayers(static_cast<float>(dt));    // Update layers
        layerStack.renderLayers();     // Render layers from bottom to top

        // Unbind the framebuffer
        frameBuffer->Unbind();

        // Display the framebuffer texture in the ImGui window
        ImGui::Image(
            (void*)(intptr_t)frameBuffer->getFrameTexture(),
            framebufferContentSize,
            ImVec2(0, 1),
            ImVec2(1, 0)
        );

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                std::string assetPath = (const char*)payload->Data;
                size_t pos = assetPath.find(".");
                assetPath = assetPath.substr(0, pos);
                // Load and instantiate the asset in the scene
#ifdef _DEBUG
                spdlog::info("Dropped asset: {}", assetPath);
#endif
                std::string newLabel = "Tutorial_Game_" + std::string(assetPath);
                // Use the factory to duplicate the selected object
                Entity newEntity = factory->CreateGameObject(inputSystem->GetCursorPosition(), { objectScale[0], objectScale[1] },
                    { objectRotation[0], objectRotation[1] }, assetPath, newLabel);

                // Optionally, update the dropdown to include the new object in the list
                
                spdlog::info("Added asset: {}", newLabel);
                allObjects[newEntity] = newLabel;

            }
            ImGui::EndDragDropTarget();
            //SnapshotManager::SaveState();
        }
   
        if (moveMode || scaleMode || rotateMode) {
            if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(0)) {
                Vector2D cursorWorldPos = InputSystem::GetCursorPosition();

                if (!isDraggingObject && !isResizingObject && !isRotatingObject) {
                    for (auto it = allObjects.begin(); it != allObjects.end(); it++) {
                        Entity entity = it->first;
                        Transform& transform = ecsHub->GetComponent<Transform>(entity);

                        Vector2D minBounds = transform.position - transform.scale * 0.5f;
                        Vector2D maxBounds = transform.position + transform.scale * 0.5f;

                        if (cursorWorldPos.x >= minBounds.x && cursorWorldPos.x <= maxBounds.x &&
                            cursorWorldPos.y >= minBounds.y && cursorWorldPos.y <= maxBounds.y) {
                            selectedObjectIndex = entity;
                            
                            // Update properties panel immediately upon selection
                            objectPosition[0] = transform.position.x;
                            objectPosition[1] = transform.position.y;
                            
                            

                            objectScale[0] = transform.scale.x;
                            objectScale[1] = transform.scale.y;
#ifdef _DEBUG
                            spdlog::info("saving state start");
#endif
#ifdef _DEBUG
                            spdlog::info("saving satte working");
#endif

                            if (moveMode) {
                                isDraggingObject = true;
                                draggedEntity = entity;
                                dragOffSet = cursorWorldPos - transform.position;
                            }
                            else if (scaleMode) {
                                isResizingObject = true;
                                resizingEntity = entity;
                            }
                            else if (rotateMode) {
                                isRotatingObject = true;
                                rotatingEntity = entity;
                                initialMouseAngle = atan2(cursorWorldPos.y - transform.position.y, cursorWorldPos.x - transform.position.x);
                                centerOfRotation = transform.position;
                            }
                            break;
                        }
                    }
                }
                else {
                    if (isDraggingObject) {
                        Transform& transform = ecsHub->GetComponent<Transform>(draggedEntity);
                        transform.position = cursorWorldPos - dragOffSet;

                        // Update object properties for the UI
                        objectPosition[0] = transform.position.x;
                        objectPosition[1] = transform.position.y;
                    }
                    if (isResizingObject) {
                        Transform& transform = ecsHub->GetComponent<Transform>(resizingEntity);
                        Vector2D resizeAmount = cursorWorldPos - transform.position;
                        transform.scale.x = std::max(std::fabs(resizeAmount.x), 50.f);
                        transform.scale.y = std::max(std::fabs(resizeAmount.y), 50.f);
                        
                        // Update object properties for the UI
                        objectScale[0] = transform.scale.x;
                        objectScale[1] = transform.scale.y;
                    }
                    if (isRotatingObject) {
                        Transform& transform = ecsHub->GetComponent<Transform>(rotatingEntity);
                        float currentMouseAngle = atan2(cursorWorldPos.y - centerOfRotation.y, cursorWorldPos.x - centerOfRotation.x);
                        float angleDifference = currentMouseAngle - initialMouseAngle;
                        float sensitivity = 50.f;
                        transform.orientation += angleDifference * sensitivity;  // This could be more complex depending on how you define orientation

                        initialMouseAngle = currentMouseAngle;  // Update the angle for continuous rotation
                    }
                }
            }
            else {
                if (isDraggingObject || isResizingObject || isRotatingObject) {
                    isDraggingObject = false;
                    isResizingObject = false;
                    isRotatingObject = false;
                    draggedEntity = -1;
                    resizingEntity = -1;
                    rotatingEntity = -1;
                }
            }
        }

        // Draw grid overlay
        if (showGrid) {
            NotifyObservers(new DrawGridMessage(true));
        }
        else {
            NotifyObservers(new DrawGridMessage(false));
        }

        
        // Draw Pause Screen Overlay
        if (isPaused) {
            drawList = ImGui::GetWindowDrawList();

            // Draw semi-transparent background over framebuffer
            drawList->AddRectFilled(
                framebufferWindowPos,
                ImVec2(framebufferWindowPos.x + InputSystem::framebufferWindowSize.x + 100.f, framebufferWindowPos.y + InputSystem::framebufferWindowSize.y + 100.f),
                IM_COL32(0, 0, 0, 150)  // Semi-transparent black
            );

            // Center the pause menu content within the viewport
            ImVec2 pauseTextPos = ImVec2(
                framebufferWindowPos.x + InputSystem::framebufferWindowSize.x / 2.0f,
                framebufferWindowPos.y + InputSystem::framebufferWindowSize.y / 2.0f - 50
            );

            // Center the text and buttons horizontally within the viewport
            drawList->AddText(ImVec2(pauseTextPos.x - ImGui::CalcTextSize("Game Paused").x / 2, pauseTextPos.y), IM_COL32(255, 255, 255, 255), "Game Paused");

            ImVec2 buttonSize = ImVec2(200, 50);
            ImVec2 buttonPos = ImVec2(pauseTextPos.x - buttonSize.x / 2, pauseTextPos.y + 30);

            ImGui::SetCursorScreenPos(buttonPos);
            if (ImGui::Button("Resume", buttonSize)) {
                isPaused = false;  // Unpause the game
                PauseManager::OffManualPause(); // Trigger manual resume
            }

            buttonPos.y += buttonSize.y + 10;
            ImGui::SetCursorScreenPos(buttonPos);
            if (ImGui::Button("Settings", buttonSize)) {
                // Open settings menu (implement separately if needed)
            }

            buttonPos.y += buttonSize.y + 10;
            ImGui::SetCursorScreenPos(buttonPos);
            if (ImGui::Button("Quit", buttonSize)) {
                // Implement quit functionality, e.g., set a quit flag or close the window
                gameActive = false;
            }
        }

        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_None)) { // Check if mouse is over the framebuffer window
            InputSystem::windowHovered = true;
        }
        else {
            InputSystem::windowHovered = false;
        }

        ImGui::End();
    }

/*!
* \brief Displays the performance panel, including FPS monitoring, a V-Sync toggle,
* and a slider to adjust the target frame rate.
*/
    void ImGuiPanels::ShowPerformancePanel() {
        static float frameTimes[100] = { 0 };  // Array to store frame times
        static int frameIndex = 0;
        static bool vSyncEnabled = true;
        static int targetFPS = 60;

        ImGui::Begin("Performance & Settings");

        // FPS monitoring
        frameTimes[frameIndex % 100] = ImGui::GetIO().DeltaTime * 1000.0f; // Record frame time in ms
        frameIndex++;
        ImGui::Text("ImGui FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::PlotLines("Frame Times (ms)", frameTimes, 100);

        // V-Sync toggle
        if (ImGui::Checkbox("Enable V-Sync", &vSyncEnabled)) {
            windowSystem->SetVSync(vSyncEnabled);  // Enable or disable V-Sync
        }

        // FPS limiter slider
        ImGui::SliderInt("Target FPS", &targetFPS, 30, 144);

        ImGui::End();
    }

/*!
* \brief Pauses the game by setting the `isPaused` flag to true and calling
* the manual pause method in the PauseManager.
*/
    void ImGuiPanels::OnIsPaused() {
        isPaused = true;
    }

/*!
* \brief Resumes the game by setting the `isPaused` flag to false and calling
* the manual resume method in the PauseManager.
*/
    void ImGuiPanels::OffIsPaused() {
        isPaused = false;
    }

/*!
* \brief Displays a hierarchy of game objects, allowing the user to select,
* drag, and drop entities between layers.
*/
    void ImGuiPanels::ShowObjectInspector() {
        ImGui::Begin("Hierarchy");
        std::unordered_map<std::string, Layer*> layerCopy;

        // Populate a copy of the layers map from the LayerStack.
        for (Layer* layer : coreEngine->GetLayerStack()) {
            std::string layerName = layer->GetName(); // Assume Layer has GetName()
            layerCopy[layerName] = layer;
        }

        // Static variables to track dragging.
        static Entity draggedEntity = 0;
        static int draggedEntityIndex = -1;
        static std::string draggedFromLayer;

        for (auto& [layerName, layerPtr] : layerCopy) {

            ImGui::PushID(layerName.c_str());

            // Render the layer header using a tree node.
            // Using DefaultOpen so that the list is visible (this doesn’t affect drop target behavior).
            bool treeOpen = ImGui::TreeNodeEx(layerName.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

            // --- Create a custom drop target on the layer header (i.e. the title).
            // We grab the bounding rectangle of the just-drawn tree node header.
            ImRect headerRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
            // Use a custom drop target on the header. This does not draw any additional UI,
            // but it makes the header receptive to drag-and-drop payloads.
            if (ImGui::BeginDragDropTargetCustom(headerRect, ImGui::GetID("LayerHeaderDrop"))) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("OBJECT_ENTITY_REORDER")) {
                    // A payload was dropped on the header.
                    // Get the target layer's object container.
                    auto* targetObjects = layerPtr->GetObjectContainer();

                    // If dropping within the same layer:
                    if (layerName == draggedFromLayer) {
                        // Only reposition if the object is not already at the top.
                        if (draggedEntityIndex != 0) {
                            // Remove the dragged entity from its current position and insert at index 0.
                            Entity temp = draggedEntity;
                            targetObjects->erase(targetObjects->begin() + draggedEntityIndex);
                            targetObjects->insert(targetObjects->begin(), temp);
                        }
                    }
                    else { // Cross-layer drop.
                        // Remove the entity from its source layer.
                        auto srcIt = layerCopy.find(draggedFromLayer);
                        if (srcIt != layerCopy.end()) {
                            auto* srcObjects = srcIt->second->GetObjectContainer();
                            if (srcObjects) {
                                auto it = std::find(srcObjects->begin(), srcObjects->end(), draggedEntity);
                                if (it != srcObjects->end()) {
                                    srcObjects->erase(it);
                                }
                            }
                        }
                        // Insert the dragged entity at the top of the target layer.
                        targetObjects->insert(targetObjects->begin(), draggedEntity);

                        // Singleton Layers
                        if (layerName == "UtilsLayer") {
                            std::string newName = factory->GetEntityType(draggedEntity);
                            size_t i = newName.find_last_of("_");
                            newName = newName.substr(i);
                            newName = "Utils" + newName;
                            factory->SetEntityType(draggedEntity, newName);
                        }
                        else if (layerName == "FPSlayer") {
                            std::string newName = factory->GetEntityType(draggedEntity);
                            size_t i = newName.find_last_of("_");
                            newName = newName.substr(i);
                            newName = "FPS" + newName;
                            factory->SetEntityType(draggedEntity, newName);
                        }
                        else if (layerName == "StoryLayer") {
                            std::string newName = factory->GetEntityType(draggedEntity);
                            size_t i = newName.find_last_of("_");
                            newName = newName.substr(i);
                            newName = "Story" + newName;
                            factory->SetEntityType(draggedEntity, newName);
                        }
                        else if (layerName == "PauseMenuLayer") {
                            std::string newName = factory->GetEntityType(draggedEntity);
                            size_t i = newName.find_last_of("_");
                            newName = newName.substr(i);
                            newName = "PauseMenu" + newName;
                            factory->SetEntityType(draggedEntity, newName);
                        }
                        else if (layerName == "MainMenuLayer") {
                            std::string newName = factory->GetEntityType(draggedEntity);
                            size_t i = newName.find_last_of("_");
                            newName = newName.substr(i);
                            newName = "MainMenu" + newName;
                            factory->SetEntityType(draggedEntity, newName);
                        }
                        // Level 1 Layers
                        else if (layerName == "GameLayer") {
                            std::string newName = factory->GetEntityType(draggedEntity);
                            size_t i = newName.find_last_of("_");
                            newName = newName.substr(i);
                            newName = "Level1_Game" + newName;
                            factory->SetEntityType(draggedEntity, newName);
                        }
                        else if (layerName == "BackgroundLayer") {
                            std::string newName = factory->GetEntityType(draggedEntity);
                            size_t i = newName.find_last_of("_");
                            newName = newName.substr(i);
                            newName = "Level1" + newName;
                            factory->SetEntityType(draggedEntity, newName);
                        }
                        else if (layerName == "EnvironmentLayer") {
                            std::string newName = factory->GetEntityType(draggedEntity);
                            size_t i = newName.find_last_of("_");
                            newName = newName.substr(i);
                            newName = "Level1_Environment" + newName;
                            factory->SetEntityType(draggedEntity, newName);
                        }
                        else if (layerName == "FontLayer") {
                            std::string newName = factory->GetEntityType(draggedEntity);
                            size_t i = newName.find_last_of("_");
                            newName = newName.substr(i);
                            newName = "Level1_Font" + newName;
                            factory->SetEntityType(draggedEntity, newName);
                        }
                        // Level 2 Layers
                        else if (layerName == "Level2GameLayer") {
                            std::string newName = factory->GetEntityType(draggedEntity);
                            size_t i = newName.find_last_of("_");
                            newName = newName.substr(i);
                            newName = "Level2_Game" + newName;
                            factory->SetEntityType(draggedEntity, newName);
                        }
                        else if (layerName == "Level2BackgroundLayer") {
                            std::string newName = factory->GetEntityType(draggedEntity);
                            size_t i = newName.find_last_of("_");
                            newName = newName.substr(i);
                            newName = "Level2" + newName;
                            factory->SetEntityType(draggedEntity, newName);
                        }
                        else if (layerName == "Level2EnvironmentLayer") {
                            std::string newName = factory->GetEntityType(draggedEntity);
                            size_t i = newName.find_last_of("_");
                            newName = newName.substr(i);
                            newName = "Level2_Environment" + newName;
                            factory->SetEntityType(draggedEntity, newName);
                        }
                        else if (layerName == "Level2FontLayer") {
                            std::string newName = factory->GetEntityType(draggedEntity);
                            size_t i = newName.find_last_of("_");
                            newName = newName.substr(i);
                            newName = "Level2_Font" + newName;
                            factory->SetEntityType(draggedEntity, newName);
                        }
                        // Tutorial Layers
                        else if (layerName == "TutorialGameLayer") {
                            std::string newName = factory->GetEntityType(draggedEntity);
                            size_t i = newName.find_last_of("_");
                            newName = newName.substr(i);
                            newName = "Tutorial_Game" + newName;
                            factory->SetEntityType(draggedEntity, newName);
                        }
                        else if (layerName == "TutorialBackgroundLayer") {
                            std::string newName = factory->GetEntityType(draggedEntity);
                            size_t i = newName.find_last_of("_");
                            newName = newName.substr(i);
                            newName = "Tutorial" + newName;
                            factory->SetEntityType(draggedEntity, newName);
                        }
                        else if (layerName == "TutorialEnvironmentLayer") {
                            std::string newName = factory->GetEntityType(draggedEntity);
                            size_t i = newName.find_last_of("_");
                            newName = newName.substr(i);
                            newName = "Tutorial_Environment" + newName;
                            factory->SetEntityType(draggedEntity, newName);
                        }
                        else if (layerName == "TutorialFontLayer") {
                            std::string newName = factory->GetEntityType(draggedEntity);
                            size_t i = newName.find_last_of("_");
                            newName = newName.substr(i);
                            newName = "Tutorial_Font" + newName;
                            factory->SetEntityType(draggedEntity, newName);
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }

            // --- If the tree node is open, display its objects.
            if (treeOpen) {
                auto* layerObjects = layerPtr->GetObjectContainer();
                if (layerObjects) {
                    for (int i = 0; i < static_cast<int>(layerObjects->size()); ++i) {
                        Entity entity = (*layerObjects)[i];
                        std::string label = factory->GetEntityType(entity) +
                            " (ID: " + std::to_string(entity) + ")";
                        //objectName = label;
                        bool isSelected = (entity == static_cast<Entity>(selectedObjectIndex));
                        if (ImGui::Selectable(label.c_str(), isSelected)) {
                            selectedObjectIndex = entity; // Update selection.
                        }

                        // Make each entity a drag source.
                        ImGui::PushID(static_cast<int>(entity));
                        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                            // Send the current index as the payload.
                            ImGui::SetDragDropPayload("OBJECT_ENTITY_REORDER", &i, sizeof(int));
                            ImGui::Text("Dragging: %s", label.c_str());
                            draggedEntity = entity;
                            draggedEntityIndex = i;
                            draggedFromLayer = layerName;
                            ImGui::EndDragDropSource();
                        }
                        ImGui::PopID();
                    }
                }
                ImGui::TreePop();
            }

            // A simple checkbox example (for enabling/disabling the layer).
            ImGui::Checkbox("Enable", layerPtr->SetVisibility());
            ImGui::PopID();
        }

        ImGui::End();
    }


/*!
* \brief Displays a detailed inspector panel for the selected game object,
* allowing the user to modify properties like position, scale, and components.
*/
    void ImGuiPanels::ShowPropertyEditor()
    {
        ImGui::Begin("Object Inspector");

        auto it = allObjects.find(selectedObjectIndex);
        if (it != allObjects.end()) {
            Entity selectedEntity = it->first; // Get the entity ID
            std::string& entityName = it->second;

            // Update displayed properties if the selection has changed
            if (selectedObjectIndex != previousObjectIndex) {
                previousObjectIndex = selectedObjectIndex;

                // Update objectName and initialize position/scale based on Transform
                //entityName = objectName;

                if (ecsHub->HasComponent<Transform>(selectedEntity)) {
                    Transform& transform = ecsHub->GetComponent<Transform>(selectedEntity);
                    objectPosition[0] = transform.position.x;
                    objectPosition[1] = transform.position.y;
                    objectScale[0] = transform.scale.x;
                    objectScale[1] = transform.scale.y;
                }
            }

            NotifyObservers(new ObjectSelectedMessage(selectedEntity));

            ImGui::Text("Object Information");
            ImGui::Separator();

            // Name Input
            ImGui::InputText("Name", &entityName[0], entityName.capacity() + 1);

            // Tag Input
            ImGui::InputText("Tag", &objectTag[0], objectTag.capacity() + 1);

            // Enabled Checkbox
            //ImGui::Checkbox("Enabled", &objectEnabled);

            ImGui::Text("Components");
            ImGui::Separator();

            // Transform section
            if (ecsHub->HasComponent<Transform>(selectedEntity)) {
                if (ImGui::CollapsingHeader("Transform")) {
                    // Modify position and scale with sliders
                    Transform& transform = ecsHub->GetComponent<Transform>(selectedEntity);
                    objectPosition[0] = transform.position.x;
                    objectPosition[1] = transform.position.y;

                    // Position Input
                    if (ImGui::InputFloat2("Position", objectPosition)) {
                        transform.position = Vector2D(objectPosition[0], objectPosition[1]);
#ifdef _DEBUG
                        spdlog::info("saving state start");
#endif
#ifdef _DEBUG
                        spdlog::info("saving satte working");
#endif
                        
                    }

                    // Scale Input
                    if (ImGui::InputFloat2("Scale", objectScale)) {
                        transform.scale = Vector2D(objectScale[0], objectScale[1]);
                        spdlog::info("Updated Transform scale to ({}, {})", objectScale[0], objectScale[1]);
                    }

                    // orientation Input
                    if (ImGui::InputFloat2("Rotation", objectRotation)) {
                        transform.orientation = Vector2D(objectRotation[0], objectRotation[1]);
                        spdlog::info("Updated Transform rotate to ({}, {})", objectScale[0], objectScale[1]);
                    }


                }
            }


            // ButtonComponent Section
            if (ecsHub->HasComponent<ButtonComponent>(selectedEntity)) {
                if (ImGui::CollapsingHeader("Button")) {
                    // Access ButtonComponent
                    ButtonComponent& buttonComponent = ecsHub->GetComponent<ButtonComponent>(selectedEntity);

                    // Create a dropdown for ButtonComponent booleans
                    ImGui::Checkbox("Is Hovered", &buttonComponent.isHovered);
                    ImGui::Checkbox("Is Clicked", &buttonComponent.isClicked);
                    ImGui::Checkbox("Is Enabled", &buttonComponent.isEnabled);
                }
            }

            // UnitComponent Section
            if (ecsHub->HasComponent<UnitComponent>(selectedEntity)) {
                if (ImGui::CollapsingHeader("Unit Component")) {
                    UnitComponent& unit = ecsHub->GetComponent<UnitComponent>(selectedEntity);

                    const char* unitTypes[] = { "Player", "Enemy" };
                    int unitTypeIndex = static_cast<int>(unit.unitType);
                    if (ImGui::Combo("Unit Type", &unitTypeIndex, unitTypes, IM_ARRAYSIZE(unitTypes))) {
                        unit.unitType = static_cast<UnitType>(unitTypeIndex);
#ifdef _DEBUG
                        spdlog::info("Changed UnitType for Entity %d to %d", selectedEntity, unitTypeIndex);
#endif
                    }

                    ImGui::InputInt("Health", &unit.health);
                    unit.health = std::clamp(unit.health, 0, unit.maxHealth);

                    ImGui::InputInt("Max Health", &unit.maxHealth);
                    unit.maxHealth = std::max(unit.maxHealth, 1);

                    ImGui::InputInt("Attack Power", &unit.attackPower);

                    ImGui::InputInt("Energy", &unit.energy);
                    unit.energy = std::clamp(unit.energy, 0, unit.maxEnergy);

                    ImGui::InputInt("Max Energy", &unit.maxEnergy);
                    unit.maxEnergy = std::max(unit.maxEnergy, 1);

                    ImGui::InputFloat2("Position", &unit.position.x);

                    ImGui::Text("Occupied Tile: %d", unit.occupiedTile);
                }
            }

            // TileComponent Section
            if (ecsHub->HasComponent<TileComponent>(selectedEntity)) {
                if (ImGui::CollapsingHeader("Tile Component")) {
                    TileComponent& tile = ecsHub->GetComponent<TileComponent>(selectedEntity);

                    ImGui::Checkbox("Walkable", &tile.walkable);
                    ImGui::Checkbox("Highlighted", &tile.highlighted);

                    const char* tileTypes[] = { "Floor", "Wall" };
                    int tileTypeIndex = static_cast<int>(tile.tileType);
                    if (ImGui::Combo("Tile Type", &tileTypeIndex, tileTypes, IM_ARRAYSIZE(tileTypes))) {
                        tile.tileType = static_cast<TileType>(tileTypeIndex);
                    }

                    // Position Input for TileComponent
                    if (ImGui::InputFloat2("Position", &tile.position.x)) {
                        // Sync the Transform's position if it exists
                        if (ecsHub->HasComponent<Transform>(selectedEntity)) {
                            Transform& transform = ecsHub->GetComponent<Transform>(selectedEntity);
                            transform.position = tile.position;
#ifdef DEBUG

                            spdlog::info("Synchronized Transform position to TileComponent position: ({}, {})", tile.position.x, tile.position.y);
#endif // DEBUG
                        }
                    }

                    ImGui::InputFloat("Movement Cost", &tile.movementCost);
                    tile.movementCost = std::max(tile.movementCost, 0.0f);

                    ImGui::Text("Occupant: %d", tile.occupant);
                    if (tile.occupant != Entity()) {
                        if (ImGui::Button("Clear Occupant")) {
                            tile.ClearOccupant();
                        }
                    }
                }
            }
            
            // ScriptComponent Section
            if (ecsHub->HasComponent<ScriptComponent>(selectedEntity)) {
                if (ImGui::CollapsingHeader("Script Component")) {
                    ScriptComponent& scriptComponent = ecsHub->GetComponent<ScriptComponent>(selectedEntity);

                    // Prepare to display a dropdown box of behaviors
                    static int current_item = -1; // Index of the selected item
                    std::vector<std::string> behaviorNames; // List of behavior names for the dropdown
                    int i = 0;

                    // Collect names from the behavior map
                    for (const auto& kv : ScriptSystem::GetBehaviorMap()) {
                        behaviorNames.push_back(kv.first);
                        if (kv.first == scriptComponent.Name) {
                            current_item = i; // Set the initial selection to the current behavior
                        }
                        ++i;
                    }

                    if (behaviorNames.empty()) {
                        ImGui::Text("No behaviors available");
                    }
                    else {
                        if (ImGui::BeginCombo("##Behavior Selection", scriptComponent.Name.c_str())) {
                            for (int n = 0; n < behaviorNames.size(); n++) {
                                const bool isSelected = (current_item == n);
                                if (ImGui::Selectable(behaviorNames[n].c_str(), isSelected)) {
                                    current_item = n;
                                    scriptComponent.Name = behaviorNames[n]; // Update the component name
                                    // Check if the behavior is valid and add it
                                    if (ScriptSystem::IsValidBehavior(behaviorNames[n])) {
                                        ScriptSystem::AddBehaviorToEntity(selectedEntity, behaviorNames[n]);
                                    }
                                }

                                // Set the initial focus when opening the combo
                                if (isSelected)
                                    ImGui::SetItemDefaultFocus();
                            }
                            ImGui::EndCombo();
                        }
                    }

                    // Check if the current name matches a valid behavior
                    bool isValidBehavior = ScriptSystem::IsValidBehavior(scriptComponent.Name);

                    if (isValidBehavior) {
                        ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(0, 255, 0, 255));
                        ImGui::Text("Valid Behavior");
                        ImGui::PopStyleColor();
                    }
                    else {
                        ImGui::Text("Invalid Behavior");
                    }

                    // Display script instance status
                    if (scriptComponent.Instance) {
                        ImGui::Text("Script Instance: Active");
                    }
                    else {
                        ImGui::Text("Script Instance: None or Invalid");
                    }
                }
            }

            // Material Section
            if (ecsHub->HasComponent<Material>(selectedEntity))
            {
                if (ImGui::CollapsingHeader("Material")) {
                    Material& material = ecsHub->GetComponent<Material>(selectedEntity);

                    // --- Display Current Texture ---
                    ImVec2 croppedPreviewSize = ImVec2(128, 128);
                    GLuint textureID = assetsManager->GetTextures(material.textureName);

                    if (textureID) {
                        ImGui::Image(
                            (void*)(intptr_t)textureID,
                            croppedPreviewSize,
                            ImVec2(material.uvMin.x, material.uvMax.y), // Adjust for flipped Y
                            ImVec2(material.uvMax.x, material.uvMin.y)  // Adjust for flipped Y
                        );
                    }
                    else {
                        ImGui::Text("No texture assigned.");
                    }

                    if (ImGui::BeginDragDropTarget()) {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                            std::string assetPath = (const char*)payload->Data;
                            size_t pos = assetPath.find(".");
                            assetPath = assetPath.substr(0, pos);

                            // Load and instantiate the asset in the scene
#ifdef _DEBUG
                            spdlog::info("Dropped asset: {}", assetPath);
#endif
                            // Use the factory to duplicate the selected object
                            material.textureName = assetPath;
                        }
                        ImGui::EndDragDropTarget();
                    }

                    // --- UV Controls ---
                    ImGui::InputFloat2("UV Min", (float*)&material.uvMin);
                    material.uvMin.x = std::clamp(material.uvMin.x, 0.0f, 1.0f);
                    material.uvMin.y = std::clamp(material.uvMin.y, 0.0f, 1.0f);

                    ImGui::InputFloat2("UV Max", (float*)&material.uvMax);
                    material.uvMax.x = std::clamp(material.uvMax.x, 0.0f, 1.0f);
                    material.uvMax.y = std::clamp(material.uvMax.y, 0.0f, 1.0f);

                    if (ImGui::Button("Delete Texture")) {
                        ImGui::OpenPopup("Delete Texture Warning");
                    }
                    if (ImGui::BeginPopupModal("Delete Texture Warning", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                        ImGui::Text("Are you sure you want to delete the texture?");
                        ImGui::Separator();

                        if (ImGui::Button("Yes", ImVec2(120, 0))) {
                            material.textureName = "fruitloops"; // Delete the texture
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("No", ImVec2(120, 0))) {
                            ImGui::CloseCurrentPopup();
                        }
                        ImGui::EndPopup();
                    }
                    
                }
            }
            
            
            

            // Font Section
            if (ecsHub->HasComponent<FontDataComponent>(selectedEntity)) {
                if (ImGui::CollapsingHeader("Font Data")) {
                    FontDataComponent& font = ecsHub->GetComponent<FontDataComponent>(selectedEntity);

                    const std::map<std::string, Font>& fontContainer = assetsManager->GetFontContainer();

                    // Prepare a list of font names for the dropdown
                    static std::vector<const char*> fontNames;
                    static std::string selectedFont = font.font; // Currently selected font
                    if (fontNames.empty()) {
                        for (const auto& [fontName, fontData] : fontContainer) {
                            fontNames.push_back(fontName.c_str());
                        }
                    }

                    // Font Dropdown
                    ImGui::Text("Font:");
                    if (ImGui::BeginCombo("##FontDropdown", selectedFont.c_str())) {
                        for (size_t i = 0; i < fontNames.size(); ++i) {
                            bool isSelected = (selectedFont == fontNames[i]);
                            if (ImGui::Selectable(fontNames[i], isSelected)) {
                                selectedFont = fontNames[i]; // Update selected font
                                font.font = selectedFont;   // Update the FontDataComponent
                            }
                            if (isSelected) {
                                ImGui::SetItemDefaultFocus();
                            }
                        }
                        ImGui::EndCombo();
                    }

                    // Text Input
                    char fontbuffer[256];
                    strncpy_s(fontbuffer, font.text.c_str(), sizeof(fontbuffer));
                    ImGui::Text("Text:");
                    if (ImGui::InputText("##TextInput", fontbuffer, IM_ARRAYSIZE(fontbuffer))) { // Use another unique label
                        font.text = fontbuffer;
                    }

                    ImGui::Text("Scale:");
                    ImGui::InputFloat("##FontScale", (float*)&font.scale);
                }
            }

            // Check if the selected object has a RigidBody component
            if (ecsHub->HasComponent<RigidBody>(selectedEntity)) {
                // Synchronize the RigidBody position with the new Transform position
                RigidBody& rigidBody = ecsHub->GetComponent<RigidBody>(selectedEntity);
                rigidBody.position = Vector2D(objectPosition[0], objectPosition[1]);
            }

            if (ecsHub->HasComponent<RigidBody>(selectedEntity) && ImGui::CollapsingHeader("Physics")) {
                RigidBody& rigidBody = ecsHub->GetComponent<RigidBody>(selectedEntity);
                PhysicsObject physicsObject;

                // Mass
                ImGui::Text("Mass");
                if (ImGui::InputFloat("Mass", &rigidBody.mass)) {
                    // Ensure mass is not zero to avoid division by zero errors
                    if (rigidBody.mass < 0.01f) rigidBody.mass = 0.01f;
                }

                // Velocity
                ImGui::Text("Velocity");
                ImGui::InputFloat2("Velocity", &rigidBody.velocity.x);

                // Acceleration
                ImGui::Text("Acceleration");
                ImGui::InputFloat2("Acceleration", &physicsObject.acceleration.x);

                // Apply Force
                static Vector2D forceToApply(0.0f, 0.0f);
                ImGui::Text("Apply Force");
                ImGui::InputFloat2("Force", &forceToApply.x);
                if (ImGui::Button("Apply Force")) {
                    physicsObject.ApplyForce(forceToApply);
                }

                // Gravity Toggle
                static bool gravityEnabled = true;
                ImGui::Checkbox("Enable Gravity", &gravityEnabled);
                if (gravityEnabled) {

                }

                // Dampening
                static float damping = 0.98f; // Adjust this to control friction or air resistance
                if (ImGui::SliderFloat("Damping", &damping, 0.0f, 1.0f)) {
                    rigidBody.velocity *= damping;
                }
            }

            
            ModifyComponents(selectedEntity);
            

            ImGui::Separator();

            // Spawn Object button
            if (ImGui::Button("Spawn Object")) {
                // Use the factory to duplicate the selected object
                Entity newEntity = factory->DuplicateGameObject(selectedEntity,
                    Vector2D(objectPosition[0] + 50.0f, objectPosition[1] + 50.0f),  // Offset new object slightly for visibility
                    Vector2D(objectScale[0], objectScale[1]),
                    Vector2D(objectRotation[0], objectRotation[1])
                );

                // Optionally, update the dropdown to include the new object in the list
                
                std::string newLabel = factory->GetEntityType(selectedEntity) + " (ID: " + std::to_string(newEntity) + ")";
                allObjects[newEntity] = newLabel;
#ifdef _DEBUG
                spdlog::info("Spawned new object with ID: {}", newEntity);
#endif
            }

            // Check if Delete key is pressed
            if (ImGui::IsKeyPressed(ImGuiKey_Delete, false)) { // `false` ensures no repeat if held
                if (selectedObjectIndex >= 0) {
                    // Show confirmation popup
                    ImGui::OpenPopup("Despawn Object Confirmation");
                }
                else {
#ifdef _DEBUG
                    spdlog::warn("No valid object selected to despawn.");
#endif
                }
            }

            // Despawn Object button
            if (ImGui::Button("Despawn Object")) {
                ImGui::OpenPopup("Despawn Object Confirmation");
            }
            if (ImGui::BeginPopupModal("Despawn Object Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
                ImGui::Text("Are you sure you want to despawn the selected object?");
                ImGui::Separator();

                if (ImGui::Button("Yes", ImVec2(120, 0))) {
                    // Ensure there is a valid selection
                    if (selectedObjectIndex >= 0) {
                        NotifyObservers(new DrawGridMessage(false));
                        showGrid = false;

                        // check if it exists in animation container, if so delete it
                        for (auto& animations : GRAPHICS->animatorContainer) {
                            if (animations.DelEntity(selectedEntity)) {
                                break;
                            }
                        }

                        // Remove the selected object from the scene
                        factory->DestroyGameObject(selectedEntity);
                        allObjects.erase(selectedEntity);
#ifdef _DEBUG
                        spdlog::info("Despawned object with ID: {}", selectedEntity);
#endif

                        // Reset the selected and previous indices
                        selectedObjectIndex = 0;
                        previousObjectIndex = -1;

                        // Optionally, you can set it to another valid object if available
                        if (!allObjects.empty()) {
                            selectedObjectIndex = 0;
                        }
                    }
                    else {
#ifdef _DEBUG
                        spdlog::warn("No valid object selected to despawn.");
#endif
                    }
                    ImGui::CloseCurrentPopup();
                }

                ImGui::SameLine();
                if (ImGui::Button("No", ImVec2(120, 0))) {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
            // TOGGLE BUTTONS
            if (ImGui::Button("Move")) {
                moveMode = true;
                scaleMode = false;
                rotateMode = false;
                PrefabManager::SetMoveMode(true);
                PrefabManager::SetScaleMode(false);
                PrefabManager::SetRotateMode(false);
            }

            ImGui::SameLine();
            if (ImGui::Button("Scale")) {
                moveMode = false;
                scaleMode = true;
                rotateMode = false;
                PrefabManager::SetMoveMode(false);
                PrefabManager::SetScaleMode(true);
                PrefabManager::SetRotateMode(false);
            }

            ImGui::SameLine();
            if (ImGui::Button("Rotate")) {
                moveMode = false;
                scaleMode = false;
                rotateMode = true;
                PrefabManager::SetMoveMode(false);
                PrefabManager::SetScaleMode(false);
                PrefabManager::SetRotateMode(true);
            }

            ImGui::Text("Current Mode: %s", PrefabManager::IsMoveMode() ? "Move" :
                PrefabManager::IsScaleMode() ? "Scale" : "Rotate");
        }
        else {
            ImGui::Text("No object selected.");
        }

        ImGui::End();
    }

/*!
* \brief Displays a content browser for managing assets, organized into tabs.
* Supports drag-and-drop functionality to load assets into the game.
*/
    void ImGuiPanels::ShowContentBrowser()
    {
        ImGui::Begin("Content Browser");

        // Create a tab bar for multiple tabs in the content browser
        if (ImGui::BeginTabBar("ContentTabs")) {

            // Tab for scenes
            if (ImGui::BeginTabItem("Logs")) {
                ImGui::Text("Engine Logs:");

                //DisplayLogsInImGui();
                ImGui::EndTabItem();
            }

            // Tab for general assets
            if (ImGui::BeginTabItem("Assets")) {
                // Button to go up one level in the directory structure
                if (currentDirectory != "../Assets") {
                    if (ImGui::Button("<-")) {
                        currentDirectory = currentDirectory.parent_path();
                    }
                }

                // Fetch directory contents
                std::vector<FileItem> items = assetsManager->LoadDirectoryContents(currentDirectory);
                // Display each file/folder name in a grid layout
                ImGui::Columns(columns, nullptr, false);
                ImGui::SetColumnWidth(-1, iconSize.x + padding);

                // Track selected item for deletion
                static std::string selectedItem;

                for (const auto& item : items) {
                    ImGui::PushID(item.name.c_str()); // Ensure unique ID for each item

                    // Begin group to handle icon and text together
                    ImGui::BeginGroup();

                    if (item.isDirectory) {
                        if (folderIconTexture != 0) {
                            //spdlog::info("Rendering folder icon for: {}", item.name);
                            ImGui::Image((void*)(intptr_t)folderIconTexture, iconSize, ImVec2(0, 1), ImVec2(1, 0));
                        }
                        else {
#ifdef _DEBUG
                            spdlog::warn("Folder icon texture not loaded!");
#endif
                            ImGui::Text("[Folder]");
                        }
                    }
                    else {
                        // Extract file extension from item.name
                        std::string extension = "";
                        size_t dotPos = item.name.find_last_of('.');
                        if (dotPos != std::string::npos) {
                            extension = item.name.substr(dotPos);
                            std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower); // Convert to lowercase
                        }
                        else {
#ifdef _DEBUG
                            spdlog::warn("No extension found for file: {}", item.name);
#endif
                        }

                        // Debug: log the extension and file name
#ifdef _DEBUG
                        //spdlog::info("File: {}, Extension: {}", item.name, extension);
#endif

                        // Handle file icons based on file type
                        GLuint iconToRender = 0;

                        if (extension == ".mp3" || extension == ".wav") {
                            iconToRender = audioIconTexture; // Audio files
                        }
                        else if (extension == ".txt" || extension == ".md") {
                            iconToRender = textIconTexture; // Text files
                        }
                        else if (extension == ".ttf" || extension == ".otf") {
                            iconToRender = textfileIconTexture; // Font files
                        }
                        else {
#ifdef _DEBUG
                            //spdlog::info("Using default icon for file: {}", item.name);
#endif
                            iconToRender = item.icon; // Default icon
                        }

                        if (iconToRender != 0) {
                            ImGui::Image((void*)(intptr_t)iconToRender, iconSize, ImVec2(0, 1), ImVec2(1, 0));
                        }
                        else {
#ifdef _DEBUG
                            spdlog::warn("No icon for file: {}", item.name);
#endif
                            ImGui::Text("[File]");
                        }
                    }

                    // Make the icon clickable
                    ImVec2 iconMin = ImGui::GetItemRectMin();
                    ImVec2 iconMax = ImGui::GetItemRectMax();
                    ImGui::SetCursorScreenPos(iconMin); // Align cursor to the icon position
                    if (ImGui::InvisibleButton("##IconClick", iconSize)) { // Create an invisible button over the icon
                        if (item.isDirectory) {
                            currentDirectory /= item.name; // Navigate into the directory if clicked
                            
#ifdef _DEBUG
                            spdlog::info("Navigated to directory: {}", item.name);
#endif
                        }
                        else {
                            selectedItem = (currentDirectory / item.name).string();
#ifdef _DEBUG
                            spdlog::info("Clicked on file: {}", selectedItem); // Handle file click
#endif
                        }
                    }

                    // Handle drag-and-drop logic for both the icon and the text
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                        ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", item.name.c_str(), item.name.size() + 1);
                        //ImGui::Text("%s", item.name.c_str()); // Show the name as the drag preview
                        // Show the icon as the drag preview
                        if (item.isDirectory) {
                            ImGui::Image((void*)(intptr_t)folderIconTexture, iconSize, ImVec2(0, 1), ImVec2(1, 0));
                        }
                        else {
                            if (item.icon != 0) {
                                ImGui::Image((void*)(intptr_t)item.icon, iconSize, ImVec2(0, 1), ImVec2(1, 0));
                            }
                            else {
                                ImGui::Text("[File]");
                            }
                        }
                        ImGui::EndDragDropSource();
                    }

                    // Center the text label below the icon
                    ImVec2 textSize = ImGui::CalcTextSize(item.name.c_str());
                    float textOffset = (iconSize.x - textSize.x) / 2.0f; // Calculate offset to center text
                    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + textOffset);
                    ImGui::TextWrapped("%s", item.name.c_str());

                    // Drag-and-drop for the text
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
                        ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID);
                        ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", item.name.c_str(), item.name.size() + 1);
                        ImGui::Text("%s", item.name.c_str());
                        ImGui::EndDragDropSource();
                    }

                    // End the group
                    ImGui::EndGroup();

                    ImGui::NextColumn();
                    ImGui::PopID();
                }
                ImGui::Columns(1);

                // Show the delete button if an item is selected
                if (!selectedItem.empty()) {
                    if (ImGui::Button("Delete Selected")) {
                        try {
                            if (std::filesystem::exists(selectedItem)) {
                                std::filesystem::remove_all(selectedItem);
                                spdlog::info("Deleted: {}", selectedItem);
                            }
                            selectedItem.clear();
                        }
                        catch (const std::filesystem::filesystem_error& e) {
                            spdlog::error("Error deleting file: {}", e.what());
                        }
                    }
                }

                ImGui::EndTabItem();
            }

            // Tab for scripts
            if (ImGui::BeginTabItem("Scripts")) {
                ImGui::Text("Here you could list scripts or script files.");
                // Add code to display script files or directories as needed
                ImGui::EndTabItem();
            }

            // End the tab bar
            ImGui::EndTabBar();
        }

        ImGui::End();
    }

/*!
* \brief Provides controls to play, pause, stop, and adjust volume for
* individual sounds or background music.
* \param soundKey The identifier for the sound being controlled.
* \param isBGM A flag indicating if the sound is background music.
*/
    void ImGuiPanels::SoundHandler(const std::string& soundKey, bool isBGM) {
        static float volume = 1.0f; // Shared volume variable
        static float bgmVolume = 0.2f; // Separate volume variable for BGM

        // Check if the audio system is initialized
        if (!audioSystem) {


            ImGui::Text("Audio system not initialized.");
            return;
        }

        ImGui::PushID(soundKey.c_str()); // Ensure unique ID for this soundKey

        if (isBGM) {
            // Only show the volume slider for BGM
            ImGui::Text("Adjust BGM Volume");
            ImGui::SliderFloat("Volume", &bgmVolume, 0.0f, 1.0f);

            FMOD::Channel* pChannel = nullptr;
            auto it = audioSystem->GetChannelMap().find(soundKey);
            if (it != audioSystem->GetChannelMap().end()) {
                pChannel = it->second;
                if (pChannel) {
                    pChannel->setVolume(bgmVolume);
                }
            }
            else {
                ImGui::Text("No active channel for BGM.");
            }
        }
        else {
            // Full sound handling for sound effects
            FMOD::Channel* pChannel = nullptr;
            auto it = audioSystem->GetChannelMap().find(soundKey);
            if (it != audioSystem->GetChannelMap().end()) {
                pChannel = it->second;
            }

            ImGui::Text("Sound: %s", soundKey.c_str());

            // Play Button
            if (ImGui::Button("Play")) {
                audioSystem->Play_Sound(soundKey,0.3f);
            }

            // Pause Button
            ImGui::SameLine();
            if (ImGui::Button("Pause")) {
                if (pChannel) {
                    bool isaudioPaused = false;
                    pChannel->getPaused(&isaudioPaused);
                    pChannel->setPaused(!isaudioPaused);
                }
            }

            // Stop Button
            ImGui::SameLine();
            if (ImGui::Button("Stop")) {
                audioSystem->Stop_Sound(soundKey);
            }

            // Volume Slider
            if (pChannel) {
                ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f);
                pChannel->setVolume(volume);
            }
            else {
                ImGui::Text("No active channel for this sound.");
            }
        }

        ImGui::PopID();
    }
    
/*!
* \brief Displays a sound manager panel for adjusting settings, changing BGM,
* and managing sound effects. Supports drag-and-drop for setting the BGM.
*/
    void ImGuiPanels::ShowSoundHandler()
    {
        static bool showInvalidFilePopup = false; // Flag to show the invalid file popup
        static std::string invalidFileName;      // Store the invalid file name

        ImGui::Begin("Sound Manager");
        ImGuiPanels::SoundHandler("bounce", false);
        ImGuiPanels::SoundHandler("bling", false);
        ImGuiPanels::SoundHandler("FoodReady", false);

        ImGui::Separator();

        ImGui::Text("Current BGM: %s", ConfigData::BGM.c_str());

        // Drag-and-drop target for audio files
        ImGui::Text("Drag and drop an audio file here to set it as BGM:");
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                std::string assetPath = (const char*)payload->Data;

                // Extract the file extension
                size_t pos = assetPath.find_last_of('.');
                std::string extension = (pos != std::string::npos) ? assetPath.substr(pos) : "";

                // Convert the extension to lowercase for comparison
                std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

                if (extension != ".wav") {
                    // Trigger the popup if the file is not a .wav
                    showInvalidFilePopup = true;
                    invalidFileName = assetPath;
                }
                else {
                    // If valid, set it as the new BGM
                    assetPath = assetPath.substr(0, pos);
                    ConfigData::BGM = assetPath;

                    // Use AudioSystem to set the BGM
                    audioSystem->TransitionMusic(ConfigData::BGM, 2.0f);

#ifdef _DEBUG
                    spdlog::info("New BGM set: {}", assetPath);
#endif
                }
            }
            ImGui::EndDragDropTarget();
        }

        // Popup for invalid file type
        if (showInvalidFilePopup) {
            ImGui::OpenPopup("Invalid File");
        }

        if (ImGui::BeginPopupModal("Invalid File", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("The file \"%s\" is not a valid .wav file.", invalidFileName.c_str());
            ImGui::Separator();

            if (ImGui::Button("OK")) {
                showInvalidFilePopup = false; // Close the popup
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGuiPanels::SoundHandler(ConfigData::BGM, true);
        ImGui::End();
    }

    void ImGuiPanels::ShowAnimationEditor()
    {
        ImGui::Begin("Animation Editor");

        // section for animation
        static int selectedAnimIndex = 0;
        std::vector<SpriteAnimator>& animContainer = GRAPHICS->animatorContainer;
        if (animContainer.empty()) {
            ImGui::Text("No animations available.");
            return;
        }

        // Dropdown to select an animation
        if (ImGui::BeginCombo("Select Animation", std::to_string(selectedAnimIndex).c_str())) {
            for (size_t i = 0; i < animContainer.size(); ++i) {
                bool isSelected = (selectedAnimIndex == i);
                if (ImGui::Selectable(("Animation " + std::to_string(i)).c_str(), isSelected)) {
                    selectedAnimIndex = static_cast<int>(i);
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        SpriteAnimator& anim = animContainer[selectedAnimIndex]; // Work on the selected animation
        GLuint animtexture = assetsManager->GetTextures(anim.GetTextureName());

        // Display the texture preview
        if (animtexture) {
            ImVec2 croppedPreviewSize = ImVec2(128, 128);
            ImGui::Image((void*)(intptr_t)animtexture, croppedPreviewSize, ImVec2(0, 1), ImVec2(1, 0));
        }
        else {
            ImGui::Text("No texture assigned.");
        }

        // Drag and drop texture assignment
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM")) {
                std::string assetPath = (const char*)payload->Data;
                size_t pos = assetPath.find(".");
                assetPath = assetPath.substr(0, pos);

#ifdef _DEBUG
                spdlog::info("Dropped asset: {}", assetPath);
#endif
                anim.SetTextureName(assetPath);
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::Separator();

        // === Entity Bucket Editing ===
        ImGui::Text("Entities in Animation:");
        std::vector<Entity> entityBucket = anim.GetEntity();

        if (entityBucket.empty()) {
            ImGui::Text("No entities assigned.");
        }
        else {
            for (size_t i = 0; i < entityBucket.size(); ++i) {
                ImGui::PushID(static_cast<int>(i));
                ImGui::Text("Entity %d", entityBucket[i]);
                ImGui::SameLine();
                if (ImGui::Button("Remove")) {
                    anim.DelEntity(entityBucket[i]); // Remove entity
                }
                ImGui::PopID();
            }
        }

        // Add new entity
        static int newEntityID = 0;
        ImGui::InputInt("New Entity ID", &newEntityID);
        if (ImGui::Button("Add Entity")) {
            anim.AddEntity(newEntityID);
#ifdef _DEBUG
            spdlog::info("Added entity: %d", newEntityID);
#endif
        }

        ImGui::Separator();

        // Animation Speed
        ImGui::Text("Animation Speed:");
        float animSpeed = anim.GetSpeed();
        if (ImGui::InputFloat("##Speed", &animSpeed, 0.01f, 0.1f)) {
            anim.SetSpeed(std::max(0.0f, animSpeed));  // Ensure non-negative speed
        }

        // Animation Position
        ImGui::Text("Animation Position:");
        Vector2D animPos = anim.GetPosition();
        if (ImGui::InputFloat2("##AnimPosition", (float*)&animPos)) {
            anim.SetPosition(animPos);
        }

        // Animation Scale
        ImGui::Text("Animation Scale:");
        Vector2D animScale = anim.GetScale();
        if (ImGui::InputFloat2("##AnimScale", (float*)&animScale)) {
            anim.SetScale(animScale);
        }

        // Animation Position Offset
        ImGui::Text("Animation Position Offset:");
        Vector2D animPosOff = anim.GetPositionOffset();
        if (ImGui::InputFloat2("##AnimPositionOffset", (float*)&animPosOff)) {
            anim.SetPositionOffset(animPosOff);
        }

        // Animation Scale Offset
        ImGui::Text("Animation Scale Offset:");
        Vector2D animScaleOff = anim.GetScaleOffset();
        if (ImGui::InputFloat2("##AnimScaleOffset", (float*)&animScaleOff)) {
            anim.SetScaleOffset(animScaleOff);
        }

        // Texture Name
        ImGui::Text("Texture:");
        char buffer[256];
        strncpy_s(buffer, anim.GetTextureName().c_str(), sizeof(buffer));
        if (ImGui::InputText("##Texture Name", buffer, IM_ARRAYSIZE(buffer))) {
            anim.SetTextureName(buffer);
        }

        // Frame Size
        ImGui::Text("Frame Size:");
        Vector2D frameSize = anim.GetFrameSize();
        if (ImGui::InputFloat2("##Width, Height", (float*)&frameSize)) {
            anim.SetFrameSize(frameSize);
        }

        // Playing Checkbox
        bool playing = anim.GetPlaying();
        if (ImGui::Checkbox("Playing", &playing)) {
            anim.SetPlaying(playing);
        }

        // Looping Checkbox
        bool looping = anim.GetLooping();
        if (ImGui::Checkbox("Looping", &looping)) {
            anim.SetLooping(looping);
        }

        // Frames Per Row
        ImGui::Text("Frames Per Row:");
        std::vector<int> framesPerRow = anim.GetFramesPerRow();
        static int newFrame = 0;

        for (size_t i = 0; i < framesPerRow.size(); ++i) {
            ImGui::PushID(static_cast<int>(i)); // Unique ID
            ImGui::InputInt(("Row " + std::to_string(i)).c_str(), &framesPerRow[i]);
            if (ImGui::Button("Delete Row")) {
                framesPerRow.erase(framesPerRow.begin() + i);
            }
            ImGui::PopID();
        }

        // Add new row
        ImGui::InputInt("New Row", &newFrame);
        if (ImGui::Button("Add Row")) {
            framesPerRow.push_back(newFrame);
            newFrame = 0;
        }

        // Apply frames per row
        anim.SetFramesPerRow(framesPerRow);

        ImGui::Separator();

        // Sound Mapping
        ImGui::Text("Attach Sound to Frame");

        std::unordered_map<size_t, std::string> frameSoundMap = anim.GetSoundMap();

        for (const auto& [frame, sound] : frameSoundMap) {
            ImGui::Text("Frame %zu: %s", frame, sound.c_str());
        }

        static int frameInput = 1;
        static char selectedSound[256] = "";
        static const char* currentSound = nullptr;

        size_t totalFrames = anim.GetTotalFrames();

        if (totalFrames) {
            ImGui::Text("Frame:");
            ImGui::InputInt("##Frame", &frameInput);
            frameInput = std::clamp(frameInput, 1, static_cast<int>(totalFrames));

            // Sound selection dropdown
            const auto& soundMap = assetsManager->GetSoundContainer();
            ImGui::Text("Sound Key:");
            if (ImGui::BeginCombo("##Sound Key", currentSound)) {
                for (const auto& [soundName, soundData] : soundMap) {
                    bool isSelected = (currentSound && strcmp(currentSound, soundName.c_str()) == 0);
                    if (ImGui::Selectable(soundName.c_str(), isSelected)) {
                        currentSound = soundName.c_str();
                        strncpy_s(selectedSound, soundName.c_str(), sizeof(selectedSound));
                    }
                    if (isSelected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            // Add new mapping
            if (ImGui::Button("Add Mapping") && currentSound) {
                frameSoundMap[frameInput] = selectedSound;
#ifdef _DEBUG
                spdlog::info("Mapped sound '%s' to frame %d", selectedSound, frameInput);
#endif
            }

            ImGui::Separator();

            // Remove a mapping
            static int removeFrame = 0;
            ImGui::Text("Remove Sound For Frame:");
            ImGui::InputInt("##Remove Sound", &removeFrame);
            if (ImGui::Button("Remove Mapping")) {
                frameSoundMap.erase(removeFrame);
#ifdef _DEBUG
                spdlog::info("Removed sound mapping for frame {}", removeFrame);
#endif
            }
            

            // Apply sound map changes
            anim.SetSoundMap(frameSoundMap);

        }
    }

/*!
* \brief Allows the user to add or remove components for the selected entity.
* Displays a list of available components and updates the entity accordingly.
* \param selectedEntity The ID of the currently selected entity.
*/
    void ImGuiPanels::ModifyComponents(Entity selectedEntity) {
        //ImGui::Begin("Modify Components");

        if (selectedEntity == -1) {
            ImGui::Text("No object selected.");
            //ImGui::End();
            return;
        }

        ImGui::Separator();
        ImGui::Text("Modify Components for Entity: %d", selectedEntity);
        

        // List all components and allow toggling
        static const std::vector<std::string> availableComponents = {
            "Transform",
            "RigidBody",
            "Material",
            "Button",
            "AnimationComponent",
            "FontDataComponent",
            "UnitComponent",
            "TileComponent",
            "ScriptComponent"
        };

        for (const auto& componentName : availableComponents) {
            bool hasComponent = false;

            // Check if the entity currently has the component
            if (componentName == "Transform") {
                hasComponent = ecsHub->HasComponent<Transform>(selectedEntity);
            }
            else if (componentName == "RigidBody") {
                hasComponent = ecsHub->HasComponent<RigidBody>(selectedEntity);
            }
            else if (componentName == "Material") {
                hasComponent = ecsHub->HasComponent<Material>(selectedEntity);
            }
            else if (componentName == "Button") {
                hasComponent = ecsHub->HasComponent<ButtonComponent>(selectedEntity);
            }
            else if (componentName == "FontDataComponent") {
                hasComponent = ecsHub->HasComponent<FontDataComponent>(selectedEntity);
            }
            else if (componentName == "UnitComponent") {
                hasComponent = ecsHub->HasComponent<UnitComponent>(selectedEntity);
            }
            else if (componentName == "TileComponent") {
                hasComponent = ecsHub->HasComponent<TileComponent>(selectedEntity);
            }
            else if (componentName == "ScriptComponent") {
                hasComponent = ecsHub->HasComponent<ScriptComponent>(selectedEntity);
            }

            // Display checkbox to add or remove the component
            bool toggle = hasComponent;
            if (ImGui::Checkbox(componentName.c_str(), &toggle)) {
                if (toggle && !hasComponent) {
                    // Add component
                    
                    if (componentName == "Transform") {
                        ecsHub->AddComponent<Transform>(selectedEntity, Transform{});
                    }
                    else if (componentName == "RigidBody") {
                        ecsHub->AddComponent<RigidBody>(selectedEntity, RigidBody{});
                        NotifyObservers(new RigidBodyComponentChange(true));
                    }
                    else if (componentName == "Material") {
                        ecsHub->AddComponent<Material>(selectedEntity, Material{});
                    }
                    else if (componentName == "Button") {
                        ecsHub->AddComponent<ButtonComponent>(selectedEntity, ButtonComponent{});
                    }
                    else if (componentName == "FontDataComponent") {
                        ecsHub->AddComponent<FontDataComponent>(selectedEntity, FontDataComponent{"Roundfont-Regular"});
                        NotifyObservers(new FontSpawnedMessage(selectedEntity));
                    }
                    else if (componentName == "UnitComponent") {
                        ecsHub->AddComponent<UnitComponent>(selectedEntity, UnitComponent{});
                    }
                    else if (componentName == "TileComponent") {
                        if (ecsHub->HasComponent<Transform>(selectedEntity)) {
                            Transform& transform = ecsHub->GetComponent<Transform>(selectedEntity);
                            ecsHub->AddComponent<TileComponent>(selectedEntity, TileComponent{ true, false, false, transform.position, TileType::Floor });
                        }
                    }
                    else if (componentName == "ScriptComponent") {
                        ecsHub->AddComponent<ScriptComponent>(selectedEntity, ScriptComponent{});
                    }

#if _DEBUG
                    spdlog::info("Added component: %s to Entity %d", componentName.c_str(), selectedEntity);
#endif
                }
                else if (!toggle && hasComponent) {
                    // Remove component
                    if (componentName == "Transform") {
                        ecsHub->RemoveComponent<Transform>(selectedEntity);
                    }
                    else if (componentName == "RigidBody") {
                        ecsHub->RemoveComponent<RigidBody>(selectedEntity);
                        NotifyObservers(new RigidBodyComponentChange(false));
                    }
                    else if (componentName == "Material") {
                        ecsHub->RemoveComponent<Material>(selectedEntity);
                    }
                    else if (componentName == "Button") {
                        ecsHub->RemoveComponent<ButtonComponent>(selectedEntity);
                    }
                    else if (componentName == "FontDataComponent") {
                        ecsHub->RemoveComponent<FontDataComponent>(selectedEntity);
                        NotifyObservers(new FontDeletedMessage(selectedEntity));
                    }
                    else if (componentName == "TileComponent") {
                        ecsHub->RemoveComponent<TileComponent>(selectedEntity);
                    }
                    else if (componentName == "UnitComponent") {
                        ecsHub->RemoveComponent<UnitComponent>(selectedEntity);
                    }
                    else if (componentName == "ScriptComponent") {
                        ecsHub->RemoveComponent<ScriptComponent>(selectedEntity);
                    }


#ifdef _DEBUG
                    spdlog::info("Removed component: %s from Entity %d", componentName.c_str(), selectedEntity);
#endif
                }
            }

            //// Active synchronization if Transform and TileComponent exist
            if (componentName == "TileComponent" && ecsHub->HasComponent<Transform>(selectedEntity) && ecsHub->HasComponent<TileComponent>(selectedEntity)) {
                Transform& transform = ecsHub->GetComponent<Transform>(selectedEntity);
                TileComponent& tile = ecsHub->GetComponent<TileComponent>(selectedEntity);
                tile.position = transform.position;
            }
        }
        //ImGui::End();
    }

    void ImGuiPanels::UpdateAllObjects(const std::map<Entity, std::string>& newAllObjects) {
        allObjects = newAllObjects;
    }
}