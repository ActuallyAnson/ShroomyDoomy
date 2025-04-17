/******************************************************************************/
/*!
\file    FPSLayer.h
\project FruitLoops
\author  Zhang TongYan
\date    April 5, 2025
\brief   Declares the FPSLayer class responsible for rendering the FPS counter
		 in the FruitLoops engine. This layer updates and displays the current
		 frames-per-second using the font system and can be toggled with a key
		 press.

This class derives from Layer and is designed to show a real-time performance
metric to the user, helpful during debugging and performance profiling.

All content © 2025 DigiPen Institute of Technology Singapore. All rights reserved.
*/
/******************************************************************************/
#pragma once
#include "Layer.h"

namespace FruitLoops {
	class FPSLayer : public Layer {
	public:

		FPSLayer() : Layer("FPSLayer") {}

		void OnAttach() override {
			isVisible = true;
			layerOpacity = 1.0f;


			std::vector<std::pair<Entity, std::string>> gameObjects = factory->GetAllGameObjects();
			for (const auto& [entity, type] : gameObjects) {
				if (type.substr(0, 3) == "FPS") {
					const FontDataComponent& fontComponent = ecsHub->GetComponent<FontDataComponent>(entity);
					FPSLayerObject = entity;
					originalScale = fontComponent.scale;
					container.emplace_back(entity);
				}

			}


		}

		void OnDetach() override {

		}

		void OnUpdate(float dt) override {
			UNUSED_PARAM(dt);

			if (ecsHub->HasComponent<FontDataComponent>(FPSLayerObject)) {
				static auto lastDrawLogTime = std::chrono::high_resolution_clock::now();
				InputSystem::update_time(1.0);
				std::stringstream stream;
				stream << std::fixed << std::setprecision(2) << InputSystem::fps;
				std::string fps = stream.str();
				std::string title = "FPS: " + fps;
				FontDataComponent& data = ecsHub->GetComponent<FontDataComponent>(FPSLayerObject);
				data.text = title;
			}
			if (inputSystem->IsKeyPressed(GLFW_KEY_L)) {
				isVisible = true;
			}
			else {
				isVisible = false;
			}

			
		}

		void OnRender() override {
            // Setup view matrix for Font shader
            GRAPHICS->FontShader.Use();
            GRAPHICS->SetUniformMat3(GRAPHICS->FontShader.GetHandle(), "u_ViewProj", GRAPHICS->camera2d.worldToNDCMatrix);

            Renderer::ResetStats();
            Renderer::BeginFontBatch();

            const Transform& transform = ecsHub->GetComponent<Transform>(FPSLayerObject);

            if (ecsHub->HasComponent<FontDataComponent>(FPSLayerObject)) {
                
                const FontDataComponent& data = ecsHub->GetComponent<FontDataComponent>(FPSLayerObject);
                const Material& materialComponent = ecsHub->GetComponent<Material>(FPSLayerObject);
                Renderer::RenderText(data.text, transform.position.x, transform.position.y, data.scale, data.font, materialComponent.color,
                    layerOpacity);

            }
            else {
#ifdef _DEBUG
                spdlog::info("Font object has no Font Component, did you forgot to include Font Component?");
#endif
            }
            Renderer::EndFontBatch();
            Renderer::FontFlush();
            GRAPHICS->FontShader.UnUse();
		}

		std::vector<Entity>* GetObjectContainer() override {
			return &container;
		}

		void HideObjects() override {
			FontDataComponent& fontComponent = ecsHub->GetComponent<FontDataComponent>(FPSLayerObject);
			fontComponent.scale = { 0.0f };
			
		}

		void ShowObjects() override {
			FontDataComponent& fontComponent = ecsHub->GetComponent<FontDataComponent>(FPSLayerObject);
			fontComponent.scale = originalScale;
		}

		void UpdateOriginalScale() override {
 
			FontDataComponent& fontComponent = ecsHub->GetComponent<FontDataComponent>(FPSLayerObject);
                originalScale = fontComponent.scale;
            
        }

		Entity FPSLayerObject;
		std::vector<Entity> container;
		float originalScale;
	};
}