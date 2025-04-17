/****************************************************************************/
/*!
\file		FontLayer.h
\Proj name  Fruitloops
\author 	Zhang TongYan
\date   	Oct 02, 2024
\brief		Declaration of the FontLayer class, an extension of the Layer class
            designed to handle and render font-related entities within the Fruitloops
            game engine. This layer observes and reacts to entity spawning and deletion,
            and it processes input events related to text manipulation. It uses the 
            AssetsManager for managing font assets, allowing dynamic text rendering and 
            interaction within the game environment.
All content (C) 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
 */
 /****************************************************************************/
#pragma once
#include "Layer.h"
#include "Graphics.h"
#include "AssetsManager.h"
#include "ImGuiPanels.h"
//#include "Audio.h"

namespace FruitLoops {

    class FontLayer : public Layer, public Observer {
    public:
        FontLayer() : Layer("FontLayer"), Observer("FontLayerObserver") {}

        std::vector<Entity>* GetObjectContainer() override {
            return &fontLayerObjects;
        }

        /*!
        \brief Handles the spawning of font-related entities, registering them for management.
        \param entity The entity that has been spawned and needs handling.
        */
        void HandleObjectSpawned(Entity entity) {
            // Get the type or other properties of the entity and add to the map
            if (ecsHub->HasComponent<FontDataComponent>(entity)) {
                const Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                std::string type = factory->GetEntityType(entity);
                size_t index = type.find_first_of("_");
                if (type.substr(index + 1, 4) == "Font" && type.substr(0, 3) != "FPS") {
                    fontLayerObjects.emplace_back(entity);
                    originalScale[entity] = transformComponent.scale;
                }
            }

            /*if (ecsHub->HasComponent<UnitComponent>(entity)) {
                auto& unitComponent = ecsHub->GetComponent<UnitComponent>(entity);
                if (unitComponent.unitType == UnitType::Player) {
                    playerContainer.emplace_back(entity);
                }
                else {
                    enemyContainer.emplace_back(entity);
                }
            }*/
            
        }

        /*!
        \brief Handles the deletion of font-related entities, removing them from management.
        \param entity The entity that has been deleted and needs removal.
        */
        void HandleObjectDeleted(Entity entity) {
            // Find and remove the entity from the map
            auto it = std::find(fontLayerObjects.begin(), fontLayerObjects.end(), entity);
            if (it != fontLayerObjects.end()) {
                fontLayerObjects.erase(it);
            }
        }

        /*!
        \brief Attaches the layer to the game engine, setting up necessary event handlers and initial states.
        */
        void OnAttach() override {

            factory->RegisterObserver(EventID::FontSpawned, this);
            factory->RegisterObserver(EventID::FontDeleted, this);
            isVisible = false;

            #ifdef _DEBUG
            imguiSystem->RegisterObserver(EventID::FontSpawned, this);
            imguiSystem->RegisterObserver(EventID::FontDeleted, this);
            #endif

            // Register handlers for spawn and delete events
            RegisterHandler(EventID::FontSpawned, [this](IMessage* message) {
                auto* spawnMessage = dynamic_cast<FontSpawnedMessage*>(message);
                if (spawnMessage) {
                    HandleObjectSpawned(spawnMessage->spawnedEntity);
                }
                });

            RegisterHandler(EventID::FontDeleted, [this](IMessage* message) {
                auto* deleteMessage = dynamic_cast<FontDeletedMessage*>(message);
                if (deleteMessage) {
                    HandleObjectDeleted(deleteMessage->deletedEntity);
                }
                });

            std::vector<Entity> fontObjects = ecsHub->GetEntitiesWithComponents<FontDataComponent>();
            for (const Entity& objects : fontObjects) {
                const Transform& transformComponent = ecsHub->GetComponent<Transform>(objects);
                std::string type = factory->GetEntityType(objects);
                if (type.substr(0, 3) != "FPS") {
                    fontLayerObjects.emplace_back(objects);
                    originalScale[objects] = transformComponent.scale;
                }
                
            }

            // Update enemy Container
            auto unitContainer = ecsHub->GetEntitiesWithComponents<UnitComponent>();
            for (auto& entity : unitContainer) {
                auto& unitComponent = ecsHub->GetComponent<UnitComponent>(entity);
                if (unitComponent.unitType == UnitType::Enemy) {
                    enemyContainer.emplace_back(entity);
                }
            }

            // Update player Container
            for (auto& entity : unitContainer) {
                auto& unitComponent = ecsHub->GetComponent<UnitComponent>(entity);
                if (unitComponent.unitType == UnitType::Player) {
                    playerContainer.emplace_back(entity);
                }
            }

        }

        /*!
        \brief Detaches the layer from the game engine, cleaning up resources and unregistering observers.
        */
        void OnDetach() override {
            factory->UnregisterObserver(EventID::FontSpawned, this);
            factory->UnregisterObserver(EventID::FontDeleted, this);
        }

        /*!
        \brief Regularly updates font-related entities based on input or game events.
        \param dt Time elapsed since the last update, in seconds.
        */
        void OnUpdate(float dt) override
        {
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
            // 1. Check for valid player
            if (playerContainer.empty() || !ecsHub->HasComponent<UnitComponent>(playerContainer[0])) {
                //FL_DEBUG_ERROR("No valid player entity found.");
                return;  // Exit the function early if no valid player exists
            }

            // Assuming only one player
            Entity player = playerContainer[0];
            auto& playerUnitComponent = ecsHub->GetComponent<UnitComponent>(player);

            // 2. Check if there are no enemies before processing enemy-related data
            if (enemyContainer.empty()) {
                //FL_DEBUG_INFO("No enemies available for updating.");
                return; // Exit the function early if no enemies exist
            }

            // 3. Verify all enemies have a UnitComponent
            for (auto& enemyEntity : enemyContainer) {
                if (!ecsHub->HasComponent<UnitComponent>(enemyEntity)) {
                    //FL_DEBUG_ERROR("No valid UnitComponent for enemy entity %d.", enemyEntity);
                    return;  // Exit early if at least one enemy lacks a UnitComponent
                }
            }

            bool shouldShowTextBox = false;
            bool shouldShowTextBox1 = false;

            // Get player position
            auto& playerTransform = ecsHub->GetComponent<Transform>(player);
            Vector2D playerPosition = playerTransform.position;

            // Get cursor position
            Vector2D cursorPosition = inputSystem->GetCursorPosition();

            // Track energy estimation
            int estimatedEnergyCost = 0;

            auto gameObjects = factory->GetAllGameObjects();
            for (const auto& [tileEntity, type] : gameObjects) {
                if (!ecsHub->HasComponent<TileComponent>(tileEntity)) {
                    continue;
                }

                auto& tileComponent = ecsHub->GetComponent<TileComponent>(tileEntity);
                auto& tileTransform = ecsHub->GetComponent<Transform>(tileEntity);

                // Check if the tile is highlighted
                if (!tileComponent.IsHighlighted()) {
                    continue;
                }

                // Compute tile center dynamically
                Vector2D halfSize = tileTransform.scale * 0.5f;
                Vector2D tileCenter = tileTransform.position; // Using transform position as center

                // Define tile bounding box
                MinMax2D tileBox;
                tileBox.min = tileTransform.position - halfSize;
                tileBox.max = tileTransform.position + halfSize;

                if (IsPointInsideBox(cursorPosition, tileBox)) {
                    // Compute tile-based movement distance
                    int tileStepsX = static_cast<int>(std::abs(playerPosition.x - tileCenter.x) / tileTransform.scale.x);
                    int tileStepsY = static_cast<int>(std::abs(playerPosition.y - tileCenter.y) / tileTransform.scale.y);

                    // Total steps needed to move to the tile
                    estimatedEnergyCost = tileStepsX + tileStepsY; // Count each step correctly
                    break;
                }
            }

            // 4. Proceed with font-layer entity updates now that both player and enemies are valid
            for (auto& entity : fontLayerObjects) {

                if (!ecsHub->HasComponent<FontDataComponent>(entity)) {
                    FL_DEBUG_ERROR("Entity %i does not have a font component, did you forget to include it?", entity);
                    continue;
                }

                auto& textComponent = ecsHub->GetComponent<FontDataComponent>(entity);
                std::string entityType = factory->GetEntityType(entity);
                size_t index = entityType.find_first_of("_");
                if (entityType.substr(index + 1, 13) == "Font_HeadButt") {
                    Entity icon = factory->GetEntityByName("headbuttIconForHover")[0];
                    cursorPosition = inputSystem->GetCursorPosition();
                    auto& transform = ecsHub->GetComponent<Transform>(icon);

                    MinMax2D entityBox;
                    Vector2D halfSize = transform.scale * 0.5f;
                    entityBox.min = transform.position - halfSize;
                    entityBox.max = transform.position + halfSize;
                   // audioSystem->Play_Sound("SFX_UI_Hover 3", 0.2f);

                    if (IsPointInsideBox(cursorPosition, entityBox)) {
                        shouldShowTextBox = true;
                        textComponent.scale = 0.6f;
                    }
                    else {
                        textComponent.scale = 0.0f;
                    }
                }
                else if (entityType.substr(index + 1, 17) == "Font_RangedAttack") {
                    Entity icon = factory->GetEntityByName("specialAttackIconForHover")[0];
                    cursorPosition = inputSystem->GetCursorPosition();
                    auto& transform = ecsHub->GetComponent<Transform>(icon);

                    MinMax2D entityBox;
                    Vector2D halfSize = transform.scale * 0.5f;
                    entityBox.min = transform.position - halfSize;
                    entityBox.max = transform.position + halfSize;

                    if (IsPointInsideBox(cursorPosition, entityBox)) {
                        shouldShowTextBox1 = true;
                        textComponent.scale = 0.6f;
                    }
                    else {
                        textComponent.scale = 0.0f;
                    }
                }
                if (factory->GetEntityByName("SkillsWindow").size() != 0) {
                    Entity txtbox = factory->GetEntityByName("SkillsWindow")[0];
                    auto& transformTxtBox = ecsHub->GetComponent<Transform>(txtbox);
                    if (shouldShowTextBox) {
                        transformTxtBox.scale = { 500.0f, 300.0f };
                        transformTxtBox.position = { -675.0f, -350.0f };
                    }
                    else if (shouldShowTextBox1) {
                        transformTxtBox.scale = { 500.0f, 300.0f };
                        transformTxtBox.position = { -675.0f, -450.0f };
                    }
                    else {
                        transformTxtBox.scale = { 0.0f, 0.0f };
                    }
                }
                

                //if (factory->GetEntityType(entity) == "Level1_Font_Inventory") {
                //    Entity icon = factory->GetEntityByName("Level1_Game_bagIcon")[0];
                //    Vector2D cursorPosition = inputSystem->GetCursorPosition();
                //    auto& transform = ecsHub->GetComponent<Transform>(icon);

                //    MinMax2D entityBox;
                //    Vector2D halfSize = transform.scale * 0.5f;
                //    entityBox.min = transform.position - halfSize;
                //    entityBox.max = transform.position + halfSize;

                //    Entity inventory = factory->GetEntityByName("Level1_Game_innerBag")[0];
                //    auto& transformInvenBox = ecsHub->GetComponent<Transform>(inventory);

                //    if (IsPointInsideBox(cursorPosition, entityBox)) {
                //        textComponent.scale = 0.6f;
                //        transformInvenBox.scale = { 500.0f, 500.0f };
                //    }
                //    else {
                //        textComponent.scale = 0.0f;
                //        transformInvenBox.scale = { 0.0f, 0.0f };
                //    }
                //}
                

                // Identify entity type (substring after prefix)
                //std::string barType = factory->GetEntityType(entity).substr(12);

                std::string eT = factory->GetEntityType(entity);
                size_t fontPos = eT.find("Font_");
                std::string barType = eT.substr(fontPos + 5); // Skip "Font_"


                // Player Health Bar
                if (barType == "HealthBar") {
                    textComponent.text = std::to_string(playerUnitComponent.health) +
                        " I" + std::to_string(playerUnitComponent.maxHealth);
                }
                // Player Energy Bar - Display estimated energy left / max energy if hovering a highlighted tile
                else if (barType == "EnergyBar") {
                    int energyAfterMove = playerUnitComponent.energy; // Default to current energy

                    if (estimatedEnergyCost > 0) {
                        // Deduct estimated cost only when hovering over a highlighted tile
                        energyAfterMove = std::max(0, playerUnitComponent.energy - estimatedEnergyCost);
                        textComponent.text = std::to_string(energyAfterMove) +
                            " I" + std::to_string(playerUnitComponent.maxEnergy);
                    }
                    else {
                        // Show normal energy/maxEnergy when NOT hovering over a highlighted tile
                        textComponent.text = std::to_string(playerUnitComponent.energy) +
                            " I" + std::to_string(playerUnitComponent.maxEnergy);
                    }
                }
                // Player Attack Power
                else if (barType == "AttackPower") {
                    textComponent.text = std::to_string(playerUnitComponent.attackPower);
                }
                // Enemy Health Bar
                else if (barType == "EnemyHealthBar") {
                    for (Entity enemyEntity : enemyContainer) {
                        auto& enemyTransformComponent = ecsHub->GetComponent<Transform>(enemyEntity);
                        cursorPosition = inputSystem->GetCursorPosition();

                        // Define the entity's bounding box using position and scale
                        MinMax2D entityBox;
                        Vector2D halfSize = enemyTransformComponent.scale * 0.5f;

                        entityBox.min = enemyTransformComponent.position - halfSize;
                        entityBox.max = enemyTransformComponent.position + halfSize;

                        if (IsPointInsideBox(cursorPosition, entityBox)) {
                            auto& enemyUnitComponent = ecsHub->GetComponent<UnitComponent>(enemyEntity);
                            textComponent.text = std::to_string(enemyUnitComponent.health) +
                                " I" + std::to_string(enemyUnitComponent.maxHealth);
                            break; // Update for the first hovered enemy only
                        }
                    }
                }
                // Enemy Energy Bar
                else if (barType == "EnemyEnergyBar") {
                    for (Entity enemyEntity : enemyContainer) {
                        auto& enemyTransformComponent = ecsHub->GetComponent<Transform>(enemyEntity);
                        cursorPosition = inputSystem->GetCursorPosition();

                        // Define the entity's bounding box using position and scale
                        MinMax2D entityBox;
                        Vector2D halfSize = enemyTransformComponent.scale * 0.5f;

                        entityBox.min = enemyTransformComponent.position - halfSize;
                        entityBox.max = enemyTransformComponent.position + halfSize;

                        if (IsPointInsideBox(cursorPosition, entityBox)) {
                            auto& enemyUnitComponent = ecsHub->GetComponent<UnitComponent>(enemyEntity);
                            textComponent.text = std::to_string(enemyUnitComponent.energy) +
                                " I" + std::to_string(enemyUnitComponent.maxEnergy);
                            break; // Update for the first hovered enemy only
                        }
                    }
                }
                // Enemy Health Bar
                else if (barType == "EnemyAttackPower") {
                    for (Entity enemyEntity : enemyContainer) {
                        auto& enemyTransformComponent = ecsHub->GetComponent<Transform>(enemyEntity);
                        cursorPosition = inputSystem->GetCursorPosition();

                        // Define the entity's bounding box using position and scale
                        MinMax2D entityBox;
                        Vector2D halfSize = enemyTransformComponent.scale * 0.5f;

                        entityBox.min = enemyTransformComponent.position - halfSize;
                        entityBox.max = enemyTransformComponent.position + halfSize;

                        if (IsPointInsideBox(cursorPosition, entityBox)) {
                            auto& enemyUnitComponent = ecsHub->GetComponent<UnitComponent>(enemyEntity);
                            textComponent.text = std::to_string(enemyUnitComponent.attackPower);
                            break; // Update for the first hovered enemy only
                        }
                    }
                }
            }
        }


        /*!
        \brief Renders the font-related entities managed by this layer.
        */
        void OnRender() override
        {
            // 1. Check for valid player
            if (playerContainer.empty() || !ecsHub->HasComponent<UnitComponent>(playerContainer[0])) {
                //FL_DEBUG_ERROR("No valid player entity found for rendering.");
                return;  // Exit early if no valid player exists
            }

            // 2. Check for valid enemies
            if (enemyContainer.empty()) {
                //FL_DEBUG_INFO("No enemies available for rendering.");
                return;  // Exit early if there are no enemies
            }
            else {
                // Verify all enemies in the container have a UnitComponent
                for (auto& enemyEntity : enemyContainer) {
                    if (!ecsHub->HasComponent<UnitComponent>(enemyEntity)) {
                        FL_DEBUG_ERROR("No valid enemy entity found for rendering (missing UnitComponent).");
                        return;  // Exit if at least one enemy is invalid
                    }
                }
            }

            // Setup view matrix for Font shader
            GRAPHICS->FontShader.Use();
            GRAPHICS->SetUniformMat3(GRAPHICS->FontShader.GetHandle(), "u_ViewProj", GRAPHICS->camera2d.worldToNDCMatrix);

            Renderer::ResetStats();
            Renderer::BeginFontBatch();

            // 3. Render Entities
            for (auto& entity : fontLayerObjects) {

                // Make sure the transform and font components exist
                if (!ecsHub->HasComponent<Transform>(entity)) {
                    FL_DEBUG_INFO("Entity %i has no Transform. Skipping.", entity);
                    continue;
                }
                if (!ecsHub->HasComponent<FontDataComponent>(entity)) {
                    FL_DEBUG_INFO("Font object has no FontDataComponent. Skipping entity %i.", entity);
                    continue;
                }

                const Transform& transform = ecsHub->GetComponent<Transform>(entity);
                std::string entityType = factory->GetEntityType(entity).substr(12, 5);

                std::string tutentityType = factory->GetEntityType(entity).substr(14, 5);

                // 3a. Enemy-Related Rendering
                if (entityType == "Enemy" || tutentityType == "Enemy") {
                    for (Entity enemyEntity : enemyContainer) {
                        auto& enemyTransformComponent = ecsHub->GetComponent<Transform>(enemyEntity);
                        Vector2D cursorPosition = inputSystem->GetCursorPosition();

                        // Define the entity's bounding box using position and scale
                        MinMax2D entityBox;
                        Vector2D halfSize = enemyTransformComponent.scale * 0.5f;

                        entityBox.min = enemyTransformComponent.position - halfSize;
                        entityBox.max = enemyTransformComponent.position + halfSize;

                        if (IsPointInsideBox(cursorPosition, entityBox)) {
                            const FontDataComponent& data = ecsHub->GetComponent<FontDataComponent>(entity);
                            const Material& materialComponent = ecsHub->GetComponent<Material>(entity);

                            Renderer::RenderText(data.text,
                                transform.position.x,
                                transform.position.y,
                                data.scale,
                                data.font,
                                materialComponent.color,
                                layerOpacity
                            );

                            // Once we've found one enemy under the cursor, we can stop
                            break;
                        }
                    }
                }
                else {
                    // 3b. Non-Enemy Rendering
                    const FontDataComponent& data = ecsHub->GetComponent<FontDataComponent>(entity);
                    const Material& materialComponent = ecsHub->GetComponent<Material>(entity);

                    Renderer::RenderText(data.text,
                        transform.position.x,
                        transform.position.y,
                        data.scale,
                        data.font,
                        materialComponent.color,
                        layerOpacity
                    );
                }
            }

            Renderer::EndFontBatch();
            Renderer::FontFlush();
            GRAPHICS->FontShader.UnUse();
        }

        void HideObjects() override {
            for (auto entity : fontLayerObjects) {
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                transformComponent.scale = { 0.0f, 0.0f };
            }
        }

        void ShowObjects() override {
            for (auto entity : fontLayerObjects) {
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                transformComponent.scale = originalScale[entity];
            }
        }

        void UpdateOriginalScale() override {
            for (auto entity : fontLayerObjects) {
                Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                originalScale[entity] = transformComponent.scale;
            }
        }

        std::vector<Entity> enemyContainer;
        std::vector<Entity> playerContainer;
        std::vector<Entity> fontLayerObjects;
        std::unordered_map<Entity, Vector2D> originalScale;
        std::string text = "";
        bool objectsShown;
    };
}