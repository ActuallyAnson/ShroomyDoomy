/******************************************************************************/
/*!
\file  Factory.cpp
\Proj name  Shroomy Doomy
\author  Zhang TongYan
\date    Oct 03, 2024
\brief  This file contains the implementation of the Object Factory in the main game,
        responsible for creating, duplicating, serializing, and deserializing game objects.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
 */
 /******************************************************************************/
#include "pch.h"
#include "Input.h"
#include "Factory.h"
#include "Serialization.h"

namespace FruitLoops {

    GameObjectFactory* factory = nullptr;
    std::map<Entity, std::string> GameObjectFactory::gameObjectTypeMap;

/**************************************************************************
* @brief
*    Constructor for the GameObjectFactory class. Initializes the factory
*    by setting the global factory pointer.
*************************************************************************/
    GameObjectFactory::GameObjectFactory() {
        factory = this;
    }

/**************************************************************************
* @brief
*    Destructor for the GameObjectFactory class. Cleans up by resetting
*    the global factory pointer.
*************************************************************************/
    GameObjectFactory::~GameObjectFactory() {
        factory = nullptr;
    }

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
    Entity GameObjectFactory::CreateGameObject(const Vector2D& position, const Vector2D& scale, const Vector2D& orientation, const std::string& textureName,
        const std::string& typeName) {
        Entity entity = ecsHub->CreateEntity();

        ecsHub->AddComponent(entity, Transform{ scale, orientation, position });
        ecsHub->AddComponent(entity, Material{ {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}, textureName});

        gameObjectTypeMap[entity] = typeName;
        NotifyObservers(new ObjectSpawnedMessage(entity));
        return entity;
    }

/**************************************************************************
* @brief
*    Destroys a game object and removes it from the ECS system and the
*    gameObjectTypeMap.
*
* @param entity
*    The entity representing the game object to be destroyed.
*************************************************************************/
    void GameObjectFactory::DestroyGameObject(Entity entity) {
        // Check if the entity exists in the gameObjectTypeMap
        auto it = gameObjectTypeMap.find(entity);
        if (it != gameObjectTypeMap.end()) {

            // Remove the entity from the ECS system
            if (it->second == "FontObj") {
                NotifyObservers(new FontDeletedMessage(entity));
            }
            else {
                NotifyObservers(new ObjectDeletedMessage(entity));
            }
            
            ecsHub->DestroyEntity(entity);
            gameObjectTypeMap.erase(it);
        }
        else {
            std::cerr << "Entity " << entity << " not found!" << std::endl;
        }
    }

    

    /**************************************************************************
* @brief
*    Destroys all game objects by iterating through the gameObjectTypeMap
*    and removing each entity.
*************************************************************************/
    void GameObjectFactory::DestroyAllGameObjects() {
        auto it = gameObjectTypeMap.begin();
        while (it != gameObjectTypeMap.end()) {
            Entity entity = it->first;
            // Destroy the entity
            DestroyGameObject(entity);
            // Erase the current entity from the map and get the next valid iterator
            it = gameObjectTypeMap.erase(it);
        }

    }

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
    Entity GameObjectFactory::DuplicateGameObject(Entity originalEntity, const Vector2D& position, const Vector2D& scale, const Vector2D& orientation) {
        auto it = gameObjectTypeMap.find(originalEntity);
        if (it != gameObjectTypeMap.end()) {
            Entity newEntity = ecsHub->CreateEntity();

            if (ecsHub->HasComponent<CameraComponent>(originalEntity)) {
                return Entity();
            }

            if (ecsHub->HasComponent<Transform>(originalEntity)) {
                ecsHub->AddComponent(newEntity, Transform{ scale, orientation, position });
            }
            if (ecsHub->HasComponent<Material>(originalEntity)) {
                ecsHub->AddComponent(newEntity, ecsHub->GetComponent<Material>(originalEntity));
            }
            if (ecsHub->HasComponent<RigidBody>(originalEntity)) {
                ecsHub->AddComponent(newEntity, ecsHub->GetComponent<RigidBody>(originalEntity));
            }
            if (ecsHub->HasComponent<FontDataComponent>(originalEntity)) {
                ecsHub->AddComponent(newEntity, ecsHub->GetComponent<FontDataComponent>(originalEntity));
            }
            if (ecsHub->HasComponent<UnitComponent>(originalEntity)) {
                const UnitComponent& originalUnitComp = ecsHub->GetComponent<UnitComponent>(originalEntity);
                UnitComponent newUnitComp = originalUnitComp;
                newUnitComp.position = position;
                ecsHub->AddComponent(newEntity, newUnitComp);
            }
            if (ecsHub->HasComponent<ButtonComponent>(originalEntity)) {
                ecsHub->AddComponent(newEntity, ecsHub->GetComponent<ButtonComponent>(originalEntity));
            }
            if (ecsHub->HasComponent<ScriptComponent>(originalEntity)) {
                const ScriptComponent& originalScript = ecsHub->GetComponent<ScriptComponent>(originalEntity);
                ScriptComponent newScript = originalScript;
                //newScript.UpdateScriptInstance();
                ecsHub->AddComponent(newEntity, newScript);
            }

            gameObjectTypeMap[newEntity] = it->second;
            if (it->second.substr(7, 4) == "Font") {
                NotifyObservers(new FontSpawnedMessage(newEntity));
            }
            else {
                NotifyObservers(new ObjectSpawnedMessage(newEntity));
            }
            return newEntity;
        }
        return Entity();  // Return an invalid entity if original not found
    }

    Entity GameObjectFactory::DuplicateGameObjectWithName(Entity originalEntity, const Vector2D& position, const Vector2D& scale, const Vector2D& orientation, const std::string& tileName) {
        auto it = gameObjectTypeMap.find(originalEntity);
        if (it != gameObjectTypeMap.end()) {
            Entity newEntity = ecsHub->CreateEntity();

            if (ecsHub->HasComponent<CameraComponent>(originalEntity)) {
                return Entity();
            }

            if (ecsHub->HasComponent<Transform>(originalEntity)) {
                ecsHub->AddComponent(newEntity, Transform{ scale, orientation, position });
            }
            if (ecsHub->HasComponent<Material>(originalEntity)) {
                ecsHub->AddComponent(newEntity, ecsHub->GetComponent<Material>(originalEntity));
            }
            if (ecsHub->HasComponent<RigidBody>(originalEntity)) {
                ecsHub->AddComponent(newEntity, ecsHub->GetComponent<RigidBody>(originalEntity));
            }
            if (ecsHub->HasComponent<FontDataComponent>(originalEntity)) {
                ecsHub->AddComponent(newEntity, ecsHub->GetComponent<FontDataComponent>(originalEntity));
            }
            if (ecsHub->HasComponent<ButtonComponent>(originalEntity)) {
                ecsHub->AddComponent(newEntity, ecsHub->GetComponent<ButtonComponent>(originalEntity));
            }
            if (ecsHub->HasComponent<TileComponent>(originalEntity)) {
                const TileComponent& originalTileComp = ecsHub->GetComponent<TileComponent>(originalEntity);
                TileComponent newTileComp = originalTileComp;
                newTileComp.position = position;
                ecsHub->AddComponent(newEntity, newTileComp);
            }
            if (ecsHub->HasComponent<UnitComponent>(originalEntity)) {
                const UnitComponent& originalUnitComp = ecsHub->GetComponent<UnitComponent>(originalEntity);
                UnitComponent newUnitComp = originalUnitComp;
                newUnitComp.position = position;
                ecsHub->AddComponent(newEntity, newUnitComp);
            }
            if (ecsHub->HasComponent<ScriptComponent>(originalEntity)) {
                const ScriptComponent& originalScript = ecsHub->GetComponent<ScriptComponent>(originalEntity);
                ScriptComponent newScript = originalScript;
                //newScript.UpdateScriptInstance();
                ecsHub->AddComponent(newEntity, newScript);
            }

            // Update gameObjectTypeMap with the new tile name
            gameObjectTypeMap[newEntity] = tileName;
            return newEntity;
        }

        return Entity();  // Return an invalid entity if original not found
    }

/**************************************************************************
* @brief
*    loads configuration data

* @param filename
*    The name of the file to load from.
*************************************************************************/
    bool GameObjectFactory::LoadConfigData(const std::string& filename)
    {
        // Load the JSON data from the file
        nlohmann::json loadedConfigData = FruitLoops::LoadGameObjectFromFile(filename);

        if (!loadedConfigData.contains("configurations")) {
            std::cerr << "No config data found in JSON file." << std::endl;
            return false;  // Return empty if no game objects found
        }

        for (const auto& configData : loadedConfigData["configurations"]) {
            try {

                // Windows data
                ConfigData::width = configData["WindowProperties"]["width"].get<int>();
                ConfigData::height = configData["WindowProperties"]["height"].get<int>();
                ConfigData::title = configData["WindowProperties"]["title"].get<std::string>();
                ConfigData::imGuiTitleHeightOffset = configData["WindowProperties"]["imGuiTitleHeightOffset"].get<int>();
                ConfigData::imGuiTitleWidthOffset = configData["WindowProperties"]["imGuiTitleWidthOffset"].get<int>();

                // Windows data
                ConfigData::fontColor = Vector3D(configData["Graphics"]["fontColor"][0], configData["Graphics"]["fontColor"][1], configData["Graphics"]["fontColor"][2]);
                ConfigData::fontDetail = configData["Graphics"]["fontdetail"].get<int>();

                // BGM
                ConfigData::BGM = configData["BGM"]["BGM"].get<std::string>();

            }
            catch (const std::exception& e) {
                std::cerr << "Error loading config data: " << e.what() << std::endl;
            }
        }
        return true;
    }

    bool GameObjectFactory::SaveConfig(const std::string& configPath) {
        std::ifstream inputFile(configPath);
        nlohmann::json configJson;

        if (inputFile.is_open()) {
            inputFile >> configJson;
            inputFile.close();
        }
        else {
#ifdef _DEBUG
            spdlog::warn("Config file not found. Creating a new one at: {}", configPath);
#endif
            configJson = { {"configurations", nlohmann::json::array()} }; // Default structure
            return false;
        }

        // Update values
        auto& config = configJson["configurations"][0];

        // Update window properties
        config["WindowProperties"]["width"] = ConfigData::width;
        config["WindowProperties"]["height"] = ConfigData::height;
        config["WindowProperties"]["title"] = ConfigData::title;
        config["WindowProperties"]["imGuiTitleWidthOffset"] = ConfigData::imGuiTitleWidthOffset;
        config["WindowProperties"]["imGuiTitleHeightOffset"] = ConfigData::imGuiTitleHeightOffset;

        // Update graphics
        config["Graphics"]["fontColor"] = { ConfigData::fontColor.x, ConfigData::fontColor.y, ConfigData::fontColor.z };
        config["Graphics"]["fontdetail"] = ConfigData::fontDetail;

        // Update BGM
        config["BGM"]["BGM"] = ConfigData::BGM;

        // Save to file
        std::ofstream outputFile(configPath);
        if (outputFile.is_open()) {
            outputFile << configJson.dump(4); // Pretty print with 4 spaces
#ifdef _DEBUG
            spdlog::info("Config saved successfully to {}", configPath);
#endif
            return true;
        }
        else {
#ifdef _DEBUG
            spdlog::error("Failed to save config to {}", configPath);
#endif
            return false;
        }
    }

    // for loading animation
    bool GameObjectFactory::LoadAnimation(const std::string& filename, std::vector<SpriteAnimator>& animContainer)
    {
        // Load the JSON data from the file
        nlohmann::json loadedConfigData = FruitLoops::LoadGameObjectFromFile(filename);

        if (!loadedConfigData.contains("Animation")) {
            std::cerr << "No Animation found in JSON file." << std::endl;
            return false;  // Return empty if no game objects found
        }

        for (const auto& animData : loadedConfigData["Animation"]) {
            try {
                SpriteAnimator animation;
                auto& entities = animData["entities"];
                for (const auto& entity : entities) {
                    animation.AddEntity(static_cast<Entity>(entity));
                }
                animation.SetSpeed(animData["animationSpeed"].get<float>());
                animation.SetFrameSize(Vector2D(animData["frameSize"][0], animData["frameSize"][1]));
                auto& framesArray = animData["framesPerRow"];
                std::vector<int> FPR;
                for (const auto& frame : framesArray) {
                    FPR.push_back(frame.get<int>());
                }
                animation.SetFramesPerRow(FPR);
                animation.SetPlaying(animData["playing"].get<bool>());
                animation.SetLooping(animData["looping"].get<bool>());
                animation.SetPosition(Vector2D(animData["position"][0], animData["position"][1]));
                animation.SetScale(Vector2D(animData["scale"][0], animData["scale"][1]));
                animation.SetPositionOffset(Vector2D(animData["positionOffset"][0], animData["positionOffset"][1]));
                animation.SetScaleOffset(Vector2D(animData["scaleOffset"][0], animData["scaleOffset"][1]));
                std::unordered_map<size_t, std::string> map;
                for (const auto& [frame, sound] : animData["soundMap"].items()) {
                    map[std::stoi(frame)] = sound;
                }
                animation.SetSoundMap(map);
                animation.SetTextureName(animData["textureName"].get<std::string>());
                
                animContainer.emplace_back(animation);
                

            }
            catch (const std::exception& e) {
                std::cerr << "Error loading config data: " << e.what() << std::endl;
            }
        }
        return true;
    }
    bool GameObjectFactory::SaveAnimation(const std::string& filename, const std::vector<SpriteAnimator>& animContainer)
    {
        std::ifstream inputFile(filename);
        nlohmann::json animJson;

        if (inputFile.is_open()) {
            inputFile >> animJson;
            inputFile.close();
        }
        else {

            animJson = { {"Animation", nlohmann::json::array()} }; // Default structure
            return false;
        }

        // Update values
        for (size_t i = 0; i < animContainer.size(); i++) {
            auto& anim = animJson["Animation"][i];

           /* anim["entities"] = nlohmann::json::array();

            for (const auto& entity : animContainer[i].GetEntity()) {
                anim["entities"][i].push_back(static_cast<int>(entity));
            }*/

            anim["animationSpeed"] = animContainer[i].GetSpeed();
            anim["frameSize"] = { animContainer[i].GetFrameSize().x, animContainer[i].GetFrameSize().y };
            anim["framesPerRow"] = animContainer[i].GetFramesPerRow();
            anim["playing"] = animContainer[i].GetPlaying();
            anim["looping"] = animContainer[i].GetLooping();
            anim["position"] = { animContainer[i].GetPosition().x, animContainer[i].GetPosition().y };
            anim["scale"] = { animContainer[i].GetScale().x, animContainer[i].GetScale().y };
            anim["positionOffset"] = { animContainer[i].GetPositionOffset().x, animContainer[i].GetPositionOffset().y };
            anim["scaleOffset"] = { animContainer[i].GetScaleOffset().x, animContainer[i].GetScaleOffset().y };
            std::unordered_map<size_t, std::string> soundMap = animContainer[i].GetSoundMap();
            anim["soundMap"] = nlohmann::json::object();
            for (const auto& pair : soundMap) {
                anim["soundMap"][std::to_string(pair.first)] = pair.second;
            }
            anim["textureName"] = animContainer[i].GetTextureName();
        }
        

        // Save to file
        std::ofstream outputFile(filename);
        if (outputFile.is_open()) {
            outputFile << animJson.dump(4); // Pretty print with 4 spaces

            return true;
        }
        else {
            return false;
        }
    }
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
    std::vector<Entity> GameObjectFactory::LoadHelper(nlohmann::json& gameObject) {

        std::vector<Entity> entities;

        for (const auto& gameLevel1 : gameObject["gameObjects"]) {
            try {
                Entity entity = ecsHub->CreateEntity();

                // Check if the "Transform" component is present
                if (gameLevel1.contains("Transform")) {
                    Vector2D scale, rot, transform;
                    if (gameLevel1["Transform"].contains("scale"))
                        scale = Vector2D(gameLevel1["Transform"]["scale"][0], gameLevel1["Transform"]["scale"][1]);
                    if (gameLevel1["Transform"].contains("orientation"))
                        rot = Vector2D(gameLevel1["Transform"]["orientation"][0], gameLevel1["Transform"]["orientation"][1]);
                    if (gameLevel1["Transform"].contains("position"))
                        transform = Vector2D(gameLevel1["Transform"]["position"][0], gameLevel1["Transform"]["position"][1]);
                    ecsHub->AddComponent(entity, Transform{ scale, rot, transform });
                }

                // Check if the "Material" component is present
                if (gameLevel1.contains("Material")) {
                    Vector2D uvMax, uvMin;
                    std::string textureName;
                    Vector3D color;
                    if (gameLevel1["Material"].contains("color"))
                        color = Vector3D(gameLevel1["Material"]["color"][0], gameLevel1["Material"]["color"][1], gameLevel1["Material"]["color"][2]);
                    if (gameLevel1["Material"].contains("textureName"))
                        textureName = gameLevel1["Material"]["textureName"].get<std::string>();
                    if (gameLevel1["Material"].contains("uvMax"))
                        uvMax = Vector2D(gameLevel1["Material"]["uvMax"][0], gameLevel1["Material"]["uvMax"][1]);
                    if (gameLevel1["Material"].contains("uvMin"))
                        uvMin = Vector2D(gameLevel1["Material"]["uvMin"][0], gameLevel1["Material"]["uvMin"][1]);
                    ecsHub->AddComponent(entity, Material{ color, uvMax, uvMin, textureName });
                }

                if (gameLevel1.contains("RigidBody")) {
                    Vector2D velocity, position;
                    if (gameLevel1["RigidBody"].contains("velocity"))
                        velocity = Vector2D(gameLevel1["RigidBody"]["velocity"][0], gameLevel1["RigidBody"]["velocity"][1]);
                    if (gameLevel1["RigidBody"].contains("position"))
                        position = Vector2D(gameLevel1["RigidBody"]["position"][0], gameLevel1["RigidBody"]["position"][1]);
                    ecsHub->AddComponent(entity, RigidBody{ velocity, position });
                }

                // Load the "PhysicsObject" component
                if (gameLevel1.contains("PhysicsObject")) {
                    Vector2D position(0.0f, 0.0f); // Default values
                    float mass = 1.0f;            // Default mass
                    //float drag = 0.1f;            // Default drag

                    // Extract position and mass
                    if (gameLevel1["PhysicsObject"].contains("position"))
                        position = Vector2D(gameLevel1["PhysicsObject"]["position"][0], gameLevel1["PhysicsObject"]["position"][1]);
                    if (gameLevel1["PhysicsObject"].contains("mass"))
                        mass = gameLevel1["PhysicsObject"]["mass"].get<float>();

                    // Create the component using the constructor
                    PhysicsObject physicsComponent(position, mass);

                    // Set additional properties
                    if (gameLevel1["PhysicsObject"].contains("velocity"))
                        physicsComponent.velocity = Vector2D(gameLevel1["PhysicsObject"]["velocity"][0], gameLevel1["PhysicsObject"]["velocity"][1]);
                    if (gameLevel1["PhysicsObject"].contains("acceleration"))
                        physicsComponent.acceleration = Vector2D(gameLevel1["PhysicsObject"]["acceleration"][0], gameLevel1["PhysicsObject"]["acceleration"][1]);
                    if (gameLevel1["PhysicsObject"].contains("drag"))
                        physicsComponent.drag = gameLevel1["PhysicsObject"]["drag"].get<float>();

                    // Add the component to the entity
                    ecsHub->AddComponent(entity, physicsComponent);
                }

                // Load UnitComponent
                if (gameLevel1.contains("UnitComponent")) {
                    UnitType unitType = static_cast<UnitType>(gameLevel1["UnitComponent"]["unitType"].get<int>());
                    int health = gameLevel1["UnitComponent"]["health"];
                    int maxHealth = gameLevel1["UnitComponent"]["maxHealth"];
                    int attackPower = gameLevel1["UnitComponent"]["attackPower"];
                    int energy = gameLevel1["UnitComponent"]["energy"];
                    int maxEnergy = gameLevel1["UnitComponent"]["maxEnergy"];
                    bool isAlive = gameLevel1["UnitComponent"]["isAlive"];
                    bool isMoving = gameLevel1["UnitComponent"]["isMoving"];
                    int speed = gameLevel1["UnitComponent"]["speed"];

                    Vector2D position = gameLevel1.contains("UnitComponent") && gameLevel1["UnitComponent"].contains("position")
                        ? Vector2D(gameLevel1["UnitComponent"]["position"][0], gameLevel1["UnitComponent"]["position"][1])
                        : Vector2D(0, 0);

                    ecsHub->AddComponent(entity, UnitComponent{ unitType, health, maxHealth, attackPower,
                        energy, maxEnergy,isAlive, isMoving, speed, position });
                }


                // Load TileComponent
                if (gameLevel1.contains("TileComponent")) {
                    bool walkable = gameLevel1["TileComponent"]["walkable"];
                    bool highlighted = gameLevel1["TileComponent"]["highlighted"];
                    bool highlightedEnemy = gameLevel1["TileComponent"]["highlightedEnemy"];
                    Vector2D position = Vector2D(gameLevel1["TileComponent"]["position"][0], gameLevel1["TileComponent"]["position"][1]);
                    TileType tileType = gameLevel1["TileComponent"]["tileType"] == "Floor" ? TileType::Floor : TileType::Wall;
                    float movementCost = gameLevel1["TileComponent"]["movementCost"];
                    ecsHub->AddComponent(entity, TileComponent{ walkable, highlighted, highlightedEnemy, position, tileType, movementCost });
                }

                if (gameLevel1.contains("CamComponent")) {
                    GLint height{ 0 };
                    GLint minHeight{ 0 }, maxHeight{ 0 };
                    GLint heightChangeDir{ 0 };
                    GLint heightChangeVal{ 0 };
                    GLfloat linearSpeed{ 0.f };

                    if (gameLevel1["CamComponent"].contains("height"))
                        height = gameLevel1["CamComponent"]["height"].get<GLint>();

                    if (gameLevel1["CamComponent"].contains("minHeight"))
                        minHeight = gameLevel1["CamComponent"]["minHeight"].get<GLint>();

                    if (gameLevel1["CamComponent"].contains("maxHeight"))
                        maxHeight = gameLevel1["CamComponent"]["maxHeight"].get<GLint>();

                    if (gameLevel1["CamComponent"].contains("heightChangeDir"))
                        heightChangeDir = gameLevel1["CamComponent"]["heightChangeDir"].get<GLint>();

                    if (gameLevel1["CamComponent"].contains("heightChangeVal"))
                        heightChangeVal = gameLevel1["CamComponent"]["heightChangeVal"].get<GLint>();

                    if (gameLevel1["CamComponent"].contains("linearSpeed"))
                        linearSpeed = gameLevel1["CamComponent"]["linearSpeed"].get<GLfloat>();
                    ecsHub->AddComponent(entity, CameraComponent{ height, minHeight, maxHeight, heightChangeDir, heightChangeVal, linearSpeed });
                }

                if (gameLevel1.contains("FontData")) {
                    if (gameLevel1["FontData"].contains("text")) {
                        ecsHub->AddComponent(entity, FontDataComponent{ gameLevel1["FontData"]["font"].get<std::string>(),
                            gameLevel1["FontData"]["text"].get<std::string>(), gameLevel1["FontData"]["scale"].get<float>() });

                    }

                }

                if (gameLevel1.contains("Button")) {
                    ecsHub->AddComponent(entity, ButtonComponent{
                        gameLevel1["Button"].value("isHovered", false),
                        gameLevel1["Button"].value("isClicked", false),
                        gameLevel1["Button"].value("isEnabled", true)
                        });
                }

                if (gameLevel1.contains("ScriptComponent")) {
                    std::string scriptName = gameLevel1["ScriptComponent"].value("name", "");
                    ecsHub->AddComponent(entity, ScriptComponent(scriptName));
                }

                if (gameLevel1.contains("AnimationComponent")) {
                    std::vector<std::string> animNames = gameLevel1["AnimationComponent"]["AnimationName"].get<std::vector<std::string>>();
                    ecsHub->AddComponent(entity, AnimationComponent({ animNames }));
                }

                // Unique identifier for all objects
                std::string type = gameLevel1["type"].get<std::string>();
                // set level 2 script to inactive first
           
                gameObjectTypeMap[entity] = type;
                entities.push_back(entity);  // Add the created entity to the vector

            }
            catch (const std::exception& e) {
                std::cerr << "Error loading game object: " << e.what() << std::endl;
            }
        }

        return entities;
    }

    
    std::vector<Entity> GameObjectFactory::LoadGameObject(const std::string& filename) {
        std::vector<Entity> entities;
        UNUSED_PARAM(filename);
        // Load the JSON data from the file
        
        nlohmann::json utilsObjects = FruitLoops::LoadGameObjectFromFile("../GameData/Utils.json");
        nlohmann::json mainMenuObjects = FruitLoops::LoadGameObjectFromFile("../GameData/MainMenu.json");
        nlohmann::json storyObjects = FruitLoops::LoadGameObjectFromFile("../GameData/Story.json");
        nlohmann::json pauseMenuObjects = FruitLoops::LoadGameObjectFromFile("../GameData/PauseMenu.json");
        nlohmann::json chestObjects = FruitLoops::LoadGameObjectFromFile("../GameData/ChestOverlay.json");
        nlohmann::json fpsObjects = FruitLoops::LoadGameObjectFromFile("../GameData/FPS.json");
        nlohmann::json uiObjects = FruitLoops::LoadGameObjectFromFile("../GameData/UI.json");
        nlohmann::json endMenuObjects = FruitLoops::LoadGameObjectFromFile("../GameData/EndMenu.json");

        std::vector<Entity> loadedEntities;
        loadedEntities = LoadHelper(utilsObjects);
        entities.insert(std::end(entities), std::begin(loadedEntities), std::end(loadedEntities));

        loadedEntities.clear();
        loadedEntities = LoadHelper(mainMenuObjects);
        entities.insert(std::end(entities), std::begin(loadedEntities), std::end(loadedEntities));

        loadedEntities.clear();
        loadedEntities = LoadHelper(storyObjects);
        entities.insert(std::end(entities), std::begin(loadedEntities), std::end(loadedEntities));

        loadedEntities.clear();
        loadedEntities = LoadHelper(pauseMenuObjects);
        entities.insert(std::end(entities), std::begin(loadedEntities), std::end(loadedEntities));

        loadedEntities.clear();
        loadedEntities = LoadHelper(chestObjects);
        entities.insert(std::end(entities), std::begin(loadedEntities), std::end(loadedEntities));


        loadedEntities.clear();
        loadedEntities = LoadHelper(fpsObjects);
        entities.insert(std::end(entities), std::begin(loadedEntities), std::end(loadedEntities));

        loadedEntities.clear();
        loadedEntities = LoadHelper(uiObjects);
        entities.insert(std::end(entities), std::begin(loadedEntities), std::end(loadedEntities));

        loadedEntities.clear();
        loadedEntities = LoadHelper(endMenuObjects);
        entities.insert(std::end(entities), std::begin(loadedEntities), std::end(loadedEntities));

        return entities;
    }

    /**************************************************************************
* @brief
*    Reload game objects from a specified file and creates entities based
*    on the serialized data.
*
* @param filename
*    The name of the file to load game objects from.
*
* @return
*    Returns a vector of entities representing the loaded game objects.
*************************************************************************/

    void GameObjectFactory::ReloadHelper(nlohmann::json& gameObject) {
        for (const auto& [entity, type] : factory->GetAllGameObjects()) {
            /*if (!ecsHub->HasComponent<UnitComponent>(entity) && !ecsHub->HasComponent<TileComponent>(entity)) {
                continue;
            }*/
            for (const auto& gameLevel1 : gameObject["gameObjects"]) {
                try {
                    std::string entityType = gameLevel1["type"].get<std::string>();
                    if (type == entityType) {
                        // Check if the "Transform" component is present
                        if (gameLevel1.contains("Transform")) {
                            Transform& transformComponent = ecsHub->GetComponent<Transform>(entity);
                            if (gameLevel1["Transform"].contains("scale"))
                                transformComponent.scale = Vector2D(gameLevel1["Transform"]["scale"][0], gameLevel1["Transform"]["scale"][1]);
                            if (gameLevel1["Transform"].contains("orientation"))
                                transformComponent.orientation = Vector2D(gameLevel1["Transform"]["orientation"][0], gameLevel1["Transform"]["orientation"][1]);
                            if (gameLevel1["Transform"].contains("position"))
                                transformComponent.position = Vector2D(gameLevel1["Transform"]["position"][0], gameLevel1["Transform"]["position"][1]);
                        }

                        if (gameLevel1.contains("Material")) {
                            Material& materialComponent = ecsHub->GetComponent<Material>(entity);
                            if (gameLevel1["Material"].contains("color"))
                                materialComponent.color = Vector3D(gameLevel1["Material"]["color"][0], gameLevel1["Material"]["color"][1], gameLevel1["Material"]["color"][2]);
                            if (gameLevel1["Material"].contains("textureName"))
                                materialComponent.textureName = gameLevel1["Material"]["textureName"].get<std::string>();
                            if (gameLevel1["Material"].contains("uvMax"))
                                materialComponent.uvMax = Vector2D(gameLevel1["Material"]["uvMax"][0], gameLevel1["Material"]["uvMax"][1]);
                            if (gameLevel1["Material"].contains("uvMin"))
                                materialComponent.uvMin = Vector2D(gameLevel1["Material"]["uvMin"][0], gameLevel1["Material"]["uvMin"][1]);
                        }

                        if (gameLevel1.contains("TileComponent")) {
                            TileComponent& tileComponent = ecsHub->GetComponent<TileComponent>(entity);
                            tileComponent.walkable = gameLevel1["TileComponent"]["walkable"];
                            tileComponent.highlighted = gameLevel1["TileComponent"]["highlighted"];
                            tileComponent.highlightedEnemy = gameLevel1["TileComponent"]["highlightedEnemy"];
                            tileComponent.position = Vector2D(gameLevel1["TileComponent"]["position"][0], gameLevel1["TileComponent"]["position"][1]);
                            tileComponent.tileType = gameLevel1["TileComponent"]["tileType"] == "Floor" ? TileType::Floor : TileType::Wall;
                            tileComponent.movementCost = gameLevel1["TileComponent"]["movementCost"];
                            tileComponent.ClearOccupant();
                        }

                        if (gameLevel1.contains("UnitComponent")) {
                            UnitComponent& unitComponent = ecsHub->GetComponent<UnitComponent>(entity);
                            unitComponent.unitType = static_cast<UnitType>(gameLevel1["UnitComponent"]["unitType"].get<int>());
                            unitComponent.health = gameLevel1["UnitComponent"]["health"];
                            unitComponent.maxHealth = gameLevel1["UnitComponent"]["maxHealth"];
                            unitComponent.attackPower = gameLevel1["UnitComponent"]["attackPower"];
                            unitComponent.energy = gameLevel1["UnitComponent"]["energy"];
                            unitComponent.maxEnergy = gameLevel1["UnitComponent"]["maxEnergy"];
                            unitComponent.isAlive = gameLevel1["UnitComponent"]["isAlive"];
                            unitComponent.isMoving = gameLevel1["UnitComponent"]["isMoving"];
                            unitComponent.speed = gameLevel1["UnitComponent"]["speed"];
                            unitComponent.position = gameLevel1.contains("UnitComponent") && gameLevel1["UnitComponent"].contains("position")
                                ? Vector2D(gameLevel1["UnitComponent"]["position"][0], gameLevel1["UnitComponent"]["position"][1])
                                : Vector2D(0, 0);
                        }
                        break;
                    }
                    
                    

                }
                catch (const std::exception& e) {
                    std::cerr << "Error loading game object: " << e.what() << std::endl;
                }
            }
        }
        
    }

    void GameObjectFactory::ReloadGameObject(const std::string& filename) {

        // Load the JSON data from the file
        nlohmann::json gameObjects = FruitLoops::LoadGameObjectFromFile(filename);
        nlohmann::json uiObjects = FruitLoops::LoadGameObjectFromFile("../GameData/UI.json");
        nlohmann::json chestObjects = FruitLoops::LoadGameObjectFromFile("../GameData/ChestOverlay.json");
        ReloadHelper(gameObjects);
        ReloadHelper(uiObjects);
        ReloadHelper(chestObjects);
    }

    bool GameObjectFactory::HasEntity(Entity entity)
    {
        return gameObjectTypeMap.find(entity) != gameObjectTypeMap.end();
    }


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
        std::vector<Entity> GameObjectFactory::HandleUndo(const std::string& cacheData) {
            std::vector<Entity> entities;

            // Load the JSON data from the file
            nlohmann::json loadedGameObjects = nlohmann::json::parse(cacheData);

            // Check if the JSON contains game objects
            if (!loadedGameObjects.contains("gameObjects")) {
                std::cerr << "No game objects found in JSON file." << std::endl;
                return entities;  // Return empty if no game objects found
            }

            // Loop through each game object in the JSON array
            for (const auto& gameLevel1 : loadedGameObjects["gameObjects"]) {
                try {
                    Entity entity = ecsHub->CreateEntity();

                    // Check if the "Transform" component is present
                    if (gameLevel1.contains("Transform")) {
                        Vector2D scale, rot, transform;
                        if (gameLevel1["Transform"].contains("scale"))
                            scale = Vector2D(gameLevel1["Transform"]["scale"][0], gameLevel1["Transform"]["scale"][1]);
                        if (gameLevel1["Transform"].contains("orientation"))
                            rot = Vector2D(gameLevel1["Transform"]["orientation"][0], gameLevel1["Transform"]["orientation"][1]);
                        if (gameLevel1["Transform"].contains("position"))
                            transform = Vector2D(gameLevel1["Transform"]["position"][0], gameLevel1["Transform"]["position"][1]);
                        ecsHub->AddComponent(entity, Transform{ scale, rot, transform });
                    }

                    // Check if the "Material" component is present
                    if (gameLevel1.contains("Material")) {
                        Vector2D uvMax, uvMin;
                        std::string textureName;
                        Vector3D color;
                        if (gameLevel1["Material"].contains("color"))
                            color = Vector3D(gameLevel1["Material"]["color"][0], gameLevel1["Material"]["color"][1], gameLevel1["Material"]["color"][2]);
                        if (gameLevel1["Material"].contains("textureName"))
                            textureName = gameLevel1["Material"]["textureName"].get<std::string>();
                        if (gameLevel1["Material"].contains("uvMax"))
                            uvMax = Vector2D(gameLevel1["Material"]["uvMax"][0], gameLevel1["Material"]["uvMax"][1]);
                        if (gameLevel1["Material"].contains("uvMin"))
                            uvMin = Vector2D(gameLevel1["Material"]["uvMin"][0], gameLevel1["Material"]["uvMin"][1]);
                        ecsHub->AddComponent(entity, Material{ color, uvMax, uvMin, textureName });
                    }

                    if (gameLevel1.contains("RigidBody")) {
                        Vector2D velocity, position;
                        if (gameLevel1["RigidBody"].contains("velocity"))
                            velocity = Vector2D(gameLevel1["RigidBody"]["velocity"][0], gameLevel1["RigidBody"]["velocity"][1]);
                        if (gameLevel1["RigidBody"].contains("position"))
                            position = Vector2D(gameLevel1["RigidBody"]["position"][0], gameLevel1["RigidBody"]["position"][1]);
                        ecsHub->AddComponent(entity, RigidBody{ velocity, position });
                    }

                    // Load the "PhysicsObject" component
                    if (gameLevel1.contains("PhysicsObject")) {
                        Vector2D position(0.0f, 0.0f); // Default values
                        float mass = 1.0f;            // Default mass
                        //float drag = 0.1f;            // Default drag

                        // Extract position and mass
                        if (gameLevel1["PhysicsObject"].contains("position"))
                            position = Vector2D(gameLevel1["PhysicsObject"]["position"][0], gameLevel1["PhysicsObject"]["position"][1]);
                        if (gameLevel1["PhysicsObject"].contains("mass"))
                            mass = gameLevel1["PhysicsObject"]["mass"].get<float>();

                        // Create the component using the constructor
                        PhysicsObject physicsComponent(position, mass);

                        // Set additional properties
                        if (gameLevel1["PhysicsObject"].contains("velocity"))
                            physicsComponent.velocity = Vector2D(gameLevel1["PhysicsObject"]["velocity"][0], gameLevel1["PhysicsObject"]["velocity"][1]);
                        if (gameLevel1["PhysicsObject"].contains("acceleration"))
                            physicsComponent.acceleration = Vector2D(gameLevel1["PhysicsObject"]["acceleration"][0], gameLevel1["PhysicsObject"]["acceleration"][1]);
                        if (gameLevel1["PhysicsObject"].contains("drag"))
                            physicsComponent.drag = gameLevel1["PhysicsObject"]["drag"].get<float>();

                        // Add the component to the entity
                        ecsHub->AddComponent(entity, physicsComponent);
                    }

                    // Load UnitComponent
                    if (gameLevel1.contains("UnitComponent")) {
                        UnitType unitType = gameLevel1["UnitComponent"]["unitType"] == "Player" ? UnitType::Player : UnitType::Enemy;
                        int health = gameLevel1["UnitComponent"]["health"];
                        int maxHealth = gameLevel1["UnitComponent"]["maxHealth"];
                        int attackPower = gameLevel1["UnitComponent"]["attackPower"];
                        int energy = gameLevel1["UnitComponent"]["energy"];
                        int maxEnergy = gameLevel1["UnitComponent"]["maxEnergy"];
                        bool isAlive = gameLevel1["UnitComponent"]["isAlive"];
                        bool isMoving  = gameLevel1["UnitComponent"]["isMoving"];
                        int speed = gameLevel1["UnitComponent"]["speed"];
                        Vector2D position = gameLevel1.contains("position")
                            ? Vector2D(gameLevel1["UnitComponent"]["position"][0], gameLevel1["UnitComponent"]["position"][1])
                            : Vector2D(0, 0);
                        ecsHub->AddComponent(entity, UnitComponent{ unitType, health, maxHealth,
                            attackPower, energy, maxEnergy, isAlive, isMoving, speed, position });
                    }

                    // Load TileComponent
                    if (gameLevel1.contains("TileComponent")) {
                        bool walkable = gameLevel1["TileComponent"]["walkable"];
                        bool highlighted = gameLevel1["TileComponent"]["highlighted"];
                        bool highlightedEnemy = gameLevel1["TileComponent"]["highlightedEnemy"];
                        Vector2D position = Vector2D(gameLevel1["TileComponent"]["position"][0], gameLevel1["TileComponent"]["position"][1]);
                        TileType tileType = gameLevel1["TileComponent"]["tileType"] == "Floor" ? TileType::Floor : TileType::Wall;
                        float movementCost = gameLevel1["TileComponent"]["movementCost"];
                        ecsHub->AddComponent(entity, TileComponent{ walkable, highlighted, highlightedEnemy, position, tileType, movementCost });
                    }

                    if (gameLevel1.contains("CamComponent")) {
                        GLint height{ 0 };
                        GLint minHeight{ 0 }, maxHeight{ 0 };
                        GLint heightChangeDir{ 0 };
                        GLint heightChangeVal{ 0 };
                        GLfloat linearSpeed{ 0.f };

                        if (gameLevel1["CamComponent"].contains("height"))
                            height = gameLevel1["CamComponent"]["height"].get<GLint>();

                        if (gameLevel1["CamComponent"].contains("minHeight"))
                            minHeight = gameLevel1["CamComponent"]["minHeight"].get<GLint>();

                        if (gameLevel1["CamComponent"].contains("maxHeight"))
                            maxHeight = gameLevel1["CamComponent"]["maxHeight"].get<GLint>();

                        if (gameLevel1["CamComponent"].contains("heightChangeDir"))
                            heightChangeDir = gameLevel1["CamComponent"]["heightChangeDir"].get<GLint>();

                        if (gameLevel1["CamComponent"].contains("heightChangeVal"))
                            heightChangeVal = gameLevel1["CamComponent"]["heightChangeVal"].get<GLint>();

                        if (gameLevel1["CamComponent"].contains("linearSpeed"))
                            linearSpeed = gameLevel1["CamComponent"]["linearSpeed"].get<GLfloat>();
                        ecsHub->AddComponent(entity, CameraComponent{ height, minHeight, maxHeight, heightChangeDir, heightChangeVal, linearSpeed });
                    }

                    if (gameLevel1.contains("FontData")) {
                        if (gameLevel1["FontData"].contains("text")) {
                            ecsHub->AddComponent(entity, FontDataComponent{ gameLevel1["FontData"]["font"].get<std::string>(),
                                gameLevel1["FontData"]["text"].get<std::string>(), gameLevel1["FontData"]["scale"].get<float>() });

                        }

                    }

                    // Unique identifier for all objects
                    std::string type = gameLevel1["type"].get<std::string>();

                    gameObjectTypeMap[entity] = type;
                    entities.push_back(entity);  // Add the created entity to the vector

                }
                catch (const std::exception& e) {
                    std::cerr << "Error loading game object: " << e.what() << std::endl;
                }
            }

            return entities;
        }
/**************************************************************************
* @brief
*    Retrieves the type of a game object based on the provided entity.
*
* @param entity
*    The entity representing the game object whose type is being retrieved.
*
* @return
*    Returns a string representing the type of the game object (e.g., "Player", "Camera"),
*    or an empty string if the entity type is not found.
*************************************************************************/
    std::string GameObjectFactory::GetEntityType(Entity entity) {
        auto it = gameObjectTypeMap.find(entity);
        if (it != gameObjectTypeMap.end()) {
            return it->second;
        }
        else {
            std::cerr << "Entity " << entity << " type not found in gameObjectTypeMap!" << std::endl;
            return "";
        }
    }
    void GameObjectFactory::SetEntityType(Entity entity, std::string type)
    {
        auto it = gameObjectTypeMap.find(entity);
        if (it != gameObjectTypeMap.end()) {
            it->second = type;
        }
        else {
            std::cerr << "Entity " << entity << " type not found in gameObjectTypeMap!" << std::endl;
        }
    }
    std::vector<std::pair<Entity, std::string>> GameObjectFactory::GetAllGameObjects() {
        std::vector<std::pair<Entity, std::string>> objects;
        for (const auto& [entity, type] : gameObjectTypeMap) {
            objects.emplace_back(entity, type);
        }
        return objects;
    }

    std::map<Entity, std::string>& GameObjectFactory::GetAllGameObjectsMap() {
        return gameObjectTypeMap;
    }

    std::vector<Entity> GameObjectFactory::GetEntityByName(const std::string& name) {
        std::vector<Entity> container;
        for (const auto& [entity, type] : gameObjectTypeMap) {
            size_t index = type.find_last_of("_");
            if (type.substr(index + 1) == name) {
                container.emplace_back(entity);
            }
        }
#ifdef _DEBUG
#endif
        return container;
    }
    
}