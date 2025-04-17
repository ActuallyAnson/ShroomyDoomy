/******************************************************************************/
/*!
\file    ScriptGlue.cpp
\project FruitLoops
\author  Benjamin Lau
\date    Dec 24, 2024
\brief   This file contains the implementation of the ScriptGlue class,
         which facilitates communication between the C++ game engine and
         the C# scripting environment. It provides internal calls for
         entity, transform, input, and component management functionalities.

All content � 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
#include "pch.h"
#include "DebugMacros.h"
#include "ScriptGlue.h"
#include "ScriptSystem.h"
#include "Core.h"
#include "Transform.h"
#include "TileComponent.h"
#include "ScriptComponent.h"
#include "Vector2D.h"
#include "Input.h"
#include "Factory.h"
#include "ComponentManager.h"
#include "ECSHub.h"
#include "Material.h"

namespace FruitLoops {

#define FL_ADD_INTERNAL_CALL(Name) mono_add_internal_call("FruitLoops.InternalCalls::" #Name, Name)

    /*---------------------------ENTITY---------------------------*/

    static MonoObject* GetScriptInstance(Entity entityID)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        return ScriptSystem::GetManagedInstance(std::to_string(entityID));
    }

    static bool Entity_HasECSComponent(Entity entityID, MonoReflectionType* componentType)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        MonoType* managedType = mono_reflection_type_get_type(componentType);
        if (!managedType)
            throw std::runtime_error("Invalid component type provided");

        const char* rawTypeName = mono_type_get_name(managedType);
        std::string typeName(rawTypeName);
        mono_free(static_cast<void*>(const_cast<char*>(rawTypeName)));
        //spdlog::info("Checking registration for component type '{}'", typeName);

        if (!ecsHub->GetComponentManager().HasComponentType(typeName)) {
            FL_DEBUG_WARN("Component type '{}' is not registered.", typeName);
            return false;
        }

        if (!ecsHub->HasComponentByName(entityID, typeName)) {
            FL_DEBUG_WARN("Entity {} does not have a component of type '{}'.", entityID, typeName);
            return false;
        }

        return true;
    }

    static MonoObject* Entity_GetECSComponent(Entity entityID, MonoReflectionType* componentType)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        MonoType* monoType = mono_reflection_type_get_type(componentType);
        if (!monoType)
            throw std::runtime_error("Invalid MonoType provided");

        const char* rawTypeName = mono_type_get_name(monoType);
        std::string typeName(rawTypeName);
        mono_free(static_cast<void*>(const_cast<char*>(rawTypeName)));


#ifdef DEBUG

        spdlog::info("Attempting to retrieve ECS component '{}'", typeName);
#endif // DEBUG

        // Directly use the provided type name
        if (!ecsHub->HasComponentByName(entityID, typeName)) {
#ifdef DEBUG

            spdlog::warn("Entity {} does not have a component of type '{}'", entityID, typeName);
#endif // DEBUG
            return nullptr;
        }

        void* componentInstance = ecsHub->GetComponentManager().GetComponentByName(entityID, typeName);
        if (!componentInstance) {
#ifdef DEBUG

            spdlog::error("Failed to retrieve component '{}' for entity {}", typeName, entityID);
#endif // DEBUG
            return nullptr;
        }

        MonoClass* monoClass = mono_class_from_mono_type(monoType);
        if (!monoClass) {
#ifdef DEBUG

            spdlog::error("Failed to retrieve MonoClass for type '{}'", typeName);
#endif // DEBUG
            return nullptr;
        }

        MonoObject* monoObject = mono_object_new(mono_domain_get(), monoClass);
        if (!monoObject) {
#ifdef DEBUG

            spdlog::error("Failed to create MonoObject for component '{}'", typeName);
#endif // DEBUG
            return nullptr;
        }
#ifdef DEBUG

        spdlog::info("Successfully retrieved component '{}' for entity {}", typeName, entityID);
#endif // DEBUG

        return monoObject;
    }

    static bool Entity_HasECSComponentByName(Entity entityID, MonoString* componentTypeName)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        char* componentTypeCStr = mono_string_to_utf8(componentTypeName);
        std::string typeName(componentTypeCStr);
        mono_free(componentTypeCStr);

        //spdlog::info("Checking registration for component type '{}'", typeName);

        if (!ecsHub->GetComponentManager().HasComponentType(typeName)) {
#ifdef DEBUG

            spdlog::warn("Component type '{}' is not registered.", typeName);
#endif // DEBUG
            return false;
        }

        if (!ecsHub->HasComponentByName(entityID, typeName)) {
#ifdef DEBUG

            spdlog::warn("Entity {} does not have a component of type '{}'.", entityID, typeName);
#endif // DEBUG
            return false;
        }

        return true;
    }

    static MonoObject* Entity_GetECSComponentByName(uint64_t entityID, MonoString* componentTypeName)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        char* componentTypeCStr = mono_string_to_utf8(componentTypeName);
        std::string typeName(componentTypeCStr);
        mono_free(componentTypeCStr);
#ifdef DEBUG

        spdlog::info("Entity_GetECSComponentByName called for type '{}'", typeName);
#endif // DEBUG

        if (!ecsHub->HasComponentByName(static_cast<Entity>(entityID), typeName))
        {
#ifdef DEBUG

            spdlog::warn("Entity {} does not have a component of type '{}'.", static_cast<Entity>(entityID), typeName);
#endif // DEBUG
            return nullptr;
        }

        void* componentInstance = ecsHub->GetComponentManager().GetComponentByName(static_cast<Entity>(entityID), typeName);
        if (!componentInstance)
        {
#ifdef DEBUG

            spdlog::error("Failed to retrieve component '{}' for entity {}", typeName, static_cast<Entity>(entityID));
#endif // DEBUG
            return nullptr;
        }

        // Retrieve the MonoImage from the core assembly
        MonoImage* coreImage = ScriptSystem::GetCoreAssemblyImage();
        if (!coreImage)
        {
#ifdef DEBUG

            spdlog::error("Failed to retrieve core assembly image.");
#endif // DEBUG
            return nullptr;
        }

        if (typeName.rfind("struct ", 0) == 0) {
            typeName = typeName.substr(7); // Remove "struct "
        }
        if (typeName.rfind("FruitLoops::", 0) == 0) {
            typeName = typeName.substr(11); // Remove "FruitLoops::"
        }
        while (!typeName.empty() && typeName[0] == ':') {
            typeName = typeName.substr(1);
        }

        // Get the MonoClass from the MonoImage
        MonoClass* monoClass = mono_class_from_name(coreImage, "FruitLoops", typeName.c_str());
        if (!monoClass)
        {
#ifdef DEBUG

            spdlog::error("Failed to retrieve MonoClass for type '{}'", typeName);
#endif // DEBUG
            return nullptr;
        }

        // Create a new MonoObject for the component
        MonoObject* monoObject = mono_object_new(mono_domain_get(), monoClass);
        if (!monoObject)
        {
#ifdef DEBUG

            spdlog::error("Failed to create MonoObject for component '{}'", typeName);
#endif // DEBUG
            return nullptr;
        }
#ifdef DEBUG

        spdlog::info("Successfully retrieved component '{}' for entity {}", typeName, static_cast<Entity>(entityID));
#endif // DEBUG

        return monoObject;
    }

    static MonoArray* Entity_GetEntitiesWithECSComponentName(MonoString* componentTypeName)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        char* componentTypeCStr = mono_string_to_utf8(componentTypeName);
        std::string typeName(componentTypeCStr);
        mono_free(componentTypeCStr);

        //spdlog::info("Entity_GetEntitiesWithECSComponentName called for type '{}'", typeName);

        // Directly use the provided type name
        auto entities = ecsHub->GetEntitiesWithComponentName(typeName);
        //spdlog::info("Found {} entities with component type '{}'", entities.size(), typeName);

        MonoClass* ulongClass = mono_get_uint64_class();
        MonoArray* resultArray = mono_array_new(mono_domain_get(), ulongClass, entities.size());

        for (size_t i = 0; i < entities.size(); ++i) {
            mono_array_set(resultArray, uint64_t, i, static_cast<uint64_t>(entities[i]));
        }
        return resultArray;
    }

    static void Entity_AddECSComponent(uint64_t entityID, MonoString* componentTypeName)
    {
        char* componentTypeCStr = mono_string_to_utf8(componentTypeName);
        std::string componentType(componentTypeCStr);
        mono_free(componentTypeCStr);

        if (componentType == "ButtonComponent") {
#ifdef DEBUG

            spdlog::warn("Component '{}' is a local-only component and cannot be added to the ECS.", componentType);
#endif // DEBUG
            return;
        }

        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        ecsHub->GetComponentManager().AddComponentByName((Entity)entityID, componentType);
#ifdef DEBUG

        spdlog::info("Added component '{}' to entity {}.", componentType, (Entity)entityID);
#endif // DEBUG
    }

	static uint64_t Entity_GetEntityByName(MonoString* entityName)
	{
		if (!ecsHub)
			throw std::runtime_error("ECSHub not initialized");
		char* entityNameCStr = mono_string_to_utf8(entityName);
		std::string name(entityNameCStr);
		mono_free(entityNameCStr);
        if (factory->GetEntityByName(name).empty()) {
            //std::cout << "Trying to get entity with name" + name + "but not found" << std::endl;
            return 0;
        }
		return static_cast<uint64_t>(factory->GetEntityByName(name)[0]);
	}

    static void Entity_SetScriptComponentName(uint64_t entityID, MonoString* name) {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        // Convert MonoString to std::string
        char* nameCStr = mono_string_to_utf8(name);
        std::string componentName(nameCStr);
        mono_free(nameCStr);

        // Access the ECS system to set the ScriptComponent's name
        auto& scriptComponent = ecsHub->GetComponentManager().GetComponent<ScriptComponent>((Entity)entityID);
        scriptComponent.Name = componentName;
    }

    static MonoString* Entity_GetEntityName(uint64_t entityID)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");


        auto entity = static_cast<Entity>(entityID);
        if (!factory->HasEntity(entity))
            throw std::runtime_error("Entity not found");


        std::string entityName = factory->GetEntityType(entity);
        return mono_string_new(mono_domain_get(), entityName.c_str());
    }

    
    static uint64_t Entity_GetEntityByTextureName(MonoString* textureNameMono)
    {
        if (!ecsHub || !factory) {
            return 0;  // Return 0 (invalid entity) if systems not initialized
        }

        // Convert MonoString to std::string
        char* nameCStr = mono_string_to_utf8(textureNameMono);
        std::string textureName(nameCStr);
        mono_free(nameCStr);

        // Get all game objects and check their materials
        auto gameObjects = factory->GetAllGameObjects();
        for (const auto& [entity, type] : gameObjects) {
            if (ecsHub->HasComponent<Material>(entity)) {
                const auto& material = ecsHub->GetComponent<Material>(entity);

                // Check if texture name matches
                if (material.textureName == textureName) {
                    return static_cast<uint64_t>(entity);
                }
            }
        }

        return 0;  // Return 0 if no entity found
    }

    /*---------------------------TRANSFORM---------------------------*/

    static void Transform_GetTranslation(Entity entityID, Vector2D* outTranslation)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<Transform>(entityID))
            throw std::runtime_error("Entity " + std::to_string(entityID) + "does not have a Transform component");

        const auto& transform = ecsHub->GetComponent<Transform>(entityID);
        *outTranslation = transform.position;
    }

    static void Transform_SetTranslation(Entity entityID, const Vector2D* newTranslation)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<Transform>(entityID))
            throw std::runtime_error("Entity " + std::to_string(entityID) + "does not have a Transform component");

        auto& transform = ecsHub->GetComponent<Transform>(entityID);

        // Just set the position directly (no local loop, no partial stepping).
        transform.position = *newTranslation;


        //// Debug log for verification
        //std::cout << "[Transform_SetTranslation] Entity ID: " << entityID
        //    << ", New Position: X: " << newTranslation->x
        //    << ", Y: " << newTranslation->y << std::endl;
    }

    static void Transform_GetScale(Entity entityID, Vector2D* outScale)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<Transform>(entityID))
            throw std::runtime_error("Entity " + std::to_string(entityID) + "does not have a Transform component");

        const auto& transform = ecsHub->GetComponent<Transform>(entityID);
        *outScale = transform.scale;
    }




    static void Transform_SetScale(Entity entityID, const Vector2D* newScale)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<Transform>(entityID))
            throw std::runtime_error("Entity does not have a Transform component");

        auto& transform = ecsHub->GetComponent<Transform>(entityID);
        transform.scale = *newScale;
    }

    static void Transform_GetOrientation(Entity entityID, Vector2D* outOrientation)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<Transform>(entityID))
            throw std::runtime_error("Entity does not have a Transform component");

        const auto& transform = ecsHub->GetComponent<Transform>(entityID);
        *outOrientation = transform.orientation;
    }

    static void Transform_SetOrientation(Entity entityID, const Vector2D* newOrientation)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<Transform>(entityID))
            throw std::runtime_error("Entity does not have a Transform component");

        auto& transform = ecsHub->GetComponent<Transform>(entityID);
        transform.orientation = *newOrientation;
    }

    /*---------------------------INPUT---------------------------*/

    // Check if the mouse is over a specific entity
    static bool Input_IsMouseOverEntity(uint64_t entityID)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<Transform>(static_cast<Entity>(entityID)))
            throw std::runtime_error("Entity does not have a Transform component");

        const auto& transform = ecsHub->GetComponent<Transform>(static_cast<Entity>(entityID));
        Vector2D cursorPos = inputSystem->GetCursorPosition();

        // Assuming the entity has a bounding box (use its scale for size)
        Vector2D scale = transform.scale;
        Vector2D position = transform.position;

        // Check if the cursor is within the bounding box
        return (cursorPos.x >= position.x - scale.x / 2 && cursorPos.x <= position.x + scale.x / 2 &&
            cursorPos.y >= position.y - scale.y / 2 && cursorPos.y <= position.y + scale.y / 2);
    }

    // Check if a specific mouse button is down
    static bool Input_IsMouseButtonDown(int button)
    {
        if (!inputSystem)
            throw std::runtime_error("InputSystem not initialized");

        return inputSystem->mouseButtonStates[button];
    }

    static bool Input_IsKeyPressed(int keyCode)
    {
        if (!inputSystem)
            throw std::runtime_error("InputSystem not initialized");
        return inputSystem->IsKeyPressed(keyCode);
    }

    static bool Input_IsKeyReleased(int keyCode)
    {
        if (!inputSystem)
            throw std::runtime_error("InputSystem not initialized");
        return inputSystem->IsKeyReleased(keyCode);
    }

    static bool Input_IsKeyHeld(int keyCode)
    {
        if (!inputSystem)
            throw std::runtime_error("InputSystem not initialized");
        return inputSystem->IsKeyHeld(keyCode);
    }

    static bool Input_IsAnyMouseButtonPressed()
    {
        if (!inputSystem)
            throw std::runtime_error("InputSystem not initialized");
        return inputSystem->IsAnyMouseButtonPressed();
    }

    static void Input_GetCursorPosition(Vector2D* outCursorPosition)
    {
        if (!inputSystem)
            throw std::runtime_error("InputSystem not initialized");
        *outCursorPosition = inputSystem->GetCursorPosition();
    }

    static int Input_GetMouseScrollY()
    {
        if (!inputSystem)
            throw std::runtime_error("InputSystem not initialized");
        return inputSystem->mouseScrollY;
    }

    /*---------------------------BUTTON COMPONENT---------------------------*/

    static bool ButtonComponent_GetIsHovered(Entity entityID)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<ButtonComponent>(static_cast<Entity>(entityID)))
            throw std::runtime_error("Entity does not have a ButtonComponent");

        const auto& button = ecsHub->GetComponent<ButtonComponent>(static_cast<Entity>(entityID));
        return button.isHovered;
    }

    static void ButtonComponent_SetIsHovered(Entity entityID, bool isHovered)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<ButtonComponent>(static_cast<Entity>(entityID)))
            throw std::runtime_error("Entity does not have a ButtonComponent");

        auto& button = ecsHub->GetComponent<ButtonComponent>(static_cast<Entity>(entityID));
        button.isHovered = isHovered;
    }

    static bool ButtonComponent_GetIsClicked(Entity entityID)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<ButtonComponent>(static_cast<Entity>(entityID)))
            throw std::runtime_error("Entity does not have a ButtonComponent");

        const auto& button = ecsHub->GetComponent<ButtonComponent>(static_cast<Entity>(entityID));
        return button.isClicked;
    }

    static void ButtonComponent_SetIsClicked(Entity entityID, bool isClicked)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<ButtonComponent>(static_cast<Entity>(entityID)))
            throw std::runtime_error("Entity does not have a ButtonComponent");

        auto& button = ecsHub->GetComponent<ButtonComponent>(static_cast<Entity>(entityID));
        button.isClicked = isClicked;
    }

    static bool ButtonComponent_GetIsEnabled(Entity entityID)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<ButtonComponent>(static_cast<Entity>(entityID)))
            throw std::runtime_error("Entity does not have a ButtonComponent");

        const auto& button = ecsHub->GetComponent<ButtonComponent>(static_cast<Entity>(entityID));
        return button.isEnabled;
    }

    static void ButtonComponent_SetIsEnabled(Entity entityID, bool isEnabled)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<ButtonComponent>(static_cast<Entity>(entityID)))
            throw std::runtime_error("Entity does not have a ButtonComponent");

        auto& button = ecsHub->GetComponent<ButtonComponent>(static_cast<Entity>(entityID));
        button.isEnabled = isEnabled;
    }

    /*---------------------------TILE COMPONENT---------------------------*/

    static bool TileComponent_GetIsWalkable(Entity entityID)
    {
        if (entityID == 0)
            throw std::runtime_error("Invalid Entity ID: 0");

        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<TileComponent>(static_cast<Entity>(entityID)))
            throw std::runtime_error("Entity does not have a TileComponent");

        const auto& tileData = ecsHub->GetComponent<TileComponent>(static_cast<Entity>(entityID));
        return tileData.IsWalkable();
    }

    static void TileComponent_SetIsWalkable(Entity entityID, bool isWalkable)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<TileComponent>(static_cast<Entity>(entityID)))
            throw std::runtime_error("Entity does not have a TileComponent");

        auto& tileData = ecsHub->GetComponent<TileComponent>(static_cast<Entity>(entityID));
        tileData.SetWalkable(isWalkable);
    }

    static bool TileComponent_GetIsHighlighted(Entity entityID)
    {
        if (entityID == 0)
            throw std::runtime_error("Invalid Entity ID: 0");

        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<TileComponent>(static_cast<Entity>(entityID)))
            throw std::runtime_error("Entity does not have a TileComponent");

        const auto& tileData = ecsHub->GetComponent<TileComponent>(static_cast<Entity>(entityID));
        return tileData.IsHighlighted();
    }

    static void TileComponent_SetIsHighlighted(Entity entityID, bool isHighlighted)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<TileComponent>(static_cast<Entity>(entityID)))
            throw std::runtime_error("Entity does not have a TileComponent");

        auto& tileData = ecsHub->GetComponent<TileComponent>(static_cast<Entity>(entityID));
        tileData.SetHighlighted(isHighlighted);
    }

    static bool TileComponent_GetIsHighlightedEnemy(Entity entityID)
    {
        if (entityID == 0)
            throw std::runtime_error("Invalid Entity ID: 0");

        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<TileComponent>(static_cast<Entity>(entityID)))
            throw std::runtime_error("Entity does not have a TileComponent");

        const auto& tileData = ecsHub->GetComponent<TileComponent>(static_cast<Entity>(entityID));
        return tileData.IsHighlightedEnemy();
    }

    static void TileComponent_SetIsHighlightedEnemy(Entity entityID, bool isHighlightedEnemy)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<TileComponent>(static_cast<Entity>(entityID)))
            throw std::runtime_error("Entity does not have a TileComponent");

        auto& tileData = ecsHub->GetComponent<TileComponent>(static_cast<Entity>(entityID));
        tileData.SetHighlightedEnemy(isHighlightedEnemy);
    }

    static void TileComponent_GetPosition(Entity entityID, Vector2D* outPosition)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<TileComponent>(static_cast<Entity>(entityID)))
            throw std::runtime_error("Entity does not have a TileComponent");

        const auto& tileData = ecsHub->GetComponent<TileComponent>(static_cast<Entity>(entityID));
        *outPosition = tileData.position;
    }

    static void TileComponent_SetPosition(Entity entityID, const Vector2D* newPosition)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<TileComponent>(static_cast<Entity>(static_cast<Entity>(entityID))))
            throw std::runtime_error("Entity does not have a TileComponent");

        auto& tileData = ecsHub->GetComponent<TileComponent>(static_cast<Entity>(static_cast<Entity>(entityID)));
        tileData.position = *newPosition;
    }

    static float TileComponent_GetMovementCost(Entity entityID)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<TileComponent>(static_cast<Entity>(entityID)))
            throw std::runtime_error("Entity does not have a TileComponent");

        const auto& tileData = ecsHub->GetComponent<TileComponent>(static_cast<Entity>(entityID));
        return tileData.movementCost;
    }

    static void TileComponent_SetMovementCost(Entity entityID, float cost)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<TileComponent>(static_cast<Entity>(entityID)))
            throw std::runtime_error("Entity does not have a TileComponent");

        auto& tileData = ecsHub->GetComponent<TileComponent>(static_cast<Entity>(entityID));
        tileData.movementCost = cost;
    }

    static void TileComponent_ClearNeighbors(Entity entityID)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<TileComponent>(static_cast<Entity>(entityID)))
            throw std::runtime_error("Entity does not have a TileComponent");

        auto& tileData = ecsHub->GetComponent<TileComponent>(static_cast<Entity>(entityID));
        tileData.ClearNeighbors();
    }

    static void TileComponent_AddNeighbor(Entity entityID, Entity neighborID)
    {
        if (entityID == 0 || neighborID == 0)
            throw std::runtime_error("Invalid Entity ID or Neighbor ID: 0");

        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<TileComponent>(static_cast<Entity>(entityID)))
            throw std::runtime_error("Entity " + std::to_string(static_cast<Entity>(entityID)) + " does not have a TileComponent");

        if (!ecsHub->HasComponent<TileComponent>(neighborID))
            throw std::runtime_error("Neighbor Entity " + std::to_string(neighborID) + " does not have a TileComponent");

        auto& tileData = ecsHub->GetComponent<TileComponent>(static_cast<Entity>(entityID));
        auto& neighborData = ecsHub->GetComponent<TileComponent>(neighborID);

        tileData.AddNeighbor(&neighborData);
    }

    static MonoArray* TileComponent_GetNeighbors(Entity entityID) {
        if (entityID == 0) {
            throw std::runtime_error("[TileComponent_GetNeighbors] Invalid Entity ID: 0");
        }

        if (!ecsHub) {
            throw std::runtime_error("[TileComponent_GetNeighbors] ECSHub not initialized");
        }

        if (!ecsHub->HasComponent<TileComponent>(static_cast<Entity>(entityID))) {
            throw std::runtime_error("[TileComponent_GetNeighbors] Entity does not have a TileComponent");
        }

        const auto& tileData = ecsHub->GetComponent<TileComponent>(static_cast<Entity>(entityID));
        const auto& neighbors = tileData.GetNeighbors();

        MonoClass* ulongClass = mono_get_uint64_class();
        MonoArray* resultArray = mono_array_new(mono_domain_get(), ulongClass, neighbors.size());

        size_t validCount = 0;

        for (const auto* neighbor : neighbors) {
            if (!neighbor) continue;

            auto neighborEntity = ecsHub->GetEntityByComponent<TileComponent>(neighbor); // Direct mapping
            if (neighborEntity == INVALID_ENTITY) continue;

            mono_array_set(resultArray, uint64_t, validCount++, static_cast<uint64_t>(neighborEntity));
        }

        // Resize if needed
        if (validCount < neighbors.size()) {
            MonoArray* resizedArray = mono_array_new(mono_domain_get(), ulongClass, validCount);
            for (size_t i = 0; i < validCount; ++i) {
                mono_array_set(resizedArray, uint64_t, i, mono_array_get(resultArray, uint64_t, i));
            }
            return resizedArray;
        }

        return resultArray;
    }

    static uint64_t TileComponent_GetOccupant(uint64_t entityID)
    {
        if (entityID == 0)
            throw std::runtime_error("Invalid Entity ID: 0");

        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        const auto& tile = ecsHub->GetComponentManager().GetComponent<TileComponent>(static_cast<Entity>(entityID));
        if (tile.occupant == 0)
        {
            return 0; // No occupant
        }
        return static_cast<uint64_t>(tile.occupant);
    }

    static void TileComponent_SetOccupant(uint64_t entityID, uint64_t occupantID)
    {
        // Debug logs to check input values
        //FL_DEBUG_INFO("[TileComponent_SetOccupant] Setting occupant. Tile ID: {}, Occupant ID: {}", entityID, occupantID);

        if (entityID == 0)
            FL_DEBUG_ERROR("[TileComponent_SetOccupant] Invalid Entity {}", entityID);

        if (occupantID == 0)
            FL_DEBUG_ERROR("[TileComponent_SetOccupant] Invalid Occupant ID {}", occupantID);

        if (!ecsHub)
            FL_DEBUG_ERROR("[TileComponent_SetOccupant] ECSHub not initialized.");

        auto& tile = ecsHub->GetComponentManager().GetComponent<TileComponent>((Entity)entityID);
        tile.occupant = static_cast<Entity>(occupantID);

        //FL_DEBUG_INFO("[TileComponent_SetOccupant] Successfully set tile occupant. Tile ID: {}, Occupant ID: {}", entityID, occupantID);
    }

    void TileComponent_SetHighlighted(Entity entityID, bool highlighted)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        if (!ecsHub->HasComponent<TileComponent>(static_cast<Entity>(entityID)))
            throw std::runtime_error("Entity does not have a TileComponent");

        auto& tileData = ecsHub->GetComponent<TileComponent>(static_cast<Entity>(entityID));
        tileData.SetHighlighted(highlighted);
    }

    /*---------------------------UNIT COMPONENT---------------------------*/

    // Get UnitType as string
    static MonoString* UnitComponent_GetUnitType(uint64_t entityID)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        std::string unitType = (unit.unitType == FruitLoops::UnitType::Player) ? "Player" : "Enemy";
        return mono_string_new(mono_domain_get(), unitType.c_str());
    }

    // Set UnitType from string
    static void UnitComponent_SetUnitType(uint64_t entityID, MonoString* monoUnitType)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        char* unitTypeCStr = mono_string_to_utf8(monoUnitType);
        std::string unitType(unitTypeCStr);
        mono_free(unitTypeCStr);

        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        unit.unitType = (unitType == "Player") ? FruitLoops::UnitType::Player : FruitLoops::UnitType::Enemy;
    }

    // Get/Set Health
    static int UnitComponent_GetHealth(uint64_t entityID)
    {
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        return unit.health;
    }

    static void UnitComponent_SetHealth(uint64_t entityID, int health)
    {
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        unit.health = health;
    }

    // Get/Set MaxHealth
    static int UnitComponent_GetMaxHealth(uint64_t entityID)
    {
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        return unit.maxHealth;
    }

    static void UnitComponent_SetMaxHealth(uint64_t entityID, int maxHealth)
    {
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        unit.maxHealth = maxHealth;
    }

    // Get/Set AttackPower
    static int UnitComponent_GetAttackPower(uint64_t entityID)
    {
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        return unit.attackPower;
    }

    static void UnitComponent_SetAttackPower(uint64_t entityID, int attackPower)
    {
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        unit.attackPower = attackPower;
    }

    // Get/Set Energy
    static int UnitComponent_GetEnergy(uint64_t entityID)
    {
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        return unit.energy;
    }

    static void UnitComponent_SetEnergy(uint64_t entityID, int energy)
    {
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        unit.energy = energy;
    }

    // Get/Set MaxEnergy
    static int UnitComponent_GetMaxEnergy(uint64_t entityID)
    {
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        return unit.maxEnergy;
    }

    static void UnitComponent_SetMaxEnergy(uint64_t entityID, int maxEnergy)
    {
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        unit.maxEnergy = maxEnergy;
    }

    // Get/Set Position
    static void UnitComponent_GetPosition(uint64_t entityID, Vector2D* outPosition)
    {
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        *outPosition = unit.position;
    }

    static void UnitComponent_SetPosition(uint64_t entityID, const Vector2D* newPosition)
    {
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        unit.position = *newPosition;
    }

    // Get/Set IsAlive
    static bool UnitComponent_GetIsAlive(uint64_t entityID)
    {
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        return unit.isAlive;
    }

    static void UnitComponent_SetIsAlive(uint64_t entityID, bool isAlive)
    {
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        unit.isAlive = isAlive;
    }

    static bool UnitComponent_GetIsMoving(uint64_t entityID)
    {
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        return unit.isMoving;
    }

    static void UnitComponent_SetIsMoving(uint64_t entityID, bool isMoving)
    {
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        unit.isMoving = isMoving;
    }

    static int UnitComponent_GetSpeed(uint64_t entityID)
    {
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        return unit.speed;
    }    
    static void UnitComponent_SetSpeed (uint64_t entityID, int speed)
    {
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        unit.speed = speed;
    }
    

    // Get/Set OccupiedTile
    static uint64_t UnitComponent_GetOccupiedTile(uint64_t entityID)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        // Access the UnitComponent of the entity
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);

        // Return the occupied tile directly (already an Entity ID)
        return static_cast<uint64_t>(unit.occupiedTile);
    }

    static void UnitComponent_SetOccupiedTile(uint64_t entityID, uint64_t tileEntityID)
    {
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        unit.occupiedTile = FruitLoops::Entity(tileEntityID);
    }

    // Reset OccupiedTile
    static void UnitComponent_ResetOccupiedTile(uint64_t entityID)
    {
        auto& unit = ecsHub->GetComponentManager().GetComponent<UnitComponent>((Entity)entityID);
        unit.ResetOccupiedTile();
    }


    /*---------------------------ANIMATOR FUNCTIONS---------------------------*/
    static void AnimationComponent_PlayAnimation(Entity entity, MonoString* animationName) {

        char* nameCStr = mono_string_to_utf8(animationName);
        std::string textureName(nameCStr);
        mono_free(nameCStr);
        for (auto& animations : GRAPHICS->animatorContainer) {
            if (animations.GetTextureName() == textureName) {
                std::vector<Entity> entityBucket = animations.GetEntity();
                if (std::find(entityBucket.begin(), entityBucket.end(), entity) == entityBucket.end())
                    animations.AddEntity(entity);
                animations.ResetAnimation();
                animations.SetPlaying(true);
            }
        }
    }

    static void AnimationComponent_ChangeAnimationTexture(MonoString* originalName, MonoString* newName) {

        char* nameCStr = mono_string_to_utf8(originalName);
        std::string originaTextureName(nameCStr);
        mono_free(nameCStr);

        nameCStr = mono_string_to_utf8(newName);
        std::string newTextureName(nameCStr);
        mono_free(nameCStr);

        for (auto& animations : GRAPHICS->animatorContainer) {
            if (animations.GetTextureName() == originaTextureName) {
                animations.SetTextureName(newTextureName);
            }
        }
        return;
    }


    static MonoString* AnimationComponent_GetAnimationName(int index, Entity entity) {

        AnimationComponent& animComponent = ecsHub->GetComponent<AnimationComponent>(entity);
        std::vector names = animComponent.AnimationName;
        return  mono_string_new(mono_domain_get(), names[index].c_str());
    }

    static int AnimationComponent_GetAnimationNameSize(Entity entity) {

        AnimationComponent& animComponent = ecsHub->GetComponent<AnimationComponent>(entity);
        std::vector names = animComponent.AnimationName;
        return static_cast<int>(names.size());
    }

    static void AnimationComponent_Stop(MonoString* animationName) {

        char* nameCStr = mono_string_to_utf8(animationName);
        std::string textureName(nameCStr);
        mono_free(nameCStr);

        for (auto& animations : GRAPHICS->animatorContainer) {
            if (textureName == animations.GetTextureName()) {
                animations.SetPlaying(false);
                animations.ResetAnimation();
            }
        }
    }

    static bool AnimationComponent_CheckEntityPlaying(Entity targetEntity) {

        for (auto& animations : GRAPHICS->animatorContainer) {
            if (animations.GetTextureName() == "PapaShroom Idle"
                || animations.GetTextureName() == "Enemy Idle") {
                continue;
            }
            for (Entity entity : animations.GetEntity()) {
                if (entity == targetEntity && animations.GetPlaying()) {
                    return true;
                }
            }
        }
        return false;
    }

    static void AnimationComponent_RemoveEntityFromAnimation(Entity entity, MonoString* animationName) {
        char* nameCStr = mono_string_to_utf8(animationName);
        std::string textureName(nameCStr);
        mono_free(nameCStr);

        
        // Remove from normal animation container
        for (auto& animations : GRAPHICS->animatorContainer)
        {
            if (animations.GetTextureName() == textureName) {
                animations.DelEntity(entity);
            }
        }
    }

    /*---------------------------INVENTORY FUNCTIONS---------------------------*/

    void SetEntityTextureOne(MonoString* textureNameMono)
    {
        if (!factory) {
            std::cerr << "[ScriptGlue] ERROR: GameObjectFactory is not initialized!\n";
            return;
        }

        // Convert MonoString to std::string
        char* nameCStr = mono_string_to_utf8(textureNameMono);
        std::string textureName(nameCStr);
        mono_free(nameCStr);  // Free the allocated string from Mono

        // Track entity that is assigned to Slot 1
        static int slot1Entity = -1;

        auto gameObjects = factory->GetAllGameObjects();

        if (slot1Entity == -1) { // First time assigning
            for (const auto& [entity, type] : gameObjects) {
                if (ecsHub->HasComponent<Material>(entity)) {
                    Material& material = ecsHub->GetComponent<Material>(entity);

                    if (material.textureName == "inventory_slot_1") {
                        slot1Entity = entity; // Store entity ID
                        material.textureName = textureName;
                        std::cout << "[ScriptGlue] Assigned entity " << entity << " to Slot 1 with texture '" << textureName << "'\n";
                        return;
                    }
                }
            }
            //std::cerr << "[ScriptGlue] ERROR: No entity found for Slot 1!\n";
        }
        else { // Update existing slot entity
            if (ecsHub->HasComponent<Material>(slot1Entity)) {
                Material& material = ecsHub->GetComponent<Material>(slot1Entity);
                material.textureName = textureName;
                return;
            }
            else {
                std::cerr << "[ScriptGlue] ERROR: Slot 1 entity lost material component!\n";
                slot1Entity = -1;
            }
        }
    }


    void SetEntityTextureTwo(MonoString* textureNameMono)
    {
        if (!factory) {
            std::cerr << "[ScriptGlue] ERROR: GameObjectFactory is not initialized!\n";
            return;
        }

        // Convert MonoString to std::string
        char* nameCStr = mono_string_to_utf8(textureNameMono);
        std::string textureName(nameCStr);
        mono_free(nameCStr);  // Free the allocated string from Mono

        // Track entity that is assigned to Slot 2
        static int slot2Entity = -1;

        auto gameObjects = factory->GetAllGameObjects();

        if (slot2Entity == -1) { // First time assigning
            for (const auto& [entity, type] : gameObjects) {
                if (ecsHub->HasComponent<Material>(entity)) {
                    Material& material = ecsHub->GetComponent<Material>(entity);

                    if (material.textureName == "inventory_slot_2") {
                        slot2Entity = entity; // Store entity ID
                        material.textureName = textureName;
                        return;
                    }
                }
            }
            //std::cerr << "[ScriptGlue] ERROR: No entity found for Slot 2!\n";
        }
        else { // Update existing slot entity
            if (ecsHub->HasComponent<Material>(slot2Entity)) {
                Material& material = ecsHub->GetComponent<Material>(slot2Entity);
                material.textureName = textureName;
                return;
            }
            else {
                //std::cerr << "[ScriptGlue] ERROR: Slot 2 entity lost material component!\n";
                slot2Entity = -1;
            }
        }
    }


    void SetEntityTextureOutput(MonoString* textureNameMono)
    {
        if (!factory) {
            std::cerr << "[ScriptGlue] ERROR: GameObjectFactory is not initialized!\n";
            return;
        }

        // Convert MonoString to std::string
        char* nameCStr = mono_string_to_utf8(textureNameMono);
        std::string textureName(nameCStr);
        mono_free(nameCStr);  // Free the allocated string from Mono

        // Track entity that is assigned to Slot 2
        static int SetEntityTextureOutput = -1;

        auto gameObjects = factory->GetAllGameObjects();

        if (SetEntityTextureOutput == -1) { // First time assigning
            for (const auto& [entity, type] : gameObjects) {
                if (ecsHub->HasComponent<Material>(entity)) {
                    Material& material = ecsHub->GetComponent<Material>(entity);

                    if (material.textureName == "inventory_output") {
                        SetEntityTextureOutput = entity; // Store entity ID
                        material.textureName = textureName;
                        return;
                    }
                }
            }
            //std::cerr << "[ScriptGlue] ERROR: No entity found for Slot 2!\n";
        }
        else { // Update existing slot entity
            if (ecsHub->HasComponent<Material>(SetEntityTextureOutput)) {
                Material& material = ecsHub->GetComponent<Material>(SetEntityTextureOutput);
                material.textureName = textureName;
                //std::cout << "[ScriptGlue] Updated Output Slot entity " << SetEntityTextureOutput << " texture to '" << textureName << "'\n";
                return;
            }
            else {
                std::cerr << "[ScriptGlue] ERROR: Output Slot entity lost material component!\n";
                SetEntityTextureOutput = -1;
            }
        }
    }

    void SetEntityTextureCook(MonoString* textureNameMono)
    {
        if (!factory) {
            std::cerr << "[ScriptGlue] ERROR: GameObjectFactory is not initialized!\n";
            return;
        }

        // Convert MonoString to std::string
        char* nameCStr = mono_string_to_utf8(textureNameMono);
        std::string textureName(nameCStr);
        mono_free(nameCStr);  // Free the allocated string from Mono

        // Track entity that is assigned to Slot 2
        static int SetEntityTextureOutput = -1;

        auto gameObjects = factory->GetAllGameObjects();

        if (SetEntityTextureOutput == -1) { // First time assigning
            for (const auto& [entity, type] : gameObjects) {
                if (ecsHub->HasComponent<Material>(entity)) {
                    Material& material = ecsHub->GetComponent<Material>(entity);

                    if (material.textureName == "cookStill") {
                        SetEntityTextureOutput = entity; // Store entity ID
                        material.textureName = textureName;
                        return;
                    }
                }
            }
            //std::cerr << "[ScriptGlue] ERROR: No entity found for Slot 2!\n";
        }
        else { // Update existing slot entity
            if (ecsHub->HasComponent<Material>(SetEntityTextureOutput)) {
                Material& material = ecsHub->GetComponent<Material>(SetEntityTextureOutput);
                material.textureName = textureName;
                //std::cout << "[ScriptGlue] Updated Output Slot entity " << SetEntityTextureOutput << " texture to '" << textureName << "'\n";
                return;
            }
            else {
                std::cerr << "[ScriptGlue] ERROR: Output Slot entity lost material component!\n";
                SetEntityTextureOutput = -1;
            }
        }
    }

    void SetEntityTextureEnemy(Entity entity, MonoString* textureNameMono)
    {
        if (!factory) {
            FL_DEBUG_ERROR ("[ScriptGlue] ERROR: GameObjectFactory is not initialized!\n");
            return;
        }


        // Convert MonoString to std::string
        char* nameCStr = mono_string_to_utf8(textureNameMono);
        std::string textureName(nameCStr);
        mono_free(nameCStr);  // Free the allocated string from Mono


        if (ecsHub->HasComponent<Material>(entity)) {
            Material& material = ecsHub->GetComponent<Material>(entity);
            material.textureName = textureName;
        }
        else {
            FL_DEBUG_ERROR ("[ScriptGlue] ERROR: Entity {0) does not have a Material component!\n", entity);
        }
    }

    void SetEntityChestTexture(MonoString* textureNameMono)
    {
        if (!factory) {
            std::cerr << "[ScriptGlue] ERROR: GameObjectFactory is not initialized!\n";
            return;
        }

        // Convert MonoString to std::string
        char* nameCStr = mono_string_to_utf8(textureNameMono);
        std::string textureName(nameCStr);
        mono_free(nameCStr);  // Free the allocated string from Mono

        // Track entity that is assigned to Slot 2
        static int SetEntityTextureOutput = -1;

        auto gameObjects = factory->GetAllGameObjects();

        if (SetEntityTextureOutput == -1) { // First time assigning
            for (const auto& [entity, type] : gameObjects) {
                if (ecsHub->HasComponent<Material>(entity)) {
                    Material& material = ecsHub->GetComponent<Material>(entity);

                    if (material.textureName == "treasure_chest") {
                        SetEntityTextureOutput = entity; // Store entity IDf
                        material.textureName = textureName;
                        return;
                    }
                }
            }
            //std::cerr << "[ScriptGlue] ERROR: No entity found for Slot 2!\n";
        }
        else { // Update existing slot entity
            if (ecsHub->HasComponent<Material>(SetEntityTextureOutput)) {
                Material& material = ecsHub->GetComponent<Material>(SetEntityTextureOutput);
                material.textureName = textureName;
                //std::cout << "[ScriptGlue] Updated Output Slot entity " << SetEntityTextureOutput << " texture to '" << textureName << "'\n";
                return;
            }
            else {
                std::cerr << "[ScriptGlue] ERROR: Output Slot entity lost material component!\n";
                SetEntityTextureOutput = -1;
            }
        }
    }

    void SetEntityTexture(MonoString* slotMono, MonoString* textureMono)
    {
        if (!factory) {
            std::cerr << "[ScriptGlue] ERROR: GameObjectFactory is not initialized!\n";
            return;
        }

        // Convert MonoString to std::string
        char* slotCStr = mono_string_to_utf8(slotMono);
        char* textureCStr = mono_string_to_utf8(textureMono);

        std::string slotTextureName(slotCStr);
        std::string textureName(textureCStr);

        mono_free(slotCStr);  // Free allocated string from Mono
        mono_free(textureCStr);

        // Track entity assigned to this slot
        static std::unordered_map<std::string, int> slotEntities;

        auto gameObjects = factory->GetAllGameObjects();

        // If the entity for this slot has not been assigned yet
        if (slotEntities.find(slotTextureName) == slotEntities.end()) {
            for (const auto& [entity, type] : gameObjects) {
                if (ecsHub->HasComponent<Material>(entity)) {
                    Material& material = ecsHub->GetComponent<Material>(entity);

                    if (material.textureName == slotTextureName) {
                        slotEntities[slotTextureName] = entity; // Store entity ID
                        material.textureName = textureName;
                        return;
                    }
                }
            }
            //std::cerr << "[ScriptGlue] ERROR: No entity found for slot '" << slotTextureName << "'!\n";
        }
        else { // Update existing slot entity
            int slotEntity = slotEntities[slotTextureName];
            if (ecsHub->HasComponent<Material>(slotEntity)) {
                Material& material = ecsHub->GetComponent<Material>(slotEntity);
                material.textureName = textureName;
                return;
            }
            else {
                std::cerr << "[ScriptGlue] ERROR: Slot '" << slotTextureName << "' entity lost material component!\n";
            }
        }
    }

    static void SetEntityTextureByID(uint64_t entityID, MonoString* textureMono)
    {
        if (!ecsHub) {
            std::cerr << "[ScriptGlue] ERROR: ECSHub is not initialized!\n";
            return;
        }

        if (!factory) {
            std::cerr << "[ScriptGlue] ERROR: GameObjectFactory is not initialized!\n";
            return;
        }

        // Convert MonoString to std::string
        char* textureCStr = mono_string_to_utf8(textureMono);
        std::string textureName(textureCStr);
        mono_free(textureCStr);

        // Check if the entity exists in the ECS and has a Material component
        if (!ecsHub->HasComponent<Material>(static_cast<Entity>(entityID))) {
            std::cerr << "[ScriptGlue] ERROR: Entity " << entityID << " does not have a Material component!\n";
            return;
        }

        // Retrieve the Material component and update its texture
        Material& material = ecsHub->GetComponent<Material>(static_cast<Entity>(entityID));
        material.textureName = textureName;

    }


    /*---------------------------GET ENTITY PLAYER FUNCTIONS---------------------------*/
    static uint64_t GetPlayerEntity()
    {
        if (!ecsHub)
        {
            std::cerr << "[ScriptGlue] ERROR: ECSHub not initialized!\n";
            return 0; // Return 0 if ECS is not ready
        }

        auto gameObjects = factory->GetAllGameObjects(); // Get all entities
        for (const auto& [entity, type] : gameObjects)
        {
            if (ecsHub->HasComponent<UnitComponent>(entity))
            {
                auto& unit = ecsHub->GetComponent<UnitComponent>(entity);
                if (unit.unitType == FruitLoops::UnitType::Player)
                {
                    return static_cast<uint64_t>(entity); // Found player entity
                }
            }
        }

        std::cerr << "[ScriptGlue] ERROR: Player entity not found!\n";
        return 0; // No player found
    }


    /*---------------------------ENGINE LEVEL FUNCTIONS---------------------------*/

    static void Engine_QuitGame() {
        coreEngine->GetGameState() = false;
    }

    /*---------------------------Material LEVEL FUNCTIONS---------------------------*/

    static MonoString* MaterialComponent_GetTextureType(uint64_t entityID)
    {
        if (!ecsHub)
            throw std::runtime_error("ECSHub not initialized");

        auto& material = ecsHub->GetComponentManager().GetComponent<Material>((Entity)entityID);
        std::string unitType = material.textureName;
        return mono_string_new(mono_domain_get(), unitType.c_str());
    }

    /*---------------------------LAYER MENU FUNCTIONS---------------------------*/
    static void PauseMenu_Toggle() {
        levelManager->TogglePauseMenu();
    }

    static void Layer_ChangeOpacity(float opacity) {
        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == "EndMenuLayer") {
                continue;
            }
            layer->SetOpacity(opacity);
        }
    }

    /*---------------------------LEVEL SWITCHING FUNCTIONS---------------------------*/

    static void LevelSwitch_TutorialToLevel1() {
        coreEngine->GetSetTutorialToLevel1() = true;
    }

    static bool BoolSwitch_TutorialToLevel1() {
        if (coreEngine->GetSetTutorialToLevel1() == true) {
            return true;
        }
        return false;
    }

    static void LevelSwitch_MainMenuToLevel1() {
        coreEngine->GetSetMainMenuToLevel1() = true;
    }

    static bool BoolSwitch_MainMenuToLevel1() {
        if (coreEngine->GetSetMainMenuToLevel1() == true) {
            return true;
        }
        return false;
    }

    static void LevelSwitch_ReturnMainMenu() {
        coreEngine->GetSetBackToMainMenu() = true;
    }
    static void LevelSwitch_ReloadCurrLevel() {
        coreEngine->GetSetReloadLevel() = true;
    }

    static bool BoolSwitch_ReloadCurrLevel() {
        if (coreEngine->GetSetReloadLevel() == true) {
            return true;
        }
        return false;
    }

    static void LevelSwitch_PauseMenuToMainMenu() {
        coreEngine->GetSetPauseMenuToMainMenu() = true;
    }

    static void LevelSwitch_Level1ToLevel2() {
        coreEngine->GetSetLevel1ToLevel2() = true;
    }

    static bool BoolSwitch_Level1ToLevel2() {
        if (coreEngine->GetSetLevel1ToLevel2() == true) {
            return true;
        }
        return false;
    }

    static void LevelSwitch_Level2ToLevel1() {
        coreEngine->GetSetLevel2ToLevel1() = true;
    }

    static bool BoolSwitch_Level2ToLevel1() {
        if (coreEngine->GetSetLevel2ToLevel1() == true) {
            return true;
        }
        return false;
    }


    static void LevelSwitch_MainMenuToTutorial() {
        coreEngine->GetSetMainMenuToTutorial() = true;
    }

    static int LevelSwitch_GetCurrLevel() {
        return levelManager->GetCurrLevel();
    }

    static bool LevelSwitch_GetLayerStatus(MonoString* monoLayerName) {

        // Convert MonoString to std::string
        char* nameCStr = mono_string_to_utf8(monoLayerName);
        std::string layerName(nameCStr);
        mono_free(nameCStr);  // Free the allocated string from Mono

        for (Layer* layer : coreEngine->GetLayerStack()) {
            if (layer->GetName() == layerName) {
                return layer->IsVisible();
            }
        }
        return false;
    }

    /*---------------------------AUDIO FUNCTIONS---------------------------*/

    static void Audio_PlaySound(MonoString* monoSoundName, float volume) {

        // Convert MonoString to std::string
        char* nameCStr = mono_string_to_utf8(monoSoundName);
        std::string soundName(nameCStr);
        mono_free(nameCStr);  // Free the allocated string from Mono

        audioSystem->Play_Sound(soundName, volume);

    }

    // Define internal call method to get BGM volume
    static void Audio_GetBGMVolume(float* volume)
    {
        *volume = FruitLoops::AudioSystem::BGMVolume;
    }

    // Define internal call method to set BGM volume 
    static void Audio_SetBGMVolume(float volume)
    {
        // Clamp volume between 0 and 1
        volume = std::max(0.0f, std::min(1.0f, volume));

        // Set the static volume property
        FruitLoops::AudioSystem::SetBGMVolume(volume);

        // Update any currently playing BGM channels
        for (auto& [key, channel] : FruitLoops::audioSystem->GetChannelMap())
        {
            if (key.substr(0, 3) == "BGM" && channel)
            {
                channel->setVolume(volume);
            }
        }
    }


    /*---------------------------FONT FUNCTIONS---------------------------*/

    static void Font_SetFont(Entity entity, MonoString* monoFontName, float scale) {

        // Convert MonoString to std::string
        char* nameCStr = mono_string_to_utf8(monoFontName);
        std::string fontName(nameCStr);
        mono_free(nameCStr);  // Free the allocated string from Mono

        if (ecsHub->HasComponent<FontDataComponent>(entity)) {
            FontDataComponent& fontComponent = ecsHub->GetComponent<FontDataComponent>(entity);
            fontComponent.text = fontName;
            fontComponent.scale = scale;
        }
    }

    /*---------------------------CHEST FUNCTIONS---------------------------*/
    static void SetChestOverlayTrigger(MonoString* monoStr) {
        char* nativeStr = mono_string_to_utf8(monoStr);
        std::string chestname(nativeStr);
        FruitLoops::ChestOverlayLayer::chestOverlayTrigger = chestname;
        mono_free(nativeStr);  // Free the allocated string from Mono
    }

    /*---------------------------REGISTER FUNCTIONS---------------------------*/

    void ScriptGlue::RegisterFunctions()
    {
        // Register Entity internal calls
        FL_ADD_INTERNAL_CALL(GetScriptInstance);
        FL_ADD_INTERNAL_CALL(Entity_HasECSComponent);
        FL_ADD_INTERNAL_CALL(Entity_GetECSComponent);
        FL_ADD_INTERNAL_CALL(Entity_GetECSComponentByName);
        FL_ADD_INTERNAL_CALL(Entity_HasECSComponentByName);
        FL_ADD_INTERNAL_CALL(Entity_GetEntitiesWithECSComponentName);
        FL_ADD_INTERNAL_CALL(Entity_AddECSComponent);
        FL_ADD_INTERNAL_CALL(Entity_SetScriptComponentName);
		FL_ADD_INTERNAL_CALL(Entity_GetEntityByName);
        FL_ADD_INTERNAL_CALL(Entity_GetEntityName);
        FL_ADD_INTERNAL_CALL(Entity_GetEntityByTextureName);
        
        // Register Transform internal calls
        FL_ADD_INTERNAL_CALL(Transform_GetTranslation);
        FL_ADD_INTERNAL_CALL(Transform_SetTranslation);
        FL_ADD_INTERNAL_CALL(Transform_GetScale);
        FL_ADD_INTERNAL_CALL(Transform_SetScale);
        FL_ADD_INTERNAL_CALL(Transform_GetOrientation);
        FL_ADD_INTERNAL_CALL(Transform_SetOrientation);

        // Register Input internal calls
        FL_ADD_INTERNAL_CALL(Input_IsMouseOverEntity);
        FL_ADD_INTERNAL_CALL(Input_IsMouseButtonDown);
        FL_ADD_INTERNAL_CALL(Input_IsKeyPressed);
        FL_ADD_INTERNAL_CALL(Input_IsKeyReleased);
        FL_ADD_INTERNAL_CALL(Input_IsKeyHeld);
        FL_ADD_INTERNAL_CALL(Input_IsAnyMouseButtonPressed);
        FL_ADD_INTERNAL_CALL(Input_GetCursorPosition);
        FL_ADD_INTERNAL_CALL(Input_GetMouseScrollY);

        // Register ButtonComponent internal calls
        FL_ADD_INTERNAL_CALL(ButtonComponent_GetIsHovered);
        FL_ADD_INTERNAL_CALL(ButtonComponent_SetIsHovered);
        FL_ADD_INTERNAL_CALL(ButtonComponent_GetIsClicked);
        FL_ADD_INTERNAL_CALL(ButtonComponent_SetIsClicked);
        FL_ADD_INTERNAL_CALL(ButtonComponent_GetIsEnabled);
        FL_ADD_INTERNAL_CALL(ButtonComponent_SetIsEnabled);

        // Register TileComponent internal calls
        FL_ADD_INTERNAL_CALL(TileComponent_GetIsWalkable);
        FL_ADD_INTERNAL_CALL(TileComponent_SetIsWalkable);
        FL_ADD_INTERNAL_CALL(TileComponent_GetIsHighlighted);
        FL_ADD_INTERNAL_CALL(TileComponent_SetIsHighlighted);
        FL_ADD_INTERNAL_CALL(TileComponent_GetIsHighlightedEnemy);
        FL_ADD_INTERNAL_CALL(TileComponent_SetIsHighlightedEnemy);
        FL_ADD_INTERNAL_CALL(TileComponent_GetPosition);
        FL_ADD_INTERNAL_CALL(TileComponent_SetPosition);
        FL_ADD_INTERNAL_CALL(TileComponent_GetMovementCost);
        FL_ADD_INTERNAL_CALL(TileComponent_SetMovementCost);
        FL_ADD_INTERNAL_CALL(TileComponent_ClearNeighbors);
        FL_ADD_INTERNAL_CALL(TileComponent_AddNeighbor);
        FL_ADD_INTERNAL_CALL(TileComponent_GetNeighbors);
        FL_ADD_INTERNAL_CALL(TileComponent_GetOccupant);
        FL_ADD_INTERNAL_CALL(TileComponent_SetOccupant);

        // Register UnitComponent internal calls
        FL_ADD_INTERNAL_CALL(UnitComponent_GetUnitType);
        FL_ADD_INTERNAL_CALL(UnitComponent_SetUnitType);
        FL_ADD_INTERNAL_CALL(UnitComponent_GetHealth);
        FL_ADD_INTERNAL_CALL(UnitComponent_SetHealth);
        FL_ADD_INTERNAL_CALL(UnitComponent_GetMaxHealth);
        FL_ADD_INTERNAL_CALL(UnitComponent_SetMaxHealth);
        FL_ADD_INTERNAL_CALL(UnitComponent_GetAttackPower);
        FL_ADD_INTERNAL_CALL(UnitComponent_SetAttackPower);
        FL_ADD_INTERNAL_CALL(UnitComponent_GetEnergy);
        FL_ADD_INTERNAL_CALL(UnitComponent_SetEnergy);
        FL_ADD_INTERNAL_CALL(UnitComponent_GetMaxEnergy);
        FL_ADD_INTERNAL_CALL(UnitComponent_SetMaxEnergy);
        
        FL_ADD_INTERNAL_CALL(UnitComponent_GetIsAlive);
        FL_ADD_INTERNAL_CALL(UnitComponent_SetIsAlive);
        FL_ADD_INTERNAL_CALL(UnitComponent_GetIsMoving);
        FL_ADD_INTERNAL_CALL(UnitComponent_SetIsMoving);
        FL_ADD_INTERNAL_CALL(UnitComponent_GetSpeed);
        FL_ADD_INTERNAL_CALL(UnitComponent_SetSpeed);

        FL_ADD_INTERNAL_CALL(UnitComponent_GetPosition);
        FL_ADD_INTERNAL_CALL(UnitComponent_SetPosition);
        FL_ADD_INTERNAL_CALL(UnitComponent_GetOccupiedTile);
        FL_ADD_INTERNAL_CALL(UnitComponent_SetOccupiedTile);
        FL_ADD_INTERNAL_CALL(UnitComponent_ResetOccupiedTile);

        // Register material internal calls
        FL_ADD_INTERNAL_CALL(MaterialComponent_GetTextureType);

        // Register Animator internal calls
        FL_ADD_INTERNAL_CALL(AnimationComponent_PlayAnimation);
        FL_ADD_INTERNAL_CALL(AnimationComponent_Stop);
        FL_ADD_INTERNAL_CALL(AnimationComponent_CheckEntityPlaying);
        FL_ADD_INTERNAL_CALL(AnimationComponent_RemoveEntityFromAnimation);
        FL_ADD_INTERNAL_CALL(AnimationComponent_ChangeAnimationTexture);
        FL_ADD_INTERNAL_CALL(AnimationComponent_GetAnimationName);
        FL_ADD_INTERNAL_CALL(AnimationComponent_GetAnimationNameSize);

        // Register Inventory System internal calls
        FL_ADD_INTERNAL_CALL(GetPlayerEntity);
        FL_ADD_INTERNAL_CALL(SetEntityTextureEnemy);
        FL_ADD_INTERNAL_CALL(SetEntityTextureCook);
        FL_ADD_INTERNAL_CALL(SetEntityChestTexture);

        FL_ADD_INTERNAL_CALL(SetEntityTextureByID);

        // Register Level switch interanl calls
        FL_ADD_INTERNAL_CALL(LevelSwitch_TutorialToLevel1);
        FL_ADD_INTERNAL_CALL(LevelSwitch_MainMenuToLevel1);
        FL_ADD_INTERNAL_CALL(LevelSwitch_ReloadCurrLevel);
        FL_ADD_INTERNAL_CALL(LevelSwitch_PauseMenuToMainMenu);
        FL_ADD_INTERNAL_CALL(LevelSwitch_GetLayerStatus);
        FL_ADD_INTERNAL_CALL(LevelSwitch_Level1ToLevel2);
        FL_ADD_INTERNAL_CALL(LevelSwitch_Level2ToLevel1);
        FL_ADD_INTERNAL_CALL(LevelSwitch_GetCurrLevel);
        FL_ADD_INTERNAL_CALL(LevelSwitch_ReturnMainMenu);
        FL_ADD_INTERNAL_CALL(LevelSwitch_MainMenuToTutorial);

        // Register Engine Level internal calls
        FL_ADD_INTERNAL_CALL(Engine_QuitGame);

        // Register Pause Menu internal calls
        FL_ADD_INTERNAL_CALL(PauseMenu_Toggle);
        FL_ADD_INTERNAL_CALL(Layer_ChangeOpacity);

        // Register for audio system
        FL_ADD_INTERNAL_CALL(Audio_PlaySound);
		FL_ADD_INTERNAL_CALL(Audio_GetBGMVolume);
		FL_ADD_INTERNAL_CALL(Audio_SetBGMVolume);

        // Font register functions
        FL_ADD_INTERNAL_CALL(Font_SetFont);

        // Chest register functions
        FL_ADD_INTERNAL_CALL(SetChestOverlayTrigger);

        // Register for Level Switching Switches
        FL_ADD_INTERNAL_CALL(BoolSwitch_ReloadCurrLevel); 
        FL_ADD_INTERNAL_CALL(BoolSwitch_MainMenuToLevel1);
        FL_ADD_INTERNAL_CALL(BoolSwitch_Level2ToLevel1);
        FL_ADD_INTERNAL_CALL(BoolSwitch_TutorialToLevel1);
        FL_ADD_INTERNAL_CALL(BoolSwitch_Level1ToLevel2);
    }
}
