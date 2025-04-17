/******************************************************************************/
/*!
\file    Serialization.cpp
\project FruitLoops
\author  Zhang TongYan & Gareth Ching
\date    Dec 03, 2024
\brief   This file implements the functionality for serialising and
         deserialising game objects using the nlohmann::json library.

         It includes methods to:
         - Save individual game objects to a specified file in JSON format.
         - Load game objects from JSON files into in-memory representations.
         - Serialise all components (e.g., Transform, RigidBody, CameraComponent)
           of game entities into JSON structures.
         - Group and save entities by layer (e.g., MainMenu, Level1, UI) into
           distinct JSON files in the GameData directory.

         These operations allow the game engine to persist and restore game
         states between sessions, enabling features like scene editing,
         prefab storage, and save/load mechanisms.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/

#include "pch.h"
#include "Serialization.h"
#include "Factory.h"
#include "ECSHub.h"

namespace FruitLoops {
    /**************************************************************************
    * @brief Saves a game object represented as a JSON object to a file.
    *
    * This function serializes a given JSON object representing a game object
    * and saves it to a file with the specified filename
    *
    * @param j The JSON object containing the game object data to be saved.
    * @param filename The name of the file to which the JSON data will be written.
    * @return True if the file was successfully written, false otherwise.
    *************************************************************************/
    bool SaveGameObjectToFile(const nlohmann::json& j, const std::string& filename) {
        std::ofstream file(filename);  // Open the file for writing
        if (file.is_open()) {
            file << j.dump(4);  // Pretty print the JSON with 4 spaces of indentation
            file.close();
            return true;
        }
        else {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return false;
        }
    }

    /**************************************************************************
    * @brief Loads a game object from a file and parses it as a JSON object.
    *
    * This function attempts to read a JSON-formatted file and parse its contents
    * into a JSON object. If the file doesn't exist or contains invalid JSON,
    * an empty JSON object is returned.
    *
    * @param filename The name of the file from which the game object data
    *        will be loaded.
    * @return A JSON object representing the game object loaded from the file.
    *         Returns an empty JSON object if the file does not exist or
    *         an error occurs during reading/parsing.
    *************************************************************************/
    nlohmann::json LoadGameObjectFromFile(const std::string& filename) {

        if (!std::filesystem::exists(filename)) {
            std::cerr << "File does not exist: " << filename << std::endl;
            return {};
        }

        std::ifstream file(filename);  // Open the file for reading
        if (!file.is_open()) {
            std::cerr << "Failed to open file: " << filename << std::endl;
            return {};  // Return an empty JSON object on failure
        }

        nlohmann::json j;
        try {
            file >> j;  // Read the JSON data from the file
        }
        catch (const std::exception& e) {
            std::cerr << "Error parsing JSON from file: " << e.what() << std::endl;
            return {};  // Return empty JSON in case of parse errors
        }

        return j;
    }

    /**************************************************************************
    * @brief Serializes all game objects into a JSON array.
    *
    * This function iterates over all game objects, serializes their components
    * (such as Transform, RigidBody, Material), and stores the serialized data
    * into a JSON array.
    *
    * @return A JSON object representing the state of all game objects.
    *************************************************************************/
    void SerializeGameObject(nlohmann::json& allGameObjects, std::vector<Entity> objects) {
        // Iterate over all game objects and serialize them
        for (Entity entity : objects) {
            nlohmann::json gameObjectJson;

            // Store the type of the object (e.g., "Player", "Treasurebox")
            gameObjectJson["type"] = factory->GetEntityType(entity);

            // Serialize the Transform component if it exists
            if (ecsHub->HasComponent<Transform>(entity)) {
                const Transform& transform = ecsHub->GetComponent<Transform>(entity);
                gameObjectJson["Transform"] = {
                    {"scale", {transform.scale.x, transform.scale.y}},
                    {"orientation", {transform.orientation.x, transform.orientation.y}},
                    {"position", {transform.position.x, transform.position.y}}
                };
            }


            // Serialize the Material component if it exists
            if (ecsHub->HasComponent<Material>(entity)) {
                const Material& material = ecsHub->GetComponent<Material>(entity);
                gameObjectJson["Material"] = {
                    {"color", {material.color.x, material.color.y, material.color.z}},
                    {"textureName", material.textureName},
                    {"uvMax", {material.uvMax.x, material.uvMax.y}},
                    {"uvMin", {material.uvMin.x, material.uvMin.y}}
                };
            }


            // Serialize the CameraComponent if it exists
            if (ecsHub->HasComponent<CameraComponent>(entity)) {
                const CameraComponent& cam = ecsHub->GetComponent<CameraComponent>(entity);
                gameObjectJson["CamComponent"] = {
                    {"height", cam.height},
                    {"minHeight", cam.minHeight},
                    {"maxHeight", cam.maxHeight},
                    {"heightChangeDir", cam.heightChangeDir},
                    {"heightChangeVal", cam.heightChangeVal},
                    {"linearSpeed", cam.linearSpeed}
                };
            }

            // Serialize the RigidBody Component if it exists
            if (ecsHub->HasComponent<RigidBody>(entity)) {
                const RigidBody& rigid = ecsHub->GetComponent<RigidBody>(entity);
                const Transform& transform = ecsHub->GetComponent<Transform>(entity);
                gameObjectJson["RigidBody"] = {
                    {"velocity", {rigid.velocity.x, rigid.velocity.y}},
                    {"position", {transform.position.x, transform.position.y}}
                };
            }

            // Serialize the Font Component if it exists
            if (ecsHub->HasComponent<FontDataComponent>(entity)) {
                const FontDataComponent& data = ecsHub->GetComponent<FontDataComponent>(entity);
                gameObjectJson["FontData"] = {
                    {"text", data.text},
                    {"font", data.font},
                    {"scale", data.scale}
                };
            }

            // Serialize the ButtonComponent if it exists
            if (ecsHub->HasComponent<ButtonComponent>(entity)) {
                const ButtonComponent& button = ecsHub->GetComponent<ButtonComponent>(entity);
                gameObjectJson["Button"] = {
                    {"isHovered", button.isHovered},
                    {"isClicked", button.isClicked},
                    {"isEnabled", button.isEnabled}
                };
            }

            // Serialize the TileComponent if it exists
            if (ecsHub->HasComponent<TileComponent>(entity)) {
                const TileComponent& tile = ecsHub->GetComponent<TileComponent>(entity);
                gameObjectJson["TileComponent"] = {
                    {"walkable", tile.walkable},
                    {"highlighted", tile.highlighted},
                    {"position", {tile.position.x, tile.position.y}},
                    {"tileType", static_cast<int>(tile.tileType)},
                    {"movementCost", tile.movementCost},
                    {"highlightedEnemy", tile.highlightedEnemy}
                };
            }

            // Serialize the UnitComponent if it exists
            if (ecsHub->HasComponent<UnitComponent>(entity)) {
                const UnitComponent& unit = ecsHub->GetComponent<UnitComponent>(entity);
                gameObjectJson["UnitComponent"] = {
                    {"unitType", static_cast<int>(unit.unitType)},
                    {"health", unit.health},
                    {"maxHealth", unit.maxHealth},
                    {"attackPower", unit.attackPower},
                    {"energy", unit.energy},
                    {"maxEnergy", unit.maxEnergy},
                    {"position", {unit.position.x, unit.position.y}},
                    {"speed", unit.speed},
                    {"isMoving", unit.isMoving},
                    {"isAlive", unit.isAlive}
                };
            }

            // Serialize the ScriptComponent if it exists
            if (ecsHub->HasComponent<ScriptComponent>(entity)) {
                const ScriptComponent& script = ecsHub->GetComponent<ScriptComponent>(entity);
                gameObjectJson["ScriptComponent"] = {
                    {"name", script.Name}
                };
            }

            // Serialize the ScriptComponent if it exists
            if (ecsHub->HasComponent<AnimationComponent>(entity)) {
                const AnimationComponent& anim = ecsHub->GetComponent<AnimationComponent>(entity);
                gameObjectJson["AnimationComponent"] = {
                    {"AnimationName", anim.AnimationName}
                };
            }

            // Add this object's JSON to the "gameObjects" array
            allGameObjects["gameObjects"].push_back(gameObjectJson);
        }
    }
    void SerializeAndSaveAllGameObjects() {
        
        nlohmann::json utilsObjects;
        nlohmann::json mainMenuObjects;
        nlohmann::json storyObjects;
        nlohmann::json pauseMenuObjects;
        nlohmann::json level1Objects;
        nlohmann::json level2Objects;
        nlohmann::json tutorialObjects;
        nlohmann::json fpsObjects;
        nlohmann::json uiObjects;
        nlohmann::json endMenuObjects;
        nlohmann::json chestObjects;

        for (Layer* layer : coreEngine->GetLayerStack()) {
            std::vector<Entity> layerObj = *layer->GetObjectContainer();
            if (layer->GetName().substr(0, 4) == "Game") {
                if (layer->GetObjectContainer()->size() > 0) {
                    if (factory->GetEntityType((*layer->GetObjectContainer())[0]).substr(0, 6) == "Level1") {
                        SerializeGameObject(level1Objects, layerObj);
                    }
                    else if (factory->GetEntityType((*layer->GetObjectContainer())[0]).substr(0, 6) == "Level2") {
                        SerializeGameObject(level2Objects, layerObj);
                    }
                    else if (factory->GetEntityType((*layer->GetObjectContainer())[0]).substr(0, 8) == "Tutorial") {
                        SerializeGameObject(tutorialObjects, layerObj);
                    }
                   
                }
                
            }
            else if (layer->GetName().substr(0,11) == "Environment") {
                if (layer->GetObjectContainer()->size() > 0) {
                    if (factory->GetEntityType((*layer->GetObjectContainer())[0]).substr(0, 6) == "Level1") {
                        SerializeGameObject(level1Objects, layerObj);
                    }
                    else if (factory->GetEntityType((*layer->GetObjectContainer())[0]).substr(0, 6) == "Level2") {
                        SerializeGameObject(level2Objects, layerObj);
                    }
                    else if (factory->GetEntityType((*layer->GetObjectContainer())[0]).substr(0, 8) == "Tutorial") {
                        SerializeGameObject(tutorialObjects, layerObj);
                    }
                  
                }

            }
            else if (layer->GetName().substr(0, 10) == "Background") {
                if (layer->GetObjectContainer()->size() > 0) {
                    if (factory->GetEntityType((*layer->GetObjectContainer())[0]).substr(0, 6) == "Level1") {
                        SerializeGameObject(level1Objects, layerObj);
                    }
                    else if (factory->GetEntityType((*layer->GetObjectContainer())[0]).substr(0, 6) == "Level2") {
                        SerializeGameObject(level2Objects, layerObj);
                    }
                    else if (factory->GetEntityType((*layer->GetObjectContainer())[0]).substr(0, 8) == "Tutorial") {
                        SerializeGameObject(tutorialObjects, layerObj);
                    }
                    
                }

            }
            else if (layer->GetName().substr(0, 4) == "Font") {
                if (layer->GetObjectContainer()->size() > 0) {
                    if (factory->GetEntityType((*layer->GetObjectContainer())[0]).substr(0, 6) == "Level1") {
                        SerializeGameObject(level1Objects, layerObj);
                    }
                    else if (factory->GetEntityType((*layer->GetObjectContainer())[0]).substr(0, 6) == "Level2") {
                        SerializeGameObject(level2Objects, layerObj);
                    }
                    else if (factory->GetEntityType((*layer->GetObjectContainer())[0]).substr(0, 8) == "Tutorial") {
                        SerializeGameObject(tutorialObjects, layerObj);
                    }
                }

            }
            else if (layer->GetName().substr(0, 7) == "EndMenu") {
                SerializeGameObject(endMenuObjects, layerObj);
            }
            else if (layer->GetName().substr(0, 8) == "MainMenu") {
                SerializeGameObject(mainMenuObjects, layerObj);
            }
            else if (layer->GetName().substr(0, 9) == "PauseMenu") {
                SerializeGameObject(pauseMenuObjects, layerObj);
            }
            else if (layer->GetName().substr(0, 5) == "Story") {
                SerializeGameObject(storyObjects, layerObj);
            }
            else if (layer->GetName().substr(0, 5) == "Utils") {
                SerializeGameObject(utilsObjects, layerObj);
            }
            else if (layer->GetName().substr(0, 3) == "FPS") {
                SerializeGameObject(fpsObjects, layerObj);
            }
            else if (layer->GetName().substr(0, 2) == "UI") {
                SerializeGameObject(uiObjects, layerObj);
            }
            else if (layer->GetName().substr(0, 12) == "ChestOverlay") {
                SerializeGameObject(chestObjects, layerObj);
            }
        }

        std::ofstream utilsFile("../GameData/Utils.json");  // Open the file for writing
        if (utilsFile.is_open()) {
            utilsFile << utilsObjects.dump(4);  // Pretty print the JSON with 4 spaces of indentation
            utilsFile.close();
            std::cerr << "Successfully saved to file: " << "../GameData/Utils.json" << std::endl;
        }
        else {
            std::cerr << "Failed to open file: " << "../GameData/Utils.json" << std::endl;
            return;
        }

        std::ofstream mainMenuFile("../GameData/MainMenu.json");  // Open the file for writing
        if (mainMenuFile.is_open()) {
            mainMenuFile << mainMenuObjects.dump(4);  // Pretty print the JSON with 4 spaces of indentation
            mainMenuFile.close();
            std::cerr << "Successfully saved to file: " << "../GameData/MainMenu.json" << std::endl;
        }
        else {
            std::cerr << "Failed to open file: " << "../GameData/MainMenu.json" << std::endl;
            return;
        }

        std::ofstream storyFile("../GameData/Story.json");  // Open the file for writing
        if (storyFile.is_open()) {
            storyFile << storyObjects.dump(4);  // Pretty print the JSON with 4 spaces of indentation
            storyFile.close();
            std::cerr << "Successfully saved to file: " << "../GameData/Story.json" << std::endl;
        }
        else {
            std::cerr << "Failed to open file: " << "../GameData/Story.json" << std::endl;
            return;
        }

        std::ofstream pauseMenuFile("../GameData/PauseMenu.json");  // Open the file for writing
        if (pauseMenuFile.is_open()) {
            pauseMenuFile << pauseMenuObjects.dump(4);  // Pretty print the JSON with 4 spaces of indentation
            pauseMenuFile.close();
            std::cerr << "Successfully saved to file: " << "../GameData/PauseMenu.json" << std::endl;
        }
        else {
            std::cerr << "Failed to open file: " << "../GameData/PauseMenu.json" << std::endl;
            return;
        }

        if (!level1Objects.empty()) {
            std::ofstream level1File("../GameData/Level1.json");  // Open the file for writing
            if (level1File.is_open()) {
                level1File << level1Objects.dump(4);  // Pretty print the JSON with 4 spaces of indentation
                level1File.close();
                std::cerr << "Successfully saved to file: " << "../GameData/Level1.json" << std::endl;
            }
            else {
                std::cerr << "Failed to open file: " << "../GameData/Level1.json" << std::endl;
                return;
            }
        }
        
        if (!level2Objects.empty()) {
            std::ofstream level2File("../GameData/Level2.json");  // Open the file for writing
            if (level2File.is_open()) {
                level2File << level2Objects.dump(4);  // Pretty print the JSON with 4 spaces of indentation
                level2File.close();
                std::cerr << "Successfully saved to file: " << "../GameData/Level2.json" << std::endl;
            }
            else {
                std::cerr << "Failed to open file: " << "../GameData/Level2.json" << std::endl;
                return;
            }
        }
        
        if (!tutorialObjects.empty()) {
            std::ofstream tutorialFile("../GameData/Tutorial.json");  // Open the file for writing
            if (tutorialFile.is_open()) {
                tutorialFile << tutorialObjects.dump(4);  // Pretty print the JSON with 4 spaces of indentation
                tutorialFile.close();
                std::cerr << "Successfully saved to file: " << "../GameData/Tutorial.json" << std::endl;
            }
            else {
                std::cerr << "Failed to open file: " << "../GameData/Tutorial.json" << std::endl;
                return;
            }
        }

        std::ofstream fpsFile("../GameData/FPS.json");  // Open the file for writing
        if (fpsFile.is_open()) {
            fpsFile << fpsObjects.dump(4);  // Pretty print the JSON with 4 spaces of indentation
            fpsFile.close();
            std::cerr << "Successfully saved to file: " << "../GameData/FPS.json" << std::endl;
        }
        else {
            std::cerr << "Failed to open file: " << "../GameData/FPS.json" << std::endl;
            return;
        }

        std::ofstream chestFile("../GameData/ChestOverlay.json");  // Open the file for writing
        if (chestFile.is_open()) {
            chestFile << chestObjects.dump(4);  // Pretty print the JSON with 4 spaces of indentation
            chestFile.close();
            std::cerr << "Successfully saved to file: " << "../GameData/ChestOverlay.json" << std::endl;
        }
        else {
            std::cerr << "Failed to open file: " << "../GameData/ChestOverlay.json" << std::endl;
            return;
        }

        std::ofstream uiFile("../GameData/UI.json");  // Open the file for writing
        if (uiFile.is_open()) {
            uiFile << uiObjects.dump(4);  // Pretty print the JSON with 4 spaces of indentation
            uiFile.close();
            std::cerr << "Successfully saved to file: " << "../GameData/UI.json" << std::endl;
        }
        else {
            std::cerr << "Failed to open file: " << "../GameData/UI.json" << std::endl;
            return;
        }

        std::ofstream endMenuFile("../GameData/EndMenu.json");  // Open the file for writing
        if (endMenuFile.is_open()) {
            endMenuFile << endMenuObjects.dump(4);  // Pretty print the JSON with 4 spaces of indentation
            endMenuFile.close();
            std::cerr << "Successfully saved to file: " << "../GameData/EndMenu.json" << std::endl;
        }
        else {
            std::cerr << "Failed to open file: " << "../GameData/EndMenu.json" << std::endl;
            return;
        }
    }


}
