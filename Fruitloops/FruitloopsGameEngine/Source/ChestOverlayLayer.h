/******************************************************************************/
/*!
\file    ChestOverlayLayer.h
\project FruitLoops
\author  Foo Ming Sheng
\date    Apr 05, 2025
\brief   Declares the ChestOverlayLayer class, which manages animated visual
         transitions for different chest types (e.g., Cheese, Tomato, Fish,
         Chicken, Bread) in the FruitLoops game engine.

         This layer listens to object spawn/delete events and controls entity
         visibility based on timed animation states (FadeIn → Hold → FadeOut).
         It operates using a trigger-based mechanism and is responsible for
         rendering corresponding chest graphics using a fade-opacity shader.

         It also interacts with ECS and graphics systems for rendering and
         component updates and supports editor-based manipulation via ImGui.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "Layer.h"
#include "Core.h"
#include "LevelManager.h"

namespace FruitLoops {

    class ChestOverlayLayer : public Layer, public Observer {
    public:
        static inline std::string chestOverlayTrigger;

        void SetChestString(std::string chestname) {
            chestOverlayTrigger = chestname;
        }

        ChestOverlayLayer()
            : Layer("ChestOverlayLayer"), Observer("ChestOverlayObserver"),
            currentChestState(ChestState::FadeIn),
            overlayOpacity(0.0f),
            chestTimer(0.0f) 
        {}

        void HandleObjectSpawned(Entity entity) {
            std::string type = factory->GetEntityType(entity);
            if (type.substr(0, 5) == "Story") {
                const Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                originalScale[entity] = transformComponent.scale;
                chestLayerObjects.push_back(entity);
            }
        }

        void HandleObjectDeleted(Entity entity) {
            auto it = std::find(chestLayerObjects.begin(), chestLayerObjects.end(), entity);
            if (it != chestLayerObjects.end()) {
                chestLayerObjects.erase(it);
            }
        }


        void OnAttach() override {
            factory->RegisterObserver(EventID::ObjectSpawned, this);
            factory->RegisterObserver(EventID::ObjectDeleted, this);

            RegisterHandler(EventID::ObjectSpawned, [this](IMessage* msg) {
                if (auto* spawnMsg = dynamic_cast<ObjectSpawnedMessage*>(msg)) {
                    HandleObjectSpawned(spawnMsg->spawnedEntity);
                }
                });

            RegisterHandler(EventID::ObjectDeleted, [this](IMessage* msg) {
                if (auto* deleteMsg = dynamic_cast<ObjectDeletedMessage*>(msg)) {
                    HandleObjectDeleted(deleteMsg->deletedEntity);
                }
                });

            // Gather all overlay entities whose type starts with "ChestOverlay_"
            for (auto& [entity, type] : factory->GetAllGameObjects()) {
                if (type.substr(0, 12) == "ChestOverlay") {
                    std::string chestName = type.substr(13);
                    std::cout << "[ChestOverlay] Mapping chest overlay: " << chestName << " -> " << entity << std::endl;
                    chestTypeToEntity[chestName] = entity;

                    chestLayerObjects.emplace_back(entity);
                    Transform& tf = ecsHub->GetComponent<Transform>(entity);
                    originalScale[entity] = tf.scale;
                    tf.scale = { 0.0f, 0.0f };  // Hide by default
                }
            }
        }

        void ShowChestType(std::string chestTypeName) {
            std::cout << "[ChestOverlay] Attempting to show chest: " << chestTypeName << std::endl;
            auto it = chestTypeToEntity.find(chestTypeName);
            if (it != chestTypeToEntity.end()) {
                Entity entity = it->second;
                std::cout << "[ChestOverlay] Found entity: " << entity << std::endl;

                Transform& tf = ecsHub->GetComponent<Transform>(entity);
                tf.scale = originalScale[entity];  // Restore original size
            }
            else {
                std::cout << "[ChestOverlay] Chest type not found: " << chestTypeName << std::endl;
            }
        }



        void OnDetach() override {
            factory->UnregisterObserver(EventID::ObjectSpawned, this);
            factory->UnregisterObserver(EventID::ObjectDeleted, this);
        }

        void OnUpdate(float dt) override {
            //FL_DEBUG_INFO("CCHESETR IS BEING ACTIVEATED");
            // Being called here.

            // Chester Cheese Time
            if (chestOverlayTrigger == "ChesterCheeseTime"){

                // Enable layer here first.
                isVisible = true;

                std::cout << chestOverlayTrigger << std::endl;
                ShowChestType("ClosedChest");

                chestTimer += dt;
                overlayOpacity = std::min(1.0f, chestTimer / fadeInDuration);

                if (overlayOpacity >= thirdDuration) {
                    //std::cout << "First LAYER OF OPACITY IS TRIGGERED " << std::endl;
                    auto it = chestTypeToEntity.find("ClosedChest");
                    if (it != chestTypeToEntity.end()) {
                        Entity entity = it->second;
                        std::cout << "[ChestOverlay] Found entity: " << entity << std::endl;

                        Transform& tf = ecsHub->GetComponent<Transform>(entity);
                        tf.scale = 0;  // Restore original size
                    }

                    ShowChestType("ChesterHalfCheese");
                }

                if (overlayOpacity >= twothirdDUration) {

                    auto it = chestTypeToEntity.find("ChesterHalfCheese");
                    if (it != chestTypeToEntity.end()) {
                        Entity entity = it->second;
                        std::cout << "[ChestOverlay] Found entity: " << entity << std::endl;

                        Transform& tf = ecsHub->GetComponent<Transform>(entity);
                        tf.scale = 0;  // Restore original size
                    }

                    ShowChestType("ChesterC");
                }

                if (chestTimer >= holdDuration) {

                    auto it = chestTypeToEntity.find("ChesterC");
                    if (it != chestTypeToEntity.end()) {
                        Entity entity = it->second;
                        std::cout << "[ChestOverlay] Found entity: " << entity << std::endl;

                        Transform& tf = ecsHub->GetComponent<Transform>(entity);
                        tf.scale = 0;  // Restore original size
                    }

                    isVisible = false;
                    chestTimer = 0;
                    chestOverlayTrigger = "NIL";
                }
            }


            // Chester Tomato Time
            if (chestOverlayTrigger == "ChesterTomatoTime") {

                // Enable layer here first.
                isVisible = true;

                std::cout << chestOverlayTrigger << std::endl;
                ShowChestType("ClosedChest");

                chestTimer += dt;
                overlayOpacity = std::min(1.0f, chestTimer / fadeInDuration);

                if (overlayOpacity >= thirdDuration) {
                    //std::cout << "First LAYER OF OPACITY IS TRIGGERED " << std::endl;
                    auto it = chestTypeToEntity.find("ClosedChest");
                    if (it != chestTypeToEntity.end()) {
                        Entity entity = it->second;
                        std::cout << "[ChestOverlay] Found entity: " << entity << std::endl;

                        Transform& tf = ecsHub->GetComponent<Transform>(entity);
                        tf.scale = 0;  // Restore original size
                    }

                    ShowChestType("ChesterHalfTomato");
                }

                if (overlayOpacity >= twothirdDUration) {

                    auto it = chestTypeToEntity.find("ChesterHalfTomato");
                    if (it != chestTypeToEntity.end()) {
                        Entity entity = it->second;
                        std::cout << "[ChestOverlay] Found entity: " << entity << std::endl;

                        Transform& tf = ecsHub->GetComponent<Transform>(entity);
                        tf.scale = 0;  // Restore original size
                    }

                    ShowChestType("ChesterT");
                }

                if (chestTimer >= holdDuration) {

                    auto it = chestTypeToEntity.find("ChesterT");
                    if (it != chestTypeToEntity.end()) {
                        Entity entity = it->second;
                        std::cout << "[ChestOverlay] Found entity: " << entity << std::endl;

                        Transform& tf = ecsHub->GetComponent<Transform>(entity);
                        tf.scale = 0;  // Restore original size
                    }

                    isVisible = false;
                    chestTimer = 0;
                    chestOverlayTrigger = "NIL";
                }
            }

            // Chester Fish Time
            if (chestOverlayTrigger == "ChesterFishTime") {

                // Enable layer here first.
                isVisible = true;

                std::cout << chestOverlayTrigger << std::endl;
                ShowChestType("ClosedChest");

                chestTimer += dt;
                overlayOpacity = std::min(1.0f, chestTimer / fadeInDuration);

                if (overlayOpacity >= thirdDuration) {
                    //std::cout << "First LAYER OF OPACITY IS TRIGGERED " << std::endl;
                    auto it = chestTypeToEntity.find("ClosedChest");
                    if (it != chestTypeToEntity.end()) {
                        Entity entity = it->second;
                        std::cout << "[ChestOverlay] Found entity: " << entity << std::endl;

                        Transform& tf = ecsHub->GetComponent<Transform>(entity);
                        tf.scale = 0;  // Restore original size
                    }

                    ShowChestType("ChesterHalfFish");
                }

                if (overlayOpacity >= twothirdDUration) {

                    auto it = chestTypeToEntity.find("ChesterHalfFish");
                    if (it != chestTypeToEntity.end()) {
                        Entity entity = it->second;
                        std::cout << "[ChestOverlay] Found entity: " << entity << std::endl;

                        Transform& tf = ecsHub->GetComponent<Transform>(entity);
                        tf.scale = 0;  // Restore original size
                    }

                    ShowChestType("ChesterF");
                }

                if (chestTimer >= holdDuration) {

                    auto it = chestTypeToEntity.find("ChesterF");
                    if (it != chestTypeToEntity.end()) {
                        Entity entity = it->second;
                        std::cout << "[ChestOverlay] Found entity: " << entity << std::endl;

                        Transform& tf = ecsHub->GetComponent<Transform>(entity);
                        tf.scale = 0;  // Restore original size
                    }

                    isVisible = false;
                    chestTimer = 0;
                    chestOverlayTrigger = "NIL";
                }
            }

            // Chester Chicken Time
            if (chestOverlayTrigger == "ChesterChickenTime") {

                // Enable layer here first.
                isVisible = true;

                std::cout << chestOverlayTrigger << std::endl;
                ShowChestType("ClosedChest");

                chestTimer += dt;
                overlayOpacity = std::min(1.0f, chestTimer / fadeInDuration);

                if (overlayOpacity >= thirdDuration) {
                    //std::cout << "First LAYER OF OPACITY IS TRIGGERED " << std::endl;
                    auto it = chestTypeToEntity.find("ClosedChest");
                    if (it != chestTypeToEntity.end()) {
                        Entity entity = it->second;
                        std::cout << "[ChestOverlay] Found entity: " << entity << std::endl;

                        Transform& tf = ecsHub->GetComponent<Transform>(entity);
                        tf.scale = 0;  // Restore original size
                    }

                    ShowChestType("ChesterHalfChicken");
                }

                if (overlayOpacity >= twothirdDUration) {

                    auto it = chestTypeToEntity.find("ChesterHalfChicken");
                    if (it != chestTypeToEntity.end()) {
                        Entity entity = it->second;
                        std::cout << "[ChestOverlay] Found entity: " << entity << std::endl;

                        Transform& tf = ecsHub->GetComponent<Transform>(entity);
                        tf.scale = 0;  // Restore original size
                    }

                    ShowChestType("ChesterY");
                }

                if (chestTimer >= holdDuration) {

                    auto it = chestTypeToEntity.find("ChesterY");
                    if (it != chestTypeToEntity.end()) {
                        Entity entity = it->second;
                        std::cout << "[ChestOverlay] Found entity: " << entity << std::endl;

                        Transform& tf = ecsHub->GetComponent<Transform>(entity);
                        tf.scale = 0;  // Restore original size
                    }

                    isVisible = false;
                    chestTimer = 0;
                    chestOverlayTrigger = "NIL";
                }
            }

            // Chester Bread Time
            if (chestOverlayTrigger == "ChesterBreadTime") {

                // Enable layer here first.
                isVisible = true;

                std::cout << chestOverlayTrigger << std::endl;
                ShowChestType("ClosedChest");

                chestTimer += dt;
                overlayOpacity = std::min(1.0f, chestTimer / fadeInDuration);

                if (overlayOpacity >= thirdDuration) {
                    //std::cout << "First LAYER OF OPACITY IS TRIGGERED " << std::endl;
                    auto it = chestTypeToEntity.find("ClosedChest");
                    if (it != chestTypeToEntity.end()) {
                        Entity entity = it->second;
                        std::cout << "[ChestOverlay] Found entity: " << entity << std::endl;

                        Transform& tf = ecsHub->GetComponent<Transform>(entity);
                        tf.scale = 0;  // Restore original size
                    }

                    ShowChestType("ChesterHalfBread");
                }

                if (overlayOpacity >= twothirdDUration) {

                    auto it = chestTypeToEntity.find("ChesterHalfBread");
                    if (it != chestTypeToEntity.end()) {
                        Entity entity = it->second;
                        std::cout << "[ChestOverlay] Found entity: " << entity << std::endl;

                        Transform& tf = ecsHub->GetComponent<Transform>(entity);
                        tf.scale = 0;  // Restore original size
                    }

                    ShowChestType("ChesterB");
                }

                if (chestTimer >= holdDuration) {

                    auto it = chestTypeToEntity.find("ChesterB");
                    if (it != chestTypeToEntity.end()) {
                        Entity entity = it->second;
                        std::cout << "[ChestOverlay] Found entity: " << entity << std::endl;

                        Transform& tf = ecsHub->GetComponent<Transform>(entity);
                        tf.scale = 0;  // Restore original size
                    }

                    isVisible = false;
                    chestTimer = 0;
                    chestOverlayTrigger = "NIL";
                }
            }
        }



        void OnRender() override {
            GRAPHICS->BatchShader.Use();
            GRAPHICS->SetUniformMat3(GRAPHICS->BatchShader.GetHandle(), "u_ViewProj", GRAPHICS->camera2d.worldToNDCMatrix);
            Renderer::ResetStats();
            Renderer::BeginBatch();

            for (Entity entity : chestLayerObjects) {
                RenderEntityWithOpacity(entity, overlayOpacity);
            }

            Renderer::EndBatch();
            Renderer::Flush();
            GRAPHICS->BatchShader.UnUse();
        }

        std::vector<Entity>* GetObjectContainer() override {
            return &chestLayerObjects;
        }

        void HideObjects() override {
            for (auto entity : chestLayerObjects) {
                //if (entity == _spacebarSkipEntity) continue; // Do NOT hide it
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                transformComponent.scale = { 0.0f, 0.0f };
            }
        }

        void ShowObjects() override {
            for (auto entity : chestLayerObjects) {
                //if (entity == _spacebarSkipEntity) continue; // Do NOT modify it
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                transformComponent.scale = originalScale[entity];
            }
        }

        void HideObject(size_t index) {
            if (index < chestLayerObjects.size()) {
                Entity entity = chestLayerObjects[index];
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                transformComponent.scale = { 0.0f, 0.0f };
            }
        }

        // Restore a storyboard element�s scale.
        void ShowObject(size_t index) {
            if (index < chestLayerObjects.size()) {
                Entity entity = chestLayerObjects[index];
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                transformComponent.scale = originalScale[entity];
            }
        }

        void UpdateOriginalScale() override {
            for (auto entity : chestLayerObjects) {
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                originalScale[entity] = transformComponent.scale;
            }
        }

    private:
        enum class ChestState {
            FadeIn,
            Hold,
            FadeOut,
            Done
        };

        void RenderEntityWithOpacity(Entity entity, float opacity) {
            const Material& material = ecsHub->GetComponent<Material>(entity);
            const Transform& transform = ecsHub->GetComponent<Transform>(entity);
            Renderer::DrawQuad(
                { transform.position.x, transform.position.y },
                { transform.scale.x, transform.scale.y },
                glm::radians(transform.orientation.x),
                assetsManager->GetTextures(material.textureName),
                { material.uvMin.x, material.uvMin.y },
                { material.uvMax.x, material.uvMax.y },
                { 1.0f, 1.0f, 1.0f, opacity }
            );
        }

        ChestState currentChestState;
        float chestTimer = 0.0f;
        float overlayOpacity = 0.0f;
        //Entity currentChestEntity = Entity(0);

        const float fadeInDuration = 1.0f;
        const float holdDuration = 2.0f;
        const float fadeOutDuration = 0.75f;
        const float thirdDuration = 0.33f;
        const float twothirdDUration = 0.66f;

        //std::unordered_map<std::string, Entity> chestTypeToOverlayEntity;
        std::unordered_map<Entity, Vector2D> originalScale;
        std::vector<Entity> chestLayerObjects;
        bool objectsShown;

        std::unordered_map<std::string, Entity> chestTypeToEntity;
    };

}
