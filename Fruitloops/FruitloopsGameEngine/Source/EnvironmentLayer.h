/******************************************************************************/
/*!
\file    EnvironmentLayer.h
\project FruitLoops
\author  Zhang TongYan
\date    Apr 05, 2025
\brief   Declares the EnvironmentLayer class, responsible for managing and
		 rendering environmental elements such as animated entities (e.g. rats)
		 in the FruitLoops game engine.

		 This layer observes object spawn and deletion events, automatically
		 tracks environment-type objects (e.g., those tagged with "Environment"
		 in their entity name), and handles timed animations and movement of
		 rats across the screen. The movement is broken into stages (entering,
		 waiting, exiting), and uses a timer-based system with linear
		 interpolation (LERP) for smooth transitions. It also integrates with
		 the engine’s batch renderer and animator system.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "Layer.h"
#include "Message.h"

namespace FruitLoops {

	struct RatMovementState {
		// 1 = moving from first start to first target,
		// 2 = waiting,
		// 3 = moving from second start to second target,
		// 4 = finished (or you can loop)
		int stage = 0;
		float timer = 0.0f;
	};

	

	class EnvironmentLayer : public Layer, public Observer {
	public:

		EnvironmentLayer() : Layer("EnvironmentLayer"), Observer("EnvironmentLayerObserver") {}

		void HandleObjectSpawned(Entity entity) {
			// Get the type or other properties of the entity and add to the map
			std::string type = factory->GetEntityType(entity);
			size_t index = type.find_first_of("_");
			if (type.substr(index + 1, 11) == "Environment") {
				const Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
				originalScale[entity] = transformComponent.scale;
				EnvironmentLayerObjects.push_back(entity);
			}
				
		}

		void HandleObjectDeleted(Entity entity) {
			// Find and remove the entity from the map
			auto it = std::find(EnvironmentLayerObjects.begin(), EnvironmentLayerObjects.end(), entity);
			if (it != EnvironmentLayerObjects.end()) {
				EnvironmentLayerObjects.erase(it);
			}
		}

		Vector2D Lerp(const Vector2D& start, const Vector2D& end, float t) {
			return start + (end - start) * t;
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
				size_t index = type.find_first_of("_");
				if (type.substr(index + 1, 11) == "Environment") {
					const Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
					EnvironmentLayerObjects.emplace_back(entity);
					originalScale[entity] = transformComponent.scale;
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
			// Iterate over all EnvironmentLayerObjects
			for (auto entity : EnvironmentLayerObjects) {
				
				// Ensure we have a movement state for this rat
				if (ratStates.find(entity) == ratStates.end()) {
					// Initialize: stage 0 means not yet started
					ratStates[entity] = RatMovementState();
				}
				RatMovementState& state = ratStates[entity];
				Transform& transform = ecsHub->GetComponent<Transform>(entity);

				// Stage 0: Set initial position and start first movement
				if (state.stage == 0) {
					transform.position = Vector2D(0.0f, -330.0f);
					state.timer += dt;
					if (state.timer >= 10.0f) {
						state.timer = 0.0f;
						state.stage = 1;  // begin first move
					}
				}
				// Stage 1: Move from (220, -330) to (0, -330)
				else if (state.stage == 1) {
					float moveDuration = 2.0f; // duration in seconds for the first move
					state.timer += dt;
					float t = state.timer / moveDuration;
					if (t > 1.0f) t = 1.0f;
					Vector2D endPos(0.0f, -330.0f);
					Vector2D startPos(220.0f, -330.0f);
					for (auto& animations : GRAPHICS->animatorContainer) {
						if (animations.GetTextureName() == "Rat") {
							animations.SetPlaying(true);
						}
					}
					transform.position = Lerp(startPos, endPos, t);
					// When the move is complete, reset timer and advance stage
					if (t >= 1.0f) {
						state.stage = 2;
						state.timer = 0.0f;
					}
				}
				// Stage 2: Wait for 10 seconds before the second move
				else if (state.stage == 2) {
					for (auto& animations : GRAPHICS->animatorContainer) {
						if (animations.GetTextureName() == "Rat") {
							animations.SetPlaying(false);
						}
					}
					state.timer += dt;
					if (state.timer >= 10.0f) {
						// Optionally reset the rat's position for the second move.
						// In this example, we start at (0, 0).
						transform.position = Vector2D(0.0f, 0.0f);
						state.stage = 3;
						state.timer = 0.0f;
					}
				}
				// Stage 3: Move from (0, 0) to (-220, 0)
				else if (state.stage == 3) {
					float moveDuration = 2.0f; // duration for the second move
					state.timer += dt;
					float t = state.timer / moveDuration;
					if (t > 1.0f) t = 1.0f;
					Vector2D endPos(-220.0f, 0.0f);
					Vector2D startPos(0.0f, 0.0f);
					for (auto& animations : GRAPHICS->animatorContainer) {
						if (animations.GetTextureName() == "Rat") {
							animations.SetPlaying(true);
						}
					}
					transform.position = Lerp(startPos, endPos, t);
					// When the second move finishes, mark it as done (or loop if desired)
					if (t >= 1.0f) {
						// Optionally, you might want to reset or loop the animation:
						for (auto& animations : GRAPHICS->animatorContainer) {
							if (animations.GetTextureName() == "Rat") {
								animations.SetPlaying(false);
							}
						}
						state.stage = 0;
						state.timer = 0.0f;
					}
				}
				
			}
		}

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

			for (auto& entity : EnvironmentLayerObjects) {

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

			Renderer::EndBatch();
			Renderer::Flush();
			GRAPHICS->BatchShader.UnUse();

			for (auto& animations : GRAPHICS->animatorContainer) {
				if (animations.GetTextureName() == "Rat") {
					animations.SetPlaying(false);
				}
			}
		}

		std::vector<Entity>* GetObjectContainer() override {
			return &EnvironmentLayerObjects;
		}


		void HideObjects() override {
			for (auto entity : EnvironmentLayerObjects) {
				Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
				transformComponent.scale = { 0.0f, 0.0f };
			}
		}

		void ShowObjects() override {
			for (auto entity : EnvironmentLayerObjects) {
				Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
				transformComponent.scale = originalScale[entity];
			}
		}

		void UpdateOriginalScale() override {
			for (auto entity : EnvironmentLayerObjects) {
				Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
				originalScale[entity] = transformComponent.scale;
			}
		}

		std::vector<Entity> EnvironmentLayerObjects;
		std::unordered_map<Entity, RatMovementState> ratStates;
		std::unordered_map<Entity, Vector2D> originalScale;
		bool objectsShown;
	};
}