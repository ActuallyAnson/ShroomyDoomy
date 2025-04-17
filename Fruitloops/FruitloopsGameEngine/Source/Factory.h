/******************************************************************************/
/*!
\file  Factory.h
\Proj name  Shroomy Doomy
\author  Zhang TongYan
\date    Oct 03, 2024
\brief  This file contains the declaration to the implementation of the Object Factory in the main game,
        responsible for creating, duplicating, serializing, and deserializing game objects.

All content ? 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
 */
 /******************************************************************************/
#pragma once

#include "ECSHub.h"
#include "Transform.h"
#include "Material.h"
#include "CameraComponent.h"
#include "Physics.h"
#include "Renderer.h"

namespace FruitLoops {

    class GameObjectFactory : public Subject {
    public:

/**************************************************************************
* @brief
*    Constructor for the GameObjectFactory class. Initializes the factory
*    by setting the global factory pointer.
*************************************************************************/
        GameObjectFactory();

/**************************************************************************
* @brief
*    Destructor for the GameObjectFactory class. Cleans up by resetting
*    the global factory pointer.
*************************************************************************/
        ~GameObjectFactory();

/**************************************************************************
* @brief
*    Creates a game object with the specified properties and adds
*    necessary components to the entity based on the type of game object.
*
* @param type
*    The type of the game object (e.g., "Player", "Camera").
* @param scale
*    The scale of the game object (Vector2D).
* @param orientation
*    The orientation of the game object (Vector2D).
* @param position
*    The position of the game object (Vector2D).
* @param color
*    The color of the game object (Vector3D).
* @param tex_id
*    The texture ID associated with the game object (int).
*
* @return
*    Returns an Entity representing the created game object.
*************************************************************************/
        Entity CreateGameObject(const Vector2D& position, const Vector2D& scale, const Vector2D& orientation, const std::string& textureName,
            const std::string& typeName);

/**************************************************************************
* @brief
*    Duplicates a game object by creating a new entity with the same type
*    and material properties as the original, but with a new position, scale,
*    and orientation.
*
* @param originalEntity
*    The entity representing the original game object to duplicate.
* @param position
*    The new position for the duplicated game object (Vector2D).
* @param scale
*    The new scale for the duplicated game object (Vector2D).
* @param orientation
*    The new orientation for the duplicated game object (Vector2D).
*
* @return
*    Returns an Entity representing the duplicated game object, or an invalid
*    entity if the original entity is not found.
*************************************************************************/
        Entity DuplicateGameObject(Entity originalEntity, const Vector2D& position, const Vector2D& scale, const Vector2D& orientation);

        Entity DuplicateGameObjectWithName(Entity originalEntity, const Vector2D& position, const Vector2D& scale, const Vector2D& orientation, const std::string& tileName);

        //static std::map<Entity, std::string> gameObjectTypeMap;


/**************************************************************************
* @brief
*    Loads game objects from a specified file and creates entities based
*    on the serialized data.
*
* @param filename
*    The name of the file to load game objects from.
*
* @return
*    Returns a vector of entities representing the loaded game objects.
*************************************************************************/
        std::vector<Entity> LoadHelper(nlohmann::json& gameObject);
        void GameObjectFactory::ReloadHelper(nlohmann::json& gameObject);
        std::vector<Entity> LoadGameObject(const std::string& filename);

        void ReloadGameObject(const std::string& filename);

        std::vector<Entity> HandleUndo(const std::string& cacheData);
/**************************************************************************
* @brief
*    Loads game objects from a specified file and creates entities based
*    on the serialized data.
*
* @param filename
*    The name of the file to load game objects from.
*
* @return
*    Returns a vector of entities representing the loaded game objects.
*************************************************************************/
        bool LoadConfigData(const std::string& filename);

        bool SaveConfig(const std::string& configPath);

        bool LoadAnimation(const std::string& filename, std::vector<SpriteAnimator>& animContainer);

        bool SaveAnimation(const std::string& filename, const std::vector<SpriteAnimator>& animContainer);
/**************************************************************************
* @brief
*    Destroys all game objects by iterating through the gameObjectTypeMap
*    and removing each entity.
*************************************************************************/
        void DestroyAllGameObjects();

/**************************************************************************
* @brief
*    Destroys a game object and removes it from the ECS system and the
*    gameObjectTypeMap.
*
* @param entity
*    The entity representing the game object to be destroyed.
*************************************************************************/
        void DestroyGameObject(Entity entity);
        //void DuplicateGameObjectJSON(nlohmann::json& gameData, const std::string& targetType, int numDuplicates, float spacingX, float spacingY);

        std::string GetEntityType(Entity entity);
        void SetEntityType(Entity entity, std::string type);

        std::map<Entity, std::string>& GetAllGameObjectsMap();
        std::vector<std::pair<Entity, std::string>> GetAllGameObjects();

        std::vector<Entity> GetEntityByName(const std::string& name);
        bool HasEntity (Entity entity);

    private:

        // A map to track game objects and their properties for duplication
        static std::map<Entity, std::string> gameObjectTypeMap;
    };

    extern GameObjectFactory* factory;
}