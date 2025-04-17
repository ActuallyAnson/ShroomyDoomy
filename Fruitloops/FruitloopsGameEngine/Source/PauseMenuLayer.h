/******************************************************************************/
/*!
\file    PauseMenuLayer.h
\project FruitLoops
\author  Zhang TongYan
\date    April 5, 2025
\brief   Declares the PauseMenuLayer class which manages UI entities related
		 to the pause menu in the FruitLoops game engine.

This layer observes entity creation and deletion events for objects tagged
with "PauseMenu", maintains their original scales for visibility toggling,
and renders them when active. It integrates with the ECS system and uses
the batch renderer for efficient drawing.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/

#pragma once
#include "Layer.h"
#include "Message.h"

namespace FruitLoops {
	class PauseMenuLayer : public Layer, public Observer {
	public:

		PauseMenuLayer() : Layer("PauseMenuLayer"), Observer("PauseMenuLayerObserver") {}

		void HandleObjectSpawned(Entity entity) {
			// Get the type or other properties of the entity and add to the map
			std::string type = factory->GetEntityType(entity);
			std::cout << "UI Layer detected object spawn" + type.substr(0, 8) << std::endl;
			if (type.substr(0, 9) == "PauseMenu") {
				const Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
				originalScale[entity] = transformComponent.scale;
				PauseMenuLayerObjects.push_back(entity);
			}
				
		}

		void HandleObjectDeleted(Entity entity) {
			// Find and remove the entity from the map
			auto it = std::find(PauseMenuLayerObjects.begin(), PauseMenuLayerObjects.end(), entity);
			if (it != PauseMenuLayerObjects.end()) {
				PauseMenuLayerObjects.erase(it);
			}
		}

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
				if (type.substr(0, 9) == "PauseMenu") {
					const Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
					PauseMenuLayerObjects.emplace_back(entity);
					originalScale[entity] = transformComponent.scale;

				}

			}


		}

		void OnDetach() override {
			factory->UnregisterObserver(EventID::ObjectSpawned, this);
			factory->UnregisterObserver(EventID::ObjectDeleted, this);
		}

		void OnUpdate(float dt) override {
			dt = 1;
			if (!isVisible) {
				HideObjects();
				objectsShown = false;
				return;
			}
			else if (isVisible && !objectsShown) {
				ShowObjects();
				objectsShown = true;
			}
		}

		void OnRender() override {
			GRAPHICS->BatchShader.Use();
			GRAPHICS->SetUniformMat3(GRAPHICS->BatchShader.GetHandle(), "u_ViewProj", GRAPHICS->camera2d.worldToNDCMatrix);

			Renderer::ResetStats();
			Renderer::BeginBatch();

			for (auto& entity : PauseMenuLayerObjects) {
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

		std::vector<Entity>* GetObjectContainer() override {
			return &PauseMenuLayerObjects;
		}
		
		void HideObjects() override {
			for (auto entity : PauseMenuLayerObjects) {
				Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
				transformComponent.scale = { 0.0f, 0.0f };
			}
		}

		void ShowObjects() override {
			for (auto entity : PauseMenuLayerObjects) {
				Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
				transformComponent.scale = originalScale[entity];
			}
		}
		
		void UpdateOriginalScale() override {
			for (auto entity : PauseMenuLayerObjects) {
				Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
				originalScale[entity] = transformComponent.scale;
			}
		}

		std::vector<Entity> PauseMenuLayerObjects;
		std::unordered_map<Entity, Vector2D> originalScale;
		bool objectsShown;
	};
}