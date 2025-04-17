/**
 * @file GameLayer.h
 * @author Zhang TongYan
 * @date  Nov 8, 2024
 * 
 * @brief Defines the GameLayer class that manages and renders game objects in the game.
 *
 * This file contains the definition of the GameLayer class which is responsible for managing entities
 * in the game world, including players, enemies, and interactable objects like buttons and treasure. It listens
 * for spawn and delete events, updates game objects, and renders them on screen. The class inherits from
 * Layer and Observer to handle game object events and rendering processes.
 *
 * @note This class interacts with various subsystems including graphics, input, and assets.
 * Copyright (C) 2024 DigiPen Institute of Technology.
 * Reproduction or disclosure of this file or its contents without the
 * prior written consent of DigiPen Institute of Technology is prohibited.
 */

#pragma once
#include "Layer.h"
#include "Graphics.h"
#include "AssetsManager.h"

namespace FruitLoops {
    /**
    * @class GameLayer
    * @brief Manages and renders game objects in the "GameObjects" layer of the game.
    *
    * This class extends Layer and implements Observer to track and manage game entities like players,
    * enemies, and other objects. It listens for events related to object spawning and deletion, updates
    * their states, and renders them on the screen.
    */

    class GameLayer : public Layer, public Observer {
    public:
        /**
         * @brief Default constructor. Initializes GameLayer with a name and observer ID.
         */

        GameLayer() : Layer("GameLayer"), Observer("GameLayerObserver"), resetanimation(false) {}

        /**
        * @brief Handles the event when an object is spawned in the game.
        *
        * Adds the spawned object to the gameLayerObjects map, ensuring it's tracked by its entity ID.
        *
        * @param entity The spawned entity to be handled.
        */
        void HandleObjectSpawned(Entity entity) {
            // Get the type or other properties of the entity and add to the map
            std::string type = factory->GetEntityType(entity);
            size_t index = type.find_first_of("_");
            if (type.substr(index + 1, 5) == "Game_") {
                const Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                originalScale[entity] = transformComponent.scale;
                gameLayerObjects.push_back(entity);
            }    
        }

        /**
         * @brief Handles the event when an object is deleted in the game.
         *
         * Removes the entity from the gameLayerObjects map to ensure it's no longer tracked.
         *
         * @param entity The deleted entity to be handled.
         */
        void HandleObjectDeleted(Entity entity) {
            // Find and remove the entity from the map
            auto it = std::find(gameLayerObjects.begin(), gameLayerObjects.end(), entity);
            if (it != gameLayerObjects.end()) {
                gameLayerObjects.erase(it);
            }
        }


        std::vector<Entity>* GetObjectContainer() override {
            return &gameLayerObjects;
        }

        /**
         * @brief Registers the observers and handles initial game objects setup when the layer is attached.
         *
         * Registers this layer to observe events like object spawning and deletion. Also pre-populates the
         * map of game objects for the player, enemies, and buttons.
         */

        void OnAttach() override {

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

            std::vector<std::pair<Entity, std::string>> gameObjects = factory->GetAllGameObjects();
            for (const auto& [entity, type] : gameObjects) {
                size_t index = type.find_first_of("_");
                if (type.substr(index + 1, 5) == "Game_") {
                    const Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                    gameLayerObjects.emplace_back(entity);
                    originalScale[entity] = transformComponent.scale;
                }
                
            }
            
        }

        /**
         * @brief Unregisters observers and cleans up when the layer is detached.
         *
         * Unsubscribes from the events related to object spawn and deletion.
         */
        void OnDetach() override {
            factory->UnregisterObserver(EventID::ObjectSpawned, this);
            factory->UnregisterObserver(EventID::ObjectDeleted, this);
        }

        /**
        * @brief Updates the game objects in the layer.
        *
        * This method handles updates to the objects' transformations, animation updates, and input processing.
        *
        * @param dt The delta time since the last frame, used for smooth updates.
        */
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
            
            // animator
            for (auto& animations : GRAPHICS->animatorContainer) {
                animations.Update(dt);
            }

            //auto entities = ecsHub->GetEntitiesWithComponents<Transform>();
            for (auto& entity : gameLayerObjects) {
                auto& transform = ecsHub->GetComponent<Transform>(entity);
                
                // maintain orientation within 360
                if (transform.orientation.x > 360.f) {
                    transform.orientation.x -= 360.f;
                }
                if (transform.orientation.x < -360.f) {
                    transform.orientation.x += 360.f;
                }

            }

        }



        /**
        * @brief Renders the game objects in the layer.
        *
        * This method handles rendering of both static and animated objects with appropriate textures and shaders.
        */
        void OnRender() override {
            GRAPHICS->BatchShader.Use();
            GRAPHICS->SetUniformMat3(GRAPHICS->BatchShader.GetHandle(), "u_ViewProj", GRAPHICS->camera2d.worldToNDCMatrix);

            Renderer::ResetStats();
            Renderer::BeginBatch();
            
            
            std::vector<Entity> animationEntity;
            for (auto& animations : GRAPHICS->animatorContainer) {
                if (animations.GetPlaying()) {
                    for (Entity ent : animations.GetEntity()) {
                        animationEntity.emplace_back(ent);
                    }
                }

            }
            
            

            for (auto& entity : gameLayerObjects) {

                if (std::find(animationEntity.begin(), animationEntity.end(), entity) != animationEntity.end())
                    continue;

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
					{ material.color.x, material.color.y, material.color.z, layerOpacity }
                );
                

            }

            for (auto& animations : GRAPHICS->animatorContainer) {
                if (animations.GetPlaying()) {
                    animations.RenderCurrentFrame(0.0f);
                }

            }
            
;
            Renderer::EndBatch();
            Renderer::Flush();
            GRAPHICS->BatchShader.UnUse();

        }
        
        void HideObjects() override {
            for (auto entity : gameLayerObjects) {
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                transformComponent.scale = { 0.0f, 0.0f };
            }
        }

        void ShowObjects() override {
            for (auto entity : gameLayerObjects) {
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                transformComponent.scale = originalScale[entity];
            }
        }

        void UpdateOriginalScale() override {
            for (auto entity : gameLayerObjects) {
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                originalScale[entity] = transformComponent.scale;
            }
        }
        /**
        * @brief Stores the game objects in this layer.
        *
        * This map keeps track of game objects by their type and entity ID for easy access and management.
        */
        std::vector<Entity> gameLayerObjects;
        std::unordered_map<Entity, Vector2D> originalScale;
        bool resetanimation;
        bool objectsShown;
    };
}