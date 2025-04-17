
/******************************************************************************/
/*!
\file    UtilsLayer.h
\project FruitLoops
\author  Zhang TongYan
\date    April 5, 2025
\brief   Declares the UtilsLayer class responsible for managing invisible or
         non-rendered utility entities in the FruitLoops engine.

The UtilsLayer is used to hold game entities prefixed with "Utils" that serve
background or structural purposes without being drawn on-screen. These may
include debugging helpers, invisible triggers, logical placeholders, or
non-interactive interface elements that support core engine functionalities.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

#pragma once
#include "Layer.h"

namespace FruitLoops {

    class UtilsLayer : public Layer {
    public:
        /**
         * @brief Default constructor. Initializes Utils Layer with a name and observer ID.
         */

        UtilsLayer() : Layer("UtilsLayer") {}


        std::vector<Entity>* GetObjectContainer() override {
            return &utilsLayerObjects;
        }

        /**
         * @brief Registers the observers and handles initial game objects setup when the layer is attached.
         *
         * Registers this layer to observe events like object spawning and deletion. Also pre-populates the
         * map of game objects for the player, enemies, and buttons.
         */

        void OnAttach() override {
            isVisible = false;
            layerOpacity = 1.0f;

            std::vector<std::pair<Entity, std::string>> gameObjects = factory->GetAllGameObjects();
            for (const auto& [entity, type] : gameObjects) {
                if (type.substr(0, 5) == "Utils") {
                    utilsLayerObjects.emplace_back(entity);
                }
            }
        }

        /**
         * @brief Unregisters observers and cleans up when the layer is detached.
         *
         * Unsubscribes from the events related to object spawn and deletion.
         */
        void OnDetach() override {

        }

        /**
        * @brief Updates the game objects in the layer.
        *
        * This method handles updates to the objects' transformations, animation updates, and input processing.
        *
        * @param dt The delta time since the last frame, used for smooth updates.
        */
        void OnUpdate(float dt) override {
            isVisible = false;
            UNUSED_PARAM(dt);

        }

        /**
        * @brief Renders the game objects in the layer.
        *
        * This method handles rendering of both static and animated objects with appropriate textures and shaders.
        */
        void OnRender() override {
            // dunnid render the utils stuff

        }

        void HideObjects() override {
            for (auto entity : utilsLayerObjects) {
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                transformComponent.scale = { 0.0f, 0.0f };
            }
        }

        void ShowObjects() override {
            for (auto entity : utilsLayerObjects) {
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                transformComponent.scale = originalScale[entity];
            }
        }

        void UpdateOriginalScale() override {
            for (auto entity : utilsLayerObjects) {
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                originalScale[entity] = transformComponent.scale;
            }
        }
        /**
        * @brief Stores the game objects in this layer.
        *
        * This map keeps track of game objects by their type and entity ID for easy access and management.
        */
        std::vector<Entity> utilsLayerObjects;
        std::unordered_map<Entity, Vector2D> originalScale;
    };
}