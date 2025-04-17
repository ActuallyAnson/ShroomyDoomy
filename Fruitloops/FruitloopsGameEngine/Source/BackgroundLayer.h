/**
 * @file BackgroundLayer.h
 * @author Zhang TongYan
 * @date  Nov 8, 2024
 * @brief Defines the BackgroundLayer class responsible for managing the background layer of the game.
 *
 * This file contains the implementation of the BackgroundLayer class, which inherits from the
 * Layer class and implements the Observer interface. The class listens for object spawn and
 * deletion events and updates the background layer accordingly. It also handles rendering and
 * updating the background entities in the game.
 * 
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */


#pragma once
#include "Layer.h"
#include "ECSDefinitions.h"
#include "Factory.h"
#include "Graphics.h"
#include "AssetsManager.h"
#include "LevelManager.h"

namespace FruitLoops {

    /**
     * @class BackgroundLayer
     * @brief A class responsible for managing the background layer of the game.
     *
     * The BackgroundLayer class handles the spawning and deletion of background objects,
     * registers and unregisters for events, and renders the background layer in the game.
     * It uses a map to store the background entities and updates them during the game loop.
     * It also provides debugging features to visualize entities and check for mouse interaction.
     */
    class BackgroundLayer : public Layer, public Observer {
    public:
        /**
         * @brief Default constructor for the BackgroundLayer class.
         *
         * This constructor initializes the BackgroundLayer and Observer with appropriate names.
         * It also prepares the BackgroundLayer for event handling and background entity management.
         */
        BackgroundLayer() : Layer("BackgroundLayer"), Observer("BackgroundLayerObserver") {
            
        }

        std::vector<Entity>* GetObjectContainer() override {
            return &backgroundLayerObjects;
        }

        /**
         * @brief Handles the spawn event of an object.
         *
         * This function is called when an object is spawned in the game world. It checks the
         * type of the spawned entity and adds it to the background layer object map if it is
         * a background object.
         *
         * @param entity The entity that was spawned.
         */
        void HandleObjectSpawned(Entity entity) {
            // Get the type or other properties of the entity and add to the map
            std::string type = factory->GetEntityType(entity);
            size_t index = type.find_first_of("_");
            if (type.substr(index + 1) == "Tile") {
                const Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                originalScale[entity] = transformComponent.scale;
                backgroundLayerObjects.push_back(entity);
            }
               
        }

        /**
         * @brief Handles the deletion event of an object.
         *
         * This function is called when an object is deleted from the game world. It searches
         * for the object in the background layer object map and removes it if found.
         *
         * @param entity The entity that was deleted.
         */
        void HandleObjectDeleted(Entity entity) {
            // Find and remove the entity from the map
            auto it = std::find(backgroundLayerObjects.begin(), backgroundLayerObjects.end(), entity);
            if (it != backgroundLayerObjects.end()) {
                backgroundLayerObjects.erase(it);
            }
        }


        /**
         * @brief Called when the layer is attached to the game.
         *
         * This function registers the BackgroundLayer to observe spawn and delete events. It
         * also initializes the background layer with entities and tiles from the GridSystem,
         * adding them to the map for rendering.
         */
        void OnAttach() override {
#ifdef _DEBUG
            std::cout << "Attach called" << std::endl;
#endif
            factory->RegisterObserver(EventID::ObjectSpawned, this);
            factory->RegisterObserver(EventID::ObjectDeleted, this);
            isVisible = false;
            layerOpacity = 1.0f;
            // Register handlers for spawn and delete events
            RegisterHandler(EventID::ObjectSpawned, [this](IMessage* message) {
                auto* spawnMessage = dynamic_cast<ObjectSpawnedMessage*>(message);
                if (spawnMessage) {
                    HandleObjectSpawned(spawnMessage->spawnedEntity);
                }
                });

            RegisterHandler(EventID::ObjectDeleted, [this](IMessage* message) {
                auto* deleteMessage = dynamic_cast<ObjectDeletedMessage*>(message);
                if (deleteMessage) {
                    HandleObjectDeleted(deleteMessage->deletedEntity);
                }
                });

            //std::vector<Entity> tileObject = GameObjectFactory::getGameObject("Tile_0_0");
            std::vector<Entity> tileObject = ecsHub->GetEntitiesWithComponents<TileComponent>();
            for (const Entity& objects : tileObject) {
                /*std::string type = factory->GetEntityType(objects);
                size_t index = type.find_first_of("_");*/
                //if (type.substr(index + 1) == "Tile") {
                    const Transform& transformComponent = ecsHub->GetComponent<Transform>(objects);
                    backgroundLayerObjects.emplace_back(objects);
                    originalScale[objects] = transformComponent.scale;
                //}
            }


#ifdef _DEBUG
            std::cout << "Size of background container aft attatch: " << backgroundLayerObjects.size() << std::endl;
#endif
        }

        /**
         * @brief Called when the layer is detached from the game.
         *
         * This function unregisters the BackgroundLayer from observing spawn and delete events.
         */
        void OnDetach() override {
            factory->UnregisterObserver(EventID::ObjectSpawned, this);
            factory->UnregisterObserver(EventID::ObjectDeleted, this);
        }

        /**
         * @brief Updates the background layer.
         *
         * This function is called every frame to update the state of the background layer.
         * It allows for any necessary background layer updates, but in this case, it is
         * currently a placeholder with no implementation beyond setting the delta time to 1.
         *
         * @param dt The time delta since the last update, typically used for animations or movements.
         */
        void OnUpdate(float dt) override {
            UNUSED_PARAM(dt);
            if (!isVisible) {
                HideObjects();
                objectsShown = false;
                return;
            }
            else if (isVisible && !objectsShown) {
                ShowObjects();
                objectsShown = true;
            }
            for (auto& entity : backgroundLayerObjects) {
                Transform& transform = ecsHub->GetComponent<Transform>(entity);
                transform.position = Renderer::SnapToGrid(transform.position, transform.scale.x);
            }
            
        }

        void DebugRender() {
            //Setup view matrix for Debug shader
            GRAPHICS->DebugShader.Use();
            GRAPHICS->SetUniformMat3(GRAPHICS->DebugShader.GetHandle(), "u_ViewProj", GRAPHICS->camera2d.worldToNDCMatrix);

            Renderer::ResetStats();
            Renderer::BeginLineBatch();

            for (auto& entity : backgroundLayerObjects) {
                Material& material = ecsHub->GetComponent<Material>(entity);
                const Transform& transform = ecsHub->GetComponent<Transform>(entity);

                // Get transformed cursor position
                Vector2D cursorPosition = inputSystem->GetCursorPosition();

                // Define the entity's bounding box using position and scale
                MinMax2D entityBox;
                Vector2D halfSize = transform.scale * 0.5f;

                entityBox.min = transform.position - halfSize;
                entityBox.max = transform.position + halfSize;


                if (IsPointInsideBox(cursorPosition, entityBox)) {
                    Renderer::DrawDebugQuad(
                        { transform.position.x, transform.position.y },
                        { transform.scale.x, transform.scale.y },
                        glm::radians(transform.orientation.x),
                        glm::vec4(0, 1, 0, layerOpacity)
                    );
                }
                else {
                    Renderer::DrawDebugQuad(
                        { transform.position.x, transform.position.y },
                        { transform.scale.x, transform.scale.y },
                        glm::radians(transform.orientation.x),
                        glm::vec4(material.color.x, material.color.y, material.color.z, layerOpacity)
                    );
                }
            }

            Renderer::EndLineBatch();
            Renderer::LineFlush();
            GRAPHICS->DebugShader.UnUse();
        }

        void NormalRender() {
            GRAPHICS->BatchShader.Use();
            GRAPHICS->SetUniformMat3(GRAPHICS->BatchShader.GetHandle(), "u_ViewProj", GRAPHICS->camera2d.worldToNDCMatrix);

            Renderer::ResetStats();
            Renderer::BeginBatch();

            for (auto& entity : backgroundLayerObjects) {
                Material& material = ecsHub->GetComponent<Material>(entity);
                const Transform& transform = ecsHub->GetComponent<Transform>(entity);

                if (!ecsHub->HasComponent<TileComponent>(entity)) {
                    spdlog::error("Entity does not have a tile component, did you forget to include it?");
                    continue;
                }

                const TileComponent& tileComponent = ecsHub->GetComponent<TileComponent>(entity);
                if (tileComponent.IsHighlightedEnemy()) {
                    material.color.x = 1.0f;  // R
                    material.color.y = 0.3f;  // G
                    material.color.z = 0.3f;  // B
                    Renderer::DrawQuad(
                        { transform.position.x, transform.position.y },
                        { transform.scale.x, transform.scale.y },
                        glm::radians(transform.orientation.x),
                        assetsManager->GetTextures(material.textureName),
                        glm::vec2(material.uvMin.x, material.uvMin.y),
                        glm::vec2(material.uvMax.x, material.uvMax.y),
                        { material.color.x, material.color.y, material.color.z, layerOpacity }
                    );
                }
                else if (tileComponent.IsHighlighted()) {
                    if (levelManager->GetCurrLevel() == 0) {
                        material.color.x = 0.0f;  // R
                        material.color.y = 0.9f;  // G
                        material.color.z = 1.0f;  // B
                        Renderer::DrawQuad(
                            { transform.position.x, transform.position.y },
                            { transform.scale.x, transform.scale.y },
                            glm::radians(transform.orientation.x),
                            assetsManager->GetTextures(material.textureName),
                            glm::vec2(material.uvMin.x, material.uvMin.y),
                            glm::vec2(material.uvMax.x, material.uvMax.y),
                            { material.color.x, material.color.y, material.color.z, layerOpacity }
                        );
                    }
                    else {
                        material.color.x = 1.0f;  // R
                        material.color.y = 1.0f;  // G
                        material.color.z = 0.2f;  // B
                        Renderer::DrawQuad(
                            { transform.position.x, transform.position.y },
                            { transform.scale.x, transform.scale.y },
                            glm::radians(transform.orientation.x),
                            assetsManager->GetTextures(material.textureName),
                            glm::vec2(material.uvMin.x, material.uvMin.y),
                            glm::vec2(material.uvMax.x, material.uvMax.y),
                            { material.color.x, material.color.y, material.color.z, layerOpacity }
                        );
                    }
                }
                else {
                    material.color.x = 1.0f;
                    material.color.y = 1.0f;
                    material.color.z = 1.0f;
                    Renderer::DrawQuad(
                        { transform.position.x, transform.position.y },
                        { transform.scale.x, transform.scale.y },
                        glm::radians(transform.orientation.x),
                        assetsManager->GetTextures(material.textureName),
                        glm::vec2(material.uvMin.x, material.uvMin.y),
                        glm::vec2(material.uvMax.x, material.uvMax.y),
                        { material.color.x, material.color.y, material.color.z, layerOpacity }
                    );
                }
            }

            Renderer::EndBatch();
            Renderer::Flush();
            GRAPHICS->BatchShader.UnUse();
        }
        /**
         * @brief Renders the background layer.
         *
         * This function is responsible for rendering the background objects stored in the
         * backgroundLayerObjects map. It uses the graphics system to draw the objects
         * with their associated materials and transforms. Debugging features are also
         * included to visualize entities and check if the mouse cursor is over them.
         */
        void OnRender() override {

            if (inputSystem->IsKeyPressed(GLFW_KEY_P)) {
                DebugRender();
            }
            else {
                NormalRender();
            }
            
        }

        void HideObjects() override {
            for (auto entity : backgroundLayerObjects) {
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                transformComponent.scale = { 0.0f, 0.0f };
            }
        }

        void ShowObjects() override {
            for (auto entity : backgroundLayerObjects) {
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                transformComponent.scale = originalScale[entity];
            }
        }

        void UpdateOriginalScale() override {
            for (auto entity : backgroundLayerObjects) {
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                originalScale[entity] = transformComponent.scale;
            }
        }

    private:
        /**
         * @brief A map of background objects, where the key is a string identifier and the value is an Entity.
         *
         * This map stores the background objects in the game, using a combination of entity type
         * and unique identifiers as the key. It allows for easy addition, removal, and access to background objects.
         */
        std::vector<Entity> backgroundLayerObjects;
        std::unordered_map<Entity, Vector2D> originalScale;
        bool objectsShown;
    };
}
