/**
 * @file Core.cpp
 * @brief Implements the core functionality of the FruitloopEngine.
 *
 * This source file contains the definitions for the methods of the
 * `FruitloopEngine` class, which serves as the main manager for
 * the game engine. It initializes subsystems, manages the main game
 * loop, and provides a mechanism to shut down the engine cleanly.
 *
 * Key functionalities include:
 * - Engine initialization and subsystem setup (graphics, input, physics).
 * - The main game loop that handles events, updates, and rendering.
 * - Logging system performance and frame timings.
 * - Gracefully shutting down the engine.
 *
 * @note The engine relies on the ECS (Entity-Component-System) architecture
 *       for managing game objects and their behaviors.
 *
 * @author Anson Teng
 * @date 4/10/2024
 */

#include "pch.h"
#include "Core.h"
#include "Graphics.h"
#include "ECSHub.h"
#include "Factory.h"
#include "Input.h"
#include "Physics.h"
#include "Log.h"
#include "Audio.h"  
#include "Serialization.h"
#include "LogicProxySystem.h"
#include "ScriptSystem.h"
#include "PauseManager.h"

 // --- IMGUI FUNCTIONS ---
#include "ImguiPanelDocking.h"
#include "ImguiCollision.h" 
#include "ImGuiPanels.h"

namespace FruitLoops {

    FruitloopEngine* coreEngine;

    /**
    * @brief Constructs the FruitloopEngine instance.
    *
    * Initializes the engine state and sets up logging.
    */
    FruitloopEngine::FruitloopEngine() {
#ifdef _DEBUG
        spdlog::info("Initializing fruits for fruit loops engine...");
#endif
        gameActive = true;
        coreEngine = this;
    }

    /**
    * @brief Destructs the FruitloopEngine instance.
    *
    * Cleans up resources and shuts down the engine.
    */
    FruitloopEngine::~FruitloopEngine() {
        coreEngine = nullptr;
    }

    /**
    * @brief Initializes the engine and its subsystems.
    *
    * This method sets up the ECSHub and registers components and systems.
    */
    void FruitloopEngine::Initialize() {

        if (factory->LoadConfigData("../GameData/config.json")) {
            spdlog::info("Loaded config data from JSON successfully.");
        }
        else {
            spdlog::warn("No config data loaded from JSON file.");
        }

        window = std::make_unique<WindowsWindow>(WindowProps(ConfigData::title, ConfigData::width, ConfigData::height));
        spdlog::info("Testing all Engine Functions...");

#ifdef _DEBUG
        // ImGui initialization
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        ImGui::StyleColorsDark();

        // Initialize ImGui for GLFW and OpenGL3
        ImGui_ImplGlfw_InitForOpenGL(window->GetWindow(), true);
        ImGui_ImplOpenGL3_Init("#version 130");
        // Initialize the framebuffer
        frameBuffer = std::make_unique<FrameBuffer>(static_cast<float>(ConfigData::width), static_cast<float>(ConfigData::height));
#endif

        ecsHub->Init();

        Registration();


        // Init of input system shadows the imgui inputs, need to implement layer structure
        // Load all game objects from the JSON file using the factory
        if (!factory->LoadGameObject("../GameData/Level1.json").empty()) {
            spdlog::info("Loaded game objects from JSON successfully.");
        }
        else {
            spdlog::warn("No game objects loaded from JSON file.");
        }

        spdlog::info("Initializing Assets Manager.");
        spdlog::info("Initializing Graphics System.");
        spdlog::info("Initializing Script System.");
        spdlog::info("Initializing Input System.");
        spdlog::info("Initializing Logic System.");
        spdlog::info("Initializing Physics System.");
        spdlog::info("Initializing Sound System.");

        assetsManager->Init();

        coreGraphicsSystem->Initialize();

        coreScriptSystem->Initialize();

        coreInputSystem->Initialize();

        InitializeLayers();

        LogicProxySystem::Initialize();

        corePhysicsSystem->Initialize();

        coreSoundSystem->Initialize();

        

#ifdef _DEBUG
        spdlog::info("Initializing ImGui System.");
        coreImGuiSystem->Initialize();
#endif

        Signature graphicsSignature;
        Signature inputSignature;

        ecsHub->SetSystemSignature<Graphics>(graphicsSignature);
        ecsHub->SetSystemSignature<InputSystem>(inputSignature);

        // Initialize layers after systems are registered
#ifdef _DEBUG
        spdlog::info("Initializing Layers.");
#endif

    }

    /**
    * @brief The main game loop that runs while the game is active.
    *
    * This method handles events, updates systems, and renders graphics each frame.
    */
    void FruitloopEngine::GameLoop() {

        LogGameLoop("Starting the Game Loop");
        // Initialize time variables.
        auto previousTime = std::chrono::high_resolution_clock::now();
        double accumulatedTime = 0.0;

        Profiler profiler;  // Initialize Profiler

        while (gameActive) {

            GLFWwindow* glfwWindow = window->GetWindow();  // Get the GLFW window pointer

            PauseManager::HandleMinimize();  // Pass window pointer
            PauseManager::HandleFocus(glfwWindow);

            // Handle full-screen toggle input
            InputSystem::HandleFullScreenInput();

            // Measure actual elapsed time.
            auto currentTime = std::chrono::high_resolution_clock::now();
            double deltaTime = std::chrono::duration<double>(currentTime - previousTime).count();
            previousTime = currentTime;
            accumulatedTime += deltaTime;

            glfwPollEvents();

            if (glfwWindowShouldClose(window->GetWindow())) {
                break;
            }


            // Begin frame
            profiler.StartFrame();
            auto frameStartTime = std::chrono::high_resolution_clock::now();

            // Update systems that require a fixed time step.
            profiler.Start("Input");
            coreInputSystem->Update(static_cast<float>(deltaTime));
            profiler.Stop("Input");

            profiler.Start("Logic");
            LogicProxySystem::Update(static_cast<float>(deltaTime));
            profiler.Stop("Logic");

            // Logic system might end game so perform early checks
            if (!gameActive) {
                break;
            }

            profiler.Start("Audio");
            coreSoundSystem->Update(static_cast<float>(deltaTime));
            profiler.Stop("Audio");

            profiler.Start("Physics");
            corePhysicsSystem->Update(static_cast<float>(deltaTime));
            profiler.Stop("Physics");

            profiler.Start("Graphics");
            coreGraphicsSystem->Update(static_cast<float>(deltaTime));
            profiler.Stop("Graphics");
            // You can add additional fixed-step systems here.

            #ifdef _DEBUG
            profiler.Start("Imgui");
            coreImGuiSystem->Update(static_cast<float>(deltaTime));
            profiler.Stop("Imgui");
            #else
            glClearColor(0.1725f, 0.1725f, 0.1725f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            InputSystem::framebufferWindowSize.x = static_cast<float>(InputSystem::windowWidth);
            InputSystem::framebufferWindowSize.y = static_cast<float>(InputSystem::windowHeight);

            // Proceed with event handling and rendering
            layerStack.handleEvents();
            layerStack.updateLayers(static_cast<float>(deltaTime));
            layerStack.renderLayers();
            #endif

            if (tutorialToLevel1) {
                levelManager->TutorialToLevel1();
                tutorialToLevel1 = false;
            }

            if (level1ToLevel2) {
                levelManager->Level1ToLevel2();
                level1ToLevel2 = false;
            }

            if (restartGame) {
                levelManager->ReloadCurrLevel();
                restartGame = false;
            }

            if (level1ToTutorial) {
                levelManager->Level1ToTutorial();
                level1ToTutorial = false;
            }

            if (mainMenuToLevel1) {
                levelManager->MainMenuToLevel1();
                mainMenuToLevel1 = false;
            }

            if (pauseToMainMenu) {
                levelManager->PauseMenuToMainMenu();
                pauseToMainMenu = false;
            }

            if (returnMainMenu) {
                levelManager->ReturnMainMenu();
                returnMainMenu = false;
            }

            if (menuToTutorial) {
                levelManager->MainMenuToTutorial();
                menuToTutorial = false;
            }

            if (level2ToLevel1) {
                levelManager->Level2ToLevel1();
                level2ToLevel1 = false;
            }
            // End frame timing
            auto frameEndTime = std::chrono::high_resolution_clock::now();
            profiler.frameDuration = frameEndTime - frameStartTime;



            #ifdef _DEBUG
            ImGui::Render();  // Ends the ImGui frame and prepares draw data
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            #endif
            // Rendering ImGui frame

            window->SwapBuffers();
        }

        LogGameLoop("Exiting Game Loop");

    }

    /**
    * @brief Shuts down the engine.
    *
    * This method sets the game state to inactive, stopping the game loop.
    */
    void FruitloopEngine::ShutDown() {
        gameActive = false;
#ifdef _DEBUG
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
#endif


    }

    /******************************************************************************/
    /*!
    \brief  Registers all necessary components and systems to the ECSHub.
    \details  Initializes and registers components like Transform, RigidBody, and others.
            It also sets up the systems that will manage these components throughout the game.
    */
    /******************************************************************************/
    void FruitloopEngine::Registration() {
        ecsHub->RegisterComponent<Transform>();
        ecsHub->RegisterComponent<RigidBody>();
        ecsHub->RegisterComponent<CameraComponent>();
        ecsHub->RegisterComponent<Material>();
        ecsHub->RegisterComponent<FontDataComponent>();
        ecsHub->RegisterComponent<PhysicsObject>();
        ecsHub->RegisterComponent<TileComponent>();
        ecsHub->RegisterComponent<UnitComponent>();
        ecsHub->RegisterComponent<ScriptComponent>();
        ecsHub->RegisterComponent<ButtonComponent>();
        ecsHub->RegisterComponent<AnimationComponent>();

        coreGraphicsSystem = ecsHub->RegisterSystem<Graphics>();
        coreInputSystem = ecsHub->RegisterSystem<InputSystem>();
        coreScriptSystem = ecsHub->RegisterSystem<ScriptSystem>();
        corePhysicsSystem = ecsHub->RegisterSystem<PhysicsObject>();
        coreSoundSystem = ecsHub->RegisterSystem<AudioSystem>();
#ifdef _DEBUG
        coreImGuiSystem = ecsHub->RegisterSystem<ImGuiPanels>();
#endif
    }

    /******************************************************************************/
    /*!
    \brief  Adds a new layer to the game layer stack.
    \param  layer  Pointer to the layer to be added to the stack.
    \details  This method pushes a new layer onto the layer stack and triggers the OnAttach event.
    */
    /******************************************************************************/
    void FruitloopEngine::PushLayer(Layer* layer)
    {
        layerStack.PushLayer(layer);
        layer->OnAttach();
    }

    /******************************************************************************/
    /*!
    \brief  Adds a new overlay layer to the game layer stack.
    \param  layer  Pointer to the overlay layer to be added to the stack.
    \details  This method pushes a new overlay on top of all other layers to ensure it is rendered last.
    */
    /******************************************************************************/
    void FruitloopEngine::PushOverlay(Layer* layer)
    {
        layerStack.PushOverlay(layer);
        layer->OnAttach();
    }

    /******************************************************************************/
    /*!
    \brief  Initializes all layers and overlays for the game.
    \details  This method sets up all the necessary layers and overlays that make up the game's UI
            and game elements. It ensures that layers are pushed in the correct order.
    */
    /******************************************************************************/
    void FruitloopEngine::InitializeLayers() {
        // Create and add layers
        layerStack = LayerStack();
        // Ensure layers are created and pushed in the correct order
        PushLayer(new UtilsLayer());
        PushLayer(new BackgroundLayer());
        PushLayer(new EnvironmentLayer());
        PushLayer(new GameLayer());
        PushLayer(new UILayer());
        PushLayer(new FontLayer());
        PushLayer(new MainMenuLayer());
        PushLayer(new StoryLayer());
        PushLayer(new EndMenuLayer());
        PushLayer(new PauseMenuLayer());
        PushLayer(new FPSLayer());
        PushLayer(new ChestOverlayLayer());
    }

    void FruitloopEngine::RestartGame() {  

        // C# game logic end
        LogicProxySystem::End();

        // C# game logic start
        SCRIPTSYSTEM->InitializeBehaviorComponents();
        LogicProxySystem::Initialize();
    }
}