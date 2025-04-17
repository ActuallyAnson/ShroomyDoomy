/******************************************************************************/
/*!
\file    LevelManager.cpp
\project FruitLoops
\author  Zhang TongYan
\date    April 5, 2025
\brief   Implements the LevelManager class, which controls transitions and
         logic between different game levels and menus in the FruitLoops engine.

The LevelManager handles loading, unloading, and transitioning between various
game levels such as the Main Menu, Tutorial, Level 1, and Level 2. It manages
visibility, object activation, music transitions, script systems, and logic
systems during level changes. This includes support for both game runtime
transitions and level editor testing.

The manager ensures a clean switch by properly ending logic proxies, clearing
unnecessary game objects, reinitializing graphic systems, and toggling script
components.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#include "LevelManager.h"
#include "Factory.h"
#include "Core.h"
#include "Graphics.h"
#include "Serialization.h"
#include "LogicProxySystem.h"

namespace FruitLoops {

    LevelManager* levelManager = NULL;

    LevelManager::LevelManager() {
        levelManager = this;
    }

    LevelManager::~LevelManager() {
        levelManager = nullptr;
    }


    // --------------------------------------------------Actual Game Functions--------------------------------------

    void LevelManager::GameStart(Layer* currLayer)
    {
        

        nlohmann::json level1Objects = FruitLoops::LoadGameObjectFromFile("../GameData/Level1.json");
        factory->LoadHelper(level1Objects);

        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "PauseMenuLayer" || layer->GetName() == "MainMenuLayer"
                || layer->GetName() == "StoryLayer" || layer->GetName() == "ChestOverlayLayer") {
                *layer->SetVisibility() = true;
                layer->ShowObjects();
            }
        }

        coreEngine->GetLayerStack().Reinitialize();
        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "GameLayer" || layer->GetName() == "BackgroundLayer"
                || layer->GetName() == "FontLayer" || layer->GetName() == "EnvironmentLayer"
                || layer->GetName() == "PauseMenuLayer" || layer->GetName() == "UILayer") {
                std::vector<Entity>* objects = layer->GetObjectContainer();
                for (Entity entity : *objects) {
                    if (ecsHub->HasComponent<ScriptComponent>(entity)) {
                        auto& scriptComponent = ecsHub->GetComponent<ScriptComponent>(entity);
                        scriptComponent.isActive = true;
                    }
                    if (ecsHub->HasComponent<TileComponent>(entity)) {
                        auto& tileComponent = ecsHub->GetComponent<TileComponent>(entity);
                        tileComponent.isActive = true;
                    }
                }
                *layer->SetVisibility() = true;
                layer->ShowObjects();
            }
        }
        GRAPHICS->ReInitialize();
        LogicProxySystem::End();
        SCRIPTSYSTEM->InitializeBehaviorComponents();
        LogicProxySystem::Initialize();
        // set it back to original state
        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "PauseMenuLayer" || layer->GetName() == "MainMenuLayer"
                || layer->GetName() == "StoryLayer" || layer->GetName() == "ChestOverlayLayer") {
                *layer->SetVisibility() = false;
                layer->HideObjects();
            }
        }
        currLevel = 1;
        *currLayer->SetVisibility() = false;
    }

    void LevelManager::TogglePauseMenu()
    {
        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "PauseMenuLayer") {
                if (layer->IsVisible()) {
                    *layer->SetVisibility() = false;
                    layer->HideObjects();
                    // Set other layer to opaque
                    for (Layer* otherLayer : coreEngine->GetLayerStack()) {
                        if (otherLayer->GetName() != "PauseMenuLayer") {
                            otherLayer->SetOpacity(1.0f);
                        }
                        else {
                            otherLayer->SetOpacity(0.3f);
                        }
                    }
                }
                else {
                    *layer->SetVisibility() = true;
                    layer->ShowObjects();
                    // Set other layer to opaque
                    for (Layer* otherLayer : coreEngine->GetLayerStack()) {
                        if (otherLayer->GetName() != "PauseMenuLayer") {
                            otherLayer->SetOpacity(0.3f);
                        }
                        else {
                            otherLayer->SetOpacity(1.0f);
                        }
                    }
                }
                break;
            }
        }
    }

    void LevelManager::MainMenuToLevel1()
    {
        audioSystem->TransitionMusic("BGM_ShroomyDoomy_Level1_v1", 1.2f);
        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "MainMenuLayer") {
                std::vector<Entity>* objects = layer->GetObjectContainer();
                for (Entity entity : *objects) {
                    if (ecsHub->HasComponent<ScriptComponent>(entity)) {
                        auto& scriptComponent = ecsHub->GetComponent<ScriptComponent>(entity);
                        scriptComponent.isActive = false;
                    }
                }
                *layer->SetVisibility() = false;
            }

            if (layer->GetName() == "StoryLayer") {
                layer->StartStoryboard();
            }
        }
        currLevel = 1;
    }

    void LevelManager::PauseMenuToMainMenu()
    {
        TogglePauseMenu();
        ReturnMainMenu();
    }

    void LevelManager::ReturnMainMenu()
    {
        LogicProxySystem::End();
        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "PauseMenuLayer" || layer->GetName() == "MainMenuLayer"
                || layer->GetName() == "StoryLayer" || layer->GetName() == "ChestOverlayLayer") {
                *layer->SetVisibility() = true;
                layer->ShowObjects();
            }
        }

        coreEngine->GetLayerStack().Reinitialize();
        std::map<Entity, std::string> tempMap = factory->GetAllGameObjectsMap();
        for (auto [entity, type] : tempMap) {
            if (currLevel == 1) {
                if (type.substr(0, 6) == "Level1") {
                    factory->DestroyGameObject(entity);
                }
            }
            else if (currLevel == 0) {
                if (type.substr(0, 8) == "Tutorial") {
                    factory->DestroyGameObject(entity);
                }
            }
            else {
                if (type.substr(0, 6) == "Level2") {
                    factory->DestroyGameObject(entity);
                }
            }
        }
        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "MainMenuLayer") {
                std::vector<Entity>* objects = layer->GetObjectContainer();
                for (Entity entity : *objects) {
                    if (ecsHub->HasComponent<ScriptComponent>(entity)) {
                        auto& scriptComponent = ecsHub->GetComponent<ScriptComponent>(entity);
                        scriptComponent.isActive = true;
                    }
                }
                *layer->SetVisibility() = true;
            }
            if (layer->GetName() == "GameLayer") {
                layer->GetObjectContainer()->clear();
                *layer->SetVisibility() = false;
            }
            if (layer->GetName() == "BackgroundLayer") {
                layer->GetObjectContainer()->clear();
                *layer->SetVisibility() = false;
            }
            if (layer->GetName() == "FontLayer") {

                layer->GetObjectContainer()->clear();
                *layer->SetVisibility() = false;
            }
            
            if (layer->GetName() == "EnvironmentLayer") {
                layer->GetObjectContainer()->clear();
                *layer->SetVisibility() = false;
            }
            
            
        }

        SCRIPTSYSTEM->InitializeBehaviorComponents();
        LogicProxySystem::Initialize();
        audioSystem->TransitionMusic("BGM_ShroomyDoomy_MainMenu_v1", 1.2f);
    }

    void LevelManager::MainMenuToTutorial() {
        // need to modify to load tutorial stuff
        audioSystem->TransitionMusic("BGM_ShroomyDoomy_Level1_v1", 1.2f);
        LogicProxySystem::End();

        nlohmann::json tutorialObjects = FruitLoops::LoadGameObjectFromFile("../GameData/Tutorial.json");
        factory->LoadHelper(tutorialObjects);

        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "PauseMenuLayer" || layer->GetName() == "MainMenuLayer"
                || layer->GetName() == "StoryLayer" || layer->GetName() == "ChestOverlayLayer") {
                *layer->SetVisibility() = true;
                layer->ShowObjects();
            }
        }

        coreEngine->GetLayerStack().Reinitialize();
        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "GameLayer" || layer->GetName() == "BackgroundLayer"
                || layer->GetName() == "FontLayer" || layer->GetName() == "EnvironmentLayer"
                || layer->GetName() == "PauseMenuLayer" || layer->GetName() == "UILayer") {
                std::vector<Entity>* objects = layer->GetObjectContainer();
                for (Entity entity : *objects) {
                    if (ecsHub->HasComponent<ScriptComponent>(entity)) {
                        auto& scriptComponent = ecsHub->GetComponent<ScriptComponent>(entity);
                        scriptComponent.isActive = true;
                    }
                    if (ecsHub->HasComponent<TileComponent>(entity)) {
                        auto& tileComponent = ecsHub->GetComponent<TileComponent>(entity);
                        tileComponent.isActive = true;
                    }
                }
                *layer->SetVisibility() = true;
                layer->ShowObjects();
            }
        }
        GRAPHICS->ReInitialize();
        SCRIPTSYSTEM->InitializeBehaviorComponents();
        LogicProxySystem::Initialize();
        // set it back to original state
        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "PauseMenuLayer" || layer->GetName() == "MainMenuLayer"
                || layer->GetName() == "StoryLayer" || layer->GetName() == "ChestOverlayLayer") {
                *layer->SetVisibility() = false;
                layer->HideObjects();
            }
        }
        currLevel = 0;
    }
    void LevelManager::ReloadCurrLevel()
    {
        std::vector<bool> cache;
        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "PauseMenuLayer" || layer->GetName() == "MainMenuLayer"
                || layer->GetName() == "StoryLayer" || layer->GetName() == "ChestOverlayLayer") {
                cache.push_back(layer->IsVisible());
                *layer->SetVisibility() = true;
                layer->ShowObjects();
            }
        }

        if (currLevel == 1) {
            LogicProxySystem::End();
            factory->ReloadGameObject("../GameData/Level1.json");
            SCRIPTSYSTEM->InitializeBehaviorComponents();
            LogicProxySystem::Initialize();

        }
        else if (currLevel == 2) {
            LogicProxySystem::End();
            factory->ReloadGameObject("../GameData/Level2.json");
            SCRIPTSYSTEM->InitializeBehaviorComponents();
            LogicProxySystem::Initialize();
        }
        else if (currLevel == 0) {
            LogicProxySystem::End();
            factory->ReloadGameObject("../GameData/Tutorial.json");
            SCRIPTSYSTEM->InitializeBehaviorComponents();
            LogicProxySystem::Initialize();
        }
        size_t i = 0;
        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "PauseMenuLayer" || layer->GetName() == "MainMenuLayer"
                || layer->GetName() == "StoryLayer" || layer->GetName() == "ChestOverlayLayer") {

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
    }

    void LevelManager::Level1ToLevel2()
    {
        LogicProxySystem::End();
        for (const auto& [entity, type] : factory->GetAllGameObjects()) {
            if (type.substr(0, 6) == "Level1") {
                factory->DestroyGameObject(entity);
            }
        }


        nlohmann::json tutorialObjects = FruitLoops::LoadGameObjectFromFile("../GameData/Level2.json");
        factory->LoadHelper(tutorialObjects);

        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "PauseMenuLayer" || layer->GetName() == "MainMenuLayer"
                || layer->GetName() == "StoryLayer" || layer->GetName() == "ChestOverlayLayer") {
                *layer->SetVisibility() = true;
                layer->ShowObjects();
            }
        }

        coreEngine->GetLayerStack().Reinitialize();
        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "GameLayer" || layer->GetName() == "BackgroundLayer"
                || layer->GetName() == "FontLayer" || layer->GetName() == "EnvironmentLayer"
                || layer->GetName() == "PauseMenuLayer" || layer->GetName() == "UILayer" ) {
                std::vector<Entity>* objects = layer->GetObjectContainer();
                for (Entity entity : *objects) {
                    if (ecsHub->HasComponent<ScriptComponent>(entity)) {
                        auto& scriptComponent = ecsHub->GetComponent<ScriptComponent>(entity);
                        scriptComponent.isActive = true;
                    }
                    if (ecsHub->HasComponent<TileComponent>(entity)) {
                        auto& tileComponent = ecsHub->GetComponent<TileComponent>(entity);
                        tileComponent.isActive = true;
                    }
                }
                *layer->SetVisibility() = true;
                layer->ShowObjects();
            }
        }
        GRAPHICS->ReInitialize();
        SCRIPTSYSTEM->InitializeBehaviorComponents();
        LogicProxySystem::Initialize();
        // set it back to original state
        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "PauseMenuLayer" || layer->GetName() == "MainMenuLayer"
                || layer->GetName() == "StoryLayer" || layer->GetName() == "ChestOverlayLayer") {
                *layer->SetVisibility() = false;
                layer->HideObjects();
            }
        }
        currLevel = 2;
    }

    // --------------------------------------------------Level editor Functions--------------------------------------
    void LevelManager::Level1ToTutorial()
    {
        LogicProxySystem::End();
        for (const auto& [entity, type] : factory->GetAllGameObjects()) {
            if (type.substr(0, 6) == "Level1") {
                factory->DestroyGameObject(entity);
            }
        }


        nlohmann::json tutorialObjects = FruitLoops::LoadGameObjectFromFile("../GameData/Tutorial.json");
        factory->LoadHelper(tutorialObjects);

        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "PauseMenuLayer" || layer->GetName() == "MainMenuLayer"
                || layer->GetName() == "StoryLayer" || layer->GetName() == "ChestOverlayLayer") {
                *layer->SetVisibility() = true;
                layer->ShowObjects();
            }
        }

        coreEngine->GetLayerStack().Reinitialize();

        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "GameLayer" || layer->GetName() == "BackgroundLayer"
                || layer->GetName() == "FontLayer" || layer->GetName() == "EnvironmentLayer"
                || layer->GetName() == "PauseMenuLayer" || layer->GetName() == "UILayer" ) {
                std::vector<Entity>* objects = layer->GetObjectContainer();
                for (Entity entity : *objects) {
                    if (ecsHub->HasComponent<ScriptComponent>(entity)) {
                        auto& scriptComponent = ecsHub->GetComponent<ScriptComponent>(entity);
                        scriptComponent.isActive = true;
                    }
                    if (ecsHub->HasComponent<TileComponent>(entity)) {
                        auto& tileComponent = ecsHub->GetComponent<TileComponent>(entity);
                        tileComponent.isActive = true;
                    }
                }
                *layer->SetVisibility() = true;
                layer->ShowObjects();
            }
        }
        GRAPHICS->ReInitialize();
        SCRIPTSYSTEM->InitializeBehaviorComponents();
        LogicProxySystem::Initialize();
        // set it back to original state
        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "PauseMenuLayer" || layer->GetName() == "MainMenuLayer"
                || layer->GetName() == "StoryLayer" || layer->GetName() == "ChestOverlayLayer" ) {
                *layer->SetVisibility() = false;
                layer->HideObjects();
            }
        }
        currLevel = 0;
    }


    void LevelManager::Level2ToLevel1()
    {
        LogicProxySystem::End();
        for (const auto& [entity, type] : factory->GetAllGameObjects()) {
            if (type.substr(0, 6) == "Level2") {
                factory->DestroyGameObject(entity);
            }
        }

        nlohmann::json tutorialObjects = FruitLoops::LoadGameObjectFromFile("../GameData/Level1.json");
        factory->LoadHelper(tutorialObjects);
        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "PauseMenuLayer" || layer->GetName() == "MainMenuLayer"
                || layer->GetName() == "StoryLayer" || layer->GetName() == "ChestOverlayLayer" ) {
                *layer->SetVisibility() = true;
                layer->ShowObjects();
            }
        }

        coreEngine->GetLayerStack().Reinitialize();
        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "GameLayer" || layer->GetName() == "BackgroundLayer"
                || layer->GetName() == "FontLayer" || layer->GetName() == "EnvironmentLayer"
                || layer->GetName() == "PauseMenuLayer" || layer->GetName() == "UILayer" ) {
                std::vector<Entity>* objects = layer->GetObjectContainer();
                for (Entity entity : *objects) {
                    if (ecsHub->HasComponent<ScriptComponent>(entity)) {
                        auto& scriptComponent = ecsHub->GetComponent<ScriptComponent>(entity);
                        scriptComponent.isActive = true;
                    }
                    if (ecsHub->HasComponent<TileComponent>(entity)) {
                        auto& tileComponent = ecsHub->GetComponent<TileComponent>(entity);
                        tileComponent.isActive = true;
                    }
                }
                *layer->SetVisibility() = true;
                layer->ShowObjects();
            }
        }
        GRAPHICS->ReInitialize();
        SCRIPTSYSTEM->InitializeBehaviorComponents();
        LogicProxySystem::Initialize();
        // set it back to original state
        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "PauseMenuLayer" || layer->GetName() == "MainMenuLayer"
                || layer->GetName() == "StoryLayer" || layer->GetName() == "ChestOverlayLayer") {
                *layer->SetVisibility() = false;
                layer->HideObjects();
            }
        }
        currLevel = 1;
    }

    void LevelManager::TutorialToLevel1()
    {
        LogicProxySystem::End();
        for (const auto& [entity, type] : factory->GetAllGameObjects()) {
            if (type.substr(0, 8) == "Tutorial") {
                factory->DestroyGameObject(entity);
            }
        }

        nlohmann::json tutorialObjects = FruitLoops::LoadGameObjectFromFile("../GameData/Level1.json");
        factory->LoadHelper(tutorialObjects);
        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "PauseMenuLayer" || layer->GetName() == "MainMenuLayer"
                || layer->GetName() == "StoryLayer" || layer->GetName() == "ChestOverlayLayer") {
                *layer->SetVisibility() = true;
                layer->ShowObjects();
            }
        }

        coreEngine->GetLayerStack().Reinitialize();

        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "GameLayer" || layer->GetName() == "BackgroundLayer"
                || layer->GetName() == "FontLayer" || layer->GetName() == "EnvironmentLayer"
                || layer->GetName() == "PauseMenuLayer" || layer->GetName() == "UILayer") {
                std::vector<Entity>* objects = layer->GetObjectContainer();
                for (Entity entity : *objects) {
                    if (ecsHub->HasComponent<ScriptComponent>(entity)) {
                        auto& scriptComponent = ecsHub->GetComponent<ScriptComponent>(entity);
                        scriptComponent.isActive = true;
                    }
                    if (ecsHub->HasComponent<TileComponent>(entity)) {
                        auto& tileComponent = ecsHub->GetComponent<TileComponent>(entity);
                        tileComponent.isActive = true;
                    }
                }
                *layer->SetVisibility() = true;
                layer->ShowObjects();
            }
        }
        GRAPHICS->ReInitialize();
        SCRIPTSYSTEM->InitializeBehaviorComponents();
        LogicProxySystem::Initialize();
        // set it back to original state
        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "PauseMenuLayer" || layer->GetName() == "MainMenuLayer"
                || layer->GetName() == "StoryLayer" || layer->GetName() == "ChestOverlayLayer") {
                *layer->SetVisibility() = false;
                layer->HideObjects();
            }
        }

        currLevel = 1;
    }

    

}
