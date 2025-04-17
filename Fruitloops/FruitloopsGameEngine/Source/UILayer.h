/******************************************************************************/
/*!
\file    UILayer.h
\project FruitLoops
\author  Zhang TongYan
\date    April 5, 2025
\brief   Declares the UILayer class which manages static user interface
         elements such as HUDs and persistent in-game overlays in the
         FruitLoops engine.

The UILayer handles the visibility, rendering, and state management of all
entities classified with the "UI" prefix. These entities remain consistent
throughout gameplay and are rendered with the batch shader system.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/


#pragma once
#include "Layer.h"

namespace FruitLoops {

    class UILayer : public Layer {
    public:
        UILayer() : Layer("UILayer") {}

        std::vector<Entity>* GetObjectContainer() override {
            return &uiLayerObjects;
        }

        void HideObjects() override {
            for (auto entity : uiLayerObjects) {
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                transformComponent.scale = { 0.0f, 0.0f };
            }
        }

        void ShowObjects() override {
            for (auto entity : uiLayerObjects) {
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                transformComponent.scale = originalScale[entity];
            }
        }

        void UpdateOriginalScale() override {
            for (auto entity : uiLayerObjects) {
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                originalScale[entity] = transformComponent.scale;
            }
        }

        void OnAttach() override {
            isVisible = false;
            layerOpacity = 1.0f;

            std::vector<std::pair<Entity, std::string>> gameObjects = factory->GetAllGameObjects();
            for (const auto& [entity, type] : gameObjects) {
                if (type.substr(0, 2) == "UI") {
                    const Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                    originalScale[entity] = transformComponent.scale;
                    uiLayerObjects.emplace_back(entity);
                }
            }
        }

        void OnDetach() override {
            // Cleanup logic when the UI layer is detached
        }

        void OnUpdate(float dt) override {
            // Update logic for UI elements
            UNUSED_PARAM(dt);
        }

        void OnRender() override {
            // Render UI elements here
            GRAPHICS->BatchShader.Use();
            GRAPHICS->SetUniformMat3(GRAPHICS->BatchShader.GetHandle(), "u_ViewProj", GRAPHICS->camera2d.worldToNDCMatrix);

            Renderer::ResetStats();
            Renderer::BeginBatch();

            for (auto& entity : uiLayerObjects) {
                const Material& material = ecsHub->GetComponent<Material>(entity);
                const Transform& transform = ecsHub->GetComponent<Transform>(entity);

                // Draw textured quads with the batch shader
                Renderer::DrawQuad(
                    { transform.position.x, transform.position.y },
                    { transform.scale.x, transform.scale.y },
                    glm::radians(transform.orientation.x),
                    assetsManager->GetTextures(material.textureName),
                    glm::vec2(material.uvMin.x, material.uvMin.y),
                    glm::vec2(material.uvMax.x, material.uvMax.y),
                    { 1.0f, 1.0f, 1.0f, layerOpacity }
                );

            }

            ;
            Renderer::EndBatch();
            Renderer::Flush();
            GRAPHICS->BatchShader.UnUse();
        }

        std::vector<Entity> uiLayerObjects;
        std::unordered_map<Entity, Vector2D> originalScale;
    };
}