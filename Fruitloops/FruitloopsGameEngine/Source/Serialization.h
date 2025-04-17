#pragma once
/******************************************************************************/
/*!
\file    Serialization.h
\project FruitLoops
\author  Zhang TongYan & Gareth Ching
\date    Dec 03, 2024
\brief   This file declares functions for serialising and deserialising
         game objects using the nlohmann::json library.

         The functionality includes:
         - Saving a JSON object representing a game entity to a file.
         - Loading a JSON object from a specified file.
         - Serialising all ECS entities and their components into JSON format.
         - Exporting grouped objects by layer (e.g., Level1, UI) to separate files.

         These declarations support runtime and editor operations such as
         saving scenes, loading prefabs, and exporting game state data.

All content © 2024 DigiPen Institute of Technology Singapore. All
rights reserved.
*/
/******************************************************************************/
namespace FruitLoops {
    /**************************************************************************
    * @brief Saves a game object represented as a JSON object to a file.
    * 
    * This function serializes the provided JSON object representing a game object 
    * and writes it to the specified file in a human-readable format with indentation.
    *
    * @param j The JSON object containing the game object data to be saved.
    * @param filename The name of the file to which the JSON data will be saved.
    * @return Returns true if the file was successfully written, otherwise false.
    *************************************************************************/
    bool SaveGameObjectToFile(const nlohmann::json& j, const std::string& filename);

    /**************************************************************************
    * @brief Loads a game object from a file and parses it as a JSON object.
    *
    * This function reads a JSON-formatted file containing game object data and
    * parses its contents into a JSON object. If the file doesn't exist or
    * contains invalid JSON, an empty JSON object is returned.
    *
    * @param filename The name of the file to load the game object data from.
    * @return A JSON object representing the game object. Returns an empty JSON
    *         object if the file doesn't exist or an error occurs during reading/parsing.
    *************************************************************************/
    nlohmann::json LoadGameObjectFromFile(const std::string& filename);

    /**************************************************************************
    * @brief Serializes all game objects into a JSON array.
    *
    * This function iterates over all game objects, serializes their components
    * (such as Transform, RigidBody, Material), and stores the serialized data
    * into a JSON array.
    *
    * @return A JSON object representing the state of all game objects.
    *************************************************************************/
    void SerializeAndSaveAllGameObjects();
}