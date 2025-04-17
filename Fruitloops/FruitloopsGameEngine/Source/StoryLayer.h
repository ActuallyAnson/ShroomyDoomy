/******************************************************************************/
/*!
\file    StoryLayer.h
\project FruitLoops
\author  Zhang TongYan
\date    April 5, 2025
\brief   Declares the StoryLayer class which manages and animates the sequence
         of storyboard elements in the FruitLoops engine.

The StoryLayer supports fade-in, hold, crossfade, and fade-out transitions
between storyboard screens. It observes entity spawn/delete events and renders
both narrative elements and fixed UI overlays such as the skip prompt.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

#pragma once
#include "Layer.h"
#include "Core.h"
#include "LevelManager.h"

namespace FruitLoops {

    class StoryLayer : public Layer, public Observer {
    public:
        StoryLayer()
            : Layer("StoryLayer"), Observer("StoryLayerObserver"),
            storyboardState(StoryboardState::FadeIn),
            storyboardTimer(0.0f),
            currentStoryboardIndex(0)
        {
            // Start fully transparent for fade-in.
            layerOpacity = 0.0f;
        }

        // Call this method to (re)start the entire storyboard sequence.
        void StartStoryboard() override {
            currentStoryboardIndex = 0;
            storyboardState = StoryboardState::FadeIn;
            storyboardTimer = 0.0f;
            layerOpacity = 0.0f;
            isVisible = true;

            // Hide all objects and then show the first.
            for (size_t i = 0; i < StoryLayerObjects.size(); ++i) {
                HideObject(i);
            }
            if (!StoryLayerObjects.empty()) {
                ShowObject(0);
            }
        }

        void HandleObjectSpawned(Entity entity) {
            std::string type = factory->GetEntityType(entity);
            if (type.substr(0, 5) == "Story") {
                const Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                originalScale[entity] = transformComponent.scale;
                StoryLayerObjects.push_back(entity);
            }    
        }

        void HandleObjectDeleted(Entity entity) {
            auto it = std::find(StoryLayerObjects.begin(), StoryLayerObjects.end(), entity);
            if (it != StoryLayerObjects.end()) {
                StoryLayerObjects.erase(it);
            }
        }

        void OnAttach() override {
            factory->RegisterObserver(EventID::ObjectSpawned, this);
            factory->RegisterObserver(EventID::ObjectDeleted, this);

            // Register handlers for spawn and delete events.
            RegisterHandler(EventID::ObjectSpawned, [this](IMessage* message) {
                if (auto* spawnMessage = dynamic_cast<ObjectSpawnedMessage*>(message)) {
                    HandleObjectSpawned(spawnMessage->spawnedEntity);
                }
                });

            RegisterHandler(EventID::ObjectDeleted, [this](IMessage* message) {
                if (auto* deleteMessage = dynamic_cast<ObjectDeletedMessage*>(message)) {
                    HandleObjectDeleted(deleteMessage->deletedEntity);
                }
                });

            // Initialize the StoryLayer objects.
            std::vector<std::pair<Entity, std::string>> gameObjects = factory->GetAllGameObjects();
             _spacebarSkipEntity = factory->GetEntityByName("spacebarSkip")[0];
            for (const auto& [entity, type] : gameObjects) {

                if (type.substr(0, 5) == "Story") {
                    if (type.substr(6, 2) == "UI") {
                        UIObjects.emplace_back(entity);
                        continue;
                    }
                    const Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                    StoryLayerObjects.emplace_back(entity);
                    originalScale[entity] = transformComponent.scale;
                    originalPosition[entity] = transformComponent.position;
                }
            }
        }


        void OnDetach() override {
            factory->UnregisterObserver(EventID::ObjectSpawned, this);
            factory->UnregisterObserver(EventID::ObjectDeleted, this);
        }

        void OnUpdate(float dt) override {
            if (!isVisible) {
                HideObjects();
                objectsShown = false;
                return;
            }
            else if (isVisible && !objectsShown) {
                ShowObjects();
                objectsShown = true;
            }

            // Ensure Story_spacebarSkip is always visible and clamped to bottom
            if (_spacebarSkipEntity != Entity(0)) {
                Transform& skipTransform = ecsHub->GetComponent<Transform>(_spacebarSkipEntity);

                // Keep scale fixed
                skipTransform.scale = originalScale[_spacebarSkipEntity];

                // Lock to bottom of the screen
                skipTransform.position = originalPosition[_spacebarSkipEntity]; // Adjust based on screen coordinates
            }

            // ---------------------------------------
            // 0. Check if user pressed SPACE -> skip
            // ---------------------------------------
            if (inputSystem->IsKeyPressed(GLFW_KEY_SPACE)) {
                storyboardState = StoryboardState::Done;
                levelManager->GameStart(this);
                return;
            }

            if (currentStoryboardIndex >= StoryLayerObjects.size()) {
                isVisible = false;
                return;
            }

            switch (storyboardState) {
            case StoryboardState::FadeIn:
                storyboardTimer += dt;
                layerOpacity = std::min(1.0f, storyboardTimer / fadeInDuration);
                if (storyboardTimer >= fadeInDuration) {
                    storyboardState = StoryboardState::Hold;
                    storyboardTimer = 0.0f;
                }
                break;

            case StoryboardState::Hold:
                storyboardTimer += dt;
                layerOpacity = 1.0f;
                if (storyboardTimer >= holdDuration) {
                    if (currentStoryboardIndex < StoryLayerObjects.size() - 1) {
                        ShowObject(currentStoryboardIndex + 1);
                        storyboardState = StoryboardState::CrossFade;
                        storyboardTimer = 0.0f;
                    }
                    else {
                        storyboardState = StoryboardState::FadeOut;
                        storyboardTimer = 0.0f;
                    }
                }
                break;

            case StoryboardState::CrossFade:
                storyboardTimer += dt;
                if (storyboardTimer >= crossFadeDuration) {
                    HideObject(currentStoryboardIndex);
                    currentStoryboardIndex++;
                    storyboardState = StoryboardState::Hold;
                    storyboardTimer = 0.0f;
                    layerOpacity = 1.0f;
                }
                break;

            case StoryboardState::FadeOut:
                storyboardTimer += dt;
                layerOpacity = std::max(0.0f, 1.0f - (storyboardTimer / fadeOutDuration));
                if (storyboardTimer >= fadeOutDuration) {
                    storyboardState = StoryboardState::Done;
                    HideObject(currentStoryboardIndex);
                    levelManager->GameStart(this);
                }
                break;

            case StoryboardState::Done:
                break;
            }
        }

        void OnRender() override {
            if (!isVisible)
                return;

            GRAPHICS->BatchShader.Use();
            GRAPHICS->SetUniformMat3(GRAPHICS->BatchShader.GetHandle(),
                "u_ViewProj", GRAPHICS->camera2d.worldToNDCMatrix);

            Renderer::ResetStats();
            Renderer::BeginBatch();

            // Render Storyboard Elements
            if (storyboardState == StoryboardState::CrossFade && currentStoryboardIndex < StoryLayerObjects.size() - 1) {
                float t = std::min(1.0f, storyboardTimer / crossFadeDuration);
                float currentAlpha = 1.0f - t;
                float nextAlpha = t;

                Entity currentEntity = StoryLayerObjects[currentStoryboardIndex];
                RenderEntityWithOpacity(currentEntity, currentAlpha);

                Entity nextEntity = StoryLayerObjects[currentStoryboardIndex + 1];
                RenderEntityWithOpacity(nextEntity, nextAlpha);
            }
            else if (currentStoryboardIndex < StoryLayerObjects.size()) {
                Entity currentEntity = StoryLayerObjects[currentStoryboardIndex];
                RenderEntityWithOpacity(currentEntity, layerOpacity);
            }

            // **Ensure Story_spacebarSkip is ALWAYS rendered at full opacity**
            if (_spacebarSkipEntity != Entity(0)) {
                RenderEntityWithOpacity(_spacebarSkipEntity, 1.0f);
            }

            Renderer::EndBatch();
            Renderer::Flush();
            GRAPHICS->BatchShader.UnUse();
        }

        std::vector<Entity>* GetObjectContainer() override {
            allObjects.clear();
            allObjects.insert(allObjects.end(), StoryLayerObjects.begin(), StoryLayerObjects.end());
            allObjects.insert(allObjects.end(), UIObjects.begin(), UIObjects.end());
            return &allObjects;
        }

        void HideObjects() override {
            for (auto entity : StoryLayerObjects) {
                if (entity == _spacebarSkipEntity) continue; // Do NOT hide it
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                transformComponent.scale = { 0.0f, 0.0f };
            }
        }

        void ShowObjects() override {
            for (auto entity : StoryLayerObjects) {
                if (entity == _spacebarSkipEntity) continue; // Do NOT modify it
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                transformComponent.scale = originalScale[entity];
            }
        }

    private:
        // Helper function to render an entity with a given opacity.
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

        // Hide a specific storyboard element (by setting its scale to zero).
        void HideObject(size_t index) {
            if (index < StoryLayerObjects.size()) {
                Entity entity = StoryLayerObjects[index];
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                transformComponent.scale = { 0.0f, 0.0f };
            }
        }

        // Restore a storyboard element�s scale.
        void ShowObject(size_t index) {
            if (index < StoryLayerObjects.size()) {
                Entity entity = StoryLayerObjects[index];
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                transformComponent.scale = originalScale[entity];
            }
        }

        void UpdateOriginalScale() override {
            for (auto entity : StoryLayerObjects) {
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                originalScale[entity] = transformComponent.scale;
            }
        }

    private:
        // Define the states for the storyboard animation.
        enum class StoryboardState {
            FadeIn,      // Fade in the current element.
            Hold,        // Hold the current element at full opacity.
            CrossFade,   // Crossfade from current to next element.
            FadeOut,     // (Optional) Fade out the final element.
            Done         // Animation complete.
        };

        StoryboardState storyboardState;
        float storyboardTimer;           // Timer used for the current state.
        size_t currentStoryboardIndex;   // Index of the currently active storyboard element.
        Entity _spacebarSkipEntity;  // Holds the reference to Story_spacebarSkip

        // Durations (in seconds) for each phase.
        const float fadeInDuration = 1.0f;
        const float holdDuration = 1.0f;
        const float crossFadeDuration = 1.0f;
        const float fadeOutDuration = 0.5f;

    public:
        std::vector<Entity> StoryLayerObjects;
        std::vector<Entity> UIObjects;
        std::vector<Entity> allObjects;
        std::unordered_map<Entity, Vector2D> originalScale;
        bool objectsShown;
        std::unordered_map<Entity, Vector2D> originalPosition;
        // Note: 'layerOpacity' is used for non-crossfade states.
    };
}
